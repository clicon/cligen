/*
  CLI generator readline. Do input processing and matching.

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

#ifndef isblank
#define isblank(c) (c==' ')
#endif /* isblank */

#include "cligen_buf.h"
#include "cligen_cv.h"
#include "cligen_cvec.h"
#include "cligen_parsetree.h"
#include "cligen_pt_head.h"
#include "cligen_callback.h"
#include "cligen_object.h"
#include "cligen_handle.h"
#include "cligen_print.h"
#include "cligen_result.h"
#include "cligen_read.h"
#include "cligen_match.h"
#include "cligen_io.h"
#include "cligen_expand.h"
#include "cligen_history_internal.h"
#include "cligen_getline.h"

/*
 * Local prototypes
 */
static int show_help_columns(cligen_handle h, FILE *fout, char *s, parse_tree *pt, cvec *cvv);
static int show_help_line(cligen_handle h, FILE *fout, char *s, parse_tree *pt, cvec *);
static int cli_complete(cligen_handle h, int *lenp, parse_tree *pt, cvec *cvv);

/*! Show help strings 
 *
 * @param[in]  h       CLIgen handle Input string to match
 * @param[in]  string  Input string
 * @param[in]  column  If 0: short/ios (column) mode, 1: long/junos (row) mode (cf CLIGEN_TABMODE_COLUMNS)
 * @retval     0       OK
 * @retval    -1       Error
 */
static int
cli_show_help_commands(cligen_handle h,
		       char         *string)
{
    int           retval = -1;
    parse_tree   *pt=NULL;     /* Orig parse-tree */
    parse_tree   *ptn = NULL;    /* Expanded */
    cvec         *cvv = NULL;

    fputs("\n", stdout);
    if ((ptn = pt_new()) == NULL)
	goto done;
    if ((pt = cligen_pt_active_get(h)) == NULL)
	goto ok;
    if ((cvv = cvec_start(string)) == NULL)
	goto done;
    if (pt_expand(h, NULL, pt, cvv,
		  1, /* Include hidden commands */
		  0, /* VARS are not expanded, eg ? <tab> */
		  ptn) < 0)      /* expansion */
	goto done;
    if (show_help_line(h, stdout, string, ptn, cvv) < 0)
	goto done;
 ok:
    retval = 0;
 done:
    if (cvv)
	cvec_free(cvv);
    if (ptn && pt_free(ptn, 0) < 0)
	return -1;
    if (pt && pt_expand_cleanup(h, pt) < 0)
	return -1;
    return retval;
}

/*! Callback from getline: '?' has been typed on command line
 * Just show help by calling long help show function. 
 * @param[in]  h            CLIgen handle
 * @param[in]  string Input string to match
 * @retval  0 OK: required by getline
 * @retval -1 Error
 * @note Flaw related to getline: Errors from sub-functions are ignored
 * @see cli_tab_hook
 */
static int
cli_qmark_hook(cligen_handle h,
	       char         *string)
{
    return cli_show_help_commands(h, string);
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
    int	          prev_cursor;
    parse_tree   *pt = NULL;     /* Orig */
    parse_tree   *ptn = NULL;    /* Expanded */
    cvec         *cvv = NULL;

    if ((ptn = pt_new()) == NULL)
	goto done;
    if ((pt = cligen_pt_active_get(h)) == NULL)
	goto ok;
    if ((cvv = cvec_start(cligen_buf(h))) == NULL)
	goto done; 
    if (pt_expand(h, NULL, pt, cvv,
		  1,   /* Include hidden commands */
		  0,   /* VARS are not expanded, eg ? <tab> */
		  ptn) < 0)      /* expansion */
	goto done;
    /* Note, can change cligen buf pointer (append and increase) */
    do {
	prev_cursor = *cursorp;
	if (cli_complete(h, cursorp, ptn, cvv) < 0) /* XXX expand-cleanup must be done here before show commands */
	    goto done;
    } while (cligen_tabmode(h)&CLIGEN_TABMODE_STEPS && prev_cursor != *cursorp);
    if (cvv){
	cvec_free(cvv);
	if ((cvv = cvec_start(cligen_buf(h))) == NULL)
	    goto done; 
    }
    fputs("\n", stdout);
    if (cligen_tabmode(h) & CLIGEN_TABMODE_COLUMNS){
	if (show_help_line(h, stdout, cligen_buf(h), ptn, cvv) <0)
	goto done;
    }
    else if (show_help_columns(h, stdout, cligen_buf(h), ptn, cvv) < 0)
	    goto done;
 ok:
    retval = 0; 
 done:
    if (cvv)
	cvec_free(cvv);
    if (ptn && pt_free(ptn, 0) < 0)
	return -1;
    if (pt && pt_expand_cleanup(h, pt) < 0)
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
	     struct cligen_help *chvec,
	     int              len,
	     int              level)
{ 
    int              retval = -1;
    int              li; /* line number */
    int              ci; /* column number */
    int              linenr;
    struct cligen_help *ch;

    linenr = (len-1)/cnr + 1;
    for (ci=0, li = 0; li < linenr; li++) {
	while ((ci < cnr) && (li*cnr+ci < len)) {
	    ch = &chvec[li*cnr+ci];
	    fprintf(fout, " %*s", 
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
 * @param[out] cvv     Cligen variable vector containing vars/values pair for completion
 * @retval     0       OK
 * @retval     -1      Error
 * @see print_help_lines
 */
static int
show_help_columns(cligen_handle h, 
		  FILE         *fout, 
		  char         *string, 
		  parse_tree   *pt, 
		  cvec         *cvv)
{
    int              retval = -1;
    int              level;
    int              i;
    int              nrcmd = 0;
    struct cligen_help *chvec = NULL;
    struct cligen_help *ch;
    cg_obj          *co;
    cbuf            *cb = NULL;
    char            *cmd;
    int              maxlen = 0;
    int              column_width;
    int              column_nr;
    int              rest;
    cvec            *cvt = NULL;      /* Tokenized string: vector of tokens */
    cvec            *cvr = NULL;      /* Rest variant,  eg remaining string in each step */
    match_result    *mr = NULL;

    if (string == NULL){
	errno = EINVAL;
	goto done;
    }
    if ((cb = cbuf_new()) == NULL){
	fprintf(stderr, "cbuf_new: %s\n", strerror(errno));
	return -1;
    }
    /* Tokenize the string and transform it into two CLIgen vectors: tokens and rests */
    if (cligen_str2cvv(string, &cvt, &cvr) < 0)
	goto done;
    if (match_pattern(h, cvt, cvr,
		      pt,
		      0, /* best: Return all options, not only best, exclude hidden */
		      cvv, 
		      NULL,
		      &mr) < 0)
	goto done;
    if ((level = cligen_cvv_levels(cvt)) < 0)
	goto done;
    if (mr_pt_len_get(mr) > 0){ /* min, max only defined if matchlen > 0 */
	/* Go through match vector and collect commands and helps */
	if ((chvec = calloc(mr_pt_len_get(mr), sizeof(struct cligen_help))) ==NULL){
	    fprintf(stderr, "%s calloc: %s\n", __FUNCTION__, strerror(errno));
	    goto done;
	}
	nrcmd = 0;
	for (i = 0; i<mr_pt_len_get(mr); i++){ // nr-1?
	    if ((co = mr_pt_i_get(mr, i)) == NULL)
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
	    ch = &chvec[nrcmd];
	    if ((ch->ch_cmd = strdup(cmd)) == NULL){
		fprintf(stderr, "%s strdup: %s\n", __FUNCTION__, strerror(errno));
		goto done;
	    }
	    if (co->co_helpstring && cligen_txt2cvv(co->co_helpstring, &ch->ch_helpvec) < 0)
		goto done;
	    if (nrcmd && cligen_help_eq(&chvec[nrcmd-1], ch, 0) == 1){
		cligen_help_clear(ch);
		continue;
	    }
	    nrcmd++;
	    maxlen = strlen(cmd)>maxlen?strlen(cmd):maxlen;
	}
	maxlen++;
	column_width = maxlen<COLUMN_MIN_WIDTH?COLUMN_MIN_WIDTH:maxlen;
	column_nr = cligen_terminal_width(h)/column_width;
	if (column_nr < 1)
	    column_nr = 1;
	rest = cligen_terminal_width(h)%column_width;
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
	for (i=0; i<nrcmd; i++)
	    cligen_help_clear(&chvec[i]);
	free(chvec);
    }
    if (cvt)
	cvec_free(cvt);
    if (cvr)
	cvec_free(cvr);
    if (cb)
	cbuf_free(cb);
    if (mr){
	mr_free(mr);
    }
    return retval;
}

/*! Show one row per command with help text for each command
 * Typically called when a question mark is pressed
 * @param[in]   h       cligen handle
 * @param[in]   fout    This is where the output (help text) is shown.
 * @param[in]   string  Input string to match
 * @param[in]   pt      Parse tree
 * @param[out]  cvv     Cligen variable vector containing vars/values pair for completion
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
	       parse_tree   *pt, 
	       cvec         *cvv)
{
    int           retval = -1;
    int           level;
    cvec         *cvt = NULL;      /* Tokenized string: vector of tokens */
    cvec         *cvr = NULL;      /* Rest variant,  eg remaining string in each step */
    cg_var       *cvlastt;         /* Last element in cvt */
    cg_var       *cvlastr;         /* Last element in cvr */
    cligen_result result;
    match_result *mr = NULL;

    if (string == NULL){
	errno = EINVAL;
	goto done;
    }
    /* Tokenize the string and transform it into two CLIgen vectors: tokens and rests */
    if (cligen_str2cvv(string, &cvt, &cvr) < 0) /* XXX cvr leaks memory */
	goto done;
    if (match_pattern(h,
		      cvt, cvr, /* token string */
		      pt,       /* command vector */
		      0,        /* best: Return all options, not only best, exclude hidden */
		      cvv, 
		      NULL,
		      &mr) < 0)
	goto done;
    if ((level =  cligen_cvv_levels(cvt)) < 0)
	goto done;

    /* If last char is blank, look for next level in parse-tree 
     * eg, syntax is x (y|z) and we have typed 'x ' then show
     * help for y and z, not x.
     */
    /* See if string is not empty and last element is empty/blank 
     * This means we need to peek in next level and if that provides a unique solution,
     * then add a <cr>
     */
    cvlastt = cvec_i(cvt, cvec_len(cvt)-1);
    if (cvec_len(cvt) > 2 && strcmp(cv_string_get(cvlastt), "")==0){
	/* if it is ok to <cr> here (at end of one mode) 
	   Example: x [y|z] and we have typed 'x ', then show
	   help for y and z and a 'cr' for 'x'.
	*/

	/* Remove the last elements. Awkward: first free the cv then truncate the cvec */
	if (cvlastt)
	    cv_reset(cvlastt);
	cvec_del_i(cvt, cvec_len(cvt)-1); /* We really just want to truncate len-1 */

	if ((cvlastr = cvec_i(cvr, cvec_len(cvr)-1)) != NULL)
	    cv_reset(cvlastr);
	cvec_del_i(cvr, cvec_len(cvr)-1);

	if (match_pattern_exact(h, cvt, cvr, pt,
				cvv,
				NULL, 
				NULL,
				&result,
				NULL) < 0)
	    goto done;

	if (result == CG_MATCH){
	    fprintf(fout, "  <cr>\n");
	    fflush(fout);
	}
    }
    if (mr_pt_len_get(mr) == 0){
	retval = 0;
	goto done;
    }
    /* ptmatch points to expanded nodes from first match_pattern call */
    if (print_help_lines(h, fout, mr_pt_get(mr)) < 0) 
	goto done;
    retval = 0;
  done:
    if (cvt)
	cvec_free(cvt);
    if (cvr)
	cvec_free(cvr);
    if (mr){
	mr_free(mr);
    }
    return retval;
}

/*! Try to complete a command as much as possible.
 * @param[in]  h       CLIgen handle
 * @param[in]  string  Input string to match
 * @param[in]  cursorp Pointer to the current cursor in string.
 * @param[in]  pt      Vector of commands (array of cligen object pointers)
 * @param[out] cvv     cligen variable vector containing vars/values pair for completion
 * @retval    -1       Error
 * @retval     0       Success
 */
static int 
cli_complete(cligen_handle h, 
	     int          *cursorp, 
	     parse_tree   *pt, 
	     cvec         *cvv)
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
    if (match_complete(h, pt, &s, &slen, cvv) < 0)
	goto done;
    extra = strlen(s) - cursor;      /* Extra characters added? */
    if (extra){
        cligen_buf_increase(h, strlen(s));
	string = cligen_buf(h);
	n = strlen(string) - cursor; /* Nr of chars right of cursor to copy */
        cligen_buf_increase(h, strlen(string)+cursor+n);
	string = cligen_buf(h);
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

/*! Trim command line. Remove any leading, trailing and multiple whitespace, remove comments
 * 
 * comment is a character (eg '#'), using "bash" comment rule:
 #  Word beginning with # causes that word and all remaining characters on that line to be ignored
 * @param[out]  line
 * @param[in]   comment character
 */
void
cli_trim(char **line, 
	 char   comment)
{
    size_t  point;
    int	    whitespace = 0;
    char   *s = *line;
    char   *s1 = s;
    char    ch;
    size_t  len;
    
    if (!isascii(comment))
	comment = 0;

    len = strlen(s);
    for (point = 0; point <= len ; point++) {
	ch = s[point];
	if (comment && ch == comment && (whitespace || point == 0 )){
	    *s1++ = '\n';
	    *s1++ = '\0';
	    break;
	}
	else
	    if (isblank(ch)) {
		if (whitespace)
		    continue;
		else {
		    whitespace = 1;
		    *s1++ = ' ';
		}
	    } else {
		whitespace = 0;
		*s1++ = ch;
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
 * @param[out] co_orig   Object that matches (if retval == 1). Free with co_free(co, 0)
 * @param[out] cvvp      Vector of cligen variables present in the input string. (if retval == 1).
 * @param[out] callbacks Callback structure of expanded treeref
 * @param[out] result    Result, < 0: errors, >=0 number of matches
 * @param[out] reason    Error reason if result is nomatch. Need to be free:d 
 * @retval     0         OK
 * @retval    -1         Error
 *
 * cvv should be created but empty on entry
 * On exit it contains the command string as 0th element, and one entry per element
 * Example: "aa <bb:str>" and inut string "aa 22" gives:
 *   0 : "aa 22"     # initial command has no "name"
 *   1 : aa = "aa"   # string has keyword itself as value
 *   2 : bb = 22     # variable
 */
int 
cliread_parse(cligen_handle  h, 
	      char          *string,
	      parse_tree    *pt,     /* Orig */
	      cg_obj       **co_orig,
	      cvec         **cvvp,
	      cg_callback  **callbacks,
	      cligen_result *result,
	      char         **reason)
{
    int         retval = -1;
    cg_obj     *match_obj = NULL;
    parse_tree *ptn = NULL;      /* Expanded */
    cvec       *cvt = NULL;      /* Tokenized string: vector of tokens */
    cvec       *cvr = NULL;      /* Rest variant,  eg remaining string in each step */
    cg_var     *cv;
    cvec       *cvv = NULL;
    
    if (cvvp == NULL || *cvvp != NULL){
	errno = EINVAL;
	goto done;
    }
    if (cligen_logsyntax(h) > 0){
	fprintf(stderr, "%s:\n", __FUNCTION__);
	pt_print1(stderr, pt, 0);
    }
    cli_trim(&string, cligen_comment(h));
    /* Tokenize the string and transform it into two CLIgen vectors: tokens and rests */
    if (cligen_str2cvv(string, &cvt, &cvr) < 0)
	goto done;
    if ((cvv = cvec_new(0)) == NULL)
	goto done;;
    if ((cv = cvec_add(cvv, CGV_REST)) == NULL)
	goto done;
    cv_name_set(cv, "cmd"); /* the whole command string */
    /* The whole command string as user entered. */
    cv_string_set(cv, string); 
    if ((ptn = pt_new()) == NULL)
	goto done;
    if (pt_expand(h, NULL, pt, cvv,
		  0,  /* Do not include hidden commands */
		  0,  /* VARS are not expanded, eg ? <tab> */
		  ptn) < 0) /* sub-tree expansion, ie choice, expand function */
	goto done;
    if (match_pattern_exact(h, cvt, cvr,
			    ptn,
			    cvv,
			    &match_obj,
			    callbacks,
			    result, reason) < 0)
	goto done;
    /* Map from ghost object match_obj to real object */
    *co_orig = match_obj;
    *cvvp = cvv;
    cvv = NULL;
    retval = 0;
  done:
    if (cvv)
	cvec_free(cvv);
    if (cvt)
	cvec_free(cvt);
    if (cvr)
	cvec_free(cvr);
    if (ptn)
	if (pt_free(ptn, 0) < 0)
	    return -1;
    if (pt_expand_cleanup(h, pt) < 0)
    	return -1;
    return retval;
}

/*! Read line interactively from terminal using getline (completion, etc)
 *
 * @param[in]  h       CLIgen handle
 * @param[out] stringp Pointer to command buffer or NULL on EOF
 * @retval     0       OK
 * @retval    -1       Error
 */
int
cliread(cligen_handle h,
	char        **stringp)
{
    int   retval = -1;
    char *buf = NULL;
    
    if (stringp == NULL){
	errno = EINVAL;
	goto done;
    }
    *stringp = NULL;
    do {
	buf = NULL;
	if (gl_getline(h, &buf) < 0)
	    goto done;
	cli_trim(&buf, cligen_comment(h));
    } while (strlen(buf) == 0 && !gl_eof());
    if (gl_eof())
	goto eof;
    if (hist_add(h, buf) < 0)
	goto done;
    *stringp = buf;
 eof:
    retval = 0;
 done:
    return retval;
}

/*! Read line from terminal, parse the string, and invoke callbacks.
 *
 * 
 * Return both results from parsing (function return), and eventual result
 * from callback (if function return =1).
 * Use this function if you want the whole enchilada without special operation
 *
 * @param[in]  h         CLIgen handle
 * @param[out] line      Pointer to new string input from terminal
 * @param[out] cb_retval Retval of callback (only if functions return value is 1)
 * @param[out] result   Number of matches
 * @param[out] reason    Error reason if result is nomatch. Need to be free:d 
 * @retval     0         OK
 * @retval    -1         Error
 */
int
cliread_eval(cligen_handle  h,
	     char         **line,
	     int           *cb_retval,
	     cligen_result *result,
	     char         **reason)
{
    int          retval = -1;
    cg_obj      *matchobj = NULL;    /* matching syntax node */
    cvec        *cvv = NULL;
    parse_tree  *pt = NULL;     /* Orig */
    cg_callback *callbacks = NULL;

    if (h == NULL){
	fprintf(stderr, "Illegal cligen handle\n");
	goto done;
    }
    if (cliread(h, line) < 0)
	goto done;
    if (*line == NULL){ /* EOF */
	*result = CG_EOF; 
	goto ok;
    }
    if ((pt = cligen_pt_active_get(h)) == NULL){
	fprintf(stderr, "No active parse-tree found\n");
	goto done;;
    }
    if (cliread_parse(h, *line, pt, &matchobj, &cvv, &callbacks, result, reason) < 0)
	goto done;
    if (*result == CG_MATCH)
	*cb_retval = cligen_eval(h, matchobj, cvv, callbacks);
 ok:
    retval = 0;
 done:
    if (callbacks)
	co_callbacks_free(&callbacks);
    if (matchobj)
	co_free(matchobj, 0);
    if (cvv)
	cvec_free(cvv);	
    return retval;
}
	       
/*! Evaluate a matched CV and a cv variable list
 *
 * @param[in]  h           CLIgen handle
 * @param[in]  co          Matched CLIgen object.
 * @param[in]  cvv         A vector of cligen variables present in the string.
 * @param[out] callbacks0  Callback structure of expanded treeref
 * @retval     int         If there is a callback, the return value of the callback is returned,
 * @retval     0           otherwise
 *
 * This is the only place where cligen callbacks are invoked
 * @see pt_expand_fnv where expand callbacks are invoked
 */
int
cligen_eval(cligen_handle h, 
	    cg_obj       *co, 
	    cvec         *cvv,
	    cg_callback  *callbacks0)
{
    int            retval = -1;
    cg_callback   *cc;
    cvec          *argv;
    cvec          *cvv1 = NULL; /* Modified */
    cgv_fnstype_t *fn;
    cg_callback   *callbacks;
    cligen_eval_wrap_fn *wrapfn = NULL;
    void          *wraparg = NULL;
    void          *wh = NULL; /* eval wrap handle */
    
    /* Save matched object for plugin use */
    if (h)
	cligen_co_match_set(h, co);
    /* Make a copy of var argument for modifications */
    if ((cvv1 = cvec_dup(cvv)) == NULL)
	goto done;
    /* Make modifications to cvv according to options:
     * 1) expand-first: expand element 0 to a complete command, not a potentially abbreviated command, 
     *    eg: "co term" -> "configure terminal
     * 2) exclude-keys: remove all constant keywords, eg "conf <a> b" -> "conf b"
     */
    if (cligen_expand_first_get(h) &&
	cvec_expand_first(cvv1) < 0)
	goto done;
    if (cligen_exclude_keys_get(h) &&
	cvec_exclude_keys(cvv1) < 0)
	goto done;
    cligen_eval_wrap_fn_get(h, &wrapfn, &wraparg);
    if ((callbacks = co->co_callbacks) == NULL)
	callbacks = callbacks0;
    else if (callbacks0){
	callbacks->cc_fn_vec = callbacks0->cc_fn_vec;
	/* Append original parameters to end of call 
	 * For example, 
	 * Before call:
	 * 0 : "/example:x"
	 * After call:
	 * 0 : "/example:x"
	 * 1 : "candidate" # cc0:s parameter copied and appended to cc
	 */
	if (callbacks0->cc_cvec){
	    cg_var *cv = NULL;
	    while ((cv = cvec_each(callbacks0->cc_cvec, cv)) != NULL)
		cvec_append_var(callbacks->cc_cvec, cv);
	}
    }
    /* Traverse callbacks */
    for (cc = callbacks; cc; cc=co_callback_next(cc)){
	/* Vector cvec argument to callback */
    	if ((fn = co_callback_fn_get(cc)) != NULL){
	    argv = cc->cc_cvec ? cvec_dup(cc->cc_cvec) : NULL;
	    cligen_fn_str_set(h, cc->cc_fn_str);
	    /* Eval wrapper function so upper layers can make checks before and after callback */
	    if (wrapfn)
		(*wrapfn)(wraparg, &wh, cc->cc_fn_str, __FUNCTION__);
	    if ((retval = (*fn)(cligen_userhandle(h)?cligen_userhandle(h):h, 
				cvv1, 
				argv)) < 0){
		if (argv != NULL)
		    cvec_free(argv);
		cligen_fn_str_set(h, NULL);
		goto done;
	    }
	    if (wrapfn && wh != NULL)
		(*wrapfn)(wraparg, &wh, cc->cc_fn_str, __FUNCTION__);
	    if (argv != NULL)
		cvec_free(argv);
	    cligen_fn_str_set(h, NULL);
	}
    }
    retval = 0;
 done:
#if 1
    if (wh)
	free(wh);
#endif
    if (cvv1)
	cvec_free(cvv1);
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
