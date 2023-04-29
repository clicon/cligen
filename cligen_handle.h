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

#ifndef _CLIGEN_HANDLE_H_
#define _CLIGEN_HANDLE_H_

/*
 * Constants
 */
#define CLIGEN_PROMPT_DEFAULT "cli> "
#define TERM_MIN_SCREEN_WIDTH 21 /* hardcoded by getline */
#define CLIGEN_HISTSIZE_DEFAULT 100 /* default size of cli history (lines) */

/* OR CLIGEN_TABMODE_* using cligen_tabmode_set() */
/* Show columns info: 0: short/ios mode, 1: long/junos mode */
#define CLIGEN_TABMODE_COLUMNS 0x01 

/* Command keyword preference over variables:
 * 0: command completion preference, 
 * 1: same preference for vars
 */
#define CLIGEN_TABMODE_VARS    0x02

/* Steps to complete
 * 0: complete single step. 1: complete all steps at once
 */
#define CLIGEN_TABMODE_STEPS    0x04

/*
 * Types
 */
/*! CLIgen eval wrap function to check state before and after a callback function 
 *
 * @param[in]  arg   Argument to wrap function
 * @param[in]  wh    Wrap handle. If NULL: init, othewise compare present state with wh
 * @param[in]  name  A name for logging
 * @param[in]  fn    A function name for loggin
 * @retval      1    OK
 * @retval      0    Fail (for info only, cligen_eval does not handle fails)
 * @retval     -1    Error
 * @code
 *  void *wh = NULL;
 *  cligen_eval_wrap_cb(h, &wh, "myfn", __FUNCTION__);
 *  .. User callback
 *  cligen_eval_wrap_cb(h, &wh, "myfn", __FUNCTION__);
 * See cligen_eval
 */
typedef int (cligen_eval_wrap_fn)(void *arg, void **wh, const char *name, const char *fn);

/*! CLIgen wrap function for making treeref lookup
 *
 * This adds an indirection based on name and context
 * @param[in]  h     CLIgen handle
 * @param[in]  name  Base tree name
 * @param[in]  cvt   Tokenized string: vector of tokens providing some context
 * @param[in]  arg   Argument given when registering wrap function (maybe not needed?)
 * @param[out] namep New (malloced) name
 * @retval     0     OK
 * @retval    -1     Error
 */
typedef int (cligen_tree_resolve_wrapper_fn)(cligen_handle h, char *name, cvec *cvt, void *arg, char **namep);

/*
 * Prototypes
 */
cligen_handle cligen_init(void);
int cligen_exit(cligen_handle);
int cligen_check(cligen_handle h);

int cligen_exiting(cligen_handle h);
int cligen_exiting_set(cligen_handle h, int status);

char cligen_comment(cligen_handle h);
int cligen_comment_set(cligen_handle h, char c);

char* cligen_prompt(cligen_handle h);
int cligen_prompt_set(cligen_handle h, char *prompt);

pt_head *cligen_pt_head_get(cligen_handle h);
int cligen_pt_head_set(cligen_handle h, pt_head *ph);

pt_head *cligen_pt_head_active_get(cligen_handle h);
int cligen_pt_head_active_set(cligen_handle h, pt_head *ph);

char *cligen_treename_keyword(cligen_handle h);
int cligen_treename_keyword_set(cligen_handle h, char *name);

/* After an evaluation (callback), which node in the parse-tree? */
cg_obj *cligen_co_match(cligen_handle h);
int cligen_co_match_set(cligen_handle h, cg_obj *co);

char *cligen_fn_str_get(cligen_handle h);
int cligen_fn_str_set(cligen_handle h, char *fn_str);

int cligen_terminal_rows(cligen_handle h);
int cligen_terminal_rows_set(cligen_handle h, int rows);

int cligen_terminal_width(cligen_handle h);
int cligen_terminal_width_set(cligen_handle h, int length);

int cligen_utf8_get(cligen_handle h);
int cligen_utf8_set(cligen_handle h, int mode);

int cligen_line_scrolling(cligen_handle h);
int cligen_line_scrolling_set(cligen_handle h, int mode);

int cligen_helpstring_truncate(cligen_handle h);
int cligen_helpstring_truncate_set(cligen_handle h, int mode);

int cligen_helpstring_lines(cligen_handle h);
int cligen_helpstring_lines_set(cligen_handle h, int lines);

int cligen_tabmode(cligen_handle h);
int cligen_tabmode_set(cligen_handle h, int mode);

char *cligen_killbuf(cligen_handle h);

char *cligen_buf(cligen_handle h);
int   cligen_buf_size(cligen_handle h);
int   cligen_killbuf_size(cligen_handle h);
int   cligen_buf_init(cligen_handle h);
int   cligen_buf_cleanup(cligen_handle h);
int   cligen_buf_increase(cligen_handle h, size_t size);
int   cligen_killbuf_increase(cligen_handle h, size_t size);

/* hack */
int   cligen_parsetree_expand(cligen_handle h, parse_tree ***pt, int **e_len, int **e_i);

int   cligen_lexicalorder(cligen_handle h);
int   cligen_lexicalorder_set(cligen_handle h, int n);
int   cligen_ignorecase(cligen_handle h);
int   cligen_ignorecase_set(cligen_handle h, int n);

int   cligen_logsyntax(cligen_handle h);
int   cligen_logsyntax_set(cligen_handle h, int n);

void *cligen_userhandle(cligen_handle h);
int   cligen_userhandle_set(cligen_handle h, void *userhandle);

int   cligen_regex_xsd(cligen_handle h);
int   cligen_regex_xsd_set(cligen_handle h, int mode);

char  cligen_delimiter(cligen_handle h);
int   cligen_delimiter_set(cligen_handle h, char delimiter);

int   cligen_preference_mode(cligen_handle h);
int   cligen_preference_mode_set(cligen_handle h, int flag);

int   cligen_caseignore_get(cligen_handle h);
int   cligen_caseignore_set(cligen_handle h, int ignorecase);

int   cligen_expand_first_get(cligen_handle h);
int   cligen_expand_first_set(cligen_handle h, int cvv0expand);

int   cligen_exclude_keys_set(cligen_handle h, int status);
int   cligen_exclude_keys_get(cligen_handle h);

/* XXX backward compatible, use cligen_ variant below instead */
int   cv_exclude_keys(int status);
int   cv_exclude_keys_get(void);

int   cligen_eval_wrap_fn_set(cligen_handle h, cligen_eval_wrap_fn *fn, void *arg);
int   cligen_eval_wrap_fn_get(cligen_handle h, cligen_eval_wrap_fn **fn, void **arg);

int   cligen_tree_resolve_wrapper_set(cligen_handle h, cligen_tree_resolve_wrapper_fn *fn, void *arg);
int   cligen_tree_resolve_wrapper_get(cligen_handle h, cligen_tree_resolve_wrapper_fn **fn, void **arg);

#endif /* _CLIGEN_HANDLE_H_ */
