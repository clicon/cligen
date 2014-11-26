/*
  Copyright (C) 2001-2014 Olof Hagsand

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

  CLIgen variable vectors - cvec
*/

#include "cligen_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <string.h>
#include <errno.h>

#include "cligen_buf.h"
#include "cligen_var.h"
#include "cligen_cvec.h"
#include "cligen_gen.h"
#include "cligen_match.h"
#include "cligen_cv.h"
#include "getline.h"

struct cvec{
    cg_var         *vr_vec;  /* vector of CLIgen variables */
    int             vr_len;  /* length of vector */
    char           *vr_name; /* name of cvec, can be NULL */
};

/*
 * cv_exclude_keys
 * set if you want to backward compliant: dont include keys in cgv vec to callback
 * that is, regular 'keys' and keys like: '<string keyword=foo>'
 */
static int excludekeys = 0;

/*! Create and initialize a new cligen variable vector (cvec)
 * 
 * See also cvec_init()
 * Each individual cv initialized with CGV_ERR and no value.
 * Returned cvec needs to be freed with cvec_free().
 *
 * @param  len    Number of cv elements. Can be zero and elements added incrementally.
 * @retval NULL   errno set
 * @retval cv     allocated cligen var
 */
cvec *
cvec_new(int len)
{
    cvec *vr;

    if ((vr = malloc(sizeof(*vr))) == NULL)
	return NULL;
    memset(vr, 0, sizeof(*vr));
    if (cvec_init(vr, len) < 0){
	free(vr);
	return NULL;
    }
    return vr;
}

/*! Free a cligen  variable vector (cvec)
 *
 * Reset and free a cligen vector as previously created by cvec_new(). this includes
 * freeing all cv:s that the cvec consists of.
 * @param  cvec    The cligen variable vector
 */
int
cvec_free(cvec *cvec)
{
    cvec_reset(cvec);
    free(cvec);
    return 0;
}

/*! Initialize a cligen variable vector (cvec) with 'len' numbers of variables.
 *
 * See also cvec_new()
 * Each individual cv initialized with CGV_ERR and no value.
 *
 * @param vr    The cligen variable vector
 * @param len  number of cv elements. Can be zero and elements added incrementally.
 */
int
cvec_init(cvec *vr, int len)
{
    vr->vr_len = len; 
    if (len && (vr->vr_vec = calloc(vr->vr_len, sizeof(cg_var))) == NULL)
	return -1;
    return 0;
}

/*! Reset cligen variable vector resetting it to an initial state as returned by cvec_new
 *
 * See also cvec_free. But this function does not actually free the cvec.
 */
int
cvec_reset(cvec *vr)
{
    cg_var *cv = NULL;
    
    while ((cv = cvec_each(vr, cv)) != NULL)  
	cv_reset(cv);
    if (vr->vr_vec)
	free(vr->vr_vec);
    if (vr->vr_name)
	free(vr->vr_name);
    memset(vr, 0, sizeof(*vr));
    return 0;
}

/*! Given a cv in a cligen variable vector (cvec) return the next cv.
 *
 * Given an element (cv0) in a cligen variable vector (cvec) return the next element.
 * if cv0 is NULL, return first element.
 */
cg_var *
cvec_next(cvec *vr, cg_var *cv0)
{
    cg_var *cv = NULL;
    int i;

    if (cv0 == NULL)
	cv = vr->vr_vec;
    else {
	i = cv0 - vr->vr_vec;
	if (i < vr->vr_len-1)
	    cv = cv0 + 1;
    }
    return cv;
}

/*! Append a new cligen variable (cv) to cligen variable vector (cvec) and return it.
 *
 * See also cv_new, but this is allocated contiguosly as a part of a cvec.
 */
cg_var *
cvec_add(cvec *vr, enum cv_type type)
{
    int     len = vr->vr_len + 1;
    cg_var *cv;

    if ((vr->vr_vec = realloc(vr->vr_vec, len*sizeof(cg_var))) == NULL)
	return NULL;
    vr->vr_len = len;
    cv = cvec_i(vr, len-1);
    memset(cv, 0, sizeof(*cv));
    cv->var_type = type;
    return cv;
}


/*! Delete a cv variable from a cvec. Note: cv is not reset & cv may be stale!
 *
 * This is a dangerous command since the cv it deletes (such as created by cvec_add)
 * may have been modified with realloc (eg cvec_add/delete) and therefore can not be 
 * used as a reference.  Safer methods are to use cvec_find/cvec_i to find a cv and 
 * then to immediately remove it. 
 */
int
cvec_del(cvec *vr, cg_var *del)
{
    int i;
    cg_var *cv;

    if (cvec_len(vr) == 0)
	return 0;
    
    i = 0;
    cv = NULL;
    while ((cv = cvec_each(vr, cv)) != NULL) {
	if (cv == del)
	    break;
	i++;
    }
    if (i >= cvec_len(vr)) /* Not found !?! */
	return cvec_len(vr);

    if (i != cvec_len(vr)-1) /* If not last entry, move the remaining cv's */
	memmove(&vr->vr_vec[i], &vr->vr_vec[i+1],
		(vr->vr_len-i-1) * sizeof(vr->vr_vec[0]));
    
    vr->vr_len--;
    vr->vr_vec = realloc(vr->vr_vec, vr->vr_len*sizeof(vr->vr_vec[0])); /* Shrink should not fail? */

    return cvec_len(vr);
}

/*! Return allocated length of a cvec.
 */
int     
cvec_len(cvec *cvv)
{
    return cvv->vr_len;
}

/*! Return i:th element of cligen variable vector cvec.
 * @param  cvv	  	Cligen variable list
 * @param  i	  	Order of element to get
 */
cg_var *
cvec_i(cvec *cvv, int i)
{
    if (i < cvv->vr_len)
	return &cvv->vr_vec[i];
    return NULL;
}

/*! Iterate through all cligen variables in a cvec list
 *
 * @param  cvv	  	Cligen variable list
 * @param  prev		Last cgv (or NULL)
 * @retval cv           Next variable structure.
 * @retval NULL         When end of list reached.
 * @code
 *	   cg_var *cv = NULL;
 *	   while ((cv = cvec_each(cvv, cv)) != NULL) {
 *	     ...
 *	   }
 * @endcode

 */
cg_var *
cvec_each(cvec *cvv, cg_var *prev)
{
  if (prev == NULL){   /* Initialization */
      if (cvv->vr_len > 0)
	  return &cvv->vr_vec[0];
      else
	  return NULL;
  }
  return cvec_next(cvv, prev);
}

/*! Like cvec_each but skip element 0. 
 *
 * Common in many cvecs where [0] is the command-line and all
 * others are arguments.
 */
cg_var *
cvec_each1(cvec *cvv, cg_var *prev)
{
  if (prev == NULL){   /* Initialization */
      if (cvv->vr_len > 1)
	  return &cvv->vr_vec[1];
      else
	  return NULL;
  }
  return cvec_next(cvv, prev);
}

/*! Create a new cvec by copying from an original
 *
 * The new cvec needs to be freed by cvec_free().
 * One can make a cvec_cp() as well but it is a little trickier to match vr_vec.
 */
cvec *
cvec_dup(cvec *old)
{
    cvec   *new;
    cg_var *cv0 = NULL;
    cg_var *cv1;
    int     i;

    if ((new = cvec_new(old->vr_len)) == NULL)
	return NULL;
    i = 0;
    while ((cv0 = cvec_each(old, cv0)) != NULL) {
	cv1 = cvec_i(new, i++);
	cv_cp(cv1, cv0);
    }
    return new;
}

/*! Create cv list by matching a CLIgen parse-tree and an input string. 
 *
 * The matched parse-tree is  given by a syntax-node in a leaf, and by following 
 * the parse-tree 'upwards' in the tree, a syntactic string can be found. 
 * This function creates CLIgen cvec structures for every keyword and variable
 * syntax node it encounters, and thus creates one vector of keys and
 * one vector of CLIgen variables.
 * Typically called internally from cliread_parse().
 *
 * The variable vr should be freed with cvec_reset()!
 *
 * @param  [in]     co_match    Leaf CLIgen syntax node
 * @param  [in]     cmd         Command string 
 * @param  [in,out] vr          Initialized cvec (cvec_new or cvec_reset). CLIgen 
 *                              variable record         
 * @retval          0           OK
 * @retval          -1          Error
 */
int
cvec_match(cg_obj *co_match, 
	   char   *cmd, 
	   cvec   *cvv)
{
    cg_obj    *co;
    cg_var    *cv;
    int        nrargs;
    int        nrlevels;
    int        v;  /* variable index */
    int        j;
    char      *val;
    int        level;
    int        retval = -1;

    nrlevels   = -1;
    nrargs   = 0;
    memset(cvv, 0, sizeof(*cvv));

    /* Count nr of keys and variables by going upwards from leaf to top */
    for (co=co_match; co; co=co_up(co)){
#ifdef notused
	if (co->co_top)
	    break;
#endif
	nrlevels++;
	switch (co->co_type){
	case CO_VARIABLE:
	    nrargs++;
	    break;
	case CO_COMMAND:
	    if (!excludekeys)
		nrargs++;
	    break;
	case CO_REFERENCE: /* shouldnt happen */
	    fprintf(stderr, "%s: type should not be REFERENCE\n", __FUNCTION__);
	    goto done;
	    break;
	}
    }

    /* cvec for each arg + command itself */
    if (cvec_init(cvv, nrargs + 1) < 0){
	fprintf(stderr, "%s: calloc: %s\n", __FUNCTION__, strerror(errno));
	goto done;
    }
    /* Historically we put command line here, maybe more logical in keyword? */
    cv = cvec_i(cvv, 0);
    cv->var_type = CGV_REST;
    cv_string_set(cv, cmd); /* the whole command string */

    v = 1; /* first ele)ment is whole string */
    for (level=0; level<=nrlevels; level++){
	/* Go up with co to the level XXX co can be in the loop? */
	for (j=nrlevels, co=co_match; j>level; j--)
	    co = co_up(co);
	/* Check if this is a variable */
	cv = cvec_i(cvv, v);
	switch (co->co_type){
	case CO_VARIABLE: /* Get the value of the variable */
	    if (co->co_value){
		val = co->co_value;
		co->co_value = NULL;
	    }
	    else
		if (co->co_vtype == CGV_REST)
		    extract_substring_rest(cmd, level, &val);
		else
		    extract_substring(cmd, level, &val);
	    cv->var_type = co->co_vtype;
	    cv->var_name = strdup(co->co_command);
	    if (co->co_show)
		cv->var_show = strdup(co->co_show);
	    cv->var_const = iskeyword(co);
	    if (co->co_vtype == CGV_DEC64) /* XXX: Seems misplaced? / too specific */
		cv_dec64_n_set(cv, co->co_dec64_n);
	    /* String value to structured type */
	    if (cv_parse(val, cv) < 0) {
		/* This should never happen, since it passes in match_variable() */
		free (val);
		goto done;
	    }
	    free (val);
	    if (co->co_vtype == CGV_REST)
		break; /* XXX should break for() */
	    v++;
	    break;
	case CO_COMMAND:
	    if (!excludekeys){
		cv->var_name = strdup(co->co_command);
		cv->var_type = CGV_STRING;
		cv_string_set(cv, co->co_command);
		cv->var_const = 1;
		v++;
	    }
	    break;
	default:
	    break;
	} /* switch */
    }
    retval = 0;
  done:
    return retval;
} /* cvec_match */


/*! Create a cv list with a single string element.
 *
 * Help function when creating cvec to cligen callbacks.
 */
cvec *
cvec_start(char *cmd)
{
    cvec *cvec;
    cg_var    *cv;

    if ((cvec = cvec_new(1)) == NULL){
	fprintf(stderr, "%s: cvec_new: %s\n", __FUNCTION__, strerror(errno));
	return NULL;
    }
    cv = cvec_i(cvec, 0);
    cv->var_type = CGV_REST;
    cv_name_set(cv, "cmd"); /* the whole command string */
    cv_string_set(cv, cmd); /* the whole command string */
    return cvec;
}

/*! Pretty print cligen variable list to a file
 */
int
cvec_print(FILE *f, cvec *cvv)
{
    cg_var *cv = NULL;
    int     i = 0;

    while ((cv = cvec_each(cvv, cv)) != NULL) {
	fprintf(f, "%d : %s = ", i++, cv_name_get(cv));
	cv_print(f, cv);
	fprintf(f, "\n");
    }
    return 0;
}

/*! Return first cv in a cvec matching a name
 *
 * Given an CLIgen variable vector cvec, and the name of a variable, return the first 
 * matching entry. 
 */
cg_var *
cvec_find(cvec *cvv, char *name)
{
    cg_var *cv = NULL;

    while ((cv = cvec_each(cvv, cv)) != NULL) 
	if (cv->var_name && strcmp(cv->var_name, name) == 0)
	    return cv;
    return NULL;
}

/*! Like cvec_find, but only search non-keywords
 */
cg_var *
cvec_find_var(cvec *cvv, char *name)
{
    cg_var *cv = NULL;

    while ((cv = cvec_each(cvv, cv)) != NULL) 
	if (cv->var_name && strcmp(cv->var_name, name) == 0 && !cv->var_const)
	    return cv;
    return NULL;
}

#ifdef notyet
/*
 * cvec_var_i
 * Like cvec_i but only works with variables.
 */
cg_var *
cvec_var_i(cvec *cvv, char *name)
{
    cg_var *cv = NULL;

    while ((cv = cvec_each(cvv, cv)) != NULL) 
	if (cv->var_name && strcmp(cv->var_name, name) == 0 && !cv->var_const)
	    return cv;
    return NULL;
}
#endif


/*! Like cvec_find, but only search keywords
 */
cg_var *
cvec_find_keyword(cvec *cvv, char *name)
{
    cg_var *cv = NULL;

    while ((cv = cvec_each(cvv, cv)) != NULL) 
	if (cv->var_name && strcmp(cv->var_name, name) == 0 && cv->var_const)
	    return cv;
    return NULL;
}

/*! Typed version of cvec_find that returns the string value.
 *
 * Note: (1) Does not see the difference between not finding the cv, and finding one
 *           with wrong type - in both cases NULL is returned.
 *       (2) the returned string must be copied since it points directly into the cv.
 */
char *
cvec_find_str(cvec *cvv, char *name)
{
    cg_var *cv;

    if ((cv = cvec_find(cvv, name)) != NULL && cv_isstring(cv->var_type))
	return cv_string_get(cv);
    return NULL;
}

/*! Get name of cligen variable vector
 * @param  cvv   A cligen variable vector
 * @retval str  The name of the cvec as a string, can be NULL, no copy
 */
char *
cvec_name_get(cvec *cvv)
{
    return cvv->vr_name;
}

/*! Allocate and set name of cligen variable vector, including NULL
 * @param  cvv     A cligen variable vector
 * @param  name   A string that is copied and used as a cvec name, or NULL
 * @retval str    The name of the cvec.
 * The existing name, if any, is freed
 */
char *
cvec_name_set(cvec *cvv, char *name)
{
    char *s1 = NULL;

    /* Duplicate name. Must be done before a free, in case name is part of the original */
    if (name){
	if ((s1 = strdup(name)) == NULL)
	    return NULL; /* error in errno */
    }
    if (cvv->vr_name != NULL)
	free(cvv->vr_name);
    cvv->vr_name = s1;
    return s1; 
}


/*! Changes cvec find function behaviour, exclude keywords or include them.
 */
int 
cv_exclude_keys(int status)
{
    excludekeys = status;
    return 0;
}

