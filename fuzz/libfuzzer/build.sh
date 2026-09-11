#!/usr/bin/env bash
#
# Build the CLIgen libFuzzer targets with AddressSanitizer + UndefinedBehavior.
#
# Requirements: clang with libFuzzer (clang >= 6). The generated parser files
# (cligen_parse.tab.c, lex.cligen_parse.c) must already exist in the repo root;
# run `./configure && make` once in the repo root if they do not.
#
# Usage:
#   cd fuzz
#   ./build.sh                 # build all targets
#   ./build.sh fuzz_spec       # build a single target
#
# Run, e.g.:
#   ./fuzz_spec   corpus_spec   -dict=cligen.dict
#   ./fuzz_match  corpus_match
#   ./fuzz_cv     corpus_cv
#   ./fuzz_system corpus_system -dict=tutorial.dict
#
set -euo pipefail

CC=${CC:-clang}
ROOT=$(cd "$(dirname "$0")/../.." && pwd)
FUZZDIR=$(cd "$(dirname "$0")" && pwd)

# Sanitizer + coverage flags shared by library objects and harnesses.
#
# Note: -fno-sanitize=object-size disables UBSan's object-size check. CLIgen
# deliberately under-allocates non-CO_VARIABLE objects to sizeof(struct
# cg_obj_common) (see co_size() in cligen_object.c) and accesses them through a
# cg_obj* pointer. The accessed fields live in the common prefix so this is safe
# in practice, but object-size flags every such access and would drown out real
# bugs. Remove this flag if you specifically want to audit that pattern.
SAN="-fsanitize=fuzzer-no-link,address,undefined -fno-sanitize=object-size -fno-omit-frame-pointer"
CFLAGS="-g -O1 ${SAN} -I${ROOT} -Wno-deprecated-declarations"

# libFuzzer needs the C++ runtime; clang may not search the gcc lib dir.
STDCPPDIR=$(dirname "$(gcc -print-file-name=libstdc++.so 2>/dev/null)" 2>/dev/null || true)
LDEXTRA=""
if [ -n "${STDCPPDIR}" ] && [ -d "${STDCPPDIR}" ]; then
  LDEXTRA="-L${STDCPPDIR}"
fi

# Hand-written library sources (mirrors SRC in ../Makefile.in) + generated parser.
LIBSRC=(
  cligen_object.c cligen_callback.c cligen_parsetree.c cligen_pt_head.c
  cligen_handle.c cligen_cv.c cligen_match.c cligen_result.c
  cligen_read.c cligen_io.c cligen_expand.c cligen_syntax.c
  cligen_print.c cligen_cvec.c cligen_buf.c cligen_util.c
  cligen_history.c cligen_regex.c cligen_getline.c build.c
  lex.cligen_parse.c cligen_parse.tab.c
)

for f in cligen_parse.tab.c lex.cligen_parse.c cligen_config.h; do
  if [ ! -f "${ROOT}/${f}" ]; then
    echo "ERROR: ${ROOT}/${f} missing. Run './configure && make' in ${ROOT} first." >&2
    exit 1
  fi
done

OBJDIR="${FUZZDIR}/obj"
mkdir -p "${OBJDIR}"

echo "== Compiling instrumented library objects =="
OBJS=()
for src in "${LIBSRC[@]}"; do
  obj="${OBJDIR}/${src%.c}.o"
  echo "  CC ${src}"
  # shellcheck disable=SC2086
  ${CC} ${CFLAGS} -c "${ROOT}/${src}" -o "${obj}"
  OBJS+=("${obj}")
done

build_target() {
  local name="$1"
  echo "== Linking ${name} =="
  # shellcheck disable=SC2086
  ${CC} ${CFLAGS} -fsanitize=fuzzer ${LDEXTRA} \
      "${FUZZDIR}/${name}.c" "${OBJS[@]}" \
      -o "${FUZZDIR}/${name}"
}

TARGETS=("$@")
if [ ${#TARGETS[@]} -eq 0 ]; then
  TARGETS=(fuzz_spec fuzz_cv fuzz_input fuzz_system)
fi
for t in "${TARGETS[@]}"; do
  build_target "${t}"
done

echo "== Seeding corpora =="
mkdir -p "${FUZZDIR}/corpus_spec" "${FUZZDIR}/corpus_cv" "${FUZZDIR}/corpus_input" "${FUZZDIR}/corpus_system"
# Spec corpus: existing .cli files are ideal seeds.
cp -f "${ROOT}"/*.cli "${FUZZDIR}/corpus_spec/" 2>/dev/null || true
cp -f "${ROOT}"/test/*.cli "${FUZZDIR}/corpus_spec/" 2>/dev/null || true
# Cv corpus: type-selector byte + a value string.
printf '\x0e1.2.3.4'                              > "${FUZZDIR}/corpus_cv/seed_ipv4"
printf '\x10f0:de:f1:1b:10:47'                    > "${FUZZDIR}/corpus_cv/seed_mac"
printf '\x12550e8400-e29b-41d4-a716-446655440000' > "${FUZZDIR}/corpus_cv/seed_uuid"
# Input corpus: representative user commands for the complex spec.
printf 'show version\n'                                           > "${FUZZDIR}/corpus_input/seed01"
printf 'show interface eth0 statistics\n'                         > "${FUZZDIR}/corpus_input/seed02"
printf 'show route 10.0.0.0/8\n'                                  > "${FUZZDIR}/corpus_input/seed03"
printf 'show route 10.0.0.0/8 nexthop 192.168.1.1\n'             > "${FUZZDIR}/corpus_input/seed04"
printf 'show route6 2001:db8::/32\n'                              > "${FUZZDIR}/corpus_input/seed05"
printf 'show mac 00:11:22:33:44:55\n'                             > "${FUZZDIR}/corpus_input/seed06"
printf 'show uuid 550e8400-e29b-41d4-a716-446655440000\n'         > "${FUZZDIR}/corpus_input/seed07"
printf 'show decimal 3.141\n'                                     > "${FUZZDIR}/corpus_input/seed08"
printf 'show history 100\n'                                       > "${FUZZDIR}/corpus_input/seed09"
printf 'show log\n'                                               > "${FUZZDIR}/corpus_input/seed10"
printf 'config hostname router1 mtu 1500 debug on\n'             > "${FUZZDIR}/corpus_input/seed11"
printf 'config log level debug\n'                                 > "${FUZZDIR}/corpus_input/seed12"
printf 'ping 192.168.1.1\n'                                       > "${FUZZDIR}/corpus_input/seed13"
printf 'ping 192.168.1.1 count 5 size 64\n'                      > "${FUZZDIR}/corpus_input/seed14"
printf 'ping6 2001:db8::1\n'                                      > "${FUZZDIR}/corpus_input/seed15"
printf 'traceroute 8.8.8.8 ttl 30\n'                             > "${FUZZDIR}/corpus_input/seed16"
printf 'file read verbose myfile.txt\n'                          > "${FUZZDIR}/corpus_input/seed17"
printf 'file write force out.bin offset 4096\n'                   > "${FUZZDIR}/corpus_input/seed18"
printf 'test int32 -42\n'                                         > "${FUZZDIR}/corpus_input/seed19"
printf 'test bool true\n'                                         > "${FUZZDIR}/corpus_input/seed20"
printf 'test rest hello world this is the rest\n'                 > "${FUZZDIR}/corpus_input/seed21"
printf 'load startup file cfg.txt validate\n'                     > "${FUZZDIR}/corpus_input/seed22"
printf 'save file backup.cfg compress\n'                          > "${FUZZDIR}/corpus_input/seed23"
printf 'show\n'                                                   > "${FUZZDIR}/corpus_input/seed24"
printf 'sh\n'                                                     > "${FUZZDIR}/corpus_input/seed25"

# System corpus: multi-line sessions against the real tutorial.cli spec.
printf 'hello world\n'                                            > "${FUZZDIR}/corpus_system/seed01"
printf 'ip tcp 80\n'                                               > "${FUZZDIR}/corpus_system/seed02"
printf 'ip udp 1.2.3.4\n'                                          > "${FUZZDIR}/corpus_system/seed03"
printf 'access-list permit 1.2.3.4 5.6.7.8\n'                     > "${FUZZDIR}/corpus_system/seed04"
printf 'ex 42\n'                                                   > "${FUZZDIR}/corpus_system/seed05"
printf 'aa bb\naa bb ff\n'                                         > "${FUZZDIR}/corpus_system/seed06"
printf 'values aa\nvalues 42\nvalues hello\n'                     > "${FUZZDIR}/corpus_system/seed07"
printf 'aa bb ca 3 dd\naa bb cb ee\n'                             > "${FUZZDIR}/corpus_system/seed08"
printf 'interface eth0\ninterface eth1\n'                          > "${FUZZDIR}/corpus_system/seed09"
printf 'secret\n'                                                  > "${FUZZDIR}/corpus_system/seed10"
printf 'change prompt newprompt>\n'                                > "${FUZZDIR}/corpus_system/seed11"
printf 'recurse hello world\n'                                     > "${FUZZDIR}/corpus_system/seed12"
printf 'recurse recurse recurse hello world\n'                    > "${FUZZDIR}/corpus_system/seed13"
printf 'increment HAL\n'                                           > "${FUZZDIR}/corpus_system/seed14"
printf 'change tree\nz x\nchange tree\nhello world\n'             > "${FUZZDIR}/corpus_system/seed15"
printf 'add x y\ndel x y\n'                                        > "${FUZZDIR}/corpus_system/seed16"
printf 'quit\nhello world\n'                                       > "${FUZZDIR}/corpus_system/seed17"

echo "Done. Example: ${FUZZDIR}/fuzz_input ${FUZZDIR}/corpus_input -dict=${FUZZDIR}/cligen.dict"
echo "Example: ${FUZZDIR}/fuzz_system ${FUZZDIR}/corpus_system -dict=${FUZZDIR}/tutorial.dict"
