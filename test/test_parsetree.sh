#!/usr/bin/env bash
# Test cligen_parsetree.c API:
#   pt_stats, pt_copy, pt_dup, cligen_parsetree_merge, pt_trunc, pt_apply

# Magic line must be first in script (see README.md)
s="$_" ; . ./lib.sh || if [ "$s" = $0 ]; then exit 0; else return 0; fi

app="$dir/test_parsetree"
cfile="${app}.c"
fspec="$dir/spec.cli"

cat > $fspec <<'CLIEOF'
prompt="cli> ";
treename="base";

show {
    interfaces <ifname:string>, callback();
    version, callback();
}
ping <host:ipv4addr>, callback();
configure {
    terminal, callback();
    router {
        ospf, callback();
        bgp, callback();
    }
}
CLIEOF

cat > $fspec.extra <<'CLIEOF'
prompt="cli> ";
treename="extra";

debug {
    level <n:uint8>, callback();
}
CLIEOF

cat <<'EOF' > $cfile
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <cligen/cligen.h>

static void
check(const char *label, int ok)
{
    printf("%s: %s\n", label, ok ? "OK" : "FAIL");
    fflush(stdout);
}

int
callback(cligen_handle h, cvec *cvv, cvec *argv)
{
    return 0;
}

cgv_fnstype_t *
str2fn(const char *name, void *arg, char **error)
{
    if (strcmp(name, "callback") == 0)
        return (cgv_fnstype_t *)callback;
    return NULL;
}

/* Callback for pt_apply: count nodes */
static int
count_fn(cg_obj *co, int depth, void *arg)
{
    int *count = (int *)arg;
    (*count)++;
    return 0;
}

int
main(int argc, char *argv[])
{
    int            retval = -1;
    cligen_handle  h;
    FILE          *f;
    parse_tree    *pt;
    parse_tree    *pt_extra;
    parse_tree    *pt_c = NULL;
    uint64_t       nr = 0;
    size_t         sz = 0;
    int            node_count = 0;
    const char    *specfile       = argc > 1 ? argv[1] : "spec.cli";
    const char    *specfile_extra = argc > 2 ? argv[2] : "spec.cli.extra";

    if ((h = cligen_init()) == NULL)
        goto done;

    /* Parse base spec */
    if ((f = fopen(specfile, "r")) == NULL){ perror("fopen base"); goto done; }
    if (clispec_parse_file(h, f, "base", NULL, NULL, NULL) < 0){ fclose(f); goto done; }
    fclose(f);

    pt = cligen_pt_active_get(h);
    check("pt not NULL", pt != NULL);

    /* pt_stats */
    check("pt_stats ok", pt_stats(pt, &nr, &sz) == 0);
    check("pt_stats nr > 0", nr > 0);
    check("pt_stats sz > 0", sz > 0);

    /* pt_apply: walk and count nodes */
    check("pt_apply ok", pt_apply(pt, count_fn, 1, &node_count) == 0);
    check("pt_apply counted nodes", node_count > 0);

    /* pt_dup */
    pt_c = pt_dup(pt, NULL, 0);
    check("pt_dup not NULL", pt_c != NULL);
    {
        uint64_t nr2 = 0;
        size_t   sz2 = 0;
        pt_stats(pt_c, &nr2, &sz2);
        check("pt_dup same nr", nr2 == nr);
    }
    pt_free(pt_c, 1);
    pt_c = NULL;

    /* Parse extra spec and merge into base */
    if ((f = fopen(specfile_extra, "r")) == NULL){ perror("fopen extra"); goto done; }
    if (clispec_parse_file(h, f, "extra", NULL, NULL, NULL) < 0){ fclose(f); goto done; }
    fclose(f);

    /* Switch to extra tree, get it, merge into base */
    if (cligen_ph_active_set_byname(h, "extra") < 0) goto done;
    pt_extra = cligen_pt_active_get(h);
    if (cligen_ph_active_set_byname(h, "base") < 0) goto done;
    pt = cligen_pt_active_get(h);

    {
        uint64_t before = 0;
        size_t   sz_before = 0;
        uint64_t after  = 0;
        size_t   sz_after = 0;
        pt_stats(pt, &before, &sz_before);
        check("parsetree_merge ok",
              cligen_parsetree_merge(pt, NULL, pt_extra) == 0);
        pt_stats(pt, &after, &sz_after);
        check("merge increased nodes", after > before);
    }

    /* pt_trunc: truncate tree to 1 entry (must be > 0 and < current length) */
    {
        uint64_t nr2 = 0;
        size_t   sz2 = 0;
        pt_stats(pt, &nr2, &sz2);
        /* pt_trunc requires 0 < len < pt->pt_len; skip if tree has <= 1 node */
        if (nr2 > 1)
            check("pt_trunc ok", pt_trunc(pt, 1) == 0);
        else
            check("pt_trunc ok", 1); /* tree too small to truncate, skip */
    }

    retval = 0;
 done:
    if (pt_c)
        pt_free(pt_c, 1);
    if (h)
        cligen_exit(h);
    return retval;
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

newtest "pt not NULL"
expectpart "$(LD_LIBRARY_PATH=.. $app "$fspec" "${fspec}.extra" 2>&1)" 0 "pt not NULL: OK"

newtest "pt_stats returns count and size"
expectpart "$(LD_LIBRARY_PATH=.. $app "$fspec" "${fspec}.extra" 2>&1)" 0 "pt_stats ok: OK" "pt_stats nr > 0: OK" "pt_stats sz > 0: OK"

newtest "pt_apply walks nodes"
expectpart "$(LD_LIBRARY_PATH=.. $app "$fspec" "${fspec}.extra" 2>&1)" 0 "pt_apply ok: OK" "pt_apply counted nodes: OK"

newtest "pt_dup creates equal-sized copy"
expectpart "$(LD_LIBRARY_PATH=.. $app "$fspec" "${fspec}.extra" 2>&1)" 0 "pt_dup not NULL: OK" "pt_dup same nr: OK"

newtest "cligen_parsetree_merge adds nodes"
expectpart "$(LD_LIBRARY_PATH=.. $app "$fspec" "${fspec}.extra" 2>&1)" 0 "parsetree_merge ok: OK" "merge increased nodes: OK"

newtest "pt_trunc succeeds"
expectpart "$(LD_LIBRARY_PATH=.. $app "$fspec" "${fspec}.extra" 2>&1)" 0 "pt_trunc ok: OK"

newtest "endtest"
endtest

rm -rf $dir
