#!/usr/bin/env bash
# CLI getline tests
# go through all control

# Magic line must be first in script (see README.md)
s="$_" ; . ./lib.sh || if [ "$s" = $0 ]; then exit 0; else return 0; fi

fspec=$dir/spec.cli

cat > $fspec <<EOF
  prompt="cli> ";              # Assignment of prompt
  comment="#";                 # Same comment as in syntax
  treename="example";          # Name of syntax (used when referencing)

  Theodoric the bold;
  chief of sea-warriors;
  ruled over;{the shores of the Hreiðsea;}
EOF

newtest "$cligen_file -f $fspec"

newtest "^abdeik"
expectpart "$(echo -n "r	c				" | $cligen_file -f $fspec 2>&1)" 0 "chief of sea-warriors"

newtest "^h"
expectpart "$(echo -n "ruled over the" | $cligen_file -f $fspec 2>&1)" 0 "ruled over"

newtest "<esc>b,^f"
expectpart "$(echo -n "ruled over theb" | $cligen_file -f $fspec 2>&1)" 0 "ruled over the"

newtest "<esc>f,^f"
expectpart "$(echo -n "ruled over thebf" | $cligen_file -f $fspec 2>&1)" 0 "ruled over the"

newtest "^p"
expectpart "$(echo -n "ruled over the" | $cligen_file -f $fspec 2>&1)" 0 "ruled over the"

newtest "^pn"
expectpart "$(echo -n "ruledTheo				" | $cligen_file -f $fspec)" 0 "Theodoric the bold "

newtest "^ut"
expectpart "$(echo -n "the Theodorcib		bold" | $cligen_file -f $fspec)" 0 ""

newtest "^Y line scroll"
expectpart "$(echo -n "ruled over the shores of the Hreiðsea" | $cligen_file -s 1 -f $fspec)" 0 "the shores of the Hreisea"

newtest "^Y page scroll"
expectpart "$(echo -n "ruled over the shores of the Hreiðsea" | $cligen_file -s 0 -f $fspec)" 0 "the shores of the Hreisea"

newtest "search ^R"
expectpart "$(echo -n "Theodoric ruledTheo 		" | $cligen_file -f $fspec)" 0 "Theodoric the bold"

newtest "^W"
expectpart "$(echo -n "TheodoricThe			" | $cligen_file -f $fspec)" 0 "Theodoric the bold"

newtest "endtest"
endtest

rm -rf $dir
