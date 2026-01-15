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

#ifndef _CLIGEN_OBJECT_H_
#define _CLIGEN_OBJECT_H_

#define CLIGEN_DELIMITERS " \t"
#define CLIGEN_QUOTES     "\""
#define CLIGEN_OPERATORS  "|"

/* CLIgen preferences, see CLIgen tutorial appendix D, higher is "better"
 */
#define COV_PREF_ERROR           0
#define COV_PREF_REST            1
#define COV_PREF_COMMAND_PARTIAL 3
#define COV_PREF_STRING          6
#define COV_PREF_STRING_REGEXP   8  /* Needs some slack to next for multiple matches */
#define COV_PREF_STRING_EXPAND  14
#define COV_PREF_BOOL           16
#define COV_PREF_INTERFACE      18
#define COV_PREF_URL            20
#define COV_PREF_UINT64         45
#define COV_PREF_INT64          (COV_PREF_UINT64+1)
#define COV_PREF_UINT32         (COV_PREF_UINT64+2)
#define COV_PREF_INT32          (COV_PREF_UINT64+3)
#define COV_PREF_UINT16         (COV_PREF_UINT64+4)
#define COV_PREF_INT16          (COV_PREF_UINT64+5)
#define COV_PREF_UINT8          (COV_PREF_UINT64+6)
#define COV_PREF_INT8           (COV_PREF_UINT64+7)
#define COV_PREF_UINT64_RANGE   45
#define COV_PREF_INT64_RANGE    (COV_PREF_UINT64_RANGE+1)
#define COV_PREF_UINT32_RANGE   (COV_PREF_UINT64_RANGE+2)
#define COV_PREF_INT32_RANGE    (COV_PREF_UINT64_RANGE+3)
#define COV_PREF_UINT16_RANGE   (COV_PREF_UINT64_RANGE+4)
#define COV_PREF_INT16_RANGE    (COV_PREF_UINT64_RANGE+5)
#define COV_PREF_UINT8_RANGE    (COV_PREF_UINT64_RANGE+6)
#define COV_PREF_INT8_RANGE     (COV_PREF_UINT64_RANGE+7)
#define COV_PREF_DECIMAL64      62
#define COV_PREF_IPV4ADDR       70
#define COV_PREF_IPV6ADDR       71
#define COV_PREF_MACADDR        72
#define COV_PREF_UUID           73
#define COV_PREF_TIME           75
#define COV_PREF_COMMAND        100

/*
 * Parse tree nodes. The different types are associated to
 * different fields in the parse tree node object, each interpreted/parsed
 * differently.
 */
enum cg_objtype{
  CO_COMMAND,   /* Simple string parsing */
  CO_VARIABLE,  /* Parse as type eg int */
  CO_REFERENCE, /* Symbolic reference to other tree */
  CO_EMPTY      /* No subtree, indicates a callback (used to be empty) */
};

/*
 * Types
 */

/* Expand callback function for vector arguments (should be in cligen_expand.h)
   Returns 0 if handled expand, that is, it returned commands for 'name'
           1 if did not handle expand
          -1 on error.
*/
typedef int (expand_cb)(cligen_handle h,       /* handler: cligen or userhandle */
                        char         *name,    /* name of this function (in text) */
                        cvec         *cvv,     /* vars vector of values in command */
                        cvec         *argv,    /* argument vector given to callback */
                        cvec         *commands,/* vector of commands */
                        cvec         *helptexts /* vector of help-texts */
                        );

#if 1  // XXX backward-compatible
typedef expand_cb expandv_cb;
#endif

/*! Callback for translating a variable,
 *
 * Typical usecase is translate a cleartext to encrypt passwords.
 * The variable type must be the same.
 * @param[in]  h   CLIgen handle
 * @param[in]  cv  CLIgen variable
 */
typedef int (translate_cb_t)(cligen_handle h, cg_var *cv);

/*! Cligen ^Z suspension callback
 */
typedef int (cligen_susp_cb_t)(void *h, const char *, int, int *);

/*! Cligen ^C interrupt callback
 */
typedef int (cligen_interrupt_cb_t)(cligen_handle h);

typedef struct parse_tree parse_tree;

/*
 * If cligen object is a variable, this is its variable spec.
 * But it is not complete, it is a part of a cg_obj.
 * A cg_var is the value of such a varspec.
 */
struct cg_varspec{
    enum cv_type    cgs_vtype;         /* its type */
    char           *cgs_show;          /* help text of variable */
    char           *cgs_expand_fn_str; /* expand callback string */
    expand_cb      *cgs_expand_fn;     /* expand callback see pt_expand */
    cvec           *cgs_expand_fn_vec; /* expand callback argument vector */
    char           *cgs_translate_fn_str; /* translate function string */
    translate_cb_t *cgs_translate_fn;  /* variable translate function */
    char           *cgs_choice;        /* list of choices */
    /* int range / str length of cvv_low/upper bound intervals. Note, the two
     * range-cvvs must have the same length. */
    int             cgs_rangelen;
    /* array of lower bound of intervals range. If cv type is CGV_EMPTY
     * it means the min value of the type (eg <a:int32 range[40]> */
    cvec           *cgs_rangecvv_low;
    cvec           *cgs_rangecvv_upp;  /* array of upper bound of intervals */
    cvec           *cgs_regex;         /* List of regular expressions */
    uint8_t         cgs_dec64_n;       /* negative decimal exponential 1..18 */
};
typedef struct cg_varspec cg_varspec;

/* Default number of fraction digits if type is DEC64 */
#define CGV_DEC64_N_DEFAULT 2

/* General purpose flags for cg_obj co_flags type
 */
#define CO_FLAGS_HIDE      0x01  /* Don't show in help/completion */
#define CO_FLAGS_MARK      0x02  /* Only used internally (for recursion avoidance) */
#define CO_FLAGS_TOPOFTREE 0x04  /* This node is top of expanded sub-tree */
#define CO_FLAGS_OPTION    0x08  /* Generated from optional [] */
#define CO_FLAGS_MATCH     0x10  /* For sets: avoid selecting same more than once */
#define CO_FLAGS_ALIAS     0x20  /* Added as an alias (see cligen_alias_cb) */

/* Flags for pt_copy and co_copy
 */
#define CO_COPY_FLAGS_TREEREF 0x01 /* If called from pt_expand_treeref: the copy point to the original */

/*! Adjusted (smaller) cg-obj for commands used for CO_COMMAND and CO_REFERENCE
 *
 * other cg-obj types (CO_VARIABLE) uses cg_obj
 * @see cg_obj
 */
struct cg_obj_common{
    parse_tree        **coc_ptvec;     /* Child parse-tree (see co_next macro below) */
    int                 coc_pt_len;    /* Length of parse-tree vector */
    struct cg_obj      *coc_prev;      /* Parent */
    enum cg_objtype     coc_type;      /* Type of object: command, variable or tree
                                         reference */
    uint16_t            coc_preference; /* Overrides default variable preference if != 0*/
    char               *coc_command;   /* malloc:ed matching string / name or type */
    char               *coc_prefix;    /* Prefix. Can be used in cases where co_command is not unique */
    cg_callback        *coc_callbacks; /* linked list of callbacks and arguments */
    cvec               *coc_cvec;      /* List of cligen local variables, such as "hide"
                                       * Special labels on @treerefs are:
                                       *     @add:<label> and @remove:<label>
                                       * which control tree ref macro expansion
                                       */
    cvec               *coc_filter;    /* List of labels that are filtered (should be removed)
                                       * In this node and all its descendant
                                       * Alt: re-use co_cvec for this.
                                       * See also reftree_filter for global filters
                                       */
    char               *coc_helpstring; /* String of CLIgen helptexts */
    uint32_t            coc_flags;     /* General purpose flags, see CO_FLAGS_HIDE and others above */
    struct cg_obj      *coc_ref;       /* Ref to original (if this is expanded)
                                        * Typical from expanded command to orig variable
                                        */
    struct cg_obj      *coc_treeref_orig; /* Ref to original (if this is a tree reference)
                                          * Only set in co_copy */
    char               *coc_value;     /* Expanded value can be a string with a constant. */

};

/*! cligen gen object is a parse-tree node. A cg_obj is either a command, a variable or a tree reference
 *
 * A cg_obj:
 *      o <--- cg_obj
 *      ^
 *      |
 *      up
 *   [0 1..n] <--- parsetree
 *    | |  |
 *    v v  v
 *    o o  o   <--- cg_obj
 *
 */
struct cg_obj{
    struct cg_obj_common co_common;   /* Inline of common fields accessed by macros below */
    /*--- Up to here common with cg_obj_cmd */
    union {                           /* depends on co_type: */
        struct {        } cou_cmd;    /* CO_COMMAND */
        struct cg_varspec cou_var;    /* CO_VARIABLE */
        //                            /* CO_REFERENCE */
    } u;

};

typedef struct cg_obj cg_obj;

/* Access macro to cligen object variable specification */
#define co2varspec(co)  &(co)->u.cou_var

/* Access fields for common fields */
#define co_ptvec         co_common.coc_ptvec
#define co_pt_len        co_common.coc_pt_len
#define co_prev          co_common.coc_prev
#define co_type          co_common.coc_type
#define co_preference    co_common.coc_preference
#define co_command       co_common.coc_command
#define co_prefix        co_common.coc_prefix
#define co_callbacks     co_common.coc_callbacks
#define co_cvec          co_common.coc_cvec
#define co_filter        co_common.coc_filter
#define co_helpstring    co_common.coc_helpstring
#define co_flags         co_common.coc_flags
#define co_ref           co_common.coc_ref
#define co_treeref_orig  co_common.coc_treeref_orig
#define co_value         co_common.coc_value

/* Access fields for code traversing parse tree */
#define co_vtype         u.cou_var.cgs_vtype
#define co_show          u.cou_var.cgs_show
#define co_expand_fn_str u.cou_var.cgs_expand_fn_str
#define co_expand_fn     u.cou_var.cgs_expand_fn
#if 1 // backward compatible
#define co_expandv_fn    co_expand_fn
#endif
#define co_expand_fn_vec u.cou_var.cgs_expand_fn_vec
#define co_translate_fn_str u.cou_var.cgs_translate_fn_str
#define co_translate_fn  u.cou_var.cgs_translate_fn
#define co_choice        u.cou_var.cgs_choice
#define co_keyword       u.cou_var.cgs_choice
#define co_rangelen      u.cou_var.cgs_rangelen
#define co_rangecvv_low  u.cou_var.cgs_rangecvv_low
#define co_rangecvv_upp  u.cou_var.cgs_rangecvv_upp
#define co_regex         u.cou_var.cgs_regex
#define co_dec64_n       u.cou_var.cgs_dec64_n

#define ISREST(co) (((co)->co_type == CO_VARIABLE && (co)->co_vtype == CGV_REST) || ((co)->co_ref && (co)->co_ref->co_type == CO_VARIABLE && (co)->co_ref->co_vtype == CGV_REST))

/*
 * Prototypes
 */
int         co_stats_global(uint64_t *created, uint64_t *nr);
int         co_stats(cg_obj *co, uint64_t *nrp, size_t *szp);
cg_obj*     co_up(cg_obj *co);
int         co_up_set(cg_obj *co, cg_obj *cop);
cg_obj*     co_top(cg_obj *co0);
parse_tree *co_pt_get(cg_obj *co);
int         co_pt_set(cg_obj *co, parse_tree *pt);
int         co_pt_clear(cg_obj *co);
void        co_flags_set(cg_obj *co, uint32_t flag);
void        co_flags_reset(cg_obj *co, uint32_t flag);
int         co_flags_get(cg_obj *co, uint32_t flag);
int         co_sets_get(cg_obj *co);
void        co_sets_set(cg_obj *co, int sets);
char       *co_prefix_get(cg_obj *co);
int         co_prefix_set(cg_obj *co, const char *prefix);
cvec       *co_filter_set(cg_obj *co, cvec *cvv);
size_t      co_size(enum cg_objtype type);
cg_obj     *co_new_only(enum cg_objtype type);
cg_obj     *co_new(const char *cmd, cg_obj *prev);
cg_obj     *cov_new(enum cv_type cvtype, cg_obj *prev);
int         co_pref(cg_obj *co, int exact);
int         co_copy(cg_obj *co, cg_obj *parent, uint32_t flags, cg_obj **conp);
int         co_copy1(cg_obj *co, cg_obj *parent, int recursive, uint32_t flags, cg_obj **conp);
int         co_eq(cg_obj *co1, cg_obj *co2);
int         co_free(cg_obj *co, int recursive);
cg_obj     *co_insert1(parse_tree *pt, cg_obj *co, int recursive);
cg_obj     *co_insert(parse_tree *pt, cg_obj *co);
cg_obj     *co_find_one(parse_tree *pt, const char *name);
int         co_value_set(cg_obj *co, const char *str);
int         co_terminal(cg_obj *co, cg_obj **cot);
char       *cligen_reason(const char *fmt, ...) __attribute__ ((format (printf, 1, 2)));

#endif /* _CLIGEN_OBJECT_H_ */

