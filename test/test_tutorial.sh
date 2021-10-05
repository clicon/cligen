#!/usr/bin/env bash
# Run through the tutorial example

# Magic line must be first in script (see README.md)
s="$_" ; . ./lib.sh || if [ "$s" = $0 ]; then exit 0; else return 0; fi

fspec=../tutorial.cli

newtest "$cligen_tutorial -f $fspec"

newtest "? TAB"
expectpart "$(echo "?	" | $cligen_tutorial -f $fspec)" 0 access-list

newtest "aa bb ca 42"
expectpart "$(echo "aa bb ca 42" | $cligen_tutorial -q -f $fspec)" 0 ca

newtest "access-list permit 1.2.3.4 4.3.2.1"
expectpart "$(echo "access-list permit 1.2.3.4 4.3.2.1" | $cligen_tutorial -q -f $fspec)" 0 'access-list'

newtest "add x y"
expectpart "$(echo "add x y" | $cligen_tutorial -q -f $fspec 2>&1)" 0 'arg 0: a'

newtest "change prompt foobar>\ "
expectpart "$(echo "change prompt \"foobar> \"" | $cligen_tutorial -q -f $fspec)" 0 'foobar'

newtest "ex ?"
expectpart "$(echo "ex ?" | $cligen_tutorial -q -f $fspec)" 0 'A number'

newtest "hello"
expectpart "$(echo "hello world" | $cligen_tutorial -q -f $fspec)" 0 'Hello World!'

newtest "increment"
expectpart "$(echo "increment 23" | $cligen_tutorial -q -f $fspec 2>&1)" 0 'value:34'

newtest "interface"
expectpart "$(echo "interface eth0" | $cligen_tutorial -q -f $fspec 2>&1)" 0 'value:eth0'

newtest "ip tcp"
expectpart "$(echo "ip tcp 8080" | $cligen_tutorial -q -f $fspec 2>&1)" 0 'value:8080'

newtest "recurse"
expectpart "$(echo "recurse recurse hello world" | $cligen_tutorial -q -f $fspec 2>&1)" 0 'Hello World!'

newtest "secret"
expectpart "$(echo "secret" | $cligen_tutorial -q -f $fspec 2>&1)" 0 'This is a hidden command'

newtest "values int64"
expectpart "$(echo "values 42676767676" | $cligen_tutorial -q -f $fspec 2>&1)" 0 'int64' --not-- int32

newtest "endtest"
endtest

rm -rf $dir
