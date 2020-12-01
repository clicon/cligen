#!/usr/bin/env bash
# CLI types, and regexps

# Magic line must be first in script (see README.md)
s="$_" ; . ./lib.sh || if [ "$s" = $0 ]; then exit 0; else return 0; fi

fspec=$dir/spec.cli

cat > $fspec <<EOF
  prompt="cli> ";              # Assignment of prompt
  comment="#";                 # Same comment as in syntax
  treename="tutorial";         # Name of syntax (used when referencing)

  # Example of variable preferences
  r0  <v:string regexp:"[a-z][0-9]*">;
  r1  <v:string regexp:"^([a-z][0-9]*)$">;
  r2  <v:string regexp:"^[a-z][0-9]*">;
  r3  <v:string regexp:"^([a-z][0-9]*">;
  r4  <v:string regexp:"[a-z][0-9]*$">;
  r5  <v:string regexp:"[a-z][0-9]*)$">;
  i0  <v:int8>;
  i1  <v:int16>;
  i2  <v:int32>;
  i3  <v:int64>;
  ui0 <v:uint8>;
  ui1 <v:uint16>;
  ui2 <v:uint32>;
  ui3 <v:uint64>;
  d0  <v:decimal64 fraction-digits:4 range[0.1:10]>;
  b0  <v:bool>;
  a0  <v:ipv4addr>;
  a1  <v:ipv4prefix>;
  a2  <v:ipv6addr>;
  a3  <v:ipv6prefix>;
  a4  <v:macaddr>;
  u0  <v:url>;
  u1  <v:uuid>;
  t0  <v:time>;
EOF

newtest "$cligen_file -f $fspec"

for x in 0 1 2 3 4 5; do
    newtest "regexp r$x"
    expectpart "$(echo "r$x a42" | $cligen_file -f $fspec)" 0 "cli> r$x a42" --not-- "regexp match fail"

    newtest "regexp $x fail"
    expectpart "$(echo "r$x ax42" | $cligen_file -f $fspec)" 0 "cli> r$x ax42" "regexp match fail"
done

for x in 0 1 2 3; do
    newtest "int i$x"
    expectpart "$(echo "i$x -77" | $cligen_file -f $fspec)" 0 "cli> i$x -77" --not-- "CLI syntax error" 
done

for x in 0 1 2 3; do
    newtest "uint ui$x"
    expectpart "$(echo "ui$x 77" | $cligen_file -f $fspec)" 0 "cli> ui$x 77" --not-- "CLI syntax error"

    newtest "uint ui$x fail"
    expectpart "$(echo "ui$x -77" | $cligen_file -f $fspec)" 0 "cli> ui$x -77" "CLI syntax error"
done

newtest "bool b0"
expectpart "$(echo "b0 true" | $cligen_file -f $fspec)" 0 "cli> b0 true" --not-- "CLI syntax error"

newtest "bool b0 fail"
expectpart "$(echo "b0 xxx" | $cligen_file -f $fspec)" 0 "cli> b0 xxx" "CLI syntax error"

newtest "dec64 d0"
expectpart "$(echo "d0 1.6742" | $cligen_file -f $fspec)" 0 "cli> d0 1.6742" --not-- "CLI syntax error"

newtest "dec64 d0 fail"
expectpart "$(echo "d0 17438598437598437598743" | $cligen_file -f $fspec)" 0 "CLI syntax error"

newtest "dec64 d0 fail"
expectpart "$(echo "d0 1.67425" | $cligen_file -f $fspec)" 0 "cli> d0 1.67425" "CLI syntax error"

newtest "addr a0"
expectpart "$(echo "a0 1.2.3.4" | $cligen_file -f $fspec)" 0 "cli> a0 1.2.3.4" --not-- "CLI syntax error"

newtest "addr a0 fail"
expectpart "$(echo "a0 1.2.3.4." | $cligen_file -f $fspec)" 0 "cli> a0 1.2.3.4." "CLI syntax error"

newtest "addr a1"
expectpart "$(echo "a1 1.2.3.4/24" | $cligen_file -f $fspec)" 0 "cli> a1 1.2.3.4/24" --not-- "regexp match fail"

newtest "addr a2"
expectpart "$(echo "a2 1::5" | $cligen_file -f $fspec)" 0 "cli> a2 1::5" --not-- "regexp match fail"

newtest "addr a3"
expectpart "$(echo "a3 1::5/48" | $cligen_file -f $fspec)" 0 "cli> a3 1::5/48" --not-- "regexp match fail"

newtest "addr a4"
expectpart "$(echo "a4 a4:4e:31:c9:d7:f4" | $cligen_file -f $fspec)" 0 "cli> a4 a4:4e:31:c9:d7:f4" --not-- "regexp match fail"

newtest "url u0"
expectpart "$(echo "u0 http://foo:bar@foo.com/path.com" | $cligen_file -f $fspec)" 0 "cli> u0 http://foo:bar@foo.com/path.com" --not-- "regexp match fail"

newtest "uuid u1"
expectpart "$(echo "u1 550e8400-e29b-41d4-a716-446655440000" | $cligen_file -f $fspec)" 0 "cli> u1 550e8400-e29b-41d4-a716-446655440000" --not-- "regexp match fail"

newtest "time t0"
expectpart "$(echo "t0 2008-09-21T18:57:21.003" | $cligen_file -f $fspec)" 0 "cli> t0 2008-09-21T18:57:21.003" --not-- "regexp match fail"

endtest

rm -rf $dir
