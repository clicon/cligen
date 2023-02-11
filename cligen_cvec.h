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


  CLIgen variable vectors - cvec
*/

#ifndef _CLIGEN_CVEC_H_
#define _CLIGEN_CVEC_H_

/*
 * CLIgen variable record that encapsulates parsed variable vectors, etc.
 * Never use these fields directly.
 * Defined internally in cligen_cvec.c
 */
typedef struct cvec cvec;


/*
 * Prototypes
 */
cvec   *cvec_new(int len);
cvec   *cvec_from_var(cg_var *cv);
int     cvec_free(cvec *vr);
int     cvec_init(cvec *vr, int len);
int     cvec_reset(cvec *vr); 
int     cvec_len(cvec *vr);
cg_var *cvec_i(cvec *vr, int i);
char   *cvec_i_str(cvec *cvv, int i);
cg_var *cvec_next(cvec *vr, cg_var *cv0);
cg_var *cvec_add(cvec *vr, enum cv_type type);
cg_var *cvec_append_var(cvec *cvv, cg_var *var);
int     cvec_del(cvec *vr, cg_var *del);
int     cvec_del_i(cvec *vr, int ix);
cg_var *cvec_each(cvec *vr, cg_var *prev);
cg_var *cvec_each1(cvec *vr, cg_var *prev);
cvec   *cvec_dup(cvec *old);
cvec   *cvec_start(char *cmd);
int     cvec_print(FILE *f, cvec *vr);
int     cvec2cbuf(cbuf *cb, cvec *cvv);
cg_var *cvec_find(cvec *vr, const char *name);
cg_var *cvec_find_keyword(cvec *vr, const char *name);
cg_var *cvec_find_var(cvec *vr, const char *name);
char   *cvec_find_str(cvec *vr, const char *name);
char   *cvec_name_get(cvec *vr);
char   *cvec_name_set(cvec *vr, const char *name);
size_t  cvec_size(cvec *cvv);
int     cligen_txt2cvv(char *str, cvec **cvp);
int     cligen_str2cvv(char *string, cvec **cvp, cvec **cvr);
int     cvec_expand_first(cvec *cvv);
int     cvec_exclude_keys(cvec *cvv);

#endif /* _CLIGEN_CVEC_H_ */

