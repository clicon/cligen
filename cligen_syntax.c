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
/*! \file */ 
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

/*!
 * \brief Parse a string containing a CLIgen spec into a parse-tree
 * 
 * Syntax parsing. A string is input and a syntax-tree is returned (or error). 
 * A variable record is also returned containing a list of (global) variable values.

 * The string contains a hierarchy of syntax specs bounded by {} and semi-colon. Comma is used
 * to tag a syntax-spec with assignments or callbacks. Help strings are delimited with ("").
 * '#' anywhere on the line means the rest is comment.
 * Example:
 * global=foo;
 * a("command") b, fn1();{
 *    c("help"), fn2("arg"), local=3;
 *    d("help");
 * }
 * (e | d)
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
	if (cligen_parseparse(&ya) != 0) {
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

/*! 
 * \brief Parse a file containing a CLIgen spec into a parse-tree
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


/*! 
 * \brief  Register (same) callback for all commands in a syntax.
 *
 * Regardless of setting (sy_callback_str) in syntax.
 */
int
cligen_callback_register(parse_tree pt, cg_fnstype_t *fn)
{
    int i;
    cg_obj *co;
    struct cg_callback *cc;

    for (i=0; i<pt.pt_len; i++){    
	if ((co = pt.pt_vec[i]) != NULL){
	    if ((cc = co->co_callbacks) == NULL){
		if ((cc = malloc(sizeof(*cc))) == NULL){
		    fprintf(stderr, "%s: malloc: %s\n", __FUNCTION__, strerror(errno));
		    return -1;
		}
		memset(cc, 0, sizeof(*cc));
		co->co_callbacks = cc;
	    }
	    cc->cc_fn = fn;
	    cligen_callback_register(co->co_pt, fn);
	}
    }
    return 0;
}

/*
 * \brief  Register callback for commands using callback function
 *
 * Alternative is to just loop yourself
 * The function also requires a mapping function between strings and functions.
 * This is a little hairy and can be solved in several ways, but the problem is 
 * as follows. The file contains callback names as strings. But the parse-tree
 * stores the function as a 'cg_fnstype_t', that is a resolved symbol (LISP would have
 * been nice,...). We could save the string name of the function and make the
 * function lookup later (in the evaluation) or we do it now when we build the
 * parse-tree. I settled on this later approach.
 * Anyway, the caller therefore has to supply a function which maps strings to 
 * functions. This could be done statically, but more powerful is to use dynamic
 * loading. In both these cases, thislibrary routine cannot do the mapping so
 * it needs the information in some way. A vector as input could do the job, but
 * in this way a dynamic search is easier to make. 
 * NOTE: str2fn can return NULL as a valid value, therefore it should allocate
 * an error string and return it in the error parameter.
 */
int
cligen_callback_str2fn(parse_tree pt, cg_str2fn_t *str2fn, void *fnarg)
{
    int     i;
    cg_obj *co;
    char   *callback_err = NULL;   /* Error from str2fn callback */
    struct cg_callback *cc;

    for (i=0; i<pt.pt_len; i++){    
	if ((co = pt.pt_vec[i]) != NULL){
	    for (cc = co->co_callbacks; cc; cc=cc->cc_next){
		if (cc->cc_fn_str != NULL && cc->cc_fn == NULL){
		    cc->cc_fn = str2fn(cc->cc_fn_str, fnarg, &callback_err);
		    if (callback_err != NULL){
			fprintf(stderr, "%s: error: No such function: %s\n",
				__FUNCTION__, cc->cc_fn_str);
			return -1;
		    }
		}
	    }
	    if (cligen_callback_str2fn(co->co_pt, str2fn, fnarg) < 0)
		return -1;
	}
    }
    return 0;
}

