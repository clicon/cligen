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

new "$cligen_file -f $fspec"

new "cligen values aa"
expectpart "$(echo "values aa" | $cligen_file -f $fspec 2>&1)" 0 "1 name:values type:string value:values" "2 name:aa type:string value:aa"

# XXX: this does not work as expected, 
# sets (type:string), valgrind leaks
# master (aa bb unknwon command) valgrind OK
if false; then
    new "cligen values aa bb"
    expectpart "$(echo "values aa bb" | $cligen_file -f $fspec 2>&1)" 0 "1 name:values type:string value:values" "2 name:x type:rest value:aa bb"
fi

new "cligen values aab"
expectpart "$(echo "values aab" | $cligen_file -f $fspec 2>&1)" 0 "1 name:values type:string value:values" "2 name:x type:rest value:aab"

new "cligen values aab foo"
expectpart "$(echo "values aab cde" | $cligen_file -f $fspec 2>&1)" 0 "1 name:values type:string value:values" "2 name:x type:rest value:aab cde"

new "xx a ? - has memory problems in 4.5"
expectpart "$(echo "xxx a ?" | $cligen_file -f $fspec 2>&1)" 0 "1 name:xxx type:string value:xxx" "2 name:x type:rest value:a"

rm -rf $dir
