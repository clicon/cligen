/*
  CLIgen tutorial application

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
#include <errno.h>
#include <netinet/in.h>

#include <cligen/cligen.h>

/*! CLI callback that just prints the function argument
 */
int
hello(cligen_handle h,
      cvec         *cvv,
      cvec         *argv)
{
    cg_var *cv;

    cv = cvec_i(argv, 0);
    printf("%s\n", cv_string_get(cv));
    return 0;
}

/*! CLI generic callback printing the variable vector and argument
 */
int
callback(cligen_handle h,
	 cvec         *cvv,
	 cvec         *argv)
{
    int     i = 0;
    cg_var *cv;
    char    buf[64];

    cligen_output(stderr, "function: %s\n", cligen_fn_str_get(h));
    cligen_output(stderr, "variables:\n");
    cv = NULL;
    while ((cv = cvec_each(cvv, cv)) != NULL) {
	cv2str(cv, buf, sizeof(buf)-1);
	cligen_output(stderr, "\t%d name:%s type:%s value:%s\n", 
		i++, 
		cv_name_get(cv),
		cv_type2str(cv_type_get(cv)),
		buf
	    );
    }
    if (argv){
	    cv = NULL;
	    i=0;
	    while ((cv = cvec_each(argv, cv)) != NULL) {
		cv2str(cv, buf, sizeof(buf)-1);
		cligen_output(stderr, "arg %d: %s\n", i++, buf);
	    }
	}
    return 0;
}

/*! CLI example callback handling a complex syntax
 */
int
letters(cligen_handle h,
	cvec         *cvv,
	cvec         *argv)
{
    char   *str;
    cg_var *cv;

    if ((str = cvec_find_str(cvv, "ca")) != NULL)
        printf("%s\n", str);
    if ((cv = cvec_find(cvv, "int")) != NULL)
        printf("%d\n", cv_int32_get(cv));
    if ((str = cvec_find_str(cvv, "cb")) != NULL)
        printf("%s\n", str);
    if ((str = cvec_find_str(cvv, "dd")) != NULL)
        printf("%s\n", str);
    if ((str = cvec_find_str(cvv, "ee")) != NULL)
        printf("%s\n", str);
    return 0;
}


/*! This callback is for hidden commands
 */
int
secret(cligen_handle h,
       cvec         *cvv,
       cvec         *argv)
{
    cg_var *cv;

    cv = cvec_i(argv, 0);
    printf("This is a hidden command: %s\n", cv_string_get(cv));
    return 0;
}


/*! This callback changes the prompt to the variable setting
 */
int
setprompt(cligen_handle h,
	  cvec         *cvv,
	  cvec         *argv)
{
    char *str;

    if ((str = cvec_find_str(cvv, "new")) != NULL)
        cligen_prompt_set(h, str);
    return 0;
}

/*! Request quitting the CLI
 */
int
quit(cligen_handle h,
     cvec         *cvv,
     cvec         *argv)
{
    cligen_exiting_set(h, 1);
    return 0;
}

/*! Change cligen tree
 */
int
changetree(cligen_handle h,
	   cvec         *cvv,
	   cvec         *argv)
{
    cg_var *cv;
    char *treename;

    cv = cvec_i(argv, 0);
    treename = cv_string_get(cv);
    return cligen_ph_active_set_byname(h, treename);
}

/*! Command without assigned callback
 */
int
unknown(cligen_handle h,
	cvec         *cvv,
	cvec         *argv)
{
    cg_var *cv = cvec_i(cvv, 0);

    fprintf(stderr, "The command has no assigned callback: %s\n", cv_string_get(cv));
    return 0;
}

/*! Example of static string to function mapper for the callback functions above.
 * Better to use dlopen, mmap or some other more flexible scheme.
 */
cgv_fnstype_t *
str2fn(char  *name,
       void  *arg,
       char **error)
{
    *error = NULL;
    if (strcmp(name, "hello") == 0)
        return hello;
    else     if (strcmp(name, "cb") == 0)
        return callback;
    else     if (strcmp(name, "add") == 0)
        return callback;
    else     if (strcmp(name, "del") == 0)
        return callback;
    else     if (strcmp(name, "letters") == 0)
        return letters;
    else     if (strcmp(name, "secret") == 0)
        return secret;
    else     if (strcmp(name, "setprompt") == 0)
        return setprompt;
    else     if (strcmp(name, "quit") == 0)
        return quit;
    else if (strcmp(name, "changetree") == 0)
        return changetree;
    else if (strcmp(name, "cligen_wp_set") == 0)
        return cligen_wp_set;
    else if (strcmp(name, "cligen_wp_show") == 0)
        return cligen_wp_show;
    else if (strcmp(name, "cligen_wp_up") == 0)
        return cligen_wp_up;
    else if (strcmp(name, "cligen_wp_top") == 0)
        return cligen_wp_top;
    return unknown; /* allow any function (for testing) */
}

/*! Example of expansion(completion) function. 
 * It is called every time a variable of the form <expand> needs to be evaluated.
 * Note the mallocing of vectors which could probably be done in a
 * friendlier way.
 * Note also that the expansion is not very dynamic, a script or reading a file
 * would have introduced som more dynamics.
 */
int
cli_expand_cb(cligen_handle h, 
	      char         *fn_str, 
	      cvec         *cvv, 
	      cvec         *argv, 
	      cvec         *commands,     /* vector of function strings */
	      cvec         *helptexts)   /* vector of help-texts */
{
    cvec_add_string(commands, NULL, "eth0");
    cvec_add_string(commands, NULL, "eth1");
    cvec_add_string(helptexts, NULL, "Interface A");
    cvec_add_string(helptexts, NULL, "Interface B");
    return 0;
}

/*! Trivial function translator/mapping function that just assigns same callback
 */
static expandv_cb *
str2fn_exp(char  *name,
	   void  *arg,
	   char **error)
{
    return cli_expand_cb;
}

/*! Translate function from an original value to a new.
 * In this case, assume string and increment characters, eg HAL->IBM
 */
int
incstr(cligen_handle h,
       cg_var       *cv)
{
    char *str;
    int i;
    
    if (cv_type_get(cv) != CGV_STRING)
	return 0;
    str = cv_string_get(cv);
    for (i=0; i<strlen(str); i++)
	str[i]++;
    return 0;
}

/* Translating functions of type translate_str2fn_t 
 * See the following rule in tutorial_cli:
 *     increment <var:string translate:incstr()>, callback();
 */
translate_cb_t *
str2fn_trans(char  *name,
	     void  *arg,
	     char **error)
{
    if (strcmp(name, "incstr") == 0)
	return incstr;
    return NULL;
}

/*
 * Global variables.
 */
static void 
usage(char *argv)
{
    fprintf(stderr, "Usage:%s [-h][-f <filename>][-q], where the options have the following meaning:\n"
	    "\t-h \t\tHelp\n"
	    "\t-f <file> \tConfig-file (or stdin) Example use: tutorial.cli for \n"
	    "\t-q \t\tQuiet\n"
	    "\t-C \t\tDont copy treeref mode\n"
	    ,
	    argv);
    exit(0);
}

/* Main */
int
main(int   argc,
     char *argv[])
{
    cligen_handle   h;
    int             retval = -1;
    parse_tree     *pt;
    pt_head        *ph;
    FILE           *f = stdin;
    char           *argv0 = argv[0];
    char           *filename=NULL;
    cvec           *globals;   /* global variables from syntax */
    char           *str;
    int             quiet = 0;

    if ((h = cligen_init()) == NULL)
        goto done;    
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
        case 'q': /* quiet */
	    quiet++;
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
    if ((globals = cvec_new(0)) == NULL)
	goto done;
    if (cligen_parse_file(h, f, filename, NULL, globals) < 0)
        goto done;
    ph = NULL;
    while ((ph = cligen_ph_each(h, ph)) != NULL) {
	pt = cligen_ph_parsetree_get(ph);
	if (cligen_callbackv_str2fn(pt, str2fn, NULL) < 0) /* map functions */
	    goto done;
	if (cligen_expandv_str2fn(pt, str2fn_exp, NULL) < 0)
	    goto done;
	if (cligen_translate_str2fn(pt, str2fn_trans, NULL) < 0)     
	    goto done;
    }
    if ((str = cvec_find_str(globals, "prompt")) != NULL)
        cligen_prompt_set(h, str);
    if ((str = cvec_find_str(globals, "comment")) != NULL)
        cligen_comment_set(h, *str);
    if ((str = cvec_find_str(globals, "tabmode")) != NULL)
	if (strcmp(str,"long") == 0)
	    cligen_tabmode_set(h, CLIGEN_TABMODE_COLUMNS);
    cvec_free(globals);
    if (!quiet){
	ph = NULL;
	while ((ph = cligen_ph_each(h, ph)) != NULL) {
	    pt = cligen_ph_parsetree_get(ph);
	    printf("Syntax:\n");
	    pt_print1(stdout, pt, 0);
	}
	fflush(stdout);
    }
    if (cligen_loop(h) < 0)
	goto done;
    retval = 0;
 done:
    fclose(f);
    if (h)
	cligen_exit(h);
    return retval;
}
