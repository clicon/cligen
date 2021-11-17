#!/usr/bin/env bash
# CLI types, and regexps
# Mainly to get coverage of type functions in cligen_cv.c

# Magic line must be first in script (see README.md)
s="$_" ; . ./lib.sh || if [ "$s" = $0 ]; then exit 0; else return 0; fi

fspec=$dir/spec.cli

cat > $fspec <<EOF
  prompt="cli> ";              # Assignment of prompt
  comment="#";                 # Same comment as in syntax
  treename="types";            # Name of syntax (used when referencing)

  # Example of variable preferences
  r0  <v:string regexp:"[a-z][0-9]*">, callback();
  r1  <v:string regexp:"^([a-z][0-9]*)$">, callback();
  r2  <v:string regexp:"^[a-z][0-9]*">, callback();
  r3  <v:string regexp:"^([a-z][0-9]*">, callback();
  r4  <v:string regexp:"[a-z][0-9]*$">, callback();
  r5  <v:string regexp:"[a-z][0-9]*)$">, callback();
  i0  <v:int8 range[-100:100]>, callback();
  i1  <v:int16 range[-10000:10000]>, callback();
  i2  <v:int32 range[-1000000:1000000]>, callback();
  i3  <v:int64 range[-10000000000:10000000000]>, callback();
  ui0 <v:uint8>, callback();
  ui1 <v:uint16>, callback();
  ui2 <v:uint32>, callback();
  ui3 <v:uint64>, callback();
  d0  <v:decimal64 fraction-digits:4 range[0.1:10]>, callback();
  b0  <v:bool>, callback();
  a0  <v:ipv4addr>, callback();
  a1  <v:ipv4prefix>, callback();
  a2  <v:ipv6addr>, callback();
  a3  <v:ipv6prefix>, callback();
  a4  <v:macaddr>, callback();
  u0  <v:url>, callback();
  u1  <v:uuid>, callback();
  t0  <v:time>, callback();
EOF

newtest "$cligen_file -f $fspec"

newtest "print syntax, check first and last"
expectpart "$(echo "" | $cligen_file -f $fspec -p)" 0  'a0 <v:ipv4addr>, callback();' 'r0 <v:string regexp:"\[a-z\]\[0-9\]\*">, callback();' 't0 <v:time>, callback();'

newtest "dump syntax"
expectpart "$(echo "" | $cligen_file -f $fspec -d)" 0  "pt types" "co a0"

# regexps
for x in 0 1 2 3 4 5; do
    newtest "regexp r$x"
    expectpart "$(echo "r$x a42" | $cligen_file -f $fspec 2> /dev/null)" 0 "cli> r$x a42" --not-- "is invalid input for cli command"

    newtest "regexp $x fail"
    expectpart "$(echo "r$x ax42" | $cligen_file -f $fspec 2> /dev/null)" 0 "cli> r$x ax42" "is invalid input for cli command"
done

# integers
for x in 0 1 2 3; do
    newtest "int i$x"
    expectpart "$(echo "i$x -77" | $cligen_file -f $fspec 2> /dev/null)" 0 "cli> i$x -77" --not-- "CLI syntax error" 
done

# integer fails, need to take them one by one
newtest "int i0 fail"
expectpart "$(echo "i0 -200" | $cligen_file -f $fspec 2> /dev/null)" 0 "out of range"

newtest "int i1 fail"
expectpart "$(echo "i1 -20000" | $cligen_file -f $fspec 2> /dev/null)" 0 "out of range"

newtest "int i2 fail"
expectpart "$(echo "i2 -2000000000" | $cligen_file -f $fspec 2> /dev/null)" 0 "out of range"

newtest "int i3 fail"
expectpart "$(echo "i2 -200000000000000000000000" | $cligen_file -f $fspec 2> /dev/null)" 0 "out of range"

# unsigned integers
for x in 0 1 2 3; do
    newtest "uint ui$x"
    expectpart "$(echo "ui$x 77" | $cligen_file -f $fspec 2> /dev/null)" 0 "cli> ui$x 77" --not-- "CLI syntax error"

    newtest "uint ui$x fail"
    expectpart "$(echo "ui$x -77" | $cligen_file -f $fspec 2> /dev/null)" 0 "cli> ui$x -77" "CLI syntax error"
done

newtest "bool b0"
expectpart "$(echo "b0 true" | $cligen_file -f $fspec 2> /dev/null)" 0 "cli> b0 true" --not-- "CLI syntax error"

newtest "bool b0 fail"
expectpart "$(echo "b0 xxx" | $cligen_file -f $fspec 2> /dev/null)" 0 "cli> b0 xxx" "CLI syntax error"

newtest "dec64 d0"
expectpart "$(echo "d0 1.6742" | $cligen_file -f $fspec 2> /dev/null)" 0 "cli> d0 1.6742" --not-- "CLI syntax error"

newtest "dec64 d0 fail"
expectpart "$(echo "d0 17438598437598437598743" | $cligen_file -f $fspec 2> /dev/null)" 0 "CLI syntax error"

newtest "dec64 d0 fail"
expectpart "$(echo "d0 1.67425" | $cligen_file -f $fspec 2> /dev/null)" 0 "cli> d0 1.67425" "CLI syntax error"

newtest "addr a0"
expectpart "$(echo "a0 1.2.3.4" | $cligen_file -f $fspec 2> /dev/null)" 0 "cli> a0 1.2.3.4" --not-- "CLI syntax error"

newtest "addr a0 fail"
expectpart "$(echo "a0 1.2.3.4." | $cligen_file -f $fspec 2> /dev/null)" 0 "cli> a0 1.2.3.4." "CLI syntax error"

newtest "addr a1"
expectpart "$(echo "a1 1.2.3.4/24" | $cligen_file -f $fspec 2> /dev/null)" 0 "cli> a1 1.2.3.4/24" --not-- "regexp match fail"

newtest "addr a2"
expectpart "$(echo "a2 1::5" | $cligen_file -f $fspec 2> /dev/null)" 0 "cli> a2 1::5" --not-- "regexp match fail"

newtest "addr a3"
expectpart "$(echo "a3 1::5/48" | $cligen_file -f $fspec 2> /dev/null)" 0 "cli> a3 1::5/48" --not-- "regexp match fail"

newtest "addr a4"
expectpart "$(echo "a4 a4:4e:31:c9:d7:f4" | $cligen_file -f $fspec 2> /dev/null)" 0 "cli> a4 a4:4e:31:c9:d7:f4" --not-- "regexp match fail"

newtest "url u0"
expectpart "$(echo "u0 http://foo:bar@foo.com/path.com" | $cligen_file -f $fspec 2> /dev/null)" 0 "cli> u0 http://foo:bar@foo.com/path.com" --not-- "Invalid URL"

newtest "url u0 fail"
expectpart "$(echo "u0 http////" | $cligen_file -f $fspec 2> /dev/null)" 0 "Invalid URL"

newtest "uuid u1"
expectpart "$(echo "u1 550e8400-e29b-41d4-a716-446655440000" | $cligen_file -f $fspec 2> /dev/null)" 0 "cli> u1 550e8400-e29b-41d4-a716-446655440000" --not-- "Invalid uuid"

newtest "uuid u1 fail"
expectpart "$(echo "u1 550e8400-e29b" | $cligen_file -f $fspec 2> /dev/null)" 0  "Invalid uuid"

newtest "time t0"
expectpart "$(echo "t0 2008-09-21T18:57:21.003" | $cligen_file -f $fspec 2> /dev/null)" 0 "cli> t0 2008-09-21T18:57:21.003" --not-- "Invalid time"

newtest "time t0 fail"
expectpart "$(echo "t0 foobar" | $cligen_file -f $fspec 2> /dev/null)" 0 "Invalid time"

newtest "endtest"
endtest

rm -rf $dir

