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
int      cbuf_alloc_get(size_t *start, size_t *threshold);
int      cbuf_alloc_set(size_t start, size_t threshold);
cbuf    *cbuf_new(void);
cbuf    *cbuf_new_alloc(size_t sz);

void     cbuf_free(cbuf *cb);
char    *cbuf_get(cbuf *cb);
size_t   cbuf_len(cbuf *cb);
size_t   cbuf_buflen(cbuf *cb);
int      cprintf(cbuf *cb, const char *format, ...) __attribute__ ((format (printf, 2, 3)));
void     cbuf_reset(cbuf *cb);
int      cbuf_append(cbuf *cb, int c);
int      cbuf_append_str(cbuf *cb, char *str);
int      cbuf_append_buf(cbuf *cb, void *src, size_t n);
int      cbuf_trunc(cbuf *cb, size_t i);

#endif /* _CLIGEN_BUF_H */
