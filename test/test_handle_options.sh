#!/usr/bin/env bash
# Comprehensive test of cligen_handle.c option get/set API.
# Exercises all untested handle options:
#   cligen_regex_xsd, cligen_delimiter, cligen_preference_mode,
#   cligen_caseignore, cligen_expand_first, cligen_exclude_keys,
#   cligen_logsyntax, cligen_utf8, cligen_line_scrolling,
#   cligen_helpstring_truncate, cligen_helpstring_lines,
#   cligen_tabmode, cligen_lexicalorder, cligen_ignorecase,
#   cligen_paging, cligen_terminal_width, cligen_terminal_rows,
#   cligen_eval_wrap_fn, cligen_node_filter, cligen_treeref_flags_fn,
#   cligen_userhandle, cligen_check

# Magic line must be first in script (see README.md)
s="$_" ; . ./lib.sh || if [ "$s" = $0 ]; then exit 0; else return 0; fi

app="$dir/test_handle_options"
cfile="${app}.c"

cat <<'EOF' > $cfile
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cligen/cligen.h>

static int errors = 0;

static void
check(const char *label, int ok)
{
    if (!ok) {
        printf("FAIL: %s\n", label);
        errors++;
    } else {
        printf("OK: %s\n", label);
    }
    fflush(stdout);
}

/* Minimal stub callbacks for fn-pointer tests */
static int
eval_wrap_stub(void *arg, void **wh, const char *name, const char *fn)
{
    (void)arg; (void)wh; (void)name; (void)fn;
    return 0;
}

static int
node_filter_stub(cligen_handle h, cg_obj *co, cvec *cvv, void *arg, int *skip)
{
    (void)h; (void)co; (void)cvv; (void)arg; (void)skip;
    return 0;
}

static int
treeref_flags_stub(cligen_handle h, const char *treename, uint32_t inherit, uint32_t *flagsp)
{
    (void)h; (void)treename; (void)inherit; (void)flagsp;
    return 0;
}

int
main(void)
{
    cligen_handle h;
    cligen_eval_wrap_fn   *got_ewfn;
    cligen_node_filter_fn *got_nffn;
    cligen_treeref_flags_fn *got_trfn;
    void *got_arg;
    int   val;

    h = cligen_init();
    if (h == NULL) {
        printf("FAIL: cligen_init\n");
        return 1;
    }
    check("cligen_init ok", h != NULL);

    /* --- bool / flag options --- */

    /* regex_xsd */
    val = cligen_regex_xsd(h);
    cligen_regex_xsd_set(h, !val);
    check("regex_xsd set/get", cligen_regex_xsd(h) == !val);
    cligen_regex_xsd_set(h, val); /* restore */

    /* preference_mode */
    val = cligen_preference_mode(h);
    cligen_preference_mode_set(h, !val);
    check("preference_mode set/get", cligen_preference_mode(h) == !val);
    cligen_preference_mode_set(h, val);

    /* caseignore */
    val = cligen_caseignore_get(h);
    cligen_caseignore_set(h, !val);
    check("caseignore set/get", cligen_caseignore_get(h) == !val);
    cligen_caseignore_set(h, val);

    /* expand_first */
    val = cligen_expand_first_get(h);
    cligen_expand_first_set(h, !val);
    check("expand_first set/get", cligen_expand_first_get(h) == !val);
    cligen_expand_first_set(h, val);

    /* exclude_keys */
    val = cligen_exclude_keys_get(h);
    cligen_exclude_keys_set(h, !val);
    check("exclude_keys set/get", cligen_exclude_keys_get(h) == !val);
    cligen_exclude_keys_set(h, val);

    /* logsyntax */
    val = cligen_logsyntax(h);
    cligen_logsyntax_set(h, 3);
    check("logsyntax set/get", cligen_logsyntax(h) == 3);
    cligen_logsyntax_set(h, val);

    /* utf8 */
    val = cligen_utf8_get(h);
    cligen_utf8_set(h, !val);
    check("utf8 set/get", cligen_utf8_get(h) == !val);
    cligen_utf8_set(h, val);

    /* line_scrolling */
    val = cligen_line_scrolling(h);
    cligen_line_scrolling_set(h, !val);
    check("line_scrolling set/get", cligen_line_scrolling(h) == !val);
    cligen_line_scrolling_set(h, val);

    /* helpstring_truncate */
    val = cligen_helpstring_truncate(h);
    cligen_helpstring_truncate_set(h, !val);
    check("helpstring_truncate set/get", cligen_helpstring_truncate(h) == !val);
    cligen_helpstring_truncate_set(h, val);

    /* helpstring_lines */
    val = cligen_helpstring_lines(h);
    cligen_helpstring_lines_set(h, 42);
    check("helpstring_lines set/get", cligen_helpstring_lines(h) == 42);
    cligen_helpstring_lines_set(h, val);

    /* tabmode */
    val = cligen_tabmode(h);
    cligen_tabmode_set(h, CLIGEN_TABMODE_STEPS);
    check("tabmode set STEPS", cligen_tabmode(h) == CLIGEN_TABMODE_STEPS);
    cligen_tabmode_set(h, CLIGEN_TABMODE_COLUMNS);
    check("tabmode set COLUMNS", cligen_tabmode(h) == CLIGEN_TABMODE_COLUMNS);
    cligen_tabmode_set(h, val);

    /* lexicalorder */
    val = cligen_lexicalorder(h);
    cligen_lexicalorder_set(h, !val);
    check("lexicalorder set/get", cligen_lexicalorder(h) == !val);
    cligen_lexicalorder_set(h, val);

    /* ignorecase */
    val = cligen_ignorecase(h);
    cligen_ignorecase_set(h, !val);
    check("ignorecase set/get", cligen_ignorecase(h) == !val);
    cligen_ignorecase_set(h, val);

    /* paging */
    val = cligen_paging_get(h);
    cligen_paging_set(h, !val);
    check("paging set/get", cligen_paging_get(h) == !val);
    cligen_paging_set(h, val);

    /* terminal_width */
    val = cligen_terminal_width(h);
    cligen_terminal_width_set(h, 132);
    check("terminal_width set/get", cligen_terminal_width(h) == 132);
    cligen_terminal_width_set(h, val);

    /* terminal_rows: set falls back to 0 if not a tty */
    cligen_terminal_rows_set(h, 50);
    check("terminal_rows set ok", cligen_terminal_rows(h) == 50 || cligen_terminal_rows(h) == 0);

    /* delimiter */
    cligen_delimiter_set(h, '/');
    check("delimiter set/get /", cligen_delimiter(h) == '/');
    cligen_delimiter_set(h, ' ');
    check("delimiter set/get space", cligen_delimiter(h) == ' ');

    /* userhandle */
    int mydata = 12345;
    cligen_userhandle_set(h, (void *)&mydata);
    check("userhandle set/get", cligen_userhandle(h) == (void *)&mydata);
    cligen_userhandle_set(h, NULL);
    check("userhandle clear", cligen_userhandle(h) == NULL);

    /* --- function pointer options --- */

    /* eval_wrap_fn */
    cligen_eval_wrap_fn_set(h, eval_wrap_stub, (void *)&mydata);
    cligen_eval_wrap_fn_get(h, &got_ewfn, &got_arg);
    check("eval_wrap_fn set/get fn", got_ewfn == eval_wrap_stub);
    check("eval_wrap_fn set/get arg", got_arg == (void *)&mydata);
    cligen_eval_wrap_fn_set(h, NULL, NULL);
    cligen_eval_wrap_fn_get(h, &got_ewfn, &got_arg);
    check("eval_wrap_fn clear", got_ewfn == NULL);

    /* node_filter */
    cligen_node_filter_set(h, node_filter_stub, (void *)&mydata);
    cligen_node_filter_get(h, &got_nffn, &got_arg);
    check("node_filter set/get fn", got_nffn == node_filter_stub);
    check("node_filter set/get arg", got_arg == (void *)&mydata);
    cligen_node_filter_set(h, NULL, NULL);
    cligen_node_filter_get(h, &got_nffn, &got_arg);
    check("node_filter clear", got_nffn == NULL);

    /* treeref_flags_fn */
    cligen_treeref_flags_fn_set(h, treeref_flags_stub);
    cligen_treeref_flags_fn_get(h, &got_trfn);
    check("treeref_flags_fn set/get", got_trfn == treeref_flags_stub);
    cligen_treeref_flags_fn_set(h, NULL);
    cligen_treeref_flags_fn_get(h, &got_trfn);
    check("treeref_flags_fn clear", got_trfn == NULL);

    /* cligen_check: should return 0 on a fresh handle */
    check("cligen_check ok", cligen_check(h) == 0);

    cligen_exit(h);

    printf("\nTotal errors: %d\n", errors);
    return errors ? 1 : 0;
}
EOF

if [ "$LINKAGE" = static ]; then
    newtest "compile $cfile (static)"
    COMPILE="$CC -DHAVE_CONFIG_H -g -Wall $CFLAGS -I.. $cfile ../libcligen.a -o $app"
else
    newtest "compile $cfile"
    COMPILE="$CC -DHAVE_CONFIG_H -g -Wall $CFLAGS -I.. $cfile ../libcligen.so.${CLIGEN_VERSION_MAJOR}.${CLIGEN_VERSION_MINOR} -o $app"
fi
expectpart "$($COMPILE 2>&1)" 0 ""

OUT="$(LD_LIBRARY_PATH=.. $app 2>/dev/null)"

newtest "cligen_init"
expectpart "$OUT" 0 "OK: cligen_init ok"

newtest "bool/flag options set/get"
expectpart "$OUT" 0 \
    "OK: regex_xsd set/get" \
    "OK: preference_mode set/get" \
    "OK: caseignore set/get" \
    "OK: expand_first set/get" \
    "OK: exclude_keys set/get" \
    "OK: logsyntax set/get" \
    "OK: utf8 set/get" \
    "OK: line_scrolling set/get" \
    "OK: helpstring_truncate set/get" \
    "OK: helpstring_lines set/get"

newtest "tabmode"
expectpart "$OUT" 0 "OK: tabmode set STEPS" "OK: tabmode set COLUMNS"

newtest "remaining scalar options"
expectpart "$OUT" 0 \
    "OK: lexicalorder set/get" \
    "OK: ignorecase set/get" \
    "OK: paging set/get" \
    "OK: terminal_width set/get" \
    "OK: terminal_rows set ok" \
    "OK: delimiter set/get /" \
    "OK: delimiter set/get space"

newtest "userhandle"
expectpart "$OUT" 0 "OK: userhandle set/get" "OK: userhandle clear"

newtest "function pointer options"
expectpart "$OUT" 0 \
    "OK: eval_wrap_fn set/get fn" \
    "OK: eval_wrap_fn set/get arg" \
    "OK: eval_wrap_fn clear" \
    "OK: node_filter set/get fn" \
    "OK: node_filter set/get arg" \
    "OK: node_filter clear" \
    "OK: treeref_flags_fn set/get" \
    "OK: treeref_flags_fn clear"

newtest "cligen_check"
expectpart "$OUT" 0 "OK: cligen_check ok"

newtest "no failures"
expectpart "$OUT" 0 "Total errors: 0"

newtest "endtest"
endtest

rm -rf $dir
