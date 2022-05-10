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

 * Result generated when matching and used inyternally in cligen
 * This include file is used internally in cligen, not part of API
 * The C struct is not exposed outside the .c file
*/

#ifndef _CLIGEN_RESULT_H
#define _CLIGEN_RESULT_H

/*
 * Types
 */
enum cligen_result{
    CG_EOF      = -2,
    CG_ERROR    = -1,
    CG_NOMATCH  =  0,
    CG_MATCH    =  1,
    CG_MULTIPLE =  2,
};
typedef enum cligen_result cligen_result;

/* see cligen_match_result.c for struct declaration */
typedef struct match_result match_result; 

/*
 * Prototypes
 */
int   mr_pt_len_get(match_result *mr);
int   mr_pt_reset(match_result *mr);
int   mr_pt_trunc(match_result *mr, int len);
int   mr_pt_append(match_result *mr, cg_obj *co, char *token);
cg_obj *mr_pt_i_get(match_result *mr, int i);
parse_tree *mr_pt_get(match_result *mr);
char *mr_reason_get(match_result *mr);
int   mr_reason_set(match_result *mr, char *reason);
int   mr_level_get(match_result *mr);
int   mr_level_set(match_result *mr, int level);
char *mr_token_get(match_result *mr);
int   mr_last_get(match_result *mr);
int   mr_last_set(match_result *mr);
int   mr_mv_reason(match_result *from, match_result *to);
match_result *mr_new(void);
int   mr_free(match_result *mr);
cligen_result mr2result(match_result *mr);
int   mr_flags_set_co_match(match_result *mr, cg_obj *co);

#endif /* _CLIGEN_RESULT_H */
