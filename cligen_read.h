/*
  CLI generator readline. Do input processing and matching.

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


#ifndef _CLIGEN_READ_H_
#define _CLIGEN_READ_H_

/*
 * Constants
 */

/*
 * Function Prototypes
 */
void cliread_init(cligen_handle h);
int  cliread(cligen_handle h, char **strinpg);
void cli_trim (char **line, char comment);
int  cliread_parse(cligen_handle h, char *, parse_tree *pt, cg_obj **,
                   cvec **cvvp, cg_callback **callbacks, cligen_result *result, char **reason);
int  cliread_eval(cligen_handle h, char **line, int *cb_ret, cligen_result *result, char **reason);
int  cligen_eval(cligen_handle h, cg_obj *co_match, cvec *cvv, cg_callback *callbacks);
void cligen_echo_on(void);
void cligen_echo_off(void);

#endif /* _CLIGEN_READ_H_ */
