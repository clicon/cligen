/*
  CLI generator match functions, used in runtime checks.

  ***** BEGIN LICENSE BLOCK *****
 
  Copyright (C) 2001-2018 Olof Hagsand

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


#ifndef _CLIGEN_MATCH_H
#define _CLIGEN_MATCH_H

/* Number of allowed matchings */
#define MATCHVECTORLEN 1024

/*
 * Internally exported variables
 */
/* if several cligen object variables match with same preference, select first */
extern int _match_cgvar_same;

/*
 * Function Prototypes
 */
int match_pattern(cligen_handle h, char *, parse_tree , int, int, pt_vec *, 
		  int **, int *, cvec *, char **);
int match_pattern_exact(cligen_handle h, char *, parse_tree, int exact, cvec *, cg_obj **);
int command_levels(char *string);
int extract_substring(char *string0, int level, char **sp);
int extract_substring_rest(char *string0, int level, char **sp);
int match_complete(cligen_handle h, parse_tree pt,
		   char **stringp, size_t *slen, cvec *cvec);

#endif /* _CLIGEN_MATCH_H */



