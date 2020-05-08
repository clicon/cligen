#!/usr/bin/env bash
# CLI lists and expand
# See https://github.com/clicon/clixon/issues/107
# These tests do three commands with and without a "semicolon" after the first "fruit <name>" command
# and tests the three command with and without an expansion function and preference set
# The test matrix is therefore:
# 3 tests x 2 expansion x 2 preference x 2 semicolon = 24 tests in total
# Note that the results are modelled after how cligen 4.4 behaves, ie to be backward compatible.
# There are some questionable results but deal with that later.

# Magic line must be first in script (see README.md)
s="$_" ; . ./lib.sh || if [ "$s" = $0 ]; then exit 0; else return 0; fi

fspec=$dir/spec.cli

# WITH SEMICOLON (ie command after fruit apple;)
cat > $fspec <<EOF
  prompt="cli> ";             # Assignment of prompt
  comment="#";                # Same comment as in syntax
  treename="example";         # Name of syntax (used when referencing)

   fruit (<name:string>|<name:string expand()>),callback();{
      taste (<taste:string>|<taste:string expand()>),callback();
      size (<size:string>|<size:string expand()>),callback();
   }
EOF

new "$cligen_file -f $fspec"

# First semicolon and "fruit apple ?"
CMD="fruit apple ?"
new "cligen $CMD"
expectpart "$(echo -n "$CMD" | $cligen_file -f $fspec)" 0 "cli> fruit apple" "<cr>" "--not--" "size" "taste"

new "cligen -P $CMD"
expectpart "$(echo -n "$CMD" | $cligen_file -P -f $fspec)" 0 "cli> fruit apple" "<cr>" "--not--" "size" "taste"

new "cligen -e $CMD"
expectpart "$(echo -n "$CMD" | $cligen_file -e -f $fspec)" 0 "cli> fruit apple" "<cr>" "size" "taste"

new "cligen -e -P $CMD"
expectpart "$(echo -n "$CMD" | $cligen_file -e -P -f $fspec)" 0 "cli> fruit apple" "<cr>" "size" "taste"

# Second: semicolon and "fruit apple<cr>"
CMD="fruit apple"
new "cligen $CMD"
expectpart "$(echo "$CMD" | $cligen_file -f $fspec)" 0 "cli> $CMD" "Ambigous command"

new "cligen -P $CMD"
expectpart "$(echo "$CMD" | $cligen_file -P -f $fspec 2>&1)" 0 "cli> $CMD" "1 name:fruit type:string value:fruit" "2 name:name type:string value:apple"

new "cligen -e $CMD"
expectpart "$(echo "$CMD" | $cligen_file -e -f $fspec 2>&1)" 0 "cli> $CMD" "1 name:fruit type:string value:fruit" "2 name:name type:string value:apple"

new "cligen -e -P $CMD"
expectpart "$(echo "$CMD" | $cligen_file -e -P -f $fspec 2>&1)" 0 "cli> $CMD" "1 name:fruit type:string value:fruit" "2 name:name type:string value:apple"

# Third: semicolon and "fruit apple size 42<cr>"
CMD="fruit apple size 42"
new "cligen $CMD"
expectpart "$(echo "$CMD" | $cligen_file -f $fspec)" 0 "cli> $CMD" 'CLI syntax error in: "fruit apple size 42": Unknown command'

new "cligen -P $CMD"
expectpart "$(echo "$CMD" | $cligen_file -P -f $fspec 2>&1)" 0 "cli> $CMD" 'CLI syntax error in: "fruit apple size 42": Unknown command'

new "cligen -e $CMD"
expectpart "$(echo "$CMD" | $cligen_file -e -f $fspec 2>&1)" 0 "cli> $CMD" "1 name:fruit type:string value:fruit" "2 name:name type:string value:apple" "3 name:size type:string value:size" "4 name:size type:string value:42"

new "cligen -e -P $CMD"
expectpart "$(echo "$CMD" | $cligen_file -e -P -f $fspec 2>&1)" 0 "cli> $CMD" "1 name:fruit type:string value:fruit" "2 name:name type:string value:apple" "3 name:size type:string value:size" "4 name:size type:string value:42"

# WITHOUT SEMICOLON (ie no stand-alone command at fruit apple)
cat > $fspec <<EOF
  prompt="cli> ";             # Assignment of prompt
  comment="#";                # Same comment as in syntax
  treename="example";         # Name of syntax (used when referencing)

   fruit (<name:string>|<name:string expand()>){
      taste (<taste:string>|<taste:string expand()>),callback();
      size (<size:string>|<size:string expand()>),callback();
   }
EOF

# Fourth no semicolon and "fruit apple ?"
CMD="fruit apple ?"

new "cligen $CMD"
# This seems wrong, why does it give <cr>??
expectpart "$(echo -n "$CMD" | $cligen_file -f $fspec)" 0 "cli> fruit apple" "<cr>" "--not--" "size" "taste"

new "cligen -P $CMD"
expectpart "$(echo -n "$CMD" | $cligen_file -P -f $fspec)" 0 "cli> fruit apple" "--not--" "<cr>" "size" "taste"

new "cligen -e $CMD"
expectpart "$(echo -n "$CMD" | $cligen_file -e -f $fspec)" 0 "cli> fruit apple" "size" "taste" "--not--" "<cr>"

new "cligen -e -P $CMD"
expectpart "$(echo -n "$CMD" | $cligen_file -e -P -f $fspec)" 0 "cli> fruit apple" "size" "taste" "--not--" "<cr>"

# Fifth: no semicolon and "fruit apple<cr>"
CMD="fruit apple"
new "cligen $CMD"
expectpart "$(echo "$CMD" | $cligen_file -f $fspec)" 0 "cli> $CMD" "Ambigous command"

new "cligen -P $CMD"
expectpart "$(echo "$CMD" | $cligen_file -P -f $fspec 2>&1)" 0 "cli> $CMD" 'CLI syntax error in: "fruit apple": Incomplete command'

new "cligen -e $CMD"
expectpart "$(echo "$CMD" | $cligen_file -e -f $fspec 2>&1)" 0 "cli> $CMD" 'CLI syntax error in: "fruit apple": Incomplete command'

new "cligen -e -P $CMD"
expectpart "$(echo "$CMD" | $cligen_file -e -P -f $fspec 2>&1)" 0 "cli> $CMD" 'CLI syntax error in: "fruit apple": Incomplete command'

# Sixth: no semicolon and "fruit apple size 42<cr>"
CMD="fruit apple size 42"
new "cligen $CMD"
expectpart "$(echo "$CMD" | $cligen_file -f $fspec)" 0 "cli> $CMD" 'CLI syntax error in: "fruit apple size 42": Unknown command'

new "cligen -P $CMD"
expectpart "$(echo "$CMD" | $cligen_file -P -f $fspec 2>&1)" 0 "cli> $CMD" 'CLI syntax error in: "fruit apple size 42": Unknown command'

new "cligen -e $CMD"
expectpart "$(echo "$CMD" | $cligen_file -e -f $fspec 2>&1)" 0 "cli> $CMD" "1 name:fruit type:string value:fruit" "2 name:name type:string value:apple" "3 name:size type:string value:size" "4 name:size type:string value:42"

new "cligen -e -P $CMD"
expectpart "$(echo "$CMD" | $cligen_file -e -P -f $fspec 2>&1)" 0 "cli> $CMD" "1 name:fruit type:string value:fruit" "2 name:name type:string value:apple" "3 name:size type:string value:size" "4 name:size type:string value:42"

rm -rf $dir
