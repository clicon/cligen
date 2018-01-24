/*
  CLI generator readline. Do input processing and matching.

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

#include "cligen_buf.h"
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
 * Local prototypes
 */
static int show_help_columns(cligen_handle h, FILE *fout, char *s, parse_tree pt, cvec *cvec);
static int show_help_line(cligen_handle h, FILE *fout, char *s, parse_tree pt, cvec *);
static int complete(cligen_handle h, int *lenp, parse_tree pt, cvec *cvec);

/*! Callback from getline: '?' has been typed on command line
 * Just show help by calling long help show function. 
 * @param[in]  string Input string to match
 * @param[in]  cursor_loc - Location of cursor
 * @retval  0 OK: required by getline
 * @retval -1 Error
 * @note Flaw related to getline: Errors from sub-functions are ignored
 * @see cli_tab_hook
 */
static int
cli_qmark_hook(cligen_handle h,
	       char         *string)
{
    int           retval = -1;
    parse_tree   *pt=NULL;     /* Orig parse-tree */
    parse_tree    ptn={0,};    /* Expanded */
    cvec         *cvec = NULL;

    fputs ("\n", stdout);
    if ((pt = cligen_tree_active_get(h)) == NULL)
	goto ok;
    if (pt_expand_treeref(h, NULL, pt) < 0) /* sub-tree expansion */
	goto done; 
    if ((cvec = cvec_start(string)) == NULL)
	goto done;
    if (pt_expand_2(h, pt, cvec, &ptn, 1) < 0)      /* expansion */
	return -1;
    if (show_help_line(h, stdout, string, ptn, cvec) <0)
	goto done;
 ok:
    retval = 0;
  done:
    if (cvec)
	cvec_free(cvec);
    if (cligen_parsetree_free(ptn, 0) < 0)
	return -1;
    if (pt && pt_expand_cleanup_2(*pt) < 0) 
	return -1;
    if (pt && pt_expand_treeref_cleanup(pt) < 0) 
	return -1;
    return retval;
}

/*! Callback from getline: TAB has been typed on keyboard
 * First try to complete the string if the possibilities
 * allow that (at least one unique common character). 
 * If no completion was made, then show the command alternatives.
 * @param[in]     h            CLIgen handle
 * @param[in,out] cursorp      Pointer to location of cursor on entry and exit
 * @retval  -1    Error
 * @retval  -2    (value != -1 required by getline)
 * @note Flaw related to getline: Errors from sub-functions are ignored
 * @see cli_qmark_hook
 */
static int 
cli_tab_hook(cligen_handle h,
	     int          *cursorp)
{
    int           retval = -1;
    int	          old_cursor;
    parse_tree   *pt;     /* Orig */
    parse_tree    ptn={0,};     /* Expanded */
    cvec         *cvec = NULL;

    old_cursor = *cursorp;  /* Save old location of cursor */
    if ((pt = cligen_tree_active_get(h)) == NULL)
	goto ok;
    if (pt_expand_treeref(h, NULL, pt) < 0) /* sub-tree expansion */
	goto done;
    if ((cvec = cvec_start(cligen_buf(h))) == NULL)
	goto done; 
    if (pt_expand_2(h, pt, cvec, &ptn, 1) < 0)      /* expansion */
	goto done;
    /* Note, can change cligen buf pointer (append and increase) */
    if (complete(h, cursorp, ptn, cvec) < 0)
	goto done;
    else {
	if (old_cursor == *cursorp) { 	/* Cursor hasnt changed */
	    fputs ("\n", stdout);
	    if (cligen_tabmode(h) == 1){
		if (show_help_line(h, stdout, cligen_buf(h), ptn, cvec) < 0)
		    goto done;
	    }
	    else
		if (show_help_columns(h, stdout, cligen_buf(h), ptn, cvec) < 0)
		    goto done;
	}
    }
 ok:
    retval = 0; 
 done:
    if (cvec)
	cvec_free(cvec);
    if (cligen_parsetree_free(ptn, 0) < 0)
	return -1;
    if (pt && pt_expand_cleanup_2(*pt) < 0)
	return -1;
    if (pt && pt_expand_treeref_cleanup(pt) < 0)
	return -1;
    return retval;	
}

/*! Initialize this module
 */
void
cliread_init(cligen_handle h)
{
    gl_qmark_hook = cli_qmark_hook;
    gl_tab_hook = cli_tab_hook; /* XXX globals */
}

/*! Print columns
 * @param[in]  cnr  Number of columns.
 * @param[in]  cw   Width of column
 */
static int
column_print(FILE            *fout, 
	     int              cnr, 
	     int              cw,
	     struct cmd_help *chvec,
	     int              len,
	     int              level)
{ 
    int              retval = -1;
    int              li; /* line number */
    int              ci; /* column number */
    int              linenr;
    struct cmd_help *ch;

    linenr = (len-1)/cnr + 1;
    for (ci=0, li = 0; li < linenr; li++) {
	while ((ci < cnr) && (li*cnr+ci < len)) {
	    ch = &chvec[li*cnr+ci];
	    fprintf (fout, " %*s", 
		     -(cw-1), 
		     ch->ch_cmd);
	    ci++;
	}
	ci = 0;
	fprintf(fout, "\n");  
    }  
    fflush(fout);
    retval = 0;
    // done:
    return retval;
}

/*! Show briefly the commands available (show no help)
 * Typically called when TAB is pressed and there are multiple options.
 * @param[in]  fout    This is where the output (help text) is shown.
 * @param[in]  string  Input string to match
 * @param[in]  pt      Vector of commands (array of cligen object pointers (cg_obj)
 * @param[in]  pt_max  Length of the pt array
 * @param[out] cvec    Cligen variable vector containing vars/values pair for completion
 * @retval     0       OK
 * @retval     -1      Error
 */
static int
show_help_columns(cligen_handle h, 
		  FILE         *fout, 
		  char         *string, 
		  parse_tree    pt, 
		  cvec         *cvec)
{
    int              retval = -1;
    int              nr = 0;
    int              level;
    pt_vec           pt1;
    int              matchlen = 0;
    int             *matchvec = NULL;
    int              vi;
    int              i;
    int              nrcmd = 0;
    struct cmd_help *chvec = NULL;
    struct cmd_help *ch;
    cg_obj          *co;
    cbuf            *cb = NULL;
    char            *cmd;
    char            *prev = NULL;
    int              maxlen = 0;
    int              column_width;
    int              column_nr;
    int              rest;

    if ((cb = cbuf_new()) == NULL){
	fprintf(stderr, "cbuf_new: %s\n", strerror(errno));
	return -1;
    }
    if (string != NULL){
	if ((nr = match_pattern(h, string, pt, 0, 1,
				&pt1, &matchvec, &matchlen, cvec, NULL)) < 0)
	    goto done;
    }
    if ((level = command_levels(string)) < 0)
	goto done;
    if (nr>0){ /* min, max only defined if nr > 0 */
	/* Go through match vector and collect commands and helps */
	if ((chvec = calloc(matchlen, sizeof(struct cmd_help))) ==NULL){
	    fprintf(stderr, "%s calloc: %s\n", __FUNCTION__, strerror(errno));
	    goto done;
	}
	nrcmd = 0;
	for (i = 0; i<matchlen; i++){ // nr-1?
	    vi=matchvec[i];
	    if ((co = pt1[vi]) == NULL)
		continue;
	    if (co->co_command == NULL)
		continue;		
	    cmd = NULL;
	    switch (co->co_type){
	    case CO_VARIABLE:
		cbuf_reset(cb);
		cov2cbuf(cb, co, 1);
		cmd = cbuf_get(cb);
		break;
	    case CO_COMMAND:
		cmd = co->co_command;
		break;
	    case CO_REFERENCE:
	    default:
		continue;
	    }
	    if (cmd == NULL || strlen(cmd)==0)
		continue;
	    if (prev && strcmp(cmd, prev)==0)
		continue;
	    ch = &chvec[nrcmd++];
	    if ((ch->ch_cmd = strdup(cmd)) == NULL){
		fprintf(stderr, "%s strdup: %s\n", __FUNCTION__, strerror(errno));
		goto done;
	    }
	    prev = ch->ch_cmd;
	    maxlen = strlen(cmd)>maxlen?strlen(cmd):maxlen;
	}
	maxlen++;
	column_width = maxlen<COLUMN_MIN_WIDTH?COLUMN_MIN_WIDTH:maxlen;
	column_nr = cligen_terminal_length(h)/column_width;
	if (column_nr < 1)
	    column_nr = 1;
	rest = cligen_terminal_length(h)%column_width;
	column_width += rest/column_nr;
	if (column_print(fout, 
			 column_nr,
			 column_width,
			 chvec, 
			 nrcmd,
			 level) < 0)
	    goto done;
    } /* nr>0 */

    retval = 0;
  done:
    if (chvec){
	for (i=0; i<nrcmd; i++){
	    if (chvec[i].ch_cmd)
		free(chvec[i].ch_cmd);
	}
	free(chvec);
    }
    if (cb)
	cbuf_free(cb);
    if (matchvec)
	free(matchvec);
    return retval;
}

/*! Show one row per command with help text for each command
 * Typically called when a question mark is pressed
 * @param[in]   h       cligen handle
 * @param[in]   fout    This is where the output (help text) is shown.
 * @param[in]   string  Input string to match
 * @param[in]   pt      Parse tree
 * @param[out]  cvec    Cligen variable vector containing vars/values pair for completion
 * @retval      0       OK
 * @retval      -1      Error
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
show_help_line(cligen_handle h, 
	       FILE         *fout, 
	       char         *string, 
	       parse_tree    pt, 
	       cvec         *cvec)
{
    int              retval = -1;
    int              nr = 0;
    int              level;
    pt_vec           pt1;
    char	    *tmp;
    char	    *tmpp;
    int              matchlen = 0;
    int             *matchvec = NULL;
    int              res;

    /* Build match vector, but why would string ever be NULL? */
    if (string != NULL){
	if ((nr = match_pattern(h, string, pt, 0, 1, &pt1, &matchvec, &matchlen, cvec, NULL)) < 0)
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
	    fprintf(stderr, "%s strdup: %s\n", __FUNCTION__, strerror(errno));
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
	    fflush(fout);
	}
	free (tmp);
#if 0
	/* The following is a kludge to correct a memory error triggered by:
	   a <rest>;
	   a input a a ?
	   Just repeat the same command as above,...
	   1801: This causes a segv for a b c ?
	*/
	if (matchvec){
	    free(matchvec);
	    matchvec = NULL;
	}
#endif
	if ((nr = match_pattern(h, string, pt, 0, 1, &pt1, &matchvec, &matchlen, cvec, NULL)) < 0)

	    goto done;
    }
    if (!nr){
	retval = 0;
	goto done;
    }
    if (print_help_lines(fout, pt1, matchvec, matchlen) < 0)
	goto done;
    retval = 0;
  done:
    if (matchvec)
	free(matchvec);
    return retval;
}

/*! Try to complete a command as much as possible.
 * @param[in]  h       CLIgen handle
 * @param[in]  string  Input string to match
 * @param[in]  cursorp Pointer to the current cursor in string.
 * @param[in]  pt      Vector of commands (array of cligen object pointers)
 * @param[out] cvec    cligen variable vector containing vars/values pair for 
 *                     completion
 * @retval    -1       Error
 * @retval     0       Success
 */
static int 
complete(cligen_handle h, 
	 int          *cursorp, 
	 parse_tree    pt, 
	 cvec         *cvec)
{
    int     retval = -1;
    char   *string;
    char   *s = NULL;
    size_t  slen;
    int     cursor = *cursorp;
    int     i, n;
    int     extra;

    string = cligen_buf(h);
    if (string == NULL){
	fprintf(stderr, "%s Input string NULL\n", __FUNCTION__);
	goto done;
    }
    slen = cligen_buf_size(h);
    if ((s = malloc(slen)) == NULL){ /* s is a temporary copy */
	fprintf(stderr, "%s malloc: %s\n", __FUNCTION__, strerror(errno));
	goto done;
    }
    strncpy(s, string, slen);
    s[cursor] = '\0';
    if (match_complete(h, pt, &s, &slen, cvec) < 0)
	goto done;
    extra = strlen(s) - cursor;      /* Extra characters added? */
    if (extra){
	while (strlen(s) >= cligen_buf_size(h)){
	    cligen_buf_increase(h);
	    string = cligen_buf(h);
	}
	n = strlen(string) - cursor; /* Nr of chars right of cursor to copy */
	for (i=cursor+n; i>=cursor; i--)             /* Copy right of cursor */
	    string[i + extra] = string[i];
	strncpy(string + cursor, s + cursor, extra); /* Copy the new stuff */
	*cursorp += extra;                           /* Increase cursor */
    }
    retval = 0;
 done:
    if (s)
	free(s);
    return retval;
}

/*! Trim command line. Remove any leading, trailing and multiple whitespace
 * comment is a character (eg '#')
 * @param[out]  line
 * @param[in]   comment
 */
void
cli_trim (char **line, 
	  char   comment)
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
 * @param[in]  h         Cligen handle
 * @param[in]  string    Input string to match
 * @param[in]  pt        Parse-tree
 * @param[out] co_orig   Object that matches (if retval == 1).
 * @param[out] vr        Variable vector (if retval == 1).
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
	       cvec         *cvv)
{
    int           retval = -1;
    cg_obj       *match_obj;
    parse_tree    ptn={0,};     /* Expanded */
    cvec         *cvec = NULL;

    if (cligen_logsyntax(h) > 0){
	fprintf(stderr, "%s:\n", __FUNCTION__);
	cligen_print(stderr, *pt, 0);
    }
    cli_trim (&string, cligen_comment(h));
    if (pt_expand_treeref(h, NULL, pt) < 0) /* sub-tree expansion, ie @ */
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
	if (cvec_match(*co_orig, string, cvv) < 0){
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

/*! Read line interactively from terminal using getline (completion, etc)
 *
 * @param[in] h       CLIgen handle
 * @retval    string  Pointer to command buffer.
 * @retval    NULL    EOF or error
 */
char *
cliread(cligen_handle h)
{
    char *string;
    
    do {
	string = NULL;
	if (gl_getline(h, &string) < 0)
	    goto done;
	cli_trim(&string, cligen_comment(h));
    } while (strlen(string) == 0 && !gl_eof());
    if (gl_eof()){
	string = NULL;
	goto done;
    }
    gl_histadd(string);
 done:    
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
 * @param[in]  h         CLIgen handle
 * @param[out] line      Pointer to new string input from terminal
 * @param[out] match_obj Matching object  (if retval = 1)
 * @param[out] cvv       Variable vector  (if retval = 1)
 * @retval    -2         EOF
 * @retval    -1         Error
 * @retval     0         No match
 * @retval     1         Exactly one match
 * @retval     2+        Multiple matches
 */
int 
cliread_getline(cligen_handle h, 
		char        **line, 
		cg_obj      **match_obj,
		cvec         *cvv)
{
    char	*string;

    string = cliread(h);
    if (string == NULL)  /* EOF */
	return CG_EOF; 

    *line = string;
    return cliread_parse(h, string, match_obj, cvv);
}
#endif /* notused */

/*! Read line from terminal, parse the string, and invoke callbacks.
 *
 * 
 * Return both results from parsing (function return), and eventual result
 * from callback (if function return =1).
 * Use this function if you want the whole enchilada without special operation
 *
 * @param[in]  h       CLIgen handle
 * @param[out] line     Pointer to new string input from terminal
 * @param[out] cb_retval  Retval of callback (only if functions return value is 1)
 *
 * @retval  -2      EOF
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
    cvec       *cvv;
    parse_tree *pt = NULL;     /* Orig */

    if (h == NULL){
	fprintf(stderr, "Illegal cligen handle\n");
	goto done;
    }
    if ((*line = cliread(h)) == NULL){ /* EOF */
	retval = CG_EOF; 
	goto done;
    }
    if ((pt = cligen_tree_active_get(h)) == NULL){
	fprintf(stderr, "No active parse-tree found\n");
	goto done;;
    }
    if ((cvv = cvec_new(0)) == NULL){
	fprintf(stderr, "%s: cvec_new: %s\n", __FUNCTION__, strerror(errno));
	goto done;;
    }
    if ((retval = cliread_parse(h, *line, pt, &match, cvv)) == CG_MATCH)
	*cb_retval = cligen_eval(h, match, cvv);
    cvec_free(cvv);	
  done:
    /* XXX: Get parse-tree */
    if (pt && pt_expand_treeref_cleanup(pt) < 0) 
	retval = -1;
    return retval;
}
	       
/*! Evaluate a matched CV and a cv variable list
 *
 * @param[in]  h    Application-specific pointer to a struct
 * @param[in]  co   Object that has been matched. This is the object furthest down 
 *                  in the syntax tree. By backtracking to the top the complete path
 *                  can be retreived.
 * @param[in]  cvv  A vector of cligen variables present in the string.
 *
 * @retval   int If there is a callback, the return value of the callback is returned,
 * @retval   0   otherwise
 *
 * This is the only place where cligen callbacks are invoked
 */
int
cligen_eval(cligen_handle h, 
	    cg_obj       *co, 
	    cvec         *cvv)
{
    struct cg_callback *cc;
    int                 retval = 0;
    cvec               *argv;

    if (h)
	cligen_co_match_set(h, co);
    for (cc = co->co_callbacks; cc; cc=cc->cc_next){
	/* Vector cvec argument to callback */
    	if (cc->cc_fn_vec){
	    argv = cc->cc_cvec ? cvec_dup(cc->cc_cvec) : NULL;
	    cligen_fn_str_set(h, cc->cc_fn_str);
	    if ((retval = (*cc->cc_fn_vec)(
					cligen_userhandle(h)?cligen_userhandle(h):h, 
					cvv, 
					argv)) < 0){
		if (argv != NULL)
		    cvec_free(argv);
		cligen_fn_str_set(h, NULL);
		break;
	    }
	    if (argv != NULL)
		cvec_free(argv);
	    cligen_fn_str_set(h, NULL);
	}
	/* Single cv argument to callback (old) */
    	if (cc->cc_fn){
	    cg_var             *cv;
	    argv = cc->cc_cvec ? cvec_dup(cc->cc_cvec) : NULL;
	    cligen_fn_str_set(h, cc->cc_fn_str);
	    cv = argv?cvec_i(argv,0):NULL;
	    if ((retval = (*cc->cc_fn)(
				       cligen_userhandle(h)?cligen_userhandle(h):h, 
				       cvv, 
				       cv)) < 0){
		if (argv != NULL)
		    cvec_free(argv);
		cligen_fn_str_set(h, NULL);
		break;
	    }
	    if (argv != NULL)
		cvec_free(argv);
	    cligen_fn_str_set(h, NULL);
	}
    }
    return retval;
}

/*! Turn echo off */
void 
cligen_echo_off(void)
{
    struct termios settings;

    tcgetattr(STDIN_FILENO, &settings);
    settings.c_lflag &= (~ECHO);
    tcsetattr(STDIN_FILENO,TCSANOW,&settings);
    return;
}

/*! Turn echo on */
void 
cligen_echo_on(void)
{
    struct termios settings;

    tcgetattr(0, &settings);
    settings.c_lflag |= ECHO;
    tcsetattr(0,TCSANOW,&settings);
    return;
}

/*! Set relaxed handling of cligen variable matching 
 * More specifically, if several cligen object variables match with same preference,
 * select the first, do not match all.
 * Example:
 * key (<a:string length[4]> | <a:string length[40]>);
 * @param[in] flag   Set to 1 to enable relaxed handling, 0 if not
 * @retval    flag   Previous value
 */
int 
cligen_match_cgvar_same(int flag)
{
    int oldval = _match_cgvar_same;

    _match_cgvar_same = flag;
    return oldval;
}

