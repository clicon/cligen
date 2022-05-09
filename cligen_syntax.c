/*
  ***** BEGIN LICENSE BLOCK *****
 
  Copyright (C) 2001-2022 Olof Hagsand

  This file is part of CLIgen.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  Alternatively, the contents of this file may be used under the terms of
  the GNU General Public License Version 2 or later (the "GPL"),
  in which case the provisions of the GPL are applicable instead
  of those above. If you wish to allow use of your version of this file only
  under the terms of the GPL, and not to allow others to
  use your version of this file under the terms of Apache License version 2, indicate
  your decision by deleting the provisions above and replace them with the 
  notice and other provisions required by the GPL. If you do not delete
  the provisions above, a recipient may use your version of this file under
  the terms of any one of the Apache License version 2 or the GPL.

  ***** END LICENSE BLOCK *****
*/
#include "cligen_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>

#include "cligen_buf.h"
#include "cligen_cv.h"
#include "cligen_cvec.h"
#include "cligen_parsetree.h"
#include "cligen_pt_head.h"
#include "cligen_callback.h"
#include "cligen_object.h"
#include "cligen_parse.h"
#include "cligen_handle.h"
#include "cligen_result.h"
#include "cligen_read.h"
#include "cligen_syntax.h"

/*! Parse a string containing a CLIgen spec into a parse-tree
 * 
 * Syntax parsing. A string is input and a syntax-tree is returned (or error). 
 * A variable record is also returned containing a list of (global) variable values.
 * The string contains a hierarchy of syntax specs bounded by {} and semi-colon. Comma is used
 * to tag a syntax-spec with assignments or callbacks. Help strings are delimited with ("").
 * '#' anywhere on the line means the rest is comment.
 * @param[in]     h    CLIgen handle
 * @param[in]     str  String to parse containing CLIgen specification statements
 * @param[in]     name Debug string identifying the spec, typically a filename
 * @param[in,out] pt   Parse-tree, if set, add commands to this. Can be NULL
 * @param[out]    cvv  Global variables
 * @see cligen_parse_file
 * @note parse-trees can be added as side-effect:s using the treename clispec:s. The tree returned
 * in pt is only the "latest" one.
 */
int
cligen_parse_str(cligen_handle h,
		 char         *str,
		 char         *name,
		 parse_tree   *ptp,
		 cvec         *cvv)
{
    int                retval = -1;
    int                i;
    cligen_yacc        cy = {0,};
    cg_obj            *co;
    cg_obj            *cot = NULL;
    parse_tree        *pt = NULL; 
    pt_head           *ph;
    
    /* "Fake" top-level object that is removed on exit */
    if ((cot = co_new(NULL, NULL)) == NULL)
	goto done;
    cy.cy_handle       = h; /* cligen_handle */
    cy.cy_name         = name;
    cy.cy_treename     = strdup(name); /* Use name as default tree name */
    cy.cy_linenum      = 1;
    cy.cy_parse_string = str;
    cy.cy_stack        = NULL;
    if (ptp != NULL)
	pt = ptp;
    else
	if ((pt = pt_new()) == NULL)
	    goto done;
    co_pt_set(cot, pt);
    if (cvv)
	cy.cy_globals  = cvv; 
    else
	if ((cy.cy_globals = cvec_new(0)) == NULL){
	    fprintf(stderr, "%s: malloc: %s\n", __FUNCTION__, strerror(errno)); 
	    goto done;
	}
    if (strlen(str)){ /* Not empty */
	if (cgl_init(&cy) < 0)
	    goto done;
	if (cgy_init(&cy, cot) < 0)
	    goto done;
	if (cligen_parseparse(&cy) != 0) { /* yacc returns 1 on error */
	    cgy_exit(&cy);
	    cgl_exit(&cy);
	    goto done;
	}
	/* Note pt/ptp is stale after parsing due to treename that replaces cot->pt 
	 * Add final tree */
	pt = co_pt_get(cot);
	if (ptp == NULL){
	    if ((ph = cligen_ph_add(cy.cy_handle, cy.cy_treename)) == NULL)
		goto done;
	    if (cligen_ph_parsetree_set(ph, pt) < 0)
		goto done;
	}
	if (cgy_exit(&cy) < 0)
	    goto done;		
	if (cgl_exit(&cy) < 0)
	    goto done;		
    }
    if (cvv == NULL) /* Not passed to caller function */
	cvec_free(cy.cy_globals);
    /*
     * Remove the fake top level object and remove references to it.
     * This does not work for (other) trees
     */
    for (i=0; i<pt_len_get(pt); i++){
	if ((co=pt_vec_i_get(pt, i)) != NULL)
	    co_up_set(co, NULL);
    }
    retval = 0;
  done:
    if (cot)
	co_free(cot, 0);
    if (cy.cy_treename)
	free (cy.cy_treename);
    return retval;
}

/*! Parse a file containing a CLIgen spec into a parse-tree
 *
 * @param[in]     h    CLIgen handle
 * @param[in]     f    Open stdio file handle
 * @param[in]     name Debug string identifying the spec, typically a filename
 * @param[in,out] pt   Parse-tree, if set, add commands to this
 * @param[out]    cvv  Global variables
 * @see cligen_parse_str
 */
int
cligen_parse_file(cligen_handle h,
		  FILE         *f,
		  char         *name,
		  parse_tree   *pt,  
		  cvec         *cvv)
{
    char         *buf;
    int           i;
    int           c;
    int           len;
    int           retval = -1;

    len = 1024; /* any number is fine */
    if ((buf = malloc(len)) == NULL){
	perror("pt_file malloc");
	return -1;
    }
    memset(buf, 0, len);

    i = 0; /* position in buf */
    while (1){ /* read the whole file */
	if ((c = fgetc(f)) == EOF)
	    break;
	if (i == len-1){
	    if ((buf = realloc(buf, 2*len)) == NULL){
		fprintf(stderr, "%s: realloc: %s\n", __FUNCTION__, strerror(errno));
		goto done;
	    }	    
	    memset(buf+len, 0, len);
	    len *= 2;
	}
	buf[i++] = (char)(c&0xff);
    } /* read a line */
    if (cligen_parse_str(h, buf, name, pt, cvv) < 0)
	goto done;
    retval = 0;
  done:
    if (buf)
	free(buf);
    return retval;
}

/*! Assign functions for variable completion using a mapper function
 *
 * The mapping is done from string to C-function. This is done recursively.
 * Example: Assume a CLIgen syntax:
 *   a <b:string>, fn();
 * where
 *    fn() is called when "a 42 <CR>" is entered.
 * In the CLIgen spec syntax, "fn" is a string and needs to be translated to actual 
 * function (pointer).
 * This function goes through a complete parse-tree (pt) and applies the translator
 * functions str2fn, if existring, to callback strings (eg "fn") 
 * in the parse-tree to produce function pointers (eg fn) which is stored in the
 * parse-tree nodes. Later, at evaluation time, the actual function (fn) is
 * called when evaluating/interpreting the syntax.
 *
 * @param[in]  pt      Parse-tree. Recursively loop through and call str2fn
 * @param[in]  str2fn  Translator function from strings to function pointers for command 
 *                     callbacks. Call this function to translate callback functions
 *                     for all nodes in the parse-tree.
 * @param[in]  arg     Argument to call str2fn with
 * @retval     0       OK
 * @retval    -1       Error and statement written on stderr
 *
 * @see cligen_expandv_str2fn    For expansion/completion callbacks
 * @note str2fn may return NULL on error and should then supply a (static) error string 
 */
int
cligen_callbackv_str2fn(parse_tree   *pt, 
			cgv_str2fn_t *str2fn, 
			void         *arg)
{
    int          retval = -1;
    cg_obj      *co;
    char        *callback_err = NULL;   /* Error from str2fn callback */
    cg_callback *cc;
    int          i;

    for (i=0; i<pt_len_get(pt); i++)
	if ((co = pt_vec_i_get(pt, i)) != NULL){
	    for (cc = co->co_callbacks; cc; cc = co_callback_next(cc)){
		if (cc->cc_fn_str != NULL &&
		    co_callback_fn_get(cc) == NULL){
		    /* Note str2fn is a function pointer */
		    co_callback_fn_set(cc, str2fn(cc->cc_fn_str, arg, &callback_err));
		    if (callback_err != NULL){
			fprintf(stderr, "%s: error: No such function: %s (%s)\n",
				__FUNCTION__, cc->cc_fn_str, callback_err);
			goto done;
		    }
		}
	    }
	    /* recursive call to next level */
	    if (cligen_callbackv_str2fn(co_pt_get(co), str2fn, arg) < 0)
		goto done;
	}
    retval = 0;
  done:
    return retval;
}

/*! Assign functions for variable completion using a mapper function
 *
 * The mapping is done from string to C-function. This is done recursively.
 * Example: Assume a CLIgen syntax:
 *   a <b:string fn("x","y")>;
 * where
 *    fn() is called when "a <TAB>" is entered
 * In the CLIgen spec syntax, "fn" is a string and needs to be translated to actual 
 * function (pointer).
 * This function goes through a complete parse-tree (pt) and applies the translator
 * functions str2fn, if existing, to callback strings (eg "fn") 
 * in the parse-tree to produce function pointers (eg fn) which is stored in the
 * parse-tree nodes. Later, at evaluation time, the actual function (fn) is
 * called when evaluating/interpreting the syntax.
 *
 * @param[in]  pt      parse-tree. Recursively loop thru this
 * @param[in]  str2fn  Translator from strings to function pointers for expand variable
 *                     callbacks. 
 * @param[in]  arg     Function argument for expand callbacks (at evaluation time).
 * @see cligen_callbackv_str2fn for translating callback functions
 */
int
cligen_expandv_str2fn(parse_tree       *pt, 
		      expandv_str2fn_t *str2fn, 
		      void             *arg)
{
    int     retval = -1;
    cg_obj *co;
    char   *callback_err = NULL;   /* Error from str2fn callback */
    int     i;

    for (i=0; i<pt_len_get(pt); i++){    
	if ((co = pt_vec_i_get(pt, i)) != NULL){
	    if (co->co_expand_fn_str != NULL && co->co_expandv_fn == NULL){
		/* Note str2fn is a function pointer */
		co->co_expandv_fn = str2fn(co->co_expand_fn_str, arg, &callback_err);
		if (callback_err != NULL){
		    fprintf(stderr, "%s: error: No such function: %s\n",
			    __FUNCTION__, co->co_expand_fn_str);
		    goto done;
		}
	    }
	    /* recursive call to next level */
	    if (cligen_expandv_str2fn(co_pt_get(co), str2fn, arg) < 0)
		goto done;
	}
    }
    retval = 0;
  done:
    return retval;
}

/*! Assign functions for translation of variables using a mapper function
 * The mapping is done from string to C-function. This is done recursively.
 * @param[in]  pt      Parse-tree. Recursively loop through and call str2fn
 * @param[in]  str2fn  Translator function from strings to function pointers
 * @param[in]  arg     Argument to call str2fn with
 */
int
cligen_translate_str2fn(parse_tree         *pt, 
			translate_str2fn_t *str2fn, 
			void               *arg)
{
    int     retval = -1;
    cg_obj *co;
    char   *callback_err = NULL;   /* Error from str2fn callback */
    int     i;

    for (i=0; i<pt_len_get(pt); i++){    
	if ((co = pt_vec_i_get(pt, i)) != NULL){
	    if (co->co_translate_fn_str != NULL && co->co_translate_fn == NULL){
		/* Note str2fn is a function pointer */
		co->co_translate_fn = str2fn(co->co_translate_fn_str, arg, &callback_err);
		if (callback_err != NULL){
		    fprintf(stderr, "%s: error: No such function: %s\n",
			    __FUNCTION__, co->co_translate_fn_str);
		    goto done;
		}
	    }
	    /* recursive call to next level */
	    if (cligen_translate_str2fn(co_pt_get(co), str2fn, arg) < 0)
		goto done;
	}
    }
    retval = 0;
  done:
    return retval;
}
