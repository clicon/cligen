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
 */

#ifndef _CLIGEN_IO_H_
#define _CLIGEN_IO_H_

/*
 * Constants
 */

#define COLUMN_WIDTH  24 /* For column formatting how many chars for command/var */

/* CLIgen event register callback type */
typedef int (cligen_fd_cb_t)(int, void*);

/*
 * Prototypes
 */
int cli_output_reset(void);
int cligen_output(FILE *f, char *templ, ... );
int cligen_regfd(int fd, cligen_fd_cb_t *cb, void *arg);
int cligen_unregfd(int fd);
void cligen_redraw(cligen_handle h);
int cligen_susp_hook(cligen_handle h, cligen_susp_cb_t *fn);
void cligen_exitchar_add(cligen_handle h, char c);
int cligen_help(FILE *f, parse_tree pt);

#endif /* _CLIGEN_IO_H_ */
