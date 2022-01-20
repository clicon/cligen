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
*/
#include "cligen_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <inttypes.h>
#include <unistd.h>
#include <sys/types.h>
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
#include "cligen_handle.h"
#include "cligen_print.h"

#define VARIABLE_PRE  '<'
#define VARIABLE_POST '>'


/* Static prototypes */
static int pt2cbuf(cbuf *cb, parse_tree *pt, int level, int brief);

/*! Print the syntax specification of a variable syntax spec to a cligen buf
 *
 * @param[out] cb    CLIgen buffer string to print to
 * @param[in]  co    Cligen object from where to print
 * @param[in]  brief If set, just <varname>, otherwise clispec parsable format
 * @retval     0     OK
 * @retval    -1     Error
 * @code
 *    cbuf *cb = cbuf_new();
 *    cov2cbuf(cb, co, brief);
 *    cbuf_free(cb);
 * @endcode
 *
 * Brief output omits help-strings and variable options except names. Eg:
 * brief=0:  a("help string") <x:int32>("variable"), cb();
 * brief=1:  a <x>;
 */
int 
cov2cbuf(cbuf   *cb,
	 cg_obj *co,
	 int     brief)
{
    int     retval = -1;
    int     i;
    cg_var *cv1;
    cg_var *cv2;

    if (co->co_choice){
	if (strchr(co->co_choice, '|'))
	    cprintf(cb, "(%s)", co->co_choice);
	else
	    cprintf(cb, "%s", co->co_choice);
    }
    else{
	if (brief){
	    cprintf(cb, "%c%s%c", VARIABLE_PRE, 
		    co->co_show!=NULL ? co->co_show : co->co_command, 
		    VARIABLE_POST);
	}
	else{
	    cprintf(cb, "%c%s:%s", VARIABLE_PRE, co->co_command, cv_type2str(co->co_vtype));

	    for (i=0; i<co->co_rangelen; i++){
		if (cv_isint(co->co_vtype))
		    cprintf(cb, " range[");
		else
		    cprintf(cb, " length[");
		cv1 = cvec_i(co->co_rangecvv_low, i);
		cv2 = cvec_i(co->co_rangecvv_upp, i);
		if (cv_type_get(cv1) != CGV_EMPTY){
		    cv2cbuf(cv1, cb);
		    cprintf(cb, ":");
		}
		cv2cbuf(cv2, cb);
		cprintf(cb, "]");
	    }
	    
	    if (co->co_show)
		cprintf(cb, " show:\"%s\"", co->co_show);
	    if (co->co_expand_fn_str){
		cprintf(cb, " %s(\"", co->co_expand_fn_str);
		if (co->co_expand_fn_vec){
		    cg_var *cv = NULL;
		    int   i = 0;
		    while ((cv = cvec_each(co->co_expand_fn_vec, cv)) != NULL) {
			if (i++)
			    cprintf(cb, ",");
			cv2cbuf(cv, cb);
		    }
		}
		cprintf(cb, "\")");
	    }
	    cv1 = NULL;
	    while ((cv1 = cvec_each(co->co_regex, cv1)) != NULL)
		cprintf(cb, " regexp:\"%s\"", cv_string_get(cv1));		
	    if (co->co_translate_fn_str)
		cprintf(cb, " translate:%s()", co->co_translate_fn_str);
	    cprintf(cb, "%c", VARIABLE_POST);
	}
    }
    retval = 0;
    return retval;
}

/*! Print a CLIgen object (cg object / co) to a CLIgen buffer
 */
static int 
co2cbuf(cbuf   *cb,
	cg_obj *co,
	int     marginal,
	int     brief)
{
    int          retval = -1;
    cg_callback *cc;
    parse_tree  *pt;
    cg_obj      *co1;
    cg_obj      *cot;
    cg_var      *cv;

    if (co == NULL){
	errno = EINVAL;
	goto done;
    }
    /* Add [] if optional. Note this is neither not complete/correct since it only notes
     * that the symbol was created within a [], not the exact composition.
     * Thus, for example, both "[a b]" and "[a[b]]" will be shown as "[a][b]".
     */
    if (co_flags_get(co, CO_FLAGS_OPTION))
	cprintf(cb, "[");
    switch (co->co_type){
    case CO_COMMAND:
	if (co->co_command)
	    cprintf(cb, "%s", co->co_command);
	break;
    case CO_REFERENCE:
	if (co->co_command)
	    cprintf(cb, "@%s", co->co_command);
	break;
    case CO_VARIABLE:
	cov2cbuf(cb, co, brief);
	break;
    case CO_EMPTY:
	cprintf(cb, ";");
	break;
    }
    if (co_flags_get(co, CO_FLAGS_OPTION))
	cprintf(cb, "]");
    if (brief == 0){
	if (co->co_helpstring){
	    cbuf_append_str(cb, "(\"");
	    cbuf_append_str(cb, co->co_helpstring);
	    cbuf_append_str(cb, "\")");
	}
	if (co_flags_get(co, CO_FLAGS_HIDE))
	    cprintf(cb, ", hide");
	cv = NULL;
	while ((cv = cvec_each(co->co_cvec, cv)) != NULL) 
	    cprintf(cb, ", %s", cv_name_get(cv));
	for (cc = co->co_callbacks; cc; cc = co_callback_next(cc)){
	    co_callback2cbuf(cb, cc);
	}
    }
    cv = NULL;
    while ((cv = cvec_each(co->co_cvec, cv)) != NULL)
	cprintf(cb, ", %s", cv_name_get(cv));
    if (co_terminal(co, &cot)){
	cg_var *cv = NULL;
	while ((cv = cvec_each(cot->co_cvec, cv)) != NULL)
	    cprintf(cb, ", %s", cv_name_get(cv));
	cprintf(cb, ";");
    }
    pt = co_pt_get(co);
    if (pt_len_get(pt) > 1){
	if (co_sets_get(co))
	    cprintf(cb, "@");
	cprintf(cb, "{\n");
    }
    else
	if (pt_len_get(pt)==1 && (co1 = pt_vec_i_get(pt, 0)) != NULL && co1->co_type != CO_EMPTY)
	    cprintf(cb, " ");
	else
	    cprintf(cb, "\n");
    if (pt2cbuf(cb, pt, marginal+3, brief) < 0)
	goto done;
    if (pt_len_get(pt)>1){
	cprintf(cb, "%*s", marginal, ""); 
	cprintf(cb, "}\n");
    }
    retval = 0;
  done:
    return retval;
}

/*! Print a CLIgen parse-tree to a cbuf
 * @param[in,out] cb     CLIgen buffer
 * @param[in]     pt     Cligen parse-tree consisting of cg objects and variables
 * @param[in]     marginal How many columns to print 
 * @param[in]     brief  Print brief output, otherwise clispec parsable format
 */
static int 
pt2cbuf(cbuf       *cb,
	parse_tree *pt,
	int         marginal,
	int         brief)
{
    int     retval = -1;
    int     i;
    cg_obj *co;

    if (pt == NULL){
	errno = EINVAL;
	goto done;
    }
    for (i=0; i<pt_len_get(pt); i++){
	if ((co = pt_vec_i_get(pt, i)) == NULL)
	    continue;
	if (co->co_type == CO_EMPTY)
	    continue;
	if (pt_len_get(pt) > 1)
	    cprintf(cb, "%*s", marginal, "");
	if (co2cbuf(cb, co, marginal, brief) < 0)
	    goto done;
    }
    retval = 0;
  done:
    return retval;
}

/*! Print CLIgen parse-tree to file, brief or detailed.
 *
 * @param[in] f      Output file
 * @param[in] pt     Cligen parse-tree consisting of cg objects and variables
 * @param[in] brief  Print brief output, otherwise clispec parsable format
 *
 * The output may not be identical to the input syntax. 
 * For example [dd|ee] is printed as:
 *   dd;
 *   ee:
 * Brief output omits help-strings and variable options except names. Eg:
 * brief=0:  a("help string") <x:int32>("variable"), cb();
 * brief=1:  a <x>;
 * @see co_print which prints an individual CLIgen syntax object, not vector
 */
int 
pt_print1(FILE       *f,
	  parse_tree *pt,
	  int         brief)
{
    int   retval = -1;
    cbuf *cb = NULL;

    if ((cb = cbuf_new()) == NULL){
	fprintf(stderr, "cbuf_new: %s\n", strerror(errno));
	goto done;
    }
    if (pt2cbuf(cb, pt, 0, brief) < 0)
	goto done;
    fprintf(f, "%s", cbuf_get(cb));
    retval = 0;
  done:
    if (cb)
	cbuf_free(cb);
    return retval;
}

int 
pt_print(FILE       *f,
	 parse_tree *pt)
{
    return pt_print1(f, pt, 1);
}

/*! Print CLIgen parse-tree object to file, brief or detailed.
 *
 * @param[in] f      Output file
 * @param[in] co     Cligen object
 * @param[in] brief  Print brief output, otherwise clispec parsable format
 *
 * @see pt_print  which prints a vector of cligen objects "parse-tree"
 */
int 
co_print1(FILE    *f,
	  cg_obj  *co,
	  int      brief)
{
    int   retval = -1;
    cbuf *cb = NULL;

    if ((cb = cbuf_new()) == NULL){
	fprintf(stderr, "cbuf_new: %s\n", strerror(errno));
	goto done;
    }
    if (co2cbuf(cb, co, 0, brief) < 0)
	goto done;
    fprintf(f, "%s", cbuf_get(cb));
    retval = 0;
  done:
    if (cb)
	cbuf_free(cb);
    return retval;
}

int 
co_print(FILE    *f,
	 cg_obj  *co)
{
    return co_print1(f, co, 1);
}

static int co_dump1(FILE *f, cg_obj *co, int indent);

static int 
pt_dump1(FILE       *f,
	 parse_tree *pt,
	 int         indent)
{
    int     i;
    cg_obj *co;
    char   *name;
    
    name = pt_name_get(pt);
    fprintf(f, "%*s %p pt %s [%d]",
	    indent*3, "", pt,
	    name?name:"",
	    pt_len_get(pt));
    fprintf(f, "\n");
    for (i=0; i<pt_len_get(pt); i++){
	if ((co = pt_vec_i_get(pt, i)) == NULL)
	    fprintf(f, "%*s NULL\n", (indent+1)*3, "");
	else
	    co_dump1(f, co, indent+1);
    }
    return 0;
}

static int 
co_dump1(FILE    *f,
	 cg_obj  *co,
	 int      indent)
{
    parse_tree *pt;
    cg_var *cv;
    
    switch (co->co_type){
    case CO_COMMAND:
	fprintf(f, "%*s %p co %s", indent*3, "", co, co->co_command);
	if (co_sets_get(co))
	    fprintf(f, " SETS");
	if (co->co_ref)
	    fprintf(f, " ref:%p", co->co_ref);
	break;
    case CO_REFERENCE:
	fprintf(f, "%*s %p co @%s", indent*3, "", co, co->co_command);
	break;
    case CO_VARIABLE:
	fprintf(f, "%*s %p co <%s> ", indent*3, "", co, co->co_command);
	if (co->co_ref)
	    fprintf(f, " ref:%p", co->co_ref);
	if (co->co_treeref_orig)
	    fprintf(f, " treeref:%p", co->co_treeref_orig);
	break;
    case CO_EMPTY:
	fprintf(f, "%*s %p empty", indent*3, "", co);
	break;
    }
    cv = NULL;
    while ((cv = cvec_each(co->co_cvec, cv)) != NULL)
	fprintf(f, ", label=%s", cv_name_get(cv));
    fprintf(f, "\n");
    if ((pt = co_pt_get(co)) != NULL)
	pt_dump1(f, pt, indent);
    return 0;
}

/*! Debugging function for dumping a tree:s pointers
 */
int 
co_dump(FILE    *f,
	cg_obj  *co)
{
    return co_dump1(f, co, 0);
}

/*! Debugging function for dumping a tree:s pointers
 */
int 
pt_dump(FILE       *f,
	parse_tree *pt)
{
    return pt_dump1(f, pt, 0);
}

/*! Print list of CLIgen parse-trees
 *
 * @param[in] f      File to print to
 * @param[in] co     Cligen object
 * @param[in] brief  Print brief output, otherwise clispec parsable format
 *
 * @see pt_print  which prints a vector of cligen objects "parse-tree"
 */
int 
cligen_print_trees(FILE         *f,
		   cligen_handle h,
		   int           brief)
{
    int          retval = -1;
    pt_head     *ph;
    parse_tree  *pt;

    ph = NULL;
    while ((ph = cligen_ph_each(h, ph)) != NULL) {
	fprintf(stderr, "%s:\n", cligen_ph_name_get(ph));
	pt = cligen_ph_parsetree_get(ph);
	if (!brief && pt_print1(f, pt, brief) < 0)
	    goto done;
    }
    retval = 0;
  done:
    return retval;
}
