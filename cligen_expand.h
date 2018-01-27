/*
  CLI generator. Take idl as input and generate a tree for use in cli.

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

*/

#ifndef _CLIGEN_EXPAND_H_
#define _CLIGEN_EXPAND_H_

/*
 * Types
 */
/* Here we should have expandv_cb but it is in cligen_gen.h */

/* backward compat macros */
#define pt_expand_1(h, coprev, pt) pt_expand_treeref(h, coprev, pt)
#define pt_expand_cleanup_1(pt) pt_expand_treeref_cleanup(pt)

/*
 * Prototypes
 */
int pt_expand_treeref(cligen_handle h, cg_obj *coprev, parse_tree *pt);
int pt_expand_2(cligen_handle h, parse_tree *pt, cvec *cvec, parse_tree *ptn, int hide);
int pt_expand_treeref_cleanup(parse_tree *pt);
int pt_expand_cleanup_2(parse_tree pt);
int pt_expand_add(cg_obj *co, parse_tree ptn);
int reference_path_match(cg_obj *co1, parse_tree pt0, cg_obj **co0p);
int transform_var_to_cmd(cg_obj *co, char *cmd, char *comment);

#endif /* _CLIGEN_EXPAND_H_ */

