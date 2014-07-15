/*
  CLI generator readline. Do input processing and matching.
  CVS Version: $Id: cligen_read.c,v 1.33 2013/06/16 11:56:11 olof Exp $ 

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
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef WIN32
#include <winsock2.h>
#else
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#endif /* WIN32 */
#define __USE_GNU /* isblank() */
#include <ctype.h>
#include <assert.h>

#ifndef isblank
#define isblank(c) (c==' ')
#endif /* isblank */

#include "cligen_var.h"
#include "cligen_cvec.h"
#include "cligen_gen.h"
#include "cligen_handle.h"
#include "cligen_print.h"
#include "cligen_io.h"
#include "cligen_match.h"
#include "cligen_expand.h"
#include "cligen_read.h"
#include "getline.h"

/*
 * Constants and Macros
 */
#define FALSE 0
#define TRUE 1

/*
 * Local prototypes
 */
static int show_multi(cligen_handle h, FILE *fout, char *s, parse_tree pt, cvec *cvec);
static int show_multi_long(cligen_handle h, FILE *fout, char *s, parse_tree pt, cvec *);
static int complete(cligen_handle h, char *s0, int *lenp, parse_tree pt, cvec *cvec);


/*
 * cli_qmark_hook
 * Callback from getline: '?' has been pressed. 
 * Just show help by calling long help show function. 
 * INPUT:
 *   string - Input string to match
 *   cursor_loc - Location of cursor
 * RETURNS:
 *   1 (OK: required by getline)
 * XXX: Two flaws that have to do with getline:
 *   2) Errors from sub-functions are ignored
 */
static int
cli_qmark_hook (void *arg, char *string, int cursor_loc)
{
    cligen_handle h = (cligen_handle)arg;
    parse_tree   *pt;     /* Orig parse-tree */
    int           retval = -1;
    char         *mode;
    parse_tree    ptn={0,};     /* Expanded */
    cvec         *cvec = NULL;

    fputs ("\n", stdout);
    mode = cligen_tree_active(h);
    if ((pt = cligen_tree(h, mode)) == NULL){
	fprintf(stderr, "No such parse-tree registered: %s\n", mode);
	return -1;
    }
    if (pt_expand_1(h, NULL, pt) < 0) /* sub-tree expansion */
	goto quit; 
    if ((cvec = cvec_start(string)) == NULL)
	goto quit;
    if (pt_expand_2(h, pt, cvec, &ptn, 1) < 0)      /* expansion */
	return -1;
    if (show_multi_long(h, stdout, string, ptn, cvec) <0)
	goto quit;
    retval = 1;
  quit:
    if (cvec)
	cvec_free(cvec);
    if (cligen_parsetree_free(ptn, 0) < 0)
	return -1;
    if (pt_expand_cleanup_2(*pt) < 0) 
	return -1;
    if (pt_expand_cleanup_1(pt) < 0) 
	goto quit;
    return retval;
}

/*
 * cli_tab_hook
 * Callback from getline: TAB has been pressed. 
 * First try to complete the string if the possibilities
 * allow that (at least one unique common character). 
 * If no completion was made, then show the command alternatives.
 * INPUT:
 *   string - Input string to match
 *   prompt_width - not used (required by getline)
 * INPUT_OUTPUT
 *   cursorp - Pointer to location of cursor on entry and exit
 * RETURNS:
 *   -2 (value != -1 required by getline)
 * XXX: Two flaws that have to do with getline:
 *   2) Errors from sub-functions are ignored
 */
static int 
cli_tab_hook(void *arg, char *string, int prompt_width, int *cursorp)
{
    cligen_handle h = (cligen_handle)arg;
    int	          old_cursor;
    parse_tree   *pt;     /* Orig */
    int           retval = -1;
    char         *treename;
    parse_tree    ptn={0,};     /* Expanded */
    cvec         *cvec = NULL;

    old_cursor = *cursorp;  /* Save old location of cursor */
    treename = cligen_tree_active(h);
    if ((pt = cligen_tree(h, treename)) == NULL){
	fprintf(stderr, "No such parse-tree registered: %s\n", treename);
	return -1;
    }
    if (pt_expand_1(h, NULL, pt) < 0) /* sub-tree expansion */
	goto quit;
    if ((cvec = cvec_start(string)) == NULL)
	goto quit; 
    if (pt_expand_2(h, pt, cvec, &ptn, 1) < 0)      /* expansion */
	return -1;
    if (complete(h, string, cursorp, ptn, cvec) < 0)
	goto quit;
    else {
	if (old_cursor == *cursorp) { 	/* Cursor hasnt changed */
	    fputs ("\n", stdout);
	    if (cligen_tabmode(h) == 1){
		if (show_multi_long(h, stdout, string, ptn, cvec) < 0)
		    goto quit;
	    }
	    else
		if (show_multi(h, stdout, string, ptn, cvec) < 0)
		    goto quit;
	}
    }
    retval = -2; /* To getline: -2 means new line, redraw everything.. */
  quit:
    if (cvec)
	cvec_free(cvec);
    if (cligen_parsetree_free(ptn, 0) < 0)
	return -1;
    if (pt_expand_cleanup_2(*pt) < 0)
	return -1;
    if (pt_expand_cleanup_1(pt) < 0)
	return -1;
    return retval;	
}

/*
 * cliread_init
 */
void
cliread_init(cligen_handle h)
{
    gl_qmark_hook = cli_qmark_hook;
    gl_tab_hook = cli_tab_hook; /* XXX globals */
}

/*
 * column_print
 */
static int
column_print(FILE *fout, int col, pt_vec pt, int min, int max, int level)
{ 
  int    i = 0;
  int    j = 0;
  int    count = max-min;
  int    d_lines;
  int    linesize = 0;
  char   *line;
  cg_obj *co;
  char   varstr[128];

  d_lines = count/col + 1;
  linesize = col * COLUMN_WIDTH;
  if ((line = (char *)malloc (linesize+1)) == NULL) {
    perror("column_print: malloc");
    return -1;
  }
  for(i = 0; i < d_lines; i++) {
    memset(line, ' ', linesize);
    while ((j < col) && ((j * d_lines + i) <= count)) {
	if ((co = pt[min + i+j*d_lines]) != NULL){
	    if (co->co_command != NULL){
		switch (co->co_type){
		case CO_VARIABLE:
		    cov_print(co, varstr, sizeof(varstr), 1);
		    memcpy(&line[j*(COLUMN_WIDTH+1)], varstr, 
			   (COLUMN_WIDTH < strlen(varstr)) ? COLUMN_WIDTH : strlen(varstr));
		    break;
		case CO_COMMAND:
		    memcpy(&line[j*(COLUMN_WIDTH+1)], co->co_command, 
			   (COLUMN_WIDTH < strlen(co->co_command)) ? COLUMN_WIDTH : strlen(co->co_command));
		    break;
		default:
		    break;
		}
	    }
	}
	j++;
    }
    line[col*COLUMN_WIDTH]='\0';
    j = 0;
    fprintf(fout, "%s\n", line);  
  }  
  free(line);
  return 0;
}

/*
 * show_multi
 * Show briefly the commands available (show no help)
 * Typically called when TAB is pressed and there are multiple options.
 * IN:
 *   fout    This is where the output (help text) is shown.
 *   string  Input string to match
 *   pt      Vector of commands (array of cligen object pointers (cg_obj)
 *   pt_max  Length of the pt array
 * OUT:
 *   cvec      cligen variable vector containing vars/values pair for completion
 * RETURNS:
 *   0       OK
 *  -1       Error
 */
static int
show_multi(cligen_handle h, 
	   FILE         *fout, 
	   char         *string, 
	   parse_tree    pt, 
	   cvec         *cvec)
{
    int    nr = 0;
    int    level;
    pt_vec pt1;
    int    matchlen = 0;
    int   *matchv = NULL;
    int    retval = -1;

    if (string != NULL){
	if ((nr = match_pattern(h, string, pt, 0, 1,
				&pt1, &matchv, &matchlen, cvec, NULL)) < 0)
	    goto done;
    }
    if ((level = command_levels(string)) < 0)
	goto done;
    
    if (nr>0) /* min, max only defined if nr > 0 */
	column_print(fout, 3, pt1, 
		     matchv[0],
		     matchv[nr-1],
		     level);
    retval = 0;
  done:
    if (matchv)
	free(matchv);
    return retval;
}

/*
 * show_multi_long
 * Show one row per command with help text for each command
 * Typically called when a question mark is pressed
 * IN:
 *   h       cligen handle
 *   fout    This is where the output (help text) is shown.
 *   string  Input string to match
 *   pt      Parse tree
 * OUT:
 *   cvec      cligen variable vector containing vars/values pair for completion
 * RETURNS:
 *   0       OK
 *   -1      Error
 *
 * Example from JunOS
# set interfaces ?
Possible completions:
  <interface_name>     Interface name
+ apply-groups         Groups from which to inherit configuration data
+ apply-groups-except  Don't inherit configuration data from these groups
  fe-0/0/1             Interface name
> interface-set        Logical interface set configuration
> traceoptions         Interface trace options

 */
static int
show_multi_long(cligen_handle h, 
		FILE         *fout, 
		char         *string, 
		parse_tree    pt, 
		cvec         *cvec)
{
    int          nr = 0;
    int	         i;
    int          level;
    pt_vec       pt1;
    char	*tmp;
    char	*tmpp;
    char 	 cmd[COLUMN_WIDTH+1];
    int          matchlen = 0;
    int         *matchv = NULL;
    int          mv;
    int          res;
    char         var[128];
    int          retval = -1;
    cg_obj      *co;
    int          skip;

    memset(cmd, 0, COLUMN_WIDTH+1); /* always a zero in last char */
    /* Build match vector, but why would string ever be NULL? */
    if (string != NULL){
	if ((nr = match_pattern(h, string, pt, 0, 1, &pt1, &matchv, &matchlen, cvec, NULL)) < 0)
	    goto done;
    }
    if ((level = command_levels(string)) < 0)
	goto done;

    /* If last char is blank, look for next level in parse-tree 
       eg, syntax is x (y|z) and we have typed 'x ' then show
       help for y and z, not x.
     */
    if (strlen(string) && isblank(string[strlen(string)-1])){
	if ((tmpp = tmp = strdup(string)) == NULL){
	    perror("show_multi_long: strdup");
	    goto done;
	}
	cli_trim(&tmpp, cligen_comment(h));

	/* if it is ok to <cr> here (at end of one mode) 
	   Example: x [y|z] and we have typed 'x ', then show
	   help for y and z and a 'cr' for 'x'.
	 */
	if ((res = match_pattern_exact(h, tmpp, pt, 0, cvec, NULL))  < 0){
	    free(tmp);
	    goto done;
	}

	if (res) {
	    fprintf (fout, "  <cr>\n");
	    fflush (fout);
	}
	free (tmp);
    }
  
    if (!nr){
	retval = 0;
	goto done;
    }

    /* Go through match vector */
    for (i = 0; i<matchlen; i++){
	assert((mv = matchv[i])!=-1);
	co = pt1[mv];
	if (co->co_command != NULL){
	    skip = 0;
	    switch (co->co_type){
	    case CO_VARIABLE:
		cov_print(co, var, sizeof(var)-1, 1);
		snprintf(cmd, COLUMN_WIDTH, "%s", var);
		break;
	    case CO_COMMAND:
		strncpy (cmd, co->co_command, COLUMN_WIDTH);
		break;
	    case CO_REFERENCE:
		skip++;
		break;
	    default:
		break;
	    }
	    if (!skip)
		fprintf (fout, "  %*s %s\n", 
			 -COLUMN_WIDTH, 
			 cmd,
			 co->co_help ? co->co_help : "");
	}
	fflush (fout);
    }
    retval = 0;
  done:
    if (matchv)
	free(matchv);
    return retval;
}

/*
 * complete
 * Try to complete a command as much as possible.
 * INPUT:
 *   string  Input string to match
 *   cursorp Pointer to the current cursor in string.
 *   pt      Vector of commands (array of cligen object pointers (cg_obj)
 *   pt_max  Length of the pt array
 * RETURNS:
 *   -1      on Error
 *   0       success
 */
static int 
complete(cligen_handle h, char *string, int *cursorp, parse_tree pt, cvec *cvec)
{
    char    *s;
    int     cursor = *cursorp;
    int     i, n;
    int     extra;

    if (string == NULL)
	return -1;
    if ((s = malloc(gl_bufsize(h))) == NULL){ /* s is a temporary copy */
	perror("complete: malloc");
	return -1;
    }
    strncpy(s, string, gl_bufsize(h));
    s[cursor] = '\0';
    if (match_complete(h, s, pt, gl_bufsize(h), cvec) < 0)
	return -1;
    extra = strlen(s) - cursor;      /* Extra characters added? */
    if (extra){
	n = strlen(string) - cursor; /* Nr of chars right of cursor to copy */
	for (i=cursor+n; i>=cursor; i--)             /* Copy right of cursor */
	    string[i + extra] = string[i]; 
	strncpy(string + cursor, s + cursor, extra); /* Copy the new stuff */
	*cursorp += extra;                           /* Increase cursor */
    }
    if (s)
	free(s);
    return 0;
}

/*
 * cli_trim
 * Trim command line. Remove any leading, trailing and multiple whitespace
 * comment is a character (eg '#')
 */
void
cli_trim (char **line, char comment)
{
    int		point;
    int		whitespace = 0;
    char	*s = *line;
    char	*s1 = s;

    if (!isascii(comment))
	comment = 0;
    for (point = 0; point <= strlen(s) ; point++) {
	if (comment && s[point] == comment){
	    *s1++ = '\n';
	    *s1++ = '\0';
	    break;
	}
	else
	    if (isblank(s[point])) {
		if (whitespace)
		    continue;
		else {
		    whitespace = 1;
		    *s1++ = ' ';
		}
	    } else {
		whitespace = 0;
		*s1++ = s[point];
	    }
    }
    
    /* strip heading whites */
    while ((strlen(s) > 0) && isblank(*s))
	s++;
    
    /* strip trailing whites and newlines */
    while ((strlen(s) > 0) && (isblank(*(s+strlen(s)-1)) || *(s+strlen(s)-1) == '\n'))
	*(s + strlen(s) - 1) = '\0';
    
    
    *line = s;    
}


/*! Given an input string, return a parse-tree.
 *
 * Given an input string and a parse-tree, return a matching parse-tree node, a
 * CLIgen keyword and CLIgen variable record vector. 
 * Some complexity in this function is due to variable expansion: if there
 * are <expand:> variables, the parse-tree needs to be expanded with current
 * values by calling user-supplied callbacks and building a 'shadow' parse-tree
 * which is purged after use. 
 * Use this function if you already have a string but you want it syntax-checked 
 * and parsed.
 *
 * @param  [in]  h         Cligen handle
 * @param  [in]  string    Input string to match
 * @param  [in]  pt        Parse-tree
 * @param  [out] co_orig   Object that matches (if retval == 1).
 * @param  [out] vr        Variable vector (if retval == 1).
 *
 * @retval  -2             Eof
 * @retval -1              Error
 * @retval  0              No match
 * @retval  1              Exactly one match
 * @retval  2+             Multiple matches
 */
int 
cliread_parse (cligen_handle h, 
	       char         *string,
	       parse_tree   *pt,     /* Orig */
	       cg_obj      **co_orig,
	       cvec         *vr)
{
    int           retval = -1;
    cg_obj       *match_obj;
    parse_tree    ptn={0,};     /* Expanded */
    cvec         *cvec = NULL;

    cli_trim (&string, cligen_comment(h));
    if (pt_expand_1(h, NULL, pt) < 0) /* sub-tree expansion, ie @ */
	goto done; 
    if ((cvec = cvec_start(string)) == NULL)
	goto done;
    if (pt_expand_2(h, pt, cvec, &ptn, 0) < 0)      /* expansion */
	goto done;

    if ((retval = match_pattern_exact(h, string, ptn, 1, cvec, &match_obj)) < 0)
	goto done;

    /* Map from ghost object match_obj to real object */
    if (retval == CG_MATCH){
	if (match_obj && match_obj->co_ref)
	    *co_orig = match_obj->co_ref;
	else
	    *co_orig = match_obj;
	if (cvec_match(*co_orig, string, vr) < 0){
	    retval = CG_ERROR;
	    goto done;
	}
    }
  done:
    if (cvec)
	cvec_free(cvec);
    if (cligen_parsetree_free(ptn, 0) < 0)
	return -1;
    if (pt_expand_cleanup_2(*pt) < 0)
	return CG_ERROR;
    return retval;
}

/*
 * cliread
 * Read line interactively from terminal using getline (completion, etc), given
 * a parse-tree.
 *
 * INPUT:
 *   prompt    Prompt string to show at beginning of line
 *   pt        Parse-tree 
 * OUTPUT:
 *   line      Pointer to new string input from terminal
 * RETURNS:
 *  Pointer to command buffer or NULL if EOF.
 */
char *
cliread(cligen_handle h)
{
    char   *string;
    
    do {
	string = gl_getline(h);
	cli_trim(&string, cligen_comment(h));
    } while (strlen(string) == 0 && !gl_eof());
    if (gl_eof())
	return NULL; 
    gl_histadd(string);
    
    return string;
}
		
#ifdef notused
/*
 * cliread_getline
 * Read line interactively from terminal using getline (completion, etc), given
 * a parse-tree.
 * Return the matching object and a CLIgen variable record.
 * Use this function if you want an interactive CLI
 *
 * INPUT:
 *   prompt    Prompt string to show at beginning of line
 *   pt        Parse-tree 
 * OUTPUT:
 *   line      Pointer to new string input from terminal
 *   match_obj Matching object  (if retval = 1)
 *   vr        Variable vector  (if retval = 1)
 * RETURNS:
 *  -2      eof
 *  -1      Error
 *   0      No match
 *   1      Exactly one match
 *   2+     Multiple matches
 */
int 
cliread_getline(cligen_handle h, 
		char **line, 
		cg_obj **match_obj,
		cvec *vr)
{
    char	*string;

    string = cliread(h);
    if (string == NULL)  /* EOF */
	return CG_EOF; 

    *line = string;
    return cliread_parse(h, string, match_obj, vr);
}
#endif /* notused */

/*! Read line from terminal, parse the string, and invoke callbacks.
 *
 * 
 * Return both results from parsing (function return), and eventual result
 * from callback (if function return =1).
 * Use this function if you want the whole enchilada without special operation
 *
 * @param [in]  h       CLIgen handle
  * @param [out] line   Pointer to new string input from terminal
 * @param [out] cb_retval  Retval of callback (only if functions return value is 1)
 *
 * @retval  -2      eof  CG_EOF
 * @retval  -1      Error
 * @retval   0      No match
 * @retval   1      Exactly one match
 * @retval   2+     Multiple matches
 */
int
cliread_eval(cligen_handle     h,
	     char            **line,
	     int              *cb_retval)
{
    cg_obj     *match;    /* matching syntax node */
    int         retval = CG_ERROR;
    cvec       *vr;
    char       *treename;
    parse_tree *pt = NULL;     /* Orig */

    if (h == NULL){
	fprintf(stderr, "Illegal cligen handle\n");
	goto done;
    }
    if ((*line = cliread(h)) == NULL){ /* EOF */
	retval = CG_EOF; 
	goto done;
    }
    treename = cligen_tree_active(h);
    if ((pt = cligen_tree(h, treename)) == NULL){
	fprintf(stderr, "No such parse-tree registered: %s\n", treename);
	goto done;;
    }
    if ((vr = cvec_new(0)) == NULL){
	fprintf(stderr, "%s: cvec_new: %s\n", __FUNCTION__, strerror(errno));
	goto done;;
    }
    if ((retval = cliread_parse(h, *line, pt, &match, vr)) == CG_MATCH)
	*cb_retval = cligen_eval(h, match, vr);
    cvec_free(vr);	
  done:
    /* XXX: Get parse-tree */
    if (pt && pt_expand_cleanup_1(pt) < 0) 
	retval = -1;
    return retval;
}
	       
/*! Evaluate a matched CV and a cv variable list
 *
 * @param  h      Application-specific pointer to a struct
 * @param  co     Object that has been matched. This is the object furthest down 
 *                in the syntax tree. By backtracking to the top the complete path
 *                can be retreived.
 * @param  vr     A vector of cligen variables present in the string.
 *
 * RETURNS:
 * @retval   int If there is a callback, the return value of the callback is returned,
 * @retval   0   otherwise
 *
 * This is the only place where cligen callbacks are invoked
 */
int
cligen_eval(cligen_handle h, cg_obj *co, cvec *vr)
{
    struct cg_callback *cc;
    int                 retval = 0;
    cg_var             *cv;

#if 0 /* debug */
    printf("argc: %d\n", argc);
    for (i=0;i<argc;i++)
      printf("%d: %d\n", i, (int)&argv[i]); 
#endif
    if (h)
	cligen_co_match_set(h, co);
    for (cc = co->co_callbacks; cc; cc=cc->cc_next){
	if (cc->cc_fn){
	    cv = cc->cc_arg ? cv_dup(cc->cc_arg) : NULL;
	    cligen_fn_str_set(h, cc->cc_fn_str);
	    if ((retval = (*cc->cc_fn)(
		     cligen_userhandle(h)?cligen_userhandle(h):h, 
		     vr, 
		     cv)) < 0){
		if (cv != NULL){
		    cv_free(cv);
		}
		cligen_fn_str_set(h, NULL);
		break;
	    }
	    if (cv != NULL)
		cv_free(cv);
	    cligen_fn_str_set(h, NULL);
	}
    }
    return retval;
}


void 
cligen_echo_off()
{
    struct termios settings;

    tcgetattr(STDIN_FILENO, &settings);
    settings.c_lflag &= (~ECHO);
    tcsetattr(STDIN_FILENO,TCSANOW,&settings);
    return;
}

void 
cligen_echo_on()
{
    struct termios settings;

    tcgetattr(0, &settings);
    settings.c_lflag |= ECHO;
    tcsetattr(0,TCSANOW,&settings);
    return;
}

