#!/usr/bin/env bash
# CLI sets

# Magic line must be first in script (see README.md)
s="$_" ; . ./lib.sh || if [ "$s" = $0 ]; then exit 0; else return 0; fi

fspec=$dir/spec.cli

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
  b @{
    c,callback(); 
    @{      
       d, callback();
    } 
    f, callback();
  }
EOF

newtest "$cligen_file -f $fspec"

# subtree a
newtest "a b b2"
expectpart "$(echo "a b b2" | $cligen_file -f $fspec 2>&1)" 0 "1 name:a type:string value:a" "2 name:b type:string value:b" "3 name:b2 type:string value:b2"

newtest "a b b2 c"
expectpart "$(echo "a b b2 c" | $cligen_file -f $fspec 2>&1)" 0 "1 name:a type:string value:a" "2 name:b type:string value:b" "3 name:b2 type:string value:b2" "4 name:c type:string value:c"

newtest "a b b2 c d"
expectpart "$(echo "a b b2 c d" | $cligen_file -f $fspec 2>&1)" 0 "1 name:a type:string value:a" "2 name:b type:string value:b" "3 name:b2 type:string value:b2" "4 name:c type:string value:c" "5 name:d type:string value:d"

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
newtest "b c d"
expectpart "$(echo "b c d" | $cligen_file -f $fspec 2>&1)" 0 "1 name:b type:string value:b" "2 name:c type:string value:c" "3 name:d type:string value:d"

if false; then # notyet (something wrong with "f")
    newtest "b c f"
    # XXX Unknown command
    expectpart "$(echo "b c f" | $cligen_file -f $fspec 2>&1)" 0 "1 name:b type:string value:b" "2 name:c type:string value:c" "3 name:f type:string value:f"

    newtest "b c d f"
    # XXX Unknown command
    expectpart "$(echo "b c d f" | $cligen_file -f $fspec 2>&1)" 0 "1 name:b type:string value:b" "2 name:c type:string value:c" "3 name:d type:string value:d" "4 name:f type:string value:f"
fi

# Negative tests
newtest "b c d d: Already matched"
expectpart "$(echo "b c d d" | $cligen_file -f $fspec 2>&1)" 0 "Already matched"

endtest

rm -rf $dir
