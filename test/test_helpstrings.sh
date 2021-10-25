#!/usr/bin/env bash
# CLI helpstring functionality
# Includes: UTF-8, multi-lines

# Magic line must be first in script (see README.md)
s="$_" ; . ./lib.sh || if [ "$s" = $0 ]; then exit 0; else return 0; fi

fspec=$dir/spec.cli

cat > $fspec <<EOF
  prompt="cli> ";              # Assignment of prompt
  comment="#";                 # Same comment as in syntax
  treename="cligen";           # Name of syntax

  help("Theodoric the bold
chief of sea-warriors
ruled overthe shores of the Hreiðsea") {
      <peer:int32>("IPv4 address");
      <peer:string length[5:10]>("Peer group name");
 }

EOF

newtest "$cligen_file -f $fspec"

newtest "multi-line help"
expectpart "$(echo "?" | $cligen_file -f $fspec )" 0 "cli>" "Theodoric the bold" "chief of sea-warriors" "ruled overthe shores of the Hreiðsea"

newtest "same command different help query"
expectpart "$(echo "help ?" | $cligen_file -f $fspec 2>&1)" 0 "cli>" "<peer>" "IPv4 address" "Peer group name"

newtest "same command different help tab"
expectpart "$(echo "help 	" | $cligen_file -f $fspec 2>&1)" 0 "cli>" "<peer>" --not-- "IPv4 address" "Peer group name"

newtest "endtest"
endtest

rm -rf $dir
