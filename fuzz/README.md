# CLIgen fuzzing

This dir contains code for fuzzing cligen. 

## Prereqs

See [AFL docs](https://afl-1.readthedocs.io/en/latest) for installing afl.

You may have to change cpu frequency:
```
  cd /sys/devices/system/cpu
  echo performance | tee cpu?/cpufreq/scaling_governor
```

And possibly change core behaviour:
```
  echo core >/proc/sys/kernel/core_pattern
```

## Build

CLIgen must be built statically, eg as follows:
```
  CC=/usr/bin/afl-gcc CXX=/usr/bin/afl-g++ LINKAGE=static ./configure
  make clean
  make
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


