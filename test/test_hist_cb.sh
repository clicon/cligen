#!/usr/bin/env bash
# Test expanded history callback (issue #135)
# Tests that abbreviated commands are expanded in the history callback
# XXX Does not work with mem valgrind tests

# Magic line must be first in script (see README.md)
s="$_" ; . ./lib.sh || if [ "$s" = $0 ]; then exit 0; else return 0; fi

app="$dir/test_hist_cb"
cfile=${app}.c
fspec=$dir/spec.cli

# Create CLI specification
cat > $fspec <<EOF
prompt="cli> ";
show("Show commands"){
  interfaces("Display interfaces") <ifname:string>("Interface name"), callback();
  version("Display version"), callback();
}
ping("Send ping") <host:ipv4addr>("Target host"), callback();
configure("Enter configuration mode"){
  terminal("Terminal configuration"), callback();
}
EOF

# Create C test program with expanded history callback
cat<<EOF > $cfile
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cligen/cligen.h>

int
callback(cligen_handle h,
         cvec         *cvv,
         cvec         *argv)
{
    return 0;
}

int
history_expanded_cb(cligen_handle h,
                    const char   *cmd,
                    const char   *cmd_expanded,
                    void         *arg)
{
    printf("RAW:%s|EXPANDED:%s\n", cmd, cmd_expanded);
    fflush(stdout);
    return 0;
}

cgv_fnstype_t *
str2fn(const char *name,
       void       *arg,
       char      **error)
{
    if (strcmp(name, "callback") == 0)
        return (cgv_fnstype_t *)callback;
    return NULL;
}

int
main(int   argc,
     char *argv[])
{
    int            retval = -1;
    cligen_handle  h;
    FILE          *f;
    parse_tree   *pt;
    char         *line = NULL;
    int           cb_retval;
    cligen_result result;
    char         *reason = NULL;

    if ((h = cligen_init()) == NULL)
        goto done;
    if ((f = fopen("$fspec", "r")) == NULL)
        goto done;
    if (clispec_parse_file(h, f, "test", NULL, NULL, NULL) < 0) {
        fclose(f);
        goto done;
    }
    fclose(f);
    pt = cligen_pt_active_get(h);
    if (cligen_callbackv_str2fn(pt, str2fn, NULL) < 0)
        goto done;
    cligen_hist_fn_set(h, history_expanded_cb, NULL);
    if (cliread_eval(h, &line, &cb_retval, &result, &reason) < 0)
        goto done;
    retval = 0;
done:
    if (h)
        cligen_exit(h);
    return retval;
}
EOF

# Compile the test program - use the newly built library in parent directory
newtest "Compile test program"
if [ "$LINKAGE" = static ]; then
    COMPILE="$CC -DHAVE_CONFIG_H -g -Wall $CFLAGS -I.. $cfile ../libcligen.a -o $app"
else
    # Link directly with .so file to pick up new symbols
    COMPILE="$CC -DHAVE_CONFIG_H -g -Wall $CFLAGS -I.. $cfile ../libcligen.so.7.6 -o $app"
fi
expectpart "$($COMPILE 2>&1)" 0 ""

# Test 1: Abbreviated "show interfaces" -> "sh int eth0"
newtest "Abbreviated: sh int eth0"
expectpart "$(echo "sh int eth0" | LD_LIBRARY_PATH=.. $app 2>&1)" 0 "RAW:sh int eth0|EXPANDED:show interfaces eth0"

# Test 2: Abbreviated "ping" -> "pin 192.168.1.1"
newtest "Abbreviated: pin 192.168.1.1"
expectpart "$(echo "pin 192.168.1.1" | LD_LIBRARY_PATH=.. $app 2>&1)" 0 "RAW:pin 192.168.1.1|EXPANDED:ping 192.168.1.1"

# Test 3: Abbreviated nested command -> "conf term"
newtest "Abbreviated: conf term"
expectpart "$(echo "conf term" | LD_LIBRARY_PATH=.. $app 2>&1)" 0 "RAW:conf term|EXPANDED:configure terminal"

# Test 4: Full command (no abbreviation) -> "show version"
newtest "Full command: show version"
expectpart "$(echo "show version" | LD_LIBRARY_PATH=.. $app 2>&1)" 0 "RAW:show version|EXPANDED:show version"

# Test 5: Partial multi-level abbreviation -> "sh ver"
newtest "Partial abbreviation: sh ver"
expectpart "$(echo "sh ver" | LD_LIBRARY_PATH=.. $app 2>&1)" 0 "RAW:sh ver|EXPANDED:show version"

newtest "endtest"
endtest

rm -rf $dir
