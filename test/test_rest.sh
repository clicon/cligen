#!/usr/bin/env bash
# CLI rest 

# Magic line must be first in script (see README.md)
s="$_" ; . ./lib.sh || if [ "$s" = $0 ]; then exit 0; else return 0; fi

fspec=$dir/spec.cli

cat > $fspec <<EOF
  prompt="cli> ";              # Assignment of prompt
  comment="#";                 # Same comment as in syntax
  treename="tutorial";         # Name of syntax (used when referencing)

  # Expand example
  values {
    <x:rest>,callback(); 
    aa,callback(); 
  }
  xxx <x:rest>, callback();
EOF

newtest "$cligen_file -f $fspec"

newtest "cligen values aa command"
expectpart "$(echo "values aa" | $cligen_file -f $fspec 2>&1)" 0 "1 name:values type:string value:values" "2 name:aa type:string value:aa"

# DOESNT WORK - DIDNT WORK in 4.4 either phew
#newtest "cligen values aa bb rest"
#expectpart "$(echo "values aa bb" | $cligen_file -f $fspec 2>&1)" 0 "1 name:values type:string value:values" "2 name:x type:rest value:aa bb"

newtest "cligen values aab rest"
expectpart "$(echo "values aab" | $cligen_file -f $fspec 2>&1)" 0 "1 name:values type:string value:values" "2 name:x type:rest value:aab"

newtest "cligen values aab foo rest"
expectpart "$(echo "values aab cde" | $cligen_file -f $fspec 2>&1)" 0 "1 name:values type:string value:values" "2 name:x type:rest value:aab cde"

newtest "endtest"
endtest

rm -rf $dir

