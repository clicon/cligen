# CLIgen

CLIgen is a Command-Line Interface generator.

Well, actually it is not really a generator, since it does
not _generate_ code for CLI:s. Instead, it builds and interprets
datastructures (a parse-tree) which a library (libcligen) interprets
in runtime.  It is fast and efficient and helps you develop CLI:s
easier. You enter a CLI syntax either in a text file, you
write callback functions where you call the right library
routines. The callback functions add the semantics, that is, what the
commands in the CLI are supposed to do. 

I try to keep thi [cligen tutorial](cligen_tutorial.pdf) up to date
and is probably the best way to understand CLIgen.

Some background material can be found on the [CLIgen project
page](http://www.cligen.se).

CLIgen is _not_ a system in itself, you need to build your own
'backend'.  We have started another project: 'clixon' which is
actually a whole system where you load dynamic frontend and backend
modules [CLIXON project page](http://www.clicon.org) and [CLIXON
github](https://github.com/clicon/clixon). Clixon provides a
system, with embedded database, commit semantics, YANG and NETCONF
interface, etc. CLIgen is a part of clixon.

The source code here is built and installed using:
```
  configure;
  make;
  sudo make install.
```

The source builds a single library. If you build applications, you should include cligen.h and link with the library.

There are several example applications:
* cligen_hello Simplest possible. Just builds a 'hello world' greeting by in-line C
* cligen_file Read a syntax specification from file. You must supply the file.
* cligen_tutorial Samples of techniques used in [cligen_tutorial.pdf](cligen_tutorial.pdf)

See also [Changelog](CHANGELOG.md).

For building the C reference documentation using doxygen, do: `make doc` and place your browser at `doc/index.html`.

CLIgen is dual license. Either Apache License, Version 2.0 or GNU
General Public License Version 2. You choose.

I can be found at olof@hagsand.se.

## getline


CLIgen uses getline with the following copyright:

Copyright (C) 1991, 1992, 1993 by Chris Thewalt (thewalt@ce.berkeley.edu)

Permission to use, copy, modify, and distribute this software 
for any purpose and without fee is hereby granted, provided
that the above copyright notices appear in all copies and that both the
copyright notice and this permission notice appear in supporting
documentation.  This software is provided "as is" without express or
implied warranty.


