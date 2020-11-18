# CLIgen fuzzing

This dir contains code for fuzzing cligen. 

## Prereqs

See [AFL docs](https://afl-1.readthedocs.io/en/latest) for installing afl.

You may have to change cpu frequency:
```
  cd /sys/devices/system/cpu
  echo performance | tee cpu?/cpufreq/scaling_governor
```

## Build

CLIgen must be built statically, eg as follows:
```
  CC=/usr/bin/afl-gcc CXX=/usr/bin/afl-g++ ./configure
  make clean
  make
  ar rcs libcligen.a cligen_object.o cligen_parsetree.o cligen_pt_head.o cligen_handle.o cligen_cv.o cligen_match.o cligen_read.o cligen_io.o cligen_expand.o cligen_syntax.o cligen_print.o cligen_cvec.o cligen_buf.o cligen_util.o cligen_history.o cligen_regex.o cligen_getline.o build.o  lex.cligen_parse.o cligen_parse.tab.o
  /usr/bin/afl-gcc -DHAVE_CONFIG_H -O2 -Wall -I. -I. cligen_file.c  -L. -o cligen_file libcligen.a
```

## Run tests

Use the script `runfuzz.sh` to run one test with a cli spec and an input string, eg:
```
  ./runfuzz.sh ./specs/commands.cli "abd a"
  ./runfuzz.sh ./specs/sets.cli "b f c d ?"  
```

CLIgen specs are taken from the [test dir](../test).

After (or during) the test, investigate results in the output dir.

Note that one test is done at a time, you cannot run concurrent tests this way, since there is a single output dir.


