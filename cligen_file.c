/*
  CLIgen application reading CLI specification from file

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
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <netinet/in.h>

#include <cligen/cligen.h>

/*
 * cligen_exec_cb
 * General callback for executing shells. The argument is a command
 * followed by arguments as defined in the input syntax.
 * Simple example:
 *   CLIgen input syntax:     <a type:int32>, cligen_exec_cb("ls ${a}");
 *   CLI input:               > 42
 *   Shell command:           ls 42
 * More advanced example:
 *   CLIgen input syntax:     [<a type:int> | <b type:ipv4addr>], 
 *                                cligen_exec_cb("foo.sh ${a:-99} ${b:-1.2.3.4}");
 *   CLI input:               > 22
 *   Shell command:           foo.sh 22 1.2.3.4
 *   CLI input:               > 2.3.4.5
 *   Shell command:           foo.sh 99 1.2.3.4.
 *
 */
int
cligen_exec_cb(cligen_handle handle, cvec *vars, cg_var *arg)
{
    cg_var *cv = NULL;
    char    buf[64];
    int     pid;
    int     ret;
    int     status;

    if (arg == NULL)
	return 0;
    if ((pid = fork()) == 0){ /* child */
	while ((cv = cvec_each1(vars, cv)) != NULL) {
	    if (cv_const_get(cv))
		continue;
	    cv2str(cv, buf, sizeof(buf)-1);
	    setenv(cv_name_get(cv), buf, 1 );
	}
	cv2str(arg, buf, sizeof(buf)-1);
	ret = system(buf);
	exit(0);
    }
    /* Wait for child to finish */
    if(waitpid (pid, &status, 0) == pid)
	ret = WEXITSTATUS(status);
    else
	ret = -1;
    return ret;
}

#ifdef notused
/*
 * This is an example of a generic expansion function which starts a program
 * and creates an expand list based on its output
 * That is, the program should be an executable script or program that produces
 * a list of output strings.
 * The name of the file is expand_<name>
 */
int
cli_expand_fn(cligen_handle h,
	      char *name, 
	      cvec *vars, 
	      cg_var *cv,
	      int *nr, 
	      char ***commands)
{
    int pid;
    int p[2];
    int retval = -1;
    char *filename;
    char buf[1024];
    FILE *f;
    struct stat st;
    int status;
    int n;
    int len;

    len = strlen(name) + strlen("expand_") + 1;
    if ((filename = malloc(len)) == NULL){
	fprintf(stderr, "%s: malloc: %s\n", __FUNCTION__, strerror(errno));
	goto done;
	}
    snprintf(filename, len, "expand_%s", name);
    if (stat(filename, &st) < 0){
	fprintf(stderr, "%s: stat(%s): %s\n", __FUNCTION__, filename, strerror(errno));
	goto done;
    }
    if (pipe(p) < 0){
	fprintf(stderr, "%s: pipe: %s\n", __FUNCTION__, strerror(errno));
	goto done;
    }
    if ((pid = fork()) < 0){ 
	fprintf(stderr, "%s: pipe: %s\n", __FUNCTION__, strerror(errno));
	goto done;
    }
    if (pid == 0){ /* child */
	char *argv[3];
	argv[0] = filename;
	argv[1] = name;
	argv[2] = NULL;
	close(p[0]);
	p[0] = -1;
	dup2(p[1], STDOUT_FILENO);
	if (execvp(filename, argv) < 0){
	    fprintf(stderr, "%s: execvp(%s): %s\n", __FUNCTION__, 
		    filename, strerror(errno));
	    exit(-1);
	}
	exit(0);
    }
    /* parent */
    close (p[1]);
    p[1] = -1;	
    if ((f = fdopen(p[0], "r")) == NULL){ 
	fprintf(stderr, "%s: fdopen: %s\n", __FUNCTION__, strerror(errno));
	goto done;
    }
    n = *nr;
    while(fgets(buf, sizeof(buf), f)){
	n++;
	if ((*commands = realloc(*commands, n*sizeof(char*))) == NULL){
	    fprintf(stderr, "%s: realloc: %s\n", __FUNCTION__, strerror(errno));
	    goto done;
	}
	if (buf[strlen(buf)-1] == '\n')
	    buf[strlen(buf)-1] = '\0';
	(*commands)[n-1] = strdup(buf);
    }
    *nr = n;
    close(p[0]);
    fclose(f);
    /* Wait for child to finish */
    if(waitpid (pid, &status, 0) == pid)
	retval = WEXITSTATUS(status);
    else
	retval = -1;
  done:
    if (filename)
	free(filename);
    return retval;
}
#endif /* notused */

/*
 * This is the actual callback.
 */
int
callback(cligen_handle handle, cvec *vars, cg_var *arg)
{
    int i = 1;
    cg_var *cv;
    char buf[64];

    fprintf(stderr, "variables:\n");

    cv = NULL;
    while ((cv = cvec_each1(vars, cv)) != NULL) {
	cv2str(cv, buf, sizeof(buf)-1);
	fprintf(stderr, "\t%d name:%s type:%s value:%s\n", 
		i++, 
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
 * Example of static string to function mapper
 * Note, the syntax need to something like: "a{help}, callback, 42"
 */
cg_fnstype_t *
str2fn(char *name, void *arg, char **error)
{
    *error = NULL;
    if (strcmp(name, "callback") == 0)
	return callback;
    if (strcmp(name, "cligen_exec_cb") == 0)
	return cligen_exec_cb;
    return callback; /* allow any function (for testing) */
}

/*
 * Global variables.
 */
static void 
usage(char *argv)
{
    fprintf(stderr, "Usage: %s [-h][-f <filename>][-q][-p]\n", argv);
    exit(0);
}

/* Main */
int
main(int argc, char *argv[])
{
    int         retval = -1;
    parse_tree  pt = {0,};
    int         callback_ret = 0;
    FILE       *f = stdin;
    char       *argv0 = argv[0];
    char       *line;
    char       *filename=NULL;
    cvec       *globals;   /* global variables from syntax */
    cligen_handle  h;
    char       *str;
    char       *treename;
    int         quit = 0;
    int         print_syntax = 0;

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
	case 'q': /* quit directly */
	    quit++;
	    break;
	case 'p': /* print syntax */
	    print_syntax++;
	    break;
	case 'f' : 
	    argc--;argv++;
	    filename = *argv;
	    if ((f = fopen(filename, "r")) == NULL){
		fprintf(stderr, "fopen(%s): %s\n", filename, strerror(errno));
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
    cligen_lexicalorder_set(h, 1);
    cligen_ignorecase_set(h, 1);
//    cligen_parse_debug(1);
    if ((globals = cvec_new(0)) == NULL)
	goto done;
    if (cligen_parse_file(h, f, filename?filename:"stdin", &pt, globals) < 0)
	goto done;

    /* map functions */
    if (cligen_callback_str2fn(pt, str2fn, NULL) < 0)     
	goto done;
    if ((str = cvec_find_str(globals, "prompt")) != NULL)
	cligen_prompt_set(h, str);
    if ((str = cvec_find_str(globals, "tabmode")) != NULL)
	cligen_tabmode_set(h, strcmp(str,"long") == 0);
    if ((str = cvec_find_str(globals, "comment")) != NULL)
	cligen_comment_set(h, *str);
    if ((treename = cvec_find_str(globals, "name")) == NULL)
	treename = "tree0";
    cligen_tree_active_set(h, treename); 
    cligen_tree_add(h, treename, pt); 
    cvec_free(globals);
    if (quit)
	goto done;
    if (print_syntax){
	printf("Syntax:\n");
	cligen_print(stdout, pt, 0);
	fflush(stdout);
    }

    /* Run the CLI command interpreter */
    while (!cligen_exiting(h)){
	switch (cliread_eval(h, &line, &callback_ret)){
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
	    if (callback_ret < 0)
		printf("CLI callback error\n");
	    break;
	default: /* multiple matches */
	    printf("Ambigous command\n");
	    break;
	}
    }
    retval = 0;
  done:
    fclose(f);
    cligen_exit(h);
    return retval;
}
