/*
  CLI generator readline. Do input processing and matching.

  Copyright (C) 2001-2014 Olof Hagsand

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
*/


#ifndef _CLIGEN_READ_H_
#define _CLIGEN_READ_H_

/*
 * Constants
 */

/*
 * Types
 */
enum cligen_result{
    CG_EOF     = -2,
    CG_ERROR   = -1,
    CG_NOMATCH = 0,
    CG_MATCH   = 1,
};

/*
 * Function Prototypes
 */
void cliread_init(cligen_handle h);
char *cliread(cligen_handle h);
void cli_trim (char **line, char comment);
int cliread_parse(cligen_handle h, char *, parse_tree *pt, cg_obj **, cvec *vr);
int cliread_eval(cligen_handle h, char **line, int *cb_ret);
int cligen_eval(cligen_handle h, cg_obj *co_match, cvec *vr);
void cligen_echo_on(void);
void cligen_echo_off(void);

#endif /* _CLIGEN_READ_H_ */
