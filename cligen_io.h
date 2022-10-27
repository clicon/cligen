/*
  CLI generator input/output support functions.

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

  * ***** END LICENSE BLOCK ***** *
 */

#ifndef _CLIGEN_IO_H_
#define _CLIGEN_IO_H_

/*
 * Constants
 */

#define COLUMN_MIN_WIDTH  21 /* For column formatting how many chars minimum 
                                for command/var */

/*
 * Types
 */
/* Struct for printing command and help */
struct cligen_help{
    char   *ch_cmd;  /* Malloced string */
    cvec   *ch_helpvec;
};

/* CLIgen event register callback type */
typedef int (cligen_fd_cb_t)(int, void*);

/*
 * Prototypes
 */
int  cli_output_reset(void);
int  cli_output_status(void);
int  cligen_output(FILE *f, const char *templ, ... ) __attribute__ ((format (printf, 2, 3)));
int  cligen_regfd(int fd, cligen_fd_cb_t *cb, void *arg);
int  cligen_unregfd(int fd);
void cligen_redraw(cligen_handle h);
int  cligen_susp_hook(cligen_handle h, cligen_susp_cb_t *fn);
int  cligen_interrupt_hook(cligen_handle h, cligen_interrupt_cb_t *fn);
void cligen_exitchar_add(cligen_handle h, char c);
int  cligen_help_eq(struct cligen_help *ch0, struct cligen_help *ch1, int help);
int  cligen_help_clear(struct cligen_help *ch0);
int  print_help_lines(cligen_handle h, FILE *fout, parse_tree *ptmatch);
int  cligen_help(cligen_handle h, FILE *f, parse_tree *pt);

#endif /* _CLIGEN_IO_H_ */
