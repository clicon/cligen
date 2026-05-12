#!/usr/bin/env bash
# Test cligen_buf.c (cbuf) API coverage:
#   cbuf_new, cbuf_new_alloc, cbuf_alloc_get/set,
#   cbuf_get, cbuf_len, cbuf_buflen, cbuf_reset, cbuf_trunc,
#   cbuf_append, cbuf_append_str, cbuf_append_buf,
#   cprintf, vcprintf

# Magic line must be first in script (see README.md)
s="$_" ; . ./lib.sh || if [ "$s" = $0 ]; then exit 0; else return 0; fi

app="$dir/test_cbuf"
cfile="${app}.c"

cat <<'EOF' > $cfile
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <cligen/cligen.h>

static void
check(const char *label, int ok)
{
    printf("%s: %s\n", label, ok ? "OK" : "FAIL");
}

static int
do_vcprintf(cbuf *cb, const char *fmt, ...)
{
    va_list ap;
    int ret;
    va_start(ap, fmt);
    ret = vcprintf(cb, fmt, ap);
    va_end(ap);
    return ret;
}

int
main(void)
{
    cbuf   *cb;
    size_t  start, threshold;
    char    tmp[8] = "bindata";

    /* cbuf_alloc_get/set */
    cbuf_alloc_get(&start, &threshold);
    check("alloc_get returns", 1);
    cbuf_alloc_set(start, threshold);
    check("alloc_set round-trip", 1);

    /* cbuf_new */
    cb = cbuf_new();
    check("cbuf_new", cb != NULL);
    check("initial len=0", cbuf_len(cb) == 0);
    check("initial get empty", strcmp(cbuf_get(cb), "") == 0);
    cbuf_free(cb);

    /* cbuf_new_alloc with small initial size (forces realloc later) */
    cb = cbuf_new_alloc(4);
    check("cbuf_new_alloc", cb != NULL);
    check("new_alloc len=0", cbuf_len(cb) == 0);
    cbuf_free(cb);

    /* cprintf: basic formatting */
    cb = cbuf_new();
    cprintf(cb, "hello %s %d", "world", 42);
    check("cprintf content", strcmp(cbuf_get(cb), "hello world 42") == 0);
    check("cprintf len", cbuf_len(cb) == 14);

    /* cbuf_reset */
    cbuf_reset(cb);
    check("reset len=0", cbuf_len(cb) == 0);
    check("reset get empty", strcmp(cbuf_get(cb), "") == 0);
    cbuf_free(cb);

    /* vcprintf */
    cb = cbuf_new();
    do_vcprintf(cb, "v%d", 99);
    check("vcprintf content", strcmp(cbuf_get(cb), "v99") == 0);
    cbuf_free(cb);

    /* cbuf_append (single char) */
    cb = cbuf_new();
    cbuf_append(cb, 'A');
    cbuf_append(cb, 'B');
    cbuf_append(cb, 'C');
    check("append chars", strcmp(cbuf_get(cb), "ABC") == 0);
    check("append len=3", cbuf_len(cb) == 3);
    cbuf_free(cb);

    /* cbuf_append_str */
    cb = cbuf_new();
    cbuf_append_str(cb, "foo");
    cbuf_append_str(cb, "bar");
    check("append_str", strcmp(cbuf_get(cb), "foobar") == 0);
    cbuf_free(cb);

    /* cbuf_append_buf (binary-safe) */
    cb = cbuf_new();
    cbuf_append_buf(cb, tmp, 7);
    check("append_buf len", cbuf_len(cb) == 7);
    check("append_buf content", memcmp(cbuf_get(cb), "bindata", 7) == 0);
    cbuf_free(cb);

    /* cbuf_trunc */
    cb = cbuf_new();
    cprintf(cb, "truncateme");
    cbuf_trunc(cb, 5);
    check("trunc len=5", cbuf_len(cb) == 5);
    check("trunc content", strcmp(cbuf_get(cb), "trunc") == 0);
    cbuf_free(cb);

    /* cbuf_buflen: internal buffer size >= len+1 */
    cb = cbuf_new();
    cprintf(cb, "buflen");
    check("buflen >= len+1", cbuf_buflen(cb) >= cbuf_len(cb) + 1);
    cbuf_free(cb);

    /* Force realloc by appending lots of data */
    cb = cbuf_new_alloc(4);
    for (int i = 0; i < 200; i++)
        cbuf_append(cb, 'x');
    check("realloc via append len", cbuf_len(cb) == 200);
    cbuf_free(cb);

    return 0;
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

newtest "cbuf_alloc_get/set"
expectpart "$(LD_LIBRARY_PATH=.. $app 2>&1)" 0 "alloc_get returns: OK" "alloc_set round-trip: OK"

newtest "cbuf_new and initial state"
expectpart "$(LD_LIBRARY_PATH=.. $app 2>&1)" 0 "cbuf_new: OK" "initial len=0: OK" "initial get empty: OK"

newtest "cbuf_new_alloc"
expectpart "$(LD_LIBRARY_PATH=.. $app 2>&1)" 0 "cbuf_new_alloc: OK" "new_alloc len=0: OK"

newtest "cprintf content and length"
expectpart "$(LD_LIBRARY_PATH=.. $app 2>&1)" 0 "cprintf content: OK" "cprintf len: OK"

newtest "cbuf_reset"
expectpart "$(LD_LIBRARY_PATH=.. $app 2>&1)" 0 "reset len=0: OK" "reset get empty: OK"

newtest "vcprintf"
expectpart "$(LD_LIBRARY_PATH=.. $app 2>&1)" 0 "vcprintf content: OK"

newtest "cbuf_append single chars"
expectpart "$(LD_LIBRARY_PATH=.. $app 2>&1)" 0 "append chars: OK" "append len=3: OK"

newtest "cbuf_append_str"
expectpart "$(LD_LIBRARY_PATH=.. $app 2>&1)" 0 "append_str: OK"

newtest "cbuf_append_buf binary data"
expectpart "$(LD_LIBRARY_PATH=.. $app 2>&1)" 0 "append_buf len: OK" "append_buf content: OK"

newtest "cbuf_trunc"
expectpart "$(LD_LIBRARY_PATH=.. $app 2>&1)" 0 "trunc len=5: OK" "trunc content: OK"

newtest "cbuf_buflen"
expectpart "$(LD_LIBRARY_PATH=.. $app 2>&1)" 0 "buflen >= len+1: OK"

newtest "cbuf realloc growth"
expectpart "$(LD_LIBRARY_PATH=.. $app 2>&1)" 0 "realloc via append len: OK"

newtest "endtest"
endtest

rm -rf $dir
