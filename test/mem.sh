#!/usr/bin/env bash
# Run valgrind leak test for cligen
# Stop on first error
# Typical run:  ./mem.sh 2>&1 | tee mylog
    
# Pattern to run tests, default is all, but you may want to narrow it down
: ${pattern:=test_*.sh}

# Run valgrindtest once, args:
# what: (cli|netconf|restconf|backend)* # no args means all
memonce(){

    valgrindfile=$(mktemp)
    echo "valgrindfile:$valgrindfile"
    valgrindtest=1
#    : ${RCWAIT:=5} # valgrind backend needs some time to get up 
# --suppressions=./valgrind-clixon.supp
    cligen_file="/usr/bin/valgrind --leak-check=full --show-leak-kinds=all  --track-fds=yes --trace-children=no --child-silent-after-fork=yes --log-file=$valgrindfile ../cligen_file"
    cligen_tutorial="/usr/bin/valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes --trace-children=no --child-silent-after-fork=yes --log-file=$valgrindfile ../cligen_tutorial"

    err=0
    for test in $pattern; do
	if [ $testnr != 0 ]; then echo; fi
	testfile=$test
	. ./$test 
	errcode=$?
	if [ $errcode -ne 0 ]; then
	    err=1
	    echo -e "\e[31mError in $test errcode=$errcode"
	    echo -ne "\e[0m"
	    exit $errcode
	fi
    done
    if [ $valgrindtest -eq 1 ]; then
	checkvalgrind
	sudo rm -f $valgrindfile
    fi
}

# Print a line with ==== under
println(){
    str=$1
    echo "$str"
    length=$(echo "$str" | wc -c)
    let i=1
    while [ $i -lt $length ]; do
	echo -n "="
	let i++
    done
    echo
}

# Then actual run
testnr=0
if [ $testnr != 0 ]; then echo; fi
println "Mem test cligen begin"
memonce $cmd1
println "Mem test cligen done"


unset pattern
