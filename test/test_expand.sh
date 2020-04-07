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

new "$cligen_tutorial -q -f $fspec"

new "cligen i<tab>"
expectpart "$(echo "i	" | $cligen_tutorial -q -f $fspec )" 0 "cli> interface " 'CLI syntax error in: "interface": Incomplete command'

new "cligen i<tab><tab>"
expectpart "$(echo "i		" | $cligen_tutorial -q -f $fspec )" 0 "cli> interface eth" 'Ambigous command'

new "cligen i<tab><tab><tab>"
expectpart "$(echo "i			" | $cligen_tutorial -q -f $fspec )" 0 "cli> interface eth" "eth0                      eth1" 'Ambigous command'

new "cligen i<tab><tab>?"
expectpart "$(echo "i		?" | $cligen_tutorial -q -f $fspec )" 0 "cli> interface eth" "eth0                  Interface A" 'Ambigous command'

new "cligen interface eth0"
expectpart "$(echo "interface eth0" | $cligen_tutorial -q -f $fspec 2>&1)" 0 "1 name:ifname type:string value:eth0"

new "cligen interface foo unknown"
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

new "cligen interface eth0"
expectpart "$(echo "interface eth0" | $cligen_tutorial -q -f $fspec 2>&1)" 0 "1 name:ifname type:string value:eth0"

new "cligen interface foo"
expectpart "$(echo "interface foo" | $cligen_tutorial -q -f $fspec 2>&1)" 0 "1 name:ifname type:string value:foo"

rm -rf $dir
