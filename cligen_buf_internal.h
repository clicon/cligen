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
  * This is an internal CLIgen header file 
  * Do not use these struct for external use, the internal structure may change. 
 * @see cligen_buf.h for external API use
 */

#ifndef _CLIGEN_BUF_INTERNAL_H
#define _CLIGEN_BUF_INTERNAL_H

/*
 * Types
 */
/*! Internal CLIgen buffer. 
 */
struct cbuf {
    char  *cb_buffer;   /* pointer to buffer */
    size_t cb_buflen;   /* allocated bytes of buffer */
    size_t cb_strlen;   /* length of string in buffer (< buflen) */
};

#endif /* _CLIGEN_BUF_INTERNAL_H */
