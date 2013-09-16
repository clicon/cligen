/*
  CLI generator. Take idl as input and generate a tree for use in cli.
  CVS Version: $Id: cligen_expand.h,v 1.11 2013/06/16 12:00:22 olof Exp $ 

  Copyright (C) 2001-2013 Olof Hagsand

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

#ifndef _CLIGEN_EXPAND_H_
#define _CLIGEN_EXPAND_H_

/*
 * Types
 */
/* Here we should have expand_cb but it is in cligen_gen.h */
/*
 * Expand mapping function type.
 * A function that maps from string to functions. Used when parsing a file that needs
 * to map function names (string) to actual function pointers.
 * (We may be stretching the power of C here,...)
 * cf cg_fnstype_t
 */
typedef expand_cb *(expand_str2fn_t)(char *str, void *arg, char **err);


/*
 * Prototypes
 */
int pt_expand_1(cligen_handle h, cg_obj *coprev, parse_tree *pt);
int pt_expand_2(cligen_handle h, parse_tree *pt, cvec *cvec, parse_tree *ptn, int hide);
int pt_expand_cleanup_1(parse_tree *pt);
int pt_expand_cleanup_2(parse_tree pt);
int cligen_expand_register(parse_tree pt, expand_cb *fn);
int cligen_expand_str2fn(parse_tree pt, expand_str2fn_t *str2fn, void *fnarg);
int pt_expand_add(cg_obj *co, parse_tree ptn);
int reference_path_match(cg_obj *co1, parse_tree pt0, cg_obj **co0p);
int transform_var_to_cmd(cg_obj *co, char *cmd, char *comment);

#endif /* _CLIGEN_EXPAND_H_ */

