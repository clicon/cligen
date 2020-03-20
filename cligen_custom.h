/*
  ***** BEGIN LICENSE BLOCK *****
 
  Copyright (C) 2001-2019 Olof Hagsand

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


  Custom file as boilerplate appended by cligen_config.h 
*/
#if 1 /* SANITY CHECK */
typedef struct {int a;} *cligen_handle;
#else
typedef void *cligen_handle; /* API */
#endif

/*! Experimental support for not expanding variables unless <tab> or <?> is
 * pressed. CLIgen otherwise calls the expand callback that may, for example,
 * contact a remote backend.
 * So, only expand on interactive CLI operations
 * Warning, dont enable this, it gives "Unknown command" in expand cases, such as tutorial interface eth0
 */
#undef EXPAND_ONLY_INTERACTIVE

/* New EXPERIMENTAL SETS functionality. 
 * Example; a @{b;c;d;}
 * allows strings with b,c,d at most once in any order.
 */
#undef USE_SETS
