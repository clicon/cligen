/*
  CLI generator match functions, used in runtime checks.

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

#define __USE_GNU /* isblank() */
#include <ctype.h>
#ifndef isblank
#define isblank(c) (c==' ')
#endif /* isblank */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "cligen_buf.h"
#include "cligen_cv.h"
#include "cligen_cvec.h"
#include "cligen_parsetree.h"
#include "cligen_object.h"
#include "cligen_handle.h"
#include "cligen_expand.h"
#include "cligen_read.h"
#include "cligen_match.h"

#ifndef MIN
#define MIN(x,y) ((x)<(y)?(x):(y))
#endif

#define ISREST(co) ((co)->co_type == CO_VARIABLE && (co)->co_vtype == CGV_REST)

/*! Match variable against input string
 * 
 * @param[in]  string  Input string to match
 * @param[in]  pvt     variable type (from definition)
 * @param[in]  cmd     variable string (from definition) - can contain range
 * 
 * @retval     -1      Error (print msg on stderr)
 * @retval     0       Not match and reason returned as malloced string.
 * @retval     1       Match
 * Who prints errors?
 * @see cvec_match where actual allocation of variables is made not only sanity
 */
static int
match_variable(cligen_handle h,
	       cg_obj       *co, 
	       char         *str, 
	       char        **reason)
{
    int         retval = -1;
    cg_var     *cv; /* Just a temporary cv for validation */
    cg_varspec *cs;

    cs = &co->u.cou_var;
    if ((cv = cv_new(co->co_vtype)) == NULL)
	goto done;
    if (co->co_vtype == CGV_DEC64) /* XXX: Seems misplaced? / too specific */
	cv_dec64_n_set(cv, cs->cgs_dec64_n);
    if ((retval = cv_parse1(str, cv, reason)) <= 0) 
	goto done;
    /* here retval should be 1 */
    /* Validate value */
    if ((retval = cv_validate(h, cv, cs, reason)) <= 0)
	goto done;
    /* here retval should be 1 */
  done:
    if (cv)
	cv_free(cv);
    return retval; 
}

/*! Given a string and one cligen object, return if the string matches
 * @param[in]  string  Input string to match (NULL is match)
 * @param[in]  co      cligen object
 * @param[out] exact   1 if match is exact (CO_COMMANDS). VARS is 0.
 * @param[out] reason  if not match and co type is 0, reason points to a (malloced) 
 *                     string containing an error explanation string. If reason is
 *                     NULL no such string will be malloced. This string needs to
 *                     be freed.
 * @retval  -1         Error
 * @retval   0         Not match
 * @retval   1         Match
 */
static int 
match_object(cligen_handle h,
	     char         *str,
	     cg_obj       *co, 
	     int          *exact,
	     char        **reason)
{
  int    match = 0;
  size_t len = 0;

  if (str)
      len = strlen(str);
  if (exact)
      *exact = 0;
  if (co==NULL)
      return 0;
  switch (co->co_type){
  case CO_COMMAND:
      if (str == NULL)
	  match++;
      else{
	  match = (strncmp(co->co_command, str, len) == 0);
	  if (exact)
	      *exact = strlen(co->co_command) == len;
	  if (match == 0 && reason){
	      if ((*reason = strdup("Unknown command")) == NULL)
		  return -1;
	  }
      }
    break;
  case CO_VARIABLE:
      if (str == NULL || len==0)
	  match++;
      else
	  if ((match = match_variable(h, co, str, reason)) < 0)
	      return -1;
    break;
  case CO_REFERENCE: /* This should never match, it is an abstract object that is expanded */
      if (reason){
	  if ((*reason = strdup("Reference")) == NULL)
	      return -1;
      }
      break;
  }
  if (match == 0){
      if (reason)
	  assert(*reason != NULL);
      return 0;
  }
  else
      return 1;
}

/*! Given a string (s0), return the next token. 
 * The string is modified to return
 * the remainder of the string after the identified token.
 * A token is found either as characters delimited by one or many delimiters.
 * Or as a pair of double-quotes(") with any characters in between.
 * if there are trailing spaces after the token, trail is set to one.
 * If string is NULL or "", NULL is returned.
 * If empty token found, s0 is NULL
 * @param[in]  s0       String, the string is modified like strtok
 * @param[out] token0   A malloced token.  NOTE: token must be freed after use.
 * @param[out] rest0    A remaining (rest) string.  NOTE: NOT malloced.
 * @param[out] leading0 If leading delimiters eg " thisisatoken"
 * Example:
 *   s0 = "  foo bar"
 * results in token="foo", leading=1
 */
static int
next_token(char **s0, 
	   char **token0,
	   char **rest0, 
	   int   *leading0)
{
    char  *s;
    char  *st;
    char  *token = NULL;
    size_t len;
    int    quote=0;
    int    leading=0;
    int    escape = 0;

    s = *s0;
    if (s==NULL){
	fprintf(stderr, "%s: null string\n", __FUNCTION__);
	return -1;
    }
    for (s=*s0; *s; s++){ /* First iterate through delimiters */
	if (index(CLIGEN_DELIMITERS, *s) == NULL)
	    break;
	leading++;
    }
    if (rest0)
	*rest0 = s;
    if (*s && index(CLIGEN_QUOTES, *s) != NULL){
	quote++;
	s++;
    }
    st=s; /* token starts */
    escape = 0;
    for (; *s; s++){ /* Then find token */
	if (quote){
	    if (index(CLIGEN_QUOTES, *s) != NULL)
		break;
	}
	else{ /* backspace tokens for escaping delimiters */
	    if (escape)
		escape = 0;
	    else{
		if (*s == '\\')
		    escape++;
		else
		    if (index(CLIGEN_DELIMITERS, *s) != NULL)
			break;
	    }
	}
    }
    if (quote && *s){
	s++;
	// fprintf(stderr, "s=\"%s\" %d %s\n", s, *s, index(CLIGEN_DELIMITERS, *s));
	if (*s && index(CLIGEN_DELIMITERS, *s) == NULL){
	    ;//	cligen_reason("Quote token error");
	}
	len = (s-st)-1;
    }
    else{
	if (quote){ /* Here we signalled error before but it is removed */
	    st--;
	}
	len = (s-st);
	if (!len){
	    token = NULL;
	    *s0 = NULL;
	    goto done;
	}
    }
    if ((token=malloc(len+1)) == NULL){
	fprintf(stderr, "%s: malloc: %s\n", __FUNCTION__, strerror(errno));
	return -1;
    }
    memcpy(token, st, len);
    token[len] = '\0';
    *s0 = s;
 done:
    *leading0 = leading;
    *token0 = token;
    return 0;
}

/*! Split a CLIgen command string into a cligen variable vector using delimeters and escape quotes
 *
 * @param[in]  string String to split
 * @param[out] cvtp   CLIgen variable vector, containing all tokens. 
 * @param[out] cvrp   CLIgen variable vector, containing the remaining strings. 
 * @retval     0      OK
 * @retval    -1      Error
 * @code
 *   cvec  *cvt = NULL;
 *   cvec  *cvr = NULL;
 *   if (cligen_str2cvv("a=b&c=d", " \t", "\"", &cvt, &cvt) < 0)
 *     err;
 *   ...
 *   cvec_free(cvt);
 *   cvec_free(cvr);
 * @endcode
 * Example, input string "aa bb cc" (0th element is always whole string)
 *   cvp : ["aa bb cc", "aa", "bb", "cc"]
 *   cvr : ["aa bb cc", "aa bb cc", "bb cc", "cc"]
 * @note both out cvv:s should be freed with cvec_free()
 */
int
cligen_str2cvv(char  *string, 
	       cvec **cvtp,
    	       cvec **cvrp)
{
    int     retval = -1;
    char   *s;
    char   *sr;
    char   *s0 = NULL;;
    cvec   *cvt = NULL; /* token vector */
    cvec   *cvr = NULL; /* rest vector */
    cg_var *cv;
    char   *t;
    int     trail;
    int     i;

    if ((s0 = strdup(string)) == NULL)
	goto done;
    s = s0;
    if ((cvt = cvec_start(string)) ==NULL)
	goto done;
    if ((cvr = cvec_start(string)) ==NULL)
	goto done;
    i = 0;
    while (s != NULL) {
	if (next_token(&s, &t, &sr, &trail) < 0)
	    goto done;
	/* If there is no token, stop, 
	 * unless it is the intial token (empty string) OR there are trailing whitespace
	 * In these cases insert an empty "" token.
	 */
	if (t == NULL && !trail && i > 0)
	    break;
	if ((cv = cvec_add(cvr, CGV_STRING)) == NULL)
	    goto done;
	if (cv_string_set(cv, sr?sr:"") == NULL) /* XXX memleak */
	    goto done;
	if ((cv = cvec_add(cvt, CGV_STRING)) == NULL)
	    goto done;
	if (cv_string_set(cv, t?t:"") == NULL) /* XXX memleak */
	    goto done;
	if (t)
	    free(t);
	i++;
    }
    retval = 0;
    assert(cvec_len(cvt)>1); /* XXX */
    assert(cvec_len(cvr)>1); /* XXX */
    if (cvtp){
	*cvtp = cvt;
	cvt = NULL;
    }
    if (cvrp){
	*cvrp = cvr;
	cvr = NULL;
    }
 done:
    if (s0)
	free(s0);
    if (cvt)
	cvec_free(cvt);
    if (cvr)
	cvec_free(cvr);
    return retval;
}

/*! Returns the total number of "levels" of a CLIgen command string
 *
 * A level is an atomic command delimetered by space or tab.
 * Example: "", "a", "abcd" has level 0
 *          "abcd ", "vb fg" has level 1
 *          "abcd gh ", "vb fg hjsa" has level 2
 *
 * @param[in] cv    CLIgen variable vector, containing all tokens. 
 * @retval    0-n   Number of levels
 * @retval    -1    Error
 */
int
cligen_cvv_levels(cvec *cvv)
{
    size_t sz;
    
    if (cvv == NULL)
	return -1;
    sz = cvec_len(cvv);
    if (sz == 0)
	return -1;
    else return sz - 2;
}

/*! Return if the parse-tree is aonly variables, or if there is at least one non-variable
 * @param[in] pt  Parse-tree
 * @retval    0   Empty or contains at least one command (non-var) 
 * @retval    1   0 elements, only variables alternatives (or commands derived from variables)
 */
static int
pt_onlyvars(parse_tree *pt)
{
    int     i;
    cg_obj *co;
    int     onlyvars = 0;
    
    for (i=0; i<pt_len_get(pt); i++){ 
	if ((co = pt_vec_i_get(pt, i)) == NULL)
	    continue;
	if (co->co_type != CO_VARIABLE && co->co_ref == NULL){
	    onlyvars = 0;
	    break;
	}
	onlyvars = 1;
    }
    return onlyvars;
}

/*! Help function to append a cv to a cvec. For expansion cvec passed to pt_expand_2
 * @param[in]  co     A cligen variable that has a matching value
 * @param[in]  cmd    Value in string of the variable
 * @param[out] cvv   The cligen variable vector to push a cv with name of co and
 *                    value in cmd
 * @retval     cv     Cligen variable
 * @retval     NULL   Error
 */
static cg_var *
add_cov_to_cvec(cg_obj *co, 
		char   *cmd, 
		cvec   *cvv)
{
    cg_var *cv = NULL;

    if ((cv = cvec_add(cvv, co->co_vtype)) == NULL)
	return NULL;
    cv_name_set(cv, co->co_command);
    cv_const_set(cv, iskeyword(co));
	if (co->co_vtype == CGV_DEC64) /* XXX: Seems misplaced? / too specific */
		cv_dec64_n_set(cv, co->co_dec64_n);
    if (cv_parse(cmd, cv) < 0) {
	cv_reset(cv);
	cvec_del(cvv, cv);
	return NULL;
    }
    return cv;
}

static int
matchv_append(int  i,
	      int *matchvec[],
	      int *matches,
	      int *matchlen)
{
    int retval = -1;

    if ((*matches)+1 > *matchlen){
	(*matchlen)++;
	if ((*matchvec = realloc(*matchvec, (*matchlen)*sizeof(int))) == NULL){
	    fprintf(stderr, "%s: realloc: %s\n", __FUNCTION__, strerror(errno));
	    goto done;
	}
    }
    (*matchvec)[(*matches)++] = i;
    retval = 0;
 done:
    return retval;
}

/*! Match a parse-tree (pt) with a command vector (cvt/cvr)
 * @param[in]  h        CLIgen handle
 * @param[in]  cvt      Tokenized string: vector of tokens
 * @param[in]  cvr      Rest variant,  eg remaining string in each step
 * @param[in]  pt       Vector of commands (array of cligen object pointers (cg_obj)
 * @param[in]  level    Current command level
 * @param[in]  best     Only return best match (for command evaluation) instead of 
 *                      all possible options
 * @param[out] matchvec A vector of indexes into ptp
 * @param[out] matchesp Number of matches in matchv, (if retval is 0)
 * @param[out] reason   If matches = 0, this may be malloced to indicate reason for 
 *                      not matching variables, if given. Neeed to be free:d
 * @retval     0        OK. Number of matches returned in matchesp
 * @retval    -1        Error
 */
static int
match_vec(cligen_handle h,
	  cvec         *cvt,
	  cvec         *cvr,
	  parse_tree   *pt,
	  int           level, 
	  int           best,
	  int          *matchvec[],
	  int          *matchesp,
	  char        **reason)
{
    int     retval = -1;
    int     matchlen = 0; /* Length of matchvec */
    int32_t pref_lower = INT32_MAX; /* Preference lower bound */
    int32_t pref_upper = 0;         /* Preference upper bound */
    int     p;             /* If all fails, save lowest(widest) preference error message */
    int     exact;
    char   *rtmp = NULL;
    int     i;
    char   *strtoken;
    char   *strrest;
    cg_obj *co;
    int     match;

    /* Either individual token or rest-of-string */
    strtoken = cvec_i_str(cvt, level+1);
    strrest  = cvec_i_str(cvr, level+1);

    /* Loop through parse-tree at this level to find matches */
    for (i=0; i<pt_len_get(pt); i++){
	if ((co = pt_vec_i_get(pt, i)) == NULL)
	    continue;
	/* Return -1: error, 0: nomatch, 1: match */
	rtmp = NULL;
	if ((match = match_object(h,
				  ISREST(co)?strrest:strtoken,
				  co, &exact,
				  &rtmp      /* if match == 0 */
				  )) < 0)
	    goto done;
	p = co_pref(co, exact); /* get match preferences (higher is better match) */
	if (match == 0){ /* No match */
	    assert(rtmp != NULL);
	    /* If all fails, save lowest(widest) preference error message,
	     * for variables only
	     */
	    if (p < pref_lower && co->co_type == CO_VARIABLE){
		pref_lower = p;
		if (*reason)
		    free(*reason);
		*reason = rtmp;
		rtmp = NULL;
	    }
	    if (rtmp){
		free(rtmp);
		rtmp = NULL;
	    }
	}
	else { /* Match: if best compare and save highest preference */
	    assert(rtmp == NULL);
	    if (best){ /* only save best match */
		if (p == pref_upper){
		    if (matchv_append(i, matchvec, matchesp, &matchlen) < 0)
			goto done;
		}
		else if (p > pref_upper){ /* Start again at this level */
		    pref_upper = p;
		    *matchesp = 0;
		    if (matchv_append(i, matchvec, matchesp, &matchlen) < 0)
			goto done;
		}
		else{ /* p < pref_upper : skip */
		}
	    } /* if best */
	    else {
		if (matchv_append(i, matchvec, matchesp, &matchlen) < 0)
		    goto done;
	    }
	} /* switch match */
	assert(rtmp == NULL);
    } /* for pt_len_get(pt) */
    /* Only return reason if matches == 0 */
    if (*matchesp != 0 && reason && *reason){
	free(*reason);
	*reason = NULL;
    }
    retval = 0;
 done:
    return retval;
}

/*! Match terminal/leaf cligen objects. Multiple matches is used for completion.
 * We must have a preference when matching when it matches a command
 * and some variables.
 * The preference is:
 *  command > ipv4,mac > string > rest
 * return in matchvector which element match (their index)
 * and how many that match.
 * return value is the number of matches on return (also returned in matchlen)
 * index is the index of the first such match.
 * @param[in]  h        CLIgen handle
 * @param[in]  cvt      Tokenized string: vector of tokens
 * @param[in]  cvr      Rest variant,  eg remaining string in each step
 * @param[in]  pt       Vector of commands (array of cligen object pointers (cg_obj)
 * @param[in]  levels   Total nr of command levels
 * @param[in]  level    Current command level
 * @param[in]  best      Only return best match (for command evaluation) instead of 
 *                       all possible options
 * @param[out] covec      Returns the vector at the place of matching
 * @param[out] matchvec A vector of integers containing indexes in covec which match
 * @param[out] matchlen Number of matches in matchvec, (if retval is 0)
 * @param[out] reasonp  If *matchlen = 0, this may be malloced to indicate reason for 
 *                      not matching variables, if given. Neeed to be free:d
 * @retval     0        OK. Number of matches returned in matchlen
 * @retval    -1        Error
 * @see match_pattern_node
 */
static int 
match_pattern_terminal(cligen_handle h, 
		       cvec         *cvt,
		       cvec         *cvr,
		       parse_tree   *pt,
		       int           levels,
		       int           level, 
		       int           best,
		       co_vec_t     *covec, 
		       int          *matchvec[], 
		       int          *matchlen,
		       char        **reasonp
		       )
{
    int     retval = -1;
    int     matches = 0;
    cg_obj *co_match;
    cg_obj *co_orig;
    char   *reason = NULL;

    if (match_vec(h, cvt, cvr, pt, level, best, matchvec, &matches, &reason) < 0)
	goto done;
    /* Number of matches is 0 (no match), 1 (exact) or many */
    switch (matches){
    case 0: /* no matches */
	/* Reason is only recorded, sent to upper layers if there are only
	 * variables.then reason is set once
	 * this may not be the best preference, we just set the first
	 * This is logic to hinder error message to relate to variable mismatch
	 * if there is a commands on same level with higher prio to match.
	 * If all match fails, it is more interesting to understand the match fails
	 * on commands, not variables.
	 */
	if (pt_onlyvars(pt) && reasonp && reason){
	    *reasonp = reason;
	    reason = NULL;
	}
	break;
    default: /* multiple matches */
	/* If set, if multiple cligen variables match use the first one */
	*covec = pt_vec_get(pt);
	break;
    case 1: /* exactly one match */
	if (matches == 1)
	    *covec = pt_vec_get(pt); /* not in fallthru */
	co_match = pt_vec_i_get(pt, (*matchvec)[0]);
	co_orig = co_match->co_ref?co_match->co_ref: co_match;
	if (co_match->co_type == CO_COMMAND && co_orig->co_type == CO_VARIABLE){
	    if (co_value_set(co_orig, co_match->co_command) < 0)
		goto done;
	}
	break;

    } /* switch matches */
    if (matchlen)
	*matchlen = matches;
    retval = 0;
 done:
    if (reason)
	free(reason);
    return retval;
}

/*! Match non-terminal cligen object. Need to match exact.
 *
 * @param[in]  h         CLIgen handle
 * @param[in]  string0   Input string to match
 * @param[in]  cvt       Tokenized string: vector of tokens
 * @param[in]  cvr       Rest variant,  eg remaining string in each step
 * @param[in]  pt        Vector of commands. Array of cligen object pointers
 * @param[in]  pt_max    Length of the pt array
 * @param[in]  levels    Total nr of command levels
 * @param[in]  level     Current command level
 * @param[in]  best      Only return best match (for command evaluation) instead of 
 *                       all possible options
 * @param[in]  hide      Respect hide setting of commands (dont show)
 * @param[in]  expandvar Set if VARS should be expanded, eg ? <tab>
 * @param[out] covec       Returns the input pt vector at the place of matching
 * @param[out] matchvec  A vector of integers containing indexes in covec which match
 * @param[out] matchlen  Number of matches in matchvec, (if retval is 0)
 * @param[out] cvv       cligen variable vector containing vars/values pair for 
 *                       completion
 * @param[out] reasonp   If retval = 0, this may be malloced to indicate reason for
 *                       not matching variables, if given. Need to be free:d
 * @retval     0         OK. Number of matches returned in matchlen
 * @retval    -1         Error
 * @see match_pattern_terminal
 */
static int 
match_pattern_node(cligen_handle h, 
		   cvec         *cvt,
		   cvec         *cvr,
		   parse_tree   *pt,
		   int           levels,
		   int           level, 
		   int           best,
		   int           hide,
		   int           expandvar,
		   co_vec_t     *covec, 
		   int          *matchvec[], 
		   int          *matchlen,
		   cvec         *cvv,
		   char        **reasonp
		   )
{
    int         retval = -1;
    int         matches = 0;
    cg_obj     *co_match = NULL;
    cg_obj     *co_orig;
    char       *reason = NULL;
    char       *str;
    parse_tree *ptn = NULL;     /* Expanded */
    cg_var     *cv = NULL;

    if ((ptn = pt_new()) == NULL)
	goto done;
    if (match_vec(h, cvt, cvr, pt, level,
		  1, /* use best preference match in non-terminal matching*/
		  matchvec, &matches, &reason) < 0)
	goto done;
    /* Number of matches is 0 (no match), 1 (exact) or many */
    switch (matches){
    case 0: /* no matches */
	if (pt_onlyvars(pt) && reasonp && reason){
	    *reasonp = reason;
	    reason = NULL;
	}
	retval = 0;
	goto done;
	break;
    case 1: /* exactly one match */
	break;
    default: /* multiple matches */
	retval = 0;
	goto done;
	break;
    } /* switch matches */
    if (matches != 1) 
	goto ok;
    co_match =  pt_vec_i_get(pt, (*matchvec)[0]);
    if (ISREST(co_match))
	str  = cvec_i_str(cvr, level+1);
    else
	str  = cvec_i_str(cvt, level+1);

    /* co_orig is original object in case of expansion */
    co_orig = co_match->co_ref?co_match->co_ref: co_match;
    if (pt_expand_treeref(h, co_match, co_pt_get(co_match)) < 0) /* sub-tree expansion */
	goto done;

    if (co_match->co_type == CO_VARIABLE){
	if ((cv = add_cov_to_cvec(co_match, str, cvv)) == NULL)
	    goto done;
	/* 
	 * Special case: we have matched a REST variable (anything) and
	 * there is more text have this word, then we can match REST
	 * This is "inline" of match_terminal
	 */
	if (ISREST(co_match)){
	    *covec = pt_vec_get(pt);
	    goto ok;
	}
    }
    else
	if (co_match->co_type == CO_COMMAND && co_orig->co_type == CO_VARIABLE)
	    if ((cv = add_cov_to_cvec(co_orig, co_match->co_command, cvv)) == NULL)
		goto done;
    if (pt_expand_2(h, co_pt_get(co_match), cvv, hide, expandvar, ptn) < 0) /* expand/choice variables */
	goto done;
    matches = 0;
    if (level+1 == levels){
	if (match_pattern_terminal(h, cvt, cvr, ptn, 
				   levels,	level+1, 
				   best,
				   covec, matchvec, &matches, reasonp) < 0)
	    goto done;
    }
    else
	if (match_pattern_node(h, cvt, cvr, ptn,
			       levels, level+1, 
			       best, hide, expandvar,
			       covec, matchvec, &matches, cvv, reasonp) < 0)
	    goto done;
    if (co_pt_exp_add(co_orig, ptn) < 0) 
	goto done;
    ptn = NULL;
    if (co_match->co_type == CO_COMMAND && co_orig->co_type == CO_VARIABLE)
	if (co_value_set(co_orig, co_match->co_command) < 0)
	    goto done;
 ok:
    *matchlen = matches;
    retval = 0;
 done:
    if (cv){ /* cv may be stale */
	cv = cvec_i(cvv, cvec_len(cvv)-1);
	cv_reset(cv);
	cvec_del(cvv, cv);
    }
    if (ptn != NULL){
	if (pt_free(ptn, 0) < 0)
	    return -1;
    }
    /* Only the last level may have multiple matches */
    return retval;
}

/*! CLIgen object matching function
 * @param[in]  h         CLIgen handle
 * @param[in]  string    Input string to match
 * @param[in]  cvt       Tokenized string: vector of tokens
 * @param[in]  cvr       Rest variant,  eg remaining string in each step
 * @param[in]  pt        Vector of commands (array of cligen object pointers (cg_obj)
 * @param[in]  best      Only return best match (for command evaluation) instead of 
 *                       all possible options
 * @param[in]  hide      Respect hide setting of commands (dont show)
 * @param[in]  expandvar Set if VARS should be expanded, eg ? <tab>
 * @param[out] covec       Returns the vector at the place of matching
 * @param[out] matchvec  A vector of integers containing indexes in covec which match
 * @param[out] matchlen  Number of matches in matchvec, (if retval is 0)
 * @param[out] cvv       cligen variable vector containing vars/values pair for completion
 * @param[out] reasonp   If retval = 0, this may be malloced to indicate reason for 
 *                       not matching variables, if given. Neeed to be free:d
 *
 * @retval -1   error.
 * @retval nr   The number of matches (0-n) in pt or -1 on error. See matchlen below.
 *
 * All options are ordered by PREFERENCE, where 
 *       command > ipv4,mac > string > rest
 */
int 
match_pattern(cligen_handle h,
	      cvec         *cvt,
	      cvec         *cvr,
	      parse_tree   *pt, 
	      int           best,
	      int           hide,
	      int           expandvar,
	      co_vec_t     *covec, 
	      int          *matchvec[],
	      int          *matchlen, 
	      cvec         *cvv,
	      char        **reasonp)
{
    int retval = -1;
    int levels;

    if (covec == NULL || cvt == NULL || cvr == NULL){
	errno = EINVAL;
	goto done;
    }
    *matchlen = 0;
    /* Get total number of command levels */
    if ((levels = cligen_cvv_levels(cvt)) < 0)
	goto done;
    if (levels == 0){
	if (match_pattern_terminal(h, cvt, cvr,
				   pt,
				   levels, 0, best,
				   covec, matchvec, matchlen, 
				   reasonp) < 0)
	    goto done;
    }
    else if (match_pattern_node(h, cvt, cvr,
				pt,
				levels, 0,
				best, hide, expandvar,
				covec, matchvec, matchlen,
				cvv,
				reasonp) < 0)
	    goto done;
    retval = 0;
 done:
    return retval;
}

/*! CLIgen object matching function for exact match
 * @param[in]  h         CLIgen handle
 * @param[in]  string    Input string to match
 * @param[in]  cvt       Tokenized string: vector of tokens
 * @param[in]  cvr       Rest variant,  eg remaining string in each step
 * @param[in]  pt        CLIgen parse tree, vector of cligen objects.
 * @param[in]  expandvar Set if VARS should be expanded, eg ? <tab>
 * @param[out] cvv       CLIgen variable vector containing vars/values pair for completion
 * @param[out] match_obj Exact object to return
 * @param[out] resultp   Result, < 0: errors, >=0 number of matches (only if retval == 0)
 * @param[out] reason    If retval is 0 and matchlen != 1, contains reason 
 *                       for not matching variables, if given. Need to be free:d
 * @retval  -1           Error
 * @retval   0           OK, resultp contains more info.
 */
int 
match_pattern_exact(cligen_handle  h, 
		    cvec          *cvt,
		    cvec          *cvr,
		    parse_tree    *pt, 
		    int            expandvar,
		    cvec          *cvv,
		    cg_obj       **match_obj,
		    cligen_result *resultp,
		    char         **reason)
{
    int           retval = -1;
    co_vec_t      res_pt;
    cg_obj       *co = NULL;
    int          *matchvec = NULL;
    int           matchlen = -1; /* length of matchvec */
    int           i;

    if ((match_pattern(h, cvt, cvr,
		       pt,
		       1, /* best: Return only best option */
		       0, 1,
		       &res_pt, 
		       &matchvec,
		       &matchlen, 
		       cvv, reason)) < 0)
	goto done;
    assert(matchlen != -1);
    /* If no match fix an error message */
    if (matchlen == 0){
	if (reason && *reason == NULL){
	    if ((*reason = strdup("Unknown command")) == NULL)
		goto done;
	}
    }
    else if (matchlen > 1){
	/* In 4.5 there is also code for detecting allvars */
	if (cligen_preference_mode(h))
	    matchlen = 1; /* choose first element */
    }
    /* Only a single match at this point */
    if (matchlen != 1)
	goto ok;
    /* Here we have an obj (res_pt[]) that is unique so far.
       We need to see if there is only one sibling to it. */
    co = res_pt[*matchvec];
    /*
     * Special case: if a NULL child is not found, then set result == GC_NOMATCH
     */
    for (i=0; i < co_vec_len_get(co); i++){
	if (co_vec_i_get(co, i) == NULL)
	    break; /* If we match here it is OK, unless no match */
    }
    if (co_vec_len_get(co) != 0 && i==co_vec_len_get(co)){
	co = NULL;
	if (reason){
	    if (*reason != NULL)
		free(*reason);
	    if ((*reason = strdup("Incomplete command")) == NULL)
		goto done;
	}
	matchlen = 0;
    }
 ok:
    if (resultp){
	switch (matchlen){
	case -1: /* shouldnt happen */
	    *resultp = CG_ERROR;
	    break;
	case 0:
	    *resultp = CG_NOMATCH;
	    break;
	case 1:
	    *resultp = CG_MATCH;
	    break;
	default:
	    *resultp = CG_MULTIPLE;
	    break;
	}
    }
    if (match_obj)
	*match_obj = co;
    retval = 0;
 done:
    if (matchvec)
	free(matchvec);
    return retval;
}

/*! Try to complete a string as far as possible using the syntax.
 * 
 * @param[in]     h       cligen handle
 * @param[in]     pt      Vector of commands (array of cligen object pointers)
 * @param[in,out] stringp Input string to match and to complete (append to)
 * @param[in,out] slen    Current string length 
 * @param[out]    cvv     cligen variable vector containing vars/values pair for completion
 * @retval       -1       Error 
 * @retval        0       No matches, no completions made
 * @retval        1       Function completed by adding characters at the end of "string"
 */
int 
match_complete(cligen_handle h, 
	       parse_tree   *pt, 
	       char        **stringp, 
	       size_t       *slenp, 
	       cvec         *cvv)
{
    int      level;
    int      slen;
    int      equal;
    int      i;
    int      j;
    int      minmatch;
    cg_obj  *co;
    cg_obj  *co1 = NULL;
    cvec    *cvt = NULL;      /* Tokenized string: vector of tokens */
    cvec    *cvr = NULL;      /* Rest variant,  eg remaining string in each step */
    char    *string;
    char    *s;
    char    *ss;
    co_vec_t covec = NULL;
    int      matchlen = 0;
    int     *matchvec = NULL;
    int      mv;
    int      append = 0; /* Has appended characters */
    int      retval = -1;

    /* ignore any leading whitespace */
    string = *stringp;
    /* Tokenize the string and transform it into two CLIgen vectors: tokens and rests */
    if (cligen_str2cvv(string, &cvt, &cvr) < 0)
	goto done;
    s = string;
    while ((strlen(s) > 0) && isblank(*s))
	s++;
 again: /* XXX ugly goto usage, replace with loop */
    matchlen = 0;
    if (match_pattern(h, cvt, cvr,
		      pt,
		      0, /* best: Return all options, not only best */
		      1,
		      1, /* expandvar: Must be one for interactive TAB to work*/
		      &covec, &matchvec, &matchlen,
		      cvv, NULL) < 0)
	goto done;
    if (matchlen == 0){
	retval = 0;
	goto done; /*  No matches */
    }
    if ((level = cligen_cvv_levels(cvt)) < 0)
	goto done;
    ss = cvec_i_str(cvt, level+1);
    slen = ss?strlen(ss):0;

    minmatch = slen;
    equal = 1;
    for (i=0; i<matchlen; i++){
	mv = matchvec[i];
	assert(mv != -1);
	co = covec[mv];
	if (co == NULL){
	    retval = 0;
	    goto done;
	}
	if ((cligen_tabmode(h) & CLIGEN_TABMODE_VARS) == 0)
	    if (co->co_type != CO_COMMAND)
		continue;
	if (co1 == NULL){
	    minmatch = strlen(co->co_command);
	    co1 = co;
	}
	else{
	    if (strcmp(co1->co_command, co->co_command)==0)
		; /* equal */
	    else{
		equal = 0;
		for (j=0; j<MIN(strlen(co1->co_command), strlen(co->co_command)); j++)
		    if (co1->co_command[j] != co->co_command[j])
			break;
		minmatch = MIN(minmatch, j);
	    }
	}
    }
    if (co1 == NULL){
        retval = 0;
	goto done;
    }
    while (strlen(*stringp) + minmatch - slen >= *slenp){
	*slenp *= 2;
	if ((*stringp = realloc(*stringp, *slenp)) == NULL){
	    fprintf(stderr, "%s realloc: %s\n", __FUNCTION__, strerror(errno));
	    goto done;
	}
	string = *stringp;
    }
    strncat(string, &co1->co_command[slen], minmatch-slen);
    append = append || minmatch-slen;
    if (equal){ /* add space */
	string[strlen(string)+1] = '\0';
	string[strlen(string)] = cligen_delimiter(h);
	level++;
	slen = 0;
	co1 = NULL;
	if (cligen_tabmode(h)&CLIGEN_TABMODE_STEPS){
	    if (matchvec)
		free(matchvec);
	    matchvec = NULL;
	    goto again;
	}
    }
    retval = append?1:0;
  done:
    if (cvt)
	cvec_free(cvt);
    if (cvr)
	cvec_free(cvr);
    if (matchvec)
	free(matchvec);
    return retval;
}

