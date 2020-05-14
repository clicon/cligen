# CLIgen tests

## Overview

Tests called `test_*.sh` and placed in this directory are
automatically run as part of `all.sh` and `sum.sh` tests. The scripts
need to follow some rules to work properly, please look at one or two
to get the idea.

## Run tests

You can run an individual test by itself, or run through all tests matching 'test_*.sh' in the directory. Prints test output and stops on first error:
```
  all.sh
```

Run all tests but continue after errors and only print a summary test output identifying which tests succeeded and which failed:
```
  sum.sh
```

## Memory leak test
The `mem.sh` runs memory checks using valgrind. If you
```
  mem.sh   2>&1 | tee mylog         
```

## Run pattern of tests

The above scripts work with the `pattern` variable to limit the scope of which tests run, eg:
```
  pattern="test_c*.sh"  mem.sh
```