cligen
======

CLIgen is a Command-Line Interface generator.

Well, actually it is not really a generator, since it does not
_generate_ code for CLI:s. Instead, it builds and interprets
datastructures (a parse-tree) which a library (libcligen) interprets
in runtime.  It is fast and efficient and helps you develop CLI:s
faster. You enter a CLI syntax either in a file or in-line in C, you
write callback functions and then you call the right library
routines. The callback functions add the semantics, that is, what the
commands in the CLI are supposed to do.

CLIgen is _not_ a system in itself, you need to build your own
'backend'. We have started another project: 'clicon' which is actually
a whole system where you load dynamic frontend and backend
modules. Clicon offers you a system, with embedded database, commit
semantics, netconf interface, etc. CLIgen is a part of clicon.

