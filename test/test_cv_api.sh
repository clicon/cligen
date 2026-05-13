#!/usr/bin/env bash
# Comprehensive test of cligen_cv.c API:
#   cv_new, cv_free, cv_reset, cv_cp, cv_dup,
#   cv_parse1/cv_parse for all types,
#   cv_cmp for all types,
#   cv_print, cv2cbuf, cv2str,
#   cv_max_set, cv_min_set, cvtype_max2str, cvtype_max2str_dup,
#   cv_str2type, cv_type2str, cv_size, cv_len,
#   cv_flag/cv_flag_set/cv_flag_clr, cv_const_get/set,
#   cligen_escape_need, cligen_escape_do

# Magic line must be first in script (see README.md)
s="$_" ; . ./lib.sh || if [ "$s" = $0 ]; then exit 0; else return 0; fi

app="$dir/test_cv_api"
cfile="${app}.c"

cat <<'EOF' > $cfile
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <netinet/in.h>
#include <arpa/inet.h>
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

int
main(void)
{
    cg_var    *cv  = NULL;
    cg_var    *cv2 = NULL;
    cbuf      *cb  = NULL;
    char      *reason = NULL;
    char       str[256];
    char      *s;
    int        ret;

    /* cv_new / cv_free */
    cv = cv_new(CGV_INT32);
    check("cv_new not NULL", cv != NULL);
    check("cv_new type correct", cv_type_get(cv) == CGV_INT32);
    cv_free(cv); cv = NULL;

    /* cv_reset lifecycle: allocate string type, set, reset, reuse */
    cv = cv_new(CGV_STRING);
    cv_string_set(cv, "hello");
    check("cv_string before reset", strcmp(cv_string_get(cv), "hello") == 0);
    cv_reset(cv);
    cv_type_set(cv, CGV_INT32);
    cv_int32_set(cv, 99);
    check("cv_reset then reuse", cv_int32_get(cv) == 99);
    cv_free(cv); cv = NULL;

    /* cv_str2type / cv_type2str roundtrip */
    check("str2type int8",   cv_str2type("int8")   == CGV_INT8);
    check("str2type int32",  cv_str2type("int32")  == CGV_INT32);
    check("str2type string", cv_str2type("string") == CGV_STRING);
    check("str2type ipv4addr", cv_str2type("ipv4addr") == CGV_IPV4ADDR);
    check("str2type url",    cv_str2type("url")    == CGV_URL);
    check("str2type uuid",   cv_str2type("uuid")   == CGV_UUID);
    check("str2type unknown returns ERR", cv_str2type("nosuchtype") == CGV_ERR);
    check("type2str int32",  strcmp(cv_type2str(CGV_INT32),  "int32")  == 0);
    check("type2str string", strcmp(cv_type2str(CGV_STRING), "string") == 0);
    check("type2str url",    strcmp(cv_type2str(CGV_URL),    "url")    == 0);

    /* cv_parse1: integers */
    cv = cv_new(CGV_INT8);
    reason = NULL;
    ret = cv_parse1("42", cv, &reason);
    check("parse int8 ok", ret == 1);
    check("parse int8 value", cv_int8_get(cv) == 42);
    cv_free(cv); cv = NULL;

    cv = cv_new(CGV_INT8);
    reason = NULL;
    ret = cv_parse1("999", cv, &reason);
    check("parse int8 overflow fails", ret == 0);
    if (reason) { free(reason); reason = NULL; }
    cv_free(cv); cv = NULL;

    cv = cv_new(CGV_INT16);
    reason = NULL;
    ret = cv_parse1("-1000", cv, &reason);
    check("parse int16 negative ok", ret == 1);
    check("parse int16 value", cv_int16_get(cv) == -1000);
    cv_free(cv); cv = NULL;

    cv = cv_new(CGV_INT32);
    reason = NULL;
    ret = cv_parse1("2147483647", cv, &reason);
    check("parse int32 max ok", ret == 1);
    check("parse int32 max value", cv_int32_get(cv) == INT32_MAX);
    cv_free(cv); cv = NULL;

    cv = cv_new(CGV_INT64);
    reason = NULL;
    ret = cv_parse1("-9223372036854775808", cv, &reason);
    check("parse int64 min ok", ret == 1);
    cv_free(cv); cv = NULL;

    cv = cv_new(CGV_UINT8);
    reason = NULL;
    ret = cv_parse1("255", cv, &reason);
    check("parse uint8 max ok", ret == 1);
    check("parse uint8 value", cv_uint8_get(cv) == 255);
    cv_free(cv); cv = NULL;

    cv = cv_new(CGV_UINT64);
    reason = NULL;
    ret = cv_parse1("18446744073709551615", cv, &reason);
    check("parse uint64 max ok", ret == 1);
    cv_free(cv); cv = NULL;

    /* cv_parse1: bool */
    cv = cv_new(CGV_BOOL);
    reason = NULL;
    ret = cv_parse1("true", cv, &reason);
    check("parse bool true ok", ret == 1);
    check("parse bool true value", cv_bool_get(cv) == 1);
    cv_free(cv); cv = NULL;

    cv = cv_new(CGV_BOOL);
    reason = NULL;
    ret = cv_parse1("false", cv, &reason);
    check("parse bool false ok", ret == 1);
    check("parse bool false value", cv_bool_get(cv) == 0);
    cv_free(cv); cv = NULL;

    cv = cv_new(CGV_BOOL);
    reason = NULL;
    ret = cv_parse1("notabool", cv, &reason);
    check("parse bool invalid fails", ret == 0);
    if (reason) { free(reason); reason = NULL; }
    cv_free(cv); cv = NULL;

    /* cv_parse1: string */
    cv = cv_new(CGV_STRING);
    reason = NULL;
    ret = cv_parse1("hello world", cv, &reason);
    check("parse string ok", ret == 1);
    check("parse string value", strcmp(cv_string_get(cv), "hello world") == 0);
    cv_free(cv); cv = NULL;

    /* cv_parse1: ipv4addr */
    cv = cv_new(CGV_IPV4ADDR);
    reason = NULL;
    ret = cv_parse1("192.168.1.1", cv, &reason);
    check("parse ipv4addr ok", ret == 1);
    cv_free(cv); cv = NULL;

    cv = cv_new(CGV_IPV4ADDR);
    reason = NULL;
    ret = cv_parse1("999.0.0.1", cv, &reason);
    check("parse ipv4addr invalid fails", ret <= 0);
    if (reason) { free(reason); reason = NULL; }
    cv_free(cv); cv = NULL;

    /* cv_parse1: ipv4prefix */
    cv = cv_new(CGV_IPV4PFX);
    reason = NULL;
    ret = cv_parse1("10.0.0.0/8", cv, &reason);
    check("parse ipv4prefix ok", ret == 1);
    check("parse ipv4prefix masklen", cv_ipv4masklen_get(cv) == 8);
    cv_free(cv); cv = NULL;

    /* cv_parse1: ipv6addr */
    cv = cv_new(CGV_IPV6ADDR);
    reason = NULL;
    ret = cv_parse1("::1", cv, &reason);
    check("parse ipv6addr ok", ret == 1);
    cv_free(cv); cv = NULL;

    /* cv_parse1: ipv6prefix */
    cv = cv_new(CGV_IPV6PFX);
    reason = NULL;
    ret = cv_parse1("2001:db8::/32", cv, &reason);
    check("parse ipv6prefix ok", ret == 1);
    cv_free(cv); cv = NULL;

    /* cv_parse1: macaddr */
    cv = cv_new(CGV_MACADDR);
    reason = NULL;
    ret = cv_parse1("aa:bb:cc:dd:ee:ff", cv, &reason);
    check("parse macaddr ok", ret == 1);
    cv_free(cv); cv = NULL;

    /* cv_parse1: decimal64 */
    cv = cv_new(CGV_DEC64);
    cv_dec64_n_set(cv, 4);
    reason = NULL;
    ret = cv_parse1("3.1415", cv, &reason);
    check("parse dec64 ok", ret == 1);
    cv_free(cv); cv = NULL;

    /* cv_parse1: URL */
    cv = cv_new(CGV_URL);
    reason = NULL;
    ret = cv_parse1("http://user:pass@host.com/path", cv, &reason);
    check("parse url ok", ret == 1);
    check("parse url proto", strcmp(cv_urlproto_get(cv), "http") == 0);
    check("parse url user", strcmp(cv_urluser_get(cv), "user") == 0);
    check("parse url passwd", strcmp(cv_urlpasswd_get(cv), "pass") == 0);
    check("parse url addr", strcmp(cv_urladdr_get(cv), "host.com") == 0);
    check("parse url path", strcmp(cv_urlpath_get(cv), "path") == 0);
    cv_free(cv); cv = NULL;

    /* URL with no user/passwd */
    cv = cv_new(CGV_URL);
    reason = NULL;
    ret = cv_parse1("ftp://ftp.example.org/pub", cv, &reason);
    check("parse url no user ok", ret == 1);
    check("parse url no user empty", strcmp(cv_urluser_get(cv), "") == 0);
    cv_free(cv); cv = NULL;

    /* URL invalid */
    cv = cv_new(CGV_URL);
    reason = NULL;
    ret = cv_parse1("notaurl", cv, &reason);
    check("parse url invalid fails", ret == 0);
    if (reason) { free(reason); reason = NULL; }
    cv_free(cv); cv = NULL;

    /* cv_parse1: uuid */
    cv = cv_new(CGV_UUID);
    reason = NULL;
    ret = cv_parse1("550e8400-e29b-41d4-a716-446655440000", cv, &reason);
    check("parse uuid ok", ret == 1);
    cv_free(cv); cv = NULL;

    cv = cv_new(CGV_UUID);
    reason = NULL;
    ret = cv_parse1("not-a-uuid", cv, &reason);
    check("parse uuid invalid fails", ret == 0);
    if (reason) { free(reason); reason = NULL; }
    cv_free(cv); cv = NULL;

    /* cv_parse1: time */
    cv = cv_new(CGV_TIME);
    reason = NULL;
    ret = cv_parse1("2008-09-21T18:57:21.003456Z", cv, &reason);
    check("parse time ok", ret == 1);
    cv_free(cv); cv = NULL;

    cv = cv_new(CGV_TIME);
    reason = NULL;
    ret = cv_parse1("notadate", cv, &reason);
    check("parse time invalid fails", ret == 0);
    if (reason) { free(reason); reason = NULL; }
    cv_free(cv); cv = NULL;

    /* cv_cp and cv_dup */
    cv = cv_new(CGV_INT32);
    cv_int32_set(cv, 12345);
    cv_name_set(cv, "myvar");

    cv2 = cv_new(CGV_INT32);
    ret = cv_cp(cv2, cv);
    check("cv_cp ok", ret == 0);
    check("cv_cp value", cv_int32_get(cv2) == 12345);
    check("cv_cp name", strcmp(cv_name_get(cv2), "myvar") == 0);
    cv_free(cv2); cv2 = NULL;

    cv2 = cv_dup(cv);
    check("cv_dup not NULL", cv2 != NULL);
    check("cv_dup value", cv_int32_get(cv2) == 12345);
    check("cv_dup name", strcmp(cv_name_get(cv2), "myvar") == 0);
    cv_free(cv2); cv2 = NULL;
    cv_free(cv); cv = NULL;

    /* cv_dup with string type */
    cv = cv_new(CGV_STRING);
    cv_string_set(cv, "dupme");
    cv2 = cv_dup(cv);
    check("cv_dup string not NULL", cv2 != NULL);
    check("cv_dup string value", strcmp(cv_string_get(cv2), "dupme") == 0);
    cv_free(cv2); cv2 = NULL;
    cv_free(cv); cv = NULL;

    /* cv_cmp: integers */
    cv  = cv_new(CGV_INT32); cv_int32_set(cv,  10);
    cv2 = cv_new(CGV_INT32); cv_int32_set(cv2, 20);
    check("cv_cmp int32 less",    cv_cmp(cv, cv2) < 0);
    check("cv_cmp int32 greater", cv_cmp(cv2, cv) > 0);
    cv_int32_set(cv2, 10);
    check("cv_cmp int32 equal",   cv_cmp(cv, cv2) == 0);
    cv_free(cv); cv_free(cv2); cv = cv2 = NULL;

    /* cv_cmp: strings */
    cv  = cv_new(CGV_STRING); cv_string_set(cv,  "aaa");
    cv2 = cv_new(CGV_STRING); cv_string_set(cv2, "bbb");
    check("cv_cmp string less",  cv_cmp(cv, cv2) < 0);
    check("cv_cmp string equal", cv_cmp(cv, cv) == 0);
    cv_free(cv); cv_free(cv2); cv = cv2 = NULL;

    /* cv_cmp: bool */
    cv  = cv_new(CGV_BOOL); cv_bool_set(cv,  0);
    cv2 = cv_new(CGV_BOOL); cv_bool_set(cv2, 1);
    check("cv_cmp bool", cv_cmp(cv, cv2) < 0);
    cv_free(cv); cv_free(cv2); cv = cv2 = NULL;

    /* cv_cmp: different types */
    cv  = cv_new(CGV_INT32);
    cv2 = cv_new(CGV_STRING);
    check("cv_cmp different types nonzero", cv_cmp(cv, cv2) != 0);
    cv_free(cv); cv_free(cv2); cv = cv2 = NULL;

    /* cv2cbuf: produce string representation */
    cb = cbuf_new();
    cv = cv_new(CGV_INT32); cv_int32_set(cv, 42);
    ret = cv2cbuf(cv, cb);
    check("cv2cbuf int32 ok", ret >= 0);
    check("cv2cbuf int32 content", strcmp(cbuf_get(cb), "42") == 0);
    cv_free(cv); cv = NULL;
    cbuf_free(cb); cb = NULL;

    cb = cbuf_new();
    cv = cv_new(CGV_BOOL); cv_bool_set(cv, 1);
    cv2cbuf(cv, cb);
    check("cv2cbuf bool true", strcmp(cbuf_get(cb), "true") == 0);
    cv_free(cv); cv = NULL;
    cbuf_free(cb); cb = NULL;

    cb = cbuf_new();
    cv = cv_new(CGV_STRING); cv_string_set(cv, "hello");
    cv2cbuf(cv, cb);
    check("cv2cbuf string", strstr(cbuf_get(cb), "hello") != NULL);
    cv_free(cv); cv = NULL;
    cbuf_free(cb); cb = NULL;

    /* cv_print: just verify it doesn't crash */
    cv = cv_new(CGV_INT32); cv_int32_set(cv, 99);
    ret = cv_print(stdout, cv);
    printf("\n");
    check("cv_print int32 ok", ret >= 0);
    cv_free(cv); cv = NULL;

    cv = cv_new(CGV_URL);
    reason = NULL;
    cv_parse1("http://a:b@c.com/d", cv, &reason);
    ret = cv_print(stdout, cv);
    printf("\n");
    check("cv_print url ok", ret >= 0);
    cv_free(cv); cv = NULL;

    /* cv_max_set / cv_min_set */
    cv = cv_new(CGV_INT8);
    check("cv_max_set int8 ok", cv_max_set(cv) == 0);
    check("cv_max_set int8 val", cv_int8_get(cv) == INT8_MAX);
    check("cv_min_set int8 ok", cv_min_set(cv) == 0);
    check("cv_min_set int8 val", cv_int8_get(cv) == INT8_MIN);
    cv_free(cv); cv = NULL;

    cv = cv_new(CGV_UINT32);
    check("cv_max_set uint32 ok", cv_max_set(cv) == 0);
    check("cv_max_set uint32 val", cv_uint32_get(cv) == UINT32_MAX);
    check("cv_min_set uint32 ok", cv_min_set(cv) == 0);
    check("cv_min_set uint32 val", cv_uint32_get(cv) == 0);
    cv_free(cv); cv = NULL;

    /* cvtype_max2str: returns length like snprintf (>0 on success) */
    ret = cvtype_max2str(CGV_INT8, str, sizeof(str));
    check("cvtype_max2str int8 ok", ret > 0);
    check("cvtype_max2str int8 val", strcmp(str, "127") == 0);

    ret = cvtype_max2str(CGV_UINT8, str, sizeof(str));
    check("cvtype_max2str uint8 ok", ret > 0);
    check("cvtype_max2str uint8 val", strcmp(str, "255") == 0);

    s = cvtype_max2str_dup(CGV_INT32);
    check("cvtype_max2str_dup int32 not NULL", s != NULL);
    check("cvtype_max2str_dup int32 val", strcmp(s, "2147483647") == 0);
    free(s);

    /* cv_size */
    cv = cv_new(CGV_INT32);
    check("cv_size int32 > 0", cv_size(cv) > 0);
    cv_free(cv); cv = NULL;

    cv = cv_new(CGV_STRING);
    cv_string_set(cv, "test");
    check("cv_size string > 0", cv_size(cv) > 0);
    cv_free(cv); cv = NULL;

    /* cv_flag, cv_flag_set, cv_flag_clr */
    cv = cv_new(CGV_INT32);
    check("cv_flag initial 0", cv_flag(cv, 0xFF) == 0);
    cv_flag_set(cv, 0x01);
    check("cv_flag after set", cv_flag(cv, 0x01) != 0);
    cv_flag_clr(cv, 0x01);
    check("cv_flag after clr", cv_flag(cv, 0x01) == 0);
    cv_free(cv); cv = NULL;

    /* cv_const_get / cv_const_set */
    cv = cv_new(CGV_INT32);
    check("cv_const initial false", cv_const_get(cv) == 0);
    cv_const_set(cv, 1);
    check("cv_const after set", cv_const_get(cv) == 1);
    cv_const_set(cv, 0);
    check("cv_const after clr", cv_const_get(cv) == 0);
    cv_free(cv); cv = NULL;

    /* cligen_escape_need / cligen_escape_do */
    check("escape_need plain", cligen_escape_need("plain") == 0);
    check("escape_need space", cligen_escape_need("has space") == 1);
    check("escape_need question", cligen_escape_need("has?q") == 1);
    check("escape_need backslash", cligen_escape_need("back\\slash") == 1);
    check("escape_need quoted", cligen_escape_need("\"already quoted\"") == 0);

    s = cligen_escape_do("has space");
    check("escape_do not NULL", s != NULL);
    check("escape_do has quotes", s[0] == '"' && s[strlen(s)-1] == '"');
    free(s);

    s = cligen_escape_do("has?question");
    check("escape_do question escaped", strstr(s, "\\?") != NULL);
    free(s);

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

newtest "cv_new / cv_free / cv_reset"
expectpart "$OUT" 0 "OK: cv_new not NULL" "OK: cv_new type correct" "OK: cv_string before reset" "OK: cv_reset then reuse"

newtest "cv_str2type / cv_type2str"
expectpart "$OUT" 0 "OK: str2type int8" "OK: str2type string" "OK: str2type url" "OK: str2type unknown returns ERR" "OK: type2str int32" "OK: type2str string"

newtest "cv_parse1 integers"
expectpart "$OUT" 0 "OK: parse int8 ok" "OK: parse int8 value" "OK: parse int8 overflow fails" "OK: parse int16 negative ok" "OK: parse int32 max ok" "OK: parse int64 min ok" "OK: parse uint8 max ok" "OK: parse uint64 max ok"

newtest "cv_parse1 bool"
expectpart "$OUT" 0 "OK: parse bool true ok" "OK: parse bool true value" "OK: parse bool false ok" "OK: parse bool false value" "OK: parse bool invalid fails"

newtest "cv_parse1 string"
expectpart "$OUT" 0 "OK: parse string ok" "OK: parse string value"

newtest "cv_parse1 network types"
expectpart "$OUT" 0 "OK: parse ipv4addr ok" "OK: parse ipv4addr invalid fails" "OK: parse ipv4prefix ok" "OK: parse ipv4prefix masklen" "OK: parse ipv6addr ok" "OK: parse ipv6prefix ok" "OK: parse macaddr ok"

newtest "cv_parse1 dec64"
expectpart "$OUT" 0 "OK: parse dec64 ok"

newtest "cv_parse1 URL - valid forms"
expectpart "$OUT" 0 "OK: parse url ok" "OK: parse url proto" "OK: parse url user" "OK: parse url passwd" "OK: parse url addr" "OK: parse url path" "OK: parse url no user ok" "OK: parse url no user empty"

newtest "cv_parse1 URL - invalid"
expectpart "$OUT" 0 "OK: parse url invalid fails"

newtest "cv_parse1 uuid"
expectpart "$OUT" 0 "OK: parse uuid ok" "OK: parse uuid invalid fails"

newtest "cv_parse1 time"
expectpart "$OUT" 0 "OK: parse time ok" "OK: parse time invalid fails"

newtest "cv_cp and cv_dup"
expectpart "$OUT" 0 "OK: cv_cp ok" "OK: cv_cp value" "OK: cv_cp name" "OK: cv_dup not NULL" "OK: cv_dup value" "OK: cv_dup name" "OK: cv_dup string not NULL" "OK: cv_dup string value"

newtest "cv_cmp integers"
expectpart "$OUT" 0 "OK: cv_cmp int32 less" "OK: cv_cmp int32 greater" "OK: cv_cmp int32 equal"

newtest "cv_cmp strings and bool"
expectpart "$OUT" 0 "OK: cv_cmp string less" "OK: cv_cmp string equal" "OK: cv_cmp bool" "OK: cv_cmp different types nonzero"

newtest "cv2cbuf"
expectpart "$OUT" 0 "OK: cv2cbuf int32 ok" "OK: cv2cbuf int32 content" "OK: cv2cbuf bool true" "OK: cv2cbuf string"

newtest "cv_print"
expectpart "$OUT" 0 "OK: cv_print int32 ok" "OK: cv_print url ok"

newtest "cv_max_set / cv_min_set"
expectpart "$OUT" 0 "OK: cv_max_set int8 ok" "OK: cv_max_set int8 val" "OK: cv_min_set int8 ok" "OK: cv_min_set int8 val" "OK: cv_max_set uint32 ok" "OK: cv_min_set uint32 val"

newtest "cvtype_max2str / cvtype_max2str_dup"
expectpart "$OUT" 0 "OK: cvtype_max2str int8 ok" "OK: cvtype_max2str int8 val" "OK: cvtype_max2str uint8 val" "OK: cvtype_max2str_dup int32 not NULL" "OK: cvtype_max2str_dup int32 val"

newtest "cv_size"
expectpart "$OUT" 0 "OK: cv_size int32 > 0" "OK: cv_size string > 0"

newtest "cv_flag / cv_const"
expectpart "$OUT" 0 "OK: cv_flag initial 0" "OK: cv_flag after set" "OK: cv_flag after clr" "OK: cv_const initial false" "OK: cv_const after set" "OK: cv_const after clr"

newtest "cligen_escape_need / cligen_escape_do"
expectpart "$OUT" 0 "OK: escape_need plain" "OK: escape_need space" "OK: escape_need question" "OK: escape_need quoted" "OK: escape_do not NULL" "OK: escape_do has quotes" "OK: escape_do question escaped"

newtest "no failures"
expectpart "$OUT" 0 "Total errors: 0"

newtest "endtest"
endtest

rm -rf $dir
