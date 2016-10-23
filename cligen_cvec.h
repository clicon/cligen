/*
  Copyright (C) 2001-2016 Olof Hagsand

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
int     cvec_free(cvec *vr);
int     cvec_init(cvec *vr, int len);
int     cvec_reset(cvec *vr); 

int     cvec_len(cvec *vr);
cg_var *cvec_i(cvec *vr, int i);
cg_var *cvec_next(cvec *vr, cg_var *cv0);
cg_var *cvec_add(cvec *vr, enum cv_type type);
int     cvec_del(cvec *vr, cg_var *del);
cg_var *cvec_each(cvec *vr, cg_var *prev);
cg_var *cvec_each1(cvec *vr, cg_var *prev);
cvec   *cvec_dup(cvec *old);
int     cvec_match(struct cg_obj *co_match, char *cmd, cvec *vr);
cvec   *cvec_start(char *cmd);
int     cvec_print(FILE *f, cvec *vr);
int     cvec2cbuf(cbuf *cb, cvec *cvv);
cg_var *cvec_find(cvec *vr, char *name);
cg_var *cvec_find_var(cvec *vr, char *name);
cg_var *cvec_find_keyword(cvec *vr, char *name);
char   *cvec_find_str(cvec *vr, char *name);
char   *cvec_name_get(cvec *vr);
char   *cvec_name_set(cvec *vr, char *name);
int     cv_exclude_keys(int status);


#endif /* _CLIGEN_CVEC_H_ */

