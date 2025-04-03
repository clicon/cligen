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
/* Initial alloc mem length of a cbuf, then grows exponentially, with 2*, 4*, etc
 * 1K could be a bit much for large syntaxes and small entries
 * @see cbuf_alloc_set
 */
#define CBUFLEN_START 1024
#define CBUFLEN_THRESHOLD 65536

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "cligen_buf.h"              /* External API */
#include "cligen_buf_internal.h"

/*
 * Variables
 */
/* This is how large an initial cbuf is after calling cbuf_new. Note that the cbuf
 * may grow after calls to cprintf or cbuf_alloc
 */
static size_t cbuflen_start     = CBUFLEN_START;

/* Threshold to where a cbuf grows exponentially, thereafter it grows linearly
 * If 0 continue with exponential growth
 */
static size_t cbuflen_threshold = CBUFLEN_THRESHOLD;

/*! Get global cbuf initial memory allocation size
 *
 * This is how large a cbuf is after calling cbuf_new. Note that the cbuf
 * may grow after calls to cprintf or cbuf_alloc
 * @param[out]  default   Initial default cbuf size
 * @param[out]  threshold Threshold where cbuf grows linearly instead of exponentially
 */
int
cbuf_alloc_get(size_t *start,
               size_t *threshold)
{
    *start = cbuflen_start;
    *threshold = cbuflen_threshold;
    return 0;
}

/*! Set global cbuf initial memory allocation size
 *
 * This is how large a cbuf is after calling cbuf_new. Note that the cbuf
 * may grow after calls to cprintf or cbuf_alloc
 * If 0 continue with exponential growth
 */
int
cbuf_alloc_set(size_t start,
               size_t threshold)
{
    cbuflen_start = start;
    cbuflen_threshold = threshold;
    return 0;
}

/*! Allocate cligen buffer. Returned handle can be used in sprintf calls
 *
 * which dynamically print a string.
 * The handle should be freed by cbuf_free()
 * @param[in] sz   How much buffer space for initial allocation
 * @retval    cb   The allocated object handle on success.
 * @retval    NULL Error.
 * @see cbuf_new  with auto buffer allocation
 */
cbuf *
cbuf_new_alloc(size_t sz)
{
    cbuf *cb;

    if ((cb = (cbuf*)malloc(sizeof(*cb))) == NULL)
        return NULL;
    memset(cb, 0, sizeof(*cb));
    cb->cb_buflen = sz;
    if ((cb->cb_buffer = malloc(cb->cb_buflen)) == NULL){
        free(cb);
        return NULL;
    }
    memset(cb->cb_buffer, 0, cb->cb_buflen);
    cb->cb_strlen = 0;
    return cb;
}

/*! Allocate cligen buffer with auto buffer allocation. Returned handle can be used in sprintf calls
 *
 * which dynamically print a string.
 * The handle should be freed by cbuf_free()
 * @retval cb   The allocated objecyt handle on success.
 * @retval NULL Error.
 * @see cbuf_new_alloc  with explicit buffer allocation
 */
cbuf *
cbuf_new(void)
{
    return cbuf_new_alloc(cbuflen_start);
}

/*! Free cligen buffer previously allocated with cbuf_new
 *
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
 *
 * @param[in]   cb  Cligen buffer
 */
char*
cbuf_get(cbuf *cb)
{
    return cb->cb_buffer;
}

/*! Return length of string in cligen buffer (not buffer length itself)
 *
 * @param[in]   cb  Cligen buffer
 * @see cbuf_buflen
 */
size_t
cbuf_len(cbuf *cb)
{
    return cb->cb_strlen;
}

/*! Return length of buffer itself, ie allocated bytes
 *
 * @param[in]   cb  Cligen buffer
 * @see cbuf_len
 */
size_t
cbuf_buflen(cbuf *cb)
{
    return cb->cb_buflen;
}

/*! Reset a cligen buffer. That is, restart it from scratch.
 *
 * @param[in]   cb  Cligen buffer
 */
void
cbuf_reset(cbuf *cb)
{
    cb->cb_strlen    = 0;
    cb->cb_buffer[0] = '\0';
}

/*! Internal buffer reallocator, Ensure buffer is large enough
 *
 * use quadratic expansion (2* size)
 * @param[in] cb   CLIgen buffer
 * @param[in] len  Extra length added
 */
static int
cbuf_realloc(cbuf  *cb,
             size_t sz)
{
    int retval = -1;
    int diff;

    diff = cb->cb_buflen - (cb->cb_strlen + sz + 1);
    if (diff <= 0){
        while (diff <= 0){
            if (cbuflen_threshold == 0 || cb->cb_buflen < cbuflen_threshold)
                cb->cb_buflen *= 2; /* Double the space - exponential */
            else
                cb->cb_buflen += cbuflen_threshold; /* Add - linear growth*/
            diff = cb->cb_buflen - (cb->cb_strlen + sz + 1);
        }
        if ((cb->cb_buffer = realloc(cb->cb_buffer, cb->cb_buflen)) == NULL)
            goto done;
    }
    retval = 0;
 done:
    return retval;
}

/*! Append a cligen buf by printf like semantics
 *
 * @param [in]  cb      cligen buffer allocated by cbuf_new(), may be reallocated.
 * @param [in]  format  arguments uses printf syntax.
 * @retval      See printf
 * @see cbuf_append_str for the optimized special case of string append
 * @note cprintf assume null-terminated string as %s, use cbuf_memcp for a raw interface
 */
int
cprintf(cbuf       *cb,
        const char *format, ...)
{
    int     retval = -1;
    va_list ap;
    int     len;
    int     ret;

    if (cb == NULL)
        goto ok;
    va_start(ap, format); /* dryrun */
    if ((len = vsnprintf(NULL, 0, format, ap)) < 0) /* dryrun, just get len */
        goto done;
    va_end(ap);
    /* Ensure buffer is large enough */
    if (cbuf_realloc(cb, len) < 0)
        goto done;
    va_start(ap, format); /* real */
    if ((ret = vsnprintf(cb->cb_buffer+cb->cb_strlen, /* str */
                         cb->cb_buflen-cb->cb_strlen, /* size */
                         format, ap)) < 0)
        goto done;
    va_end(ap);
    cb->cb_strlen += ret;
 ok:
    retval = 0;
 done:
    return retval;
}

int
vcprintf(cbuf       *cb,
         const char *format,
         va_list     ap)
{
    int     retval = -1;
    int     len;
    int     ret;
    va_list ap1;

    va_copy(ap1, ap);
    if (cb == NULL)
        goto ok;
    if ((len = vsnprintf(NULL, 0, format, ap)) < 0) /* dryrun, just get len */
        goto done;
    /* Ensure buffer is large enough */
    if (cbuf_realloc(cb, len) < 0)
        goto done;
    if ((ret = vsnprintf(cb->cb_buffer+cb->cb_strlen, /* str */
                         cb->cb_buflen-cb->cb_strlen, /* size */
                         format, ap1)) < 0)
        goto done;
    cb->cb_strlen += ret;
 ok:
    retval = 0;
 done:
    return retval;
}

/*! Append a string to a cbuf
  *
  * An optimized special case of cprintf
  * @param [in]  cb  cligen buffer allocated by cbuf_new(), may be reallocated.
  * @param [in]  str NULL terminated string
  * @retval      0   OK
  * @retval     -1   Error
  * @see cprintf for the generic function
  */
int
cbuf_append_str(cbuf *cb,
                const char *str)
{
    size_t  len0;
    size_t  len;

    if (str == NULL){
        errno = EINVAL;
        return -1;
    }
    len0 = strlen(str);
    len = cb->cb_strlen + len0;
    /* Ensure buffer is large enough */
    if (cbuf_realloc(cb, len) < 0)
        return -1;
    strncpy(cb->cb_buffer+cb->cb_strlen, str, len0+1);
    cb->cb_strlen = len;
    return 0;
}

/*! Append a character to a cbuf
  *
  * @param [in]  cb  cligen buffer allocated by cbuf_new(), may be reallocated.
  * @param [in]  c   character to append
  * @retval      0   OK
  * @retval     -1   Error
  * @see cbuf_append_str, use that function instead
  */
int
cbuf_append(cbuf *cb,
            int   c)
{
    char str[2] = {0,};

    str[0] = c;
    return cbuf_append_str(cb, str);
}

/*! Append a raw memory buffer and add null-termination
  *
  * A raw buffer handler to cprintf
  * @param [in]  cb  cligen buffer allocated by cbuf_new(), may be reallocated.
  * @param [in]  src Source buffer
  * @param [in]  n   Number of bytes to copy, add a null
  * @retval      0   OK
  * @retval     -1   Error
  * @see cprintf for the generic function
  */
int
cbuf_append_buf(cbuf  *cb,
                void  *src,
                size_t n)
{
    size_t  len0;
    size_t  len;

    if (src == NULL){
        errno = EINVAL;
        return -1;
    }
    len0 = cb->cb_strlen;
    len = cb->cb_strlen + n;
    /* Ensure buffer is large enough */
    if (cbuf_realloc(cb, len+1) < 0)
        return -1;
    memcpy(cb->cb_buffer+len0, src, n);
    cb->cb_buffer[len] = '\0'; /* Add a null byte */
    cb->cb_strlen = len;
    return 0;
}

/*! Truncate cligen buf to a shorther length
 *
 * @param [in]  cb  cligen buffer allocated by cbuf_new(), may be reallocated.
 * @param [in]  i   Truncate string to this length
 * @retval      0   OK
 * @retval     -1   Error
 */
int
cbuf_trunc(cbuf  *cb,
           size_t i)
{
    if (i > cb->cb_strlen){
        errno = EINVAL;
        return -1;
    }
    cb->cb_strlen = i;
    cb->cb_buffer[i] = '\0'; /* Add a null byte */
    return 0;
}
