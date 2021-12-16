/*
  CLI generator. Take idl as input and generate a tree for use in cli.

  ***** BEGIN LICENSE BLOCK *****
 
  Copyright (C) 2001-2021 Olof Hagsand

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

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>

#include "cligen_buf.h"
#include "cligen_cv.h"
#include "cligen_cvec.h"
#include "cligen_parsetree.h"
#include "cligen_pt_head.h"
#include "cligen_callback.h"
#include "cligen_object.h"
#include "cligen_handle.h"
#include "cligen_print.h"
#include "cligen_expand.h"
#include "cligen_syntax.h"

/* Callback function for expand variables */

/*! Copy and expand a cligen object.
 * this object could actually give rise to several if it is a variable
 * with expand (co_exp) or choice (co_choice) set.
 * Set co_ref to point back to the original.
 * @param[in]  co     Original cg_obj
 * @param[in]  co_parent Parent of original co object
 * @param[out] conp   New, shadow object
 * @see co_copy XXX: maybe this could call co_copy?
 */
static int
co_expand_sub(cg_obj  *co, 
	      cg_obj  *co_parent, 
	      cg_obj **conp)
{
    cg_obj     *con = NULL;
    parse_tree *pt;

    if ((con = co_new_only()) == NULL)
	return -1;
    memcpy(con, co, sizeof(cg_obj));
    /* Point to same underlying pt */
    con->co_ptvec = NULL;
    con->co_pt_len = 0;
    pt = co_pt_get(co);
    if (co_pt_set(con, pt) < 0)
	return -1;
    /* Replace all pointers */
    co_up_set(con, co_parent);
    if (co->co_command)
	if ((con->co_command = strdup(co->co_command)) == NULL){
	    fprintf(stderr, "%s: strdup: %s\n", __FUNCTION__, strerror(errno));
	    return -1;
	}
    if (co->co_prefix)
	if ((con->co_prefix = strdup(co->co_prefix)) == NULL){
	    fprintf(stderr, "%s: strdup: %s\n", __FUNCTION__, strerror(errno));
	    return -1;
	}
    if (co->co_cvec)
	con->co_cvec = cvec_dup(co->co_cvec);
    if (co->co_filter)
	con->co_filter = cvec_dup(co->co_filter);
    if (co_callback_copy(co->co_callbacks, &con->co_callbacks) < 0)
	return -1;
#ifdef CLIGEN_HELPSTRING_VEC
    if (co->co_helpvec)
	if ((con->co_helpvec = cvec_dup(co->co_helpvec)) == NULL)
	    return -1;
#else /* CLIGEN_HELPSTRING_VEC */
    if (co->co_helpstring)
	if ((con->co_helpstring = strdup(co->co_helpstring)) == NULL)
	    return -1;
#endif /* CLIGEN_HELPSTRING_VEC */
    if (co->co_type == CO_VARIABLE){
	if (co->co_expand_fn_str)
	    if ((con->co_expand_fn_str = strdup(co->co_expand_fn_str)) == NULL){
		fprintf(stderr, "%s: strdup: %s\n", __FUNCTION__, strerror(errno));
		return -1;
	    }
	if (co->co_expand_fn_vec)
	    if ((con->co_expand_fn_vec = cvec_dup(co->co_expand_fn_vec)) == NULL)
		return -1;
	if (co->co_translate_fn_str)
	    if ((con->co_translate_fn_str = strdup(co->co_translate_fn_str)) == NULL){
		fprintf(stderr, "%s: strdup: %s\n", __FUNCTION__, strerror(errno));
		return -1;
	    }
	if (co->co_show)
	    if ((con->co_show = strdup(co->co_show)) == NULL){
		fprintf(stderr, "%s: strdup: %s\n", __FUNCTION__, strerror(errno));
		return -1;
	    }
	if (co->co_rangecvv_low)
	    if ((con->co_rangecvv_low = cvec_dup(co->co_rangecvv_low)) == NULL)
		return -1;
	if (co->co_rangecvv_upp)
	    if ((con->co_rangecvv_upp = cvec_dup(co->co_rangecvv_upp)) == NULL)
		return -1;
	if (co->co_choice)
	    if ((con->co_choice = strdup(co->co_choice)) == NULL){
		fprintf(stderr, "%s: strdup: %s\n", __FUNCTION__, strerror(errno));
		return -1;
	    }
	if (co->co_regex)
	    if ((con->co_regex = cvec_dup(co->co_regex)) == NULL){
		fprintf(stderr, "%s: cvec_dup: %s\n", __FUNCTION__, strerror(errno));
		return -1;
	    }
    } /* CO_VARIABLE */
    con->co_ref = co;
    *conp = con;
    return 0;
}

/*! Transform string variables to commands
 * Expansion of choice or expand takes a variable (<expand> <choice>)
 * and transform them to a set of commands: <string>...<string>
 * @param[in]  co        The variable to transform to a command
 * @param[in]  cmd       Command name
 * @param[out] helptext  Helptext of command, can be NULL, is copied
 */
static int
transform_var_to_cmd(cg_obj *co, 
		     char   *cmd, 
		     char   *helptext)
{
    if (co->co_command)
	free(co->co_command);
    co->co_command = cmd; 
    if (helptext){
#ifdef CLIGEN_HELPSTRING_VEC
	if (co->co_helpvec){
	    cvec_free(co->co_helpvec);
	    co->co_helpvec = NULL;
	}
	/* helpstr can be on the form "txt1\n    txt2" */
	if (cligen_txt2cvv(helptext, &co->co_helpvec) < 0)
	    return -1;
#else /* CLIGEN_HELPSTRING_VEC */
	if (co->co_helpstring){
	    free(co->co_helpstring);
	    co->co_helpstring = NULL;
	}
	if (helptext)
	    co->co_helpstring = strdup(helptext); /* XXX: can change to consume helpstring */
#endif /* CLIGEN_HELPSTRING_VEC */
    }
    if (co->co_expandv_fn)
	co->co_expandv_fn = NULL;
    if (co->co_expand_fn_str){
	free(co->co_expand_fn_str);
	co->co_expand_fn_str = NULL;
    }
    if (co->co_expand_fn_vec){
	cvec_free(co->co_expand_fn_vec);
	co->co_expand_fn_vec = NULL;
    }
    if (co->co_translate_fn_str){
	free(co->co_translate_fn_str);
	co->co_translate_fn_str = NULL;
    }
    if (co->co_show){
	free(co->co_show);
	co->co_show = NULL;
    }
    if (co->co_rangecvv_low){
	cvec_free(co->co_rangecvv_low);
	co->co_rangecvv_low = NULL;
    }
    if (co->co_rangecvv_upp){
	cvec_free(co->co_rangecvv_upp);
	co->co_rangecvv_upp = NULL;
    }
    if (co->co_choice){
	free(co->co_choice);
	co->co_choice = NULL;
    }
    if (co->co_regex){
	cvec_free(co->co_regex);
	co->co_regex = NULL;
    }
    co->co_type = CO_COMMAND;
    return 0;
}

/*! Expand treeref one level: make a copy of cot into conp
 * 
 * @param[in]  cot      Original object
 * @param[in]  coparent Parent to both cot and con
 * @param[out] conp     New object
 * @retval     0
 * @retval    -1
 */
static int
pt_expand_treeref_one(cg_obj  *cot,
		      cg_obj  *coparent,
		      cg_obj **conp)
{
    int     retval = -1;
    cg_obj *con;

    /* see co_expand_sub */
    if ((con = co_new_only()) == NULL)
	goto done;
    memcpy(con, cot, sizeof(cg_obj));
    co_flags_set(con, CO_FLAGS_TREEREF); /* Mark expanded refd tree */
    con->co_treeref_orig = cot;
    /* Point to same underlying pt */
    con->co_ptvec = NULL;
    con->co_pt_len = 0;
    if (co_pt_set(con, co_pt_get(cot)) < 0)
	goto done;
    /* Replace all pointers */
    co_up_set(con, coparent);
    if (cot->co_command)
	if ((con->co_command = strdup(cot->co_command)) == NULL){
	    fprintf(stderr, "%s: strdup: %s\n", __FUNCTION__, strerror(errno));
	    goto done;
	}
    if (cot->co_prefix)
	if ((con->co_prefix = strdup(cot->co_prefix)) == NULL){
	    fprintf(stderr, "%s: strdup: %s\n", __FUNCTION__, strerror(errno));
	    goto done;
	}
    if (cot->co_cvec)
	con->co_cvec = cvec_dup(cot->co_cvec);
    if (cot->co_filter)
	con->co_filter = cvec_dup(cot->co_filter);
    if (co_callback_copy(cot->co_callbacks, &con->co_callbacks) < 0)
	goto done;
#ifdef CLIGEN_HELPSTRING_VEC
    if (cot->co_helpvec)
	if ((con->co_helpvec = cvec_dup(cot->co_helpvec)) == NULL)
	    goto done;
#else /* CLIGEN_HELPSTRING_VEC */
    if (cot->co_helpstring)
	if ((con->co_helpstring = strdup(cot->co_helpstring)) == NULL)
	    goto done;
#endif /* CLIGEN_HELPSTRING_VEC */
    if (cot->co_type == CO_VARIABLE){
	if (cot->co_expand_fn_str)
	    if ((con->co_expand_fn_str = strdup(cot->co_expand_fn_str)) == NULL){
		fprintf(stderr, "%s: strdup: %s\n", __FUNCTION__, strerror(errno));
		goto done;
	    }
	if (cot->co_expand_fn_vec)
	    if ((con->co_expand_fn_vec = cvec_dup(cot->co_expand_fn_vec)) == NULL)
		goto done;
	if (cot->co_translate_fn_str)
	    if ((con->co_translate_fn_str = strdup(cot->co_translate_fn_str)) == NULL){
		fprintf(stderr, "%s: strdup: %s\n", __FUNCTION__, strerror(errno));
		goto done;
	    }
	if (cot->co_show)
	    if ((con->co_show = strdup(cot->co_show)) == NULL){
		fprintf(stderr, "%s: strdup: %s\n", __FUNCTION__, strerror(errno));
		goto done;
	    }
	if (cot->co_rangecvv_low)
	    if ((con->co_rangecvv_low = cvec_dup(cot->co_rangecvv_low)) == NULL)
		goto done;
	if (cot->co_rangecvv_upp)
	    if ((con->co_rangecvv_upp = cvec_dup(cot->co_rangecvv_upp)) == NULL)
		goto done;
	if (cot->co_choice)
	    if ((con->co_choice = strdup(cot->co_choice)) == NULL){
		fprintf(stderr, "%s: strdup: %s\n", __FUNCTION__, strerror(errno));
		goto done;
	    }
	if (cot->co_regex)
	    if ((con->co_regex = cvec_dup(cot->co_regex)) == NULL){
		fprintf(stderr, "%s: cvec_dup: %s\n", __FUNCTION__, strerror(errno));
		goto done;
	    }
    } /* CO_VARIABLE */
    *conp = con;
    retval = 0;
 done:
    return retval;
}

/*! Find filter labels defined as "filter:@remove:<label>": and return in cvv  
 * @param[out]  cvv  Cligen variable vector with filters as names
 */
static int
co_find_label_filters(cligen_handle h,
		      cg_obj       *co,
		      cvec         *cvv)
{
    int     retval = -1;
    cg_var *cv;
    char   *name;
    char   *filter;
    cg_var *cv1;
    
    cv = NULL;
    while ((cv = cvec_each(co->co_cvec, cv)) != NULL){
	if ((name = cv_name_get(cv)) != NULL){
	    if (strncmp(CLIGEN_REF_REMOVE, name, strlen(CLIGEN_REF_REMOVE)) == 0){
		filter = name+strlen(CLIGEN_REF_REMOVE);
		/* If filter not in cvv, add it */
		if (cvec_find(cvv, filter) == NULL){
		    if ((cv1 = cvec_add(cvv, CGV_STRING)) == NULL)
			goto done;
		    cv_name_set(cv1, filter);
		}
	    }
	}
    }
    retval = 0;
 done:
    return retval;
}

/*! Make a pt expand of single co using shallow copy
 *
 * @param[in]     h      Handle needed to resolve tree-references (\@tree)
 * @param[in]     co     Reference object, @tree
 * @param[in]     ptref  Referenced by pt0 orig
 * @param[in,out] pt0    parse-tree to expand. In: original, out: expanded
 * @retval        0      OK
 * @retval       -1      Error
 */
static int 
co_expand_treeref_copy_shallow(cligen_handle h,
			       cg_obj       *co,
			       parse_tree   *ptref,
			       parse_tree   *pt0)
{
    int     retval = -1;
    cg_obj *coparent;
    cvec   *cvv = NULL;
    int     i;
    cg_obj *cot;            /* treeref object */
    cg_obj *con;
    
    /* make a copy of ptref -> pt1ref */
    coparent = co_up(co);
    /* Filter label code
     * Prepare new cvv filter add/subtract to cvvfilter depending on co_cvec */
    if ((cvv = cvec_new(0)) == NULL)
	goto done;
    /* Find filter labels defined as "filter:@remove:<label>": and return in cvv  */
    if (co_find_label_filters(h, co, cvv) < 0)
	goto done;
    /* Copy top-levels into original parse-tree */
    for (i=0; i<pt_len_get(ptref); i++){
	if ((cot = pt_vec_i_get(ptref, i)) == NULL)
	    continue;
	/* caveats for tree expansion */
	if (cot->co_type == CO_EMPTY)
	    continue;
	if (pt_expand_treeref_one(cot, coparent, &con) < 0)
	    goto done;
	con->co_ref = co; /* Backpointer to the "ref" node */
	if (cvec_len(cvv) &&
	    (con->co_filter = cvec_dup(cvv)) == NULL)
	    goto done;
	if (co_insert(pt0, con) == NULL) 
	    goto done;
    } /* for i */
    /* Due to loop above, all co in vec should be moved, it should
       be safe to remove */
    co_flags_set(co, CO_FLAGS_REFDONE);
    retval = 0;
 done:
    if (cvv)
	cvec_free(cvv);

    return retval;
}

/*! Take a top-object parse-tree (pt0), and expand all tree references one level. 
 * 
 * One level only. Parse-tree is expanded itself (not copy).
 *
 * @param[in]     h     Handle needed to resolve tree-references (\@tree)
 * @param[in]     co0   Parent, if any
 * @param[in,out] pt0   parse-tree to expand. In: original, out: expanded
 * @retval        0     OK
 * @retval       -1     Error
 * @note The loop may need to suboptimal iterations since after every time you
 *       find a tree reference, you add new elements to the list and re-iterates
 *       from start. Since the expanded elements may be references,... 
 * @see pt_expand_treeref_cleanup
 */
static int
pt_expand_treeref(cligen_handle h, 
		  cg_obj       *co0, 
		  parse_tree   *pt0)
{
    int         retval = -1;
    int         i;
    cg_obj     *co;
    parse_tree *ptref = NULL;   /* tree referenced by pt0 orig */
    char       *treename;
    cg_obj     *cow;
    pt_head    *ph;
    
 again: /* XXX ugly goto , try to replace with a loop */
    for (i=0; i<pt_len_get(pt0); i++){ /*  */
	if ((co = pt_vec_i_get(pt0, i)) == NULL)
	    continue;
	if (co->co_type == CO_REFERENCE && !co_flags_get(co, CO_FLAGS_REFDONE)){
	    /* Expansion is made in-line so we need to know if already 
	       expanded */
	    treename = co->co_command;
	    /* Get parse tree header */
	    if ((ph = cligen_ph_find(h, treename)) == NULL){
		fprintf(stderr, "CLIgen tree '%s' not found\n", treename);
		goto done;
	    }
	    /* Get working point of tree, if any */
	    if ((cow = cligen_ph_workpoint_get(ph)) != NULL)
		ptref = co_pt_get(cow);
	    else
		ptref = cligen_ph_parsetree_get(ph);	    
	    if (co_expand_treeref_copy_shallow(h, co, ptref, pt0) < 0)
		goto done;
	    goto again; 
	}
    }
    retval = 0;
 done:
    return retval;
}

/*! Escape special characters in a string for its usage as CLI keyword.
 *  If no escaping is required, return original string.
 *  Otherwise, allocate a new string for escaped result.
 *
 * @param[in]   s   Raw string.
 *
 * @return  Either original string equal to @paramref s
 *          or a escaped string which must be freed by the caller.
 **/
static const char*
cligen_escape(const char* s)
{
	char       *copy;
	size_t      len;
	int         chars_to_escape = 0;
	const char *spec;
	int         i, j;

	spec = s;
	//	while ((spec = strpbrk(spec, "?\\"))) {
	while ((spec = strpbrk(spec, "?\\ \t"))) {
		if (chars_to_escape == 0) {
			chars_to_escape = 2; /* escapes */
		}
		if (!isspace(*spec)) {
			chars_to_escape++;
		}
		spec++;
	}

	if (!chars_to_escape) {
		return s;
	}

	len = strlen(s);

	copy = (char*)malloc(len + 1 + chars_to_escape);
	if (!copy) {
		return NULL;
	}

	copy[0] = '"';
	for (i = 0, j = 1; i < len; i++, j++) {
		if ((s[i] == '?') || (s[i] == '\\')) {
			copy[j++] = '\\';
		}
		copy[j] = s[i];
	}
	copy[j++] = '"';
	copy[j] = '\0';

	return copy;
}

/*! Call expand callback and insert expanded commands in place of variable
 * variable argument callback variant
 * @param[in]  h       CLIgen handle
 * @param[in]  co      CLIgen object
 * @param[out] cvv     Cligen variable vector containing vars/values pair for completion
 * @param[out] ptn     New parse-tree initially an empty pointer, its value is returned.
 * @param[in]  co      CLIgen object parent
 * @retval     0       OK
 * @retval    -1       Error
 * This is the only place where expand callbacks are invoked
 * @see cligen_eval where cligen callbacks are invoked
 */
static int
pt_expand_fnv(cligen_handle h, 
	      cg_obj       *co,     
	      cvec         *cvv,
	      parse_tree   *ptn,
	      cg_obj       *co_parent)
{
    int         retval = -1;
    cvec       *commands;
    cvec       *helptexts;
    cg_var     *cv = NULL;
    char       *helpstr;
    cg_obj     *con = NULL;
    int         i;
    const char *value;
    const char *escaped;
    cvec       *cvv1 = NULL; /* Modified */

    if (cvv == NULL){
	errno = EINVAL;
	goto done;
    }
    if ((commands = cvec_new(0)) == NULL)
	goto done;
    if ((helptexts = cvec_new(0)) == NULL)
	goto done;
    /* Make a copy of var argument for modifications */
    if ((cvv1 = cvec_dup(cvv)) == NULL)
	goto done;
    /* Make modifications to cvv */
    if (cvec_exclude_keys(cvv1) < 0)
	goto done;
    if ((*co->co_expandv_fn)(cligen_userhandle(h)?cligen_userhandle(h):h, 
			     co->co_expand_fn_str, 
			     cvv1,
			     co->co_expand_fn_vec,
			     commands, 
			     helptexts) < 0)
	goto done;
    i = 0;
    while ((cv = cvec_each(commands, cv)) != NULL) {
	if (i < cvec_len(helptexts))
	    helpstr = strdup(cv_string_get(cvec_i(helptexts, i)));
	else
	    helpstr = NULL;
	i++;
	con = NULL;
	if (co_expand_sub(co, co_parent, &con) < 0)
	    goto done;
	if (pt_vec_append(ptn, con) < 0)
	    goto done;
	value = cv_string_get(cv);
	escaped = cligen_escape(value);
	if (escaped == value) {
	    if ((escaped = strdup(escaped)) == NULL) /* XXX: leaks memory */
		goto done;
	}
	/* 'escaped' always points to mutable string */
	if (transform_var_to_cmd(con, (char*)escaped, helpstr) < 0)
	    goto done;
	if (helpstr){
	    free(helpstr);
	    helpstr = NULL;
	}
    }
    if (commands)
	cvec_free(commands);
    if (helptexts)
	cvec_free(helptexts);
    retval = 0;
 done:
    if (cvv1)
	cvec_free(cvv1);
    return retval;

}

/*! Expand a choice rule with actual commands
 * @param[in]  co        Original cligen object (to expand into ptn)
 * @param[out] ptn       New parse-tree initially an empty pointer, its value is returned.
 * @retval     0         OK
 * @retval    -1         Error
 */
static int
pt_expand_choice(cg_obj       *co,     
		 parse_tree   *ptn)
{
    int     retval = -1;
    char   *ccmd;
    char   *cp = NULL;
    char   *c;
    cg_obj *con = NULL;

    /* parse co_command and get alternatives <alt:hej,hopp> */
    if (co->co_choice){
	cp = ccmd = strdup(co->co_choice);
	while ((c = strsep(&ccmd, ",|")) != NULL){
	    con = NULL;
	    if (co_expand_sub(co, NULL, &con) < 0)
		goto done;
	    if (pt_vec_append(ptn, con) < 0)
		goto done;
	    if (transform_var_to_cmd(con, strdup(c), NULL) < 0) 
		goto done;
	}
    }
    retval = 0;
 done:
    if (cp)
	free(cp);
    return retval;
}

/*! Check if label is filtered in co
 *
 * @retval    0   No, label is not in co_filter, keep it
 * @retval    1   Yes, it is filtered, remove it
 */
int
co_isfilter(cvec *cvv_filter,
	    char *label)
{
    cg_var *cv = NULL;
    char   *name;
    
    /* use filter cache? 
     */
    while ((cv = cvec_each(cvv_filter, cv)) != NULL){
	if ((name = cv_name_get(cv)) != NULL && strcmp(cv_name_get(cv), label) == 0)
	    return 1;
    }
    return 0;
}
			
/*! Take a pattern pt and expand all <variables> with option 'choice' or 'expand' into new ptn tree
 *
 * The pattern is expanded by examining the objects they point to: those objects that are expand 
 * or choice variables
 * (eg <string expand:foo>) are transformed into a set of new commands with a reference point back
 * to the original.
 * The structure of the new parsetree ptn is a little peculiar, it only creates a new top-level
 * with new, temporary expanded cg-objects, but they in turn point back to the original
 * parse-tree. Therefore this new parse-tree cannot be free:d recursively.
 * @param[in]  h       Cligen handle
 * @param[in]  pt      Original parse-tree consisting of a vector of cligen objects
 * @param[in]  hide    If not set, include hidden commands. If set, do not include hidden commands. 
 * @param[in]  expandvar Set if VARS should be expanded, eg ? <tab>
 * @param[in]  cvv_filter  Label filter, remove if present
 * @param[out] cvv_var Cligen variable vector containing vars/values pair for completion
 * @param[out] ptn     New parse-tree initially an empty pointer, its value is returned.
 * @retval     0       OK
 * @retval    -1       Error
 */
static int
pt_expand(cligen_handle h, 
	  parse_tree   *pt,
	  int           hide,
	  int           expandvar,
	  cvec         *cvv_filter,
	  cvec         *cvv_var,
	  parse_tree   *ptn)
{
    int     i;
    cg_obj *co;
    cg_obj *con = NULL;
    int     retval = -1;
    cg_var *cv = NULL;

    if (pt_len_get(ptn) != 0){
	errno = EINVAL;
	goto done;	
    }
    pt_sets_set(ptn, pt_sets_get(pt));
    if (pt_len_get(pt) == 0)
	goto ok;
    for (i=0; i<pt_len_get(pt); i++){ /* Build ptn (new) from pt (orig) */
	if ((co = pt_vec_i_get(pt, i)) != NULL){
	    if (co_value_set(co, NULL) < 0)
		goto done;
	    if (hide && co_flags_get(co, CO_FLAGS_HIDE))
		continue;
	    /* Loop labels from object itself and see if any of the  are filtered, if so skip it
	     */
	    cv = NULL;
	    while ((cv = cvec_each(co->co_cvec, cv)) != NULL){
		if (co->co_filter && co_isfilter(co->co_filter, cv_name_get(cv)))
		    break;
		if (cvv_filter && co_isfilter(cvv_filter, cv_name_get(cv)))
		    break;
	    }
	    if (cv)
		continue;
	    /*
	     * Choice variable - Insert the static choices as commands in place
	     * of the variable
	     */
	    if (co->co_type == CO_VARIABLE && co->co_choice != NULL){
		if (pt_expand_choice(co, ptn) < 0)
		    goto done;
	    }
	    /* Expand variable - call expand callback and insert expanded
	     * commands in place of the variable
	     */
	    else if (co->co_type == CO_VARIABLE && 
		     co->co_expandv_fn != NULL){
		/* If I add conditional here, you need to explicitly have a
		 * a "free" variable expression, not just expands.
		 * eg (<v:int expand_dbvar()>|<v:int>)
		 * If I put the conditional in the if-statement above you
		 * may then get two variables and ambiguous command
		 * MAYBE you could see if there are any other same variables on
		 * this iteration and if not add it?
		 */
		if (expandvar){
		    if (pt_expand_fnv(h, co, cvv_var, ptn, NULL) < 0)
			goto done;
		}
	    }
	    else{
		/* Copy original cg_obj to shadow list*/
		con = NULL;
		if (co_expand_sub(co, NULL, &con) < 0)
		    goto done;
		if (pt_vec_append(ptn, con) < 0)
		    goto done;
		if (cvv_filter && cvec_len(cvv_filter))
		    if ((con->co_filter = cvec_dup(cvv_filter)) == NULL) /* XXX merge? */
			goto done;
	    }
	}
	else{ 
	    pt_realloc(ptn); /* empty child */
	}
    } /* for */
    /* Sorting (Alt: ensure all elements are inserted properly)
     * Sorting is disabled for now. Left this comment but seems to work fine without
     *    cligen_parsetree_sort(ptn, 1);
     */
    if (cligen_logsyntax(h) > 0){
	fprintf(stderr, "%s:\n", __FUNCTION__);
	pt_print1(stderr, ptn, 0);
    }
 ok:
    retval = 0;
 done:
    return retval;
}

/*! Go through tree and clean & delete all extra memory from pt_expand_treeref()
 * More specifically, delete all expanded subtrees co_ref
 * @param[in] h     CLIgen handle
 * @param[in] pt   Parsetree
 * @retval    0    OK
 * @retval   -1    Error
 * @see pt_expand_treeref
 * XXX Could make a limit to recursion at some level, but need to research more
 */
static int
pt_expand_treeref_cleanup(cligen_handle h,
			  parse_tree   *pt,
			  int           level)
{
    int     retval = -1;
    int     i;
    cg_obj *co;

    for (i=0; i<pt_len_get(pt); i++){
      again:
	if ((co = pt_vec_i_get(pt, i)) != NULL){
	    if (co_flags_get(co, CO_FLAGS_REFDONE)){
		co_flags_reset(co, CO_FLAGS_REFDONE);
	    }
	    if (co_flags_get(co, CO_FLAGS_TREEREF)){
		if (pt_vec_i_delete(pt, i, 0) < 0)
		    goto done;
		if (i < pt_len_get(pt))
		    goto again;
		else
		    break;
	    }
	    else
		if (pt_expand_treeref_cleanup(h, co_pt_get(co), level+1) < 0)
		    return -1;
	}
    }
    retval = 0;
 done:
    return retval;
}

/*! Go through tree and clean & delete all extra memory from pt_expand()
 * More specifically, delete all co_values and co_pt_exp.
 * @param[in] pt   Parsetree
 * @retval    0    OK
 * @retval   -1    Error
 * @see pt_expand
 */
static int
pt_expand_cleanup(parse_tree *pt)
{
    int         retval = -1;
    int         i;
    cg_obj     *co;

    for (i=0; i<pt_len_get(pt); i++){
	if ((co = pt_vec_i_get(pt, i)) != NULL){
	    if (co_value_set(co, NULL) < 0)
		return -1;
	}
    }
    retval = 0;
    // done:
    return retval;
}

/*! Take a pattern pt and expand all tree references, and all <variables> into new ptn tree
 *
 * @param[in]  h       Cligen handle
 * @param[in]  co      Parent, if any
 * @param[in]  pt      Original parse-tree consisting of a vector of cligen objects
 * @param[out] cvv     Cligen variable vector containing vars/values pair for completion
 * @param[in]  hide    If not set, include hidden commands. If set, do not include hidden commands. 
 * @param[in]  expandvar Set if VARS should be expanded, eg ? <tab>
 * @param[out] ptn     New parse-tree initially an empty pointer, its value is returned.
 * @retval     0       OK
 * @retval    -1       Error
 * @see pt_expand1_cleanup
 */
int
pt_expand1(cligen_handle h, 
	   cg_obj       *co, 
	   parse_tree   *pt, 
	   cvec         *cvv_var,
	   int           hide,
	   int           expandvar,
	   parse_tree   *ptn)
{
    int     retval = -1;

    if (pt_expand_treeref(h, co, pt) < 0) /* sub-tree expansion */
	goto done;
    if (pt_expand(h, pt, hide, expandvar, co?co->co_filter:NULL, cvv_var, ptn) < 0)
	goto done;
    retval = 0;
 done:
    return retval;
}

/*! Go through tree and clean & delete all extra memory from pt_expand and pt_expand_treeref
 * More specifically, delete all co_values and co_pt_exp and expanded subtrees co_ref
 * @param[in] h     CLIgen handle
 * @param[in] pt   Parsetree
 * @retval    0    OK
 * @retval   -1    Error
 * @see pt_expand1
 */
int
pt_expand1_cleanup(cligen_handle h,
		   parse_tree *pt)
{
    if (pt_expand_cleanup(pt) < 0)
	return -1;
    if (pt_expand_treeref_cleanup(h, pt, 0) < 0)
	return -1;
    return 0;
}

/*! Return object in original tree from  object in a referenced tree
 * Given an object in a referenced tree, and the top of the original tree,
 * return the corresponding object in the original tree.
 * The figure tries to show:
     ref-tree      orig
     -----        ----- pt0
       o            o (given top)
     /   \        /   \
    o     o      o     o
     \            \
      o co1         o co0
       (given)     (we want to find this)
 */
int
reference_path_match(cg_obj     *co1, 
		     parse_tree *pt0, 
		     cg_obj    **co0p)
{
    cg_obj    *co0, *co;

    if (co1 == NULL)
	return -1;
    if (co_flags_get(co1, CO_FLAGS_TREEREF)){ /* at top */
	if ((co0 = co_find_one(pt0, co1->co_command)) == NULL)
	    return -1;
	*co0p = co0;
	return 0;
    }
    if (reference_path_match(co_up(co1), pt0, &co) < 0)
	return -1;
    if ((co0 = co_find_one(co_pt_get(co), co1->co_command)) == NULL)
	return -1;
    *co0p = co0;
    return 0;
}

