#!/usr/bin/env bash
# CLI variable preference
# tests multiple variable matches with different preferences
# Tie-breaks with equal preferences for terminal and non-terminals and preference keyword

# Magic line must be first in script (see README.md)
s="$_" ; . ./lib.sh || if [ "$s" = $0 ]; then exit 0; else return 0; fi

fspec=$dir/spec.cli

cat > $fspec <<EOF
  prompt="cli> ";              # Assignment of prompt
  comment="#";                 # Same comment as in syntax
  treename="tutorial";         # Name of syntax (used when referencing)

  # Example of variable preferences
  values (<int32> | <string regexp:"[a-z][0-9]*"> | <int64> | aa), callback();
  # Example of preference tie break, terminal and non-terminal
  tiebreak (<reg1:string regexp:"[a-z]*"> | <reg2:string regexp:"[a-z]*[0-9]*">), callback();{
    port <nr:int16>, callback();
  }
  tiebreak_keyword (<reg1:string regexp:"[a-z]*" preference:9> | <reg2:string regexp:"[a-z]*[0-9]*">), callback();{
    port <nr:int16>, callback();
  }
EOF

newtest "$cligen_file -f $fspec"

newtest "cligen query all"
expectpart "$(echo "values ? aa" | $cligen_file -f $fspec 2>&1)" 0 "cli> values" "aa" "<int32>" "<int64>" "<string>" "1 name:values type:string value:values" "2 name:aa type:string value:aa"

newtest "cligen pref keyword aa"
expectpart "$(echo "values aa" | $cligen_file -f $fspec 2>&1)" 0 "cli> values aa" "2 name:aa type:string value:aa"

newtest "cligen pref string regexp a99"
expectpart "$(echo "values a99" | $cligen_file -f $fspec 2>&1)" 0 "cli> values a99" "2 name:string type:string value:a99"

newtest "cligen pref int32"
expectpart "$(echo "values 42" | $cligen_file -f $fspec 2>&1)" 0 "cli> values 42" "2 name:int32 type:int32 value:42"

newtest "cligen pref int64"
expectpart "$(echo "values 427438287432" | $cligen_file -f $fspec 2>&1)" 0 "cli> values 427438287432" "2 name:int64 type:int64 value:427438287432"

newtest "cligen pref int64"
expectpart "$(echo "values 427438287432" | $cligen_file -f $fspec 2>&1)" 0 "cli> values 427438287432" "2 name:int64 type:int64 value:427438287432"

# Maybe failure should be string regexp failure?
newtest "cligen pref no match"
expectpart "$(echo "values 9aa" | $cligen_file -f $fspec 2>&1)" 0 "CLI syntax error in: \"values 9aa\": \"9aa\" is invalid input for cli command: string"

# -----------tiebreak

newtest "tiebreak terminal OK"
expectpart "$(echo "tiebreak abc99" | $cligen_file -f $fspec 2>&1)" 0 "2 name:reg2 type:string value:abc99"

newtest "tiebreak terminal OK query"
expectpart "$(echo -n "tiebreak abc99 ?" | $cligen_file -f $fspec 2>&1)" 0 "<cr>" "port"

newtest "tiebreak terminal ambiguous"
expectpart "$(echo "tiebreak abc" | $cligen_file -f $fspec 2>&1)" 0 "Ambiguous command"

newtest "tiebreak terminal query"
expectpart "$(echo -n "tiebreak abc ?" | $cligen_file -f $fspec 2>&1)" 0 --not-- "<cr>" "por"t

newtest "tiebreak non-terminal OK"
expectpart "$(echo "tiebreak abc99 port 42" | $cligen_file -f $fspec 2>&1)" 0 "2 name:reg2 type:string value:abc99" "4 name:nr type:int16 value:42"

newtest "tiebreak non-terminal ambiguous"
expectpart "$(echo "tiebreak abc port 42" | $cligen_file -f $fspec 2>&1)" 0 "Ambiguous command"

#----- Break tiebreak
newtest "tiebreak break terminal (-P 1)"
expectpart "$(echo "tiebreak abc" | $cligen_file -P 1 -f $fspec 2>&1)" 0 "2 name:reg1 type:string value:abc" --not-- "Ambiguous command"

newtest "tiebreak not break terminal (-P 2) "
expectpart "$(echo "tiebreak abc" | $cligen_file -P 2 -f $fspec 2>&1)" 0 "Ambiguous command"

newtest "tiebreak break terminal (-P 3)"
expectpart "$(echo "tiebreak abc" | $cligen_file -P 3 -f $fspec 2>&1)" 0 "2 name:reg1 type:string value:abc" --not-- "Ambiguous command"

newtest "tiebreak not break non-terminal (-P 1)"
expectpart "$(echo "tiebreak abc port 42" | $cligen_file -P 1 -f $fspec 2>&1)" 0  "Ambiguous command"

newtest "tiebreak break non-terminal (-P 2)"
expectpart "$(echo "tiebreak abc port 42" | $cligen_file -P 2 -f $fspec 2>&1)" 0  "2 name:reg1 type:string value:abc" "4 name:nr type:int16 value:42" --not-- "Ambiguous command"

newtest "tiebreak break non-terminal (-P 3)"
expectpart "$(echo "tiebreak abc port 42" | $cligen_file -P 3 -f $fspec 2>&1)" 0  "2 name:reg1 type:string value:abc" "4 name:nr type:int16 value:42" --not-- "Ambiguous command"

#----- Break tiebreak with preference keyword
newtest "tiebreak break terminal using preference keyword"
expectpart "$(echo "tiebreak_keyword abc" | $cligen_file -f $fspec 2>&1)" 0 "2 name:reg1 type:string value:abc" --not-- "Ambiguous command"

newtest "endtest"
endtest

rm -rf $dir
