/*
  CVS Version: $Id: cligen_handle.h,v 1.17 2013/06/16 12:01:44 olof Exp $ 

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

#ifndef _CLIGEN_HANDLE_H_
#define _CLIGEN_HANDLE_H_

/*
 * Constants
 */
#define CLIGEN_PROMPT_DEFAULT "cli> "

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

parse_tree *cligen_tree(cligen_handle h, char *name);
parse_tree *cligen_treetop(cligen_handle h, char *name);
int cligen_tree_add(cligen_handle h, char *name, parse_tree pt);
int cligen_tree_del(cligen_handle h, char *name);

/* A mode is really a parse-tree */
char *cligen_tree_active(cligen_handle h);
int cligen_tree_active_set(cligen_handle h, char *mode);

/* After an evaluation (callback), which node in the parse-tree? */
cg_obj *cligen_co_match(cligen_handle h);
int cligen_co_match_set(cligen_handle h, cg_obj *co);

char *cligen_fn_str_get(cligen_handle h);
int cligen_fn_str_set(cligen_handle h, char *fn_str);

//#ifdef CLIGEN_SUBMODE
parse_tree *cligen_submode(cligen_handle h, char *name);
int cligen_submode_set(cligen_handle h, char *name, parse_tree *pt);
//#endif

int cligen_completion(cligen_handle h);
int cligen_completion_set(cligen_handle h, int mode);

char *cligen_nomatch(cligen_handle h);
int cligen_nomatch_set(cligen_handle h, const char *fmt, ...);

int cligen_terminalrows(cligen_handle h);
int cligen_terminalrows_set(cligen_handle h, int rows);

int cligen_terminal_length(cligen_handle h);
int cligen_terminal_length_set(cligen_handle h, int length);

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

#endif /* _CLIGEN_HANDLE_H_ */
