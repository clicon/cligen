/*
  CVS Version: $Id: cligen_syntax.c,v 1.38 2013/04/13 12:24:44 olof Exp $ 

  Copyright (C) 2001-2013 Olof Hagsand

  This file is part of CLIgen.

  CLIgen is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  CLIgen is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with CLIgen; see the file COPYING.
*/
#include "cligen_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <netinet/in.h>

#include "cligen_var.h"
#include "cligen_cvec.h"
#include "cligen_gen.h"
#include "cligen_parse.h"
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
		 char *str,
		 char *name, /* just for errs */
		 parse_tree *pt,
		 cvec *vr
    )
{
    int                retval = -1;
    int                i;
    struct cligen_parse_yacc_arg ya = {0,};
    cg_obj            *co;
    cg_obj             co0; /* tmp top object: NOT malloced */
    cg_obj            *co_top = &co0;

    memset(&co0, 0, sizeof(co0));
    ya.ya_handle       = h; /* cligen_handle */
    ya.ya_name         = name;
    ya.ya_linenum      = 1;
    ya.ya_parse_string = str;
    ya.ya_stack        = NULL;
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
    *pt = co_top->co_pt;
    for (i=0; i<co_top->co_max; i++){
	co=co_top->co_next[i];
	if (co)
	    co_up_set(co, NULL);
    }
    retval = 0;
  done:
    return retval;

}

/*! Parse a file containing a CLIgen spec into a parse-tree
 *
 * Similar to cligen_parse_str(), just read a file first
 */
int
cligen_parse_file(cligen_handle h,
		  FILE *f,
		  char *name, /* just for errs */
		  parse_tree *pt,
		  cvec *globals)
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

/*! Assign functions for callbacks in a parse-tree using a translate function
 *
 * Assume a CLIgen syntax:
 *   a <b:string f()>, g();
 * where f and g are functions:
 *    f is called when "a <TAB>" is entered
 *    g is called when "a 42 <CR>" is entered.
 * In the syntax, "f" and "g" are strings and need to be translated to actual function
 * (pointers).
 * This function goes through a complete parse-tree (pt) and applies the translator
 * functions str2fn1 and str2fn2, if existring, to callback strings (eg "f" and "g") 
 * in the parse-tree to produce function pointers (eg f, g) which are stored in the
 * parse-tree nodes. Later, at evaluation time, the actual functions (f, g) can be
 * called when evaluating/interpreting the syntax.
 *
 * @param  pt      parse-tree. Loop thru this
 * @param  str2fn1 Translator from strings to function pointers for command callbacks. 
 *         E.g. for g() above.
 * @param  fnarg1  Function argument for command callbacks (at evaluation time).
 * @param  str2fn2 Translator from strings to function pointers for expand variable
 *         callbacks. E.g. for f() above.
 * @param  fnarg2  Function argument for expand callbacks (at evaluation time).
 *
 * @retval   0   OK
 * @retval  -1   error and statement written on stderr
 *
 * NOTE: str2fn may return NULL on error and should then supply a (static) error string 
 * NOTE: str2fn does not use type-checking for its return value (the actual function)
 *       for a simpler implementation.
 *       If you need full type-checking, see the wrapper functions:
 *               cligen_callback_str2fn() and cligen_expand_str2fn()
 */
int
cligen_str2fn(parse_tree pt, 
	      str2fn_mapper *str2fn1, void *fnarg1, 
	      str2fn_mapper *str2fn2, void *fnarg2)
{
    int                 retval = -1;
    cg_obj             *co;
    char               *callback_err = NULL;   /* Error from str2fn callback */
    struct cg_callback *cc;
    int     i;

    for (i=0; i<pt.pt_len; i++){    
	if ((co = pt.pt_vec[i]) != NULL){
	    /* first map command callbacks */
	    if (str2fn1 != NULL)
		for (cc = co->co_callbacks; cc; cc=cc->cc_next){
		    if (cc->cc_fn_str != NULL && cc->cc_fn == NULL){
			cc->cc_fn = str2fn1(cc->cc_fn_str, fnarg1, &callback_err);
			if (callback_err != NULL){
			    fprintf(stderr, "%s: error: No such function: %s\n",
				    __FUNCTION__, cc->cc_fn_str);
			    goto done;
			}
		    }
		}
	    /* then variable expand callbacks */
	    if (str2fn2 != NULL)
		if (co->co_expand_fn_str != NULL && co->co_expand_fn == NULL){
		    co->co_expand_fn = str2fn2(co->co_expand_fn_str, fnarg2, &callback_err);
		    if (callback_err != NULL){
			fprintf(stderr, "%s: error: No such function: %s\n",
				__FUNCTION__, co->co_expand_fn_str);
			goto done;
		    }
		}
	    /* recursive call to next level */
	    if (cligen_str2fn(co->co_pt, str2fn1, fnarg1, str2fn2, fnarg2) < 0)
		goto done;
	}
    }
    retval = 0;
  done:
    return retval;
}

/*! Assign functions for callbacks in a parse-tree using a translate function
 *
 * This is wrapper for better type-checking of the mapper (str2fn) function. See 
 * cligen_str2fn for the underlying function (without type-checking).
 */
int
cligen_callback_str2fn(parse_tree pt, cg_str2fn_t *str2fn, void *fnarg)
{
    return cligen_str2fn(pt, (str2fn_mapper*)str2fn, fnarg, NULL, NULL);
}
