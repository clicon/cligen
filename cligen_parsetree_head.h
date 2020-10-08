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

 * CLIgen parsetree head structure, ie the top-level structure holding all info about parse-trees
*/

#ifndef _CLIGEN_PARSETREE_HEAD_H_
#define _CLIGEN_PARSETREE_HEAD_H_

/*
 * Types
 */
/*! List of cligen parse-trees, can be searched, and activated */
typedef struct parse_tree_head  { /* Linked list of cligen parse-trees */
    struct parse_tree_head  *ph_next;
    char                    *ph_name;
    parse_tree              *ph_parsetree; /* should be free:d */
    int                      ph_active;    /* First one is active */
    cg_obj                  *ph_workpt;    /* Shortcut to "working point" cligen object, or more 
                                              specifically its parse-tree sub vector. */
} parse_tree_head;

/*
 * Prototypes
 */
char       *cligen_ph_name_get(parse_tree_head *ph);
int         cligen_ph_name_set(parse_tree_head *ph, char *name);
parse_tree *cligen_ph_parsetree_get(parse_tree_head *ph);
cg_obj     *cligen_ph_workpoint_get(parse_tree_head *ph);
int         cligen_ph_workpoint_set(parse_tree_head *ph, cg_obj *cow);

parse_tree_head *cligen_ph_find(cligen_handle h, char *name);
parse_tree *cligen_tree_workpt_pt(cligen_handle h, char *name);

int cligen_tree_add(cligen_handle h, char *name, parse_tree *pt);
int cligen_tree_del(cligen_handle h, char *name);

parse_tree_head *cligen_ph_each(cligen_handle h, parse_tree_head *ph);
parse_tree_head *cligen_ph_i(cligen_handle h, int i);

parse_tree *cligen_tree_active_get(cligen_handle h);
int         cligen_tree_active_set(cligen_handle h, char *name);

parse_tree *cligen_tree_find(cligen_handle h, char *name); /* OBSOLETE: KEEP FOR BACKWARD COMPAT */

#endif /* _CLIGEN_PARSETREE_HEAD_H_ */
