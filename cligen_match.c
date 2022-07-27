/*
  CLI generator match functions, used in runtime checks.

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
#include "cligen_pt_head.h"
#include "cligen_callback.h"
#include "cligen_object.h"
#include "cligen_print.h"
#include "cligen_handle.h"
#include "cligen_expand.h"
#include "cligen_result.h"
#include "cligen_read.h"
#include "cligen_match.h"

#ifndef MIN
#define MIN(x,y) ((x)<(y)?(x):(y))
#endif

/* Development debugging for sets matching */
#undef _DEBUG_SETS

/*! Match variable against input string
 * 
 * @param[in]  h       CLIgen handle
 * @param[in]  co      cligen object
 * @param[in]  str     Input string to match
 * @param[out] reason  if not match and co type is 0, reason points to a (malloced) err string
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
    if ((retval = cv_validate(h, cv, cs, co->co_command, reason)) <= 0)
	goto done;
    /* here retval should be 1 */
  done:
    if (cv)
	cv_free(cv);
    return retval; 
}

/*! Given a string and one cligen object, return if the string matches
 *
 * @param[in]  h       CLIgen handle
 * @param[in]  str     Input string to match (NULL is match)
 * @param[in]  co      cligen object
 * @param[in]  best    Only return best match (for command evaluation) instead of all possible options
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
	     int           best, 
	     int          *exact,
	     char        **reason)
{
  int    match = 0;
  size_t len = 0;

  if (str)
      len = strlen(str);
  if (exact)
      *exact = 0;
  if (co==NULL) /* shouldnt happen */
      return 0;
  switch (co->co_type){
  case CO_COMMAND:
      if (str == NULL)
	  match++;
      else{
	  if (best && *co->co_command == '\"'){ /* escaped */
	      if (cligen_caseignore_get(h))
		  match = (strncasecmp(co->co_command+1, str, len) == 0);
	      else
		  match = (strncmp(co->co_command+1, str, len) == 0);
	      if (exact)
		  *exact = strlen(co->co_command+1) == len;
	  }
	  else{
	      if (cligen_caseignore_get(h))
		  match = (strncasecmp(co->co_command, str, len) == 0);
	      else
		  match = (strncmp(co->co_command, str, len) == 0);
	      if (exact)
		  *exact = strlen(co->co_command) == len;
	  }
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
  case CO_EMPTY: /* Nothing match with empty (same as co = NULL) */
      if (reason){
	  if ((*reason = strdup("Empty")) == NULL)
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
    
    levels = cligen_cvv_levels(cvt);
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
    size_t  len;

    len = pt_len_get(pt);
    if (len == 0)
	return 1;
    for (i=0; i<len; i++){
	if ((co = pt_vec_i_get(pt, i)) == NULL ||
	    co->co_type == CO_EMPTY)
	    return len==1?1:2;
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
 *
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
    cv_name_set(cv, co->co_command);
    if (co->co_vtype == CGV_DEC64) /* XXX: Seems misplaced? / too specific */
	cv_dec64_n_set(cv, co->co_dec64_n);
    if (cv_parse(cmd, cv) < 0) {
	cv_reset(cv);
	cvec_del(cvv, cv);
	return NULL;
    }
    //    if (co->co_show)
    //	cv->var_show = strdup4(co->co_show);
    /* If translator function defined, here translate value */
    if (co->co_translate_fn != NULL &&
	co->co_translate_fn(h, cv) < 0)
	return NULL;
    return cv;
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
	  int           lasttoken,
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
    cg_obj *cop = NULL;
    int     match;

    /* Loop through parse-tree at this level to find matches */
    for (i=0; i<pt_len_get(pt); i++){
	if ((co = pt_vec_i_get(pt, i)) == NULL)
	    continue;
	/* Return -1: error, 0: nomatch, 1: match */
	tmpreason = NULL;
	if ((match = match_object(h,
				  ISREST(co)?resttokens:token,
				  co, best, &exact,
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
	/* An alternative is to sort away these after the call in match_pattern_sets_local */
	else if (co_flags_get(co, CO_FLAGS_MATCH)){
	    p = 1; /* XXX lower than any variables*/
	    if (p < pref_lower){
		char *r;
		pref_lower = p;
		if ((r = strdup("Already matched")) == NULL)
		    goto done;
		mr_reason_set(mr, r);
	    }
	}
	else { /* Match: if best compare and save highest preference */
	    assert(tmpreason == NULL);
	    if (best){ /* only save best match */
		if (p == pref_upper){
		    if (cligen_preference_mode(h) == 1 &&
			lasttoken &&
			cop &&
			cop->co_type == CO_VARIABLE && 
			co->co_type == CO_VARIABLE) /* Skip terminal pref if preference mode */
			;
		    else if (cligen_preference_mode(h) == 2 &&
			!lasttoken &&
			cop->co_type == CO_VARIABLE && 
			co->co_type == CO_VARIABLE) /* Skip same pref if preference mode */
			;
		    else if (cligen_preference_mode(h) == 3 &&
			cop->co_type == CO_VARIABLE && 
			co->co_type == CO_VARIABLE) /* Skip same pref if preference mode */
			;
		    else
			if (mr_pt_append(mr, co, ISREST(co)?resttokens:token) < 0)
			    goto done;
		}
		else if (p > pref_upper){ /* Start again at this level */
		    pref_upper = p;
		    if (mr_pt_reset(mr) < 0)
			goto done;
		    if (mr_pt_append(mr, co, ISREST(co)?resttokens:token) < 0)
			goto done;
		    cop = co;
		}
		else{ /* p < pref_upper : skip */
		}
	    } /* if best */
	    else {
		if (mr_pt_append(mr, co, ISREST(co)?resttokens:token) < 0)
		    goto done;
	    }
	} /* switch match */
	assert(tmpreason == NULL);
    } /* for pt_len_get(pt) */
    /* Only return reason if matches == 0 */
    if (mr_pt_len_get(mr) != 0)
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
 */
static int
match_bindvars(cligen_handle h,
	       cg_obj       *co,
	       char         *token,
	       cvec         *cvv)
{
    int     retval = -1;
    cg_var *cv = NULL;
    cg_obj *co_orig;

    /* co_orig is original object in case of expansion */
    if ((co_orig = co->co_treeref_orig) == NULL)
	co_orig = co->co_ref;
    if (co->co_type == CO_VARIABLE){
	/* Once so translate only is done once */
	if ((cv = add_cov_to_cvec(h, co, token, cvv)) == NULL)
	    goto done;
    }
    else if (co->co_type == CO_COMMAND && co_orig->co_type == CO_VARIABLE){
	/* Once so translate only is done once */
	if ((cv = add_cov_to_cvec(h, co_orig,
				  co->co_value?co->co_value:co->co_command,
				  cvv)) == NULL)
	    goto done;
    }
    else{
	if ((cv = cvec_add(cvv, co->co_vtype)) == NULL)
	    goto done;
	cv_name_set(cv, co->co_command);
	cv_type_set(cv, CGV_STRING);
	cv_string_set(cv, co->co_command);
	cv_const_set(cv, 1);
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
 * @param[out]    mrp       Match result including how many matches, level, reason for nomatc, etc
 * @param[in,out] cvv       cligen variable vector containing vars/values pair for completion
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
			 cvec         *cvv,
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
    mr_level_set(mr0, level);

    /* How many matches of cvt[level+1] in pt */
    if (match_vec(h,
		  pt, token, resttokens,
		  lasttoken,
		  lasttoken?best:1, /* use best preference match in non-terminal matching*/
		  mr0) < 0)
	goto done;
    /* Number of matches is 0 (no match), 1 (exact) or many */
    switch (mr_pt_len_get(mr0)){
    case 0: /* no matches */
	if (pt_onlyvars(pt))
	    ; /* XXX Uuh mr0 already has a reason,... */
	mr_pt_reset(mr0);
	goto ok; 
	break;
    case 1: /* exactly one match */
	break;
    default: /* multiple matches:
	      * note that there is code in match_patter_exact that can collapse multiple matches 
	      * to one under certain circumstances
	      */
	if (lasttoken){
	    if (best){
		co_match = mr_pt_i_get(mr0, 0);
		if (match_bindvars(h, co_match, 
				   ISREST(co_match)?resttokens:token,
				   cvv) < 0)
		    goto done;
	    }
	    goto ok; /* will return matches > 1 */ 
	}
	mr_pt_reset(mr0);
	mr_reason_set(mr0, strdup("Ambiguous command"));
	goto ok;  /* will return matches = 0 */
	break;
    } /* switch matches */
    if (mr_pt_len_get(mr0) != 1){
	errno = EFAULT; /* shouldnt happen */
	goto done;
    }
    /* Get the single match object */
    co_match = mr_pt_i_get(mr0, 0);
    /* co_orig is original object in case of expansion */
    co_orig = co_match->co_ref?co_match->co_ref: co_match;

    /* Already matched (sets functionality) */
    if (co_flags_get(co_match, CO_FLAGS_MATCH)){ /* XXX: orig?? */
	char *r;
	if ((r = strdup("Already matched")) == NULL)
	    goto done;
	mr_reason_set(mr0, r);
	mr_pt_reset(mr0);
	goto ok; /* will return matches = 0 */
    }

    /* Do it if not last or best */
    if (!lasttoken || best){
	/* Bind vars and constants to variable vectors used for completion and callbacks */
	if (match_bindvars(h, co_match, 
			   ISREST(co_match)?resttokens:token,
			   cvv) < 0)
	    goto done;
    }
    if (lasttoken ||
	(co_match->co_type == CO_VARIABLE && ISREST(co_match))){
	/* 
	 * Special case: we have matched a REST variable (anything) and
	 * there is more text have this word, then we can match REST
	 * This is "inline" of match_terminal
	 */
	mr_last_set(mr0); /* dont go to children */
    }
 ok: 
    /* mr0:local or mrc:child
     * if mrc has result, take that, otherwise take mr0
     */
    switch (mr_pt_len_get(mr0)) {
    case 0:
	break;
    case 1:
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
 * @param[in]     best      If set, only return best match (for command evaluation) instead of 
 *                          all possible options. Match also hidden options.
 *                          If not set, return all possible matches, do not return hidden options 
 * @param[in,out] cvv       cligen variable vector containing vars/values pair for completion
 * @param[out]    callbacks Callback structure of expanded treeref
 * @param[out]    mrp       Match result including how many matches, level, reason for nomatc, etc
 * @retval        0         OK. result returned in mrp
 * @retval        -1        Error
 * Note: parameter "best" is only set in call from match_pattern_exact().
 */
static int 
match_pattern_sets(cligen_handle h, 
		   cvec         *cvt,
		   cvec         *cvr,
		   parse_tree   *pt,
		   int           level,
		   int           best,
		   cvec         *cvv,
		   cg_callback **callbacks,
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
#ifdef _DEBUG_SETS
    fprintf(stderr, "%s %*s level: %d token:%s\npt:\n", __FUNCTION__, level*3,"",
		level, strlen(token)?token:"\"\"");
    pt_print(stderr, pt);
#endif
    /* Match the current token */
    if (match_pattern_sets_local(h, cvt, cvr, pt, level, best, 
				 cvv, &mr0) < 0)
	goto done;
#ifdef _DEBUG_SETS
    fprintf(stderr, "%s %*s matchnr:%d\n", __FUNCTION__, level*3,"", mr_pt_len_get(mr0));
#endif
    if (mr_pt_len_get(mr0) != 1){ /* If not unique match exit here */
	*mrp = mr0;
	mr0 = NULL;
	goto ok;
    }
    /* Unique match */
    co_match = mr_pt_i_get(mr0, 0);

    /* If instantiated tree reference copy the callbacks 
     * See also callbacks code in pt_expand1_co
     * This code may need refactoring
     */
    if (callbacks &&
	co_flags_get(co_match, CO_FLAGS_TREEREF)){
	cg_obj *coref = co_match;
	while (coref->co_ref){
	    coref = coref->co_ref;
	}
	if (coref->co_type ==  CO_REFERENCE &&
	    coref->co_callbacks)
	    if (co_callback_copy(coref->co_callbacks, callbacks) < 0)
		goto done;
    }
#ifdef _DEBUG_SETS
    fprintf(stderr, "%s %*s match co:%s\n", __FUNCTION__, level*3,"", co_match->co_command);
#endif
    if (mr_last_get(mr0) && (strcmp(token,"") != 0)){
	mr_flags_set_co_match(mr0, co_match);
	*mrp = mr0;
	mr0 = NULL;
	goto ok;
    }
    if ((ptn = pt_new()) == NULL)
	goto done;
    if (pt_expand(h,
		   co_match,
		   co_pt_get(co_match),
		   cvv,
		  !best,  /* If best is set, include hidden commands, otherwise do not */
		  1,      /* VARS are expanded, eg ? <tab> */
		  ptn) < 0) /* expand/choice variables */
	goto done;
    /* Check termination criteria */
    lastsyntax = last_pt(ptn); /* 0, 1 or 2 */
    switch (lastsyntax){
    case 0: /* Not last in syntax tree, continue */
	break;
    case 1: /* Last in syntax tree (not token) */
	mr_flags_set_co_match(mr0, co_match);
	*mrp = mr0;
	mr0 = NULL;
	goto ok;
	break;
    case 2: /* Last in syntax tree but can continue,... */
	break;
    }
    if (pt_sets_get(ptn)){ /* For sets, iterate */
#ifdef _DEBUG_SETS
	fprintf(stderr, "%s %*s sets:\n", __FUNCTION__, level*3,"");
#endif
	while (!last_level(cvt, level)){
	    if (mrc != NULL)
		mrc = NULL;
	    if (match_pattern_sets(h, cvt, cvr, ptn,
				   level+1,
				   best, 
				   cvv,
				   callbacks,
				   &mrc) < 0)
		goto done;		
#ifdef _DEBUG_SETS
	    fprintf(stderr, "%s %*s sets matchnr: %d\n", __FUNCTION__, level*3,"", mr_pt_len_get(mrc));
#endif
	    if (mr_pt_len_get(mrc) != 1)
		break;
#ifdef _DEBUG_SETS
	    fprintf(stderr, "%s %*s sets match co: %s\n", __FUNCTION__, level*3,"",
		    mr_pt_i_get(mrc, 0)->co_command);
#endif
	    if (mrcprev != NULL){
		mr_free(mrcprev);
		mrcprev = NULL;
	    }
	    mrcprev = mrc;
	    level = mr_level_get(mrc); /* XXX level always 0 */
	} /* while */
	if (mrc == NULL){
	    *mrp = mr0;
	    mr0 = NULL;
	    goto ok;
	}
    }
    else{
	if (last_level(cvt, level)){
	    *mrp = mr0;
	    mr0 = NULL;
	    goto ok;    
	}
	else if (match_pattern_sets(h, cvt, cvr, ptn,
				    level+1, 
				    best, 
				    cvv,
				    callbacks,
				    &mrc) < 0)
	    goto done;
    }
    /* Clear all CO_FLAGS_MATCH recursively 
     * Only co_match is set with CO_FLAGS_MATCH
     */
    pt_apply(ptn, co_clearflag, 1, (void*)CO_FLAGS_MATCH);
    
    /* If child match fails, use previous */
    if (mr_pt_len_get(mrc) == 0 && mrcprev){
	/* Transfer match flags from ptn to pt if this tree has no more matches */
	mr_flags_set_co_match(mr0, co_match);
	mr_mv_reason(mrc, mrcprev); 	/* transfer error reason if any from child */
	*mrp = mrcprev;
	mrcprev = NULL;
    }
    else if (mr_pt_len_get(mrc) == 0 && lastsyntax == 2){ /* If no child match, then use local */
	mr_flags_set_co_match(mr0, co_match);
	mr_mv_reason(mrc, mr0); 	/* transfer error reason if any from child */
	*mrp = mr0;
	mr0 = NULL;
    }
    else{ /* child match,  use that */
	if (mr_pt_len_get(mrc) == 1)
	    mr_flags_set_co_match(mr0, co_match);
	*mrp = mrc;
	if (mrcprev == mrc)
	    mrcprev = NULL;
	mrc = NULL;
    }
 ok:
    retval = 0;
 done:
    if (mrcprev && mrcprev != mrc){
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
 * @param[in]  cvt       Tokenized string: vector of tokens
 * @param[in]  cvr       Rest variant,  eg remaining string in each step
 * @param[in]  pt        Vector of commands (array of cligen object pointers (cg_obj)
 * @param[in]  best      If set, only return best match (for command evaluation) instead of 
 *                       all possible options. Match also hidden options. Only from match_pattern_exact
 *                       If not set, return all possible matches, do not return hidden options 
 * @param[out] cvv       cligen variable vector containing vars/values pair for completion
 * @param[out] callbacks Callback structure of expanded treeref
 * @param[out] mrp       CLIgen match result struct encapsulating several return parameters
 * @retval    -1         Error
 * @retval     0         OK
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
	      cvec         *cvv,
	      cg_callback **callbacks,
	      match_result **mrp)
{
    int           retval = -1;
    match_result *mr = NULL;
    char         *r;
    cg_obj       *co;
    int           i;
    cg_obj       *co_match;
    cg_obj       *co1 = NULL;
    parse_tree   *ptc;
    
    if (cvt == NULL || cvr == NULL || mrp == NULL){
	errno = EINVAL;
	goto done;
    }
    if (match_pattern_sets(h, cvt, cvr,
			   pt,
			   0,
			   best, 
			   cvv, 
			   callbacks,
			   &mr) < 0)
	goto done;
    if (mr == NULL){  /* shouldnt happen */
	errno = EFAULT;
	goto done;
    }
    /* Clear all CO_FLAGS_MATCH recursively */
    pt_apply(pt, co_clearflag, 1, (void*)CO_FLAGS_MATCH);
    /* Lots of complex code follows, 
     * Good news is that these have been moved from calling functions to here
     * Hopefully it may be easier to simplify
     */
    /* Intermediate match, have not matched whole command set, stopped short. 
     * Cases:
     * 1) Single match: 
     *      - special case: can accept if REST command, 
     *      - otherwise set to no match
     * 2) Multiple match: set no match
     */
    if (!last_level(cvt, mr_level_get(mr))){ /* XXX level always 0 */
	if (mr_pt_len_get(mr) == 1){
	    co_match = mr_pt_i_get(mr, 0);
	    if (co_match->co_type == CO_VARIABLE && ISREST(co_match))
		;
	    else if (ISREST(co_match))
		;
	    else{
		if (mr_reason_get(mr) == NULL){ /* If pre-existing error reason use that */
		    if ((r = strdup("Unknown command")) == NULL) /* else create unknown error */
			goto done;
		    mr_reason_set(mr, r);
		}
		mr_pt_reset(mr);
	    }
	}
	else { /* */
	    if (mr_reason_get(mr) == NULL){
		if ((r = strdup("Unknown command")) == NULL)
		    goto done;
		mr_reason_set(mr, r);
		mr_pt_reset(mr);
	    }
	}
    }
    /* If multiple match (at final command), collapse to single command if:
     * 1) All are commands and commands are equal
     */
    if (mr_pt_len_get(mr) > 1){ 
	char *string1;

	/* Collapse many choices to one
	 * if all alternatives are equal commands */
	string1 = NULL;
	for (i=0; i<mr_pt_len_get(mr); i++){
	    co = mr_pt_i_get(mr, i);
	    if (co->co_type != CO_COMMAND)
		break;
	    if (i == 0)
		string1 = co->co_command;
	    else if (string1){
		if ((!cligen_caseignore_get(h) && strcmp(string1, co->co_command)==0) ||
		    (cligen_caseignore_get(h) && strcasecmp(string1, co->co_command)==0))
		    ;
		else
		    break;
	    }
	}
	if (string1 != NULL && i==mr_pt_len_get(mr)) /* No break in loop */
	    if (mr_pt_trunc(mr, 1))
		goto done;
    }
    /* Final check:
     * 1) If no match ensure there is an error message
     * 2) If single match, ensure there is a NULL child (eg ";" in cligen syntax)
     *    - if not set to no match
     */
    switch (mr_pt_len_get(mr)){
    case 0:
	/* If no match fix an error message */
	if (mr_pt_len_get(mr) == 0){
	    if (mr_reason_get(mr) == NULL){
		if ((r = strdup("Unknown command")) == NULL)
		    goto done;
		mr_reason_set(mr, r);
	    }
	}
	break;
    case 1:
	/* Here we have an obj that is unique so far. We need to see if there is 
	 * only one sibling to it. */
	co1 = mr_pt_i_get(mr, 0);
	/*
	 * Special case: if a NULL child is not found, then set result == CG_NOMATCH
	 */
	if ((ptc = co_pt_get(co1)) != NULL && best){
	    parse_tree *ptn;
	    cvec       *cvv;

	    if ((ptn = pt_new()) == NULL)
		goto done;
	    if ((cvv = cvec_new(0)) == NULL)
		goto done;
	    if (pt_expand(h, co1, ptc, cvv, 1, 0, ptn) < 0)
		goto done;
	    /* Loop sets i which is used below */
	    for (i=0; i<pt_len_get(ptn); i++){
		if ((co = pt_vec_i_get(ptn, i)) == NULL ||
		    co->co_type == CO_EMPTY)
		    break; /* If we match here it is OK, unless no match */
	    }
	    if (pt_len_get(ptn)==0 ||
		(pt_len_get(ptn) != 0 && i == pt_len_get(ptn))){
		co1 = NULL;
		if ((r = strdup("Incomplete command")) == NULL)
		    goto done;
		mr_reason_set(mr, r);
		mr_pt_reset(mr);
	    }
	    pt_expand_cleanup(h, ptn);
	    pt_free(ptn, 0);
	    cvec_free(cvv);
	}
	break;
    default:
	break;
    } /* switch */
    *mrp = mr;
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
 * @param[out] cvv       CLIgen variable vector containing vars for matching path
 * @param[out] match_obj Exact object to return, must be freed by caller
 * @param[out] callbacks Callback structure of expanded treeref
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
		    cvec          *cvv,
		    cg_obj       **match_obj,
		    cg_callback  **callbacks,
		    cligen_result *resultp,
		    char         **reason
		    )
{
    int           retval = -1;
    match_result *mr = NULL;
    cg_obj       *co;
    
    if (resultp == NULL){
	errno = EINVAL;
	goto done;
    }
    if ((match_pattern(h,
		       cvt, cvr, /* token string */
		       pt,       /* command vector */
		       1,        /* best: Return only best option including hidden options */
		       cvv,
		       callbacks,
		       &mr)) < 0){
	goto done;
    }
    if (mr == NULL){ /* shouldnt happen */
	errno = EFAULT;
	goto done;
    }
    if (reason &&
	mr_reason_get(mr))
	*reason = strdup(mr_reason_get(mr));
    if (mr_pt_len_get(mr) == 1 &&
	match_obj){
	/* Must make a copy since the mr will be freed */
	co = mr_pt_i_get(mr, 0);
	if (co_copy1(co, NULL, 0, 0x0, match_obj) < 0)
	    goto done;
    }
    *resultp = mr2result(mr);
    retval = 0;
 done:
    if (mr)
	mr_free(mr); 
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
    int      slen = 0;
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
    int      append = 0; /* Has appended characters */
    int      retval = -1;
    size_t   len;
    match_result *mr = NULL;
    char    *command;
    char    *command1 = NULL; /* prev */

    /* ignore any leading whitespace */
    string = *stringp;
    /* Tokenize the string and transform it into two CLIgen vectors: tokens and rests */
    if (cligen_str2cvv(string, &cvt, &cvr) < 0)
	goto done;
    s = string;
    while ((strlen(s) > 0) && isblank(*s))
	s++;
    if (match_pattern(h, cvt, cvr,
		      pt,
		      0, /* best: Return all options, not only best, exclude hidden options */
		      cvv, 
		      NULL,
		      &mr) < 0)
	goto done;
    if (mr == NULL || mr_pt_len_get(mr) == 0){
	retval = 0;
	goto done; /*  No matches */
    }
    equal = 1;
    for (i=0; i<mr_pt_len_get(mr); i++){
	co = mr_pt_i_get(mr, i);
	if (co == NULL){
	    retval = 0;
	    goto done;
	}
	if ((cligen_tabmode(h) & CLIGEN_TABMODE_VARS) == 0){
	    if (co->co_type != CO_COMMAND)
		continue;
	}
	command = co->co_value?co->co_value:co->co_command;
	if (co1 == NULL){
	    slen = strlen(mr_token_get(mr));
	    minmatch = strlen(command);
	    co1 = co;
	    command1 = command;
	}
	else{
	    command1 = co1->co_value?co1->co_value:co1->co_command;
	    if (!cligen_caseignore_get(h) && strcmp(command1, command)==0)
		; /* equal */
	    else if (cligen_caseignore_get(h) && strcasecmp(command1, command)==0)
		; /* equal */
	    else{
		equal = 0;
		len = MIN(strlen(command1), strlen(command));
		for (j=0; j<len; j++)
		    if (command1[j] != command[j])
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
    strncat(string, &command1[slen], minmatch-slen);
    append = append || minmatch-slen;
    if (equal){ /* add space */
	string[strlen(string)+1] = '\0';
	string[strlen(string)] = cligen_delimiter(h);
	*slenp = 0;
	co1 = NULL;
    }
    retval = append?1:0;
  done:
    if (cvt)
	cvec_free(cvt);
    if (cvr)
	cvec_free(cvr);
    if (mr){
	mr_free(mr);
    }
    return retval;
}

