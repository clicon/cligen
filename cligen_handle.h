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

#ifndef _CLIGEN_HANDLE_H_
#define _CLIGEN_HANDLE_H_

/*
 * Constants
 */
#define CLIGEN_PROMPT_DEFAULT "cli> "
#define TERM_MIN_SCREEN_WIDTH 21 /* hardcoded by getline */

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

parse_tree *cligen_tree_find(cligen_handle h, char *name);
int cligen_tree_add(cligen_handle h, char *name, parse_tree pt);
int cligen_tree_del(cligen_handle h, char *name);

parse_tree *cligen_tree_each(cligen_handle h, parse_tree *pt);
parse_tree *cligen_tree_i(cligen_handle h, int i);

parse_tree *cligen_tree_active_get(cligen_handle h);
int         cligen_tree_active_set(cligen_handle h, char *name);

char *cligen_treename_keyword(cligen_handle h);
int cligen_treename_keyword_set(cligen_handle h, char *name);

/* After an evaluation (callback), which node in the parse-tree? */
cg_obj *cligen_co_match(cligen_handle h);
int cligen_co_match_set(cligen_handle h, cg_obj *co);

char *cligen_fn_str_get(cligen_handle h);
int cligen_fn_str_set(cligen_handle h, char *fn_str);

int cligen_completion(cligen_handle h);
int cligen_completion_set(cligen_handle h, int mode);

char *cligen_nomatch(cligen_handle h);
int cligen_nomatch_set(cligen_handle h, const char *fmt, ...);

int cligen_terminalrows(cligen_handle h);
int cligen_terminalrows_set(cligen_handle h, int rows);

int cligen_terminal_length(cligen_handle h);
int cligen_terminal_length_set(cligen_handle h, int length);

int cligen_line_scrolling(cligen_handle h);
int cligen_line_scrolling_set(cligen_handle h, int mode);

int cligen_tabmode(cligen_handle h);
int cligen_tabmode_set(cligen_handle h, int mode);

char *cligen_buf(cligen_handle h);
char *cligen_killbuf(cligen_handle h);

int       gl_bufsize(cligen_handle h);
int       gl_buf_init(cligen_handle h);
int       gl_buf_cleanup(cligen_handle h);
int       gl_buf_increase(cligen_handle h);

/* hack */
int cligen_parsetree_expand(cligen_handle h, parse_tree ***pt, int **e_len, int **e_i);

int cligen_lexicalorder(cligen_handle h);
int cligen_lexicalorder_set(cligen_handle h, int n);
int cligen_ignorecase(cligen_handle h);
int cligen_ignorecase_set(cligen_handle h, int n);

int cligen_logsyntax(cligen_handle h);
int cligen_logsyntax_set(cligen_handle h, int n);

void *cligen_userhandle(cligen_handle h);
int cligen_userhandle_set(cligen_handle h, void *userhandle);

/*
 * backward compatibabilty functions, consider remove
 */
#if 1
parse_tree *cligen_tree(cligen_handle h, char *name);

/* Get name of active tree back. cligen_tree_active_get() gets parse-tree which is 
   usually what you want. */
static inline char *cligen_tree_active(cligen_handle h){
    parse_tree *pt;
    if ((pt = cligen_tree_active_get(h)) != NULL)
	return pt->pt_name;
    return NULL;
}

#endif

#endif /* _CLIGEN_HANDLE_H_ */
