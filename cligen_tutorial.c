/*
  CLIgen tutorial application

  Copyright (C) 2001-2014 Olof Hagsand

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
#include <errno.h>
#include <netinet/in.h>

#include <cligen/cligen.h>

/*
 * This callback just prints the function argument
 */
int
hello(cligen_handle h, cvec *vars, cg_var *arg)
{
    printf("%s\n", cv_string_get(arg));
    return 0;
}


/*
 * This is a generic callback printing the variable vector and argument
 */
int
cb(cligen_handle h, cvec *vars, cg_var *arg)
{
    int i=1;
    cg_var *cv;
    char buf[64];

    fprintf(stderr, "variables:\n");
    cv = NULL;
    while ((cv = cvec_each1(vars, cv)) != NULL) {
        cv2str(cv, buf, sizeof(buf)-1);
        fprintf(stderr, "\t%d name:%s type:%s value:%s\n", 
                i, 
                cv_name_get(cv),
                cv_type2str(cv_type_get(cv)),
                buf
            );

    }
    if (arg){
        cv2str(arg, buf, sizeof(buf)-1);
        fprintf(stderr, "argument: %s\n", buf);
    }
    return 0;
}

/*
 * An example of a callback handling a complex syntax
 */
int
letters(cligen_handle h, cvec *vars, cg_var *arg)
{
    char *str;
    cg_var *cv;

    if ((str = cvec_find_str(vars, "ca")) != NULL)
        printf("%s\n", str);
    if ((cv = cvec_find(vars, "int")) != NULL)
        printf("%d\n", cv_int32_get(cv));
    if ((str = cvec_find_str(vars, "cb")) != NULL)
        printf("%s\n", str);
    if ((str = cvec_find_str(vars, "dd")) != NULL)
        printf("%s\n", str);
    if ((str = cvec_find_str(vars, "ee")) != NULL)
        printf("%s\n", str);
    return 0;
}


/*
 * This callback changes the prompt to the variable setting
 */
int
secret(cligen_handle h, cvec *vars, cg_var *arg)
{
    printf("This is a hidden command: %s\n", cv_string_get(arg));
    return 0;
}


/*
 * This callback changes the prompt to the variable setting
 */
int
setprompt(cligen_handle h, cvec *vars, cg_var *arg)
{
    char *str;

    if ((str = cvec_find_str(vars, "new")) != NULL)
        cligen_prompt_set(h, str);
    return 0;
}

/*
 * request quitting the CLI
 */
int
quit(cligen_handle h, cvec *vars, cg_var *arg)
{
    cligen_exiting_set(h, 1);
    return 0;
}

/*! Change cligen tree
 */
int
changetree(cligen_handle h, cvec *vars, cg_var *arg)
{
    char *treename = cv_string_get(arg);

    return cligen_tree_active_set(h, treename);
}

/*
 * Command without assigned callback
 */
int
unknown(cligen_handle h, cvec *vars, cg_var *arg)
{
    cg_var *cv = cvec_i(vars, 0);

    printf("The command has no assigned callback: %s\n", cv_string_get(cv));
    return 0;
}


/*
 * str2fn
 * Example of static string to function mapper for the callback
 * functions above.
 * Better to use dlopen, mmap or some other more flexible scheme.
 */
cg_fnstype_t *
str2fn(char *name, void *arg, char **error)
{
    *error = NULL;
    if (strcmp(name, "hello") == 0)
        return hello;
    if (strcmp(name, "cb") == 0)
        return cb;
    if (strcmp(name, "add") == 0)
        return cb;
    if (strcmp(name, "del") == 0)
        return cb;
    if (strcmp(name, "letters") == 0)
        return letters;
    if (strcmp(name, "secret") == 0)
        return secret;
    if (strcmp(name, "setprompt") == 0)
        return setprompt;
    if (strcmp(name, "quit") == 0)
        return quit;
    if (strcmp(name, "changetree") == 0)
        return changetree;

    return unknown; /* allow any function (for testing) */
}

/*
 * This is an example of an expansion function. It is called every time
 * a variable of the form <expand> needs to be evaluated.
 * Note the mallocing of vectors which could probably be done in a
 * friendlier way.
 * Note also that the expansion is not very dynamic, a script or reading a file
 * would have introduced som more dynamics.
 */
int
cli_expand_cb(cligen_handle h, char *fn_str, cvec *vars, cg_var *cv, 
	      int  *nr,
	      char ***commands,     /* vector of function strings */
	      char ***helptexts)   /* vector of help-texts */
{
    int n = 2;
     /* Interface name expansion. */
    *commands = calloc(n, sizeof(char*));
    (*commands)[0] = strdup("eth0");
    (*commands)[1] = strdup("eth1");
    *nr = n;
    *helptexts = calloc(n, sizeof(char*));
    (*helptexts)[0] = strdup("Interface A");
    (*helptexts)[1] = strdup("Interface B");
    return 0;
}

static expand_cb *
str2fn_exp(char *name, void *arg, char **error)
{
    return cli_expand_cb;
}


/*
 * Global variables.
 */
static void 
usage(char *argv)
{
    fprintf(stderr, "Usage: %s [-h][-f <filename>]\n", argv);
    exit(0);
}

/* Main */
int
main(int argc, char *argv[])
{
    cligen_handle   h;
    int             retval = -1;
    parse_tree     *pt;
    FILE           *f = stdin;
    char           *argv0 = argv[0];
    char           *filename=NULL;
    cvec           *globals;   /* global variables from syntax */
    char           *str;

    argv++;argc--;
    for (;(argc>0)&& *argv; argc--, argv++){
        if (**argv != '-')
            break;
        (*argv)++;
        if (strlen(*argv)==0)
            usage(argv0);
        switch(**argv) {
        case 'h': /* help */
            usage(argv0); /* usage exits */
            break;
        case 'f' : 
            argc--;argv++;
            filename = *argv;
            if ((f = fopen(filename, "r")) == NULL){
		fprintf(stderr, "fopen %s: %s\n", filename, strerror(errno));
                exit(1);
            }
            break;
        default:
            usage(argv0);
            break;
        }
  }
    if ((h = cligen_init()) == NULL)
        goto done;    
    if ((globals = cvec_new(0)) == NULL)
	goto done;
    if (cligen_parse_file(h, f, filename, NULL, globals) < 0)
        goto done;
    pt = NULL;
    while ((pt = cligen_tree_each(h, pt)) != NULL) {
	if (cligen_callback_str2fn(*pt, str2fn, NULL) < 0) /* map functions */
	    goto done;
	if (cligen_expand_str2fn(*pt, str2fn_exp, NULL) < 0)
	    goto done;
    }
    if ((str = cvec_find_str(globals, "prompt")) != NULL)
        cligen_prompt_set(h, str);
    if ((str = cvec_find_str(globals, "comment")) != NULL)
        cligen_comment_set(h, *str);
    if ((str = cvec_find_str(globals, "tabmode")) != NULL)
	cligen_tabmode_set(h, strcmp(str,"long") == 0);
    cvec_free(globals);
    pt = NULL;
    while ((pt = cligen_tree_each(h, pt)) != NULL) {
	printf("Syntax:\n");
	cligen_print(stdout, *pt, 0);
    }
    fflush(stdout);

    if (cligen_loop(h) < 0)
	goto done;
    retval = 0;
  done:
    fclose(f);
    cligen_exit(h);
    return retval;
}
