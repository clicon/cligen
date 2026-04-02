#!/usr/bin/env bash
# CLIgen node filter callback (-n option)
# Tests that cligen_node_filter_fn can hide individual nodes from completion,
# including nodes at nested levels (containers/lists) and that variable nodes
# (CO_VARIABLE) are not affected by the filter.

# Magic line must be first in script (see README.md)
s="$_" ; . ./lib.sh || if [ "$s" = $0 ]; then exit 0; else return 0; fi

fspec=$dir/spec.cli

cat > $fspec <<EOF
  prompt="cli> ";
  comment="#";
  treename="example";

  a, callback();
  b, callback();
  c {
    ca, callback();
    cb, callback();
    cc, callback();
  }
  d {
    <val:string>, callback();
  }
EOF

newtest "$cligen_file -f $fspec"

# 1. Without filter: all top-level nodes visible
newtest "1: No filter - all nodes visible"
expectpart "$(echo "?" | $cligen_file -f $fspec)" 0 "a" "b" "c" "d"

# 2. Hide 'b' via -n flag: 'b' not visible, others still are
newtest "2: Hide b - only a, c, d visible"
expectpart "$(echo "?" | $cligen_file -n b -f $fspec)" 0 "a" "c" "d" --not-- "b"

# 3. Hide 'a' and 'b' together (multiple -n flags)
newtest "3: Hide a and b"
expectpart "$(echo "?" | $cligen_file -n a -n b -f $fspec)" 0 "c" "d" --not-- "a" --not-- "b"

# 4. Hide nested node 'ca' inside container 'c'
newtest "4: Hide nested ca inside c"
expectpart "$(echo "c ?" | $cligen_file -n ca -f $fspec)" 0 "cb" "cc" --not-- "ca"

# 5. Filter does NOT hide variable nodes (CO_VARIABLE) - 'd' uses <val:string>
newtest "5: Filter by variable name has no effect (CO_VARIABLE not filtered)"
expectpart "$(echo "d ?" | $cligen_file -n val -f $fspec)" 0 "<val>"

# 6. Completion still works for hidden node's parent
newtest "6: Parent 'c' still visible when only children hidden"
expectpart "$(echo "?" | $cligen_file -n ca -n cb -n cc -f $fspec)" 0 "a" "b" "c" "d"

# 7. All children hidden: parent still present but entering it gives no children
newtest "7: All c children hidden - c still visible at top"
expectpart "$(echo "?" | $cligen_file -n ca -n cb -n cc -f $fspec)" 0 "c"

newtest "7b: All c children hidden - 'c ?' gives no completions"
expectpart "$(echo "c ?" | $cligen_file -n ca -n cb -n cc -f $fspec)" 0 --not-- "ca" --not-- "cb" --not-- "cc"

newtest "endtest"
endtest

rm -rf $dir
