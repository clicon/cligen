/*
  ***** BEGIN LICENSE BLOCK *****
 
  Copyright (C) 2001-2017 Olof Hagsand

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
#include "cligen_var.h"
#include "cligen_cvec.h"
#include "cligen_gen.h"
#include "cligen_io.h"
#include "cligen_handle.h"
#include "cligen_read.h"
#include "cligen_parse.h"
#include "getline.h"

#if 0
#define handle(h) (fprintf(stderr, "%s\n", __FUNCTION__),	\
                   assert(cligen_check(h)==0),		\
                   (struct cligen_handle *)(h))
#endif

/* With sanity check */
#define handle(h) (assert(cligen_check(h)==0),(struct cligen_handle *)(h))

//#define handle(h) ((struct cligen_handle *)(h))

/*
 * CLIgen handle code.
 * Should be moved into its own, but there are some quirks with mutual dependencies
 * with rest cligen_gen.h that I didnt have time to sort out.
 */
/*
 * Constants
 */
#define TERM_ROWS_DEFAULT 24
#define GETLINE_BUFLEN_DEFAULT 64 /* startsize, increased with 2x when run out */


/*! list of cligen parse-trees, can be searched, and activated */
typedef struct parse_tree_list  { /* Linked list of cligen parse-trees */
    struct parse_tree_list  *ptl_next;
    parse_tree               ptl_parsetree; /* should be free:d */
    int                      ptl_active;    /* First one is active */
} parse_tree_list;

#define CLIGEN_MAGIC 0x56ab55aa

/* CLIgen handle. Its members should be hidden and only the typedef visible */
struct cligen_handle{
    int         ch_magic;        /* magic */
    char        ch_exiting;      /* Set by callback to request exit of CLIgen */
    char        ch_comment;      /* comment sign - everything behind it is ignored */
    char       *ch_prompt;       /* current prompt used */
    parse_tree_list *ch_tree;         /* Linked list of parsetrees */
    char       *ch_treename_keyword; /* Name of treename parsing keyword */
    cg_obj     *ch_co_match;     /* Matching object in latest evaluation */
    char       *ch_fn_str;       /* Name of active callback function */
    int         ch_completion;   /* completion mode */    
    char       *ch_nomatch;      /* Why did a string not match an evaluation? */
    int         ch_terminal_rows; /* Number of output terminal rows */
    int         ch_terminal_length; /* Length of terminal row */
    int         ch_tabmode;      /* short or long output mode on TAB */

    int         ch_lexicalorder; /* strcmp (0) or strverscmp (1) syntax order.
                                    Also, this is global for now */
    int         ch_ignorecase; /* dont care about aA (0), care about aA (1) 
				     does not work if lexicalorder is set.
				     Also this is global for now
				  */

    char       *ch_buf;          /* getline input buffer */
    char       *ch_killbuf;      /* getline killed text */

    int         ch_logsyntax;    /* Debug syntax by printing dynamically on stderr */
    void       *ch_userhandle;   /* Use this as app-specific callback handle */
    void       *ch_userdata;     /* application-specific data (any data) */
};

/*! Get window size and set terminal row size
 */
static int
cligen_gwinsz(cligen_handle h)
{
    struct winsize ws;

    if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) == -1)
	return -1;
    cligen_terminalrows_set(h, ws.ws_row);
    cligen_terminal_length_set(h, ws.ws_col);
    return 0;
}

void
sigwinch_handler(int arg)
{
}

/*! This is the first call the CLIgen API and returns a handle. 
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
    h = (cligen_handle)ch;
    cligen_prompt_set(h, CLIGEN_PROMPT_DEFAULT);
    if (cligen_gwinsz(h) < 0)
	return NULL;
    cligen_interrupt_hook(h, cligen_gwinsz);
    memset(&sigh, 0, sizeof(sigh));
    sigh.sa_handler = sigwinch_handler;
    if (sigaction(SIGWINCH, &sigh, NULL) < 0){
	perror("sigaction");
	return NULL;
    }
    cliread_init(h);
    cligen_buf_init(h);

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
    parse_tree_list *ptl;

    gl_histclear();
    cligen_buf_cleanup(h);
    if (ch->ch_prompt)
	free(ch->ch_prompt);
    if (ch->ch_nomatch)
	free(ch->ch_nomatch);  
    if (ch->ch_treename_keyword)
	free(ch->ch_treename_keyword);
    if (ch->ch_fn_str)
	free(ch->ch_fn_str);
    while ((ptl = ch->ch_tree) != NULL){
	ch->ch_tree =  ptl->ptl_next;
	cligen_parsetree_free(ptl->ptl_parsetree, 1);
	free(ptl);
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

/*! Find a parsetree by its name, if name==NULL, return first parse-tree
 * @param[in] h       CLIgen handle
 * @param[in] name    Name of tree
 * Note name of parse-tree is assigned when you do cligen_tree_add
 */
parse_tree *
cligen_tree_find(cligen_handle h, 
		 char         *name)
{
    parse_tree_list      *ptl;
    parse_tree           *pt;
    struct cligen_handle *ch = handle(h);

    for (ptl = ch->ch_tree; ptl; ptl = ptl->ptl_next){
	pt = &ptl->ptl_parsetree;
	if (strcmp(pt->pt_name, name) == 0)
	    return pt;
    }
    return NULL;
}

/*! Add a new parsetree last in list
 * @param[in] h       CLIgen handle
 * @param[in] name    name of parse-tree
 * @param[jn] pt      parse-tree passed by value(!)
 * Note, if this is the first tree, it is activated by default
 */
int 
cligen_tree_add(cligen_handle h, 
		char         *name, 
		parse_tree    pt)
{
    parse_tree_list           *ptl;
    parse_tree_list           *ptlast;
    parse_tree                *ptn;
    struct cligen_handle *ch = handle(h);

    if ((ptl = (parse_tree_list *)malloc(sizeof(*ptl))) == NULL){
	fprintf(stderr, "%s malloc: %s\n", __FUNCTION__, strerror(errno));
	return -1;
    }
    memset(ptl, 0, sizeof(*ptl));
    ptl->ptl_parsetree = pt;
    ptn =  &ptl->ptl_parsetree;
    if ((ptn->pt_name = strdup(name)) == NULL){
	fprintf(stderr, "%s strdup: %s\n", __FUNCTION__, strerror(errno));
	return -1;
    }
    if ((ptlast = ch->ch_tree) == NULL){
	ptl->ptl_active++;
	ch->ch_tree = ptl;
    }
    else {
	while (ptlast->ptl_next)
	    ptlast = ptlast->ptl_next;
	ptlast->ptl_next = ptl;
    }
    return 0;
}

/*! Delete a parsetree list entry not parsetree itself
 * @param[in] h       CLIgen handle

 */
int 
cligen_tree_del(cligen_handle h, 
		char         *name)
{
    parse_tree_list  *ptl;
    parse_tree_list **ptl_prev;
    parse_tree       *pt;
    struct cligen_handle *ch = handle(h);

    for (ptl_prev = &ch->ch_tree, ptl = *ptl_prev; 
	 ptl; 
	 ptl_prev = &ptl->ptl_next, ptl = ptl->ptl_next){
	pt = &ptl->ptl_parsetree;
	if (strcmp(pt->pt_name, name) == 0){
	    *ptl_prev = ptl->ptl_next;
	    free(ptl);
	    break;
	}
    }
    return 0;
}

/*! Iterate through all parsed cligen trees 
 *
 * @param[in] h   CLIgen handle
 * @param[in] pt  Cligen parse-tree iteration variable. Must be initialized to NULL
 * @retval pt     Next parse-tree structure.
 * @retval NULL   When end of list reached.
 * @code
 *    parse_tree *pt = NULL;
 *    while ((pt = cligen_tree_each(h, pt)) != NULL) {
 *	     ...
 *    }
 * @endcode
 * Note: you may not delete (or add) parse-trees while iterating through them
 * Note: the list contains all parse-trees added by cligen_tree_add()
 */
parse_tree *
cligen_tree_each(cligen_handle h, 
		 parse_tree   *pt0)
{
    struct cligen_handle *ch = handle(h);
    parse_tree_list      *ptl;
    parse_tree           *pt;
    int                   next = 0;

    for (ptl = ch->ch_tree; ptl; ptl = ptl->ptl_next){
	pt = &ptl->ptl_parsetree;
	if (pt0 == NULL || next)
	    return pt; /* Initial */
	if (pt0->pt_vec == pt->pt_vec)
	    next++;
    }
    return NULL;
}

/*! Return i:th parse-tree of parsed cligen trees 
 * @param[in] h  CLIgen handle
 * @param[in] i  Order of element to get
 */
parse_tree *
cligen_tree_i(cligen_handle h, 
	      int           i0)
{
    struct cligen_handle *ch = handle(h);
    parse_tree_list      *ptl;
    int                   i;

    for (ptl = ch->ch_tree, i=0; ptl; ptl = ptl->ptl_next, i++)
	return &ptl->ptl_parsetree;
    return NULL;
}

/*! Get name of currently active parsetree.
 * @param[in] h       CLIgen handle
 */
parse_tree *
cligen_tree_active_get(cligen_handle h)
{
    struct cligen_handle *ch = handle(h);
    parse_tree_list      *ptl;

    for (ptl = ch->ch_tree; ptl; ptl = ptl->ptl_next)
	if (ptl->ptl_active)
	    return &ptl->ptl_parsetree;
    return NULL;
}

/*! Set currently active parsetree by name
 * @param[in] h       CLIgen handle
 * @retval -1     Parse-tree not found, active tree not changed
 * @retval  1     Parse-tree found and set as active.
 * If parse-tree not found all are inactivated.
 */
int
cligen_tree_active_set(cligen_handle h, 
		       char         *name)
{
    struct cligen_handle *ch = handle(h);
    int                   retval = -1;
    parse_tree_list      *ptl;
    parse_tree           *pt = NULL;

    /* First see if there is such a tree, and set it */
    for (ptl = ch->ch_tree; ptl; ptl = ptl->ptl_next){
	pt = &ptl->ptl_parsetree;
	if (strcmp(name, pt->pt_name) == 0){
	    ptl->ptl_active = 1;
	    break;
	}
    }
    if (ptl != NULL){
	/*  Then reset all other trees */
	for (ptl = ch->ch_tree; ptl; ptl = ptl->ptl_next){
	    pt = &ptl->ptl_parsetree;
	    if (strcmp(name, pt->pt_name) != 0)
		ptl->ptl_active = 0;
	}
	retval = 0;
    }
    return retval;
}

#define TREENAME_KEYWORD_DEFAULT "treename"

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



/*! Get completion mode. 0: complete 1 level. 1: complete all
 *
 * Example: syntax is 'a b;'. mode = 0 gives completion to 'a ' on first TAB and 
 * to 'a b ' on second. mode = 1 gives completion to 'a b ' on first TAB.
 *
 * @param[in] h       CLIgen handle
 * @retval 0   for each TAB complete one level. (default)
 * @retval 1   complete all unique levels at once
 */
int
cligen_completion(cligen_handle h)
{
    struct cligen_handle *ch = handle(h);

    return ch->ch_completion;
}

/*! Set completion mode. 
 *
 * @param[in] h       CLIgen handle
 * @param[in] mode    0: complete 1 level. 1: complete all
 */
int
cligen_completion_set(cligen_handle h, 
		      int           mode)
{
    struct cligen_handle *ch = handle(h);

    ch->ch_completion = mode;
    return 0;
}

/*! Get Error string explaining why there was no match.
 *
 * Fill error string buffer
 * Why is there no match of an input string in the parse-tree?
 * The call to cliread_parse/getline/eval returns CG_NOMATCH and this is where
 * the reason is stored.
 * @param[in] h       CLIgen handle
 */
char *
cligen_nomatch(cligen_handle h)
{
    struct cligen_handle *ch = handle(h);

    return ch->ch_nomatch;
}

/*! Set Error string explaining why there was no match.
 * @param[in] h       CLIgen handle
 * @param[in] fmt     Format string, printf style followed by arguments
 */
int
cligen_nomatch_set(cligen_handle h, 
		   const char   *fmt, ...)
{
    struct cligen_handle *ch = handle(h);
    int res;
    int len;
    va_list ap;
	
    if (fmt == NULL){
	if (ch->ch_nomatch){
	    free(ch->ch_nomatch);
	    ch->ch_nomatch = NULL;
	}
	return 0;
    }
    assert(ch->ch_nomatch == NULL);
    va_start (ap, fmt);
    len = vsnprintf(NULL, 0, fmt, ap);
    len++;
    va_end (ap);
    if ((ch->ch_nomatch = malloc(len)) == NULL){
	fprintf(stderr, "%s: malloc: %s\n", __FUNCTION__, strerror(errno));
	return -1;
    }
    va_start (ap, fmt);
    res = vsnprintf(ch->ch_nomatch, len, fmt, ap);
    va_end (ap);
    
    return res;	
}

static int _terminalrows; /* XXX: global since cli_output dont take handle */

/*! Get number of displayed terminal rows.
 * @param[in] h       CLIgen handle
 */
int 
cligen_terminalrows(cligen_handle h)
{
//    struct cligen_handle *ch = handle(h);

    return _terminalrows; /* ch->ch_terminalrows; */
}

/*! Set number of displayed terminal rows.
 * @param[in] h       CLIgen handle
 * @param[in] rows    Number of lines in a terminal (y-direction)
 */
int 
cligen_terminalrows_set(cligen_handle h, 
			int           rows)
{
    struct cligen_handle *ch = handle(h);

    ch->ch_terminal_rows = rows;
    _terminalrows = rows;
    return 0;
}

/*! Get length of lines (number of 'columns' in a line)
 *
 * @param[in] h       CLIgen handle
 */
int 
cligen_terminal_length(cligen_handle h)
{
//    struct cligen_handle *ch = handle(h);

    return gl_getwidth();
}

/*! Set length of lines (number of 'columns' in a line)
 *
 * @param[in] h       CLIgen handle
 * @param[in] length  Number of characters in a line - x-direction
 */
int 
cligen_terminal_length_set(cligen_handle h, 
			   int           length)
{
    struct cligen_handle *ch = handle(h);

    ch->ch_terminal_length = length;
    if (length < TERM_MIN_SCREEN_WIDTH)
	length = TERM_MIN_SCREEN_WIDTH;
    gl_setwidth(length);
    return 0;
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


/*! Get tab-mode. 
 *
 * @param[in] h       CLIgen handle
 * @retval 0    'short/ios' mode.
 * @retval 1    long/junos mode.
 */
int 
cligen_tabmode(cligen_handle h)
{
    struct cligen_handle *ch = handle(h);

    return ch->ch_tabmode;
}

/*! Set tab-mode
 *
 * @param[in] h       CLIgen handle
 * @param[in] mode    0 is 'short/ios' mode, 1 is long/junos mode.
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
 * @param[in] h       CLIgen handle
 * @param n  strcmp (0) or strverscmp (1).
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

static int _getline_bufsize = GETLINE_BUFLEN_DEFAULT;

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

/*!
 * @param[in] h       CLIgen handle
 */
int 
cligen_buf_size(cligen_handle h)
{
    return _getline_bufsize;
}

/*!
 * @param[in] h       CLIgen handle
 */
int
cligen_buf_init(cligen_handle h)
{
    struct cligen_handle *ch = handle(h);

    if ((ch->ch_buf = malloc(_getline_bufsize)) == NULL){
	perror("malloc");
	return -1;
    }
    memset(ch->ch_buf, 0, _getline_bufsize);
    if ((ch->ch_killbuf = malloc(_getline_bufsize)) == NULL){
	perror("malloc");
	return -1;
    }
    memset(ch->ch_killbuf, 0, _getline_bufsize);
    return 0;
}

/*!
 * @param[in] h       CLIgen handle
 */
int       
cligen_buf_increase(cligen_handle h)
{
    struct cligen_handle *ch = handle(h);
    int len0 = _getline_bufsize;

    _getline_bufsize *= 2;
    if ((ch->ch_buf = realloc(ch->ch_buf, _getline_bufsize)) == NULL)
	return -1;
    memset(ch->ch_buf+len0, 0, _getline_bufsize-len0);
    if ((ch->ch_killbuf = realloc(ch->ch_killbuf, _getline_bufsize)) == NULL)
	return -1;
    memset(ch->ch_killbuf+len0, 0, _getline_bufsize-len0);
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

/*
 * backward compatibabilty functions, consider remove
 */
#if 1 
/* note cant inline since it may be used in plugins 
 * @param[in] h       CLIgen handle
 */
parse_tree *cligen_tree(cligen_handle h, 
			char         *name)
{
    return cligen_tree_find(h, name);
}
#endif
