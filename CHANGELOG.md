# Cligen Changelog
	
## 4.5.0 (Expected: May 2020)
* C API changes
  * `cligen_nomatch()` replaced by `reason` function parameter.
  * `cligenread_parse()` and `cliread_eval()` added `reason` and `result` parameter. The latter replaces the return values
  * `cliread()` added return string as call-by-reference and changed return value to standard-style 0/-1.
* Fixed [https://github.com/clicon/clixon/issues/106](simple list model in yang is broken in cligen)

## 4.4.0 (3 April 2020)
* CLIgen buffer functions and API changes
  * A CLIgen buffer now starts at a "start" size and grows quadratic (2x) up to a "threshold" after it grows linearly with "threshold".
  * Global start and threshold limit can be set and read with `cbuf_alloc_set()` and `cbuf_alloc_get()`, (signature changed).
  * New function `cbuf_new_alloc()` to start from another (individual) buffer size.
* API-change
  * `cligen_print()` replaced by `pt_print()` 
  * `cligen_print_obj()` replaced by `co_print()`
  * Size functions for memory analysis of cv and cvecs: `cv_size()` and `cvec_size()`.
* BugFix: Negative uint:s get parse error messages as if they are uint64, such as when uint8 parses -1.
* C-API change: Renamed `cligen_match_cgvar_same(int flag)` to `cligen_preference_mode_set(cligen_handle h, int flag)`

## 4.3.0 (1 January 2020)
Bugfixes and internal struct reorganizing. Bumped version to match with clixon 4.3

* Experimental syntax `@{}` for expressing _sets_ of commands, as opposed to alternative commands.
  * Not enabled by default. Enable by defining `USE_SETS` in cligen_custom.h
  * Example: `@{a;b;c;}` where a, b, and c can occur at most once in any order, as opposed to either a, b, or c.
  * Note that subtrees are not sets aware, ie `@{a;b;c;} d;` means the set a,b,c OR d, not the set a,b,c FOLLOWED by d.
  * This was only possible previously by workarounds or by full command expansion consuming quadratric memory.
  * This feature has been a root of many issues, including the following:
    * [Syntax to enter commands regardless of order #32](https://github.com/olofhagsand/cligen/issues/32)
    * [CLIgen code stuck in recursion at the time of parsing #23](https://github.com/olofhagsand/cligen/issues/23)
    * [CLI syntax help #22](https://github.com/olofhagsand/cligen/issues/22)
* Fixed [Ability to use show attribute to display a multiword string #33](https://github.com/olofhagsand/cligen/issues/33)
* Fixed: [Is it possible to hide non-terminal commands? #31](https://github.com/olofhagsand/cligen/issues/31)
* Removed "auth" as a local variable in docs, since it is not implemented
* C-API change:
  * `cligen_print()` renamed to `pt_print()`
  * `cligen_print_obj()` renamed to `co_print()`
* C Restructure of cligen-object (cg_obj) struct, for simplicity and smaller footprint:
  * Moved `co_delimiter` to cligen_handle
  * Added a generic `co_flags` field with `CO_FLAGS_*` constants
  * Moved `co_hide`, `co_mark`, `co_treeref`, and `co_refdone` to `co_flags`
  * Removed `co_userdata` since it is not used.

## 4.0.1 (18 August 2019)
No new functionality, just a release number bump to synchronize with Clixon

## 4.0.0 (13 July 2019)

* Added enable/disable as alternative boolean truth values on input
  * On request by dave@netgate.
  * Added header constants to enable disable on/off and enable/disable as alternatives
* Error messages for invalid number ranges and string lengths have been uniformed and changed.
  * Error messages for invalid ranges are now on the form:
  ```
    Number 23 out of range: 1-10
    String length 23 out of range: 1-10
  ```
  * Bug fix so that the above message is shown instead of "Unknown command" in several places
* Allow NULL as name in a cvec, for cvec_find and cvec_add
* Experimental: do not expand on non-interactive operations
  * Set EXPAND_ONLY_INTERACTIVE to enable
* Added support for the "void" type, so you can assign e.g. pointers.
* Added support for multiple regexps as well as "inverted" regexps
  * Example: `<name:string regexp:"[a-zA-Z]+" regexp:!"cli.*">;`
* Added XSD regexp w libxml2 support
  * Added libxml2 regex compile and exec functions
  * Added libxml2 to configure, enable it at install-time with:
    * `./configure --with-libxml2`	
  * Enable libxml2 based regexp:s at program start with: `cligen_regexp_xsd_set(h, 1)`
* [Choice with space is not working in CLIgen code](https://github.com/olofhagsand/cligen/issues/24) is fixed, but you need to use escape backslash character `\` to make it work.
  * Example using spec: `choice <string choice:nospace|with\ space>;`
    ```
    cli> choice ?
    nospace                   with\ space              
    cli> choice with\ space
    cli>
    ```
* Fixed: [Cligen history does not work well with multi line commands #26](https://github.com/olofhagsand/cligen/issues/26)
  * Also fixed truncated log commands when loading history files
* Handled clispec cornercase: `a("b"")` (where 'b"' is a comment)
* Allowed empty choice statements
* Made Makefile concurrent so that it can be compiled with -jN
* Removed CLIGEN_COMPAT_INT (finally)
  * `int`, `number`, and `long`must be replaced with int32 or other integer type
* Exposed `cligen_tonum()` for external usage
* Added saving of CLI command history between sessions on file and some getline restucturing:
  * Design is inspired by bash history with some constraints:
    * The API will load/save its complete history to an open file
    * The size (number of lines) of the file is the same as `hist_size`.
    * Only the latest session dumping its history will survive (bash merges multiple session history).
  * Renamed getline.c to cligen_getline.c
  * Split out the history part of getline.c to cligen_history.c
  * Added three API functions for history and history persistence: `cligen_hist_init`, `cligen_hist_file_load` and `cligen_hist_file_save`.
* Added support for multiple range and length statements for number and string types. This means thatthe following is allowed, for example:
  ```
  <s:string length[2:4] length[8:12]>
  <x:int8 range[-12:80] range[100:110]>>
  ```
  * See [yang type range statement does not support multiple values](https://github.com/clicon/clixon/issues/59)

## 3.9.0 (21 Feb 2019)
* Added cligen_utf8_set/get functions with default 0 to ignore UTF-8, set to 1 for _experimental_ UTF-8 mode.
* Fixed: (feature request: make cligen unicode safe)[https://github.com/olofhagsand/cligen/issues/21]
* Fixed: [serial terminals line wrap at 21 characters](https://github.com/olofhagsand/cligen/issues/20)
  * If no terminal (eg serial) set the terminal width to a large number effectively disabling scrolling behaviour.
* Renamed terminal setting functions:
  * cligen_terminal_length -> cligen_terminal_width
  * cligen_terminal_length_set -> cligen_terminal_width_set
  * cligen_terminalrows -> cligen_terminal_rows
  * cligen_terminalrows_set -> cligen_terminal_rows_set
* Changed default cbuf start buf from 8K->1K.
  * Added functions to get and set this limit: cbuf_alloc_(get|set).
* Added support for ISO 8601 UTC timezone designators Z or +/- for the cligen variable type `time`.
  * CLIgen did not use timezone and omission of UTC timezone designators is invalid
  * Example of a proper UTC time syntax is: 2008-09-21T18:57:21.003456Z
* Added cv_min_set() and cv_max_set() to set min and max values of a cligen variable respectively
* Added cligen_print_trees() function
* Fixed problem reported by mgsmith at netgate.com where two files loaded will not call callback after "a b" command:
	```
	a b c, fn();
	a b, fn();

	CLICON_MODE="master";
	CLICON_PLUGIN="example_cli";
	a b, fn();
	```

## 3.8.0 (6 Nov 2018)
* Merged a large number of Netgate commits. Thanks!
* Moved hidden C structures from .c files to internal .h headers
  * Requested by Dcarnejo in https://github.com/olofhagsand/cligen/issues/15
  * Four new internal headers created: cligen_cv_internal.h, cligen_cvec_internal.h, cligen_buf_internal.h and cligen_handle_internal.h
* Renamed cligen_var.[ch] to  cligen_cv.[ch] 
* Restored cvec_find_var (as requested by Matt Smith Netgate)

## 3.7.0 (20 July 2018)

* Fixed bug that REST variable did not work with regexp (thanks David Cornejo, Netgate)
  * For example this now works: <a:rest regexp:".*">;
* Added three CLIGEN_TABMODE_* flags for setting with cligen_tabmode_set:
  * CLIGEN_TABMODE_COLUMNS: 0: short/ios mode, 1: long/junos mode 
  * CLIGEN_TABMODE_VARS:    0: command preference, 1: vars have equal pref
  * CLIGEN_TABMODE_STEPS:   0: complete single step. 1: all steps at once
  * Deprecated `cligen_completion_set() and cligen_completion()`, use cligen_tabmode() instead.
* Append arguments to reference callback
  * such as: @datamodel:example, cli_show_auto("candidate", "text");		   
* Added --enable-debug to configure. Thanks rbgarga
* Removed support for single callback functions
  * Enable by defining CALLBACK_SINGLEARG
* Added variable translator function.
  * see cligen issue https://github.com/olofhagsand/cligen/issues/8
  * thanks dcornejo.
  * Example in tutorial: type increment HAL -> increment IBM
* Added new cligen_print_obj() function to print CLIgen syntax object (not just parse-trees).
* Fixed issues with mixed variables, choices and commands, see https://github.com/olofhagsand/cligen/issues/5. Thanks Nie WeiYang for detecting this.
* Removed support for type "int".  Replace with int8, int16, int32 or int64.
* Removed pre-R3.6.0 expand functions. Only multi-argument callback CLI expand
  functions supported, eg cligen_expandv_str2fn()

* Fuzzing performed using AFL. Following errors found and corrected:
  * Check for null arg in cligen_tutorial
  * Range check of search string in getline.c
  * buffer error in tab completion
  * dual free when recursive calls
  * Separated line buffer and kill buffer handling to indefinite length, with
    separet functions, eg cligen_buf_increase() +-> cligen_killbuf_increase().
  
* Fixed bug that appeared when expanding non-completed sub-strings. Eg "Interface eth0
  10.1.2. ?" could produce a cligen parse error and program termination
  in some circumstances. Detected by Netgate.

* Accept input with multiple lines into a single variable. (Thanks Matthew Smith)
	
## R3.6.0 (19 November 2017)

* Fixed newline for non-line scrolling mode

* Port to Mac / Apple Darwin

* New non-line scrolling mode in getline. Use non-scrolling with:
```
  cligen_line_scrolling_set(h, 0);
```

* Added signal SIGWINCH handling for window change size and adapting printing
  of help string to window resize. 

* Added support for multi-argument callback CLI expand functions.
    See cligen_expandv_str2fn()
	
* Added support for multi-argument callback CLI functions. See below where the third argument is a vector instead of a single argument.
  To use this, all CLI callback functions need to be rewritten.
  The old style remains but cannot be mixed with the new.
  See cligen_callback_str2fnv
```
    int callback(cligen_handle handle, cvec *cvv, cvec *argv);
```
	
* Removed alias int for int32.	
* Added option for "relaxed" handling of variable matching. So that 
  * eg <a:string length[4]> | <a:string length[40]> is allowed. 
  * See cligen_match_cgvar_same().

* type_max2str - get max value of a type

* Added doxygen reference and function call documentation. Just do 'make doc' 
  and direct your browser to doc/index.html

* Fixed parse bugs for some choices syntax, eg <a choice:1.0|length> did not work.

* Merge sub-trees with original trees containing same symbols. Eg a cligen-spec:
	"a @TREE; a b;"
  if TREE contains the symbol 'b', the two 'b:s' were not merged properly.

* Allow CR/LF in help-texts

## R3.5.0 (December 2014)

* Extended type system with signed and unsigned ints: int8, int16, int32, int64, uint8, uint16, uint32, uint64.
  * You need to to the following in your code:
  * Rename all cligen syntax types.
  - int      -> int32
  - number   -> int32
  * For example in cli syntax files: <x:int> -> <x:int32>.
  * 'int' will remain as an alias for int32.

* Rename types :
-	CGV_INT  -> CGV_INT32
-	CGV_LONG -> CGV_INT64
* Rename cv access functions:
-	 cv_int_get -> cv_int32_get
-	 cv_int_set -> cv_int32_set
-	 cv_long_get -> cv_int64_get
-	 cv_long_set -> cv_int64_set
  * For the new int types, see Section 5.1 in the CLIgen tutorial

* A new decimal64 type is added. Decimal64 defined in the YANG standard
(RFC6020) is a 64-bit integer with a fraction-index numbers defining a
number of decimals. For example, a decimal64 with 4 fraction-index is
written as: 123.4567. Ranges allowed for decimal64 as well.

* Autoheader support. clicon_config.h.in may be generated by autoheader,
but is still part of the release.

* Cligen variable vectors (cvec) extended with 'name'. New functions:
	char *cvec_name_get(cvec *vr);
	char *cvec_name_set(cvec *vr, char *name);

A new module: cligen_buf.[ch] is added. This adds a simple variable buffer allocation
(cbuf) functionality. Example:
	cbuf *cb = cbuf_new();
	cprintf(cb, "%d %s", 43, "go");
        write(1, cbuf_get(cb), cbuf_len(cb)) < 0)
        cbuf_free(cb);

The string type is extended with a length keyword. This adds the
possibility to limit the length of a string. Example: <s:string
length[2:10]>; # string length must bebetween 2 and 10 <s:string
length[10]>; # string length must be shorter than 10

Integer ranges can be defined with only an upper limit, lower limit can be 
suppressed: 
   <x:int32 range[125]>;

Added a 'show' field in the variable syntax to distinguish between a name of a variable used for reference and how it is displayed in the CLI:
	<x:int32 show:number>;
Here, 'x' is used to reference the variable in callbacks, but 'number' is used when displaying the variable.

Added support for multiple CLIgen trees in a single file. Example:
  treename="tree1";
  x {y;z;}
  treename="tree2";
  a {b;c;}

Added a function to get the calling callback function:
cligen_fn_str_get(). This may be useful in case the callback is not
known and is primarily used in the ongoing python port.

Empty list parser bug fixed: "a;{}" now supported, not only "a{}".

Parser bug fixed: "a;<a:int>" did not recognize the 2nd 'a', ie
commands and variables on the same level could not have the same name.

Made cligen compilable with g++.

R3.4.0 September 2013
=====================
Changed 'range' variable syntax, the old did not allow negative numbers.
NOTE: You need to change all variable syntax statements of the form:
    <a:int range:3-89>
to 
    <a:int range[3:89]>
The old syntax will be kept for some releases.

Setting of terminal length, not hardcode to 80:
   cligen_terminal_length_set(h, 160);

Added help-text and cligen variable vector to expanded variables. An
expand callback now has a 'vars' and 'helptext' argument:
NOTE: You need to change all expand callbacks in your user C-code:
   int expandcb(cligen_handle h, char *fn_str, cg_var *arg, 
                int *nr, char ***commands);
to
   int expandcb(cligen_handle h, char *fn_str, cvec *vars, cg_var *arg, 
                int *nr, char ***commands, char ***helptexts);
The helptext variable is set in the same way as 'commands'. If it is left empty, 
the help-text in the specification is used.
'vars' is a cligen variable vector of the command line. Same semantics as in cligen
calbacks.

cligen_print() now prints a complete parsable syntax when brief=0. It was
previously not complete for variables. It now prints also expand and regex.

Removed byacc support. It leaks memory and can't get the generated code to work.

Support for clang compiler

Added a userdata field for cligen_objects (co) to add app-specific
fields. The default field is removed and can be implemented by
userdata.

Added an empty variable type that has no values (thanks Benny)
	
Added pt_apply that applies a function call recursively on all cg_obj:s in a parse-tree

Added cligen_logsyntax[_set]() as a debug-function to track dynamic
cligen syntax, including tree-references and completion.

Removed the --gl-select configure option by making it mandatory.

R3_3_0 April 2013
=================

Variable syntax: '<' <name> 'type:'<type>'>' is no longer supported. 
Use instead: '<' <name>':'<type>'>'.
NOTE: You need to change all variable syntax statements of the form:
   <a type:int> 
to 
   <a:int> 

Syntax for referencing sub-trees (like function-calls) have been
introduced. The new reference operator is '@'. A subtree is unfolded
in the place of the reference, much like a function call or macro. A syntax
can even refence itself.
Example:
   T:
   a @T;
gives a recursive syntax on the form 'a a a a a ...'.

Three new variable types introduced: bool, uuid, and (ISO) time. Examples:
   uuid: f47ac10b-58cc-4372-a567-0e02b2c3d479
   time: 2008-09-21T18:57:21.003456
   bool: true, false, on, off

Complete rewrite of cligen_var API. Complete rewrite of and rename of
varrec->cvec API.
Both types are now hidden C-variables using handles and accessed via
get/set functions. cvec being a vector of cv:s.
Example:
   cvec *vr;
   cg_var *cv = NULL;
   while ((cv = cvec_each(vr, cv)) != NULL) {
      str = cv_name_get(cv);
   }
See cligen_tutorial for a documentation of the API

R3_2_0 January 2012.
====================

New variable and expand syntax. Example:
 <name:int fn()>
Used to be: <name type:int expand:fn>
NOTE: You need to change all statements of the form:
<name:int expand:fn> to <name:int fn()>

List of callbacks. Example:
  a b, x("foo"), y("bar");
  a b, z("fie");
Both x, y and z will be called when 'a b' is typed.

regexp variables re-introduced. 
Example: 
  <name:string regexp:"[a-z]+[0-8]+\\.[0-9]">;
  <name:string regexp:"(ab|a)b*c">;

Refactoring (no API changes):
1. Parse-code - Merged the two yacc parsers into one. This gives
better performance but more importantly less complex code. Documented in
docs/block-chart.pdf. 
2. Search and insertion is made using binary search instead of linear. Load 30K syntax 
lines now takes 0.2s instead of 13.2s.

R3_1_0 2011-11-19
=================
NOTE: API changes (You need to search and replace in your code):
 cli_output -> cligen_output
 pt_print -> cligen_parsetree_print
 cligen_expand(h) -> cligen_expand(h, name)
 cligen_parsetree_set(h, pt) -> cligen_parsetree_add(h, name, pt)
                             -> cligen_parsetree_del(h, name)
 cligen_parsetree(h) -> cligen_parsetree(h, name)
 cligen_parse_*(str/f, name,...) -> cligen_parse(h, name, str/f,...)

New API functions:
  char *cligen_mode(cligen_handle ch);
  int cligen_mode_set(cligen_handle ch, char *mode);
  int cligen_tabmode(cligen_handle ch);
  int cligen_tabmode_set(cligen_handle ch, int mode);

R3_0_3 2013-06-30 (branched)
=================
Added help-text to expanded variables. An expand callback now has a 'comments' argument:
NOTE: You need to change all expand callbacks in your user C-code:
   int expandcb(cligen_handle h, char *fn_str, cg_var *arg, int *nr, char ***commands);
to
   int expandcb(cligen_handle h, char *fn_str, cg_var *arg, int *nr, char ***commands, char ***comments);
The comments variable is set in the same way as 'commands'. If it is left empty, 
the help-text in the specification is used.

Setting of terminal length, not hardcode to 80:
cligen_terminal_length_set(h, 160);

Support for clang, and copt as config-option

R3_0_2 2011-10-27
=================
url was limited: protocol only allowed some, and trailing slash was mandatory
vt100 application cursor bug disabling arrow keys

R3_0_1 2011-10-19
=================
Callbacks signatures have changed to:
  int cb(void *handle, varrec args, cg_var *arg)
CLI specification syntax has radically changed. Example:
  prompt=foo;
  command1("helptext"){
    sub1("help"),callback(arg);
    sub2, hide;
  }
New handles and cleaned up API.
More or less complete manual.

R3_0_0 2011-10-0
================
Pre- R3.0 version. dont use

R2_0
====
Stable version.

R1_0
====
GPL version available at http://www.nada.kth.se/~olofh (~2004).

