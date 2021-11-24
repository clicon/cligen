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
  abd {
    a,callback();
    b,callback();
  }
  exec <a:string>, cligen_exec_cb("echo \$a"); # Trivial: just echo
EOF

newtest "$cligen_file -f $fspec"

newtest "b unknown"
expectpart "$(echo "b" | $cligen_file -f $fspec)" 0 "Unknown command"

newtest "a OK"
expectpart "$(echo "a" | $cligen_file -f $fspec 2>&1)" 0 "1 name:a type:string value:a"

newtest "ab ambiguous"
expectpart "$(echo "ab" | $cligen_file -f $fspec)" 0 "Ambiguous command"

newtest "ab ambiguous preference mode"
expectpart "$(echo "ab" | $cligen_file -P -f $fspec 2>&1)" 0 "Ambiguous command"

newtest "abc ok"
expectpart "$(echo "abc" | $cligen_file -f $fspec 2>&1)" 0 "1 name:abc type:string value:abc"

newtest "abd incomplete"
expectpart "$(echo "abd" | $cligen_file -f $fspec 2>&1)" 0 'CLI syntax error in: "abd": Incomplete command'

newtest "run hello"
expectpart "$(echo "hello world" | $cligen_hello 2>&1)" 0 'hello world'

newtest "run exec"
expectpart "$(echo "exec foo" | $cligen_file -f $fspec 2>&1)" 0 foo

newtest "endtest"
endtest

rm -rf $dir

