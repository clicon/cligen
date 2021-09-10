#!/usr/bin/env bash
# CLI variable preference
# tests multiple variable matches with different preferences
# Also test reference using filter statements: @<tree>, filter:<flag>

# Magic line must be first in script (see README.md)
s="$_" ; . ./lib.sh || if [ "$s" = $0 ]; then exit 0; else return 0; fi

fspec=$dir/spec.cli

cat > $fspec <<EOF
  prompt="cli> ";              # Assignment of prompt
  comment="#";                 # Same comment as in syntax
  treename="tutorial";         # Name of syntax (used when referencing)

  # Example of variable preferences
  values (<int64> | @subtree), callback();

  # Parametrized reference 
  parameter @subtree, filter:local, callback();

  treename="subtree";           
  xx{
    yy, callback();
  }
  zz1, local, callback();
  zz2{
    zz3, local, callback(); # filter sub-tree
    zz4, callback();
  }
EOF

newtest "$cligen_file -f $fspec"

newtest "cligen ref 42"
expectpart "$(echo "values ? 42" | $cligen_file -f $fspec 2>&1)" 0 "cli> values" "<int64>" "xx" "2 name:int64 type:int64 value:42" "zz1"

newtest "cligen ref xx"
expectpart "$(echo "values xx?" | $cligen_file -f $fspec 2>&1)" 0 "cli> values xx" 'CLI syntax error in: "values xx": Incomplete command'

newtest "cligen ref xx y<tab>"
expectpart "$(echo "values xx y	" | $cligen_file -f $fspec 2>&1)" 0 "cli> values xx yy" "2 name:xx type:string value:xx" "3 name:yy type:string value:yy" 

# test reference using filter statements
newtest "parameter reference ?"
expectpart "$(echo "parameter ?" | $cligen_file -f $fspec 2>&1)" 0 "xx" "zz2" --not-- "zz1"

newtest "parameter reference xx yy"
expectpart "$(echo "parameter xx yy" | $cligen_file -f $fspec 2>&1)" 0 "1 name:parameter type:string value:parameter" "2 name:xx type:string value:xx" "3 name:yy type:string value:yy"

newtest "parameter reference zz1"
expectpart "$(echo "parameter zz1" | $cligen_file -f $fspec 2>&1)" 0 'CLI syntax error in: "parameter zz1": Unknown command' --not-- "1 name:parameter type:string value:parameter" "2 name:zz1 type:string value:zz1"

newtest "parameter reference zz2 ?"
expectpart "$(echo "parameter zz2 ?" | $cligen_file -f $fspec 2>&1)" 0 --not-- zz3

newtest "parameter reference zz2 zz3"
expectpart "$(echo "parameter zz2 zz3" | $cligen_file -f $fspec 2>&1)" 0 "CLI syntax error in:" --not-- "1 name:parameter type:string value:parameter" "2 name:zz2 type:string value:zz2" "3 name:zz3 type:string value:zz3"

newtest "parameter reference zz2 zz4"
expectpart "$(echo "parameter zz2 zz4" | $cligen_file -f $fspec 2>&1)" 0 "1 name:parameter type:string value:parameter" "2 name:zz2 type:string value:zz2" "3 name:zz4 type:string value:zz4"

newtest "endtest"
endtest

rm -rf $dir
