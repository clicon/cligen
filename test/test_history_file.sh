#!/usr/bin/env bash
# Test cligen_history.c file API:
#   cligen_hist_init, cligen_hist_file_load, cligen_hist_file_save, save/load roundtrip

# Magic line must be first in script (see README.md)
s="$_" ; . ./lib.sh || if [ "$s" = $0 ]; then exit 0; else return 0; fi

app="$dir/test_history_file"
cfile="${app}.c"
histfile="$dir/history.txt"

cat <<'EOF' > $cfile
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

int
main(int argc, char *argv[])
{
    int            retval = -1;
    cligen_handle  h;
    FILE          *f;
    const char    *histpath = argc > 1 ? argv[1] : "/tmp/cligen_hist_test.txt";
    const char    *histpath2 = argc > 2 ? argv[2] : "/tmp/cligen_hist_test2.txt";

    if ((h = cligen_init()) == NULL)
        goto done;

    /* Test hist_init with invalid argument */
    check("hist_init lines=0 fails", cligen_hist_init(h, 0) < 0);

    /* Test hist_init with valid size */
    check("hist_init lines=10 ok", cligen_hist_init(h, 10) == 0);

    /* Load history from file */
    if ((f = fopen(histpath, "r")) == NULL){
        perror("fopen load");
        goto done;
    }
    check("hist_file_load ok", cligen_hist_file_load(h, f) == 0);
    fclose(f);

    /* Save history to second file */
    if ((f = fopen(histpath2, "w")) == NULL){
        perror("fopen save");
        goto done;
    }
    check("hist_file_save ok", cligen_hist_file_save(h, f) == 0);
    fclose(f);

    /* Re-init and reload saved file to verify roundtrip */
    check("hist_init reinit ok", cligen_hist_init(h, 10) == 0);
    if ((f = fopen(histpath2, "r")) == NULL){
        perror("fopen reload");
        goto done;
    }
    check("hist_roundtrip_load ok", cligen_hist_file_load(h, f) == 0);
    fclose(f);

    /* Resize history: reinit with different size */
    check("hist_init resize ok", cligen_hist_init(h, 5) == 0);

    retval = 0;
 done:
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

# Create a history file with some entries
cat > "$histfile" <<'HIST'
show interfaces
show version
configure terminal
ping 1.2.3.4
HIST

newtest "hist_init invalid size rejected"
expectpart "$(LD_LIBRARY_PATH=.. $app "$histfile" "$dir/hist_saved.txt" 2>&1)" 0 "hist_init lines=0 fails: OK"

newtest "hist_init valid size"
expectpart "$(LD_LIBRARY_PATH=.. $app "$histfile" "$dir/hist_saved.txt" 2>&1)" 0 "hist_init lines=10 ok: OK"

newtest "hist_file_load"
expectpart "$(LD_LIBRARY_PATH=.. $app "$histfile" "$dir/hist_saved.txt" 2>&1)" 0 "hist_file_load ok: OK"

newtest "hist_file_save"
expectpart "$(LD_LIBRARY_PATH=.. $app "$histfile" "$dir/hist_saved.txt" 2>&1)" 0 "hist_file_save ok: OK"

newtest "hist roundtrip: saved file is non-empty"
LD_LIBRARY_PATH=.. $app "$histfile" "$dir/hist_saved.txt" >/dev/null 2>&1
expectpart "$(cat "$dir/hist_saved.txt" 2>/dev/null)" 0 "show interfaces" "ping 1.2.3.4"

newtest "hist roundtrip: load saved file"
expectpart "$(LD_LIBRARY_PATH=.. $app "$histfile" "$dir/hist_saved.txt" 2>&1)" 0 "hist_roundtrip_load ok: OK"

newtest "hist_init resize"
expectpart "$(LD_LIBRARY_PATH=.. $app "$histfile" "$dir/hist_saved.txt" 2>&1)" 0 "hist_init resize ok: OK"

# Empty history file edge case
touch "$dir/hist_empty.txt"
newtest "hist_file_load empty file"
expectpart "$(LD_LIBRARY_PATH=.. $app "$dir/hist_empty.txt" "$dir/hist_saved2.txt" 2>&1)" 0 "hist_file_load ok: OK"

newtest "endtest"
endtest

rm -rf $dir
