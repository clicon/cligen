# Cligen Changelog

* [7.5.0](#750) 29 July 2025
* [7.4.0](#740) 3 April 2025
* [7.3.0](#730) 30 January 2025
* [7.2.0](#720) 28 October 2024
* [7.1.0](#710) 3 July 2024
* [7.0.0](#770) 8 March 2024
* [6.5.0](#650) 5 December 2023
* [6.4.0](#640) 30 September 2023
* [6.3.0](#630) 29 July 2023
* [6.2.0](#620) 30 April 2023
* [6.1.0](#610) 19 Feb 2023
* [6.0.0](#600) 29 Nov 2022

## 7.5.0
29 July 2025

### Features

* Added label support in combination with tree references
  * Example: `@tree, @add:<label>

### Corrected Bugs

* Fixed: [Space needed before pipes](https://github.com/clicon/cligen/issues/103)

## 7.4.0
3 April 2025

### Corrected Bugs

* Fixed: [Expand of variables broken in 7.3](https://github.com/clicon/cligen/issues/127)

## 7.3.0
30 January 2025

### Corrected Bugs

* Fixed: Added parenthesis as valid clispec choice tokens
* Fixed: [Expansion removes the double quote](https://github.com/clicon/clixon/issues/524)
  * Remove auto-escaping of expanded nodes
  * (Internal:) remove co_value and storing of non-escaped command

## 7.2.0
28 October 2024

### Features

* New version string on the form: `7.1.0-1+11+g2b25294`
* Deb build script
* New: [CLI simple alias](https://github.com/clicon/cligen/issues/112)

### Corrected Bugs

* Fixed: [CLI help text for YANG data model with multi-line description field is misformatted](https://github.com/clicon/clixon/issues/543)

## 7.1.0
3 July 2024

### Features

* Added history callbacks API: `cligen_hist_fn_set`
  * Added history subsection in tutorial
* Changed intermediate version numbers to be git-style, eg `7.0.0-39` instead of `7.1.0-PRE`

## 7.0.0
8 March 2024

### Minor items

* Added new cligen_paging_get/set for a direct way to disable/enable output paging/scrolling.
* Made coverity analysis and fixed most of them
  * Some were ignored being for generated code (eg lex) or not applicable
* Added vcprintf

## 6.5.0
5 December 2023

### Corrected Bugs

* Fixed: [Space after comma not allowed in pre-function parsing](https://github.com/clicon/cligen/issues/101)

## 6.4.0
30 September 2023

### C/CLI-API changes on existing features
Developers may need to change their code

* Changed `tv` parameter of `time2str()` from struct to a pointer.
  * All calls to `time2str()` need to be converted.
* Removed obsolete functions `pt_name_get()` / `pt_name_set()`

### Corrected Bugs

* Output pipes released in 6.3 have several minor bugfixes
* Fixed: [CLI show config | display <format> exits over mountpoints with large YANGs](https://github.com/clicon/clixon-controller/issues/39)
  * Removed timeout for pipes and enabled ctrl-C
* Fixed: [Escaping with backslash(\) in variable strings cannot be done for backslash itself](https://github.com/clicon/cligen/issues/96)

## 6.3.0
29 July 2023

### New features
* Output pipes
  * Callbacks with name starting with `|` is a pipe tree containing pipe output function
  * Use 'pipetree="|<pipe-tree>"` to reference a default pipe-tree.
  * For more info:
    * CLIgen tutorial Section 2.8
    * [Best way to implement output pipes?](https://github.com/clicon/cligen/issues/19)

### Minor features
* Added callback_argument access functions for keeping track of api-paths for recursive expands
  * This is to enable memory footprint optimization in higher layers (clixon)
* Removed  extras/ build-root/ and rpm build code since they are not properly maintained

### C/CLI-API changes on existing features
Developers may need to change their code

* Refactoring of tree expansion code,
  * Most of these API changes are internal and should not affect external API (but may):
  * `cligen_eval()`: Removed `callback` parameter
  * `cliread_parse()`: Removed `callback` parameter
  * Renamed co flag: `CO_FLAGS_TREEREF` to `CO_FLAGS_TOPOFTREE`
* Renamed `cligen_parse` functions and added treename parameter:
  * Renamed `cligen_parse_str` to `clispec_parse_str` and `cligen_parse_file` to `clispec_parse_file`
  * Added `treename` parameter, default `NULL`

## 6.2.0
30 April 2023

### Minor features

* Added wrapper callback for accessing treerefs, in an effort to make them dynamical/programmable

### Corrected Bugs

* Fixed: [Giving empty string as value for integer variable exists the CLI shell](https://github.com/clicon/cligen/issues/85)

## 6.1.0
19 february 2023

### Minor features

* Make cligen_* functions const by @dima1308 in https://github.com/clicon/cligen/pull/83
* Eliminate linear search for active parse tree by @rcmcdonald91 in https://github.com/clicon/cligen/pull/84
* Added print functions for VOID type using printf `%p`

### Corrected Bugs

* Fixed: [datamodel tree generated from basemodel tree is not proper when a list has more than one key and key is of enum type](https://github.com/clicon/clixon/issues/417)

## 6.0.0
29 November 2022

### Minor features

* [Code formatting: Change indentation style to space](https://github.com/clicon/clixon/issues/379)
  * Applies to all c/h/y/l/sh files and .editorconfig

### Corrected Bugs

* Fixed: [More specific error shall be provided when there is a type def on top of default types](https://github.com/clicon/clixon/issues/319)

## 5.8.0
28 July 2022

### New Features

* The Variable preference implementation has been updated and documented
  * If more than one variable have the same preference, an "Ambiguos command" error is returned
  * Such tiebreaks can be resolved by a new "preference" keyword, eg: `<string preference:20>`
  * There is also an extended API: `cligen_preference_mode_set()`
  * This change may affect existing code in the following ways:
    * A string variable with "expand()" function has higher preference than without expand function
    * `Ambiguous command` error is returned in more cases
  * More info: Section 3.14 of the CLIgen tutorial

### Corrected Bugs

* [Expansion does not work properly for inner hierarchical nodes on pressing TAB](https://github.com/clicon/clixon/issues/332)

## 5.7.0
17 May 2022

### Corrected Bugs

* Fixed: Preference mode (used by clixon) collapsed all `<var>`:s into one to avoid ambiguity.
* Fixed: `cligen_output` wrap problem: word dropped if exactly on line width
* Fixes to make `<rest>` variables with delimiters work better
  * This includes expansion, re-adding, etc that had several flaws
* Fixed: [CLI combination of cligen choice and expand dbvar gives ambiguous command](https://github.com/clicon/clixon/issues/321)
* Fixed: [Bug that happens when having the same container name in .cli file and yang file](https://github.com/clicon/clixon/issues/309)
* Fixed again: CLIgen output scrolling did not work if lines were wrapped
  * Did not work if cligen_output was called without `\n`
* Fixed: [Compiling with -DNDEBUG results in failure of test applications.](https://github.com/clicon/cligen/issues/76)

## 5.6.0
8 March 2022

### Corrected Bugs

* Fixed: [Multiple commands with common beginning gets collapsed into one](https://github.com/clicon/cligen/issues/75)
* Fixed: CLIgen output scrolling did not work if lines were wrapped
* Fixed: `Ctlr R`(reverse-i-search) adds additional character C on pressing right arrow`: https://github.com/clicon/cligen/issues/74
* Fixed: Helpstrings were duplicated in conjunction with option `[]`
* Fixed: Duplicates in autocli expansion of leafs and treeref includes:
  * https://github.com/clicon/cligen/issues/73
  * https://github.com/clicon/clixon/issues/301
  * This was introduced in CLIgen 5.4

## 5.5.0
20 January 2022

### Changes

* Refactor and optimize tree-reference handling
  * Removed "deep" treeref copy which consumed lots of memory for large trees
    * In other words, use "shallow" copy instead so you can state things like: `@tree, @remove foo;`
  * Ensured indirect treeref functionality: eg tree referencing another tree:
    * `@t0; treeref="t0"; @t1;`
  * Removed `cligen_reftree_filter_get()/_set()` functions and `@delete:<label>` constructs
    * You can now only do eg: `ref @tree, @remove:<label>`
    * I.e., no default remove labels and no `@add:<label>` supported
* Removed `#ifdef __GNUC__` around printf-like prototypes since both clang and gcc have format/printf macros defined
* Removed `hide-database` and `hide-database-auto-completion` labels
  * They were no-ops, only `hide` has meaning.
  * C flag `CO_FLAGS_HIDE_DATABASE` removed
* Removed optional `CLIGEN_HELPSTRING_VEC` and `CLIGEN_HELPSTRING_SINGLE` from cligen_custom.h
  * Turns out when profiling a cvec consumes too much cpu cycles
* Added proper error message with line number when encountering errors in the CLIgen YACC parsing and its sub-routines, not only for LEX errors.
* Changed `pt_print()` signature to follow other print functions:
  * pt_print(FILE*, parsetree*)

### Corrected Bugs

* Fixed: clispec parser escaped double quote: `\"` was not parsed correctly
* Fixed: 5.4.0 introduced an issue with expand if exclude_keys is 0,
  * <tab> and <query> may return a constant instead of the variable in many cases
* Revisited [Check if cg_obj is optional #38](https://github.com/clicon/cligen/issues/38).
  * The orignal workaround/fix in [1e9964e](https://github.com/clicon/cligen/commit/1e9964ec5f58ca9a21c39063f8ee596ce4980376) to disallow multiple []: "Do not think this is a costraining fix" was wrong.
  * Fixed it by adding the CO_FLAGS_OPTION on any new cligen object created when within [].
  * Also added [] in the print function, although this is not complete.

## 5.4.0
30 November 2021

### Changes

* Performance changes of C code
  * Added a new expansion of treeref method that does not copy the whole tree but only necessary additions. The old method is still kept as default in cligen, using a new cligen_reftree_copy() API. The no-copy method reduces memory for large specs but does not work in tree recursions and @add/@remove labels.
    * Internally this led to a new callback argument that is passed in all match functions.
  * Rearranged and simplified expansions (at tab and ?) by making a single pt_expand1 API.
  * Added size functions for co callbacks, and changed free from single to list of callbacks
  * Changed CLI helpstrings back to use a single malloced string instead of vector
    * Added CLIGEN_HELPSTRING_VEC to cligen_custom.h, byt default off
  * Added eval wrap function where a higher layer can add hooks around a cligen callback call. See the new cligen_eval_wrap_fn() API.
  * Changed function name: cligen_ph_active_set -> cligen_ph_active_set_byname
* C API changes
  * cliread_parse() changed where fifth cvv parameter changed type from `cvec*` to `cvec**`.
  * Added flags in co/pt_copy functions.
* Changes on cvv callback options
  * Moved cv_exclude_keys() to cligen_exclude_keys_set() and created cvec_exclude_keys()
  * See cligen_tutorial Section 6.1
  * New: cligen_expand_first_get/set() and cvec_expand_first()
  * Merged the use of cvv and cvvall parameters into a single cvv parameter in match_pattern code
  * Solved [Get the expanded string of entire command executed even if entered string is partial](https://github.com/clicon/cligen/issues/65)
* Same commands with different help strings are now displayed separately
  * See [Help-text support for multiple types](https://github.com/clicon/cligen/issues/27)
* Added case-insensitive command matching
  * See [Case-Insensitive Command Matching](https://github.com/clicon/cligen/issues/46)
  * To set cligen ignore case, you must call `cligen_caseignore_set(h, 1)`
  * Note applications using cligen (eg clixon) may have case-sensitive code despite setting this
* Refactored Makefile for static linking

### Corrected Bugs

* Fixed: [very slow execution of load_set_file #288](https://github.com/clicon/clixon/issues/288)
  * Large refactoring was made noted elsewhere in this release
* Fix/Workaround:[Check if cg_obj is optional #38](https://github.com/clicon/cligen/issues/38)
  * Added a check to not allow double level of options, eg ``[[cc]]``. Do not think this is a constraining fix, (bit may be wrong).
* Fixed: [Performance issue when parsed string size is damn large #66](https://github.com/clicon/cligen/issues/66)

## 5.3.0
27 September 2021

### New Features

* Added filtering of tree references, using prefix `@add:` and `@remove`
  * Example, `@t, @remove:local` removes all nodes labelled with `local` in tree `t` before applying the tree "macro" expansion
  * Initialize filters with: `cligen_reftree_filter_set()`
  * See cligen tutorial "Filtering trees" section
* Added cbuf_trunc() as requested in [Yang patch pull request](https://github.com/clicon/clixon/pull/248)
* Added constant `CLIGEN_SINGLE_HELPSTRING` to show only single help-string
  * Some YANG models have very large comments and produces time-consuming memory allocation
  * Default: disabled
* Internal: replace empty placeholder in parse-tree (pt) vector with a co of type CO_EMPTY as indication of terminal
* Added `co_count()` function to keep track how many cligen objects have been allocated
* Added "prefix" field in cligen parse-object struct to prepare for namespace separation

## 5.2.0
1 July 2021

### New Features

* Moved CI from travis to github actions
* Two new hide variables added (thanks: shmuelnatan)
  * hide-database : specifies that a command is not visible in database. This can be useful for setting passwords and not exposing them to users.
  * hide-database-auto-completion : specifies that a command is not visible in database and in auto completion. This can be useful for a password that was put in device by super user, not be changed.

### Corrected Bugs

* Fixed: Error in `cbuf_append_buf`: did not work for more than one call.
* Fixed: [Auto completion does not work for strings including space. Seen only when autocompletion is strict and is via customary callback in .cli file #221](https://github.com/clicon/clixon/issues/221)
* Fixed: [Auto Completion does not display options on pressing TAB for inner hierarchical nodes . #208](https://github.com/clicon/clixon/issues/208)

## 5.1.0
15 April 2021

### Corrected Bugs

* Fixed: [cli will hang in match_complete with CLICON_CLI_TAB_MODE equal 4 #196](https://github.com/clicon/clixon/issues/196)
* Changed winsize default to: non-scrolling, term width 80

## 5.0.0
26 February 2021

Mainly a release to keep in sync with Clixon major new 5.0 release

### New Features

* [Simplifying error messages for regex validations. #174](https://github.com/clicon/clixon/issues/174)

### Corrected Bugs

* Fixed CLI sets bug:
  * In syntax: `a @{ b b2; c; }`, the following did not work: `a c b b2 ?` (c was again possible)

* Fixed CLI translator bug

## 4.9.0
18 Dec 2020

### New features

* Fuzzing using AFL scripts added, see [fuzz](fuzz).
* Change comment character to be active anywhere to beginning of _word_ only.
  * See [Change CLIgen comments](https://github.com/clicon/cligen/issues/55)
* Support for building static lib: `LINKAGE=static configure`

### Corrected Bugs

* Fixed bug regarding number of lines in CLIgen history.
  * The line number given to `cligen_hist_init()` saved one less than the number given.
* Fixed [Using a regular expression with either ^ OR $ breaks the regular expression match logic](https://github.com/clicon/cligen/issues/53)
* Recursive sets did not work properly, especially completion and query.

## 4.8.0
18 October 2020

### New features

* Tree reference workpoint and mode support.
  * When using tree references it is possible to set an active _workpoint_ for that tree, which can change dynamically. In this way, a user can navigate up and down the tree in its references for it. This is useful when implementing automatic modes for example.

### C/CLI-API changes on existing features

Developers may need to change their code

* Code restructure: exposed "parse_tree_head" as a first class object that was previously hidden in cligen_handle code.
  * For example, `pt = cligen_tree_i` -> `ph = cligen_ph_i(h, 0); pt = cligen_ph_parsetree_get(ph);`

## 4.7.0
14 September 2020

### New features

* Changed help strings behaviour on query (?) for long and multi-line help strings.
  * If multiple strings (eg "\n" in text), indent before each new line
  * cligen_helpstring_truncate() to wrap or truncate long lines at right terminal width margin
  * cligen_helpstring_line() to limit number of help string lines
  * undefine CO_HELPVEC to revert (temporary)

## 4.6.0
10 August 2020

Again considerable refactoring has been made, with some API changes as
documented below.

### New features

* The `sets` functionality is a separate branch which is expected to take be the main branch in the next release.
* Changed cligen output row scrolling/pageing behaviour for raw terminals, such as serial consoles: if there is a tty but cannot determine window size, then `cligen_terminal_rows_set()` can be called to set a fixed page size.

### Changed API

* Refactored the CLIgen object and parsetree structure. The object
structure had a object-within-objct structure, where a CLIgen object
(cg_obj) contained a "parse-tree" object which in turn contains all
children. This is a work to make parse-tree a first level object for future enhancements.
  * Split cligen_gen.[ch] into cligen_object.[ch] and cligen_parsetree.[ch]
    * parsetree structure hidden in cligen_parsetree.c
  * Access macros replace direct structure access as follows:
    * `pt->pt_vec` --> `pt_vec_get(pt)`
    * `pt->pt_vec[i]` --> `pt_vec_i_get(pt, i)`
    * `pt->pt_len` --> `pt_len_get(pt)`
    * `co->co_max` --> `parse_tree *pt = co_pt_get(co); pt_len_get(pt, i)`
    * `co->co_next[i]` --> `parse_tree *pt = co_pt_get(co); pt_vec_i_get(pt, i)`
  * All functions taking call-by-value: `parse-tree pt` have been replaced by call-by-reference: `parse_tree *pt`.
    * This includes:  `cligen_tree_add(), cligen_help(), match_pattern(), pt_print(), cligen_callback_str2fn()`,  and many others.
* C-API: Renamed `pt_expand_2()` -> `pt_expand()`, removed backward compatible `pt_expand_1()`
* Corrected spelling mistake: "Ambigous" -> "Ambiguous".
  * This may affect error output

### Corrected Bugs

* Fixed: [Need to add the possibility to use anchors around patterns #51](https://github.com/clicon/cligen/issues/51):
  * checked if `^$` already around string, if so, do not add it.
* Fixed: [Double free when using libxml2 as regex engine #117](https://github.com/clicon/clixon/issues/117)
* Fixed: [Reading in a file exactly the same size as the buffer could leave the buffer not terminated with a 0 byte](https://github.com/clicon/cligen/pull/49)

## 4.5.0
12 May 2020

A refactoring release. Many have requested a "sets" functionality and
this was necessary in order to prepare for that going forward. Note
also a small (non-backward-compatible) API change.

* Refactoring of matching code in order to cleanup and make a core functions better structured and easier to add functionality. Should not change API:s, but it took some time getting the behaviour right. The following errors in clixon were related to these (Thanks [Rombie](https://github.com/rombie) for detecting and verifying these):
  * [Once an option is used, other options fail to appear in CLI](https://github.com/clicon/clixon/issues/107)
  * [Simple list model in yang is broken in cligen](https://github.com/clicon/clixon/issues/106)
* Added a test dir, see [test readme](test/README.md)
* C API changes (you need to update your C-code calling CLIgen)
  * Added new specialized cbuf function: `cbuf_append_str()`
  * `cligen_nomatch()` replaced by `reason` function parameter.
  * `cliread_parse()` and `cliread_eval()` added `reason` and `result` parameter. The latter replaces the return values
  * `cliread()` added return string as call-by-reference and changed return value to standard-style 0/-1.

## 4.4.0
3 April 2020

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

## 4.3.0
1 January 2020

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

## 4.0.1
18 August 2019

No new functionality, just a release number bump to synchronize with Clixon

## 4.0.0
13 July 2019

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

## 3.9.0
21 Feb 2019

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
## 3.8.0
6 Nov 2018

* Merged a large number of Netgate commits. Thanks!
* Moved hidden C structures from .c files to internal .h headers
  * Requested by Dcarnejo in https://github.com/olofhagsand/cligen/issues/15
  * Four new internal headers created: cligen_cv_internal.h, cligen_cvec_internal.h, cligen_buf_internal.h and cligen_handle_internal.h
* Renamed cligen_var.[ch] to  cligen_cv.[ch]
* Restored cvec_find_var (as requested by Matt Smith Netgate)

## 3.7.0
20 July 2018

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
-       CGV_INT  -> CGV_INT32
-       CGV_LONG -> CGV_INT64
* Rename cv access functions:
-        cv_int_get -> cv_int32_get
-        cv_int_set -> cv_int32_set
-        cv_long_get -> cv_int64_get
-        cv_long_set -> cv_int64_set
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

## R3.4.0
September 2013

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

## R3_3_0
April 2013

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

## R3_2_0
January 2012

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

## R3_1_0
2011-11-19

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

## R3_0_3
2013-06-30 (branched)

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

## R3_0_2
2011-10-27

url was limited: protocol only allowed some, and trailing slash was mandatory
vt100 application cursor bug disabling arrow keys

## R3_0_1
2011-10-19

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

## R3_0_0
2011-10-1

Pre- R3.0 version. dont use

## R2_0

Stable version.

## R1_0

GPL version available at http://www.nada.kth.se/~olofh (~2004).

