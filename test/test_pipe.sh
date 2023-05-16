#!/usr/bin/env bash
# Output pipe modifier tests
# Assume pipe_shell_fn and grep_fn in cligen_file.c

# Magic line must be first in script (see README.md)
s="$_" ; . ./lib.sh || if [ "$s" = $0 ]; then exit 0; else return 0; fi

fspec=$dir/spec.cli

lines='"line6 def", "line7 def", "line1 abc", "line2 abc", "line3 abc", "line4 abc", "line5 def"'

cat > $fspec <<EOF
  prompt="cli> ";              # Assignment of prompt
  comment="#";                 # Same comment as in syntax
  treename="test";             # Name of syntax (used when referencing)
  pipetree="|mypipe";

  set1, output_fn($lines);
  other;
  set2, output_fn($lines); {     
      @|mypipe, output_fn($lines); 
  }
  set3 {     
      @|mypipe, output_fn($lines); 
  }
  set4 @treeref;

  treename="|mypipe";
  pipetree="";
  \| { 
      grep <arg:rest>, pipe_shell_fn("grep -e", "arg");
      tail,  pipe_shell_fn("tail -3");
      count, pipe_shell_fn("wc -l");
      cat,   pipe_shell_fn("cat");
  }

  treename="treeref";
  pipetree="|mypipe";
  extra, output_fn($lines);
EOF

# Run pipe commands
# Parameters:
# 1: Top-level command
# 2: Direct command (ie no pipe)
function runtest()
{
    cmd=$1
    direct=$2
    
    if $direct; then
        newtest "base"
        expectpart "$(echo "$cmd" | $cligen_file -f $fspec)" 0 "line1 abc" "line5 def"
    fi
    
    newtest "grep"
    expectpart "$(echo "$cmd \| grep abc" | $cligen_file -f $fspec)" 0 "line1 abc" --not-- "line6 def"

    newtest "tail"
    expectpart "$(echo "$cmd \| tail" | $cligen_file -f $fspec)" 0 "line3 abc" --not-- "line2 abc"

    newtest "count"
    expectpart "$(echo "$cmd \| count" | $cligen_file -f $fspec)" 0 7
}

newtest "$cligen_file -f $fspec"

newtest "Implicit output pipe"
runtest set1 true

newtest "Explicit output pipe"
runtest set2 true

newtest "Explicit no direct"
runtest set3 false

newtest "Implicit via treeref"
runtest "set4 extra" true

newtest "endtest"
endtest

rm -rf $dir

