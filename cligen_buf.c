/*
  ***** BEGIN LICENSE BLOCK *****
 
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

 * CLIgen dynamic buffers 
 * @code
 *   cbuf *cb;
 *   if ((cb = cbuf_new()) == NULL)
 *      err();
 *   cprintf(cb, "content %d", 42);
 *   if (write(f, cbuf_get(cb), cbuf_len(cb)) < 0)
 *      err();
 *   cbuf_free(cb);
 * @endcode
 */

/*
 * Constants
 */
#define CBUFLEN_DEFAULT 1024   /* Start alloc mem length 1K */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "cligen_buf.h"              /* External API */
#include "cligen_buf_internal.h"            

/*
 * Variables
 */
/* This is how large a cbuf is after calling cbuf_new. Note that the cbuf 
 * may grow after calls to cprintf or cbuf_alloc */
static uint32_t cbuflen_alloc = CBUFLEN_DEFAULT;

/*! Get cbuf initial memory allocation size
 * This is how large a cbuf is after calling cbuf_new. Note that the cbuf 
 * may grow after calls to cprintf or cbuf_alloc
 */
uint32_t
cbuf_alloc_get(void)
{
    return cbuflen_alloc;
}

/*! Set cbuf initial memory allocation size
 * This is how large a cbuf is after calling cbuf_new. Note that the cbuf 
 * may grow after calls to cprintf or cbuf_alloc
 */
int
cbuf_alloc_set(uint32_t alloc)
{
    cbuflen_alloc = alloc;
    return 0;
}

/*! Allocate cligen buffer. The handle returned can be used in  successive sprintf calls
 * which dynamically print a string.
 * The handle should be freed by cbuf_free()
 * @retval cb   The allocated objecyt handle on success.
 * @retval NULL Error.
 */
cbuf *
cbuf_new(void)
{
    cbuf *cb;

    if ((cb = (cbuf*)malloc(sizeof(*cb))) == NULL)
	return NULL;
    memset(cb, 0, sizeof(*cb));
    if ((cb->cb_buffer = malloc(cbuflen_alloc)) == NULL)
	return NULL;
    cb->cb_buflen = cbuflen_alloc;
    memset(cb->cb_buffer, 0, cb->cb_buflen);
    cb->cb_strlen = 0;
    return cb;
}

/*! Free cligen buffer previously allocated with cbuf_new
 * @param[in]   cb  Cligen buffer
 */
void
cbuf_free(cbuf *cb)
{
    if (cb) {
	if (cb->cb_buffer)
	    free(cb->cb_buffer);
	free(cb);
    }
}

/*! Return actual byte buffer of cligen buffer
 * @param[in]   cb  Cligen buffer
 */
char*
cbuf_get(cbuf *cb)
{
    return cb->cb_buffer;
}

/*! Return length of string in cligen buffer (not buffer length itself)
 * @param[in]   cb  Cligen buffer
 * @see cbuf_buflen
 */
int
cbuf_len(cbuf *cb)
{
    return cb->cb_strlen;
}

/*! Return length of buffer itself, ie allocated bytes
 * @param[in]   cb  Cligen buffer
 * @see cbuf_len
 */
int
cbuf_buflen(cbuf *cb)
{
    return cb->cb_buflen;
}

/*! Reset a cligen buffer. That is, restart it from scratch.
 * @param[in]   cb  Cligen buffer
 */
void
cbuf_reset(cbuf *cb)
{
    cb->cb_strlen    = 0; 
    cb->cb_buffer[0] = '\0'; 
}

/*! Append a cligen buf by printf like semantics
 * 
 * @param [in]  cb      cligen buffer allocated by cbuf_new(), may be reallocated.
 * @param [in]  format  arguments uses printf syntax.
 * @retval      See printf
 */
int
cprintf(cbuf       *cb, 
	const char *format, ...)
{
    va_list ap;
    int diff;
    int retval;

    va_start(ap, format);
  again:
    if (cb == NULL)
	return 0;
    retval = vsnprintf(cb->cb_buffer+cb->cb_strlen, 
		       cb->cb_buflen-cb->cb_strlen, 
		       format, ap);
    if (retval < 0)
	return -1;
    diff = cb->cb_buflen - (cb->cb_strlen + retval + 1);
    if (diff <= 0){
	cb->cb_buflen *= 2; /* Double the space - alt linear growth */
	if ((cb->cb_buffer = realloc(cb->cb_buffer, cb->cb_buflen)) == NULL)
	    return -1;
	va_end(ap);
	va_start(ap, format);
	goto again;
    }
    cb->cb_strlen += retval;

    va_end(ap);
    return retval;
}

/*! Append a character to a cbuf
  *
  * @param [in]  cb      cligen buffer allocated by cbuf_new(), may be reallocated.
  * @param [in]  c       character to append
  */
int
cbuf_append(cbuf       *cb,
            int        c)
{
    int diff;

    /* make sure we have enough space */
    diff = cb->cb_buflen - (cb->cb_strlen + 1);
    if (diff <= 0) {
	cb->cb_buflen *= 2;
	if ((cb->cb_buffer = realloc(cb->cb_buffer, cb->cb_buflen)) == NULL) {
	    return -1;
	}
    }

    cb->cb_buffer[cb->cb_strlen++] = c;
    cb->cb_buffer[cb->cb_strlen] = 0;

    return 0;
}
