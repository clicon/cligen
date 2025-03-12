/*
  CLI generator. Take idl as input and generate a tree for use in cli.

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

  * "Expand" means make a new "shadow" parse-tree (ptn) from an original (pt)
  * The shadow parsetree has new objects, but they in turn do NOT copy their parsetrees if any
  * The copying is made as follows:
  * pt -->  [0 1..n]                        [0 1..m]  <-- ptn
  *          | |  |                          | |  |
  *          v v  v                          v v  v
  *          o o  o     --- transform --->   o o  o   <-- new cg_obj:s
  *            |                               |
  *            v                               v
  * ptc -->  [0..n]                          [0..n]   <-- NOT copied
  *
  * The transformation rules:
  * 1. Do not copy if:
  *    1a. If there is any (remove) filter that matches
  *    1b. If the HIDE flags is set
  * 2. If co is VARIABLE and has expand function, copy and insert set of objects returned
  *    - set co_ref to corresponding object in co
  * 3. If co is REFERENCE, copy and insert set of objects from original tree
  *    - set co_treeref_orig to corresponding object in co
  * 4. Else, copy object
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

/*! Copy and expand a cligen object.
 *
 * This object could actually give rise to several if it is a variable
 * with expand (co_exp) or choice (co_choice) set.
 * Set co_ref to point back to the original.
 * @param[in]  co0      Original cg_obj
 * @param[in]  coparent Parent of original co object (as well as of conp)
 * @param[out] conp     New "shadow" object
 * @see pt_expand_treeref_one
 */
static int
co_expand_sub(cg_obj  *co0,
              cg_obj  *coparent,
              cg_obj **conp)
{
    int     retval = -1;
    cg_obj *con;
    size_t  size;

    if ((con = co_new_only(co0->co_type)) == NULL)
        goto done;
    size = co_size(con->co_type);
    memcpy(con, co0, size);
    /* Point to same underlying pt */
    con->co_ptvec = NULL;
    con->co_pt_len = 0;
    if (co_pt_set(con, co_pt_get(co0)) < 0)
        goto done;
    /* Replace all pointers */
    co_up_set(con, coparent);
    if (co0->co_command)
        if ((con->co_command = strdup(co0->co_command)) == NULL){
            fprintf(stderr, "%s: strdup: %s\n", __FUNCTION__, strerror(errno));
            goto done;
        }
    if (co0->co_prefix)
        if ((con->co_prefix = strdup(co0->co_prefix)) == NULL){
            fprintf(stderr, "%s: strdup: %s\n", __FUNCTION__, strerror(errno));
            goto done;
        }
    if (co0->co_cvec)
        con->co_cvec = cvec_dup(co0->co_cvec);
    if (co0->co_filter)
        co_filter_set(con, co0->co_filter);
    if (co_callback_copy(co0->co_callbacks, &con->co_callbacks) < 0)
        goto done;
    if (co0->co_helpstring)
        if ((con->co_helpstring = strdup(co0->co_helpstring)) == NULL)
            goto done;
    if (co0->co_type == CO_VARIABLE){
        if (co0->co_expand_fn_str)
            if ((con->co_expand_fn_str = strdup(co0->co_expand_fn_str)) == NULL){
                fprintf(stderr, "%s: strdup: %s\n", __FUNCTION__, strerror(errno));
                goto done;
            }
        if (co0->co_expand_fn_vec)
            if ((con->co_expand_fn_vec = cvec_dup(co0->co_expand_fn_vec)) == NULL)
                goto done;
        if (co0->co_translate_fn_str)
            if ((con->co_translate_fn_str = strdup(co0->co_translate_fn_str)) == NULL){
                fprintf(stderr, "%s: strdup: %s\n", __FUNCTION__, strerror(errno));
                goto done;
            }
        if (co0->co_show)
            if ((con->co_show = strdup(co0->co_show)) == NULL){
                fprintf(stderr, "%s: strdup: %s\n", __FUNCTION__, strerror(errno));
                goto done;
            }
        if (co0->co_rangecvv_low)
            if ((con->co_rangecvv_low = cvec_dup(co0->co_rangecvv_low)) == NULL)
                goto done;
        if (co0->co_rangecvv_upp)
            if ((con->co_rangecvv_upp = cvec_dup(co0->co_rangecvv_upp)) == NULL)
                goto done;
        if (co0->co_choice)
            if ((con->co_choice = strdup(co0->co_choice)) == NULL){
                fprintf(stderr, "%s: strdup: %s\n", __FUNCTION__, strerror(errno));
                goto done;
            }
        if (co0->co_regex)
            if ((con->co_regex = cvec_dup(co0->co_regex)) == NULL){
                fprintf(stderr, "%s: cvec_dup: %s\n", __FUNCTION__, strerror(errno));
                goto done;
            }
    } /* CO_VARIABLE */
    con->co_ref = co0; /* Backpointer to the original node */
    *conp = con;
    con = NULL;
    retval = 0;
 done:
    if (con)
        co_free(con, 0);
    return retval;
}

/*! Transform string variables to commands
 *
 * Expansion of choice or expand takes a variable (<expand> <choice>)
 * and transform them to a set of commands: <string>...<string>
 * @param[in]  co        The variable to transform to a command
 * @param[in]  cmd       Command name (must be malloced, consumed here)
 * @param[in]  helptext  Helptext of command, can be NULL already malloced is consumed
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
        if (co->co_helpstring){
            free(co->co_helpstring);
            co->co_helpstring = NULL;
        }
        co->co_helpstring = helptext;
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

/*! Look up treeref from @<treeref> and return expanded parse-tree
 *
 * @param[in]  h       Cligen handle
 * @param[in]  co      Cligen treeref object
 * @param[in]  cvt     Tokenized string: vector of tokens
 * @param[out] ptrefp  Treeref Parse-tree result
 * @retval     1       OK
 * @retval     0       No such tree
 * @retval    -1       Error
 */
static int
tree_resolve(cligen_handle h,
             cg_obj       *coref,
             cvec         *cvt,
             parse_tree  **ptrefp)
{
    int                             retval = -1;
    char                           *treename;
    char                           *treename2 = NULL;
    pt_head                        *ph;
    cg_obj                         *cow;
    cligen_tree_resolve_wrapper_fn *fn = NULL;
    void                           *arg = NULL;
    int                             ret;

    treename = coref->co_command;
    cligen_tree_resolve_wrapper_get(h, &fn, &arg);
    if (fn){
        if ((ret = fn(h, treename, cvt, arg, &treename2)) < 0)
            goto done;
        if (ret == 0) /* No wrapper use existing */ // XXX empty tree
            ;
        else          /* New malloced name */
            treename = treename2;
    }
    /* Get parse tree header */
    if ((ph = cligen_ph_find(h, treename)) == NULL) {
        fprintf(stderr, "CLIgen tree '%s' not found\n", treename);
        goto done;
    }
    /* Get working point of tree, if any,
     * and thereby the original tree (ptref)
     */
    if ((cow = cligen_ph_workpoint_get(ph)) != NULL)
        *ptrefp = co_pt_get(cow);
    else
        *ptrefp = cligen_ph_parsetree_get(ph);
    retval = 0;
 done:
    if (treename2)
        free(treename2);
    return retval;
}

/*! Find filter labels defined as "filter:@remove:<label>": and return in cvv
 *
 * @param[in]  h    CLIgen handle
 * @param[in]  co
 * @param[out] cvv  Cligen variable vector with filters as names
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
                    if ((cv1 = cvec_add(cvv, CGV_BOOL)) == NULL)
                        goto done;
                    cv_name_set(cv1, filter);
                    cv_bool_set(cv1, 0); /* remove */
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
 * @param[in]   h          Handle needed to resolve tree-references (\@tree)
 * @param[in]   co0        Reference object, @tree
 * @param[in]   cvv_filter Add these to expanded nodes co_filter, eg remove them
 * @param[in]   cvt        Tokenized string: vector of tokens
 * @param[in]   ptorig     Original parsetree
 * @param[out]  ptnew      Expanded parse-tree to expand. In: original, out: expanded
 * @retval      0          OK
 * @retval     -1          Error
 * Makes recursive unfolding of tree references which means it handles eg:
 * @a
 * a:
 * @b
 * This is to handle cases of predefined trees, like @datamodel
 */
static int
co_expand_treeref_copy_shallow(cligen_handle h,
                               cg_obj       *co0,
                               cvec         *cvv_filter,
                               cvec         *cvt,
                               parse_tree   *ptorig,
                               parse_tree   *ptnew)
{
    int         retval = -1;
    cg_obj     *coparent;
    int         i;
    cg_obj     *cot;            /* treeref object */
    cg_obj     *con = NULL;
    parse_tree *ptref2 = NULL;   /* tree referenced by pt0 orig */
    cvec       *cvv = NULL;
    cg_var     *cv;

    if (ptorig == NULL){
        errno = EINVAL;
        goto done;
    }
    coparent = co_up(co0);
    for (i=0; i<pt_len_get(ptorig); i++){
        if ((cot = pt_vec_i_get(ptorig, i)) == NULL)
            continue;
        /* caveats for tree expansion */
        if (cot->co_type == CO_EMPTY)
            continue;
        if (cot->co_type == CO_REFERENCE){
            if (tree_resolve(h, cot, cvt, &ptref2) < 0)
                goto done;
            if ((cvv = cvec_new(0)) == NULL)
                goto done;
            if (co_find_label_filters(h, cot, cvv) < 0)
                goto done;
            /* Merge with cvv_filter */
            cv = NULL;
            while ((cv = cvec_each(cvv_filter, cv)) != NULL) {
                if (cv_name_get(cv) && cvec_find(cvv, cv_name_get(cv)) != NULL)
                    continue;
                if (cvec_append_var(cvv, cv) == NULL)
                    goto done;
            }
            if (co_expand_treeref_copy_shallow(h, co0, cvv, cvt, ptref2, ptnew) < 0)
                goto done;
            cvec_free(cvv);
            cvv = NULL;
        }
        else{
            if (co_expand_sub(cot, coparent, &con) < 0)
                goto done;
            /* Backpointer to the "ref" node - overrides co_expand_sub
             * This actually seems to be to the treehead?
             */
            con->co_ref = co0;
            co_flags_set(con, CO_FLAGS_TOPOFTREE); /* Mark expanded refd tree */
            con->co_treeref_orig = cot;
            if (cvec_len(cvv_filter) &&
                co_filter_set(con, cvv_filter) == NULL)
                goto done;
            if (co_insert(ptnew, con) == NULL)
                goto done;
            con = NULL;
        }
    } /* for i */
    retval = 0;
 done:
    if (con)
        co_free(con, 0);
    if (cvv)
        cvec_free(cvv);
    return retval;
}

/*! Count length of escaped string
 *
 * @param[in]  s    Original string.
 * @retval     len  Length of string after escaping
 */
static int
cligen_escape_nr(const char *s)
{
    int         nr = 0;
    const char *sp;

    sp = s;
    while ((sp = strpbrk(sp, "?\\ \t"))) {
        if (nr == 0)
            nr = 2; /* escapes */
        if (!isspace(*sp)) {
            nr++;
        }
        sp++;
    }
    return nr;
}

/*! Check if string needs escaping
 *
 * @param[in]  s    Original string.
 * @retval     1    Yes needs escaping
 * @retval     0    No does not need escaping
 * @retval    -1    Error
 */
int
cligen_escape_need(const char *s)
{
    size_t      len;
    int         nr = 0;

    if (s == NULL){
        errno = EINVAL;
        return -1;
    }
    nr = cligen_escape_nr(s);
    if (nr == 0)
        return 0;
    len = strlen(s);
    if (len > 1 && s[0] == '"' && s[len-1] == '"')
        return 0;
    return 1;
}

/*! Escape string
 *
 * @param[in]  s0   Original string.
 * @retval     s1   New malloced string
 * @retval     NULL Error does not need escaping
 */
char *
cligen_escape_do(const char *s0)
{
    char  *s1 = NULL;
    size_t len;
    int    nr = 0;
    int    i;
    int    j;

    if (s0 == NULL){
        errno = EINVAL;
        return NULL;
    }
    len = strlen(s0);
    nr = cligen_escape_nr(s0);
    if ((s1 = (char*)malloc(len + 1 + nr)) == NULL){
        return NULL;
    }
    j = 0;
    s1[j++] = '"';
    for (i = 0; i < len; i++, j++) {
        if ((s0[i] == '?') || (s0[i] == '\\')) {
            s1[j++] = '\\';
        }
        s1[j] = s0[i];
    }
    s1[j++] = '"';
    s1[j] = '\0';
    return s1;
}

/*! Call expand callback and insert expanded commands in place of variable
 *
 * Variable argument callback variant
 * @param[in]  h          CLIgen handle
 * @param[in]  co         CLIgen object
 * @param[in]  co_parent  CLIgen object parent
 * @param[out] cvv_var    Cligen variable vector containing vars/values pair for completion
 * @param[in]  cvv_filter Add these to expanded nodes co_filter, eg remove them
 * @param[in]  callbacks  Callback structure of expanded treeref
 * @param[in]  transient  co may be "transient" if so use co->co_ref as new co_ref,...
 * @param[out] ptn        New parse-tree initially an empty pointer, its value is returned.
 * @retval     0          OK
 * @retval    -1          Error
 * This is the only place where expand callbacks are invoked
 * @see cligen_eval where cligen callbacks are invoked
 */
static int
pt_expand_fn(cligen_handle h,
             cg_obj       *co,
             cg_obj       *co_parent,
             cvec         *cvv_var,
             cvec         *cvv_filter,
             cg_callback  *callbacks,
             int           transient,
             parse_tree   *ptn)
{
    int         retval = -1;
    cvec       *commands = NULL;
    cvec       *helptexts = NULL;
    cg_var     *cv = NULL;
    char       *helpstr = NULL;
    cg_obj     *con = NULL;
    int         i;
    const char *value;
    const char *cmd;
    cvec       *cvv1 = NULL; /* Modified */

    if (cvv_var == NULL){
        errno = EINVAL;
        goto done;
    }
    if ((commands = cvec_new(0)) == NULL)
        goto done;
    if ((helptexts = cvec_new(0)) == NULL)
        goto done;
    /* Make a copy of cvv argument for modifications */
    if ((cvv1 = cvec_dup(cvv_var)) == NULL)
        goto done;
    /* Make modifications to cvv */
    if (cvec_exclude_keys(cvv1) < 0)
        goto done;
    if (callbacks && callbacks->cc_cvec){
        cligen_callback_arguments_set(h, callbacks->cc_cvec);
    }
    cligen_co_match_set(h, co);     /* For eventual use in callback */
    if ((*co->co_expandv_fn)(cligen_userhandle(h)?cligen_userhandle(h):h,
                             co->co_expand_fn_str,
                             cvv1,
                             co->co_expand_fn_vec,
                             commands,
                             helptexts) < 0)
        goto done;
    i = 0;
    while ((cv = cvec_each(commands, cv)) != NULL) {
        if (i < cvec_len(helptexts)){
            if ((helpstr = strdup(cv_string_get(cvec_i(helptexts, i)))) == NULL)
                goto done;
        }
        else
            helpstr = NULL;
        i++;
        con = NULL;
        if (co_expand_sub(co, co_parent, &con) < 0)
            goto done;
        if (transient && con->co_ref){
            con->co_ref = co->co_ref;
        }
        if (pt_vec_append(ptn, con) < 0)
            goto done;
        if (cvv_filter && cvec_len(cvv_filter))
            if (co_filter_set(con, cvv_filter) == NULL)
                goto done;
        value = cv_string_get(cv);
        if ((cmd = strdup(value)) == NULL)
            goto done;
        /* 'cmd' always points to mutable string, helpstr is consumed */
        if (transform_var_to_cmd(con, (char*)cmd, helpstr) < 0){
            helpstr = NULL;
            goto done;
        }
        helpstr = NULL;
        /* Save the unescaped string */
        if (cmd != value)
            co_value_set(con, (char*)value);
    }
    retval = 0;
 done:
    if (commands)
        cvec_free(commands);
    if (helptexts)
        cvec_free(helptexts);
    if (callbacks && callbacks->cc_cvec)
        cligen_callback_arguments_set(h, NULL);
    if (cvv1)
        cvec_free(cvv1);
    if (helpstr)
        free(helpstr);
    return retval;
}

/*! Expand a choice rule with actual commands
 *
 * @param[in]  co         Original cligen object (to expand into ptn)
 * @param[in]  cvv_filter Add these to expanded nodes co_filter, eg remove them
 * @param[out] ptn        New parse-tree initially an empty pointer, its value is returned.
 * @retval     0          OK
 * @retval    -1          Error
 */
static int
pt_expand_choice(cg_obj       *co,
                 cvec         *cvv_filter,
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
            if (co_expand_sub(co, NULL, &con) < 0)
                goto done;
            if (transform_var_to_cmd(con, strdup(c), NULL) < 0)
                goto done;
            if (cvv_filter && co_filter_set(con, cvv_filter) == NULL)
                goto done;
            /* con may be deleted in the call and need to be replaced */
            if (co_insert1(ptn, con, 0) == NULL)
                goto done;
            con = NULL;
        }
    }
    retval = 0;
 done:
    if (con)
        co_free(con, 0);
    if (cp)
        free(cp);
    return retval;
}

/*! Check if label is filtered in co
 *
 * @retval    1   Yes, it is filtered, remove it
 * @retval    0   No, label is not in co_filter, keep it
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

/*! pt_expand function for expanding children to parse-tree
 *
 * @param[in]     h          Cligen handle
 * @param[in]     co         Original cligen object, new copy into ptn
 * @param[in]     hide       If 0, include hidden commands. If 1, do not include hidden commands.
 * @param[in]     expandvar  Set if VARS should be expanded, eg ? <tab>
 * @param[in]     cvv_var    Cligen variable vector containing vars/values pair for completion
 * @param[in]     cvv_filter Add these to expanded nodes co_filter, eg remove them
 * @param[in]     callbacks  Callback structure of expanded treeref
 * @param[in]     transient  co may be "transient" if so use co->co_ref as new co_ref,...
 * @param[in,out] ptn        New expanded parse-tree
 * @retval        0          OK
 * @retval       -1          Error
 */
static int
pt_expand1_co(cligen_handle h,
              cg_obj       *co,
              int           hide,
              int           expandvar,
              cvec         *cvv_var,
              cvec         *cvv_filter,
              cg_callback  *callbacks,
              int           transient,
              parse_tree   *ptn)
{
    int     retval = -1;
    cg_var *cv;
    cg_obj *con = NULL;
    char   *label;

    if (co_value_set(co, NULL) < 0)
        goto done;
    if (hide && co_flags_get(co, CO_FLAGS_HIDE))
        goto ok;
    /* Loop labels from object itself and see if any of the elements are filtered, if so skip it
     */
    cv = NULL;
    while ((cv = cvec_each(co->co_cvec, cv)) != NULL){
        label = cv_name_get(cv);
        if (co->co_filter && co_isfilter(co->co_filter, label))
            break;
        if (cvv_filter && co_isfilter(cvv_filter, label))
            break;
    }
    if (cv) /* found: break in the while loop ^ */
        goto ok;
    /*
     * Choice variable - Insert the static choices as commands in place
     * of the variable
     */
    if (co->co_type == CO_VARIABLE && co->co_choice != NULL){
        if (pt_expand_choice(co, cvv_filter, ptn) < 0) // XXX filter
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
            if (pt_expand_fn(h, co, NULL,
                             cvv_var,
                             cvv_filter,
                             callbacks,
                             transient,
                             ptn) < 0)
                goto done;
        }
    }
    else{
        /* Copy original cg_obj to shadow list*/
        con = NULL;
        if (co_expand_sub(co, NULL, &con) < 0)
            goto done;
        /* con may be deleted in the call and need to be replaced */
        if ((con = co_insert1(ptn, con, 0)) == NULL)
            goto done;
        if (transient && con->co_ref){
            con->co_ref = co->co_ref;
        }
        if (cvv_filter && cvec_len(cvv_filter))
            if (co_filter_set(con, cvv_filter) == NULL)
                goto done;
    }
 ok:
    retval = 0;
 done:
    return retval;
}

/*! Return name of local pipe reference, if any
 */
char *
pt_local_pipe(parse_tree *pt)
{
    int     i;
    cg_obj *co;

    for (i=0; i<pt_len_get(pt); i++){ /* From pt (orig) build ptn (new) */
        if ((co = pt_vec_i_get(pt, i)) != NULL &&
            co->co_type == CO_REFERENCE &&
            *co->co_command == '|'){
            return co->co_command;
        }
    }
    return NULL;
}

/*! Sub-routine to pt_expand for tree reference nodes
 *
 * @param[in]     h          Cligen handle
 * @param[in]     co         Tree reference cligen object
 * @param[in]     cvt        Tokenized string: vector of tokens
 * @param[in]     cvv_var    Cligen variable vector containing vars/values pair for completion
 * @param[in]     cvv_filter Add these to expanded nodes co_filter, eg remove them
 * @param[in]     hide       If 0, include hidden commands. If 1, do not include hidden commands.
 * @param[in]     expandvar  Set if VARS should be expanded, eg ? <tab>
 * @param[in]     callbacks  Callback structure of expanded treeref
 * @param[in,out] ptn        New expanded parse-tree
 * @retval        0          OK
 * @retval       -1          Error
 */
static int
pt_expand_reference(cligen_handle h,
                    cg_obj       *coref,
                    cvec         *cvt,
                    cvec         *cvv_var,
                    cvec         *cvv_filter,
                    int           hide,
                    int           expandvar,
                    cg_callback  *callbacks,
                    parse_tree   *ptn)
{
    int         retval = -1;
    parse_tree *ptref = NULL;   /* tree referenced by pt0 orig */
    cvec       *cvv2 = NULL;
    parse_tree *pttmp = NULL;
    cg_obj     *cot;
    int         i;

    ptref = NULL;
    if (tree_resolve(h, coref, cvt, &ptref) < 0)
        goto done;
    /* pttmp is a transient copy of the expanded tree */
    if ((pttmp = pt_new()) == NULL)
        goto done;
    if ((cvv2 = cvec_new(0)) == NULL)
        goto done;
    if (co_find_label_filters(h, coref, cvv2) < 0)
        goto done;
    /* Expand ptref to pttmp */
    if (co_expand_treeref_copy_shallow(h, coref, cvv2, cvv_var, ptref, pttmp) < 0)
        goto done;
    /* Copy the expand tree to the final tree.
     */
    for (i=0; i<pt_len_get(pttmp); i++){
        if ((cot = pt_vec_i_get(pttmp, i)) == NULL)
            continue;
        if (pt_expand1_co(h, cot, hide, expandvar,
                          cvv_var,
                          cvv_filter,
                          callbacks,
                          1,
                          ptn) < 0)
            goto done;
    }
    retval = 0;
 done:
    if (cvv2)
        cvec_free(cvv2);
    if (pttmp)
        pt_free(pttmp, 0);
    return retval;
}

/*! Expand all tree references and <variables> from an original to a new parsetree
 *
 * The parsetree is expanded by examining the objects they point to:
 * - those objects that are expansion-variables (eg <string expand:foo>)
 * - or choice variables
 * are transformed into a new parsetree vector with commands with a reference point back
 * to the original.
 * Further,
 * - all tree references on the form @tree are expanded with its original content.
 * The structure of the new parsetree ptn only creates a new top-level with new, temporary
 * expanded cg-objects, but they in turn point back to the original
 * parse-tree. Therefore this new parse-tree cannot be free:d recursively.
 *
 * @param[in]     h         Cligen handle
 * @param[in]     co0       Parent, if any
 * @param[in]     pt        Original parse-tree consisting of a vector of cligen objects
 * @param[in]     cvt       Tokenized string: vector of tokens
 * @param[in]     cvv_var   Cligen variable vector containing vars/values pair for completion
 * @param[in]     hide      If 0, include hidden commands. If 1, do not include hidden commands.
 * @param[in]     expandvar Set if VARS should be expanded, eg ? <tab>
 * @param[in]     callbacks Callback structure of expanded treeref
 * @param[in]     co_pipe   Pipe output extra default menu
 * @param[in,out] ptn       New expanded parse-tree
 * @retval        0         OK
 * @retval       -1         Error
 * @see pt_expand_cleanup
 * @note there is a cornercase that has to do with multiple non-disjoint tree references:
 *       If tree t1 and t2 have sub-co:s in common, they may destructively modify each other,
 *       eg @t1:{a;b} @t2:{b;c}
 *       then the common sub-elements of t1 can have extra elements of t2: @t1:{a;b;c}
 */
int
pt_expand(cligen_handle h,
          cg_obj       *co0,
          parse_tree   *pt,
          cvec         *cvt,
          cvec         *cvv_var,
          int           hide,
          int           expandvar,
          cg_callback  *callbacks,
          cg_obj       *co_pipe,
          parse_tree   *ptn)
{
    int         retval = -1;
    cg_obj     *co;
    int         i;
    cvec       *cvv_filter = NULL;
    cg_obj     *cop;

    if (pt_len_get(ptn) != 0){
        errno = EINVAL;
        goto done;
    }
    /* Maybe require */
    cvv_filter = co0?co0->co_filter:NULL;
    pt_sets_set(ptn, pt_sets_get(pt));
    if (pt_len_get(pt) == 0)
        goto ok;
    /* There is already a @|pipe menu on this level, no need for default */
    for (i=0; i<pt_len_get(pt); i++){ /* From pt (orig) build ptn (new) */
        if ((co = pt_vec_i_get(pt, i)) == NULL){
            pt_realloc(ptn); /* empty child */
        }
        else {
            if (co->co_type == CO_REFERENCE){
                if (pt_expand_reference(h, co, cvt, cvv_var,
                                        cvv_filter,
                                        hide, expandvar,
                                        callbacks,
                                        ptn) < 0)
                    goto done;
            }
            else{
                if (pt_expand1_co(h, co, hide, expandvar,
                                  cvv_var,
                                  cvv_filter,
                                  callbacks,
                                  0,
                                  ptn) < 0)
                    goto done;
                /* Given a terminal and output-pipe, add default output pipe tree reference
                 * See also match_pattern_sets where "callbacks" is set
                 * Note ph is either top-level tree or current, see assignment of pipe_default above
                 */
                if (co->co_type == CO_EMPTY &&
                    (cop = co->co_prev) != NULL &&
                    cop->co_callbacks &&
                    co_pipe != NULL){
                    if (co0->co_callbacks){
                        if (co_callback_copy(co0->co_callbacks, &co_pipe->co_callbacks) < 0)
                            goto done;
                    }
                    if (pt_expand_reference(h, co_pipe, cvt, cvv_var,
                                            cvv_filter,
                                            hide, expandvar,
                                            callbacks,
                                            ptn) < 0)
                        goto done;
                }
            }
        } /* for */
    } /* for */
    /* Sorting (Alt: ensure all elements are inserted properly)
     * Sorting must be here so that duplicates can be detected
     */
    cligen_parsetree_sort(ptn, 0);
    if (cligen_logsyntax(h) > 0){
        fprintf(stderr, "%s:\n", __FUNCTION__);
        pt_print1(stderr, ptn, 0);
    }
 ok:
    retval = 0;
 done:
    return retval;
}

/*! Go through tree and clean & delete all extra memory from pt_expand and pt_expand_treeref
 *
 * @param[in] h    CLIgen handle
 * @param[in] pt   Parsetree
 * @retval    0    OK
 * @retval   -1    Error
 * @see pt_expand
 */
int
pt_expand_cleanup(cligen_handle h,
                  parse_tree *pt)
{
    int         retval = -1;
    int         i;
    cg_obj     *co;

    for (i=0; i<pt_len_get(pt); i++){
        if ((co = pt_vec_i_get(pt, i)) != NULL){
            if (co_value_set(co, NULL) < 0)
                goto done;
        }
    }
    retval = 0;
 done:
    return retval;
}

/*! Return object in original tree from  object in a referenced tree
 *
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
    if (co_flags_get(co1, CO_FLAGS_TOPOFTREE)){ /* at top */
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
