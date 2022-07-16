#!/usr/bin/env bash
# CLI lists and expand
# See https://github.com/clicon/clixon/issues/107
# The following four tests are made:
#   1. show apple ?
#   2. show apple <cr>
#   3. show apple size 42<cr>
#   4. show auto ?
# Variations (on/off with the above tests):
# 1. In the cli.spec, with/without a "semicolon" after the first "fruit <name>" command
#
# The test matrix is therefore:
# 4 tests x 2 semicolon = 8 tests in total
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

newtest "$cligen_file -f $fspec"

# First semicolon and "fruit apple ?"
CMD="fruit apple"
newtest "cligen $CMD ?"
expectpart "$(echo -n "$CMD ?" | $cligen_file -e -f $fspec)" 0 "cli> $CMD" "<cr>" "size" "taste"

# Second: semicolon and "fruit apple<cr>"
CMD="fruit apple"
newtest "cligen $CMD"
expectpart "$(echo "$CMD" | $cligen_file -e -f $fspec 2>&1)" 0 "cli> $CMD" "1 name:fruit type:string value:fruit" "2 name:name type:string value:apple"

# Third: semicolon and "fruit apple size 42<cr>"
CMD="fruit apple size 42"
newtest "cligen $CMD"
expectpart "$(echo "$CMD" | $cligen_file -e -f $fspec 2>&1)" 0 "cli> $CMD" "1 name:fruit type:string value:fruit" "2 name:name type:string value:apple" "3 name:size type:string value:size" "4 name:size type:string value:42"

# Fourth: semicolon and "fruit auto ?"
# without -e should be same as "first" above
CMD="fruit auto"
newtest "cligen $CMD ?"
expectpart "$(echo -n "$CMD ?" | $cligen_file -e -f $fspec)" 0 "cli> $CMD" "<cr>" "size" "taste"

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

# Fifth no semicolon and "fruit apple ?"
CMD="fruit apple"

newtest "cligen $CMD ?"
expectpart "$(echo -n "$CMD ?" | $cligen_file -e -f $fspec)" 0 "cli> $CMD" "size" "taste" "--not--" "<cr>"

# Sixth: no semicolon and "fruit apple<cr>"
CMD="fruit apple"
newtest "cligen $CMD"
expectpart "$(echo "$CMD" | $cligen_file -e -f $fspec 2>&1)" 0 "cli> $CMD" 'CLI syntax error in: "fruit apple": Incomplete command'

# Seventh: no semicolon and "fruit apple size 42<cr>"
CMD="fruit apple size 42"
newtest "cligen $CMD"
expectpart "$(echo "$CMD" | $cligen_file -e -f $fspec 2>&1)" 0 "cli> $CMD" "1 name:fruit type:string value:fruit" "2 name:name type:string value:apple" "3 name:size type:string value:size" "4 name:size type:string value:42"

# Eigth: no semicolon and "fruit auto ?"
# without -e should be same as "fifth" above
CMD="fruit auto"

newtest "cligen $CMD ?"
expectpart "$(echo -n "$CMD ?" | $cligen_file -e -f $fspec)" 0 "cli> $CMD" "size" "taste" "--not--" "<cr>"

newtest "endtest"
endtest

rm -rf $dir
