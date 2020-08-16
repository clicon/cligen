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
  values (<int32> | <string regexp:"[a-z][0-9]*"> | <int64> | aa), callback();
EOF

newtest "$cligen_file -f $fspec"

newtest "cligen query all"
expectpart "$(echo "values ? aa" | $cligen_file -f $fspec 2>&1)" 0 "cli> values" "aa" "<int32>" "<int64>" "<string>" "1 name:values type:string value:values" "2 name:aa type:string value:aa"

newtest "cligen pref keyword aa"
expectpart "$(echo "values aa" | $cligen_file -f $fspec 2>&1)" 0 "cli> values aa" "2 name:aa type:string value:aa"

newtest "cligen pref string regexp a99"
expectpart "$(echo "values a99" | $cligen_file -f $fspec 2>&1)" 0 "cli> values a99" "2 name:string type:string value:a99"

newtest "cligen pref int32"
expectpart "$(echo "values 42" | $cligen_file -f $fspec 2>&1)" 0 "cli> values 42" "2 name:int32 type:int32 value:42"

newtest "cligen pref int64"
expectpart "$(echo "values 427438287432" | $cligen_file -f $fspec 2>&1)" 0 "cli> values 427438287432" "2 name:int64 type:int64 value:427438287432"

newtest "cligen pref int64"
expectpart "$(echo "values 427438287432" | $cligen_file -f $fspec 2>&1)" 0 "cli> values 427438287432" "2 name:int64 type:int64 value:427438287432"

# Maybe failure should be string regexp failure?
newtest "cligen pref no match"
expectpart "$(echo "values 9aa" | $cligen_file -f $fspec 2>&1)" 0 "CLI syntax error in: \"values 9aa\": regexp match fail: 9aa does not match \[a-z\]\[0-9\]*"

endtest

rm -rf $dir
