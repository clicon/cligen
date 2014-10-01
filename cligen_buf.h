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
 *
 *
 * CLIgen dynamic buffers 
 * @code
 *   cbuf *cb;
 *   if ((cb = cbuf_new()) == NULL)
 *      err();
 *   cprintf(cb, "%d %s", 43, "go");
 *   if (write(f, cbuf_get(cb), cbuf_len(cb)) < 0)
 *      err();
 *   cbuf_free(cb);
 * @endcode
 */

#ifndef _CLIGEN_BUF_H
#define _CLIGEN_BUF_H

#include <stdarg.h> 

/*
 * Types
 */
typedef struct cbuf cbuf; /* cligen buffer type is fully defined in c-file */

/*
 * Prototypes
 */
cbuf *cbuf_new(void);
void  cbuf_free(cbuf *cb);
char *cbuf_get(cbuf *cb);
int   cbuf_len(cbuf *cb);
int   cprintf(cbuf *cb, const char *format, ...);
void  cbuf_reset(cbuf *cb);

#endif /* _CLIGEN_BUF_H */
