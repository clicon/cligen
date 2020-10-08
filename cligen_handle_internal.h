/*
  ***** BEGIN LICENSE BLOCK *****
 
  Copyright (C) 2001-2020 Olof Hagsand

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

 * This is an internal CLIgen header file 
 * Do not use these struct for external use, the internal structure may change. 
 * @see cligen_handle.h for external API use
*/

#ifndef _CLIGEN_HANDLE_INTERNAL_H_
#define _CLIGEN_HANDLE_INTERNAL_H_

#if 1
#define handle(h) ((struct cligen_handle *)(h))
#else /* Sanity test but requires assert all over */
#define handle(h) (assert(cligen_check(h)==0),(struct cligen_handle *)(h))
#endif

/*
 * CLIgen handle code.
 * Should be moved into its own, but there are some quirks with mutual dependencies
 * with rest cligen_object.h that I didnt have time to sort out.
 */
/*
 * Constants
 */
#define TERM_ROWS_DEFAULT 24
#define GETLINE_BUFLEN_DEFAULT 64 /* startsize, increased with 2x when run out */

#define CLIGEN_MAGIC 0x56ab55aa

/*
 * Types
 */
/* CLIgen handle. Its members should be hidden and only the typedef visible */
struct cligen_handle{
    int         ch_magic;        /* magic */
    char        ch_exiting;      /* Set by callback to request exit of CLIgen */
    char        ch_comment;      /* comment sign - everything behind it is ignored */
    char       *ch_prompt;       /* current prompt used */
    parse_tree_head *ch_parsetree_head; /* Linked list of parsetrees */
    char       *ch_treename_keyword; /* Name of treename parsing keyword */
    cg_obj     *ch_co_match;     /* Matching object in latest evaluation */
    char       *ch_fn_str;       /* Name of active callback function */
    int         ch_completion;   /* completion mode */    
    char       *ch_nomatch;      /* Why did a string not match an evaluation? */
    int         ch_tabmode;      /* short or long output mode on TAB */

    int         ch_lexicalorder; /* strcmp (0) or strverscmp (1) syntax order.
                                    Also, this is global for now */
    int         ch_ignorecase; /* dont care about aA (0), care about aA (1) 
				     does not work if lexicalorder is set.
				     Also this is global for now
				  */

    char       *ch_buf;          /* getline input buffer */
    char       *ch_killbuf;      /* getline killed text */

    int         ch_logsyntax;    /* Debug syntax by printing dynamically on stderr */
    int         ch_hist_size;    /* Number of history lines MUST be >0 */
    char      **ch_hist_buf;     /* Array of history lines */
    int         ch_hist_cur;     /* Current position (line) in history */
    int         ch_hist_last;    /* Last position in history */
    char       *ch_hist_pre;     /* Previous position in history */
    
    void       *ch_userhandle;   /* Use this as app-specific callback handle */
    void       *ch_userdata;     /* application-specific data (any data) */
    int         ch_regex_xsd;    /* 0: POSIX / REGEX(3); 1: LIBXML2 XSD */
    char        ch_delimiter;    /* Delimiter between objects */
    int         ch_preference_mode;   /* Relaxed variable match preference handling */
};

#endif /* _CLIGEN_HANDLE_INTERNAL_H_ */
