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
#include "cligen_object.h"
#include "cligen_handle.h"
#include "cligen_match.h"
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
    if (co_callback_copy(co->co_callbacks, &con->co_callbacks) < 0)
	return -1;
    if (co->co_helpvec)
	if ((con->co_helpvec = cvec_dup(co->co_helpvec)) == NULL)
	    return -1;
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
 * @param[out] helptext  Helptext of command
 */
int
transform_var_to_cmd(cg_obj *co, 
		     char   *cmd, 
		     char   *helptext)
{
    if (co->co_command)
	free(co->co_command);
    co->co_command = cmd; 
    if (helptext){
	if (co->co_helpvec){
	    cvec_free(co->co_helpvec);
	    co->co_helpvec = NULL;
	}
	/* helpstr can be on the form "txt1\n    txt2" */
	if (cligen_txt2cvv(helptext, &co->co_helpvec) < 0)
	    return -1;
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

/*! Recursively copy callback structure to all terminal nodes in the parse-tree.
 * XXX: Actually copies to every node, even if not terminal
 * Problem is that the argument is modified according to reference rule
 * @param[in]  pt   Parse-tree
 * @param[in]  cc0  This is the parameter (calling) callback. eg fn in @sub,fn().
 *
 * The function installs the calling callback in all executable non-terminal nodes.
 * That is, it only install callbacks in non-terminal with NULL child which is 
 * where a ';' is in the syntax, eg:
 *	   a ;{} # Here a is executable
 *	   b {}  # But b is not
 * Somewhat strange semantics though:
 * - Always replace (or add if empty) original callback in co0
 * - Use local argument-list _unless_ there is none, then use callback list from cc0
 * The effect of this is that the generated trees argument are first (eg api-path)
 * and the ones in the @ref cal are appended.
 */
static int
pt_callback_reference(parse_tree         *pt, 
		      struct cg_callback *cc0)
{
    int                 i;
    cg_obj             *co;
    int                 retval = -1;
    struct cg_callback *cc;
    cg_var             *cv;

    for (i=0; i<pt_len_get(pt); i++){    
	if ((co = pt_vec_i_get(pt, i)) == NULL ||
	    co->co_type == CO_EMPTY)
	    continue;

	/* Filter out non-executable non-terminals. */
	if (co_terminal(co)){
	    /* Copy the callback from top */
	    if ((cc = co->co_callbacks) == NULL){
		if (co_callback_copy(cc0, &co->co_callbacks) < 0)
		    return -1;
	    }
	    else {
		cc->cc_fn_vec = cc0->cc_fn_vec; /*  */
		if (cc0->cc_fn_str){
		    if (cc->cc_fn_str)
			free (cc->cc_fn_str);
		    cc->cc_fn_str = strdup(cc0->cc_fn_str);
		}
		/* Append original parameters to end of call */
		if (cc0->cc_cvec){
		    cv = NULL;
		    while ((cv = cvec_each(cc0->cc_cvec, cv)) != NULL)
			cvec_append_var(cc->cc_cvec, cv);
		}
	    }
	}
	if (pt_callback_reference(co_pt_get(co), cc0) < 0)
	    goto done;
    }
    retval = 0;
  done:
    return retval;
}

/*! Truncate parse tree
 */
static int
pt_reference_trunc(parse_tree *pt)
{
    int     i;
    cg_obj *co;
    int     retval = -1;

    for (i=0; i<pt_len_get(pt); i++){    
	if ((co = pt_vec_i_get(pt, i)) == NULL)
	    continue;
	if (pt_reference_trunc(co_pt_get(co)) < 0)
	    goto done;
    }
    retval = 0;
  done:
    return retval;
}

/*! Recursively remove any parse-tree objects labelled by filter
 *
 * @param[in]  pt       CLIgen parse-tree
 * @param[in]  filter   If statement has this flag, filter it
 * @retval     0        OK
 * @retval    -1        Error
 * @see cligen_reftree_filter_set
 */
static int
pt_recurse_filter(parse_tree  *pt,
		  const char *filter)
{
    int         retval = -1;
    parse_tree *ptc;
    cg_obj     *co;
    int         i;
    cg_var     *cv = NULL;
    
    /* Iterate over all objects under pt (note can be removed in loop) */
    for (i=0; i<pt_len_get(pt); i++){
	if ((co = pt_vec_i_get(pt, i)) == NULL)
	    continue;
	/* Check if filter matches this object, if so remove it */
	cv = NULL;
	while ((cv = cvec_each(co->co_cvec, cv)) != NULL)
	    if (strcmp(filter, cv_name_get(cv)) == 0) /* match */
		break;
	if (cv){ /* match -> remove */
	    if (pt_vec_i_delete(pt, i) < 0)
		goto done;
	    i--; /* co is removed, get next in place */
	    continue;	    
	}
	if ((ptc = co_pt_get(co)) != NULL)
	    if (pt_recurse_filter(ptc, filter) < 0)
		goto done;
    }
    retval = 0;
 done:
    return retval;
}

/*! Take a top-object parse-tree (pt0), and expand all tree references one level. 
 * 
 * One level only. Parse-tree is expanded itself (not copy).
 *
 * @param[in]     h     Handle needed to resolve tree-references (\@tree)
 * @param[in]     co0   Parent, if any
 * @param[in,out] pt0   parse-tree to expand. In: original, out: expanded
 * @note The loop may need to suboptimal iterations since after every time you
 *       find a tree reference, you add new elements to the list and re-iterates
 *       from start. Since the expanded elements may be references,... 
 * @see pt_expand_treeref_cleanup
 */
int
pt_expand_treeref(cligen_handle h, 
		  cg_obj       *co0, 
		  parse_tree   *pt0)
{
    int         retval = -1;
    int         i;
    int         j;
    cg_obj     *co;
    parse_tree *ptref = 0;      /* tree referenced by pt0 orig */
    parse_tree *pt1ref = NULL;  /* tree referenced by pt0 resolved */
    cg_obj     *cot;            /* treeref object */
    char       *treename;
    cg_obj     *co02;
    cg_obj     *cow;
    pt_head    *ph;
    cg_var     *cv;
    char       *name;
    char       *filter;
    cvec       *cvv0;
    cvec       *cvv = NULL;
    cg_var     *cv1;
    
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

	    /* make a copy of ptref -> pt1ref */
	    co02 = co_up(co);

	    if ((pt1ref = pt_dup(ptref, co02)) == NULL) /* From ptref -> pt1ref */
		goto done;
	    /* Recursively add extra NULLs in non-terminals */
	    if (co_flags_get(co, CO_FLAGS_HIDE) && /* XXX: hide to trunk? */
		pt_reference_trunc(pt1ref) < 0)
		goto done;
	    /* Recursively install callback all through the referenced tree */
	    if (co->co_callbacks && 
		pt_callback_reference(pt1ref, co->co_callbacks) < 0)
		goto done;
	    /* Filter label code
	     * 1. Prepare new cvv filter add/subtract to cvvfilter depending on co_cvec */
	    if ((cvv0 = cligen_reftree_filter_get(h)) != NULL){
		if ((cvv = cvec_dup(cvv0)) == NULL)
		    goto done;
	    }
	    else
		if ((cvv = cvec_new(0)) == NULL)
		    goto done;
	    /* 2. Find filter labels defined in "filter:-<label>" or ""filter:+<label> local flags,
	     * add/remove to filter cvv list
	     */
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
		    else if (strncmp(CLIGEN_REF_ADD, name, strlen(CLIGEN_REF_ADD)) == 0){
			filter = name+strlen(CLIGEN_REF_ADD);
			/* If filter in cvv, remove it (set to NULL) */
			if ((cv1 = cvec_find(cvv, filter)) != NULL)
			    cv_name_set(cv1, NULL);
		    }
		}
	    }
	    /* 3. Now iterate over the names of cvv and filter out those on pt1ref 
	     * Then traverse pt1ref and remove all objects labelled with "filter"
	     */
	    cv = NULL;
	    while ((cv = cvec_each(cvv, cv)) != NULL){
		if ((filter = cv_name_get(cv)) != NULL){
		    if (pt_recurse_filter(pt1ref, filter) < 0)
			goto done;
		}
			
	    }
	    /* Copy top-levels into original parse-tree */
	    for (j=0; j<pt_len_get(pt1ref); j++)
		if ((cot = pt_vec_i_get(pt1ref, j)) != NULL){
		    co_flags_set(cot, CO_FLAGS_TREEREF); /* Mark expanded refd tree */
		    cot->co_ref = co; /* Backpointer so we know where this treeref is from */
		    if (co_insert(pt0, cot) == NULL) 
			goto done;
		    if (pt_vec_i_clear(pt1ref, j) < 0)
			goto done;
		}
	    /* Due to loop above, all co in vec should be moved, it should
	       be safe to remove */
	    co_flags_set(co, CO_FLAGS_REFDONE);
	    if (pt1ref){
		pt_free(pt1ref, 1);
		pt1ref = NULL;
	    }
	    goto again; 
	}
    }
    retval = 0;
 done:
    if (cvv)
	cvec_free(cvv);
    if (pt1ref)
	pt_free(pt1ref, 1);
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

    if (cvv == NULL){
	errno = EINVAL;
	goto done;
    }
    if ((commands = cvec_new(0)) == NULL)
	goto done;
    if ((helptexts = cvec_new(0)) == NULL)
	goto done;
    if ((*co->co_expandv_fn)(cligen_userhandle(h)?cligen_userhandle(h):h, 
			     co->co_expand_fn_str, 
			     cvv,
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
 * @param[out] cvv     Cligen variable vector containing vars/values pair for completion
 * @param[in]  hide    If not set, include hidden commands. If set, do not include hidden commands. 
 * @param[in]  expandvar Set if VARS should be expanded, eg ? <tab>
 * @param[out] ptn     New parse-tree initially an empty pointer, its value is returned.
 * @retval     0       OK
 * @retval    -1       Error
 */
int
pt_expand(cligen_handle h, 
	  parse_tree   *pt, 
	  cvec         *cvv,
	  int           hide,
	  int           expandvar,
	  parse_tree   *ptn)
{
    int     i;
    cg_obj *co;
    cg_obj *con = NULL;
    int     retval = -1;

    pt_sets_set(ptn, pt_sets_get(pt));
    if (pt_len_get(pt) == 0)
	goto ok;
    for (i=0; i<pt_len_get(pt); i++){ /* Build ptn (new) from pt (orig) */
	if ((co = pt_vec_i_get(pt, i)) != NULL){
	    if (co_value_set(co, NULL) < 0)
		goto done;
	    if (hide && co_flags_get(co, CO_FLAGS_HIDE))
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
		if (expandvar)
		    if (pt_expand_fnv(h, co, cvv, ptn, NULL) < 0)
			goto done;
	    }
	    else{
		/* Copy original cg_obj to shadow list*/
		con = NULL;
		if (co_expand_sub(co, NULL, &con) < 0)
		    goto done;
		if (pt_vec_append(ptn, con) < 0)
		    goto done;
	    }
	}
	else{ 
	    pt_realloc(ptn); /* empty child */
	}
    } /* for */
    cligen_parsetree_sort(ptn, 1);
    if (cligen_logsyntax(h) > 0){
	fprintf(stderr, "%s:\n", __FUNCTION__);
	pt_print(stderr, ptn, 0);
    }
 ok:
    retval = 0;
 done:
    return retval;
}

/*! Go through tree and clean & delete all extra memory from pt_expand_treeref()
 * More specifically, delete all expanded subtrees co_ref
 * @param[in] pt   Parsetree
 * @retval    0    OK
 * @retval   -1    Error
 * @see pt_expand_treeref
 */
int
pt_expand_treeref_cleanup(parse_tree *pt)
{
    int     retval = -1;
    int     i;
    cg_obj *co;

    for (i=0; i<pt_len_get(pt); i++){
      again:
	if ((co = pt_vec_i_get(pt, i)) != NULL){
	    if (co_flags_get(co, CO_FLAGS_REFDONE))
		co_flags_reset(co, CO_FLAGS_REFDONE);
	    if (co_flags_get(co, CO_FLAGS_TREEREF)){
		if (pt_vec_i_delete(pt, i) < 0)
		    goto done;
		if (i < pt_len_get(pt))
		    goto again;
		else
		    break;
	    }
	    else
		if (pt_expand_treeref_cleanup(co_pt_get(co)) < 0)
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
int
pt_expand_cleanup(parse_tree *pt)
{
    int         retval = -1;
    int         i;
    cg_obj     *co;

    for (i=0; i<pt_len_get(pt); i++){
	if ((co = pt_vec_i_get(pt, i)) != NULL){
	    if (co_value_set(co, NULL) < 0)
		return -1;
	    if (pt_expand_cleanup(co_pt_get(co)) < 0)
		goto done;
	}
    }
    retval = 0;
 done:
    return retval;
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

