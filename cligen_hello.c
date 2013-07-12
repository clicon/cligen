/*
  CLIgen hello world application

  CVS Version: $Id: cligen_hello.c,v 1.29 2013/04/12 10:59:52 olof Exp $ 
  Copyright (C) 2011-2013 Olof Hagsand

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>

#include <cligen/cligen.h>

/* Callbacks */
static int
hello_cb(cligen_handle h, cvec *vars, cg_var *arg)
{
    printf("%s\n", cv_string_get(arg));
    return 0;
}

/* This is the command syntax specification */
static char *hello_syntax = "prompt=\"hello> \";\n" 
    "hello(\"Greet the world\") world, cb(\"Hello World!\");"
    ;

/* Main */
int
main(int argc, char *argv[])
{
    int                retval = -1;
    int                cb_ret;
    char              *line;          /* line read from input */
    parse_tree         pt = {0,};     /* parse tree */
    cvec              *globals;       /* global variables from syntax */
    cg_var            *cv;
    cligen_handle      h;

    if ((h = cligen_init()) == NULL)
	goto done;
    if ((globals = cvec_new(0)) == NULL)
	goto done;
    if (cligen_parse_str(h, hello_syntax, "hello world", &pt, globals) < 0)
	goto done;
    cligen_callback_register(pt, hello_cb);
    /* find global assignments: prompt and comment sign */
    if ((cv = cvec_find(globals, "prompt")) != NULL)
	cligen_prompt_set(h, cv_string_get(cv));
    else
	fprintf(stderr, "prompt not found\n");
    cvec_free(globals);
    cligen_comment_set(h, '#');
    cligen_tree_add(h, "hello", pt);
    cligen_tree_active_set(h, "hello");
    /* Run the CLI command interpreter */
    while (!cligen_exiting(h)){
	switch (cliread_eval(h, &line, &cb_ret)){
	case CG_EOF: /* eof */
	    goto done;
	    break;
	case CG_ERROR: /* cligen match errors */
	    printf("CLI read error\n");
	    goto done;
	case CG_NOMATCH: /* no match */
	    printf("CLI syntax error in: \"%s\": %s\n", line, cligen_nomatch(h));
	    break;
	case CG_MATCH: /* unique match */
	    if (cb_ret < 0)
		printf("CLI callback error\n");
	    break;
	default: /* multiple matches */
	    printf("Ambigous command\n");
	    break;
	}
    }
    retval = 0;
  done:
    cligen_exit(h);
    return retval;
}

