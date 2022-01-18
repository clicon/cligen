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

#ifndef _CLIGEN_CALLBACK_H_
#define _CLIGEN_CALLBACK_H_

/*
 * Types
 */

/*
 * Callback function type. Is called after a specific syntax node has been identified.,
 *   arg is an optionalargument
 *   argc is number of variables (1...)
 *   argv[] is a vector of variables. The first is always the whole syntax string as entered.
 */
typedef int (cgv_fnstype_t)(cligen_handle h, cvec *vars, cvec *argv);

typedef int (cg_fnstype_t)(cligen_handle h, cvec *vars, cg_var *arg); /* XXX obsolete? */

typedef struct cg_callback cg_callback; /* struct defined in cligen_callback.c.c */

/*! A CLIgen object may have one or several callbacks. This type defines one callback
 */
struct cg_callback  { /* Linked list of command callbacks */
    struct cg_callback *cc_next;    /**< Next callback in list.  */
    cgv_fnstype_t       *cc_fn_vec;  /**< callback/function pointer using cvec.  */
    char                *cc_fn_str;  /**< callback/function name. malloced */
    cvec                *cc_cvec;    /**< callback/function arguments */
};

/*
 * Prototypes
 */
cgv_fnstype_t *co_callback_fn_get(cg_callback *cc);
int            co_callback_fn_set(cg_callback *cc, cgv_fnstype_t *fn);
cg_callback   *co_callback_next(cg_callback *cc);
int            co_callback_copy(cg_callback *cc0, cg_callback **ccn);
int            co_callbacks_free(cg_callback **ccp);
int            co_callback2cbuf(cbuf *cb, cg_callback *cc);
size_t         co_callback_size(cg_callback *cc);

#endif /* _CLIGEN_CALLBACK_H_ */

