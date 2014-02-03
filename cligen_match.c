/*
  CLI generator match functions, used in runtime checks.
  CVS Version: $Id: cligen_match.c,v 1.20 2013/04/12 10:59:52 olof Exp $ 

  Copyright (C) 2001-2013 Olof Hagsand

  This file is part of CLIgen.

  CLIgen is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  CLIgen is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with CLIgen; see the file COPYING.
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

#include "cligen_var.h"
#include "cligen_cvec.h"
#include "cligen_gen.h"
#include "cligen_handle.h"
#include "cligen_expand.h"
#include "cligen_match.h"

#ifndef MIN
#define MIN(x,y) ((x)<(y)?(x):(y))
#endif


/*
 * match_variable
 * INPUT:
 *   string    Input string to match
 *   pvt       variable type (from definition)
 *   cmd       variable string (from definition) - can contain range
 * RETURNS:
 *   -1        Error (print msg on stderr)
 *   0         Not match and reason returned as malloced string.
 *   1         Match
 * Who prints errors?
 */
static int
match_variable(cg_obj *co, char *str, char **reason)
{
    int         retval = -1;
    cg_var     *cv;
    cg_varspec *cs;

    cs = &co->u.cou_var;
    if ((cv = cv_new(co->co_vtype)) == NULL)
	goto done;
    if ((retval = cv_parse1(str, cv, reason)) <= 0) 
	goto done;
    retval = cv_validate(cv, cs, reason);
  done:
    if (cv)
	cv_free(cv);
    return retval; 
}

/*
 * match_object
 * INPUT:
 *   string    Input string to match
 *   co        cligen object
 * OUTPUT
 *   exact:    1 if match is exact (CO_COMMANDS). VARS is 0.
 *   reason:   if not match and co type is 0, reason points to a (malloced) string 
 *             containing an error explanation string. If reason is NULL no such
 *             string will be malloced. This string needs to be freed.
 * RETURNS:
 *   -1        Error
 *   0         Not match
 *   1         Match
 * Given a string and one cligen object, return if the string matches
 * the object. Note, if string = NULL it is a match. 
 */
static int 
match_object(char *string, cg_obj *co, int *exact, char **reason)
{
  int match = 0;

  if (string==NULL)
      return 1;
  if (co==NULL)
      return 0;
  switch (co->co_type){
  case CO_COMMAND:
    match = (strncmp(co->co_command, string, strlen(string)) == 0);
    *exact = strlen(co->co_command) == strlen(string);
    break;
  case CO_VARIABLE:
      if ((match = match_variable(co, string, reason)) < 0)
	  return -1;
      *exact = 0; /* N/A */ 
    break;
  case CO_REFERENCE:
      break;
  }
  return match!=0 ? 1 : 0;
}

/*
 * match_perfect
 */
static int
match_perfect(char *string, cg_obj *co)
{
  return  ((co->co_type==CO_COMMAND) &&
	   (strcmp(string, co->co_command)==0));
}

/*
 * next_token
 * Given a string (s0), return the next token. The string is modified to return
 * the remainder of the string after the identified token.
 * A token is found either as characters delimited by one or many delimiters.
 * Or as a pair of double-quotes(") with any characters in between.
 * if there are trailing spaces after the token, trail is set to one.
 * If string is NULL or "", NULL is returned.
 * If empty token found, s0 is NULL
 * Example:
 *   s0 = "  foo bar"
 * results in token="foo", leading=1
 * INOUT
 *   s0     - string, the string is modified like strtok
 * OUT:
 *  token0  - A malloced token.  NOTE: token must be freed after use.
 *  leading0  - if leading delimiters eg " thisisatoken"
 * 
 */
static int
next_token(char **s0, char **token0, int *leading0)
{
  char *s;
  char *st;
  char *token = NULL;
  size_t len;
  int quote=0;
  int leading=0;

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
  if (*s && index(CLIGEN_QUOTES, *s) != NULL){
    quote++;
    s++;
  }
  st=s; /* token starts */
  for (; *s; s++){ /* Then find token */
    if (quote){
      if (index(CLIGEN_QUOTES, *s) != NULL)
	break;
    }
    else
      if (index(CLIGEN_DELIMITERS, *s) != NULL)
	break;
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

/*
 * command_levels
 * Returns the number of "levels" of a command.
 * A level is an atomic command delimetered by space or tab.
 * Eg: "", "a", "abcd" has level 0
 *     "abcd ", "vb fg" has level 1
 *     "abcd gh ", "vb fg hjsa" has level 2
 * RETURNS:
 *  -1      Error (XXX: not checked)
 * XXX: Use " and escape characters to group strings with space.
 */
int
command_levels(char *string)
{
    int i = 0;
    char *ss = strdup(string);
    char *ss0;
    char *t;
    int trail;

    ss0=ss;
    if (next_token(&ss, &t, &trail) < 0)
	return -1;
    while (t){
	free(t);
	if (next_token(&ss, &t, &trail) < 0)
	    return -1;
	if (t == NULL){
	    if (trail)
		i++;
	    break;
	}
	i++;
    }
    free(ss0);
#if 0
    {
	int trail;
	char *s0 = strdup(string);  
	char *s = s0;
	int len = strlen(s0);

	i = 0;
	if (s0 == NULL){
	    perror("command_levels: strdup");
	    return -1;
	}
	s = strtok(s0, CLIGEN_DELIMITERS);
	while (s){
	    /* Check for extra trailing delimiters */
	    if (strlen(s)){
		if (s[0] == '\"'){
		    if (strlen(s)>1 && s[strlen(s)-1] == '\"')
			; /* single */
		    else
			; /* find quote in s0 */
		}
	    }
	    trail = ((s + strlen(s) ) < (s0 + len)); 
	    s = strtok(NULL, CLIGEN_DELIMITERS);
	    if (s == NULL){
		if (trail)
		    i++;
		break;
	    }
	    i++;
	}
	free(s0);
	fprintf(stderr, "Should be i=%d\n", i);
    }
#endif
    return i;
}

/*
 * extract_substring
 * INPUT:
 *   string0    Input string to match
 *   level      Which substring to extract
 * RETURNS:
 *   0          OK
 *   -1         Error
 * OUTPUT:
 *   sp         malloc:d extracted sub-string
 * Returns a substring of a command at a specific "level".
 * Eg:     extract_substring("abcd gh foo", 1, &s)
 * returns a pointer to the (malloc:d) string "gh".
 * NOTE: the caller must free the pointer after use.
 * XXX: Use " and escape characters to group strings with space.
 */
int
extract_substring(char *string0, int level, char **sp)
{
    char *ss;  
    int i = 0;
    int trail;
    char *ss0;
    char *t;

    assert(sp && string0);
    *sp = NULL;

    if ((ss = strdup(string0)) == NULL){
	fprintf(stderr, "extract_substring: strdup: %s\n", strerror(errno));
	return -1;
    }
    ss0=ss;
    if (next_token(&ss, &t, &trail) < 0)
	return -1;
    while (t){
	if (i==level){
	    *sp = strdup(t);
	    free(t);
	    break;
	}
	free(t);
	if (next_token(&ss, &t, &trail) < 0)
	    return -1;
	if (t == NULL){
	    if (trail)
		i++;
	    break;
	}
	i++;
    }
    free(ss0);
//	i=0;
//	*sp = NULL;

    return 0;
}

/*
 * Similar to extract_substring, but return the rest of the string
 * at the given level.
 * Eg:     extract_substring_rest("abcd gh foo", 1, &s)
 * returns a pointer to the (malloc:d) string "gh foo".
 * NOTE: the caller must free the pointer after use.
 */
int
extract_substring_rest(char *string0, int level, char **sp)
{
    char *string;  
    int len = strlen(string0);
    int i = 0;
    char *s;
    int trail;

    assert(sp && string0);
    *sp = NULL;
    string = strdup(string0);
    s = strtok(string, CLIGEN_DELIMITERS);
    while (s){
	if (i==level){
	    *sp = strdup(string0 + (s-string));
	    break;
	}
	/* Check for extra trailing delimiters */
	trail = ((s + strlen(s) ) < (string + len)); 
	s = strtok(NULL, CLIGEN_DELIMITERS);
	if (s == NULL){
	    if (trail)
		i++;
	    break;
	}
	i++;
    }
    free(string);
    return 0;
}


/*
 * match_pattern_terminal
 * This is the last level. Multiple matches may be OK and is used
 * for completion.
 * We must have a preference when matching when it matches a command
 * and some variables.
 * The preference is:
 *  command > ipv4,mac > string > rest
 * return in matchvector which element match (their index)
 * and how many that match.
 * return value is the number of matches on return (also returned in matchlen)
 * index is the index of the first such match.
 * INPUT:
 *   h         CLIgen handle
 *   string0   Input string to match
 *   pt        Vector of commands (array of cligen object pointers (cg_obj)
 *   pt_max    Length of the pt array
 *   level     How many levels (words) into string0
 *   use_pref  Set this flag value if you want to use the preferences between
 *             matches. It is only when you want a final exact match (not 
 *             completion or show options) that you should set this.
 * RETURNS:
 *   The number of matches (0-n) in pt or -1 on error. See matchlen below.
 *
 * OUTPUT:
 *   ptp       Returns the vector at the place of matching
 *   matchv    A vector of integers containing which 
 *   matchlen  Length of matchv. That is, # of matches and same as return 
 *              value (if 0-n)
 *   reason0   If retval = 0, this may be malloced to indicate reason for not
 *             matching variables, if given. Neeed to be free:d
 */
static int 
match_pattern_terminal(cligen_handle h, 
		       char *string0, parse_tree pt,
		       int level, int use_pref,
		       pt_vec *ptp, 
		       int *matchv[], int *matchlen,
		       char **reason0
		       )
{
    char   *string;
    int     i;
    int     match;
    int     matches = 0;
    int     preference = 0;
    int     p;
    cg_obj *co, *co_match;
    cg_obj *co_orig;
    int     exact;
    char   *reason;
    int     findreason;

    co_match = NULL;
    if (level > command_levels(string0)){
	fprintf(stderr, "%s: level > command_level in %s\n",
		__FUNCTION__, string0);
	return -1;
    }
    /* If there are only variables in the list, then keep track of variable match errors */
    findreason = 0;
    if (reason0)
	for (i=0; i<pt.pt_len; i++){ 
	    if ((co = pt.pt_vec[i]) == NULL)
		continue;
	    if (co->co_type != CO_VARIABLE){
		findreason = 0;
		break;
	    }
	    findreason++;
	}
    extract_substring(string0, level, &string);
    for (i=0; i<pt.pt_len; i++){
	if ((co = pt.pt_vec[i]) == NULL)
	    continue;
	reason = NULL;
	if ((match = match_object(string, co, &exact, findreason?&reason:NULL)) < 0)
	    goto error;
	if (match){
	    assert(reason==NULL);
	    if (use_pref){
		p = co_pref(co, exact);
		if (p < preference)
		    continue; /* ignore */
		if (p > preference){
		    preference = p;
		    matches = 0; /* Start again at this level */
		}
	    }
	    *matchlen = *matchlen + 1;
	    if ((*matchv = realloc(*matchv, (*matchlen)*sizeof(int))) == NULL){
		fprintf(stderr, "%s: realloc: %s\n", __FUNCTION__, strerror(errno));
		return -1;
	    }
	    co_match = co;
	    (*matchv)[matches++] = i; 
	}
	/* match == 0, co type is variable and findreason, then reason is set 
	   this may not be the best preference, we just set the first
	 */
	if (reason){
	    if (*reason0 == NULL)
		*reason0 = reason;
	    reason = NULL;
	    findreason = 0;
	}
    }

    if (matches){
	*ptp = pt.pt_vec;
	if (reason0 && *reason0){
	    free(*reason0);
	    *reason0 = NULL;
	}
	if (matches == 1){
	    assert(co_match);

	    co_orig = co_match->co_ref?co_match->co_ref: co_match;
	    if (co_match->co_type == CO_COMMAND && co_orig->co_type == CO_VARIABLE){
		if (co_value_set(co_orig, co_match->co_command) < 0)
		    goto error;
	    }
	    /* Cleanup made on top-level */
	}
    }
    *matchlen = matches;
    if (string)
	free(string);
    return matches;
  error:
    if (string)
	free(string);
    return -1;
}

/*
 * Help function to append a cv to a cvec. For expansion cvec passed to pt_expand_2
 * IN:
 *  co     A cligen variable that has a matching value
 *  cmd    Value in string of the variable
 * OUT:
 *  cvec   The cligen variable vector to push a cv with name of co and value in cmd
 */
static cg_var *
add_cov_to_cvec(cg_obj *co, char *cmd, cvec *cvec)
{
    cg_var *cv = NULL;

    if ((cv = cvec_add(cvec, co->co_vtype)) == NULL)
	return NULL;
    cv_name_set(cv, co->co_command);
    cv_const_set(cv, iskeyword(co));
    if (cv_parse(cmd, cv) < 0) {
	cv_reset(cv);
	cvec_del(cvec, cv);
	return NULL;
    }
    return cv;
}

/*
 * match_pattern_node
 * Non-terminal. Need to match exact.
 * INPUT:
 *   h         CLIgen handle
 *   string0   Input string to match
 *   pt        Vector of commands (array of cligen object pointers (cg_obj)
 *   pt_max    Length of the pt array
 *   level     How many levels (words) into string0
 *   use_pref  Set this flag value if you want to use the preferences between
 *             matches. It is only when you want a final exact match (not 
 *             completion or show options) that you should set this.
 * RETURNS:
 *   The number of matches (0-n) in pt or -1 on error. See matchlen below.
 * OUTPUT:
 *   ptp       Returns the vector at the place of matching
 *   matchv    A vector of integers containing which 
 *   matchlen  Length of matchv. That is, # of matches and same as return 
 *              value (if 0-n)
 *   cvec      cligen variable vector containing vars/values pair for completion
 *   reason0   If retval = 0, this may be malloced to indicate reason for not
 *             matching variables, if given. Need to be free:d
 */
static int 
match_pattern_node(cligen_handle h, 
		   char *string0, 
		   parse_tree pt,
		   int level, 
		   int use_pref, 
		   int hide,
		   pt_vec *ptp, 
		   int *matchv[], 
		   int *matchlen,
		   cvec  *cvec,
		   char **reason0
		   )
{
    char *string = NULL;
    int i;
    int match;
    int matches = 0;
    int perfect = 0;
    int retval = -1;
    cg_obj *co, *co_match;
    cg_obj *co_orig;
    int rest_match = -1;
    int cmd_levels;
    int p;
    int preference = 0;
    int exact;
    char *reason;
    int findreason;
    parse_tree ptn={0,};     /* Expanded */
    cg_var *cv = NULL;

    co_match = NULL;
    if (level > command_levels(string0)){
	fprintf(stderr, "%s: level > command_level in %s\n",
		__FUNCTION__, string0);
	return -1;
    }
    /* If there are only variables in the list, then keep track of variable match errors */
    findreason = 0;
    if (reason0)
	for (i=0; i<pt.pt_len; i++){ 
	    if ((co = pt.pt_vec[i]) == NULL)
		continue;
	    if (co->co_type != CO_VARIABLE){
		findreason = 0;
		break;
	    }
	    findreason++;
	}
    extract_substring(string0, level, &string);
    for (i=0; i<pt.pt_len; i++){
	if ((co = pt.pt_vec[i]) == NULL)
	    continue;
	reason = NULL;
	if ((match = match_object(string, co, &exact, findreason?&reason:NULL)) < 0)
	    goto error;
	if (match){
	    assert(reason==NULL);
	    /* Special case to catch rest variable and space delimited
	       arguments after it */
	    if (co->co_type == CO_VARIABLE && co->co_vtype == CGV_REST)
		rest_match = i;
	    if (match_perfect(string, co)){
		if (!perfect){
		    matches = 0;
		    perfect = 1;
		}
	    }
	    else{
		if (perfect)
		    break;
 		if (1 || use_pref){
		    p = co_pref(co, exact);
		    if (p < preference)
			continue; /* ignore */
		    if (p > preference){
			preference = p;
			matches = 0; /* Start again at this level */
		    }
		}
	    }
	    co_match = co;
	    matches++;
	}
	/* match == 0, co type is variable and findreason, then reason is set 
	   this may not be the best preference, we just set the first
	*/
	if (reason){
	    if (*reason0 == NULL)
		*reason0 = reason;
	    reason = NULL;
	    findreason = 0;
	}
    } /* for */
    if (matches != 0 && reason0 && *reason0){
	    free(*reason0);
	    *reason0 = NULL;
	}

    if (matches != 1) {
#ifdef notneeded
	if (matches == 0){
	    cligen_nomatch_set(h, "Unrecognized command");
	}
	else
	    cligen_nomatch_set(h, "Ambigious command");
#endif
	retval = 0;
	goto quit;
    }
    assert(co_match);
    if ((cmd_levels = command_levels(string0)) < 0)
	goto error;

    /* co_orig is original object in case of expansion */
    co_orig = co_match->co_ref?co_match->co_ref: co_match;
    if (pt_expand_1(h, co_match, &co_match->co_pt) < 0) /* sub-tree expansion */
	goto error; 

    if (co_match->co_type == CO_VARIABLE){
	if ((cv = add_cov_to_cvec(co_match, string, cvec)) == NULL)
	    goto error;
    }
    else
	if (co_match->co_type == CO_COMMAND && co_orig->co_type == CO_VARIABLE)
	    if ((cv = add_cov_to_cvec(co_orig, string, cvec)) == NULL)
		goto error;
    if (pt_expand_2(h, &co_match->co_pt, cvec, &ptn, hide) < 0) /* expand/choice variables */
	goto error;
    if (level+1 == cmd_levels)
	retval = match_pattern_terminal(h,
					string0, ptn, 
					level+1, use_pref,
					ptp, matchv, matchlen, reason0);
    else
	retval = match_pattern_node(h, 
				    string0, ptn,
				    level+1, use_pref, hide,
				    ptp, matchv, matchlen, cvec, reason0);

    if (pt_expand_add(co_orig, ptn) < 0) /* add expanded ptn to orig parsetree */
	goto error;
    if (co_match->co_type == CO_COMMAND && co_orig->co_type == CO_VARIABLE)
	if (co_value_set(co_orig, co_match->co_command) < 0)
	    goto error;


    /* Cleanup made on top-level */
    
    /* 
     * Special case: we have matched a REST variable (anything) and
     * there is more text have this word, then we can match REST
     */
    if (retval == 0 && rest_match != -1){
	retval = 1;
	if (*matchlen < 1){
	    *matchlen = 1;
	    if ((*matchv = realloc(*matchv, (*matchlen)*sizeof(int))) == NULL){
		fprintf(stderr, "%s: realloc: %s\n", __FUNCTION__, strerror(errno));
		return -1;
	    }
	}
	else
	    *matchlen = 1;
	*ptp = pt.pt_vec;
	(*matchv)[0] = rest_match;
    }
  quit:
    if (cv){ /* cv may be stale */
	cv = cvec_i(cvec, cvec_len(cvec)-1);
	cv_reset(cv);
	cvec_del(cvec, cv);
    }
    /* Only the last level may have multiple matches */
    if (string)
	free(string);
    return retval;
  error:
    retval = -1;
    goto quit;
}

/*
 * match_pattern
 * Complex function with several input and output parameters
 * INPUT:
 *   h         CLIgen handle
 *   string    Input string to match
 *   pt        Vector of commands (array of cligen object pointers (cg_obj)
 *   pt_max    Length of the pt array
 *   use_pref  Set this flag value if you want to use the preferences between
 *             matches. It is only when you want a final exact match (not 
 *             completion or show options) that you should set this.
 *   hide
 * RETURNS:
 *   The number of matches (0-n) in pt or -1 on error. See matchlen below.

 * OUTPUT
 *   ptp       Returns the vector at the place of matching
 *   matchv    A vector of integers containing which 
 *   matchlen  Length of matchv. That is, # of matches and same as return 
 *              value (if 0-n)
 *   cvec      cligen variable vector containing vars/values pair for completion
 *   reason0   If retval = 0, this may be malloced to indicate reason for not
 *             matching variables, if given. Neeed to be free:d
 *
 * All options are ordered by PREFERENCE, where 
 *       command > ipv4,mac > string > rest
 */
int 
match_pattern(cligen_handle h,
	      char *string, parse_tree pt, int use_pref, int hide,
	      pt_vec *ptp, 
	      int *matchv[], int *matchlen, 
	      cvec *cvec,
	      char **reason0)
{
    int retval;
    int levels;

    assert(ptp);
    if ((levels = command_levels(string)) < 0)
	return -1;
    if (levels == 0)
	retval = match_pattern_terminal(h, 
					string, pt, 0, use_pref,
					ptp, matchv, matchlen, 
					reason0);
    else
	retval = match_pattern_node(h, 
				    string, pt, 0, use_pref, hide,
				    ptp, matchv, matchlen, 
				    cvec,
				    reason0);
    return retval;
}

/*
 * match_pattern_exact
 * INPUT:
 *   h          CLIgen handle
 *   string     Input string to match
 *   pt         CLIgen parse tree, vector of cligen objects.
 *   exact      Try to find an exact match. if 0 dont bother about errors.
 * OUTPUT:
 *   cvec      cligen variable vector containing vars/values pair for completion
 *   match_obj  Exact object to return
 * RETURNS:
 *  -1       Error
 *   0       No match
 *   1       Exactly one match
 *   2+      Multiple matches
 * 
 * Only if retval == 0 _AND> exact == 1 then cligen_nomatch() is set, otherwise not.
 */
int 
match_pattern_exact(cligen_handle h, 
		    char         *string, 
		    parse_tree    pt, 
		    int           exact,
		    cvec         *cvec,
		    cg_obj      **match_obj)
{
  pt_vec  res_pt;
  cg_obj *co;
  int     matchlen = 0;
  int     *matchv = NULL;
  int     i;
  int     ret;
  char   *reason = NULL;

  /* clear old errors */
  if (exact)
      cligen_nomatch_set(h, NULL); 
  if ((ret = match_pattern(h, string, pt, 1, 0, &res_pt, 
			   &matchv, &matchlen, cvec, &reason)) < 0)
      return -1;
  if (ret == 0) {
      if (exact){
	  if (reason != NULL){
	      cligen_nomatch_set(h, "%s", reason);
	      free(reason);
	  }
	  else
	      cligen_nomatch_set(h, "Unknown command");
      }
  }
  else{
      if (ret > 1){
	  /* Special case: command and exact-> use that */
	  int j;
	  char *string1;

	  extract_substring(string, command_levels(string), &string1);
	  for (j=0; j<ret; j++){
	      co = res_pt[matchv[j]];
	      if (co->co_type == CO_COMMAND && string1 && 
		  strcmp(string1, co->co_command)==0){
		  ret = 1;
		  matchv[0] = matchv[j];
		  break;
	      }
	  }
      }
  }
  if (ret != 1){
      if (matchv)
	  free(matchv);
      return ret;
  }
  /* Here we have an obj (res_pt[]) that is unique so far.
     We need to see if there is only one sibling to it. */
  co = res_pt[*matchv];
  free(matchv);
  matchv = NULL;
  /*
   * Special case: if matching object has a NULL child,
   * we match.
   */
  if (co->co_max == 0)
      goto done;
  for (i=0; i< co->co_max; i++)
      if (co->co_next[i] == NULL)
	  goto done;

#if 0
  {
  int variable = 0;
  /* The last object should be unique */
  while (co->co_max == 1){
      co = co->co_next[0];
      if (co->co_type == CO_VARIABLE && co->co_vtype != CGV_REST)
	  variable++;
  }
  if (co->co_max)
      return 0;
  if (variable) 
      return 0;
  }
#else
  if (exact)
      cligen_nomatch_set(h, "Incomplete command");
  return 0;
#endif
 done:
  if (match_obj)
      *match_obj = co;
  return 1;
}

/*
 * match_complete
 * Try to complete a string as far as possible using the syntax.
 * 
 * Parameters
 * IN
 *   h       cligen handle
 *   string  Input string to match
 *   pt      Vector of commands (array of cligen object pointers (cg_obj)
 *   pt_max  Length of the pt array
 *   maxlen  Max length of string
 * OUT
 *   cvec      cligen variable vector containing vars/values pair for completion
 * RETURNS:
 *   -1 - Error 
 *   0  - No matches, no completions made
 *   1  - We have completed by adding characters at the end of "string"
 */
int 
match_complete(cligen_handle h, 
	       char         *string, 
	       parse_tree    pt, 
	       int           maxlen, 
	       cvec         *cvec)
{
    int level;
    int slen;
    int equal;
    int i, j, minmatch;
    cg_obj *co, *co1 = NULL;
    char *s;
    char *ss;
    pt_vec pt1;
    int nr;
    int matchlen = 0;
    int *matchv = NULL;
    int mv;
    int append = 0; /* Has appended characters */
    int retval = -1;

    /* ignore any leading whitespace */
    s = string;
    while ((strlen(s) > 0) && isblank(*s))
	    s++;
 again:
    matchlen = 0;
    if ((nr = match_pattern(h, s, pt, 0, 1, &pt1, &matchv, &matchlen, cvec, NULL)) < 0)
	goto done;
    if (nr==0){
	retval = 0;
	goto done; /*  No matches */
    }
    if ((level = command_levels(s)) < 0)
	goto done;
    extract_substring(s, level, &ss);
    slen = ss?strlen(ss):0;
    if (ss)
	free(ss);

    minmatch = slen;
    equal = 1;
    for (i=0; i<matchlen; i++){
	assert((mv = matchv[i])!=-1);
	co = pt1[mv];
	if (co == NULL){
	    retval = 0;
	    goto done;
	}
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
    assert(strlen(string) + minmatch - slen < maxlen);
    strncat(string, &co1->co_command[slen], minmatch-slen);
    append = append || minmatch-slen;
    if (equal){ /* add space */
	string[strlen(string)+1] = '\0';
	string[strlen(string)] = co1?co1->co_delimiter:' ';
	level++;
	slen = 0;
	co1 = NULL;
	if (cligen_completion(h) == 1){
	    if (matchv)
		free(matchv);
	    matchv = NULL;
	    goto again;
	}
    }
    retval = append?1:0;
  done:
    if (matchv)
	free(matchv);
    return retval;
}


