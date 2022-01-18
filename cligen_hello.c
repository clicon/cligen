/*
  CLIgen hello world application

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>

#include <cligen/cligen.h>

/*! CLI callback that just prints the function argument
 */
static int
hello_cb(cligen_handle h, cvec *cvv, cvec *argv)
{
    cg_var *cv;

    cv = cvec_i(argv, 0);
    printf("%s\n", cv_string_get(cv));
    return 0;
}

/*! Trivial function translator/mapping function that just assigns same callback
 */
cgv_fnstype_t *
str2fn(char *name, void *arg, char **error)
{
    return hello_cb;
}


/*! The command syntax specification */
static char *hello_syntax = "prompt=\"hello> \";\n" 
    "hello(\"Greet the world\") world, cb(\"Hello World!\");"
    ;

int
main(int argc, char *argv[])
{
    int             retval = -1;
    parse_tree     *pt;            /* cligen parse tree */
    pt_head        *ph;            /* cligen parse tree head */
    cligen_handle   h;

    if ((h = cligen_init()) == NULL)
	goto done;
    if (cligen_parse_str(h, hello_syntax, "hello world", NULL, NULL) < 0)
	goto done;
    /* find global assignments: prompt and comment sign */
    cligen_prompt_set(h, "hello> ");
    cligen_comment_set(h, '#');
    /* Get the default (first) parse-tree */
    if ((ph = cligen_ph_i(h, 0)) == NULL)
	goto done;
    pt = cligen_ph_parsetree_get(ph);
    /* Bind callback (hello_cb) to all commands */
    if (cligen_callbackv_str2fn(pt, str2fn, NULL) < 0)     
	goto done;
    /* Run the CLI command interpreter */
    if (cligen_loop(h) < 0)
	goto done;
    retval = 0;
  done:
    if (h)
	cligen_exit(h);
    return retval;
}

