/*
  CLI generator input/output support functions.

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
#include "cligen_callback.h"
#include "cligen_object.h"
#include "cligen_handle.h"
#include "cligen_result.h"
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
static int D_LINES=0; /* XXX: global */
static int D_COLUMNS=0; /* XXX: global */

/*! Reset cligen_output to initial state
 * For new output or when 'q' is pressed that sets d_line to -1
 */
int
cli_output_reset(void)
{
    D_LINES = 0;
    D_COLUMNS = 0;
    return 0;
}

int
cli_output_status(void)
{
    return D_LINES;
}

/*! cligen_output support function for the actual scrolling
 *
 * @param[in] f           Open stdio FILE pointer
 * @param[in] ibuf        Input buffer containing all chars to be printed including 0 or many \n
 * @param[in] linelen     Length of single printable line, less than or equal to width of 
 *                        terminal window, unless the terminal window is 0
 * @param[in] term_rows   Height of terminal window
 * @see cligen_output
 */
#include <assert.h> // XXX

static int
cligen_output_scroll(FILE   *f,
		     char   *ibuf,
		     ssize_t linelen,
		     int     term_rows)
{
    int     retval = -1;
    char   *ibend;
    char   *ib0;  /* Moving window start */
    char   *ib1;  /* Moving window end */
    char   *ibcr; 
    char    c;
    char   *linebuf = NULL;
    ssize_t remain;
    
    ib0 = ibuf;
    ib1 = ibuf;
    ibend = ibuf + strlen(ibuf);
    /* A terminal line */
    if ((linebuf = malloc(linelen+1)) == NULL)
	goto done;
    remain = linelen - D_COLUMNS;
    while (ib1 < ibend && D_LINES >= 0){
	/* Four cases:
	 * 1. There is a CR in [ib0,ibend]
	 *   1a) greater than remaining: (inc D_LINE)
	 *   1b) less than or equal to remain: Only case where line has (terminating) CR
	 * 2. No CR
	 *   2a) greater than remain: (inc D_LINE)
	 *   2b) less than or equal to remain: 
	 */
	if ((ibcr = strstr(ib0, "\n")) != NULL){
	    if ((ibcr - ib0) > remain){
		ib1 = ib0 + remain; /* 1a */
		D_LINES++;
		remain = linelen;
	    }
	    else{
		ib1 = ibcr+1;        /* 1b */
		D_LINES++;
		remain = linelen;
	    }
	}
	else if (ibend - ib0 >= remain){
	    ib1 = ib0 + remain;     /* 2a */
	    D_LINES++;
	    remain = linelen;
	}
	else{
	    remain -= ibend-ib1;
	    assert(remain<=linelen && remain>=0);
	    ib1 = ibend;             /* 2b */
	}
	if (ib0 == ib1)
	    break;
	memcpy(linebuf, ib0, (ib1-ib0));
	linebuf[(ib1-ib0)] = '\0';
	assert(*ib0 != '\0');
	fprintf(f, "%s", linebuf);
	ib0 = ib1;
	if (D_LINES >= (term_rows -1)){
	    gl_char_init();
	    
	    fprintf(f, "--More--");
	    fflush(f);
	    c = fgetc(stdin);
	    if (c == '\n')
		D_LINES--;
	    else if (c == ' ')
		D_LINES = 0;
	    else if (c == 'q' || c == 3) /* ^c */
		D_LINES = -1;
	    else if (c == '?')
		fprintf(f, "Press CR for one more line, SPACE for next page, q to quit\n");
	    else 
		D_LINES = 0;  
	    fprintf(f, "        ");
	    gl_char_cleanup();
	}
    }
    D_COLUMNS = linelen-remain;
    retval = 0;
 done:
    if (linebuf)
	free(linebuf);
    return retval;
}

/*! CLIgen output function. All printf-style output should be made via this function.
 * 
 * Note only scrolling for stdout
 * It deals with formatting, page breaks, etc, (but only if f is stdout)
 * @param[in] f           Open stdio FILE pointer
 * @param[in] template... See man printf(3)
 *
 * @note: There has also been a discussion on the use of handles in this code (it relies on a
 * global variable _terminalrows and D_LINES). However, the signature needs to be the same as
 * fprintf in order to make compatible printing code.
 *
 * @note Related to the handle question is the use of the global variable D_LINES in order to
 * handle multiple calls (such as in a loop) to cligen_output. However, this assumes D_LINES
 * is reset before a new usage to D_LINES using the function cli_output_reset(). This therefore
 * be done between invocations. Especially this applies to 'q'. Further, to react to quit, you need
 * to poll cli_output_status() < 0.
 *
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
 * Chop up input buffers as follows:
 * in:    01234567890123456789\n0123456
 * width: |----------|
 * line1: 012345678901
 * line2: 23456789
 * line3: 0123456
 */
int
cligen_output(FILE       *f,
	      const char *template,
	      ... )
{
    int     retval = -1;
    va_list args;
    char   *inbuf = NULL;
    ssize_t linelen;
    int     term_rows;
    int     term_width;
    ssize_t inbuflen;
    
    /* Get terminal width and height, note discussion regarding NULL handle */
    term_rows = cligen_terminal_rows(NULL);
    term_width = cligen_terminal_width(NULL);

    /* form a string in inbuf from all args */
    va_start(args, template);
    inbuflen = vsnprintf(NULL, 0, template, args);
    va_end(args);

    if ((inbuf = malloc(inbuflen+1)) == NULL)
	goto done;
    va_start(args, template);
    vsnprintf(inbuf, inbuflen+1, template, args);
    va_end(args);

    if (term_width > 0)
	linelen = term_width;
    else
	linelen = inbuflen;

    /* if writing to stdout, format output
     */
    if (term_rows && (f == stdout)){
	if (cligen_output_scroll(f, inbuf, linelen, term_rows) < 0)
	    goto done;
    }
    else{
	fprintf(f, "%s", inbuf);
    }  
    fflush(f);
    retval = 0;
 done:
    if (inbuf)
	free(inbuf);
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
		struct cligen_help *ch)
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

/*! Check equivalence of two cligen help structs
 *
 * Utility function for printing commands and help. If two are equal, do not repeat them, just
 * print once. If "help" is set, also consider helt string, or at least first line of text.
 * The difference is whether only commands are written (eg in columns) or written in lines where
 * also helps trings are shown.
 * @param[in] ch0   First CLIgen help struct
 * @param[in] ch1   Second CLIgen help struct
 * @param[in] help  If 0, only compare commands, if 1, also compare helstrings (first row)
 * @retval    0     Not equal
 * @retval    1     Equal
 */
int
cligen_help_eq(struct cligen_help *ch0,
	       struct cligen_help *ch1,
	       int                 help)
{
    char   *cmd0  = ch0->ch_cmd;
    char   *cmd1  = ch1->ch_cmd;
    cvec   *help0 = ch0->ch_helpvec;
    cvec   *help1 = ch1->ch_helpvec;
    cg_var *cv0;
    cg_var *cv1;

    if (cmd0 == NULL && cmd1 == NULL)
	return 1;
    if (cmd0 == NULL || cmd1 == NULL)
	return 0;
    if (strcmp(cmd0, cmd1) != 0)
	return 0;
    if (help == 0)
	return 1;
    /* Commands are equal, check help string */
    if (help0 == NULL && help1 == NULL)
	return 1;
    if (help0 == NULL || help1 == NULL)
	return 0;
    /* Get first line only as equality check */
    cv0 = cvec_i(help0, 0);
    cv1 = cvec_i(help1, 0);
    if (cv0 == NULL && help1 == NULL)
	return 1;
    if (cv0 == NULL || help1 == NULL)
	return 0;
    return strcmp(cv_string_get(cv0), cv_string_get(cv1)) == 0;
}

/* Dont actually free it */
int
cligen_help_clear(struct cligen_help *ch)
{
    if (ch == NULL)
	return 0;
    if (ch->ch_cmd)
	free(ch->ch_cmd);
    if (ch->ch_helpvec)
	cvec_free(ch->ch_helpvec);
    memset(ch, 0, sizeof(*ch));
    return 0;
}

/*! Print help lines for subset of a parsetree vector
 * @param[in] fout     File to print to, eg stdout
 * @param[in] ptvec    Cligen parse-node vector
 * @param[in] matchvec Array of indexes into ptvec to match (the subset)
 * @param[in] matchlen Length of matchvec
 * @see show_help_columns
 */
int
print_help_lines(cligen_handle h,
		 FILE         *fout, 
		 parse_tree   *ptmatch)
{
    int              retval = -1;
    cg_obj          *co;
    char            *cmd;
    int              i;
    cbuf            *cb = NULL;
    struct cligen_help *chvec = NULL;
    struct cligen_help *ch;
    int              maxlen = 0;
    int              nrcmd = 0;
    int              column_width;

    if ((cb = cbuf_new()) == NULL)
	return -1;
    /* Go through match vector and collect commands and helps */
    if ((chvec = calloc(pt_len_get(ptmatch), sizeof(struct cligen_help))) ==NULL){
	perror("calloc");
	goto done;
    }
    for (i=0; i<pt_len_get(ptmatch); i++){
	co = pt_vec_i_get(ptmatch, i);
	if (co->co_command == NULL)
	    continue;
	cmd = NULL;
	switch(co->co_type){
	case CO_VARIABLE:
	    cbuf_reset(cb);
	    cov2cbuf(cb, co, 1);
	    cmd = cbuf_get(cb);
	    break;
	case CO_COMMAND:
	    cmd = co->co_command;
	    break;
	default:
	    continue;
	    break;
	}
	ch = &chvec[nrcmd];
	if ((ch->ch_cmd = strdup(cmd)) == NULL)
	    goto done;
	if (co->co_helpstring && cligen_txt2cvv(co->co_helpstring, &ch->ch_helpvec) < 0)
	    goto done;
	if (nrcmd && cligen_help_eq(&chvec[nrcmd-1], ch, 1) == 1){
	    cligen_help_clear(ch);
	    continue;
	}
	nrcmd++;
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
	for (i=0; i<nrcmd; i++)
	    cligen_help_clear(&chvec[i]);
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
    return print_help_lines(h, fout, pt);
}

