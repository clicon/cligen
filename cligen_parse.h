/*
  CLI generator. 

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
    char                 *ya_treename;     /* Name of syntax (for error string) */
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
