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

# Per-choice helptexts: choice:opt1("help1")|opt2("help2")|opt3
cat > $fspec << 'SPEC'
  prompt="cli> ";
  comment="#";
  treename="example";
  set fmt <fmt:string choice:xml("XML format")|json("JSON format")|text>("Output format"), callback();
SPEC

newtest "choice helptext: xml"
expectpart "$(echo "set fmt ?" | $cligen_file -f $fspec 2>&1)" 0 "xml" "XML format"

newtest "choice helptext: json"
expectpart "$(echo "set fmt ?" | $cligen_file -f $fspec 2>&1)" 0 "json" "JSON format"

newtest "choice helptext: text falls back to leaf helptext"
expectpart "$(echo "set fmt ?" | $cligen_file -f $fspec 2>&1)" 0 "text" "Output format"

newtest "choice helptext: xml match"
expectpart "$(echo "set fmt xml" | $cligen_file -f $fspec 2>&1)" 0 "value:xml" --not-- "value:json"

# Per-choice helptexts with number type
cat > $fspec << 'SPEC'
  prompt="cli> ";
  comment="#";
  treename="example";
  set level <level:int32 choice:1("Low")|2("Medium")|3>("Level"), callback();
SPEC

newtest "choice helptext number: 1 shows help"
expectpart "$(echo "set level ?" | $cligen_file -f $fspec 2>&1)" 0 "1" "Low"

newtest "choice helptext number: 2 shows help"
expectpart "$(echo "set level ?" | $cligen_file -f $fspec 2>&1)" 0 "2" "Medium"

newtest "choice helptext number: 3 falls back to leaf helptext"
expectpart "$(echo "set level ?" | $cligen_file -f $fspec 2>&1)" 0 "3" "Level"

newtest "choice helptext number: match 2"
expectpart "$(echo "set level 2" | $cligen_file -f $fspec 2>&1)" 0 "value:2" --not-- "value:1"

# Per-choice helptexts with decimal type
cat > $fspec << 'SPEC'
  prompt="cli> ";
  comment="#";
  treename="example";
  set ratio <ratio:decimal64 fraction-digits:2 choice:0.25("Quarter")|0.50("Half")|0.75>("Ratio"), callback();
SPEC

newtest "choice helptext decimal: 0.25 shows help"
expectpart "$(echo "set ratio ?" | $cligen_file -f $fspec 2>&1)" 0 "0.25" "Quarter"

newtest "choice helptext decimal: 0.50 shows help"
expectpart "$(echo "set ratio ?" | $cligen_file -f $fspec 2>&1)" 0 "0.50" "Half"

newtest "choice helptext decimal: 0.75 falls back to leaf helptext"
expectpart "$(echo "set ratio ?" | $cligen_file -f $fspec 2>&1)" 0 "0.75" "Ratio"

newtest "choice helptext decimal: match 0.50"
expectpart "$(echo "set ratio 0.50" | $cligen_file -f $fspec 2>&1)" 0 "value:0.50" --not-- "value:0.25"

newtest "endtest"
endtest

unset ret
unset count

rm -rf $dir
