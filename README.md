cligen
======

CLIgen is a Command-Line Interface generator written by Olof Hagsand.

Well, actually it is not really a generator, since it does
not_generate_ code for CLI:s. Instead, it builds and interprets
datastructures (a parse-tree) which a library (libcligen) interprets
in runtime.  It is fast and efficient and helps you develop CLI:s
easier. You enter a CLI syntax either in a file or in-line in C, you
write callback functions and then you call the right library
routines. The callback functions add the semantics, that is, what the
commands in the CLI are supposed to do. 

This is all covered in the [CLIgen tutorial](http://www.hagsand.se/cligen/cligen_tutorial.pdf).

CLIgen is _not_ a system in itself, you need to build your own
'backend'. We have started another project: 'clicon' which is actually
a whole system where you load dynamic frontend and backend
modules. Clicon offers you a system, with embedded database, commit
semantics, netconf interface, etc. CLIgen is a part of clicon.

The source code here is built and installed using configure; make;
sudo make install. The source builds a single library. If you build
applications, you should include cligen.h and link with the library.

The directory contains source for CLIgen. There are several example 
applications:
* cligen_hello Simplest possible. Just builds a 'hello world' greeting by in-line C
* cligen_file Read a syntax specification from file. You must supply the file.
* cligen_tutorial Samples of techniques used in cligen_tutorial.pdf.

CLIgen is covered by GPLv2, but can be obtained with a commercial license.

I can be found at olof@hagsand.se.

getline
-------
CLIgen uses getline with the following copyright:

Copyright (C) 1991, 1992, 1993 by Chris Thewalt (thewalt@ce.berkeley.edu)

Permission to use, copy, modify, and distribute this software 
for any purpose and without fee is hereby granted, provided
that the above copyright notices appear in all copies and that both the
copyright notice and this permission notice appear in supporting
documentation.  This software is provided "as is" without express or
implied warranty.


