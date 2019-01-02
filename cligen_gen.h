/*
  ***** BEGIN LICENSE BLOCK *****
 
  Copyright (C) 2001-2019 Olof Hagsand

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

#ifndef _CLIGEN_GEN_H_
#define _CLIGEN_GEN_H_

#define CLIGEN_DELIMITERS " \t"
#define CLIGEN_QUOTES     "\""

/*
 * Parse tree nodes. The different types are associated to
 * different fields in the parse tree node object, each interpreted/parsed
 * differently.
 */
enum cg_objtype{
  CO_COMMAND,   /* Simple string parsing */
  CO_VARIABLE,  /* parse as type eg int */
  CO_REFERENCE  /* Symbolic reference to other tree */
};

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



/*! A parse tree is a top object containing a vector of parse-tree nodes 
 *
 * @code
 *      o
 *      ^
 *      |
 *      up
 *   [0 1..n] <--- parse-tree
 *    | |  |
 *    v v  v
 *    o o  o
 * @endcode
 */
struct parse_tree{
    struct cg_obj     **pt_vec;    /**< vector of pointers to parse-tree nodes */
    int                 pt_len;    /**< length of vector */
    struct parse_tree  *pt_up;     /**< parent cligen object, if any */
    char               *pt_name;
#if 0
    int                 pt_obj;    /**< if this parse-tree is a part of a cg_obj (can be 
				      typecast to (cg_obj*). See co_up() below */
#endif
};
typedef struct parse_tree parse_tree;

/*! A CLIgen object may have one or several callbacks. This type defines one callback
 */
struct cg_callback  { /* Linked list of command callbacks */
    struct  cg_callback *cc_next;    /**< Next callback in list.  */
#ifdef CALLBACK_SINGLEARG
    cg_fnstype_t        *cc_fn;      /**< callback/function pointer using cv.  */
#endif
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
    expandv_cb     *cgs_expandv_fn;    /* expand callback see pt_expand_2 */
    cvec           *cgs_expand_fn_vec; /* expand callback argument vector */
    char           *cgs_translate_fn_str; /* translate function string */
    translate_cb_t *cgs_translate_fn;  /* variable translate function */
    char           *cgs_choice;        /* list of choices */
    int             cgs_range;         /* int range / str length interval valid */
    cg_var         *cgs_rangecv_low;   /* range/length interval lower limit */
    cg_var         *cgs_rangecv_high;  /* range/length interval upper limit */
    char           *cgs_regex;         /* regular expression */
    uint8_t         cgs_dec64_n;       /* negative decimal exponential 1..18 */
};
typedef struct cg_varspec cg_varspec;

/* Default number of fraction digits if type is DEC64 */
#define CGV_DEC64_N_DEFAULT 2

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
    parse_tree        co_pt;        /* Child parse-tree (see co_next macro below) */
    struct cg_obj    *co_prev;
    enum cg_objtype   co_type;      /* Type of object */
    char              co_delimiter; /* Delimiter (after this obj) */
    char             *co_command;   /* malloc:ed matching string / name or type */
    struct cg_callback *co_callbacks;/* linked list of callbacks and arguments */
    cvec             *co_cvec;       /* List of cligen variables (XXX: not visible to callbacks) */
    int               co_mark;      /* Only used internally (for recursion avoidance) */
    char	     *co_help;	    /* Brief helptext */
#ifdef notused
    int	              co_top;	    /* Top of syntax tree: use as virtual top */
#endif
    int	              co_hide;      /* Don't show in help/completion */
    char             *co_mode;      /* Name of other syntax mode */

    /* Expand data: expand, choice temporarily replaces the original parse-tree
       with one where expand and choice is replaced by string constants. */
    struct parse_tree co_pt_exp;   /* Expanded parse-trees (why > 1?) */
    struct cg_obj    *co_ref;       /* Ref to original (if this is expanded) */
    char             *co_value;     /* Expanded value can be a string with a 
				       constant. Store the constant in the original
				       variable. */
    void             *co_userdata;  /* User-specific data, malloced and defined by
				       the user. Will be freed by cligen on exit */
    size_t            co_userlen;   /* Length of the userdata (need copying) */

#ifdef notyet
    struct cg_var    *co_cv;        /* Store value directly at matching */
#endif
    int               co_treeref;   /* This node is top of expanded sub-tree */
    int               co_refdone;   /* This reference has already been expanded */
    /* Experiment to make the junos edit style work. When we graft the tree,
       these places should insert extra NULL child to be able to edit.
     */
    int               co_nonterminal; /* Set if we should be able to <CR> here */

    /* Ugly application code */
    struct parse_tree co_pt_push;   /* Saved orig of parse-tree (ugly user code) */
    int               co_pushed;    /* if pushed contains data */


    union {
	struct {        } cou_cmd;
	struct cg_varspec cou_var;
    } u;
};
typedef struct cg_obj cg_obj; /* in cli_var.h */

typedef cg_obj** pt_vec;  /* vector of (pointers to) parse-tree nodes */

/* Callback for pt_apply() */
typedef int (cg_applyfn_t)(cg_obj *co, void *arg);

/* Access macro to cligen object variable specification */
#define co2varspec(co)  &(co)->u.cou_var

/* Access fields for code traversing parse tree */
#define co_next          co_pt.pt_vec
#define co_max           co_pt.pt_len
#define co_vtype         u.cou_var.cgs_vtype
#define co_show          u.cou_var.cgs_show
#define co_expand_fn_str u.cou_var.cgs_expand_fn_str
#define co_expandv_fn  	 u.cou_var.cgs_expandv_fn
#define co_expand_fn_vec u.cou_var.cgs_expand_fn_vec
#define co_translate_fn_str u.cou_var.cgs_translate_fn_str
#define co_translate_fn  u.cou_var.cgs_translate_fn
#define co_choice	 u.cou_var.cgs_choice
#define co_keyword	 u.cou_var.cgs_choice
#define co_range	 u.cou_var.cgs_range
#define co_rangecv_low	 u.cou_var.cgs_rangecv_low
#define co_rangecv_high	 u.cou_var.cgs_rangecv_high
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
void    cligen_parsetree_sort(parse_tree pt, int recursive);
cg_obj *co_new(char *cmd, cg_obj *prev);
cg_obj *cov_new(enum cv_type cvtype, cg_obj *prev);
int     co_pref(cg_obj *co, int exact);
int     pt_realloc(parse_tree *);
int     co_callback_copy(struct cg_callback *cc0, struct cg_callback **ccn);
int     co_copy(cg_obj *co, cg_obj *parent, cg_obj **conp);
int     pt_copy(parse_tree pt, cg_obj *parent, parse_tree *ptn);
int     cligen_parsetree_merge(parse_tree *pt0, cg_obj *parent0, parse_tree pt1);
int     cligen_parsetree_free(parse_tree pt, int recurse);
int     co_free(cg_obj *co, int recursive);
cg_obj *co_insert(parse_tree *pt, cg_obj *co);
cg_obj *co_find_one(parse_tree pt, char *name);
int     co_value_set(cg_obj *co, char *str);
#if defined(__GNUC__) && __GNUC__ >= 3
char   *cligen_reason(const char *fmt, ...) __attribute__ ((format (printf, 1, 2)));
#else
char   *cligen_reason(const char *fmt, ...);
#endif
int     pt_apply(parse_tree pt, cg_applyfn_t fn, void *arg);

#endif /* _CLIGEN_GEN_H_ */

