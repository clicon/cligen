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

typedef struct parse_tree parse_tree; /* struct defined internally in cligen_parsetree.c */

/* Callback for pt_apply() 
 * @param[in]  co   CLIgen parse-tree object
 * @param[in]  arg  Argument, cast to application-specific info
 * @retval     -1   Error: break and return
 * @retval     0    OK and continue
 * @retval     1    OK and return (abort iteration)
*/
typedef int (cg_applyfn_t)(cg_obj *co, void *arg);

/*
 * Prototypes
 * Note: pt_ vs cligen_parsetree_
vec_ */
int         pt_stats(parse_tree *pt, uint64_t *nrp, size_t *szp);
cg_obj     *pt_vec_i_get(parse_tree *pt, int i);
int         pt_vec_i_clear(parse_tree *pt, int i);
int         pt_vec_i_insert(parse_tree *pt, int i, cg_obj *co);
int         pt_vec_append(parse_tree *pt, cg_obj *co);
int         pt_vec_i_delete(parse_tree *pt, int i, int recurse);
int         pt_len_get(parse_tree *pt);
char       *pt_name_get(parse_tree *pt);
int         pt_name_set(parse_tree *pt, char *name);
int         pt_sets_get(parse_tree *pt);
int         pt_sets_set(parse_tree *pt, int sets);
void        cligen_parsetree_sort(parse_tree *pt, int recursive);
int         pt_realloc(parse_tree *pt);
int         pt_copy(parse_tree *pt, cg_obj *parent, uint32_t flags, parse_tree *ptn);
parse_tree *pt_dup(parse_tree *pt, cg_obj *cop, uint32_t flags);
int         cligen_parsetree_merge(parse_tree *pt0, cg_obj *parent0, parse_tree *pt1);
int         pt_free(parse_tree *pt, int recurse);
int         cligen_parsetree_free(parse_tree *pt, int recurse);
int         pt_trunc(parse_tree *pt, int len);
parse_tree *pt_new(void);
int         pt_apply(parse_tree *pt, cg_applyfn_t fn, int depth, void *arg);

#endif /* _CLIGEN_PARSETREE_H_ */

