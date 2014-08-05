/*
  CVS Version: $Id: cligen_gen.c,v 1.55 2013/04/23 13:04:53 olof Exp $ 

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
#include <stdint.h>
#include <stdarg.h>
#include <inttypes.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <ctype.h>
#define __USE_GNU /* strverscmp */
#include <string.h>
#include <errno.h>

#include "cligen_var.h"
#include "cligen_cvec.h"
#include "cligen_gen.h"
#include "cligen_read.h"
#include "cligen_parse.h"
#include "cligen_handle.h"
#include "getline.h"

/* Internal functions */
static int pt_free1(parse_tree, int recursive);

/*
 * preference. 
 * Prefer more specific commands/variables 
 * if you have to choose from several. 
 * The preference is (higher more preferred):
 *
 * command / keyword
 * (ip|mac) 
 * decimal64
 * int8 with range , uint8 with range 
 * ...
 * int64 with range , uint64 with range 
 * int8 , uint8
 * ...
 * int64 , uint64
 * interface
 * regexp
 * string 
 * rest
 * NOTE: havent thought so much about preferences with ints: be careful if you offer
 * a choice: <int32>|<uint16>. Which is most preferred if both match?
 */
static int
cov_pref(cg_obj *co)
{
    int pref = 0;

    switch (co->co_vtype){
    case CGV_ERR:
	pref = 0; /* Illegal */
	break;
	/* ints in range 22-60 */
    case CGV_INT8:
	if (co->co_range)
	    pref = 60;
	else
	    pref = 52;
	break;
    case CGV_INT16:
	if (co->co_range)
	    pref = 58;
	else
	    pref = 50;
	break;
    case CGV_INT32:
	if (co->co_range)
	    pref = 56;
	else
	    pref = 48;
	break;
    case CGV_INT64:
	if (co->co_range)
	    pref = 54;
	else
	    pref = 46;
	break;
    case CGV_UINT8:
	if (co->co_range)
	    pref = 59;
	else
	    pref = 51;
	break;
    case CGV_UINT16:
	if (co->co_range)
	    pref = 57;
	else
	    pref = 49;
	break;
    case CGV_UINT32:
	if (co->co_range)
	    pref = 55;
	else
	    pref = 47;
	break;
    case CGV_UINT64:
	if (co->co_range)
	    pref = 53;
	else
	    pref = 45;
	break;
    case CGV_DEC64:
	pref = 62;
	break;
    case CGV_BOOL:
	pref = 12;
	break;
    case CGV_REST:
	pref = 1;
	break;
    case CGV_STRING:
	if (co->co_regex)
	    pref = 7;
	else
	    pref = 5;
	break;
    case CGV_INTERFACE:
	pref = 10;
	break;
    case CGV_IPV4ADDR:
    case CGV_IPV4PFX:
	pref = 70;
	break;
    case CGV_IPV6ADDR:
    case CGV_IPV6PFX:
	pref = 71;
	break;
    case CGV_MACADDR:
	pref = 72;
	break;
    case CGV_URL:
	pref = 20;
	break;
    case CGV_UUID:
	pref = 73;
	break;
    case CGV_TIME:
	pref = 74;
	break;
    case CGV_VOID: /* N/A */
	break;
    }
    return pref;
}

/*
 * co_pref
 * Prefer more specific commands/variables 
 * INPUT:
 *   co    cligen_object
 *   exact if match was exact (only applies to CO_COMMAND)
 *
 * The higher the better
 * if you have to choose from several.
 * The preference is:
 * command > ip|mac > int > interface > string > expand > rest
 * 'expand' is a command with not exact match that is derived from a <expand> or <choice>
 */
int
co_pref(cg_obj *co, int exact)
{
    int pref = 0;;

    switch (co->co_type){
    case CO_COMMAND:
	if (co->co_ref && !exact)
	    pref = 3; /* expand */
	else
	    pref = 100;
	break;
    case CO_VARIABLE:
	pref = cov_pref(co);
	break;
    case CO_REFERENCE: /* ? */
	
	break;
    }
    return pref;
}


/*! Create new cligen parse-tree command object
 *
 * That is, a cligen parse-tree object with type == CO_COMMAND
 */
cg_obj *
co_new(char *cmd, cg_obj *prev)
{
    cg_obj *co;

    if ((co = malloc(sizeof(cg_obj))) == NULL){
	perror("co_new: malloc");
	return NULL;
    }
    memset(co, 0, sizeof(cg_obj));
    co->co_type    = CO_COMMAND;
    co->co_command = strdup(cmd);
    co_up_set(co, prev);
    co->co_max = 0;                  /* pt len */
    co->co_next = NULL;
    co->co_delimiter = ' ';
#ifdef notyet
    co->co_cv = cv_new(CGV_ERR);
#endif
    return co;
}

/*! Create new cligen parse-tree variable object
 *
 * That is, a cligen parse-tree object with type == CO_VARIABLE
 * See also co_new
 */
cg_obj *
cov_new(enum cv_type cvtype, cg_obj *prev)
{
    cg_obj *co;

    if ((co = malloc(sizeof(cg_obj))) == NULL){
	perror("co_new: malloc");
	return NULL;
    }
    memset(co, 0, sizeof(cg_obj));
    co->co_type    = CO_VARIABLE;
    co->co_vtype   = cvtype;
    if (prev)
	co_up_set(co, prev);
    co->co_max = 0;                  /* pt len */
    co->co_next = NULL;
    co->co_delimiter = ' ';
    co->co_dec64_n = CGV_DEC64_N_DEFAULT;
#ifdef notyet
    co->co_cv = cv_new(CGV_ERR);
#endif
    return co;
}


/*! Enlarge the child-vector (pattern) of a parse-tree
 *
 * Suppose we have a pattern pt, with lists of cg_obj's 1..4:
 * pt -> .-.-.-.
 *       | | | |
 *       v v v v
 *       1 2 3 4
 * and a new cg_obj 5,
 * then reallocate pt for another list of cg_objs and copy it into the structure:
 * pt -> .-.-.-.-.
 *       | | | | |
 *       v v v v v
 *       1 2 3 4 5
 */
int 
pt_realloc(parse_tree *pt)
{
    pt->pt_len++;
    /* Allocate larger cg_obj vector */
    if ((pt->pt_vec = realloc(pt->pt_vec, (pt->pt_len)*sizeof(cg_obj *))) == 0){
	fprintf(stderr, "%s: realloc", __FUNCTION__);
	return -1;
    }
    pt->pt_vec[pt->pt_len - 1] = NULL; /* init field */
    return 0;
}

/*! Copy a linked list of cg_obj callback objects
 *
 * Copy a linked list of cg_obj callback objects, including function pointer, 
 * function name,
 *
 * @param cc0    - The object to copy from
 * @param ccn    - Pointer to the object to copy to (is allocated)
 * @param cgv    - if given, is a string that overrides the arg in cc.
 */
int
co_callback_copy(struct cg_callback *cc0, struct cg_callback **ccn, cg_var *cgv)
{
    struct cg_callback *cc, *cc1, **ccp;

    ccp = ccn;
    for (cc = cc0; cc; cc=cc->cc_next){
	if ((cc1 = malloc(sizeof(*cc1))) == NULL){
	    fprintf(stderr, "%s: malloc: %s\n", __FUNCTION__, strerror(errno));
	    return -1;
	}
	memset(cc1, 0, sizeof(*cc1));
	cc1->cc_fn = cc->cc_fn;
	if (cc->cc_fn_str)
	    if ((cc1->cc_fn_str = strdup(cc->cc_fn_str)) == NULL){
		fprintf(stderr, "%s: strdup: %s\n", __FUNCTION__, strerror(errno));
		return -1;
	    }
	if (cgv){
	    if ((cc1->cc_arg = cv_dup(cgv)) == NULL)
		return -1;
	}
	else
	    if (cc->cc_arg && ((cc1->cc_arg = cv_dup(cc->cc_arg)) == NULL))
		return -1;
	*ccp = cc1;
	ccp = &cc1->cc_next;
    }
    return 0;
}

/*! Recursively copy a cligen object.
 *
 * See also co_expand_sub
 *
 * @param  co     - The object to copy from
 * @param parent - The parent of the new object (need not be same as parent of co)
 * @param conp   - Pointer to the object to copy to (is allocated)
 */
int
co_copy(cg_obj *co, cg_obj *parent, cg_obj **conp)
{
    cg_obj *con;

    if ((con = malloc(sizeof(cg_obj))) == NULL){
	fprintf(stderr, "%s: malloc: %s\n", __FUNCTION__, strerror(errno));
	return -1;
    }
    memcpy(con, co, sizeof(cg_obj));
    memset(&con->co_pt_push, 0, sizeof(struct parse_tree));

    con->co_ref  = NULL;
    con->co_mark = 0;
    con->co_refdone = 0;
    /* Replace all pointers */
    co_up_set(con, parent);
    if (co->co_command)
	if ((con->co_command = strdup(co->co_command)) == NULL){
	    fprintf(stderr, "%s: strdup: %s\n", __FUNCTION__, strerror(errno));
	    return -1;
	}
    if (co_callback_copy(co->co_callbacks, &con->co_callbacks, NULL) < 0)
	return -1;
    if (co->co_cvec)
	con->co_cvec = cvec_dup(co->co_cvec);
    if (co->co_userdata && co->co_userlen){
	if ((con->co_userdata = malloc(co->co_userlen)) == NULL){
	    fprintf(stderr, "%s: malloc: %s\n", __FUNCTION__, strerror(errno));
	    return -1;
	}
	memcpy(con->co_userdata, co->co_userdata, co->co_userlen);
    }

    if (co->co_next)
	if (pt_copy(co->co_pt, con, &con->co_pt) < 0)
	    return -1;
    if (co->co_help)
	if ((con->co_help = strdup(co->co_help)) == NULL){
	    fprintf(stderr, "%s: strdup: %s\n", __FUNCTION__, strerror(errno));
	    return -1;
	}
    if (co->co_mode){
	if ((con->co_mode = strdup(co->co_mode)) == NULL){
	    fprintf(stderr, "%s: strdup: %s\n", __FUNCTION__, strerror(errno));
	    return -1;
	}
    }
    if (co_value_set(con, co->co_value) < 0) /* XXX: free på co->co_value? */
	return -1;

    if (co->co_type == CO_VARIABLE){
	if (co->co_expand_fn_str)
	    if ((con->co_expand_fn_str = strdup(co->co_expand_fn_str)) == NULL){
		fprintf(stderr, "%s: strdup: %s\n", __FUNCTION__, strerror(errno));
		return -1;
	    }
	if (co->co_expand_fn_arg)
	    if ((con->co_expand_fn_arg = cv_dup(co->co_expand_fn_arg)) == NULL)
		return -1;

	if (co->co_choice){
	    if ((con->co_choice = strdup(co->co_choice)) == NULL){
		fprintf(stderr, "%s: strdup: %s\n", __FUNCTION__, strerror(errno));
		return -1;
	    }
	}
	if (co->co_regex){
	    if ((con->co_regex = strdup(co->co_regex)) == NULL){
		fprintf(stderr, "%s: strdup: %s\n", __FUNCTION__, strerror(errno));
		return -1;
	    }
	}
    } /* VARIABLE */
    *conp = con;
    return 0;
}

/*! Recursively copy a parse-tree.
 *
 * No common pointers between the two structures
 *
 * @param pt       Original parse-tree
 * @param parent   The parent of the new parsetree (need not be same as parent of pt)
 * @param ptnp     New parse-tree
 */
int
pt_copy(parse_tree pt, cg_obj *parent, parse_tree *ptnp)
{
    int i, j;
    parse_tree ptn = {0,};
    cg_obj *co;

    if (pt.pt_vec == NULL){
	*ptnp = ptn;
	return 0;
    }
    /* subtract treereferences, which are instances of other trees */
    for (i=0; i<pt.pt_len; i++){
	if ((co = pt.pt_vec[i]) && co->co_treeref)
	    ;
	else
	    ptn.pt_len++;
    }

    if ((ptn.pt_vec = (cg_obj **)malloc(ptn.pt_len*sizeof(cg_obj *))) == NULL){
	fprintf(stderr, "%s: malloc: %s\n", __FUNCTION__, strerror(errno));
	return -1;
    }
    j=0;
    for (i=0; i<pt.pt_len; i++){
	if ((co = pt.pt_vec[i]) != NULL){
	    if (!co->co_treeref)
		if (co_copy(co, parent, &ptn.pt_vec[j++]) < 0)
		    return -1;
	}
	else
	    ptn.pt_vec[j++] = NULL;
    }
    for (i=0; i<ptn.pt_len; i++){
	if ((co = ptn.pt_vec[i]) != NULL){
	    assert(co->co_treeref == 0);
	    assert(co->co_mark == 0);
	}
    }
    *ptnp = ptn;
    return 0;
}

/*
 * str_cmp
 * Compare according to 'order', which is basically strcmp, but there 
 * are some complexities which one may enable.
 * Also handles NULL (NULL < all strings)
 * Complexity: 
 * strcmp orders:  1 10 2
 * wheras strverscmp orders: 1 2 10
 * but:
 * strcmp orders:  1b 16 6b  
 * wheras strverscmp orders: 1b 6b 16
 * If we use strverscmp we also must use it in e.g. complete
 */
static inline int
str_cmp(char *s1, char *s2)
{
    if (s1 == NULL && s2 == NULL) 
	return 0;
    if (s1 == NULL) /* empty string first */
	return -1;
    if (s2 == NULL)
	return 1;
    /*
     * XXX: the cligen handler code uses NULL here which is wrong, but those
     * options are for now global settings.
     */
#ifdef  HAVE_STRVERSCMP
    if (cligen_lexicalorder(NULL))
	return strverscmp(s1, s2); /* can't combine lexicalorder and ignorecase */
    else
	return cligen_ignorecase(NULL) ? strcasecmp(s1, s2) : strcmp(s1, s2);
#else /* HAVE_STRVERSCMP */
    return cligen_ignorecase(NULL) ? strcasecmp(s1, s2) : strcmp(s1, s2);
#endif /* HAVE_STRVERSCMP */
}


/*! Examine equality between two cligen objects (cg_obj)
 *
 * Two cligen objects are equal if they have:
 * - same type (variable or command)
 * - if command, they should have same command name
 * - if they are a variable, they should also have:
 *          + same variable type.
 *          + same expand, choice, range and regexp options. 
 * cf str_cmp
 *
 * @retval  0    If equal
 * @retval <0    if co1 is less than co2
 * @retval >0    if co1 is greater than co2
 */
static int 
co_eq(cg_obj *co1, cg_obj *co2)
{
    int eq;

    /* eq == 0 means equal */
    eq = !(co1->co_type == co2->co_type);
    if (eq){ /* special case, keywords */
	if (co1->co_type == CO_COMMAND && 
	    co2->co_vtype == CGV_STRING && 
	    iskeyword(co2)){
	    if ((eq = strcmp(co1->co_command, co2->co_keyword)) == 0)
		goto done;
	}
	else
	if (co2->co_type == CO_COMMAND && 
	    co1->co_vtype == CGV_STRING && 
	    iskeyword(co1))
	    eq = strcmp(co2->co_command, co1->co_keyword);
	else
	    eq = str_cmp(co1->co_command, co2->co_command);
	goto done;
    }
    switch (co1->co_type){
    case CO_COMMAND:
	eq = str_cmp(co1->co_command, co2->co_command);
	break;
    case CO_REFERENCE:
	eq = str_cmp(co1->co_command, co2->co_command);
	break;
    case CO_VARIABLE:
	eq = str_cmp(co1->co_command, co2->co_command); /* XXX: 2012-10-17: Really a bug if I didnt detect this until now,... */
	/* Same variable type */
	if (eq != 0)
	    goto done;
	eq = (co1->co_vtype == co2->co_vtype)?0:(co1->co_vtype < co2->co_vtype)?-1:1;
	/* Same variable type */
	if (eq != 0)
	    goto done;
	/* Examine expand: at least one set, and then strcmp */
	if (co1->co_expand_fn_str!=NULL || co2->co_expand_fn_str!=NULL){
	    eq = str_cmp(co1->co_expand_fn_str, co2->co_expand_fn_str);
	    goto done;
	}

	/* Examine choice: at least one set, and then strcmp */
	if (co1->co_choice!=NULL || co2->co_choice!=NULL){
	    eq = str_cmp(co1->co_choice, co2->co_choice);
	    goto done;
	}
	/* Examine regexp, at least one set, and then strcmp */
	if (co1->co_regex!=NULL || co2->co_regex!=NULL){
	    eq = str_cmp(co1->co_regex, co2->co_regex);
	    goto done;
	}
	/* Examine int and range */
	if (cv_isint(co1->co_vtype)) {
	    if (co1->co_range == co2->co_range){
		eq = 0;
		goto done;
	    }
	    else
		if (co1->co_range){ /* both ranges set */
		    if ((co1->co_range_low == co2->co_range_low) &&
			(co1->co_range_high == co2->co_range_high)){
			    eq = 0;
			    goto done;
			}
		    if ((eq = co2->co_range_low - co1->co_range_low) != 0)
			goto done;
		    eq = co2->co_range_high - co1->co_range_high;
		    goto done;
		}
	}
	break;
    }
  done:
    return eq;
}


/*
 * cligen_parsetree_merge
 * recursively merge two parse-trees: pt1 into pt0
 */
int
cligen_parsetree_merge(parse_tree *pt0, cg_obj *parent0, parse_tree pt1)
{
    cg_obj *co0=NULL, *co1, *co1c;
    int i, j;
    int retval = -1;
    int exist;

    for (j=0; j<pt1.pt_len; j++){ 
	co1 = pt1.pt_vec[j];
	exist = 0;
	for (i=0; i<pt0->pt_len; i++){
	    co0 = pt0->pt_vec[i];
	    if (co0 == NULL && co1 == NULL){
		exist = 1;
		break;
	    }
	    if (co0 && co1 && co_eq(co0, co1)==0){
		exist = 1;
		break;
	    }
	}
	if (co1==NULL){ /* empty */
	    if (exist)
		continue;
	    if (pt_realloc(pt0) < 0)
		goto done;
	    pt0->pt_vec[pt0->pt_len-1]= NULL;
	    continue;
	}
	if (exist){
	    if (cligen_parsetree_merge(&co0->co_pt, co0, co1->co_pt) < 0)
		goto done;
	}
	else{
	    if (pt_realloc(pt0) < 0)
		goto done;
	    if (co_copy(co1, parent0, &co1c) < 0)
		goto done;
	    pt0->pt_vec[pt0->pt_len-1]= co1c;
	}
    }
    cligen_parsetree_sort(*pt0, 0);
    retval = 0;
  done:
    return retval;
}


/*
 * Help function to qsort for sorting entries in pattern file.
 * return 1 if arg1 is > arg2, -1 if arg2 > arg1, 0 if equal.
 */
static int
co_cmp(const void* arg1, const void* arg2)
{
    cg_obj* co1 = *(cg_obj**)arg1;
    cg_obj* co2 = *(cg_obj**)arg2;

    return str_cmp(co1 ? co1->co_command : NULL, co2 ? co2->co_command : NULL);
}

void 
cligen_parsetree_sort(parse_tree pt, int recursive)
{
    cg_obj *co;
    int i;
    
    qsort(pt.pt_vec, pt.pt_len, sizeof(cg_obj*), co_cmp);
    for (i=0; i<pt.pt_len; i++){
	co = pt.pt_vec[i];
	if (co){ /* XXX: Is this necessary? Can the sub-call re-sort this pt? */
	    if (co->co_mark == 0){ /* not recursive */
		co->co_mark = 1;
		if (co->co_next && recursive)
		    cligen_parsetree_sort(co->co_pt, 1);
		co->co_mark = 0;
	    }
	}
    }
}

/*
 * Free an individual syntax node (cg_obj).
 * Note that the co_var pointer is not freed. The application
 * needs to handle it (dont use a pointer to the stack for example).
 * Note: of you add a free here, you should probably add somthing in
 * co_copy and co_expand_sub
 */
int 
co_free(cg_obj *co, int recursive)
{
    struct cg_callback *cc;

    if (recursive && co->co_next)
	pt_free1(co->co_pt, 1); /* recursive */
    if (co->co_help)
	free(co->co_help);
    if (co->co_command)
	free(co->co_command);
    if (co->co_mode)
	free(co->co_mode);
    if (co->co_value)
	free(co->co_value);
    if (co->co_userdata)
	free(co->co_userdata);
    if (co->co_cvec)
	cvec_free(co->co_cvec);
    while ((cc = co->co_callbacks) != NULL){
	if (cc->cc_arg)	
	    cv_free(cc->cc_arg);
	if (cc->cc_fn_str)     
	    free(cc->cc_fn_str);
	co->co_callbacks = cc->cc_next;
	free(cc);
    }
    if (co->co_type == CO_VARIABLE){
	if (co->co_expand_fn_str)
	    free(co->co_expand_fn_str);
	if (co->co_expand_fn_arg){
	    cv_free(co->co_expand_fn_arg);
	}
	if (co->co_choice)
	    free(co->co_choice);
	if (co->co_regex)
	    free(co->co_regex);
    }
#ifdef notyet
    if (co->co_cv)
	cv_free(co->co_cv);
#endif
    free(co);
    return 0;
}

/*
 * pt_free1
 * free all parse-tree nodes of the parse-tree, 
 * do it recursively if recurse is set.
 */
static int
pt_free1(parse_tree pt, int recursive)
{
    int i;

    if (pt.pt_vec == NULL)
	return 0;
    for (i=0; i<pt.pt_len; i++)
	if (pt.pt_vec[i])
	    co_free(pt.pt_vec[i], recursive);
    free(pt.pt_vec);
    pt.pt_vec = NULL;
    pt.pt_len = 0;
    return 0;
}

int
cligen_parsetree_free(parse_tree pt, int recursive)
{
    return pt_free1(pt, recursive);
}

static cg_obj *
co_search1(parse_tree pt, char *name, int low, int high)
{
    int     mid;
    int     cmp;
    cg_obj *co;

    if (high < low)
	return NULL; /* not found */
    mid = (low + high) / 2;
    if (mid >= pt.pt_len)  /* beyond range */
	return NULL;
    co = pt.pt_vec[mid];
    cmp = str_cmp(name, co ? co->co_command : NULL);
    if (cmp < 0)
	return co_search1(pt, name, low, mid-1);
    else if (cmp > 0)
	return co_search1(pt, name, mid+1, high);
    else
	return co;
}

/* 
 * co_insert_pos
 * at what position to insert <name>
 */
static int
co_insert_pos(parse_tree pt, cg_obj *co1, int low, int high)
{
    int     mid;
    int     cmp;
    cg_obj *co2;

    if (high < low)
	return low; /* not found */
    mid = (low + high) / 2;
    if (mid >= pt.pt_len)
	return pt.pt_len; 
    if (co1 == NULL)
	return 0; /* Insert in 1st pos */
    co2 = pt.pt_vec[mid];
    /* XXX: MATCHING PROBLEM: 
       should be same as in co_insert
     */
#if 0
    cmp = str_cmp(co1->co_command, co2 ? co2->co_command : NULL);
#else
    if (co2 == NULL)
	cmp = 1;
    else{
#if 1
	cmp = co_eq(co1, co2);
#else
        cmp = str_cmp(co1->co_command, co2->co_command);
#endif
    }
#endif
    if (cmp < 0)
	return co_insert_pos(pt, co1, low, mid-1);
    else if (cmp > 0)
	return co_insert_pos(pt, co1, mid+1, high);
    else
	return mid;
}

/*
 * co_insert
 * Add a cligen object (co1) to a parsetree(pt) alphabetically.
 * This involves searching in the parsetree for the position where it should be added,
 * Then checking whether an equivalent version already exists.
 * Then modifying the parsetree by shifting it down, and adding the new object.
 * There is som complexity if co == NULL.

 * Return value:
 *   co object if found (old _or_ new). NOTE: you must replace calling cg_obj with return.
 *   NULL error
 */
cg_obj*
co_insert(parse_tree *pt, cg_obj *co1)
{
    int pos;
    size_t size;
    cg_obj *co2;

    /* find closest to co in parsetree, last one that is < co */
    pos = co_insert_pos(*pt, co1, 0, pt->pt_len);
    /* check if exists */
    if (pos < pt->pt_len){
	co2 = pt->pt_vec[pos];
	if (co1 == NULL && co2==NULL)
	    return NULL;
	/* XXX: MATCHING PROBLEM:
	   Note, there is a subtle difference between co_eq and
	   equality in str_cmp, so maybe we should use str_cmp instead? 
	   At least, equality should be the same as in co_insert_pos()
	*/
#if 1
	if (co1 && co2 && co_eq(co1, co2)==0)
#else
	if (co1 && co2 && !str_cmp(co1->co_command, co2->co_command))
#endif
{
	    co_free(co1, 1);
	    return co2;
	}
    }
    if (pt_realloc(pt) < 0)
	return NULL;
    if ((size = (pt->pt_len - (pos+1))*sizeof(cg_obj*)) != 0)
	memmove(&pt->pt_vec[pos+1], 
		&pt->pt_vec[pos], 
		size);
    pt->pt_vec[pos] = co1;
    return co1;
}

/*
 * co_find_one
 * Given a parse tree, find the first command that matches NO RECURSION!
 * Note also that you can only use this if the child-list is alphatetically
 * sorted. You get this automatically with co_insert(), bit some code
 * may add children w/o co_insert.
 */
cg_obj *
co_find_one(parse_tree pt, char *name)
{
  return co_search1(pt, name, 0, pt.pt_len);
}

/*
 * co_value_set
 * malloc new string, remove old if set.
 */
int
co_value_set(cg_obj *co, char *str)
{
    if (co->co_value){ /* This can happen in '?/TAB' since we call match twice */
	free(co->co_value);
	co->co_value = NULL;
    }
    if (str != NULL)
	if ((co->co_value = strdup(str)) == NULL){
	    fprintf(stderr, "%s: strdup: %s\n", __FUNCTION__, strerror(errno));
	    return -1;
	}
    return 0;
}

/*
 * cligen_reason
 * Create (malloc) a reason string using var-list args
 * Really a general purpose routine but used (now) for error handling.
 * Return values:
 * Upon successful return, it returns a malloced string containing the reason.
 * NULL on error. 
 * NB: this needs to be freed.
 *
 * Arguments:
 * fmt:    format string followed by a variable list (as in  printf) 
 */
char *
cligen_reason(const char *fmt, ...)
{
    char   *reason;
    int     res;
    int     len;
    va_list ap;

    va_start (ap, fmt);
    len = vsnprintf(NULL, 0, fmt, ap);
    len++;
    va_end (ap);
    if ((reason = malloc(len)) == NULL){
	fprintf(stderr, "%s: malloc: %s\n", __FUNCTION__, strerror(errno));
	return NULL;
    }
    va_start (ap, fmt);

    if ((res = vsnprintf(reason, len, fmt, ap)) < 0){
	free(reason);
	reason = NULL;
    }
    return reason;	
}

/*! Apply a function call recursively on all cg_obj:s in a parse-tree
 *
 * Recursively traverse all cg_obj in a parse-tree and apply fn(arg) for each
 * object found. The function is called with the cg_obj and an argument as args.
 */
int
pt_apply(parse_tree pt, cg_applyfn_t fn, void *arg)
{
    cg_obj *co;
    int     i;
    int     retval = -1;

    if (pt.pt_vec == NULL)
	return 0;
    for (i=0; i<pt.pt_len; i++){
	if ((co = pt.pt_vec[i]) == NULL)
	    continue;
	if (fn(co, arg) < 0)
	    goto done;
	if (pt_apply(co->co_pt, fn, arg) < 0)
	    goto done;
    }
    retval = 0;
  done:
    return retval;
}
