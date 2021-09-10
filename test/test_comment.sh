#!/usr/bin/env bash
# CLIgen comment (runtime not clispec)

# Magic line must be first in script (see README.md)
s="$_" ; . ./lib.sh || if [ "$s" = $0 ]; then exit 0; else return 0; fi

fspec=$dir/spec.cli

cat > $fspec <<EOF
  prompt="cli> ";              # Assignment of prompt
  comment="#";                 # Same comment as in syntax
  treename="tutorial";         # Name of syntax (used when referencing)

  bac;
  abc,callback();{
    <s:rest>, callback();
  }
EOF

newtest "$cligen_file -f $fspec"

newtest "comment line"
expectpart "$(echo "# Comment" | $cligen_file -f $fspec)" 0 "cli> # Comment"

newtest "comment whitespace line"
expectpart "$(echo "   # Comment" | $cligen_file -f $fspec)" 0 "# Comment"

newtest "comment after command"
expectpart "$(echo "abc # Comment" | $cligen_file -f $fspec 2>&1)" 0 "cli> abc # Comment" "1 name:abc type:string value:abc"

newtest "comment in command"
expectpart "$(echo "abc#Comment" | $cligen_file -f $fspec 2>&1)" 0 "cli> abc#Comment" "CLI syntax error in: \"abc#Comment\": Unknown command" --not-- "1 name:abc type:rest value:abc#Comment"

newtest "comment after command+str"
expectpart "$(echo "abc str # Comment" | $cligen_file -f $fspec 2>&1)" 0 "cli> abc str # Comment" "1 name:abc type:string value:abc" "2 name:s type:rest value:str"

newtest "comment in str"
expectpart "$(echo "abc str#Comment" | $cligen_file -f $fspec 2>&1)" 0 "cli> abc str#Comment" "1 name:abc type:string value:abc" "2 name:s type:rest value:str#Comment"

newtest "comment in rest"
expectpart "$(echo "abc str s# Comment" | $cligen_file -f $fspec 2>&1)" 0 "cli> abc str s# Comment" "1 name:abc type:string value:abc" "2 name:s type:rest value:str s# Comment"

newtest "endtest"
endtest

rm -rf $dir

