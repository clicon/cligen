/*
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
#include <stdarg.h>
#include <inttypes.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <ctype.h>
#define __USE_GNU /* strverscmp */
#include <string.h>
#include <errno.h>
#include <termios.h>
#include <signal.h>
#include <sys/ioctl.h>

#include "cligen_buf.h"
#include "cligen_cv.h"
#include "cligen_cvec.h"
#include "cligen_parsetree.h"
#include "cligen_pt_head.h"
#include "cligen_callback.h"
#include "cligen_object.h"
#include "cligen_io.h"
#include "cligen_handle.h"
#include "cligen_result.h"
#include "cligen_read.h"
#include "cligen_parse.h"
#include "cligen_print.h"
#include "cligen_history.h"
#include "cligen_getline.h"
#include "cligen_handle_internal.h"
#include "cligen_history.h"
#include "cligen_history_internal.h"

/*
 * Constants
 */
#define TREENAME_KEYWORD_DEFAULT "treename"

/* forward */
static int terminal_rows_set1(int rows);

/*
 * Variables
 */
/* Number of terminal rows as used by cligen_output pageing routine
 * @see cligen_output
 */
static int _terminalrows = 0;

/* Truncate help string on right margin mode
 * This only applies if you have really long help strings, such as when generating them from a
 * spec.
 * @see print_help_line
 */
static int _helpstr_truncate = 0;

/* Limit number of lines to show, 0 means unlimited
 * This only applies if you have multi-line help strings, such as when generating them from a
 * spec.
 * @see print_help_line
 */
static int _helpstr_lines = 0;

/*! Get window size and set terminal row size
 * @param[in] h       CLIgen handle
 * The only real effect this has is to set the getline width parameter which effects scrolling
 * man ioctl_tty(2)
  Get and set window size
       Window  sizes  are  kept in the kernel, but not used by the kernel (except in the
       case of virtual consoles, where the kernel will update the window size  when  the
       size of the virtual console changes, for example, by loading a new font).

       The following constants and structure are defined in <sys/ioctl.h>.

       TIOCGWINSZ     struct winsize *argp
              Get window size.
 */
static int
cligen_gwinsz(cligen_handle h)
{
    struct winsize ws;

    if (ioctl(0, TIOCGWINSZ, &ws) == -1){
	perror("ioctl(STDIN_FILENO,TIOCGWINSZ)");
	return -1;
    }
    terminal_rows_set1(ws.ws_row); /* note special treatment of 0 in sub function */
    cligen_terminal_width_set(h, ws.ws_col);

    return 0;
}

void
sigwinch_handler(int arg)
{
    cligen_gwinsz(0);
}

/*! This is the first call the CLIgen API and returns a handle. 
 * Allocate CLIgen handle to be used in API calls
 * Initialize prompt, tabs, query terminal setting for width etc.
 * @retval h  CLIgen handle
 */
cligen_handle 
cligen_init(void)
{
    struct cligen_handle *ch;
    cligen_handle         h = NULL;
    struct sigaction      sigh;

    if ((ch = malloc(sizeof(*ch))) == NULL){
	fprintf(stderr, "%s: malloc: %s\n", __FUNCTION__, strerror(errno));
	goto done;
    }
    memset(ch, 0, sizeof(*ch));
    ch->ch_magic = CLIGEN_MAGIC;
    ch->ch_tabmode = 0x0; /* see CLIGEN_TABMODE_* */
    ch->ch_delimiter = ' ';
    h = (cligen_handle)ch;
    cligen_prompt_set(h, CLIGEN_PROMPT_DEFAULT);
    /* Only if stdin and stdout refers to a terminal make win size check */
    if (isatty(0) && isatty(1)){
	if (cligen_gwinsz(h) < 0){
	    free(ch);
	    return NULL;
	}
	cligen_interrupt_hook(h, cligen_gwinsz);
	memset(&sigh, 0, sizeof(sigh));
	sigh.sa_handler = sigwinch_handler;
	if (sigaction(SIGWINCH, &sigh, NULL) < 0){
	    perror("sigaction");
	    return NULL;
	}
    }
    else
	terminal_rows_set1(0); 
    cliread_init(h);
    cligen_buf_init(h);
    /* getline cant function without some history */
    (void)cligen_hist_init(h, CLIGEN_HISTSIZE_DEFAULT);
  done:
    return h;
}

/*! This is the last call to the CLIgen API an application should make
 * @param[in] h       CLIgen handle
 */
int
cligen_exit(cligen_handle h)
{
    struct cligen_handle *ch = handle(h);
    pt_head              *ph;

    hist_exit(h);
    cligen_buf_cleanup(h);
    if (ch->ch_prompt)
	free(ch->ch_prompt);
    if (ch->ch_nomatch)
	free(ch->ch_nomatch);  
    if (ch->ch_treename_keyword)
	free(ch->ch_treename_keyword);
    if (ch->ch_fn_str)
	free(ch->ch_fn_str);
    while ((ph = ch->ch_pt_head) != NULL){
	ch->ch_pt_head = ph->ph_next;
	cligen_ph_free(ph);
    }
    free(ch);
    return 0;
}

/*! Check struct magic number for sanity checks
 * @param[in] h       CLIgen handle
 * return 0 if OK, -1 if fail.
 */
int
cligen_check(cligen_handle h)
{
    /* Dont use handle macro to avoid recursion */
    struct cligen_handle *ch = (struct cligen_handle *)(h);

    return ch->ch_magic == CLIGEN_MAGIC ? 0 : -1;
}

/*! return CLIgen exiting status
 * @param[in] h       CLIgen handle
 */
int 
cligen_exiting(cligen_handle h)
{
    struct cligen_handle *ch = handle(h);

    return ch->ch_exiting;
}

/*! Set  CLIgen exiting status
 * @param[in] h       CLIgen handle
 */
int 
cligen_exiting_set(cligen_handle h, 
		   int           status)
{
    struct cligen_handle *ch = handle(h);

    ch->ch_exiting = status;
    return 0;
}

/*! Get comment character.
 * @param[in] h       CLIgen handle
 */
char
cligen_comment(cligen_handle h)
{
    struct cligen_handle *ch = handle(h);

    return ch->ch_comment;
}

/*! Set comment character.
 * @param[in] h       CLIgen handle
 */
int
cligen_comment_set(cligen_handle h, 
		   char          c)
{
    struct cligen_handle *ch = handle(h);

    ch->ch_comment = c;
    return 0;
}

/*! Get current prompt string
 * @param[in] h       CLIgen handle
 */
char*
cligen_prompt(cligen_handle h)
{
    struct cligen_handle *ch = handle(h);

    return ch->ch_prompt;
}

/*! Set CLIgen prompt string. See manual for special prompt characters.
 * @param[in] h       CLIgen handle
 * @param[in] prompt  Prompt string
 */
int
cligen_prompt_set(cligen_handle h, 
		  char         *prompt)
{
    struct cligen_handle *ch = handle(h);

    if (ch->ch_prompt){
	if (strcmp(prompt, ch->ch_prompt) == 0)
	    return 0;
	free(ch->ch_prompt);
	ch->ch_prompt = NULL;
    }
    if (prompt){
	if ((ch->ch_prompt = strdup(prompt)) == NULL)
	    return -1;
    }
    return 0;
}

/*! Get CLIgen parse-tree head holding all parsetrees in the system
 */
pt_head *
cligen_pt_head_get(cligen_handle h)
{
    struct cligen_handle *ch = handle(h);

    return ch->ch_pt_head;
}

/*! Set CLIgen parse-tree head holding all parsetrees in the system
 */
int
cligen_pt_head_set(cligen_handle h,
		   pt_head      *ph)
{
    struct cligen_handle *ch = handle(h);

    ch->ch_pt_head = ph;
    return 0;
}

/*! Get name of treename keyword used in parsing
 * @param[in] h       CLIgen handle
 * Example in CLIgen file where 'treename' is treename_keyword:
 * treename = "foo";
 * bar @bar;
 * y;
 */
char*
cligen_treename_keyword(cligen_handle h)
{
    struct cligen_handle *ch = handle(h);

    return ch->ch_treename_keyword?ch->ch_treename_keyword:TREENAME_KEYWORD_DEFAULT;
}

/*! Set currently active parsetree by name.
 * @param[in] h       CLIgen handle
 */
int
cligen_treename_keyword_set(cligen_handle h, 
			    char         *treename)
{
    struct cligen_handle *ch = handle(h);

    if (ch->ch_treename_keyword){
	free(ch->ch_treename_keyword);
	ch->ch_treename_keyword = NULL;
    }
    if (treename)
	if ((ch->ch_treename_keyword = strdup(treename)) == NULL)
	    return -1;
    return 0;
}

/*! Return CLIgen object that matched in the current callback.
 *  After an evaluation when calling a callback, a node has been matched in the
 * current parse-tree. This matching node is returned (and set) here.
 * @param[in] h       CLIgen handle
 */
cg_obj *
cligen_co_match(cligen_handle h)
{
    struct cligen_handle *ch = handle(h);

    return ch->ch_co_match;
}

/*!
 * @param[in] h       CLIgen handle
 */
int
cligen_co_match_set(cligen_handle h, 
		    cg_obj       *co)
{
    struct cligen_handle *ch = handle(h);

    ch->ch_co_match = co;
    return 0;
}

/*! Get callback function name string
 *
 * @code
 * static int
 * my_cb(cligen_handle h, cvec *vars, cg_var *arg)
 * {
 *   printf("cb: %s\n", cligen_fn_str_get(h));
 *   return 0;
 * }
 * @endcode
 * @param[in] h       CLIgen handle
 */
char *
cligen_fn_str_get(cligen_handle h)
{
    struct cligen_handle *ch = handle(h);

    return ch->ch_fn_str;
}

/*! Set callback function name string
 *
 * @param[in] h       CLIgen handle
 * @param[in] fn_str  Name of function that was called in this callback
 */
int
cligen_fn_str_set(cligen_handle h, 
		  char         *fn_str)
{
    struct cligen_handle *ch = handle(h);

    if (ch->ch_fn_str){
	free(ch->ch_fn_str);
	ch->ch_fn_str = NULL;
    }
    if (fn_str){
	if ((ch->ch_fn_str = strdup(fn_str)) == NULL)
	    return -1;
    }
    return 0;
}

/*! Get number of displayed terminal rows.
 * @param[in] h       CLIgen handle
 */
int 
cligen_terminal_rows(cligen_handle h)
{
//    struct cligen_handle *ch = handle(h);

    return _terminalrows; /* ch->ch_terminalrows; */
}

/*! Set number of displayed terminal rows, internal function
 * @param[in] h       CLIgen handle
 * @param[in] rows    Number of lines in a terminal (y-direction)
 */
static int 
terminal_rows_set1(int rows)
{
    _terminalrows = rows;
    return 0;
}

/*! Set number of displayed terminal rows.
 * @param[in] h       CLIgen handle
 * @param[in] rows    Number of lines in a terminal (y-direction)
 */
int 
cligen_terminal_rows_set(cligen_handle h, 
			int           rows)
{
    int            retval = -1;
    struct winsize ws;

    /* Sanity checks :
     * (1) only set new value if it runs in a tty
     * (2) cannot determine window size
     */
    if (!isatty(0) || !isatty(1))
	goto ok;
    if (ioctl(0, TIOCGWINSZ, &ws) == -1){
	perror("ioctl(STDIN_FILENO,TIOCGWINSZ)");
	goto done;
    }
    if (ws.ws_row !=0 )
	goto ok;
    terminal_rows_set1(rows);
 ok:
    retval = 0;
 done:
    return retval;
}

/*! Get length of lines (number of 'columns' in a line)
 *
 * @param[in] h       CLIgen handle
 */
int 
cligen_terminal_width(cligen_handle h)
{
//    struct cligen_handle *ch = handle(h);

    return gl_getwidth()==0xffff?80:gl_getwidth();
}

/*! Set width of a CLIgen line in characters, ie, the number of 'columns' in a line
 *
 * @param[in] h       CLIgen handle
 * @param[in] width   Number of characters in a line - x-direction (see notes)
 * @note if length = 0, then set it to 65535 to effectively disable all scrolling mechanisms
 * @note if length < 21 set it to 21, which is getline's limit.
 */
int 
cligen_terminal_width_set(cligen_handle h, 
			  int           width)
{
    //    struct cligen_handle *ch = handle(h);
    int retval = -1;

    /* if width = 0, then set it to 65535 to effectively disable all scrolling mechanisms 
     * This is somewhat complex and there may be some missed cornercases:
     * - Set very high width to disable horizontal scrolling
     * - But return 80 width see cligen_terminal_width
     */
    if (width == 0)
	width = 0xffff;
    /* if width < 21 set it to 21, which is getline's limit. */
    else if (width < TERM_MIN_SCREEN_WIDTH)
	width = TERM_MIN_SCREEN_WIDTH;
    if (gl_setwidth(width) < 0)
	goto done; /* shouldnt happen */
    retval = 0;
 done:
    return retval;
}

/*! Get cligen/getline UTF-8 experimental mode
 *
 * @param[in] h       CLIgen handle
 * @retval    0       UTF-8 mode disabled
 * @retval    1       UTF-8 mode enabled
 */
int 
cligen_utf8_get(cligen_handle h)
{
    return gl_utf8_get();
}

/*! Set cligen/getline UTF-8 experimental mode
 *
 * @param[in] h       CLIgen handle
 * @retval    0       UTF-8 mode disabled
 * @retval    1       UTF-8 mode enabled
 */
int 
cligen_utf8_set(cligen_handle h,
		int           mode)
{
    return gl_utf8_set(mode);
}

/*! Get line scrolling mode
 *
 * @param[in] h       CLIgen handle
 * @retval    0       Line scrolling off
 * @retval    1       Line scrolling on
 */
int 
cligen_line_scrolling(cligen_handle h)
{
    return gl_getscrolling();
}

/*! Set line scrolling mode
 *
 * @param[in] h      CLIgen handle
 * @param[in] mode   0: turn line scrolling off, 1: turn on
 * @retval    old    Previous setting
 */
int 
cligen_line_scrolling_set(cligen_handle h,
			  int           mode)
{
    int prev = gl_getscrolling();

    gl_setscrolling(mode);
    return prev;
}

/*! Return help string truncate mode (for ?)
 *
 * Whether to truncate help string on right margin or wrap long help lines.
 * This only applies if you have really long help strings, such as when generating them from a
 * spec.
 * @param[in] h       CLIgen handle (dummy, need to be called where h is NULL)
 * @retval    0       Do not truncate help string on right margin (wrap long help lines)
 * @retval    1       Truncate help string on right margin (do not wrap long help lines)
 * @see print_help_line
 */
int 
cligen_helpstring_truncate(cligen_handle h)
{
    return _helpstr_truncate;
}

/*! Set help string truncate mode (for ?)
 *
 * Whether to truncate help string on right margin or wrap long help lines.
 * This only applies if you have really long help strings, such as when generating them from a
 * spec.
 * @param[in]  h     CLIgen handle (dummy, need to be called where h is NULL)
 * @param[in]  mode  0: Wrap long help strings, 1: Truncate help string
 * @retval     0     OK
 * @see print_help_line
 */
int 
cligen_helpstring_truncate_set(cligen_handle h,
			       int           mode)
{
    _helpstr_truncate = mode;
    return 0;
}

/*! Return number of help string lines to display (for ?)
 *
 * This only applies if you have multi-line help strings, such as when generating them from a
 * spec.
 * @param[in] h       CLIgen handle (dummy, need to be called where h is NULL)
 * @retval    n       Number of help string lines to display per command, 0 is unlimted
 * @see print_help_line
 */
int 
cligen_helpstring_lines(cligen_handle h)
{
    return _helpstr_lines;
}

/*! Set help string truncate mode (for ?)
 *
 * This only applies if you have multi-line help strings, such as when generating them from a
 * spec.
 * @param[in] h       CLIgen handle (dummy, need to be called where h is NULL)
 * @retval    n       Number of help string lines to display per command, 0 means unlimited.
 * @see print_help_line
 */
int 
cligen_helpstring_lines_set(cligen_handle h,
			       int        lines)
{
    _helpstr_lines = lines;
    return 0;
}

/*! Get tab-mode. 
 *
 * @param[in] h       CLIgen handle
 * @retval    flags   Bitwise OR of CLIGEN_TABMODE_* flags
 * @see cligen_tabmode_set for documentation on mode
 */
int 
cligen_tabmode(cligen_handle h)
{
    struct cligen_handle *ch = handle(h);

    return ch->ch_tabmode;
}

/*! Set Cligen tab mode. Combination of CLIGEN_TABMODE_* flags
 *
 * @param[in] h       CLIgen handle
 * @param[in] mode    Bitwise OR of CLIGEN_TABMODE_* flags
 * CLIGEN_TABMODE_COLUMNS:
 * 0 is 'short/ios' mode, 1 is long/junos mode.
 * Two ways to show commands: show_help_column and show_help_line
 * show_help_line
 *  cli> interface name 
 * 100GigabyteEthernet6/0/0 TenGigabyteEthernet6/0/0 TenGigabyteEthernet6/0
 * TenGigabyteEthernet6/0/0 TenGigabyteEthernet6/0/0 TenGigabyteEthernet6/0
 *
 * show_help_columns:
 * cli> interface name TenGigabyteEthernet6/0/0.
 * TenGigabyteEthernet6/0/0 This is one mighty interface
 * TenGigabyteEthernet6/0/0 This is one mighty interface
 *
 * short/ios:  ?:   show_multi_long
 *             TAB: show_multi  (many columns)
 * long/junos: TAB and ?: show_multi_long
 *
 * CLIGEN_TABMODE_VARS:
 * 0: command completion preference, 
 * 1: same preference for vars
 * @example if clispec is:
 * a {
 *   b;
 *  <c:int32>, cb("a.b");
 * }
 * Typing "a <TAB>"
 * 0: completes to "a b"
 * 1: does not complete, shows:
 *   b
 *   <c>
 *
 * CLIGEN_TABMODE_STEPS
 * 0: complete 1 level. 1: complete all
 * Example: syntax is 'a b;':
 * 0: gives completion to 'a ' on first TAB and to 'a b ' on second. 
 * 1: gives completion to 'a b ' on first TAB.
 */
int 
cligen_tabmode_set(cligen_handle h, 
		   int           mode)
{
    struct cligen_handle *ch = handle(h);

    ch->ch_tabmode = mode;
    return 0;
}

static int _lexicalorder = 0; /* XXX shouldnt be global */

/*! Get lexical matching order
 * 
 * @param[in] h       CLIgen handle
 * @retval 0  strcmp
 * @retval 1  strverscmp
 */
int
cligen_lexicalorder(cligen_handle h)
{
//    struct cligen_handle *ch = handle(h);

//    return ch->ch_lexicalorder;
    return _lexicalorder;
}

/*! Set lexical matching order.
 * 
 * @param[in] h  CLIgen handle
 * @param[in] n  strcmp (0) or strverscmp (1).
 */
int
cligen_lexicalorder_set(cligen_handle h, 
			int           n)
{
//    struct cligen_handle *ch = handle(h);

//    ch->ch_lexicalorder = n;
    _lexicalorder = n;
    return 0;
}

static int _ignorecase = 0; /* XXX shouldnt be global */

/*! Ignore uppercase/lowercase or not
 * @param[in] h       CLIgen handle
 */
int
cligen_ignorecase(cligen_handle h)
{
//    struct cligen_handle *ch = handle(h);

//    return ch->ch_ignorecase;
    return _ignorecase;
}

/*! Ignore uppercase/lowercase or not
 * @param[in] h       CLIgen handle
 */
int
cligen_ignorecase_set(cligen_handle h, 
		      int           n)
{
//    struct cligen_handle *ch = handle(h);

//    ch->ch_ignorecase = n;
    _ignorecase = n;
    return 0;
}

/*! Debug syntax by printing dynamically on stderr. Get function.
 * @param[in] h       CLIgen handle
 */
int cligen_logsyntax(cligen_handle h)
{
    struct cligen_handle *ch = handle(h);

    return ch->ch_logsyntax;
}

/*! Debug syntax by printing dynamically on stderr. Set function.
 * @param[in] h       CLIgen handle
 */
int cligen_logsyntax_set(cligen_handle h, 
			 int           n)
{
    struct cligen_handle *ch = handle(h);

    ch->ch_logsyntax = n;
    return 0;
}

/*! Get app-specific handle for callbacks instead of cligen handle.
 *
 * An application may choose to use another handle than cligen_handle in callbacks
 * and completion functions. 
 * @param[in] h       CLIgen handle
 */
void*
cligen_userhandle(cligen_handle h)
{
    struct cligen_handle *ch = handle(h);

    return ch->ch_userhandle;
}

/*! Set app-specific handle for callbacks instead of cligen handle
 * @param[in] h       CLIgen handle
 */
int
cligen_userhandle_set(cligen_handle h, 
		      void         *userhandle)
{
    struct cligen_handle *ch = handle(h);

    ch->ch_userhandle = userhandle;
    return 0;
}

/*! Get app-specific data associated with cligen handle
 * @param[in] h       CLIgen handle
 */
void*
cligen_userdata(cligen_handle h)
{
    struct cligen_handle *ch = handle(h);

    return ch->ch_userdata;
}

/*! Set app-specific data to associate with cligen handle.
 * @param[in] h       CLIgen handle
 */
int
cligen_userdata_set(cligen_handle h, 
		      void         *userdata)
{
    struct cligen_handle *ch = handle(h);

    ch->ch_userdata = userdata;
    return 0;
}

/*! Get regex engine / method
 *
 * @param[in] h   CLIgen handle
 * @retval    0   Posix regex
 * @retval    1   XSD Libxml2 regex
 */
int
cligen_regex_xsd(cligen_handle h)
{
    struct cligen_handle *ch = handle(h);

    return ch->ch_regex_xsd;
}

/*! Set regex engine to 0: posix, or 1: XSD / Libxml2
 * @param[in] h       CLIgen handle
 * @param[in] mode    0: posix (default), 1: libxml2 xsd regex
 */
int
cligen_regex_xsd_set(cligen_handle h, 
		     int           mode)
{
    struct cligen_handle *ch = handle(h);

    ch->ch_regex_xsd = mode;
    return 0;
}


static int _getline_bufsize = GETLINE_BUFLEN_DEFAULT;
static int _getline_killbufsize = GETLINE_BUFLEN_DEFAULT;

/*!
 * @param[in] h       CLIgen handle
 */
char*
cligen_buf(cligen_handle h)
{
    struct cligen_handle *ch = handle(h);

    return ch->ch_buf;
}

/*!
 * @param[in] h       CLIgen handle
 */
char*
cligen_killbuf(cligen_handle h)
{
    struct cligen_handle *ch = handle(h);

    return ch->ch_killbuf;
}

/*! Return length cligen line buffer
 * @param[in] h       CLIgen handle
 * @see cligen_buf_increase
 */
int 
cligen_buf_size(cligen_handle h)
{
    return _getline_bufsize;
}

/*! Return length cligen kill buffer
 * @param[in] h       CLIgen handle
 */
int 
cligen_killbuf_size(cligen_handle h)
{
    return _getline_killbufsize;
}

/*!
 * @param[in] h       CLIgen handle
 */
int
cligen_buf_init(cligen_handle h)
{
    struct cligen_handle *ch = handle(h);

    if ((ch->ch_buf = malloc(_getline_bufsize)) == NULL){
	fprintf(stderr, "%s malloc: %s\n", __FUNCTION__, strerror(errno));
	return -1;
    }
    memset(ch->ch_buf, 0, _getline_bufsize);
    if ((ch->ch_killbuf = malloc(_getline_killbufsize)) == NULL){
	fprintf(stderr, "%s malloc: %s\n", __FUNCTION__, strerror(errno));
	return -1;
    }
    memset(ch->ch_killbuf, 0, _getline_killbufsize);
    return 0;
}

/*! Increase cligen buffer length with *2 to satisfy new length
 * @param[in] h       CLIgen handle
 * @param[in] len1    New minimal length (add null for end of string)
 * |-------------------|------|--------||------|
 * ^                   ^      ^        ^^      ^
 * ch_buf              len0   |     len1+1     |
 *                            bufsize0         bufsize1 = 2^n*bufsize0
 */
int       
cligen_buf_increase(cligen_handle h,
		    size_t        len1)
{
    struct cligen_handle *ch = handle(h);
    size_t                len0 = _getline_bufsize; /* orig length */

    if (_getline_bufsize >= len1 + 1)
      return 0;
    while (_getline_bufsize < len1 + 1)
      _getline_bufsize *= 2;      
    if ((ch->ch_buf = realloc(ch->ch_buf, _getline_bufsize)) == NULL){
	fprintf(stderr, "%s realloc: %s\n", __FUNCTION__, strerror(errno));
	return -1;
    }
    memset(ch->ch_buf+len0, 0, _getline_bufsize-len0);
    return 0;
}

/*! Increase cligen kill buffer length with *2 to satisfy new length
 * @param[in] h       CLIgen handle
 * @param[in] len1    New minimal length
 */
int       
cligen_killbuf_increase(cligen_handle h,
			size_t        len1)
{
    struct cligen_handle *ch = handle(h);
    int                   len0 = _getline_killbufsize;

    if (_getline_killbufsize >= len1 + 1)
      return 0;
    while (_getline_killbufsize < len1 + 1)
      _getline_killbufsize *= 2;      
    if ((ch->ch_killbuf = realloc(ch->ch_killbuf, _getline_killbufsize)) == NULL){
	fprintf(stderr, "%s realloc: %s\n", __FUNCTION__, strerror(errno));
	return -1;
    }
    memset(ch->ch_killbuf+len0, 0, _getline_killbufsize-len0);
    return 0;
}

/*!
 * @param[in] h       CLIgen handle
 */
int
cligen_buf_cleanup(cligen_handle h)
{
    struct cligen_handle *ch = handle(h);

    if (ch->ch_buf){
	free(ch->ch_buf);
	ch->ch_buf = NULL;
    }
    if (ch->ch_killbuf){
	free(ch->ch_killbuf);
	ch->ch_killbuf = NULL;
    }
    return 0;
}

char
cligen_delimiter(cligen_handle h)
{
    struct cligen_handle *ch = handle(h);
    return ch->ch_delimiter;
}

int
cligen_delimiter_set(cligen_handle h,
		     char          delimiter)
{
    struct cligen_handle *ch = handle(h);

    ch->ch_delimiter = delimiter;
    return 0;
}

/*! Get preference mode, return all with same pref(ambiguos) or first (1)
 * @param[in] h      CLIgen handle
 * @retval    1      Preference mode is set (return first)
 * @retval    0      Preference mode is not set (ambiguous)
 */
int 
cligen_preference_mode(cligen_handle h)
{
    struct cligen_handle *ch = handle(h);

    return ch->ch_preference_mode;
}

/*! Set preference mode, return all with same pref(ambiguous) or first 0-4
 *
 * Often, multiple matches will resolve by preference, but not if several matches have same.
 * This applies to complete (terminal matches), assume length of a is <=4:
 * @code
 * key (<a:string length[4]> | <a:string length[40]>);
 * @endcode
 * Assume the user types the following command which matches both variables:
 *   key foo
 * Will lead to: "Ambiguous command"
 * If set to 1 or 3 will select first variable.
 *
 * For non-terminals, example:
 * @code
 * key (<a:string length[4]> | <a:string length[40]>){
 *     port <nr:int32>;
 * }
 * @endcode
 * If set to 2 or 3 will select first variable.
 *
 * Do NOT use this if the two variables leads to different choices, eg:
 * @code
 * key <a:string length[4]>{
 *     port <nr:int32>;
 * }
 * key <a:string length[40]>){
 *     description <text:string>;
 * }
 * @endcode
 * @param[in] h      CLIgen handle
 * @param[in] flag   0: ambiguous error, 1: terminal first, 2: non-terminal first match, 3: non-terminal+terminal
 * @retval    0      OK
 */
int 
cligen_preference_mode_set(cligen_handle h,
			   int           flag)
{
    struct cligen_handle *ch = handle(h);

    ch->ch_preference_mode = flag;
    return 0;
}

/*! Get status of string case compare 
 *
 * @param[in] h   CLIgen handle
 * @retval    0   Case-sensitive, ie aA != aa
 * @retval    1   Ignore case, ie aA == aa
 */
int
cligen_caseignore_get(cligen_handle h)
{
    struct cligen_handle *ch = handle(h);

    return ch->ch_ignore_case;
}

/*! Set string case compare status
 *
 * @param[in]  h          CLIgen handle
 * @param[in]  ignorecase 0: case-sensitive, 1: ignore case
 * @retval     0          OK
 */
int
cligen_caseignore_set(cligen_handle h,
		      int           ignorecase)
{
    struct cligen_handle *ch = handle(h);
    
    ch->ch_ignore_case = ignorecase;
    return 0;
}

/*! Get status of cvv0 expand
 *
 * First arg of callback cvv is:
 * If 0: original string as the user typed it
 * If 1, expanded string matching keywords
 *
 * @param[in] h   CLIgen handle
 * @retval    0   cvv0 is original
 * @retval    1   cvv0 is expanded
 * @see cvec_cvv0expand
 */
int
cligen_expand_first_get(cligen_handle h)
{
    struct cligen_handle *ch = handle(h);

    return ch->ch_expand_first;
}

/*! Set status of cvv0 expand
 *
 * First arg of callback cvv is:
 * If 0: original string as the user typed it
 * If 1, expanded string matching keywords
 * @param[in]  h          CLIgen handle
 * @param[in]  cvv0expand 0: original, 1:; expanded
 * @retval     0          OK
 * @see cvec_exclude_keys
 */
int
cligen_expand_first_set(cligen_handle h,
			int           expand_first)
{
    struct cligen_handle *ch = handle(h);
    
    ch->ch_expand_first = expand_first;
    return 0;
}

/*! Changes cvec find function behaviour, exclude keywords or include them.
 * @param[in] h
 * @param[in] status
 */
int
cligen_exclude_keys_set(cligen_handle h,
			int           status)
{
    struct cligen_handle *ch = handle(h);
    
    ch->ch_exclude_keys = status;
    return 0;
}

/*! Changes cvec find function behaviour, exclude keywords or include them.
 * @param[in] h
 * @see cv_exclude_keys
 */
int
cligen_exclude_keys_get(cligen_handle h)
{
    struct cligen_handle *ch = handle(h);

    return ch->ch_exclude_keys;
}

#if 1 // XXX backward compatible
/*
 * cv_exclude_keys
 * set if you want to backward compliant: dont include keys in cgv vec to callback
 * that is, regular 'keys' and keys like: '<string keyword=foo>'
 */
static int excludekeys = 0;

/*! Changes cvec find function behaviour, exclude keywords or include them.
 * @param[in] status
 */
int
cv_exclude_keys(int status)
{
    excludekeys = status;
    return 0;
}
/*! Changes cvec find function behaviour, exclude keywords or include them.
 * @param[in] status
 */
int
cv_exclude_keys_get(void)
{
    return excludekeys;
}
#endif


/*! Set CLIgen eval wrap function to check state before and after a callback function 
 *
 * @param[in] h     CLIgen handle
 * @param[in] fn    Register function to call before and after each callback
 * @param[in] arg   Call function with this argument
 */
int
cligen_eval_wrap_fn_set(cligen_handle        h, 
			cligen_eval_wrap_fn *fn,
			void                *arg)
{
    struct cligen_handle *ch = handle(h);

    ch->ch_eval_wrap_fn = fn;
    ch->ch_eval_wrap_arg = arg;
    return 0;
}

/*! Get CLIgen eval wrap function to check state before and after a callback function 
 *
 * @param[in]  h     CLIgen handle
 * @param[out] fn    Register function to call before and after each callback
 * @param[out] arg   Call function with this argument
 */
int
cligen_eval_wrap_fn_get(cligen_handle         h,
			cligen_eval_wrap_fn **fn,
    			void                **arg)
{
    struct cligen_handle *ch = handle(h);

    if (fn)
	*fn = ch->ch_eval_wrap_fn;
    if (arg)
	*arg = ch->ch_eval_wrap_arg;
    return 0;
}
