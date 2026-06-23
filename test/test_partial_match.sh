#!/usr/bin/env bash

# Magic line must be first in script (see README.md)
s="$_" ; . ./lib.sh || if [ "$s" = $0 ]; then exit 0; else return 0; fi

fspec=$dir/spec.cli

cat > $fspec <<EOF
  prompt="cli> ";              # Assignment of prompt
  comment="#";                 # Same comment as in syntax
  treename="partial";          # Name of syntax (used when referencing)

  set (100|<vlan:uint16 range[2:1000]>), callback();
  show (history|<count:uint32>), callback();
EOF

newtest "$cligen_file -f $fspec"

# Issue #134: partial keyword match must not suppress a constraint-validation error.
# Input "1" parses as uint16 but violates range[2:1000]; "1" also prefix-matches
# keyword "100".  The range error must win over the partial keyword match.
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

# Issue #140 Issue 2: a type-parse failure (not a constraint error) from a sibling
# variable must NOT suppress a partial keyword match.
# Input "his" cannot parse as uint32 at all; it prefix-matches keyword "history".
# The partial keyword match must be preserved and the type error discarded.
newtest "type-mismatch error does not suppress partial keyword match"
expectpart "$(echo "show his" | $cligen_file -f $fspec 2> /dev/null)" 0 "cli> show his" --not-- "CLI syntax error"

# Sanity: exact keyword and valid variable still work alongside the above fix.
newtest "exact keyword match works"
expectpart "$(echo "show history" | $cligen_file -f $fspec 2> /dev/null)" 0 "cli> show history" --not-- "CLI syntax error"

newtest "valid uint32 value works"
expectpart "$(echo "show 42" | $cligen_file -f $fspec 2> /dev/null)" 0 "cli> show 42" --not-- "CLI syntax error"

newtest "endtest"
endtest

rm -rf $dir
