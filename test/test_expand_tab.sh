#!/usr/bin/env bash
# Regression test for issue #92: Custom Expansion Functions called twice on TAB
#
# Tests that TAB completion and help display work correctly with the
# optimization that avoids duplicate match_pattern/expansion calls.
# The fix computes match_pattern once in cli_tab_hook and reuses the
# result for both completion and help display.
#
# Test scenarios from issue #92:
# 1. "set person <TAB>" - expand function for name should be called
# 2. "set person john age <TAB>" - expand function for name should NOT be re-called
# We test this indirectly by verifying correct completion and help behavior.

# Magic line must be first in script (see README.md)
s="$_" ; . ./lib.sh || if [ "$s" = $0 ]; then exit 0; else return 0; fi

fspec=$dir/spec.cli

# Test 1: Basic TAB completion with expand function (issue #92, scenario 1)
# Verify TAB after keyword shows expanded alternatives correctly
cat > $fspec <<EOF
  prompt="cli> ";
  comment="#";
  treename="tutorial";

  # Two-level syntax with expand at first variable
  set {
    person {
      <name:string interface()>("person name") {
        age <value:int32>("age value"), cb();
      }
    }
  }
EOF

newtest "$cligen_tutorial -q -f $fspec"

# TAB after "set person " completes common prefix "eth"
newtest "set person <tab> completes common prefix"
expectpart "$(echo "set person 	" | $cligen_tutorial -q -f $fspec)" 0 "set person eth"

# Double TAB after "set person " shows expanded names
newtest "set person <tab><tab> shows expanded names"
expectpart "$(echo "set person 		" | $cligen_tutorial -q -f $fspec)" 0 "eth0" "eth1"

# TAB completion: "set person e<tab>" should complete to "eth"
newtest "set person e<tab> completes to eth"
expectpart "$(echo "set person e	" | $cligen_tutorial -q -f $fspec)" 0 "set person eth"

# TAB after completed name: "set person eth0 <tab>" should show next level
newtest "set person eth0 <tab> shows age"
expectpart "$(echo "set person eth0 	" | $cligen_tutorial -q -f $fspec)" 0 "age"

# Full command with expanded value should work
newtest "set person eth0 age 42"
expectpart "$(echo "set person eth0 age 42" | $cligen_tutorial -q -f $fspec 2>&1)" 0 "name:name type:string value:eth0"

# Test 2: Multiple TABs in sequence (issue #92 double-call scenario)
# First TAB completes, second TAB shows help - both should work
newtest "set person e<tab><tab> complete then show"
expectpart "$(echo "set person e		" | $cligen_tutorial -q -f $fspec)" 0 "eth0" "eth1"

# Test 3: TAB at deeper level should not re-expand earlier levels
# "set person eth0 age <tab>" - the name expand should not be re-called
newtest "set person eth0 age <tab> shows value help"
expectpart "$(echo "set person eth0 age 	" | $cligen_tutorial -q -f $fspec)" 0 "cli>"

# Test 4: Multiple expand functions at different levels
cat > $fspec <<EOF
  prompt="cli> ";
  comment="#";
  treename="tutorial";

  # Two expand functions at different levels
  config {
    <iface:string interface()>("interface") {
      address <addr:string interface()>("address"), cb();
    }
  }
EOF

newtest "config <tab><tab> shows first level expand"
expectpart "$(echo "config 		" | $cligen_tutorial -q -f $fspec)" 0 "eth0" "eth1"

newtest "config eth0 address <tab><tab> shows second level expand"
expectpart "$(echo "config eth0 address 		" | $cligen_tutorial -q -f $fspec)" 0 "eth0" "eth1"

newtest "config eth0 address eth1"
expectpart "$(echo "config eth0 address eth1" | $cligen_tutorial -q -f $fspec 2>&1)" 0 "name:iface type:string value:eth0" "name:addr type:string value:eth1"

# Test 5: Tab completion with TABMODE_COLUMNS (the default non-COLUMNS mode)
# Verify both column and line help display work with shared match_result
newtest "triple tab shows columns help"
expectpart "$(echo "set person 			" | $cligen_tutorial -q -f $fspec)" 0 "cli>"

newtest "endtest"
endtest
