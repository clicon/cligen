#!/usr/bin/env bash
# Run a fuzzing test using american fuzzy lop
set -eux

if [ $# -ne 2 ]; then 
    echo "usage: $0 <cli spec file> <input string>\n"
    exit 255
fi

fspec=$1
input=$2

MEGS=500 # memory limit for child process (50 MB)

# remove input and input dirs
test ! -d input || rm -rf input
test ! -d output || rm -rf output

# create if dirs dont exists
test -d input || mkdir input
test -d output || mkdir output

# Create input string
cat > input/1.cli <<EOF
$input
EOF

# Run script
afl-fuzz -i input -o output -m $MEGS -- ../cligen_file -f $fspec
