#!/usr/bin/env bash
# CLI var backslash escaping

# Magic line must be first in script (see README.md)
s="$_" ; . ./lib.sh || if [ "$s" = $0 ]; then exit 0; else return 0; fi

fspec=$dir/spec.cli
fin=$dir/in

cat > $fspec <<EOF
  prompt="cli> ";              # Assignment of prompt
  comment="#";                 # Same comment as in syntax
  treename="example";          # Name of syntax (used when referencing)

  a <s:string>, callback();
EOF

newtest "$cligen_file -f $fspec"

echo "$cligen_file -f $fspec" # XXX

newtest "no \\ -> xy"
expectpart "$(echo 'a xyz' | $cligen_file -f $fspec 2>&1)" 0 '0 name:cmd type:rest value:a xy' '1 name:a type:string value:a' '2 name:s type:string value:xy'

newtest "one \\"
expectpart "$(echo 'a x\y' | $cligen_file -f $fspec 2>&1)" 0 '0 name:cmd type:rest value:a xy' '1 name:a type:string value:a' '2 name:s type:string value:xy'

newtest "two \\"
expectpart "$(echo 'a x\\y' | $cligen_file -f $fspec 2>&1)" 0 '0 name:cmd type:rest value:a x\\y' '1 name:a type:string value:a' '2 name:s type:string value:xy'

newtest "three \\"
expectpart "$(echo 'a x\\\y' | $cligen_file -f $fspec 2>&1)" 0 "0 name:cmd type:rest value:a x\\\y" '1 name:a type:string value:a' '2 name:s type:string value:xy'

newtest "four \\"
expectpart "$(echo 'a x\\\\y' | $cligen_file -f $fspec 2>&1)" 0 '0 name:cmd type:rest value:a x\\\\y' '1 name:a type:string value:a' '2 name:s type:string value:x\\y'

newtest "five \\"
expectpart "$(echo 'a x\\\\\y' | $cligen_file -f $fspec 2>&1)" 0 '0 name:cmd type:rest value:a x\\\\y' '1 name:a type:string value:a' '2 name:s type:string value:x\\y'

newtest "no \\ EOF"
cat <<'EOF' > $fin
a xy
EOF
expectpart "$(cat $fin | $cligen_file -f $fspec 2>&1)" 0 '0 name:cmd type:rest value:a xy' '1 name:a type:string value:a' '2 name:s type:string value:xy'

newtest "one \\ EOF"
cat <<'EOF' > $fin
a x\y
EOF
expectpart "$(cat $fin | $cligen_file -f $fspec 2>&1)" 0 '0 name:cmd type:rest value:a xy' '1 name:a type:string value:a' '2 name:s type:string value:xy'

newtest "two \\ EOF"
cat <<'EOF' > $fin
a x\\y
EOF
expectpart "$(cat $fin | $cligen_file -f $fspec 2>&1)" 0 "0 name:cmd type:rest value:a x\\\y" '1 name:a type:string value:a' '2 name:s type:string value:xy'

newtest "three \\ EOF"
cat <<'EOF' > $fin
a x\\\y
EOF
expectpart "$(cat $fin | $cligen_file -f $fspec 2>&1)" 0 "0 name:cmd type:rest value:a x\\\y" '1 name:a type:string value:a' '2 name:s type:string value:xy'

newtest "four \\ EOF"
cat <<'EOF' > $fin
a x\\\\y
EOF
expectpart "$(cat $fin | $cligen_file -f $fspec 2>&1)" 0 '0 name:cmd type:rest value:a x\\\\y' '1 name:a type:string value:a' '2 name:s type:string value:x\\y'

# Some problems on freebsd, just ignore it
#newtest "five \\ EOF"
#cat <<'EOF' > $fin
#a x\\\\\y
#EOF
#expectpart "$(cat $fin | $cligen_file -f $fspec 2>&1)" 0 '0 name:cmd type:rest value:a x\\\\\y' '1 name:a type:string value:a' '2 name:s type:string value:x\\y'

newtest "six \\ EOF"
cat <<'EOF' > $fin
a x\\\\\\y
EOF
expectpart "$(cat $fin | $cligen_file -f $fspec 2>&1)" 0 '0 name:cmd type:rest value:a x\\\\\\y' '1 name:a type:string value:a' '2 name:s type:string value:x\\y'

newtest "endtest"
endtest

rm -rf $dir
