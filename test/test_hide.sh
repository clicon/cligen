#!/usr/bin/env bash
# CLIgen comment (runtime not clispec)

# Magic line must be first in script (see README.md)
s="$_" ; . ./lib.sh || if [ "$s" = $0 ]; then exit 0; else return 0; fi

fspec=$dir/spec.cli

cat > $fspec <<EOF
  prompt="cli> ";             # Assignment of prompt
  comment="#";                # Same comment as in syntax
  treename="example";         # Name of syntax (used when referencing)

  a00, hide, callback();{
    aa0 <s:string>, hide, callback();
    ab0 <s:string>, callback();
  }
  b00, callback();{
    ba0, hide{
        <s:string>, callback();
    }
    bb0 {
      bca, callback();
      bcb,hide {
        <s:string>, callback();
      }
    }
    bc0, hide{
      bda, callback();
      bdb <s:string>, callback();
    }
  }
EOF

newtest "$cligen_file -f $fspec"

newtest "top level a00 hidden"
expectpart "$(echo "?" | $cligen_file -f $fspec)" 0 "b00" --not-- "a00"

newtest "first level hidden"
expectpart "$(echo "a00 ? " | $cligen_file -f $fspec)" 0 --not-- "aa0"  "ab0"

newtest "second level command"
expectpart "$(echo "a00 ab0 str" | $cligen_file -f $fspec 2>&1)" 0 "0 name:cmd type:rest value:a00 ab0 str" "1 name:a00 type:string value:a00" "2 name:ab0 type:string value:ab0" "3 name:s type:string value:str"

newtest "first level shown"
expectpart "$(echo "b00 ? " | $cligen_file -f $fspec)" 0 bb0 --not-- ba0 bc0

newtest "second level shown"
expectpart "$(echo "b00 bb0 ? " | $cligen_file -f $fspec)" 0 bca --not-- bcb

newtest "third level command"
expectpart "$(echo "b00 bb0 bcb foo" | $cligen_file -f $fspec 2>&1)" 0 "0 name:cmd type:rest value:b00 bb0 bcb foo" "1 name:b00 type:string value:b00" "2 name:bb0 type:string value:bb0" "3 name:bcb type:string value:bcb" "4 name:s type:string value:foo"

newtest "endtest"
endtest

rm -rf $dir

