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
  aaa <int32>, callback();
  b <b:int32>, callback();
  c <c:int32 show:"a number">("A 32-bit number"), callback();

# 3.10 Choice
  interface <ifname:string choice:eth0|eth1>("Interface name"), callback();

# * Choice with variable
  extra (<crypto:string>|<crypto:string choice:mc:aes|mc:foo|des:des|des:des3>), callback();
EOF

newtest "$cligen_file -f $fspec"

# 3.1 Basic structure
# commands
# Note variants of aa, aaa:
# aa is input, aaa is completed
newtest "aa 42"
expectpart "$(echo "aa 42" | $cligen_file -f $fspec 2>&1)" 0 "0 name:cmd type:rest value:aa 42" "1 name:aaa type:string value:aaa" "2 name:int32 type:int32 value:42"

# No string param
newtest "aa 42 exclude keys"
expectpart "$(echo "aa 42" | $cligen_file -E -f $fspec 2>&1)" 0 "0 name:cmd type:rest value:aa 42" "1 name:int32 type:int32 value:42" --not-- "name:aaa type:string value:aaa"

# aa->aaa in first arg
newtest "aa 42 expand cvv0"
expectpart "$(echo "aa 42" | $cligen_file -c -f $fspec 2>&1)" 0 "0 name:cmd type:rest value:aaa 42" "1 name:aaa type:string value:aaa" "2 name:int32 type:int32 value:42"

# both of the above
newtest "aa 42 exclude keys + expand cvv0"
expectpart "$(echo "aa 42" | $cligen_file -c -E -f $fspec 2>&1)" 0 "0 name:cmd type:rest value:aaa 42" "1 name:int32 type:int32 value:42" --not-- "name:aaa type:string value:aaa"

newtest "b 42"
expectpart "$(echo "b 42" | $cligen_file -f $fspec 2>&1)" 0 "1 name:b type:string value:b" "2 name:b type:int32 value:42"

newtest "c 42"
expectpart "$(echo "c 42" | $cligen_file -f $fspec 2>&1)" 0 "1 name:c type:string value:c" "2 name:c type:int32 value:42"

# query
newtest "a ?"
expectpart "$(echo -n "a ?" | $cligen_file -f $fspec 2>&1)" 0 "<int32>"

newtest "b ?"
expectpart "$(echo -n "b ?" | $cligen_file -f $fspec 2>&1)" 0 "<b>"

newtest "c ?"
expectpart "$(echo -n "c ?" | $cligen_file -f $fspec 2>&1)" 0 "<a number>            A 32-bit number"

# completion
newtest "a<tab><tab>"
expectpart "$(echo -n "a		" | $cligen_file -f $fspec 2>&1)" 0 "<int32>"

newtest "b <tab><tab>"
expectpart "$(echo -n "b		" | $cligen_file -f $fspec 2>&1)" 0 "<b>"

newtest "c <tab><tab>"
expectpart "$(echo -n "c		" | $cligen_file -f $fspec 2>&1)" 0 "<a number>"

# 3.10 Choice
newtest "interface eth0"
expectpart "$(echo "interface eth0" | $cligen_file -f $fspec 2>&1)" 0 "1 name:interface type:string value:interface" "2 name:ifname type:string value:eth0"

newtest "interface eth1"
expectpart "$(echo "interface eth1" | $cligen_file -f $fspec 2>&1)" 0 "1 name:interface type:string value:interface" "2 name:ifname type:string value:eth1"

newtest "interface ethx unknown"
expectpart "$(echo "interface ethx" | $cligen_file -f $fspec 2>&1)" 0 "Unknown command"

newtest "interface eth0 ?"
expectpart "$(echo -n "interface ?" | $cligen_file -f $fspec 2>&1)" 0 "eth0                  Interface name" "eth1                  Interface name"

# * Choice with variable
newtest "extra foo"
expectpart "$(echo "extra xxx" | $cligen_file -f $fspec 2>&1)" 0 "1 name:extra type:string value:extra" "2 name:crypto type:string value:xxx"

newtest "extra mc:aes"
expectpart "$(echo "extra mc:aes" | $cligen_file -f $fspec 2>&1)" 0 "1 name:extra type:string value:extra" "2 name:crypto type:string value:mc:aes"

newtest "extra des:des3"
expectpart "$(echo "extra des:des3" | $cligen_file -f $fspec 2>&1)" 0 "1 name:extra type:string value:extra" "2 name:crypto type:string value:des:des3"

newtest "extra ?"
expectpart "$(echo -n "extra ?" | $cligen_file -f $fspec 2>&1)" 0 "<crypto>" "des:des" "des:des3" "mc:aes" "mc:foo"

newtest "extra des:?"
expectpart "$(echo -n "extra des:?" | $cligen_file -f $fspec 2>&1)" 0 "<crypto>" "des:des" "des:des3" --not-- "mc:aes" "mc:foo"

newtest "endtest"
endtest

rm -rf $dir
