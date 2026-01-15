#!/usr/bin/env bash
# CLI expand callbacks - the tutorial has eth0/1 use them

# Magic line must be first in script (see README.md)
s="$_" ; . ./lib.sh || if [ "$s" = $0 ]; then exit 0; else return 0; fi

fspec=$dir/spec.cli

cat > $fspec <<EOF
  prompt="cli> ";              # Assignment of prompt
  comment="#";                 # Same comment as in syntax
  treename="tutorial";         # Name of syntax (used when referencing)

  # Expand example
  interface {
    <ifname:string interface()>("interface name"),cb(); 
  }
EOF

newtest "$cligen_tutorial -q -f $fspec"

newtest "cligen i<tab>"
expectpart "$(echo "i	" | $cligen_tutorial -q -f $fspec)" 0 "cli> interface " 'CLI syntax error in: "interface": Incomplete command'

newtest "cligen i<tab><tab>"
expectpart "$(echo "i		" | $cligen_tutorial -q -f $fspec)" 0 "cli> interface eth" 'Ambiguous command'

newtest "cligen i<tab><tab><tab>"
expectpart "$(echo "i			" | $cligen_tutorial -q -f $fspec)" 0 "cli> interface eth" "eth0                      eth1" 'Ambiguous command'

newtest "cligen i<tab><tab>?"
expectpart "$(echo "i		?" | $cligen_tutorial -q -f $fspec)" 0 "cli> interface eth" "eth0                  Interface A" 'Ambiguous command'

newtest "cligen interface eth0"
expectpart "$(echo "interface eth0" | $cligen_tutorial -q -f $fspec 2>&1)" 0 "2 name:ifname type:string value:eth0"

newtest "cligen interface foo unknown"
expectpart "$(echo "interface foo" | $cligen_tutorial -q -f $fspec)" 0 'CLI syntax error in: "interface foo": Unknown command'

# Here is using an extra variable for new (unknown) expanded variables
cat > $fspec <<EOF
  prompt="cli> ";              # Assignment of prompt
  comment="#";                 # Same comment as in syntax
  treename="tutorial";         # Name of syntax (used when referencing)

  # Expand example
  interface {
    (<ifname:string>|<ifname:string interface()>),cb(); 
  }
EOF

newtest "cligen interface eth0"
expectpart "$(echo "interface eth0" | $cligen_tutorial -q -f $fspec 2>&1)" 0 "2 name:ifname type:string value:eth0"

newtest "cligen interface foo"
expectpart "$(echo "interface foo" | $cligen_tutorial -q -f $fspec 2>&1)" 0 "2 name:ifname type:string value:foo"

# Two overlapping expand sets (there was a problem when the same occurs in two sets, the
# succeeding items did not work)
# Here is using an extra variable for new (unknown) expanded variables
cat > $fspec <<EOF
  prompt="cli> ";              # Assignment of prompt
  comment="#";                 # Same comment as in syntax
  treename="example";          # Name of syntax (used when referencing)

  # Expand example using cligen_file special expand functions that give different sets
  a (<x:string>|<x:string exp()>|<x:string other()>) y, callback();
EOF

newtest "a foo y"
expectpart "$(echo "a foo y" | $cligen_file -e -f $fspec 2>&1)" 0 "1 name:a type:string value:a" "2 name:x type:string value:foo" "3 name:y type:string value:y"

newtest "a exp1 y"
expectpart "$(echo "a exp1 y" | $cligen_file -e -f $fspec 2>&1)" 0 "1 name:a type:string value:a" "2 name:x type:string value:exp1" "3 name:y type:string value:y"

# XXX: this does not work as expected, you get unknown command,
# It is a known issue and tricky to fix
if false; then
    newtest "a exp2 y"
    expectpart "$(echo "a exp2 y" | $cligen_file -e -f $fspec 2>&1)" 0 "1 name:a type:string value:a" "2 name:x type:string value:exp2" "3 name:y type:string value:y"
fi

# Tab modes
# See description in cligen_handle.c
cat > $fspec <<EOF
  prompt="cli> ";              # Assignment of prompt
  comment="#";                 # Same comment as in syntax
  treename="example";          # Name of syntax (used when referencing)

  # Expand example using cligen_file special expand functions that give different sets

  abbb("First command"), callback();
  accc("Second command"), callback();
  x {
    b, callback();
    <c:int32>, callback();
   }	     
   ya; {
     d        ("A command");
     e        ("A command");
     <f:int32>("A variable");
   }
   za zb zc, callback();
EOF


# Double tab tests
# CLIGEN_TABMODE_COLUMNS
newtest "cligen a<tab><tab> tabmode:0"
expectpart "$(echo "a		b" | $cligen_file -t 0 -e -f $fspec 2>&1)" 0 "abbb                      accc" "1 name:abbb type:string value:abbb"

newtest "cligen a<tab><tab> tabmode:1"
expectpart "$(echo "a		b" | $cligen_file -t 1 -e -f $fspec 2>&1)" 0 "1 name:abbb type:string value:abbb" "abbb                  First command"  "accc                  Second command" --not-- "abbb                      accc" 

# CLIGEN_TABMODE_VARS
newtest "cligen x<tab><tab> tabmode:0"
expectpart "$(echo "x		" | $cligen_file -t 0 -e -f $fspec 2>&1)" 0 "1 name:x type:string value:x" "2 name:b type:string value:b"

newtest "cligen x <tab><tab> tabmode:2"
expectpart "$(echo "x		" | $cligen_file -t 2 -e -f $fspec 2>&1)" 0 'CLI syntax error in: "x": Incomplete command'

newtest "cligen x <tab><tab> tabmode:2"
expectpart "$(echo "x		b" | $cligen_file -t 2 -e -f $fspec 2>&1)" 0 "1 name:x type:string value:x" "2 name:b type:string value:b" --not-- 'CLI syntax error in: "x": Incomplete command'

# CLIGEN_TABMODE_STEPS
newtest "cligen z<tab> tabmode:0"
expectpart "$(echo "z	" | $cligen_file -t 0 -e -f $fspec 2>&1)" 0 'CLI syntax error in: "za": Incomplete command' --not-- "za zb zc"

newtest "cligen z<tab> tabmode:4"
expectpart "$(echo "z	" | $cligen_file -t 4 -e -f $fspec 2>&1)" 0 "za zb zc" "1 name:za type:string value:za" "2 name:zb type:string value:zb" "3 name:zc type:string value:zc"

# Test of show help strings
newtest "cligen y<tab> tabmode: 1 no show"
expectpart "$(echo "y	" | $cligen_file -t 1 -e -f $fspec 2>&1)" 0 "ya " --not-- "d                     A command" "<f>                   A variable"

newtest "cligen y<tab><tab> tabmode: 1 no show"
expectpart "$(echo "y		" | $cligen_file -t 1 -e -f $fspec 2>&1)" 0 "ya " "d                     A command" "<f>                   A variable"

newtest "cligen y<tab> tabmode: 9 show"
expectpart "$(echo "y	" | $cligen_file -t 9 -e -f $fspec 2>&1)" 0 "ya " "d                     A command" "<f>                   A variable"
#----------------------------

# Multi-command expansions
cat > $fspec <<EOF
  prompt="cli> ";              # Assignment of prompt
  comment="#";                 # Same comment as in syntax
  treename="tutorial";         # Name of syntax (used when referencing)

  # Expand example
  aaa {
    value;
    value1;
    value2;
  }
EOF

newtest "multiple options"
expectpart "$(echo "aaa value?" | $cligen_file -t 0 -e -f $fspec 2>&1)" 0 value1 value2

newtest "endtest"
endtest

rm -rf $dir
