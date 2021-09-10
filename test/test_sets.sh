#!/usr/bin/env bash
# CLI sets

# Magic line must be first in script (see README.md)
s="$_" ; . ./lib.sh || if [ "$s" = $0 ]; then exit 0; else return 0; fi

fspec=$dir/spec.cli

# The calllbacks() are just to get a result back if you do any command (not syntax error)
cat > $fspec <<EOF
  prompt="cli> ";              # Assignment of prompt
  comment="#";                 # Same comment as in syntax
  treename="sets";         # Name of syntax (used when referencing)

  a @{
    <v:int32> <u:int32> <w:int32>, callback();
    b b2, callback();
    c, callback();
    d, callback();
    e <v:int32>, callback();
  }
  b,callback(); @{
    c, callback(); @{
      d, callback();
      e, callback();
    } 
    f,callback(); 
  }
  c,callback(); @{
    d,callback();
    @subtree, callback();
  }

  treename="subtree";           
  xx{
    yy, callback();
  }
EOF

newtest "$cligen_file -f $fspec"

newtest "?"
expectpart "$(echo "?" | $cligen_file -f $fspec )" 0 "cli>" "a" "b"

newtest "a?"
expectpart "$(echo "a?" | $cligen_file -f $fspec )" 0 "cli>" "a" --not-- "b"

newtest "a ?"
expectpart "$(echo "a ?" | $cligen_file -f $fspec )" 0 "cli>" "b" "c" "d" "e" "<v>"

newtest "a c?"
expectpart "$(echo "a c?" | $cligen_file -f $fspec  2>&1)" 0 "cli>" "c" --not-- "  b" "  d" "  e" "<v>"

newtest "a c ?"
expectpart "$(echo "a c ?" | $cligen_file -f $fspec  2>&1)" 0 "cli>" "  b" "  d" "  e" "<v>" --not-- "  c"

newtest "a c d ?"
expectpart "$(echo "a c d ?" | $cligen_file -f $fspec  2>&1)" 0 "cli>" "  b" "  e" "<v>" --not-- "  c" "  d"

newtest "a c b b2 ?"
expectpart "$(echo "a c b b2 ?" | $cligen_file -f $fspec  2>&1)" 0 "cli>" "  d" "  e" "<v>" --not-- "  b" "  c"

# subtree a
newtest "a b b2"
expectpart "$(echo "a b b2" | $cligen_file -f $fspec 2>&1)" 0 "1 name:a type:string value:a" "2 name:b type:string value:b" "3 name:b2 type:string value:b2"

newtest "a b b2 c"
expectpart "$(echo "a b b2 c" | $cligen_file -f $fspec 2>&1)" 0 "1 name:a type:string value:a" "2 name:b type:string value:b" "3 name:b2 type:string value:b2" "4 name:c type:string value:c"

newtest "a b b2 c d"
expectpart "$(echo "a b b2 c d" | $cligen_file -f $fspec 2>&1)" 0 "1 name:a type:string value:a" "2 name:b type:string value:b" "3 name:b2 type:string value:b2" "4 name:c type:string value:c" "5 name:d type:string value:d"

newtest "a c"
expectpart "$(echo "a c" | $cligen_file -f $fspec 2>&1)" 0 "1 name:a type:string value:a" "2 name:c type:string value:c"

newtest "a 33 34 35 e 42"
expectpart "$(echo "a 33 34 35 e 42" | $cligen_file -f $fspec 2>&1)" 0  "1 name:a type:string value:a" "2 name:v type:int32 value:33" "3 name:u type:int32 value:34" "4 name:w type:int32 value:35" "5 name:e type:string value:e" "6 name:v type:int32 value:42"

newtest "a e 42 c 33 34 35"
expectpart "$(echo "a e 42 c 33 34 35" | $cligen_file -f $fspec 2>&1)" 0  "1 name:a type:string value:a" "2 name:e type:string value:e" "3 name:v type:int32 value:42" "4 name:c type:string value:c" "5 name:v type:int32 value:33" "6 name:u type:int32 value:34" "7 name:w type:int32 value:35"

# Negative tests
newtest "a b: Incomplete command"
expectpart "$(echo "a b" | $cligen_file -f $fspec 2>&1)" 0 'CLI syntax error in: "a b": Incomplete command'

newtest "a d d: Already matched"
expectpart "$(echo "a d d" | $cligen_file -f $fspec 2>&1)" 0 'CLI syntax error in: "a d d": Already matched'

newtest "a 33 34 35 99: Already matched"
expectpart "$(echo "a 33 34 35 99" | $cligen_file -f $fspec 2>&1)" 0 "Already matched"

newtest "a 33 34 35 99 100 101: Already matched"
expectpart "$(echo "a 33 34 35 99 100 101" | $cligen_file -f $fspec 2>&1)" 0 "Already matched"

newtest "a e 42 e 99: Already matched"
expectpart "$(echo "a e 42 e 99" | $cligen_file -f $fspec 2>&1)" 0 "Already matched"

# subtree b
newtest "b ?"
expectpart "$(echo "b ?" | $cligen_file -f $fspec 2>&1)" 0 "cli>" "  c" "  f" "1 name:b type:string value:b" --not-- "  d"

newtest "b c ?"
expectpart "$(echo "b c ?" | $cligen_file -f $fspec 2>&1)" 0 "cli>" "  d" "  e" "1 name:b type:string value:b" "2 name:c type:string value:c" --not-- "  c" "  f"

newtest "b c d ?"
expectpart "$(echo "b c d ?" | $cligen_file -f $fspec 2>&1)" 0 "  e" "1 name:b type:string value:b" "2 name:c type:string value:c" "3 name:d type:string value:d" --not-- "  b" "  c" "  d " "  f"

newtest "b c e ?"
expectpart "$(echo "b c e ?" | $cligen_file -f $fspec 2>&1)" 0 "  d" "1 name:b type:string value:b" "2 name:c type:string value:c" "3 name:e type:string value:e" --not-- 

newtest "b c d e ?"
expectpart "$(echo "b c d e ?" | $cligen_file -f $fspec 2>&1)" 0 "  f" "1 name:b type:string value:b" "2 name:c type:string value:c" "3 name:d type:string value:d" "4 name:e type:string value:e" --not-- "  a" "  b" "  c" "  d" "  e"

newtest "b f ?"
expectpart "$(echo "b f ?" | $cligen_file -f $fspec 2>&1)" 0 "  c" "1 name:b type:string value:b" "2 name:f type:string value:f" --not-- "  a" "  b" "  d" "  e"

newtest "b c f ?"
expectpart "$(echo "b c f ?" | $cligen_file -f $fspec 2>&1)" 0 "1 name:b type:string value:b" "2 name:c type:string value:c" "3 name:f type:string value:f" --not-- "  a" "  b" "  c" "  d" "  e"

newtest "b f c ?"
expectpart "$(echo "b f c ?" | $cligen_file -f $fspec 2>&1)" 0 "  d" "  e" "1 name:b type:string value:b" "2 name:f type:string value:f" "3 name:c type:string value:c" --not-- "  a" "  b" "  c"

newtest "b f c d ?"
expectpart "$(echo "b f c d ?" | $cligen_file -f $fspec 2>&1)" 0 "  e" "1 name:b type:string value:b" "2 name:f type:string value:f" "3 name:c type:string value:c" --not-- "  a" "  b" "  c" "  d"

newtest "b f c d e ?"
expectpart "$(echo "b f c d e ?" | $cligen_file -f $fspec 2>&1)" 0 "1 name:b type:string value:b" "2 name:f type:string value:f" "3 name:c type:string value:c" --not-- "  a" "  b" "  c" "  d" "  e"

# Negative tests
newtest "b c d d: Already matched"
expectpart "$(echo "b c d d" | $cligen_file -f $fspec 2>&1)" 0 "Already matched"

newtest "endtest"
endtest

rm -rf $dir

