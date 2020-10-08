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
#include "cligen_parsetree_head.h"
#include "cligen_object.h"
#include "cligen_handle.h"
#include "cligen_expand.h"
#include "cligen_read.h"
#include "cligen_match.h"

#ifndef MIN
#define MIN(x,y) ((x)<(y)?(x):(y))
#endif

#define ISREST(co) ((co)->co_type == CO_VARIABLE && (co)->co_vtype == CGV_REST)

/*! Result vector from match_pattern_* family of functions
 */
struct match_result{
    uint32_t     mr_len;
    uint32_t     mr_size;
    int         *mr_vec;
    int          mr_level;
    parse_tree  *mr_parsetree;
    int          mr_last;
    char        *mr_reason; /* Error reason if mr_len=0. Can also be carried by a mr_len!=0 
			     * to store first error in case it is needed in a later error */
};
typedef struct match_result match_result;

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

#ifdef CO_HELPVEC
/*! Transform a single helpstr to a vector of help strings and strip preceeding whitespace
 *
 * @param[in]  str  String on the form <str1>\n<str2>
 * @param[out] cvp  CLIgen variable vector containing a vector of help strings 
 * @retval     0    OK
 * @retval    -1    Error
 * Eg, the string:
 *     "abcd\n     efgh\nijkl"
 * is translated to a cvec:
 * 0: "abcd"
 * 1: "efgh"
 * 2: "ijkl"
 */
int
cligen_txt2cvv(char  *str,
	       cvec **cvp)
{
    int     retval = -1;
    int     i;
    int     i0;
    char    c;
    cvec   *cvv = NULL;
    cg_var *cv = NULL;
    int     whitespace = 1;
    
    if ((cvv = cvec_new(0)) == NULL)
	goto done;
    i0 = 0;
    for (i=0; i<strlen(str); i++){
	c = str[i];
	if (whitespace && isblank(c))
	    i0 = i+1; /* skip */
	else if (c == '\n'){
	    if ((cv = cvec_add(cvv, CGV_STRING)) == NULL)
		goto done;
	    if (cv_strncpy(cv, &str[i0], i-i0) == NULL)
		goto done;
	    i0 = i+1;
	    whitespace = 1;
	}
	else
	    whitespace = 0;
    }
    /* There may be a case here where last charis \n */
    if (i-i0){
	if ((cv = cvec_add(cvv, CGV_STRING)) == NULL)
	    goto done;
	if (cv_strncpy(cv, &str[i0], i-i0) == NULL)
	    goto done;
    }
    if (cvp){
	assert(*cvp == NULL); /* XXX */
	*cvp = cvv;
    }
    retval = 0;
 done:
    return retval;
}
#endif


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

/*! Termination criterium foir command string
 */
static int
last_level(cvec *cvt,
	   int   level)
{
    int levels;
    
    assert((levels = cligen_cvv_levels(cvt)) >= 0);
    if (level >= levels)
	return 1;
    return 0;
}

/*! Termination criterium for parse-tree
 * Assume:
 * 1) pt -> []
 * 2) pt -> [null]
 * 3) pt -> [null, a, ..]
 * 4) pt -> [a, ..] # with no null element
 * @retval 0 case 4
 * @retval 1 case 3
 * @retval 2 case 1,2
 */
static int
last_pt(parse_tree *pt)
{
    int     i;
    cg_obj *co;

    if (pt_len_get(pt) == 0)
	return 1;
    for (i=0; i<pt_len_get(pt); i++){
	if ((co = pt_vec_i_get(pt, i)) == NULL)
	    return pt_len_get(pt)==1?1:2;
    }
    return 0;
}

/*! Return if the parse-tree is only variables, or if there is at least one non-variable
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

/*! Help function to append a cv to a cvec. For expansion cvec passed to pt_expand
 * @param[in]  h      CLIgen handle
 * @param[in]  co     A cligen variable that has a matching value
 * @param[in]  cmd    Value in string of the variable
 * @param[out] cvv   The cligen variable vector to push a cv with name of co and
 *                    value in cmd
 * @retval     cv     Cligen variable
 * @retval     NULL   Error
 * XXX see cvec_match
 */
static cg_var *
add_cov_to_cvec(cligen_handle h,
		cg_obj       *co, 
		char         *cmd, 
		cvec         *cvv)
{
    cg_var *cv = NULL;

    if ((cv = cvec_add(cvv, co->co_vtype)) == NULL)
	return NULL;
#if 1
    //    if (co->co_show)
    //	cv->var_show = strdup4(co->co_show);
    /* If translator function defined, here translate value */
    if (co->co_translate_fn != NULL &&
	co->co_translate_fn(h, cv) < 0)
	return NULL;
#endif
    cv_name_set(cv, co->co_command);
    if (co->co_vtype == CGV_DEC64) /* XXX: Seems misplaced? / too specific */
	cv_dec64_n_set(cv, co->co_dec64_n);
    if (cv_parse(cmd, cv) < 0) {
	cv_reset(cv);
	cvec_del(cvv, cv);
	return NULL;
    }
    return cv;
}


/*! Append int vector with one index
 */
static int
mr_vec_append(match_result *mr,
	      int           index)
{
    int retval = -1;

    if (mr->mr_size <= mr->mr_len){ /* need increased size */
	if ((mr->mr_vec = realloc(mr->mr_vec, (mr->mr_len+1)*sizeof(int))) == NULL)
	    goto done;
    }
    mr->mr_vec[mr->mr_len++] = index;
    mr->mr_size = mr->mr_len;
    retval = 0;
 done:
    return retval;
}

/*! Reset/empty matchvec of indexes by g and incrementing vector
 * @param[in,out]  mr  Match result struct
 */
static int
mr_vec_reset(match_result *mr)
{
    if (mr->mr_len)
	mr->mr_len = 0;
    return 0;
}

/*! Reset/empty matchvec of indexes by g and incrementing vector
 * @param[in,out]  mr      Match result struct
 * @param[in]      reason  Malloced string (consumed here)
 */
static int
mr_reason_set(match_result *mr,
	      char         *reason)
{
    if (mr->mr_reason)
	free(mr->mr_reason);
    mr->mr_reason = reason;
    return 0;
}

static int
mr_parsetree_set(match_result *mr,
		 parse_tree   *pt)
{
    mr->mr_parsetree = pt;
    return 0;
}

/*! Move an error reason from one mr to the next
 * There is a case for keeping the first error reason in case of multiple
 */
static int
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

static match_result *
mr_new(void)
{
    match_result *mr;
    
    if ((mr = malloc(sizeof(*mr))) == NULL)
	return NULL;
    memset(mr, 0, sizeof(*mr));
    return mr;
}

/*! Free a return structure
 * Dont free the parse tree mr_parsetree
 */
static int
mr_free(match_result *mr)
{
    if (mr->mr_vec)
	free(mr->mr_vec);
    if (mr->mr_reason)
	free(mr->mr_reason);
    free(mr);
    return 0;
}

/*! Match a parse-tree (pt) with a command vector (cvt/cvr)
 * @param[in]  h        CLIgen handle
 * @param[in]  token    Token to match at this level
 * @param[in]  resttokens Rest of tokens at this level (special case if type is REST)
 * @param[in]  pt       Vector of commands (array of cligen object pointers (cg_obj)
 * @param[in]  best     Only return best match (for command evaluation) instead of 
 *                      all possible options
 * @param[out] mr       Match result, when retval = 0
 * @retval     0        OK. result in mr parameter
 * @retval    -1        Error
 */
static int
match_vec(cligen_handle h,
	  parse_tree   *pt,
	  char         *token,
	  char         *resttokens,
	  int           best,
	  match_result *mr)
{
    int     retval = -1;
    int32_t pref_lower = INT32_MAX; /* Preference lower bound */
    int32_t pref_upper = 0;         /* Preference upper bound */
    int     p;             /* If all fails, save lowest(widest) preference error message */
    int     exact;
    char   *tmpreason = NULL;
    int     i;
    cg_obj *co;
    int     match;

    /* Loop through parse-tree at this level to find matches */
    for (i=0; i<pt_len_get(pt); i++){
	if ((co = pt_vec_i_get(pt, i)) == NULL)
	    continue;
	/* Return -1: error, 0: nomatch, 1: match */
	tmpreason = NULL;
	if ((match = match_object(h,
				  ISREST(co)?resttokens:token,
				  co, &exact,
				  &tmpreason      /* if match == 0 */
				  )) < 0)
	    goto done;
	p = co_pref(co, exact); /* get match preferences (higher is better match) */
	if (match == 0){ /* No match */
	    assert(tmpreason != NULL);
	    /* If all fails, save lowest(widest) preference error message,
	     * for variables only
	     */
	    if (p < pref_lower && co->co_type == CO_VARIABLE){
		pref_lower = p;
		mr_reason_set(mr, tmpreason);
		tmpreason = NULL;
	    }
	    if (tmpreason){
		free(tmpreason);
		tmpreason = NULL;
	    }
	}
	else { /* Match: if best compare and save highest preference */
	    assert(tmpreason == NULL);
	    if (best){ /* only save best match */
		if (p == pref_upper){
		    if (mr_vec_append(mr, i) < 0)
			goto done;
		}
		else if (p > pref_upper){ /* Start again at this level */
		    pref_upper = p;
		    if (mr_vec_reset(mr) < 0)
			goto done;
		    if (mr_vec_append(mr, i) < 0)
			goto done;
		}
		else{ /* p < pref_upper : skip */
		}
	    } /* if best */
	    else {
		if (mr_vec_append(mr, i) < 0)
		    goto done;
	    }
	} /* switch match */
	assert(tmpreason == NULL);
    } /* for pt_len_get(pt) */
    /* Only return reason if matches == 0 */
    if (mr->mr_len != 0)
	mr_reason_set(mr, NULL);
    retval = 0;
 done:
    return retval;
}

/*! Bind vars and constants to variable vectors used for completion and callbacks
 * @param[in]  h         CLIgen handle
 * @param[in]  co_match  Matched cligen parse object
 * @param[in]  token     Token 
 * @param[out] cvv       cligen variable vector containing vars/values pair for completion
 * @param[out] cvvall    cligen variable vector containing vars/values + keywords for callbacks
 */
static int
match_bindvars(cligen_handle h,
	       cg_obj       *co,
	       char         *token,
	       cvec         *cvv,
	       cvec         *cvvall)
{
    int     retval = -1;
    cg_var *cv = NULL;
    cg_obj *co_orig;

    /* co_orig is original object in case of expansion */
    co_orig = co->co_ref?co->co_ref: co;
    if (co->co_type == CO_VARIABLE){
	if (add_cov_to_cvec(h, co, token, cvv) == NULL)
	    goto done;
	if (cvvall && add_cov_to_cvec(h, co, token, cvvall) == NULL)
	    goto done;
    }
    else if (co->co_type == CO_COMMAND && co_orig->co_type == CO_VARIABLE){
	    if (add_cov_to_cvec(h, co_orig, co->co_command, cvv) == NULL)
		goto done;
	    if (cvvall && add_cov_to_cvec(h, co_orig, co->co_command, cvvall) == NULL)
		goto done;
    }
    else{
	if (!cv_exclude_keys_get() && cvvall){
	    if ((cv = cvec_add(cvvall, co_orig->co_vtype)) == NULL)
		goto done;
	    cv_name_set(cv, co_orig->co_command);
	    cv_type_set(cv, CGV_STRING);
	    cv_string_set(cv, co_orig->co_command);
	    cv_const_set(cv, 1);
	}
    }
    retval = 0;
 done:
    return retval;
}

static int
co_clearflag(cg_obj *co,
	     void   *arg)
{
    co_flags_reset(co, (intptr_t)arg);
    return 0;
}

/*! Matchpattern sets local
 *
 * @param[in]     h         CLIgen handle
 * @param[in]     cvt       Tokenized string: vector of tokens
 * @param[in]     cvr       Rest variant,  eg remaining string in each step
 * @param[in]     pt        Vector of commands. Array of cligen object pointers
 * @param[in]     pt_max    Length of the pt array
 * @param[in]     level     Current command level
 * @param[in]     best      Only return best match (for command evaluation) instead of 
 *                          all possible options. Only called from match_pattern_exact()
 * @param[in]     hide      Respect hide setting of commands (dont show)
 * @param[in]     expandvar Set if VARS should be expanded, eg ? <tab>
 * @param[out]    mrp       Match result including how many matches, level, reason for nomatc, etc
 * @param[in,out] cvv       cligen variable vector containing vars/values pair for completion
 * @param[in,out] cvvall    cligen variable vector containing vars/values + keywords for callbacks
 * @retval        0         OK. result returned in mrp
 * @retval        -1        Error
 */
static int 
match_pattern_sets_local(cligen_handle h, 
			 cvec         *cvt,
			 cvec         *cvr,
			 parse_tree   *pt,
			 int           level,
			 int           best,
			 int           hide,
			 int           expandvar,
			 cvec         *cvv,
			 cvec         *cvvall,
			 match_result **mrp)
{
    int         retval = -1;
    cg_obj     *co_match = NULL;
    cg_obj     *co_orig = NULL;
    int         lasttoken = 0;
    char       *token;
    char       *resttokens;
    match_result *mr0 = NULL;

    if ((mr0 = mr_new()) == NULL)
	goto done;
    /* Tokens of this level */
    token = cvec_i_str(cvt, level+1);
    /* Is this last token? */
    lasttoken = last_level(cvt, level); 
    resttokens  = cvec_i_str(cvr, level+1);
    
    /* Return level at this point, can be overriden by recursive call */
    mr0->mr_level = level;

    /* How many matches of cvt[level+1] in pt */
    if (match_vec(h,
		  pt, token, resttokens,
		  lasttoken?best:1, /* use best preference match in non-terminal matching*/
		   mr0) < 0)
	goto done;
    /* Number of matches is 0 (no match), 1 (exact) or many */
    switch (mr0->mr_len){
    case 0: /* no matches */
	if (pt_onlyvars(pt))
	    ; /* XXX Uuh mr0 already has a reason,... */
	mr_vec_reset(mr0);
	goto ok; 
	break;
    case 1: /* exactly one match */
	break;
    default: /* multiple matches:
	      * note that there is code in match_patter_exact that can collapse multiple matches 
	      * to one under certain circumstances
	      */
	if (lasttoken){
	    mr_parsetree_set(mr0, pt);
	    if (best){
		co_match = pt_vec_i_get(pt, mr0->mr_vec[0]); 
		if (match_bindvars(h, co_match, 
				   ISREST(co_match)?resttokens:token,
				   cvv, cvvall) < 0)
		    goto done;
	    }
	    goto ok; /* will return matches > 1 */ 
	}
	mr_vec_reset(mr0);
	goto ok;  /* will return matches = 0 */
	break;
    } /* switch matches */
    assert(mr0->mr_len == 1);
    /* Get the single match object */
    co_match = pt_vec_i_get(pt, mr0->mr_vec[0]);
    /* co_orig is original object in case of expansion */
    co_orig = co_match->co_ref?co_match->co_ref: co_match;

    /* Already matched (sets functionality) */
    if (co_flags_get(co_match, CO_FLAGS_MATCH)){ /* XXX: orig?? */
	char *r;
	if ((r = strdup("Already matched")) == NULL)
	    goto done;
	mr_reason_set(mr0, r);
	mr_vec_reset(mr0);
	goto ok; /* will return matches = 0 */
    }

    /* Do it if not last or best */
    if (!lasttoken || best){
	if (match_bindvars(h, co_match, 
			   ISREST(co_match)?resttokens:token,
			   cvv, cvvall) < 0)
	    goto done;
    }
    if (lasttoken ||
	(co_match->co_type == CO_VARIABLE && ISREST(co_match))){
	/* 
	 * Special case: we have matched a REST variable (anything) and
	 * there is more text have this word, then we can match REST
	 * This is "inline" of match_terminal
	 */
	mr_parsetree_set(mr0, pt);
	mr0->mr_last = 1; /* dont go to children */
    }
 ok: 
    /* mr0:local or mrc:child
     * if mrc has result, take that, otherwise take mr0
     */
    switch (mr0->mr_len) {
    case 0:
	break;
    case 1:
	assert(co_match);
	if (co_match->co_type == CO_COMMAND &&
	    co_orig && co_orig->co_type == CO_VARIABLE)
	    if (co_value_set(co_orig, co_match->co_command) < 0)
		goto done;
	break;
    default:
	break;
    } /* matches */
    *mrp = mr0;
    mr0 = NULL;
    retval = 0;
 done:
#if 0 /* Only if no match? */
    if (cv){ /* cv may be stale */
	cv = cvec_i(cvv, cvec_len(cvv)-1);
	cv_reset(cv);
	cvec_del(cvv, cv);
    }
#endif
    if (mr0)
	mr_free(mr0);
    /* Only the last level may have multiple matches */
    return retval;
} /* match_pattern_sets_local */


/*! Matchpattern sets
 *
 * @param[in]     h         CLIgen handle
 * @param[in]     cvt       Tokenized string: vector of tokens
 * @param[in]     cvr       Rest variant,  eg remaining string in each step
 * @param[in]     pt        Vector of commands. Array of cligen object pointers
 * @param[in]     pt_max    Length of the pt array
 * @param[in]     level     Current command level
 * @param[in]     best      Only return best match (for command evaluation) instead of 
 *                          all possible options. Only called from match_pattern_exact()
 * @param[in]     hide      Respect hide setting of commands (dont show)
 * @param[in]     expandvar Set if VARS should be expanded, eg ? <tab>
 * @param[out]    mrp       Match result including how many matches, level, reason for nomatc, etc
 * @param[in,out] cvv       cligen variable vector containing vars/values pair for completion
 * @param[in,out] cvvall    cligen variable vector containing vars/values + keywords for callbacks
 * @retval        0         OK. result returned in mrp
 * @retval        -1        Error
 */
static int 
match_pattern_sets(cligen_handle h, 
		   cvec         *cvt,
		   cvec         *cvr,
		   parse_tree   *pt,
		   int           level,
		   int           best,
		   int           hide,
		   int           expandvar,
		   cvec         *cvv,
		   cvec         *cvvall,
		   match_result **mrp)
{
    int           retval = -1;
    match_result *mr0 = NULL; /* Local */
    parse_tree   *ptn = NULL;   /* Expanded */
    cg_obj       *co_match;
    int           lastsyntax = 0;
    match_result *mrc = NULL; /* child result */
    match_result *mrcprev = NULL; /* previous succesful result */
    char         *token;

    token = cvec_i_str(cvt, level+1); /* for debugging */
    if (0)
	fprintf(stderr, "%s %s\n", __FUNCTION__, token);
    if (match_pattern_sets_local(h, cvt, cvr, pt, level, best, hide, expandvar,
				 cvv, cvvall, &mr0) < 0)
	goto done;
    if (mr0->mr_len != 1){
	*mrp = mr0;
	mr0 = NULL;
	goto ok;
    }
    co_match = pt_vec_i_get(pt, mr0->mr_vec[0]);
    if (mr0->mr_last){
	co_flags_set(co_match, CO_FLAGS_MATCH);
	*mrp = mr0;
	mr0 = NULL;
	goto ok;
    }
    if (pt_expand_treeref(h, co_match, co_pt_get(co_match)) < 0) /* sub-tree expansion */
	goto done;
    if ((ptn = pt_new()) == NULL)
	goto done;
    if (pt_expand(h, co_pt_get(co_match), cvv, hide, expandvar, ptn) < 0) /* expand/choice variables */
	goto done;
    /* Check termination criteria */
    lastsyntax = last_pt(ptn); /* 0, 1 or 2 */
    switch (lastsyntax){
    case 0: /* Not last in syntax tree, continue */
	break;
    case 1: /* Last in syntax tree (not token) */
	mr_parsetree_set(mr0, pt);
	co_flags_set(co_match, CO_FLAGS_MATCH);
	*mrp = mr0;
	mr0 = NULL;
	goto ok;
	break;
    case 2: /* Last in syntax tree but can continue,... */
	break;
    }
    if (pt_sets_get(ptn)){ /* For sets, iterate */
	while (!last_level(cvt, level)){
	    if (mrc != NULL)
		mrc = NULL;
	    if (match_pattern_sets(h, cvt, cvr, ptn,
				   level+1,
				   best, hide, expandvar,
				   cvv,
				   cvvall,
				   &mrc) < 0)
		goto done;		
	    if (mrc->mr_len != 1)
		break;
	    if (mrcprev != NULL){
		if (mrcprev->mr_parsetree != ptn &&
		    mrcprev->mr_parsetree != mrc->mr_parsetree)
		    pt_free(mrcprev->mr_parsetree, 0);
		mr_free(mrcprev);
		mrcprev = NULL;
	    }
	    mrcprev = mrc;
	    level = mrc->mr_level;
	}
    }
    else{
	if (match_pattern_sets(h, cvt, cvr, ptn,
			       level+1, 
			       best, hide, expandvar,
			       cvv,
			       cvvall,
			       &mrc) < 0)
	    goto done;
    }
    /* Clear all CO_FLAGS_MATCH recursively */
    pt_apply(pt, co_clearflag, (void*)CO_FLAGS_MATCH);
    assert(mrc != NULL);
    /* If child match fails, use previous */
    if (mrc->mr_len == 0 && mrcprev){
	mr_mv_reason(mrc, mrcprev); 	/* transfer error reason if any from child */
	*mrp = mrcprev;
	mrcprev = NULL;
    }
    else if (mrc->mr_len == 0 && lastsyntax == 2){ /* If no child match, then use local */
	mr_parsetree_set(mr0, pt);
	co_flags_set(co_match, CO_FLAGS_MATCH);
	mr_mv_reason(mrc, mr0); 	/* transfer error reason if any from child */
	*mrp = mr0;
	mr0 = NULL;
    }
    else{ /* child match,  use that */
	if (mrc->mr_len == 1)
	    co_flags_set(co_match, CO_FLAGS_MATCH);
	*mrp = mrc;
	if (mrcprev == mrc)
	    mrcprev = NULL;
	mrc = NULL;
    }
    if (*mrp && (*mrp)->mr_parsetree == ptn){
	ptn = NULL; /* passed to upper layers, dont free here */
    }
 ok:
    retval = 0;
 done:
    if (mrcprev && mrcprev != mrc){
	assert(mrcprev != *mrp);
	mr_free(mrcprev);
    }
    if (ptn)
	pt_free(ptn, 0);
    if (mrc)
	mr_free(mrc);
    if (mr0)
	mr_free(mr0);
    return retval;   
} /* match_pattern_sets */

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
 * @param[out] ptmatch   Returns the parsetree at the place of matching
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
	      parse_tree  **ptmatch, 
	      int          *matchvec[],
	      int          *matchlen, 
	      cvec         *cvv,
	      cvec         *cvvall,
	      char        **reasonp)
{
    int retval = -1;
    match_result *mr = NULL;
    
    if (ptmatch == NULL || cvt == NULL || cvr == NULL || matchvec == NULL || matchlen == NULL){
	errno = EINVAL;
	goto done;
    }
    *matchlen = 0;

    if (match_pattern_sets(h, cvt, cvr,
			   pt,
			   0,
			   best, hide, expandvar,
			   cvv, cvvall,
			   &mr) < 0)
	goto done;
    /* Clear all CO_FLAGS_MATCH recursively */
    pt_apply(pt, co_clearflag, (void*)CO_FLAGS_MATCH);
#if 1 /* XXX: should move up to callers? */
    if (mr){
	if (!last_level(cvt, mr->mr_level)){
	    cg_obj *co_match;
	    char *r;
	    if (mr->mr_len == 1){
		co_match = pt_vec_i_get(mr->mr_parsetree,mr->mr_vec[0]);
		if (co_match->co_type == CO_VARIABLE && ISREST(co_match))
		    ;
		else{
		    if (mr->mr_reason == NULL){ /* If pre-existing error reason use that */
			if ((r = strdup("Unknown command")) == NULL) /* else create unknown error */
			    goto done;
			mr_reason_set(mr, r);
		    }
		    mr_vec_reset(mr);
		}
	    }
	    else {
		if ((r = strdup("Unknown command")) == NULL)
		    goto done;
		mr_reason_set(mr, r);
		mr_vec_reset(mr);
	    }
	}
	*ptmatch = mr->mr_parsetree;
	*matchvec = mr->mr_vec;
	*matchlen = mr->mr_len;
	if (reasonp){
	    *reasonp = mr->mr_reason;
	}
	else if (mr->mr_reason)
	    free(mr->mr_reason);
	free(mr);
    }
#endif
    retval = 0;
 done:
    return retval;
} /* match_pattern */

/*! CLIgen object matching function for exact match
 * @param[in]  h         CLIgen handle
 * @param[in]  string    Input string to match
 * @param[in]  cvt       Tokenized string: vector of tokens
 * @param[in]  cvr       Rest variant,  eg remaining string in each step
 * @param[in]  pt        CLIgen parse tree, vector of cligen objects.
 * @param[in]  expandvar Set if VARS should be expanded, eg ? <tab>
 * @param[out] cvv       CLIgen variable vector containing vars for matching path
 * @param[out] cvvall    CLIgen variable vector containing vars and constants for matching vars
 * @param[out] match_obj Exact object to return
 * @param[out] ptmatch   Expanded parse tree in which match_obj occurs
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
		    cvec          *cvvall,
		    cg_obj       **match_obj,
		    parse_tree   **ptmatchp,
		    cligen_result *resultp,
		    char         **reason)
{
    int           retval = -1;
    parse_tree   *ptmatch = NULL;
    cg_obj       *co = NULL;
    int          *matchvec = NULL;
    int           matchlen = -1; /* length of matchvec */
    int           i;
    parse_tree   *ptc;

    if ((match_pattern(h, cvt, cvr,
		       pt,
		       1, /* best: Return only best option */
		       0, 1,
		       &ptmatch, 
		       &matchvec,
		       &matchlen, 
		       cvv, cvvall,
		       reason)) < 0)
	goto done;
    assert(matchlen != -1);
    /* If no match fix an error message */
    if (matchlen == 0){
	if (reason && *reason == NULL){
	    if ((*reason = strdup("Unknown command")) == NULL)
		goto done;
	}
    }
    else if (matchlen > 1){ /* There is some magic to this. Collapse many choices to one
			     * if all alternatives are variables.
			     */
	    int j;
	    int allvars = 1;
	    char *string1;
	    string1 = cvec_i_str(cvt, cligen_cvv_levels(cvt)+1);
	    for (j=0; j<matchlen; j++){
		co = pt_vec_i_get(ptmatch,matchvec[j]);
		/* XXX If variable dont compare co_command */
		if (co->co_type == CO_COMMAND && string1 && 
		    strcmp(string1, co->co_command)==0){
		    matchlen = 1;
		    matchvec[0] = matchvec[j];
		    break;
		}
		if (co->co_type != CO_VARIABLE)
		    allvars = 0; /* should mean onlyvars*/
	    }
	    if (allvars && cligen_preference_mode(h)){
		matchlen = 1; /* choose first element */
	    }
    }
    /* Only a single match at this point */
    if (matchlen != 1)
	goto ok;
    /* Here we have an obj that is unique so far. We need to see if there is only one sibling to it. */
    co = pt_vec_i_get(ptmatch, *matchvec);
    /*
     * Special case: if a NULL child is not found, then set result == GC_NOMATCH
     */
    if ((ptc = co_pt_get(co)) != NULL){
	for (i=0; i<pt_len_get(ptc); i++){
	    if (pt_vec_i_get(ptc, i) == NULL)
		break; /* If we match here it is OK, unless no match */
	}
	if (pt_len_get(ptc) != 0 && i==pt_len_get(ptc)){
	    co = NULL;
	    if (reason){
		if (*reason != NULL)
		    free(*reason);
		if ((*reason = strdup("Incomplete command")) == NULL)
		    goto done;
	    }
	    matchlen = 0;
	}
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
    if (ptmatchp){
	*ptmatchp = ptmatch;
	ptmatch = NULL;
    }
    retval = 0;
 done:
    if (ptmatch && pt != ptmatch)
	pt_free(ptmatch, 0);
    if (matchvec)
	free(matchvec);
    return retval;
} /* match_pattern_exact */

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
    parse_tree *ptmatch = NULL; 
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
		      &ptmatch, 
		      &matchvec, &matchlen,
		      cvv, NULL,
		      NULL) < 0)
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
	co = pt_vec_i_get(ptmatch, mv);
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
	if ((*stringp = realloc(*stringp, *slenp)) == NULL)
	    goto done;
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
    if (ptmatch && pt != ptmatch)
	pt_free(ptmatch, 0);
    if (cvt)
	cvec_free(cvt);
    if (cvr)
	cvec_free(cvr);
    if (matchvec)
	free(matchvec);
    return retval;
}

