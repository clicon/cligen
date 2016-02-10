/*
  CLIgen hello world application

  Copyright (C) 2001-2016 Olof Hagsand

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

cg_fnstype_t *
str2fn(char *name, void *arg, char **error)
{
    return hello_cb;
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
    parse_tree        *pt;            /* cligen parse tree */
    cligen_handle      h;

    if ((h = cligen_init()) == NULL)
	goto done;
    if (cligen_parse_str(h, hello_syntax, "hello world", NULL, NULL) < 0)
	goto done;
    /* find global assignments: prompt and comment sign */
    cligen_prompt_set(h, "hello> ");
    cligen_comment_set(h, '#');
    /* Get the default (first) parse-tree */
    if ((pt = cligen_tree_i(h, 0)) == NULL)
	goto done;
    /* Bind callback (hello_cb) to all commands */
    if (cligen_callback_str2fn(*pt, str2fn, NULL) < 0)     
	goto done;
    /* Run the CLI command interpreter */
    if (cligen_loop(h) < 0)
	goto done;
    retval = 0;
  done:
    cligen_exit(h);
    return retval;
}

