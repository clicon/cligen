#!/usr/bin/env bash
# CLI tree mode 

# Magic line must be first in script (see README.md)
s="$_" ; . ./lib.sh || if [ "$s" = $0 ]; then exit 0; else return 0; fi

fspec=$dir/spec.cli
fin=$dir/in

cat > $fspec <<EOF
  prompt="cli> ";              # Assignment of prompt
  comment="#";                 # Same comment as in syntax
  treename="tutorial";         # Name of syntax (used when referencing)

  edit,cligen_wp_set("working");{
    @working, cligen_wp_set("working");
  }
  show, cligen_wp_show("working");
  up, cligen_wp_up("working");
  top, cligen_wp_top("working");

  treename="working";           # Example on how to set and change a working point in a tree
  a; {
    b <v:int32>; {
      d;
    }
    c;
  }
EOF

newtest "$cligen_tutorial -f $fspec" # To bind the cligen_wp_* functions

newtest "cligen show top tree"
expectpart "$(echo "show" | $cligen_tutorial -q -f $fspec 2>&1)" 0 "a;{" "b <v>;{" "d;" "c;"

cat <<EOF > $fin
up
show
EOF
newtest "cligen up"
expectpart "$(cat $fin | $cligen_tutorial -q -f $fspec 2>&1)" 0 "a;{" "b <v>;{" "d;" "c;"

cat <<EOF > $fin
edit a
show
EOF
newtest "cligen edit a"
expectpart "$(cat $fin | $cligen_tutorial -q -f $fspec 2>&1)" 0 "b <v>;{" "d;" "c;" --not-- "a;{"

cat <<EOF > $fin
edit a
edit b 23
show
EOF

newtest "cligen edit a b <int>"
expectpart "$(cat $fin | $cligen_tutorial -q -f $fspec 2>&1)" 0 "d;" --not-- "b <v>;{" "c;"  "a;{"

cat <<EOF > $fin
edit a b 23
up
show
EOF

newtest "cligen edit a b <int> up"
expectpart "$(cat $fin | $cligen_tutorial -q -f $fspec 2>&1)" 0 "<v>;{" "d;" --not-- "b <v>;{" "c;"  "a;{"

cat <<EOF > $fin
edit a b 23
up
up
show
EOF

newtest "cligen edit a b <int> up up"
expectpart "$(cat $fin | $cligen_tutorial -q -f $fspec 2>&1)" 0 "b <v>;{" "c;" "d;" --not-- "a;{"

cat <<EOF > $fin
edit a b 23
top
show
EOF

newtest "cligen edit a b <int> top"
expectpart "$(cat $fin | $cligen_tutorial -q -f $fspec 2>&1)" 0 "a;{" "b <v>;{" "d;" "c;"

newtest "endtest"
endtest

rm -rf $dir

