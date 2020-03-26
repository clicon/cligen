#!/usr/bin/env bash
# CLI variable preference
# tests multiple variable matches with different preferences

# Magic line must be first in script (see README.md)
s="$_" ; . ./lib.sh || if [ "$s" = $0 ]; then exit 0; else return 0; fi

fspec=$dir/spec.cli

cat > $fspec <<EOF
  prompt="cli> ";              # Assignment of prompt
  comment="#";                 # Same comment as in syntax
  treename="tutorial";         # Name of syntax (used when referencing)

  # Example of variable preferences
  values (<int64> | @subtree), callback();

  treename="subtree";           
  xx{
    yy, callback();
  }

EOF

new "CLIgen variable preference tests: $cligen_file -f $fspec"

new "cligen ref 42"
expectpart "$(echo "values ? 42" | $cligen_file -f $fspec 2>&1)" 0 "cli> values" "<int64>" "xx" "2 name:int64 type:int64 value:42"

new "cligen ref xx"
expectpart "$(echo "values xx?" | $cligen_file -f $fspec 2>&1)" 0 "cli> values xx" 'CLI syntax error in: "values xx": Incomplete command'

new "cligen ref xx y<tab>"
expectpart "$(echo "values xx y	" | $cligen_file -f $fspec 2>&1)" 0 "cli> values xx yy" "2 name:xx type:string value:xx" "3 name:yy type:string value:yy"

rm -rf $dir
