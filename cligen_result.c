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

 * Result generated when matching and used inyternally in cligen
 * This include file is used internally in cligen, not part of API
 * The C struct is not exposed outside the .c file
 */
#include "cligen_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#include <limits.h>
#include <sys/types.h>
#include <assert.h>

#include "cligen_buf.h"
#include "cligen_cv.h"
#include "cligen_cvec.h"
#include "cligen_callback.h"
#include "cligen_object.h"
#include "cligen_parsetree.h"
#include "cligen_result.h"

/*! Result vector from match_pattern_* family of functions
 */
struct match_result{
    parse_tree  *mr_pt;        
    char        *mr_reason; /* Error reason if mr_len=0. Can also be carried by a mr_len!=0 
			     * to store first error in case it is needed in a later error */
    int          mr_level;
    int          mr_last;
    char        *mr_token;  /* Direct, not copied */
    cg_obj      *mr_co_match_orig; /* Kludge, save (latest) matched object, see 
                                      mr_flags_set_co_match() */
};

int
mr_pt_len_get(match_result *mr)
{
    if (mr->mr_pt == NULL)
	return 0;
    return pt_len_get(mr->mr_pt);
}

/*! Reset parse-tree vector, DONT free any co-objects
 */
int
mr_pt_reset(match_result *mr)
{
    pt_free(mr->mr_pt, 0);
    if ((mr->mr_pt = pt_new()) == NULL)
	return -1;
    return 0;
}

/*! Truncate parse-tree vector: free all extra elements, keep nr of heading elements
 *
 * the objects in the tail of the pt tree is freed.
 * @param[in]  mr  
 * @param[in]  len   >0
 */
int
mr_pt_trunc(match_result *mr,
	    int           len)
{
    return pt_trunc(mr->mr_pt, len);
}

int
mr_pt_append(match_result *mr,
	     cg_obj       *co,
	     char         *token)
{
    cg_obj *co1 = NULL;

    if (co_copy1(co, NULL, 0, 0x0, &co1) < 0)
	return -1;
    mr->mr_co_match_orig = co;
    mr->mr_token = token;
    return pt_vec_append(mr->mr_pt, co1);
}

cg_obj*
mr_pt_i_get(match_result *mr,
	    int           i)
{
    return pt_vec_i_get(mr->mr_pt, i);
}

parse_tree*
mr_pt_get(match_result *mr)
{
    return mr->mr_pt;
}

char*
mr_reason_get(match_result *mr)
{
    return mr->mr_reason;
}

/*! Reset/empty matchvec of indexes by g and incrementing vector
 * @param[in,out]  mr      Match result struct
 * @param[in]      reason  Malloced string (consumed here)
 */
int
mr_reason_set(match_result *mr,
	      char         *reason)
{
    if (mr->mr_reason)
    	free(mr->mr_reason);
    mr->mr_reason = reason;
    return 0;
}

int
mr_level_get(match_result *mr)
{
    return mr->mr_level;
}

int
mr_level_set(match_result *mr,
	     int           level)
{
    mr->mr_level = level;
    return 0;
}

char *
mr_token_get(match_result *mr)
{
    return mr->mr_token;
}

int
mr_last_get(match_result *mr)
{
    return mr->mr_last;
}

int
mr_last_set(match_result *mr)
{
    mr->mr_last = 1;
    return 0;
}

/*! Move an error reason from one mr to the next
 * There is a case for keeping the first error reason in case of multiple
 */
int
mr_mv_reason(match_result *from,
	     match_result *to)
{
    char *reason;

    if ((reason = from->mr_reason) != NULL &&
	to->mr_reason == NULL){
	to->mr_reason = reason;
	from->mr_reason = NULL;
    }
    return 0;
}

/*! Create new CLIgen match result
 * @see mr_free
 */
match_result *
mr_new(void)
{
    match_result *mr;
    
    if ((mr = malloc(sizeof(*mr))) == NULL)
	return NULL;
    memset(mr, 0, sizeof(*mr));
    if ((mr->mr_pt = pt_new()) == NULL){
	free(mr);
	return NULL;
    }
    return mr;
}

/*! Free a return structure
 * Dont free the parse tree mr_parsetree XXX why?
 */
int
mr_free(match_result *mr)
{
    if (mr->mr_pt){
	pt_free(mr->mr_pt, 0);
    }
    if (mr->mr_reason)
	free(mr->mr_reason);
    free(mr);
    return 0;
}

/*! Map from match-result to cligen-result
 * Could probably collapse the two concepts into one, but there is some history
 */
cligen_result 
mr2result(match_result *mr)
{
    switch (mr_pt_len_get(mr)){
    case -1: /* shouldnt happen */
	return CG_ERROR;
	break;
    case 0:
	return CG_NOMATCH;
	break;
    case 1:
	return CG_MATCH;
	break;
    default:
	return CG_MULTIPLE;
	break;
    }
}

/*! Set CO_FLAGS_MATCH
 *
 * Kludge to mark both the copy and the most recently matched object
 */
int
mr_flags_set_co_match(match_result *mr,
		      cg_obj       *co)
{
    co_flags_set(mr->mr_co_match_orig, CO_FLAGS_MATCH);
    co_flags_set(co, CO_FLAGS_MATCH);
    return 0;
}
