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
  values (<int32> | <string> | <int64> | aa), callback();
EOF

new "CLIgen variable preference tests: $cligen_file -f $fspec"

new "cligen query all"
expectpart "$(echo "values ? aa" | $cligen_file -f $fspec 2>&1)" 0 "cli> values" "aa" "<int32>" "<int64>" "<string>" "1 name:values type:string value:values" "2 name:aa type:string value:aa"

new "cligen pref keyword aa"
expectpart "$(echo "values aa" | $cligen_file -f $fspec 2>&1)" 0 "cli> values aa" "2 name:aa type:string value:aa"

new "cligen pref string kalle"
expectpart "$(echo "values kalle" | $cligen_file -f $fspec 2>&1)" 0 "cli> values kalle" "2 name:string type:string value:kalle"

new "cligen pref int32"
expectpart "$(echo "values 42" | $cligen_file -f $fspec 2>&1)" 0 "cli> values 42" "2 name:int32 type:int32 value:42"

new "cligen pref int64"
expectpart "$(echo "values 427438287432" | $cligen_file -f $fspec 2>&1)" 0 "cli> values 427438287432" "2 name:int64 type:int64 value:427438287432"

rm -rf $dir
