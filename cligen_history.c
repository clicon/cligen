/*
 * Code in this file is based on:
 *
 * Copyright (C) 1991, 1992, 1993 by Chris Thewalt (thewalt@ce.berkeley.edu)
 *
 * Permission to use, copy, modify, and distribute this software
 * for any purpose and without fee is hereby granted, provided
 * that the above copyright notices appear in all copies and that both the
 * copyright notice and this permission notice appear in supporting
 * documentation.  This software is provided "as is" without express or
 * implied warranty.
 *
 * Thanks to the following people who have provided enhancements and fixes:
 *   Ron Ueberschaer, Christoph Keller, Scott Schwartz, Steven List,
 *   DaviD W. Sanderson, Goran Bostrom, Michael Gleason, Glenn Kasten,
 *   Edin Hodzic, Eric J Bivona, Kai Uwe Rommel, Danny Quah, Ulrich Betzler

 * Modifications are under:
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

 */

#include "cligen_config.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <signal.h>

#include "cligen_buf.h"
#include "cligen_cv.h"
#include "cligen_cvec.h"
#include "cligen_parsetree.h"
#include "cligen_pt_head.h"
#include "cligen_callback.h"
#include "cligen_object.h"
#include "cligen_io.h"
#include "cligen_handle.h"
#include "cligen_getline.h"
#include "cligen_handle_internal.h"
#include "cligen_history_internal.h"
#include "cligen_history.h"

/*! Makes a copy of the string
 *
 * @param[in] p     String input
 * @retval    str   Malloced copied string output
 * @retval    NULL  Error
 */
static char *
hist_save(char *p)
{
    char *s = NULL;
    int   len = strlen(p)+1;
    char *nl = strchr(p, '\n'); /* newline */

    if (nl) {
        if ((s = malloc(len)) == NULL)
            goto done;
        strcpy(s, p);
        s[len-1] = 0;
    }
    else {
        if ((s = strdup(p)) == NULL)
            goto done;
    }
 done:
    return s;
}

/*! Add a line to the CLIgen history
 *
 * @param[in] h   CLIgen handle
 * @param[in] buf String to add to history
 * @retval    0   OK
 * @retval   -1   Error
 */
int
hist_add(cligen_handle h,
         char         *buf)
{
    struct cligen_handle *ch = handle(h);
    int   retval = -1;
    char *p = buf;
    int   len;

    if (strlen(buf) >= cligen_buf_size(h))
        if (cligen_buf_increase(h, strlen(buf)) < 0)
            goto done;
    while (*p == ' ' || *p == '\t' || *p == '\n')
        p++;
    if (*p) {
        len = strlen(buf);
        if (strchr(p, '\n'))    /* previously line already has NL stripped */
            len--;
        if (ch->ch_hist_pre == 0 || strlen(ch->ch_hist_pre) != len ||
                            strncmp(ch->ch_hist_pre, buf, len) != 0) {
            if ((ch->ch_hist_buf[ch->ch_hist_last] = hist_save(buf)) == NULL)
                goto done;
            ch->ch_hist_pre = ch->ch_hist_buf[ch->ch_hist_last];
            ch->ch_hist_last = (ch->ch_hist_last + 1) % ch->ch_hist_size;
            if (ch->ch_hist_buf[ch->ch_hist_last] && *ch->ch_hist_buf[ch->ch_hist_last]) {
                free(ch->ch_hist_buf[ch->ch_hist_last]);
            }
            ch->ch_hist_buf[ch->ch_hist_last] = ""; /* NB not-malloced, check in hist_free */
        }
    }
    ch->ch_hist_cur = ch->ch_hist_last;
    retval = 0;
 done:
    return retval;
}


/*! Clear the history and deallocate all memory of the history
 *
 * @param[in] h     CLIgen handle
 */
int
hist_exit(cligen_handle h)
{
    struct cligen_handle *ch = handle(h);
    int                   i;

    for (i=0; i < ch->ch_hist_size; i++)
        if (ch->ch_hist_buf[i] && strlen(ch->ch_hist_buf[i])){
            free(ch->ch_hist_buf[i]);
            ch->ch_hist_buf[i] = NULL;
        }
    free(ch->ch_hist_buf);
    ch->ch_hist_buf = NULL;
    // done:
    return 0;
}

/*! Loads previous hist entry into input buffer, sticks on first
 *
 * @param[in] h     CLIgen handle
 */
char *
hist_prev(cligen_handle h)
{
    struct cligen_handle *ch = handle(h);
    char *p = 0;
    int   next = (ch->ch_hist_cur - 1 + ch->ch_hist_size) % ch->ch_hist_size;

    if (ch->ch_hist_buf[ch->ch_hist_cur] != 0 && next != ch->ch_hist_last) {
        ch->ch_hist_cur = next;
        p = ch->ch_hist_buf[ch->ch_hist_cur];
    }
    if (p == 0) {
        p = "";
        gl_putc('\007');
    }
    return p;
}

/* Loads next hist entry into input buffer, clears on last
 *
 * @param[in] h     CLIgen handle
 */
char *
hist_next(cligen_handle h)
{
    struct cligen_handle *ch = handle(h);
    char *p = 0;

    if (ch->ch_hist_cur != ch->ch_hist_last) {
        ch->ch_hist_cur = (ch->ch_hist_cur+1) % ch->ch_hist_size;
        p = ch->ch_hist_buf[ch->ch_hist_cur];
    }
    if (p == 0) {
        p = "";
        gl_putc('\007');
    }
    return p;
}

/*!
 * @param[in] h     CLIgen handle
 */
int
hist_pos_set(cligen_handle h,
             int           pos)
{
    struct cligen_handle *ch = handle(h);

    ch->ch_hist_cur = pos;
    return 0;
}

/*!
 * @param[in] h     CLIgen handle
 */
int
hist_pos(cligen_handle h)
{
    struct cligen_handle *ch = handle(h);

    return ch->ch_hist_cur;
}

/*!
 * @param[in] h     CLIgen handle
 */

int
hist_last_get(cligen_handle h)
{
    struct cligen_handle *ch = handle(h);

    return ch->ch_hist_last;
}

/*! Copy history line/pos to cligen buffer
 *
 * @param[in] h   CLIgen handle
 * @param[in] pos Line number to copy from history to cligen main buffer
 */
int
hist_copy(cligen_handle h,
          char         *ptr)
{
    strncpy(cligen_buf(h), ptr, cligen_buf_size(h));
    return 0;
}

/*!
 * @param[in] h     CLIgen handle
 */
int
hist_copy_prev(cligen_handle h)
{
    char *ptr = hist_prev(h);

    strncpy(cligen_buf(h), ptr, cligen_buf_size(h));
    return 0;
}

/*!
 * @param[in] h     CLIgen handle
 */
int
hist_copy_pos(cligen_handle h)
{
    struct cligen_handle *ch = handle(h);
    int pos;

    pos = hist_pos(h);
    strncpy(cligen_buf(h), ch->ch_hist_buf[pos], cligen_buf_size(h));
    return 0;
}

/*!
 * @param[in] h     CLIgen handle
 */
int
hist_copy_next(cligen_handle h)
{
    char *ptr = hist_next(h);

    strncpy(cligen_buf(h), ptr, cligen_buf_size(h));
    return 0;
}

/*---------------------------- Public API -----------------------------*/

/*! Initialize CLIgen history.
 *
 * Getline assumes there is a history, so you need to call this at time of
 * init. But you can also call it later to resize, but that will erase all
 * existing entries.
 * @param[in] h         CLIgen handle
 * @param[in] lines     Number of lines in history
 */
int
cligen_hist_init(cligen_handle h,
                 int           lines)
{
    struct cligen_handle *ch = handle(h);
    int                   retval = -1;
    int                   i;
    int                   old_size;

    if (lines < 1){
        errno = EINVAL;
        goto done;
    }
    old_size = ch->ch_hist_size;
    ch->ch_hist_size = lines+1; /* circular buffer needs an extra element */
    for (i=0; i < old_size; i++)
        if (ch->ch_hist_buf[i]){
            if (strlen(ch->ch_hist_buf[i]))
                free(ch->ch_hist_buf[i]);
            ch->ch_hist_buf[i] = NULL;
        }
    if ((ch->ch_hist_buf = (char**)realloc(ch->ch_hist_buf, ch->ch_hist_size*sizeof(char*))) == NULL)
        goto done;
    ch->ch_hist_cur = 0;
    ch->ch_hist_last = 0;
    ch->ch_hist_pre = 0;
    ch->ch_hist_buf[0] = ""; /* NB not-malloced, check in hist_free */
    for (i=1; i < ch->ch_hist_size; i++) /* reset all entries */
        ch->ch_hist_buf[i] = (char *)0;
    retval = 0;
 done:
    return retval;
}

/*! Read history entries from file
 *
 * @param[in] h  CLIgen handle
 * @param[in] f  Open file for read
 * @see cligen_hist_init must be called before
 * @note open file f instead of filename so that caller can have better error
 *       control if file not found or lacking permissions
 */
int
cligen_hist_file_load(cligen_handle h,
                      FILE         *f)
{
    int           retval = -1;
    int           ret;
    unsigned char ch;
    cbuf         *cb = NULL;

    if ((cb = cbuf_new()) == NULL)
        goto done;
    while (1){
        ret = fgetc(f);
        if (ret == EOF)
            break; /* eof or error */
        ch = (unsigned char)ret;
        if (ch == '\n'){
            if (hist_add(h, cbuf_get(cb)) < 0)
                goto done;
            cbuf_reset(cb);
        }
        else
            if (cbuf_append(cb, ch) < 0)
                goto done;
    }
    retval = 0;
 done:
    if (cb)
        cbuf_free(cb);
    return retval;
}

/*! Write history entries back to file
 *
 * @param[in] h         CLIgen handle
 * @param[in] filename  Name of history file (or NULL if no history file)
 * @see cligen_exit  Call before this function before cligen_exit
 */
int
cligen_hist_file_save(cligen_handle h,
                      FILE         *f)
{
    int                   retval = -1;
    struct cligen_handle *ch = handle(h);
    int                   i;
    char                 *line;
    int                   i1;

    /* rewind to last */
    i = (ch->ch_hist_last+1)%ch->ch_hist_size;
    while(ch->ch_hist_buf[i] == NULL)
        i = (i+1)%ch->ch_hist_size;
    i1 = ch->ch_hist_last;
    while(i != i1){
        if ((line = ch->ch_hist_buf[i]) == NULL) /* shouldnt happen */
            break;
        fprintf(f, "%s\n", line);
        i = (i+1)%ch->ch_hist_size;
    }
    retval = 0;
    return retval;
}

