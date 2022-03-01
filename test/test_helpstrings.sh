#!/usr/bin/env bash
# CLI helpstring functionality
# Includes: UTF-8, multi-lines, Multi-instance

# Magic line must be first in script (see README.md)
s="$_" ; . ./lib.sh || if [ "$s" = $0 ]; then exit 0; else return 0; fi

fspec=$dir/spec.cli

cat > $fspec <<EOF
  prompt="cli> ";              # Assignment of prompt
  comment="#";                 # Same comment as in syntax
  treename="cligen";           # Name of syntax

  help("Theodoric the bold
chief of sea-warriors
ruled over the shores of the Hreiðsea") {
      <peer:int32>("IPv4 address");
      <peer:string length[5:10]>("Peer group name");
 }
[aaa("Now he sits armed on")|bbb("his Gothic horse")] ccc("his shield strapped");
[aaa("Now he sits armed on")|bbb("his Gothic horse")] ddd("the prince of the Mærings");
EOF

newtest "$cligen_file -f $fspec"

newtest "multi-line help"
expectpart "$(echo "?" | $cligen_file -f $fspec )" 0 "cli>" "Theodoric the bold" "chief of sea-warriors" "ruled over the shores of the Hreiðsea"

newtest "same command different help query"
expectpart "$(echo "help ?" | $cligen_file -f $fspec 2>&1)" 0 "cli>" "<peer>" "IPv4 address" "Peer group name"

newtest "same command different help tab"
expectpart "$(echo "help 	" | $cligen_file -f $fspec 2>&1)" 0 "cli>" "<peer>" --not-- "IPv4 address" "Peer group name"

newtest "cligen instance ?"
ret=$(echo "?" | $cligen_file -f $fspec 2>&1 2>/dev/null)

newtest "Nr of aaa should be 1"
count=$(echo "$ret" | grep -c "Now he sits armed on")
if [ $count -ne 1 ]; then
    err "number of aaa: 1" $count
fi

newtest "Nr of bbb should be 1"
count=$(echo "$ret" | grep -c "his Gothic horse")
if [ $count -ne 1 ]; then
    err "number of bbb: 1" $count
fi

newtest "Nr of ccc should be 1"
count=$(echo "$ret" | grep -c "his shield strapped")
if [ $count -ne 1 ]; then
    err "number of ccc: 1" $count
fi

newtest "Nr of ddd should be 1"
count=$(echo "$ret" | grep -c "the prince of the Mærings")
if [ $count -ne 1 ]; then
    err "number of ddd: 1" $count
fi

newtest "endtest"
endtest

unset ret
unset count

rm -rf $dir
