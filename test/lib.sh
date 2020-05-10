#!/usr/bin/env bash
# CLIgen tests
# Create working dir as variable "dir"
# The functions are somewhat wildgrown, a little too many:
# - expectfn
# - expecteq
# - expecteof
# - expecteofeq
# - expecteofx
# - expecteof_file
# - expectwait
# - expectmatch

#set -e
# : ${A=B} vs : ${A:=B} # colon also checks for NULL

# Testfile (not including path)
: ${testfile:=$(basename $0)}

# Add test to this list that you dont want run
# Typically add them in your site file
: ${SKIPLIST:=""}

>&2 echo "Running $testfile"

# Site file, an example of this file in README.md
if [ -f ./site.sh ]; then
    . ./site.sh
    if [ $? -ne 0 ]; then
	return -1 # skip
    fi
    # test skiplist.
    for f in $SKIPLIST; do
	if [ "$testfile" = "$f" ]; then
	    echo "...skipped (see site.sh)"
	    return -1 # skip
	fi
    done
fi

# Test number from start
: ${testnr:=0}

# Test number in this test
testi=0

# Single test. Set by "new"
testname=

# Valgind memory leak check.
# The values are:
# 0: No valgrind check
# 1: Start valgrind at every new testcase. Check result every next new
# 2: Start valgrind every new backend start. Check when backend stops
# 3: Start valgrind every new restconf start. Check when restconf stops
# 
: ${valgrindtest=0}

# Valgrind log file. This should be removed automatically. Note that mktemp
# actually creates a file so do not call it by default
#: ${valgrindfile=$(mktemp)}

# If set, enable debugging (of backend and restconf daemons)
: ${DBG:=0}

# Follow the binary programs that can be parametrized (eg with valgrind)

: ${cligen_file:=../cligen_file}

: ${cligen_tutorial:=../cligen_tutorial}

dir=/var/tmp/$0
if [ ! -d $dir ]; then
    mkdir $dir
fi

# Some tests may set owner of testdir to something strange and quit, need
# to reset to me
if [ ! -G $dir ]; then 
    u=$(whoami)
    sudo chown $u $dir
    sudo chgrp $u $dir
fi

# error and exit,
# arg1: expected
# arg2: errmsg[optional]
err(){
  echo -e "\e[31m\nError in Test$testnr [$testname]:"
  if [ $# -gt 0 ]; then 
      echo "Expected: $1"
      echo
  fi
  if [ $# -gt 1 ]; then 
      echo "Received: $2"
  fi
  echo -e "\e[0m"
  echo "$ret"| od -t c > $dir/clixon-ret
  echo "$expect"| od -t c > $dir/clixon-expect
  diff $dir/clixon-expect $dir/clixon-ret 

  exit -1 #$testnr
}

# Test is previous test had valgrind errors if so quit
checkvalgrind(){
    if [ -f $valgrindfile ]; then
	res=$(cat $valgrindfile | grep -e "reachable" -e "lost:"|awk '{print  $4}' | grep -v '^0$')
	if [ -n "$res" ]; then
	    >&2 cat $valgrindfile
	    sudo rm -f $valgrindfile
	    exit -1	    
	fi
	sudo rm -f $valgrindfile
    fi
}

# Increment test number and print a nice string
new(){
    if [ $valgrindtest -eq 1 ]; then 
	checkvalgrind
    fi
    testnr=`expr $testnr + 1`
    testi=`expr $testi + 1`
    testname=$1
    >&2 echo "Test $testi($testnr) [$1]"
}

# Evaluate and return
# like expecteq but partial match is OK
# Example: expectpart $(fn arg) 0 "my return" -- "foo"
# - evaluated expression
# - expected command return value (0 if OK)
# - expected stdout outcome*
# - the token "--not--"
# - not expected stdout outcome*
# @note need to escape \[\]
expectpart(){
  r=$?
  ret=$1
  retval=$2
  expect=$3

#  echo "r:$r"
#  echo "ret:\"$ret\""
#  echo "retval:$retval"
#  echo "expect:\"$expect\""
  if [ $r != $retval ]; then
      echo -e "\e[31m\nError ($r != $retval) in Test$testnr [$testname]:"
      echo -e "\e[0m:"
      exit -1
  fi
  if [ -z "$ret" -a -z "$expect" ]; then
      return
  fi
  # Loop over all variable args expect strings (skip first two args)
  # note that "expect" var is never actually used
  # Then test positive for strings, if the token --neg-- is detected, then test negative for the rest
  positive=true;
  let i=0;
  for exp in "$@"; do
      if [ "$exp" == "--not--" ]; then
	  positive=false;
      elif [ $i -gt 1 ]; then
#	   echo "echo \"$ret\" | grep --null -o \"$exp"\"
	   match=$(echo "$ret" | grep --null -o "$exp") # XXX -EZo: -E cant handle {}
	   r=$? 
	   if $positive; then
	       if [ $r != 0 ]; then
		   err "$exp" "$ret"
	       fi
	   else
	       if [ $r == 0 ]; then
		   err "not $exp" "$ret"
	       fi
	   fi
       fi
       let i++;
  done
#  if [[ "$ret" != "$expect" ]]; then
#      err "$expect" "$ret"
#  fi
}

