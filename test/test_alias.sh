#!/usr/bin/env bash
# Test cligen_syntax.c alias API:
#   cligen_alias_add, cligen_alias_call

# Magic line must be first in script (see README.md)
s="$_" ; . ./lib.sh || if [ "$s" = $0 ]; then exit 0; else return 0; fi

app="$dir/test_alias"
cfile="${app}.c"
fspec="$dir/spec.cli"

cat > $fspec <<'CLIEOF'
prompt="cli> ";
treename="test";

show("Show commands") {
    interfaces("Display interfaces") <ifname:string>("Interface name"), callback();
    version("Display version"), callback();
}
ping("Send ping") <host:ipv4addr>("Target host"), callback();
configure("Configure") {
    terminal("Terminal"), callback();
}
CLIEOF

cat <<'EOF' > $cfile
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cligen/cligen.h>

static int last_cmd_len = 0;

static void
check(const char *label, int ok)
{
    printf("%s: %s\n", label, ok ? "OK" : "FAIL");
    fflush(stdout);
}

int
callback(cligen_handle h, cvec *cvv, cvec *argv)
{
    last_cmd_len = cvec_len(cvv);
    printf("CALLBACK cvv_len=%d\n", cvec_len(cvv));
    fflush(stdout);
    return 0;
}

cgv_fnstype_t *
str2fn(const char *name, void *arg, char **error)
{
    if (strcmp(name, "callback") == 0)
        return (cgv_fnstype_t *)callback;
    return NULL;
}

int
main(int argc, char *argv[])
{
    int            retval = -1;
    cligen_handle  h;
    FILE          *f;
    parse_tree    *pt;
    char          *line = NULL;
    int            cb_retval;
    cligen_result  result;
    char          *reason = NULL;
    const char    *specfile = argc > 1 ? argv[1] : "spec.cli";

    if ((h = cligen_init()) == NULL)
        goto done;
    if ((f = fopen(specfile, "r")) == NULL){
        perror("fopen");
        goto done;
    }
    if (clispec_parse_file(h, f, "test", NULL, NULL, NULL) < 0){
        fclose(f);
        goto done;
    }
    fclose(f);
    pt = cligen_pt_active_get(h);
    if (cligen_callbackv_str2fn(pt, str2fn, NULL) < 0)
        goto done;

    /* Add alias: "sv" -> "show version" */
    check("alias_add sv ok",
          cligen_alias_add(h, NULL, "sv", "Alias: show version",
                           "show version", NULL) == 0);

    /* Add alias: "si eth0" -> "show interfaces eth0" (with argument) */
    check("alias_add si ok",
          cligen_alias_add(h, NULL, "si", "Alias: show interfaces",
                           "show interfaces eth0", NULL) == 0);

    /* Adding duplicate alias should replace (not error) */
    check("alias_add replace ok",
          cligen_alias_add(h, NULL, "sv", "Alias: show version (replaced)",
                           "show version", NULL) == 0);

    /* Invoke the aliased command via cliread_eval */
    check("alias_call via cliread_eval sv",
          cliread_eval(h, &line, &cb_retval, &result, &reason) == 0);

    retval = 0;
 done:
    if (reason)
        free(reason);
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

newtest "alias_add: add new alias sv"
expectpart "$(echo "sv" | LD_LIBRARY_PATH=.. $app "$fspec" 2>&1)" 0 "alias_add sv ok: OK"

newtest "alias_add: add new alias si"
expectpart "$(echo "sv" | LD_LIBRARY_PATH=.. $app "$fspec" 2>&1)" 0 "alias_add si ok: OK"

newtest "alias_add: replace duplicate"
expectpart "$(echo "sv" | LD_LIBRARY_PATH=.. $app "$fspec" 2>&1)" 0 "alias_add replace ok: OK"

newtest "alias: sv executes show version"
expectpart "$(echo "sv" | LD_LIBRARY_PATH=.. $app "$fspec" 2>&1)" 0 "CALLBACK cvv_len="

newtest "alias: sv completion visible"
expectpart "$(printf 's\t' | LD_LIBRARY_PATH=.. $app "$fspec" 2>&1)" 0 ""

newtest "alias: si completion visible"
expectpart "$(printf 's\t' | LD_LIBRARY_PATH=.. $app "$fspec" 2>&1)" 0 ""

newtest "endtest"
endtest

rm -rf $dir
