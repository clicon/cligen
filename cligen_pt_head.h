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

 * CLIgen parsetree head structure, ie the top-level structure holding all info about parse-trees
*/

#ifndef _CLIGEN_PT_HEAD_H_
#define _CLIGEN_PT_HEAD_H_

/*
 * Types
 */
typedef struct pt_head pt_head;  /* defined in cligen_handle_internal.h */


/*
 * Prototypes
 */
char       *cligen_ph_name_get(pt_head *ph);
int         cligen_ph_name_set(pt_head *ph, char *name);
parse_tree *cligen_ph_parsetree_get(pt_head *ph);
int         cligen_ph_parsetree_set(pt_head *ph, parse_tree *pt);

cg_obj     *cligen_ph_workpoint_get(pt_head *ph);
int         cligen_ph_workpoint_set(pt_head *ph, cg_obj *cow);

pt_head    *cligen_ph_find(cligen_handle h, char *name);
int         cligen_ph_free(pt_head *ph);
#ifdef NOTUSED
int         cligen_ph_del(cligen_handle h, char *name);
#endif
pt_head    *cligen_ph_add(cligen_handle h, char *name);
pt_head    *cligen_ph_each(cligen_handle h, pt_head *ph);
pt_head    *cligen_ph_i(cligen_handle h, int i);

parse_tree *cligen_pt_active_get(cligen_handle h); /* consider replace w cligen_ph_active_get */
pt_head    *cligen_ph_active_get(cligen_handle h);

int         cligen_ph_active_set_byname(cligen_handle h, char *name);

/* CLIgen callbacks */
int         cligen_wp_set(cligen_handle h, cvec *cvv, cvec *argv);
int         cligen_wp_show(cligen_handle h, cvec *cvv, cvec *argv);
int         cligen_wp_up(cligen_handle h, cvec *cvv, cvec *argv);
int         cligen_wp_top(cligen_handle h, cvec *cvv, cvec *argv);

#endif /* _CLIGEN_PT_HEAD_H_ */
