/*
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

  ***** END LICENSE BLOCK *****

*/

#ifndef _CLIGEN_PARSETREE_H_
#define _CLIGEN_PARSETREE_H_

/*
 * Types
 */
/*! A parse tree is a top object containing a vector of parse-tree nodes 
 *
 * @code
 *   [0 1..n]
 *    | |  |
 *    v v  v
 *    o o  o  cg_obj:s
 * @endcode
 */
/* Forward declarations for cg_obj declared in cligen_object.h */
typedef struct cg_obj cg_obj;
typedef cg_obj** co_vec_t;  /* vector of (pointers to) parse-tree nodes XXX is really cg_vec */

struct parse_tree{
    struct cg_obj     **pt_vec;    /* vector of pointers to parse-tree nodes */
    int                 pt_len;    /* length of vector */
    char               *pt_name;
    char                pt_set;    /* Parse-tree is a SET */ 
};
typedef struct parse_tree parse_tree;

/* Callback for pt_apply() */
typedef int (cg_applyfn_t)(cg_obj *co, void *arg);

/*
 * Prototypes
 * Note: pt_ vs cligen_parsetree_
 */
co_vec_t    pt_vec_get(parse_tree *pt);
cg_obj     *pt_vec_i_get(parse_tree *pt, int i);
int         pt_vec_set(parse_tree *pt, co_vec_t cov);
int         pt_len_get(parse_tree *pt);
void        cligen_parsetree_sort(parse_tree *pt, int recursive);
int         pt_realloc(parse_tree *pt);
int         pt_copy(parse_tree *pt, cg_obj *parent, parse_tree *ptn);
parse_tree *pt_dup(parse_tree *pt, cg_obj *cop);
int         cligen_parsetree_merge(parse_tree *pt0, cg_obj *parent0, parse_tree *pt1);
int         pt_free(parse_tree *pt, int recurse);
int         cligen_parsetree_free(parse_tree *pt, int recurse);
parse_tree *pt_new(void);
int         pt_apply(parse_tree *pt, cg_applyfn_t fn, void *arg);

#endif /* _CLIGEN_PARSETREE_H_ */

