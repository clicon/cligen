/*
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

#ifndef _CLIGEN_PRINT_H_
#define _CLIGEN_PRINT_H_

/*
 * Prototypes
 */
int cov2cbuf(cbuf *cb, cg_obj *co, int brief);
int cligen_print(FILE *f, parse_tree pt, int brief);

#endif /* _CLIGEN_PRINT_H_ */

