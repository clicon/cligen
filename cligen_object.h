/*
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

#ifndef _CLIGEN_OBJECT_H_
#define _CLIGEN_OBJECT_H_

#define CLIGEN_DELIMITERS " \t"
#define CLIGEN_QUOTES     "\""

/*
 * Parse tree nodes. The different types are associated to
 * different fields in the parse tree node object, each interpreted/parsed
 * differently.
 */
enum cg_objtype{
  CO_COMMAND,   /* Simple string parsing */
  CO_VARIABLE,  /* Parse as type eg int */
  CO_REFERENCE  /* Symbolic reference to other tree */
};

/*
 * Types
 */
enum cligen_result{
    CG_EOF      = -2,
    CG_ERROR    = -1,
    CG_NOMATCH  =  0,
    CG_MATCH    =  1,
    CG_MULTIPLE =  2,
};
typedef enum cligen_result cligen_result;

/*
 * Callback function type. Is called after a specific syntax node has been identified.,
 *   arg is an optionalargument
 *   argc is number of variables (1...)
 *   argv[] is a vector of variables. The first is always the whole syntax string as entered.
 */
typedef int (cg_fnstype_t)(cligen_handle h, cvec *vars, cg_var *arg);
typedef int (cgv_fnstype_t)(cligen_handle h, cvec *vars, cvec *argv);

/* Expand callback function for vector arguments (should be in cligen_expand.h) 
   Returns 0 if handled expand, that is, it returned commands for 'name'
           1 if did not handle expand 
          -1 on error.
*/
typedef int (expandv_cb)(cligen_handle h,       /* handler: cligen or userhandle */
			 char         *name,    /* name of this function (in text) */
			 cvec         *cvv,     /* vars vector of values in command */
			 cvec         *argv,    /* argument vector given to callback */
			 cvec         *commands,/* vector of commands */
			 cvec         *helptexts /* vector of help-texts */
			     );

/* Translate callback. Translate a variable, eg from cleartext to
   encrypted passwords The variable type must be kept */
typedef int (translate_cb_t)(cligen_handle h, cg_var *cv);

/*! Cligen ^Z suspension callback
 */
typedef int (cligen_susp_cb_t)(void *h, char *, int, int *);

/*! Cligen ^C interrupt callback
 */
typedef int (cligen_interrupt_cb_t)(cligen_handle h);

typedef struct parse_tree parse_tree;

/*! A CLIgen object may have one or several callbacks. This type defines one callback
 */
struct cg_callback  { /* Linked list of command callbacks */
    struct  cg_callback *cc_next;    /**< Next callback in list.  */
    cgv_fnstype_t       *cc_fn_vec;  /**< callback/function pointer using cvec.  */
    char                *cc_fn_str;  /**< callback/function name. malloced */
    cvec                *cc_cvec;    /**< callback/function arguments */
};

/*
 * If cligen object is a variable, this is its variable spec.
 * But it is not complete, it is a part of a cg_obj.
 * A cg_var is the value of such a varspec.
 */
struct cg_varspec{
    enum cv_type    cgs_vtype;         /* its type */
    char           *cgs_show;          /* help text of variable */
    char           *cgs_expand_fn_str; /* expand callback string */
    expandv_cb     *cgs_expandv_fn;    /* expand callback see pt_expand */
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
#define CO_FLAGS_TREEREF   0x04  /* This node is top of expanded sub-tree */
#define CO_FLAGS_REFDONE   0x08  /* This reference has already been expanded */
#ifdef USE_SETS
#define CO_FLAGS_SETS_SUB  0x20  /* Parent is SETS, ie direct child of set (static) */
#define CO_FLAGS_SETS_GEN  0x40  /* Parent is SUBS or GEN,  */
#define CO_FLAGS_SETS_EXP  0x80  /* Child set is generated */
#endif
#define CO_FLAGS_OPTION   0x100  /* Generated from optional [] */

/*! cligen gen object is a parse-tree node. A cg_obj is either a command or a variable
 * A cg_obj 
 * @code
 *      o <--- cg_obj
 *      ^
 *      |
 *      up
 *   [0 1..n]
 *    | |  |
 *    v v  v
 *    o o  o   <--- cg_obj
 * @endcode
 */
struct cg_obj{
    parse_tree         *co_pt;        /* Child parse-tree (see co_next macro below) */
    /* Expand data: expand, choice temporarily replaces the original parse-tree
     * with one where expand and choice is replaced by string constants. 
     * This is only a reference to an original tree in co_pt. Dont free it.
     */
    struct parse_tree **co_pt_exp;    /* vector of parse-trees */
    int                 co_pt_exp_len;

    struct cg_obj      *co_prev;      /* Parent */
    enum cg_objtype     co_type;      /* Type of object: command, variable or tree
					 reference */
    char               *co_command;   /* malloc:ed matching string / name or type */
    struct cg_callback *co_callbacks; /* linked list of callbacks and arguments */
    cvec               *co_cvec;      /* List of cligen variables (XXX: not visible to
					 callbacks) */
    char	       *co_help;      /* Helptext */
    uint32_t            co_flags;     /* General purpose flags, see CO_FLAGS_ above*/

    struct cg_obj      *co_ref;       /* Ref to original (if this is expanded) */
    char               *co_value;     /* Expanded value can be a string with a constant. 
					 Store the constant in the original variable. */
    union {
	struct {        } cou_cmd;
	struct cg_varspec cou_var;
    } u;
};
typedef struct cg_obj cg_obj; /* in cli_var.h */

typedef cg_obj** co_vec_t;  /* vector of (pointers to) parse-tree nodes XXX is really cg_vec */

/* Access macro to cligen object variable specification */
#define co2varspec(co)  &(co)->u.cou_var

/* Access fields for code traversing parse tree */
#define co_vtype         u.cou_var.cgs_vtype
#define co_show          u.cou_var.cgs_show
#define co_expand_fn_str u.cou_var.cgs_expand_fn_str
#define co_expandv_fn  	 u.cou_var.cgs_expandv_fn
#define co_expand_fn_vec u.cou_var.cgs_expand_fn_vec
#define co_translate_fn_str u.cou_var.cgs_translate_fn_str
#define co_translate_fn  u.cou_var.cgs_translate_fn
#define co_choice	 u.cou_var.cgs_choice
#define co_keyword	 u.cou_var.cgs_choice
#define co_rangelen	 u.cou_var.cgs_rangelen 
#define co_rangecvv_low	 u.cou_var.cgs_rangecvv_low
#define co_rangecvv_upp  u.cou_var.cgs_rangecvv_upp
#define co_regex         u.cou_var.cgs_regex
#define co_dec64_n       u.cou_var.cgs_dec64_n

#define iskeyword(CV) 0

/* Access macro */
static inline cg_obj* 
co_up(cg_obj *co) 
{
    return co->co_prev;
}

static inline int
co_up_set(cg_obj *co, cg_obj *cop) 
{
    co->co_prev = cop;
    return 0;
}

/*! return top-of-tree (ancestor) */
static inline cg_obj* 
co_top(cg_obj *co0) 
{
    cg_obj *co = co0;
    cg_obj *co1;

    while ((co1 = co_up(co)) != NULL)
	co = co1;
    return co;
}

/*
 * Prototypes
 */
parse_tree *co_pt_get(cg_obj *co);
int         co_pt_set(cg_obj *co, parse_tree *pt);
int         co_pt_exp_purge(cg_obj *co);
int         co_pt_exp_clear(cg_obj *co);
int         co_pt_exp_add(cg_obj *co, parse_tree *ptx);

cg_obj     *co_vec_i_get(cg_obj *co, int i);
int         co_vec_len_get(cg_obj *co);
void        co_flags_set(cg_obj *co, uint32_t flag);
void        co_flags_reset(cg_obj *co, uint32_t flag);
int         co_flags_get(cg_obj *co, uint32_t flag);
int         co_sets_get(cg_obj *co);
void        co_sets_set(cg_obj *co, int sets);
cg_obj     *co_new(char *cmd, cg_obj *prev);
cg_obj     *cov_new(enum cv_type cvtype, cg_obj *prev);
int         co_pref(cg_obj *co, int exact);
int         co_callback_copy(struct cg_callback *cc0, struct cg_callback **ccn);
int         co_copy(cg_obj *co, cg_obj *parent, cg_obj **conp);
int         co_eq(cg_obj *co1, cg_obj *co2);
int         co_free(cg_obj *co, int recursive);
cg_obj     *co_insert(parse_tree *pt, cg_obj *co);
cg_obj     *co_find_one(parse_tree *pt, char *name);
int         co_value_set(cg_obj *co, char *str);
#if defined(__GNUC__) && __GNUC__ >= 3
char       *cligen_reason(const char *fmt, ...) __attribute__ ((format (printf, 1, 2)));
#else
char       *cligen_reason(const char *fmt, ...);
#endif

#endif /* _CLIGEN_OBJECT_H_ */

