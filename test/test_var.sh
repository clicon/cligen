#!/usr/bin/env bash
# CLI variable test.
# The examples are derived from the CLIgen tutorial, see section numbering.
# Not complete yet

# Magic line must be first in script (see README.md)
s="$_" ; . ./lib.sh || if [ "$s" = $0 ]; then exit 0; else return 0; fi

fspec=$dir/spec.cli

cat > $fspec <<EOF
  prompt="cli> ";              # Assignment of prompt
  comment="#";                 # Same comment as in syntax
  treename="example";          # Name of syntax (used when referencing)

# 3.1 Basic structure
  a <int32>, callback();
  b <b:int32>, callback();
  c <c:int32 show:"a number">("A 32-bit number"), callback();

# 3.12 Choice
  interface <ifname:string choice:eth0|eth1>("Interface name"), callback();

EOF

new "$cligen_file -f $fspec"

# 3.1 Basic structure
# commands
new "a 42"
expectpart "$(echo "a 42" | $cligen_file -f $fspec 2>&1)" 0 "1 name:a type:string value:a" "2 name:int32 type:int32 value:42"

new "b 42"
expectpart "$(echo "b 42" | $cligen_file -f $fspec 2>&1)" 0 "1 name:b type:string value:b" "2 name:b type:int32 value:42"

new "c 42"
expectpart "$(echo "c 42" | $cligen_file -f $fspec 2>&1)" 0 "1 name:c type:string value:c" "2 name:c type:int32 value:42"

# query
new "a ?"
expectpart "$(echo -n "a ?" | $cligen_file -f $fspec 2>&1)" 0 "<int32>"

new "b ?"
expectpart "$(echo -n "b ?" | $cligen_file -f $fspec 2>&1)" 0 "<b>"

new "c ?"
expectpart "$(echo -n "c ?" | $cligen_file -f $fspec 2>&1)" 0 "<a number>            A 32-bit number"

# completion
new "a<tab><tab>"
expectpart "$(echo -n "a		" | $cligen_file -f $fspec 2>&1)" 0 "<int32>"

new "b <tab><tab>"
expectpart "$(echo -n "b		" | $cligen_file -f $fspec 2>&1)" 0 "<b>"

new "c <tab><tab>"
expectpart "$(echo -n "c		" | $cligen_file -f $fspec 2>&1)" 0 "<a number>"

# 3.10 Choice
new "interface eth0"
expectpart "$(echo "interface eth0" | $cligen_file -f $fspec 2>&1)" 0 "1 name:interface type:string value:interface" "2 name:ifname type:string value:eth0"

new "interface eth1"
expectpart "$(echo "interface eth1" | $cligen_file -f $fspec 2>&1)" 0 "1 name:interface type:string value:interface" "2 name:ifname type:string value:eth1"

new "interface ethx unknown"
expectpart "$(echo "interface ethx" | $cligen_file -f $fspec 2>&1)" 0 "Unknown command"

new "interface eth0 ?"
expectpart "$(echo -n "interface ?" | $cligen_file -f $fspec 2>&1)" 0 "eth0                  Interface name" "eth1                  Interface name"

rm -rf $dir
