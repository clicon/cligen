/*
  CLI generator input/output support functions.

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

  This file includes utility functions (good-to-have) for CLIgen applications
 */

#ifndef _CLIGEN_UTIL_H_
#define _CLIGEN_UTIL_H_

/*
 * Prototypes
 */
int cvec_add_string(cvec *cvv, char *name, char *val);

int cligen_loop(cligen_handle h);

#endif /* _CLIGEN_UTIL_H_ */
