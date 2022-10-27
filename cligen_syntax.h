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

*/

#ifndef _CLIGEN_SYNTAX_H_
#define _CLIGEN_SYNTAX_H_

/*
 * Types
 */

/* The following function types map from strings (function names) to actual
 * functions.  
 * These are used when parsing a file (eg CLI specification) that needs
 * to map function names (string) to actual function pointers.
 * (We may be stretching the power of C here,...)
 */

/* Map function names as strings to cligen function callback */
typedef cg_fnstype_t *(cg_str2fn_t)(char *str, void *arg, char **err); /* XXX obsolete? */

/* Map function names as strings to cligen vector function callback */
typedef cgv_fnstype_t *(cgv_str2fn_t)(char *str, void *arg, char **err);

/* Map function names as strings to CLIgen expand callback */
typedef expandv_cb *(expandv_str2fn_t)(char *str, void *arg, char **err);

/* Map function names as strings to CLIgen variable translate functions */
typedef translate_cb_t *(translate_str2fn_t)(char *str, void *arg, char **err);

/*
 * Prototypes
 */
int
cligen_parse_str(cligen_handle  h,
                 char          *str,
                 char          *name, 
                 parse_tree    *pt,
                 cvec          *globals);
int
cligen_parse_file(cligen_handle h,
                  FILE         *f,
                  char         *name, 
                  parse_tree   *obsolete,
                  cvec         *globals);

int cligen_callbackv_str2fn(parse_tree *pt, cgv_str2fn_t *str2fn, void *arg);
int cligen_expandv_str2fn(parse_tree *pt, expandv_str2fn_t *str2fn, void *arg);
int cligen_translate_str2fn(parse_tree *pt, translate_str2fn_t *str2fn, void *arg);
int cligen_parse_debug(int d); 

#endif /* _CLIGEN_SYNTAX_H_ */

