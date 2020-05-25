#!/usr/bin/env bash
# CLI completion functionality
# Test s simple spec "value <var>" and try tabs in different variations

# Magic line must be first in script (see README.md)
s="$_" ; . ./lib.sh || if [ "$s" = $0 ]; then exit 0; else return 0; fi

fspec=$dir/spec.cli

cat > $fspec <<EOF
  prompt="cli> ";              # Assignment of prompt
  comment="#";                 # Same comment as in syntax
  treename="tutorial";         # Name of syntax (used when referencing)

  # Complex syntax and callback argument handling
  # Example of complex variable matching
  
  vb;
  values (<int32> | aa), callback();

EOF

new "$cligen_file -f $fspec"

new "empty?"
expectpart "$(echo "?" | $cligen_file -f $fspec )" 0 "cli>" "values" "vb"

new "empty<tab>"
expectpart "$(echo "	  " | $cligen_file -f $fspec )" 0 "cli> v  " "Ambigous command"

new "empty<tab><tab>"
expectpart "$(echo "		" | $cligen_file -f $fspec )" 0 "cli> v" "values                    vb"

new "v<tab> ambiguous"
expectpart "$(echo "v	  " | $cligen_file -f $fspec )" 0 "values                    vb" "Ambigous command"

new "v<tab>a<tab> incomplete"
expectpart "$(echo "v	a	" | $cligen_file -f $fspec)" 0  "values                    vb" "cli> values" 'CLI syntax error in: "values": Incomplete command'

new "v<tab>a<tab>42 OK"
expectpart "$(echo "v	a	42" | $cligen_file -f $fspec 2>&1)" 0  "cli> values 42" "1 name:values type:string value:values" "2 name:int32 type:int32 value:42" 

rm -rf $dir
