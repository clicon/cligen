/*
  CLI generator input/output support functions.

  ***** BEGIN LICENSE BLOCK *****
 
  Copyright (C) 2001-2020 Olof Hagsand

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

  * ***** END LICENSE BLOCK ***** 
 */

#include "cligen_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <string.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>

#include "cligen_buf.h"
#include "cligen_cv.h"
#include "cligen_cvec.h"
#include "cligen_parsetree.h"
#include "cligen_pt_head.h"
#include "cligen_object.h"
#include "cligen_handle.h"
#include "cligen_read.h"
#include "cligen_print.h"
#include "cligen_io.h"
#include "cligen_getline.h"

/*
 * Constants
 */
/* When doing query (?) how large left margin before first command.
 * For example, the space before foo
 * @code
 * > set ?
 *    foo   <--- number of spaces left of "foo"
 * @endcode
 * 
 */
#define CLIGEN_HELP_LEFT_MARGIN 3

/*
 * Local variables
 */
static int d_lines=0; /* XXX: global */

int
cli_output_reset(void)
{
    d_lines = 0;
    return 0;
}

/*! CLIgen output function. All printf-style output should be made via this function.
 * 
 * It deals with formatting, page breaks, etc. 
 * @param[in] f           Open stdio FILE pointer
 * @param[in] template... See man printf(3)
 * @note: There has been a debate whether this function is the right solution to the
 * pageing problem of CLIgen or not. 
 * (1) On the one hand, a less/more like sub-process could be forked and stdout piped to this
 * sub-process. This would handle all prints to stdout, instead of relying on all output
 * functions using this function and not printf.
 * (2) On the other hand, this gives a slim and simple solution with smaller footprint (no forked
 * process), but all output functions need to pass though this code.
 * For now (2) is used and extended also for clixon functions. However (1) could still be 
 * implemented as an option.
 *
 * @note: There has also been a discussion on the use of handles in this code (it relies on a
 * global variable _terminalrows). However, the signature needs to be the same as fprintf in
 * order to make compatible printing code.
 */
int
cligen_output(FILE       *f,
	      const char *template,
	      ... )
{
    int     retval = -1;
    va_list args;
    char   *buf = NULL;
    char   *start;
    char   *end;
    char   *bufend;
    char    c;
    int     term_rows;
    int     len;

    term_rows = cligen_terminal_rows(NULL);
    /* form a string in buf from all args */

    va_start(args, template);
    len = vsnprintf(NULL, 0, template, args);
    va_end(args);
    len++;
    if ((buf = malloc(len)) == NULL){
	fprintf(stderr, "%s: malloc: %s\n", __FUNCTION__, strerror(errno));
	goto done;
    }
    va_start(args, template);
    vsnprintf(buf, len, template, args);
    va_end(args);

    /* if writing to stdout, format output
     */
    if ((term_rows) && (f == stdout)){
	start = end = buf;
	bufend = buf + strlen(buf);
	while (end < bufend){
	    end = strstr(start, "\n");
	    if (end) /* got a NL */{
		if (d_lines >= 0)
		    d_lines++;
		*end = '\0';
		if (d_lines > -1)
		    fprintf(f, "%s\n", start);
	      
		if (end < bufend)
		    start = end+1;
	      
		if (d_lines >= (term_rows -1)){		    
		    gl_char_init();

		    fprintf(f, "--More--");
		    c = fgetc(stdin);
		    if (c == '\n')
			d_lines--;
		    else if (c == ' ')
			    d_lines = 0;
		    else if (c == 'q' || c == 3) /* ^c */
			d_lines = -1;
		    else if (c == '?')
			fprintf(f, "Press CR for one more line, SPACE for next page, q to quit\n");
		    else 
			d_lines = 0;  
		    fprintf(f, "        ");
		    gl_char_cleanup();
		}
	    } /* NL */
	    else{
		/* do only print if we have data */
		if (d_lines >=0 && *start != '\0')
		    fprintf(f, "%s", start);
		end = start + strlen(start);
		start = end;
	    }
	}
    }
    else{
	fprintf(f, "%s", buf);
    }  
    fflush(f);
    retval = 0;
 done:
    if (buf)
	free(buf);
    return retval;
}

#ifdef notyet
/*
 * Yes/No question. Returns 1 for yes and 0 for no.
 */
int
cli_yesno(const char *fmt, ...)
{
    va_list	  ap;
    char	  buf[1024];

    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf)-1, fmt, ap);
    va_end(ap);
    printf("%s [yes/no]: ", buf);
    if (cli_getln(buf, sizeof(buf)) == 0)
	if (strlen(buf) && !strncasecmp(buf, "yes", strlen(buf)))
	    return 1;
    return 0;
}
#endif

#if CLIGEN_REGFD
/* The following three callback functions are just wrappers in order to
   not expose getline to external interface */
int 
cligen_regfd(int fd, cligen_fd_cb_t *cb, void *arg)
{
    return gl_regfd(fd, cb, arg);
}

int 
cligen_unregfd(int fd)
{
    return gl_unregfd(fd);
}
#endif /* CLIGEN_REGFD */

void 
cligen_redraw(cligen_handle h)
{
    gl_redraw(h);
}

/*! Register a suspend (^Z) function hook 
 */
int
cligen_susp_hook(cligen_handle     h, 
		 cligen_susp_cb_t *fn)
{
    gl_susp_hook = fn; /* XXX global */
    return 0;
}

/*! Register an interrupt hook, called if read() interrupted with (some) SIG
 */
int
cligen_interrupt_hook(cligen_handle          h, 
		      cligen_interrupt_cb_t *fn)
{
    gl_interrupt_hook = fn; 
    return 0;
}

/*! Register extra exit characters (in addition to ctrl-c)
 */
void 
cligen_exitchar_add(cligen_handle h, 
		    char          c)
{
    gl_exitchar_add(c); /* XXX global */
}

/*! Display multi help lines on query (?)
 * Function handles multiple options on how to display help strings at query (?)
 * This includes indentation, limit on lines, truncation, etc.
 * @param[in]  h             Cligen handle
 * @param[in]  fout          File to print to, eg stdout
 * @param[in]  column_width  Space for first, command width
 * @param[in]  ch            Help command and string struct
 */
static int
print_help_line(cligen_handle    h,
		FILE            *fout,
		int              column_width,
		struct cmd_help *ch)
{
    int     retval = -1;
    cg_var *cv = NULL;
    int     w;
    char   *str;
    char   *str2;
    int     j;
    int     linesmax;
    int     termwidth;
    int     truncate;
	    
    /* First print command */
    fprintf(fout, "  %*s", -column_width, ch->ch_cmd);
    /* Then print help */
    if (ch->ch_helpvec && cvec_len(ch->ch_helpvec)){
	linesmax = cligen_helpstring_lines(h);
	truncate = cligen_helpstring_truncate(h);
	termwidth = cligen_terminal_width(h);
	j = 0;
	while ((cv = cvec_each(ch->ch_helpvec, cv)) != NULL &&
	       (linesmax==0 || j<linesmax)){
	    w = termwidth - column_width - CLIGEN_HELP_LEFT_MARGIN;
	    str = cv_string_get(cv);
	    if (j > 0) /* skip first line */
		fprintf(fout, "  %*s", -column_width, "");
	    if (truncate == 0 ||
		strlen(str) < w){
		fprintf(fout, " %*s", -w, str);
	    }
	    else {
		if ((str2 = strdup(str)) == NULL)
		    goto done;
		str2[w] = '\0';
		fprintf(fout, " %*s", -w, str2);
		free(str2);
		str2 = NULL;
	    }
	    fprintf(fout, "\n");
	    j++;
	}
    }
    else
	fprintf(fout, "\n");
    retval = 0;
 done:
    return retval;
}

/*! Print help lines for subset of a parsetree vector
 * @param[in] fout     File to print to, eg stdout
 * @param[in] ptvec    Cligen parse-node vector
 * @param[in] matchvec Array of indexes into ptvec to match (the subset)
 * @param[in] matchlen Length of matchvec
 */
int
print_help_lines(cligen_handle h,
		 FILE         *fout, 
		 parse_tree   *ptmatch, 
		 int          *matchvec,
		 size_t        matchlen)
{
    int              retval = -1;
    cg_obj          *co;
    char            *cmd;
    int              i;
    cbuf            *cb = NULL;
    struct cmd_help *chvec = NULL;
    struct cmd_help *ch;
    int              maxlen = 0;
    char            *prev = NULL;
    int              nrcmd = 0;
    int              column_width;
    int              vi;

    if ((cb = cbuf_new()) == NULL){
	fprintf(stderr, "cbuf_new: %s\n", strerror(errno));
	return -1;
    }
    /* Go through match vector and collect commands and helps */
    if ((chvec = calloc(matchlen, sizeof(struct cmd_help))) ==NULL){
	perror("calloc");
	goto done;
    }
    for (i=0; i<matchlen; i++){
	vi = matchvec[i]; /* index into array, extra indirection */
	co = pt_vec_i_get(ptmatch, vi);
	if (co->co_command == NULL)
	    continue;
	cmd = NULL;
	switch(co->co_type){
	case CO_VARIABLE:
	    cbuf_reset(cb);
	    cov2cbuf(cb, co, 1);
	    if ((cmd = strdup(cbuf_get(cb))) == NULL){
		perror("strdup");
		goto done;
	    }
	    break;
	case CO_COMMAND:
	    if ((cmd = strdup(co->co_command)) == NULL){
		perror("strdup");
		goto done;
	    }
	    break;
	default:
	    continue;
	    break;
	}
	if (prev && strcmp(cmd, prev)==0)
	    continue;
	ch = &chvec[nrcmd++];
	ch->ch_cmd = cmd;
	ch->ch_helpvec = co->co_helpvec;
	prev = cmd;
	/* Compute longest command */
	maxlen = strlen(cmd)>maxlen?strlen(cmd):maxlen;
    }
    maxlen++;
    column_width = maxlen<COLUMN_MIN_WIDTH?COLUMN_MIN_WIDTH:maxlen;
    /* Actually print */
    for (i = 0; i<nrcmd; i++){
	ch = &chvec[i];
	if (print_help_line(h, fout, column_width, ch) < 0)
	    goto done;
    }
    fflush(fout);
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
    return retval;
}

/*! Print top-level help (all commands) of a parse-tree
 * @param[in] fout  File to print to, eg stdout
 * @param[in] pt    Cligen parse-tree
 */
int
cligen_help(cligen_handle h,
	    FILE         *fout, 
	    parse_tree   *pt)
{
    int              retval = -1;
    int              i;
    int             *matchvec = NULL;

    /* intermediate struct to fit into print_help_lines() parameters */
    if ((matchvec = calloc(pt_len_get(pt), sizeof(int))) == NULL){
	perror("calloc");
	goto done;
    }
    for (i=0; i<pt_len_get(pt); i++)
	matchvec[i] = i;
    if (print_help_lines(h, fout, pt, matchvec, pt_len_get(pt)) < 0)
	goto done;
    retval = 0;
 done:
    if (matchvec)
	free(matchvec);
    return retval;
}

