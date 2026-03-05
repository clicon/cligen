#!/usr/bin/env bash

# Magic line must be first in script (see README.md)
s="$_" ; . ./lib.sh || if [ "$s" = $0 ]; then exit 0; else return 0; fi

fspec=$dir/spec.cli

cat > $fspec <<EOF
  prompt="cli> ";              # Assignment of prompt
  comment="#";                 # Same comment as in syntax
  treename="partial";          # Name of syntax (used when referencing)

  set (100|<vlan:uint16 range[2:1000]>), callback();
EOF

newtest "$cligen_file -f $fspec"

newtest "partial match out of range reports error"
expectpart "$(echo "set 1" | $cligen_file -f $fspec 2> /dev/null)" 0 "CLI syntax error" "out of range: 2 - 1000"

newtest "exact match of existing command"
expectpart "$(echo "set 100" | $cligen_file -f $fspec 2> /dev/null)" 0 "cli> set 100" --not-- "CLI syntax error"

newtest "valid value in range"
expectpart "$(echo "set 500" | $cligen_file -f $fspec 2> /dev/null)" 0 "cli> set 500" --not-- "CLI syntax error"

newtest "partial match in range matches variable"
expectpart "$(echo "set 10" | $cligen_file -f $fspec 2> /dev/null)" 0 "cli> set 10" --not-- "CLI syntax error"

newtest "value above range reports error"
expectpart "$(echo "set 2000" | $cligen_file -f $fspec 2> /dev/null)" 0 "CLI syntax error" "out of range: 2 - 1000"

newtest "value below range reports error"
expectpart "$(echo "set 0" | $cligen_file -f $fspec 2> /dev/null)" 0 "CLI syntax error" "out of range: 2 - 1000"

newtest "endtest"
endtest

rm -rf $dir
