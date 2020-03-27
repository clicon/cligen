#!/usr/bin/env bash
# CLI simple commands: ambiguous, unknown, perfect match

# Magic line must be first in script (see README.md)
s="$_" ; . ./lib.sh || if [ "$s" = $0 ]; then exit 0; else return 0; fi

fspec=$dir/spec.cli

cat > $fspec <<EOF
  prompt="cli> ";              # Assignment of prompt
  comment="#";                 # Same comment as in syntax
  treename="tutorial";         # Name of syntax (used when referencing)

  a,callback();
  abc,callback();
  abd {a,callback();b,callback();}
EOF

new "$cligen_file -f $fspec"

new "cligen b unknown"
expectpart "$(echo "b" | $cligen_file -f $fspec )" 0 "Unknown command"

new "cligen a OK"
expectpart "$(echo "a" | $cligen_file -f $fspec 2>&1)" 0 "1 name:a type:string value:a"

new "cligen ab ambiguous"
expectpart "$(echo "ab" | $cligen_file -f $fspec )" 0 "Ambigous command"

new "cligen abc ok"
expectpart "$(echo "abc" | $cligen_file -f $fspec 2>&1)" 0 "1 name:abc type:string value:abc"

new "cligen abd incomplete"
expectpart "$(echo "abd" | $cligen_file -f $fspec 2>&1)" 0 'CLI syntax error in: "abd": Incomplete command'

rm -rf $dir
