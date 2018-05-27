/*
  ***** BEGIN LICENSE BLOCK *****
 
  Copyright (C) 2001-2018 Olof Hagsand

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
#include <assert.h>
#include <netinet/in.h>

#include "cligen_buf.h"
#include "cligen_var.h"
#include "cligen_cvec.h"
#include "cligen_gen.h"
#include "cligen_parse.h"
#include "cligen_handle.h"
#include "cligen_read.h"
#include "cligen_syntax.h"

/*! Parse a string containing a CLIgen spec into a parse-tree
 * 
 * Syntax parsing. A string is input and a syntax-tree is returned (or error). 
 * A variable record is also returned containing a list of (global) variable values.

 * The string contains a hierarchy of syntax specs bounded by {} and semi-colon. Comma is used
 * to tag a syntax-spec with assignments or callbacks. Help strings are delimited with ("").
 * '#' anywhere on the line means the rest is comment.
 * Example:
 * @code
 * global=foo;
 * a("command") b, fn1();{
 *    c("help"), fn2("arg"), local=3;
 *    d("help");
 * }
 * (e | d)
 * @endcode
 * Gives the following allowed strings: "a b", "a b c", "a b d", "e", "d".
 * ;
 */
int
cligen_parse_str(cligen_handle h,
		 char         *str,
		 char         *name, /* just for errs */
		 parse_tree   *pt,
		 cvec         *vr
    )
{
    int                retval = -1;
    int                i;
    cliyacc            ya = {0,};
    cg_obj            *co;
    cg_obj             co0; /* tmp top object: NOT malloced */
    cg_obj            *co_top = &co0;

    memset(&co0, 0, sizeof(co0));
    ya.ya_handle       = h; /* cligen_handle */
    ya.ya_name         = name;
    ya.ya_treename     = strdup(name); /* Use name as default tree name */
    ya.ya_linenum      = 1;
    ya.ya_parse_string = str;
    ya.ya_stack        = NULL;
    if (pt)
	co_top->co_pt      = *pt;
    if (vr)
	ya.ya_globals       = vr; 
    else
	if ((ya.ya_globals = cvec_new(0)) == NULL){
	    fprintf(stderr, "%s: malloc: %s\n", __FUNCTION__, strerror(errno)); 
	    goto done;
	}
    if (strlen(str)){ /* Not empty */
	if (cgl_init(&ya) < 0)
	    goto done;
	if (cgy_init(&ya, co_top) < 0)
	    goto done;
	if (cligen_parseparse(&ya) != 0) { /* yacc returns 1 on error */
	    cgy_exit(&ya);
	    cgl_exit(&ya);
	    goto done;
	}
	/* Add final tree (but only with new API) */
	if (pt == NULL){
	    for (i=0; i<co_top->co_max; i++){
		if ((co=co_top->co_next[i]) != NULL)
		    co_up_set(co, NULL);
	    }
	    if (cligen_tree_add(ya.ya_handle, ya.ya_treename, co_top->co_pt) < 0)
		goto done;
	    memset(&co_top->co_pt, 0, sizeof(parse_tree));
	}
	if (cgy_exit(&ya) < 0)
	    goto done;		
	if (cgl_exit(&ya) < 0)
	    goto done;		
    }
    if (vr)
	vr= ya.ya_globals;
    else
	cvec_free(ya.ya_globals);
    /*
     * Remove the fake top level object and remove references to it.
     */
    if (pt)
	*pt = co_top->co_pt;
    for (i=0; i<co_top->co_max; i++){
	co=co_top->co_next[i];
	if (co)
	    co_up_set(co, NULL);
    }
    retval = 0;
  done:
    if (ya.ya_treename)
	free (ya.ya_treename);
    return retval;

}

/*! Parse a file containing a CLIgen spec into a parse-tree
 *
 * Similar to cligen_parse_str(), just read a file first
 */
int
cligen_parse_file(cligen_handle h,
		  FILE         *f,
		  char         *name, /* just for errs */
		  parse_tree   *pt,   /* obsolete */
		  cvec         *globals)
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
	if ((c =  fgetc(f)) == EOF)
	    break;
	if (len==i){
	    if ((buf = realloc(buf, 2*len)) == NULL){
		fprintf(stderr, "%s: realloc: %s\n", __FUNCTION__, strerror(errno));
		goto done;
	    }	    
	    memset(buf+len, 0, len);
	    len *= 2;
	}
	buf[i++] = (char)(c&0xff);
    } /* read a line */
    if (cligen_parse_str(h, buf, name, pt, globals) < 0)
	goto done;
    retval = 0;
  done:
    if (buf)
	free(buf);
    return retval;
}

/*! Assign functions for callbacks (end of line) using a mapper function
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
 * @see cligen_expand_str2fn    For expansion/completion callbacks
 * @see cligen_callbackv_str2fnv Same but for callback vector argument
 * @note str2fn may return NULL on error and should then supply a (static) error string 
 * @note Try to use cligen_callbackv_str2fnv instead
 */
int
cligen_callback_str2fn(parse_tree pt, cg_str2fn_t *str2fn, void *arg)
{
    int                 retval = -1;
    cg_obj             *co;
    char               *callback_err = NULL;   /* Error from str2fn callback */
    struct cg_callback *cc;
    int                 i;

    for (i=0; i<pt.pt_len; i++)
	if ((co = pt.pt_vec[i]) != NULL){
	    for (cc = co->co_callbacks; cc; cc=cc->cc_next){
		if (cc->cc_fn_str != NULL && cc->cc_fn == NULL){
		    /* Note str2fn is a function pointer */
		    cc->cc_fn = str2fn(cc->cc_fn_str, arg, &callback_err);
		    if (callback_err != NULL){
			fprintf(stderr, "%s: error: No such function: %s (%s)\n",
				__FUNCTION__, cc->cc_fn_str, callback_err);
			goto done;
		    }
		}
	    }
	    /* recursive call to next level */
	    if (cligen_callback_str2fn(co->co_pt, str2fn, arg) < 0)
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
 * @see cligen_callback_str2fn Same but for callback single argument
 * @note str2fn may return NULL on error and should then supply a (static) error string 
 */
int
cligen_callbackv_str2fn(parse_tree    pt, 
			cgv_str2fn_t *str2fn, 
			void         *arg)
{
    int                 retval = -1;
    cg_obj             *co;
    char               *callback_err = NULL;   /* Error from str2fn callback */
    struct cg_callback *cc;
    int                 i;

    for (i=0; i<pt.pt_len; i++)
	if ((co = pt.pt_vec[i]) != NULL){
	    for (cc = co->co_callbacks; cc; cc=cc->cc_next){
		if (cc->cc_fn_str != NULL && cc->cc_fn_vec == NULL){
		    /* Note str2fn is a function pointer */
		    cc->cc_fn_vec = str2fn(cc->cc_fn_str, arg, &callback_err);
		    if (callback_err != NULL){
			fprintf(stderr, "%s: error: No such function: %s (%s)\n",
				__FUNCTION__, cc->cc_fn_str, callback_err);
			goto done;
		    }
		}
	    }
	    /* recursive call to next level */
	    if (cligen_callbackv_str2fn(co->co_pt, str2fn, arg) < 0)
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
cligen_expandv_str2fn(parse_tree        pt, 
		      expandv_str2fn_t *str2fn, 
		      void             *arg)
{
    int                 retval = -1;
    cg_obj             *co;
    char               *callback_err = NULL;   /* Error from str2fn callback */
    int                 i;

    for (i=0; i<pt.pt_len; i++){    
	if ((co = pt.pt_vec[i]) != NULL){
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
	    if (cligen_expandv_str2fn(co->co_pt, str2fn, arg) < 0)
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
cligen_translate_str2fn(parse_tree          pt, 
			translate_str2fn_t *str2fn, 
			void               *arg)
{
    int                 retval = -1;
    cg_obj             *co;
    char               *callback_err = NULL;   /* Error from str2fn callback */
    int                 i;

    for (i=0; i<pt.pt_len; i++){    
	if ((co = pt.pt_vec[i]) != NULL){
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
	    if (cligen_translate_str2fn(co->co_pt, str2fn, arg) < 0)
		goto done;
	}
    }
    retval = 0;
  done:
    return retval;
}
