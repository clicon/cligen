#!/usr/bin/env bash
# Compile a simple application and go through types
# Partly a test for developing
# Partly a coverage test of all set/get functions (with no actual testof semantics)
# XXX needs coverage flags

# Magic line must be first in script (see README.md)
s="$_" ; . ./lib.sh || if [ "$s" = $0 ]; then exit 0; else return 0; fi

app=$dir/example-api
cfile=${app}.c

cat<<EOF > $cfile
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <cligen/cligen.h>

int
main(int   argc,
     char *argv[])
{
    int           retval = -1;
    cligen_handle h;
    cvec         *cvv = NULL;   
    cg_var       *cv;
    enum cv_type  type;
    
    if ((h = cligen_init()) == NULL)
        goto done;
    if ((cvv = cvec_new(0)) == NULL)
        goto done;
    for (type=CGV_ERR; type<=CGV_EMPTY; type++){
        if ((cv = cvec_add(cvv, type)) == NULL)
            goto done;
        switch(type){
        case CGV_ERR:
            break;
        case CGV_INT8:
            cv_int8_set(cv, 99);
            cv_int8_get(cv);
            break;
        case CGV_INT16:
            cv_int16_set(cv, 9999);
            cv_int16_get(cv);
            break;
        case CGV_INT32:
            cv_int32_set(cv, 999999);
            cv_int32_get(cv);
            break;
        case CGV_INT64:
            cv_int64_set(cv, 999999000);
            cv_int64_get(cv);
            break;
        case CGV_UINT8:
            cv_uint8_set(cv, 99);
            cv_uint8_get(cv);
            break;
        case CGV_UINT16:
            cv_uint16_set(cv, 9999);
            cv_uint16_get(cv);
            break;
        case CGV_UINT32:
            cv_uint32_set(cv, 999999);
            cv_uint32_get(cv);
            break;
        case CGV_UINT64:
            cv_uint64_set(cv, 9999990000);
            cv_uint64_get(cv);
            break;
        case CGV_DEC64:
            cv_dec64_n_set(cv, 12);
            cv_dec64_n_get(cv);
            cv_dec64_i_set(cv, 123748327482);
            cv_dec64_i_get(cv);
            break;
        case CGV_BOOL:
            cv_bool_set(cv, 1);
            cv_bool_get(cv);
            break;
        case CGV_STRING:
        case CGV_REST:
            cv_string_set_direct(cv, strdup("foobar"));
            cv_string_get(cv);
            break;
        case CGV_INTERFACE:
            cv_string_set(cv, "eth/0/1");
            cv_string_get(cv);
            break;
        case CGV_IPV4ADDR:
        case CGV_IPV4PFX:{
            struct in_addr val;
            if (inet_pton(AF_INET, "1.2.3.4", &val) < 0)
                goto done;
            cv_ipv4addr_set(cv, &val);
            cv_ipv4masklen_set(cv, 24);
            cv_ipv4addr_get(cv);
            cv_ipv4masklen_get(cv);
            break;
        }
        case CGV_IPV6ADDR:
        case CGV_IPV6PFX:{
            cv_ipv6addr_get(cv);
            cv_ipv6masklen_get(cv);
            break;
        }
        case CGV_MACADDR:
            cv_mac_get(cv);
            break;
        case CGV_URL:
            cv_urlproto_set(cv, "http");
            cv_urladdr_set(cv, "1.2.3.4");
            cv_urlpath_set(cv, "/foo/bar");
            cv_urluser_set(cv, "root");
            cv_urlpasswd_set(cv, "foobar");
            cv_urlproto_get(cv);
            cv_urladdr_get(cv);
            cv_urlpath_get(cv);
            cv_urluser_get(cv);
            cv_urlpasswd_get(cv);
            break;
        case CGV_UUID:{
            uuid_t u;
            str2uuid("a0a2feb6-4807-4b41-bb18-beefc5ef6081", u);
            cv_uuid_set(cv, u);
            cv_uuid_get(cv);
            break;
        }
        case CGV_TIME:{
            struct timeval tv;
            str2time("2008-09-21T18:57:21.003456Z", &tv);
            cv_time_set(cv, tv);
            cv_time_get(cv);
            break;
        }
        case CGV_VOID:
            cv_void_set(cv, NULL);
            cv_void_get(cv);
        case CGV_EMPTY:
        default:
            break;
        }
    }
    cvec_print(stdout, cvv);
    retval = 0;
 done:
    if (cvv)
        cvec_free(cvv);
    if (h)
        cligen_exit(h);
    return retval;
}
EOF

newtest "compile $cfile"
if [ "$LINKAGE" = static ]; then
    COMPILE="$CC -DHAVE_CONFIG_H -g -Wall $CFLAGS -I /usr/local/include $cfile ../libcligen.a -o $app"
else
    COMPILE="$CC -DHAVE_CONFIG_H -g -Wall $CFLAGS -I /usr/local/include $cfile -L/usr/local/lib -lcligen -o $app"
fi
echo "COMPILE:$COMPILE"
expectpart "$($COMPILE)" 0 ""

newtest "run $app"
expectpart "$($app)" 0 "1 : 99" "2 : 9999" "3 : 999999" "4 : 999999000" "5 : 99" "6 : 9999" "7 : 999999" "8 : 9999990000" "9 : 0.123748327482" "10 : true" "11 : foobar" "12 : \"foobar\"" "13 : \"eth/0/1\"" "14 : 1.2.3.4" "15 : 1.2.3.4/24" "16 : ::" "17 : ::/0" "18 : 00:00:00:00:00:00" "19 : http://root:foobar@1.2.3.4//foo/bar" "20 : a0a2feb6-4807-4b41-bb18-beefc5ef6081" "21 : 2008-09-21T18:57:21.003456Z"

newtest "endtest"
endtest

rm -rf $dir

