/*
  CLI generator. 

  CVS Version: $Id: cligen_parse.h,v 1.31 2013/06/20 10:42:40 olof Exp $ 

  Copyright (C) 2010-2013 Olof Hagsand

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

  Header files for yacc/lex parsing
*/

#ifndef _CLIGEN_PARSE_H_
#define _CLIGEN_PARSE_H_

/*
 * Types
 */

/*
 * Two data structures: a list and a stack.
 * The stack keeps track of syntactic levels: (decl) and [decl] so that
 * the stack is pushed and later popped  to keep track of where you are.
 * The list keeps track of the list of objects that are currently affected. Every choice and 
 * option extends the list. Operations apply to all objects on the list..
 */
struct cgy_list{
    struct cgy_list *cl_next;
    cg_obj *cl_obj;
};

struct cgy_stack{
    struct cgy_stack *cs_next;
    struct cgy_list *cs_list;  /* Pointer to a list (saved state)*/
    struct cgy_list *cs_saved; /* Saved state for options (used in pop_add) */

};


struct cligen_parse_yacc_arg{
    cligen_handle         ya_handle;       /* cligen_handle */
    char                 *ya_name;         /* Name of syntax (for error string) */
    int                   ya_linenum;      /* Number of \n in parsed buffer */
    char                 *ya_parse_string; /* original (copy of) parse string */
    void                 *ya_lexbuf;       /* internal parse buffer from lex */
    cvec                 *ya_globals;     /* global variables after parsing */
    cvec                 *ya_cvec;     /* local variables (per-command) */
    struct cgy_stack     *ya_stack;     /* Stack of levels: push/pop on () and [] */
    struct cgy_list      *ya_list;      /* (Parallel) List of objects currently 'active' */
    cg_obj               *ya_var;
    struct cg_callback   *ya_callbacks; 
    int                   ya_lex_state;  /* lex start condition (ESCAPE/COMMENT) */
    int                   ya_lex_string_state; /* lex start condition (STRING) */

};


/*
 * Variables
 */
extern char *cligen_parsetext;

/*
 * Prototypes
 */

int cgl_init(struct cligen_parse_yacc_arg *ya);
int cgl_exit(struct cligen_parse_yacc_arg *ya);

int cgy_init(struct cligen_parse_yacc_arg *ya, cg_obj *co_top);
int cgy_exit(struct cligen_parse_yacc_arg *ya);

int cligen_parselex(void *_ya);
int cligen_parseparse(void *);
void cligen_parseerror(void *_ya, char*);
int cligen_parse_debug(int d);

#endif	/* _CLIGEN_PARSE_H_ */
