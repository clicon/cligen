/*
  CLIgen libFuzzer harness: full system fuzzer.

  ***** BEGIN LICENSE BLOCK *****

  Copyright (C) 2026 Olof Hagsand

  This file is part of CLIgen.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  Alternatively, the contents of this file may be used under the terms of
  the GNU General Public License Version 2 or later (the "GPL"),
  in which case the provisions of the GPL are applicable instead
  of those above. If you wish to allow use of your version of this file only
  under the terms of the GPL, and not to allow others to
  use your version of this file under the terms of Apache License version 2, indicate
  your decision by deleting the provisions above and replace them with the
  notice and other provisions required by the GPL. If you do not delete
  the provisions above, a recipient may use your version of this file under
  the terms of any one of the Apache License version 2 or the GPL.

  ***** END LICENSE BLOCK *****

  This harness drives the same full pipeline a real application such as cligen_tutorial uses:

    1. Parse a real, unmodified .cli spec file (tutorial.cli by default)
       with clispec_parse_str().
    2. Bind real callback/expand/translate functions to it, exactly as
       cligen_tutorial.c's str2fn()/str2fn_exp()/str2fn_trans() do.
    3. Treat each '\n'-separated line of the fuzzer input as one user command
       and drive it through cliread_parse() + cligen_eval(), including the
       history-expansion hook, so real callbacks (hello, callback, letters,
       setprompt, changetree, quit, incstr, cli_expand_cb, ...) execute with
       fuzzer-controlled cvv/argv content.

  A session (handle) is fresh per input for clean AddressSanitizer/
  LeakSanitizer attribution, but the spec text itself is read from disk once
  in LLVMFuzzerInitialize() and reused, since re-reading the file on every
  call would dominate runtime.

  Because commands such as "change tree" (switch active tree) and "quit"
  (set exiting flag) mutate session state, and "recurse @tutorial" lets one
  line match arbitrarily deep, multiple lines of a single input are
  processed against accumulating session state -- this is the point: it
  exercises multi-command session behaviour that a single-shot unit fuzzer
  cannot reach.

  The callbacks' own diagnostic printing (eg "callback"'s dump of every cvv/
  argv entry) is off by default -- at fuzzing throughput it would dominate
  runtime for no coverage benefit -- and gated on FUZZ_SYSTEM_VERBOSE; the
  value extraction/formatting itself always runs regardless. Set it to see
  output when manually reproducing one input, eg a saved crash-*:
    FUZZ_SYSTEM_VERBOSE=1 ./fuzz_system crash-<sha1>

  Build: see build.sh (add fuzz_system to TARGETS).

  Run, e.g. from fuzz/libfuzzer/ (tutorial.cli found via ../../tutorial.cli):
    ./fuzz_system corpus_system -dict=tutorial.dict

  Point at a different spec file (must define a "tutorial" tree, or adjust
  the ACTIVE_TREE define below):
    CLIGEN_FUZZ_SPEC=/path/to/other.cli ./fuzz_system corpus_system
*/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cligen/cligen.h>

/* Name of the tree in the spec file to activate and start feeding input to.
 * Must match a "treename=..." assignment in the loaded spec. */
#define ACTIVE_TREE "tutorial"

/* Hard cap on lines processed from a single fuzzer input, so that a large
 * input (eg all newlines) cannot blow up runtime; -max_len already bounds
 * input size, this is a second, cheap belt-and-braces bound. */
#define MAX_LINES 32

/* Candidate locations for tutorial.cli, tried in order, relative to the
 * directory the fuzzer binary is invoked from. The default (per build.sh /
 * README.md) is to run from fuzz/libfuzzer/, ie two levels below the repo
 * root where tutorial.cli lives. */
static const char *spec_candidates[] = {
    "tutorial.cli",
    "../tutorial.cli",
    "../../tutorial.cli",
    "../../../tutorial.cli",
    NULL
};

/* Spec text, read once in LLVMFuzzerInitialize() and reused for every
 * input; clispec_parse_str() does not modify it. */
static char *g_spec = NULL;

/* Diagnostic printing in the reused tutorial callbacks (below) is off by
 * default: at fuzzing throughput (thousands of execs/sec) it would dominate
 * runtime for no coverage benefit. Set FUZZ_SYSTEM_VERBOSE=1 to see it when
 * manually reproducing a specific input (eg a crash-* file). The value
 * extraction/formatting the callbacks do (cv2str, cv_int32_get, ...) always
 * runs regardless -- only the actual write to stdout/stderr is gated. */
static int g_verbose = 0;

/*! Read a whole file into a NUL-terminated malloc:d buffer
 *
 * @param[in]  path  File to read
 * @retval     buf   Malloc:d NUL-terminated file content, free with free()
 * @retval     NULL  File could not be opened or read
 */
static char *
read_file(const char *path)
{
    FILE *f = NULL;
    char *buf = NULL;
    long  len;

    if ((f = fopen(path, "rb")) == NULL)
        return NULL;
    if (fseek(f, 0, SEEK_END) < 0)
        goto done;
    if ((len = ftell(f)) < 0)
        goto done;
    if (fseek(f, 0, SEEK_SET) < 0)
        goto done;
    if ((buf = malloc((size_t)len + 1)) == NULL)
        goto done;
    if (len > 0 && fread(buf, 1, (size_t)len, f) != (size_t)len){
        free(buf);
        buf = NULL;
        goto done;
    }
    buf[len] = '\0';
 done:
    fclose(f);
    return buf;
}

/*
 * The callback, expand and translate functions below are the same
 * implementations cligen_tutorial.c registers for tutorial.cli, duplicated
 * here so this harness is self-contained (does not link cligen_tutorial.c
 * and its conflicting main()). Keep in sync with cligen_tutorial.c if the
 * spec's callback names change.
 */

static int
hello(cligen_handle h,
      cvec         *cvv,
      cvec         *argv)
{
    cg_var *cv;

    cv = cvec_i(argv, 0);
    if (g_verbose)
        cligen_output(stdout, "%s\n", cv_string_get(cv));
    return 0;
}

static int
callback(cligen_handle h,
         cvec         *cvv,
         cvec         *argv)
{
    int     i = 0;
    cg_var *cv;
    char    buf[64];

    if (g_verbose)
        cligen_output(stderr, "function: %s\n", cligen_fn_str_get(h));
    cv = NULL;
    while ((cv = cvec_each(cvv, cv)) != NULL) {
        cv2str(cv, buf, sizeof(buf)-1);
        if (g_verbose)
            cligen_output(stderr, "\t%d name:%s type:%s value:%s\n",
                          i, cv_name_get(cv), cv_type2str(cv_type_get(cv)), buf);
        i++;
    }
    if (argv){
        cv = NULL;
        i = 0;
        while ((cv = cvec_each(argv, cv)) != NULL) {
            cv2str(cv, buf, sizeof(buf)-1);
            if (g_verbose)
                cligen_output(stderr, "arg %d: %s\n", i, buf);
            i++;
        }
    }
    return 0;
}

static int
letters(cligen_handle h,
        cvec         *cvv,
        cvec         *argv)
{
    char   *str;
    cg_var *cv;

    if ((str = cvec_find_str(cvv, "ca")) != NULL && g_verbose)
        cligen_output(stdout, "%s\n", str);
    if ((cv = cvec_find(cvv, "int")) != NULL && g_verbose)
        cligen_output(stdout, "%d\n", cv_int32_get(cv));
    if ((str = cvec_find_str(cvv, "cb")) != NULL && g_verbose)
        cligen_output(stdout, "%s\n", str);
    if ((str = cvec_find_str(cvv, "dd")) != NULL && g_verbose)
        cligen_output(stdout, "%s\n", str);
    if ((str = cvec_find_str(cvv, "ee")) != NULL && g_verbose)
        cligen_output(stdout, "%s\n", str);
    return 0;
}

static int
secret(cligen_handle h,
       cvec         *cvv,
       cvec         *argv)
{
    cg_var *cv;

    cv = cvec_i(argv, 0);
    if (g_verbose)
        cligen_output(stdout, "This is a hidden command: %s\n", cv_string_get(cv));
    return 0;
}

static int
setprompt(cligen_handle h,
          cvec         *cvv,
          cvec         *argv)
{
    char *str;

    if ((str = cvec_find_str(cvv, "new")) != NULL)
        cligen_prompt_set(h, str);
    return 0;
}

static int
quit(cligen_handle h,
     cvec         *cvv,
     cvec         *argv)
{
    cligen_exiting_set(h, 1);
    return 0;
}

static int
changetree(cligen_handle h,
           cvec         *cvv,
           cvec         *argv)
{
    cg_var *cv;
    char   *treename;

    cv = cvec_i(argv, 0);
    treename = cv_string_get(cv);
    return cligen_ph_active_set_byname(h, treename);
}

static int
unknown(cligen_handle h,
        cvec         *cvv,
        cvec         *argv)
{
    cg_var *cv = cvec_i(cvv, 0);

    if (g_verbose)
        cligen_output(stderr, "The command has no assigned callback: %s\n", cv_string_get(cv));
    return 0;
}

static cgv_fnstype_t *
str2fn(const char *name,
       void       *arg,
       char      **error)
{
    *error = NULL;
    if (strcmp(name, "hello") == 0)
        return hello;
    else if (strcmp(name, "cb") == 0)
        return callback;
    else if (strcmp(name, "add") == 0)
        return callback;
    else if (strcmp(name, "del") == 0)
        return callback;
    else if (strcmp(name, "fn") == 0)
        return callback;
    else if (strcmp(name, "letters") == 0)
        return letters;
    else if (strcmp(name, "secret") == 0)
        return secret;
    else if (strcmp(name, "setprompt") == 0)
        return setprompt;
    else if (strcmp(name, "quit") == 0)
        return quit;
    else if (strcmp(name, "changetree") == 0)
        return changetree;
    return unknown; /* allow any function name (for testing) */
}

static int
cli_expand_cb(cligen_handle h,
              const char   *fn_str,
              cvec         *cvv,
              cvec         *argv,
              cvec         *commands,
              cvec         *helptexts)
{
    cvec_add_string(commands, NULL, "eth0");
    cvec_add_string(commands, NULL, "eth1");
    cvec_add_string(helptexts, NULL, "Interface A");
    cvec_add_string(helptexts, NULL, "Interface B");
    return 0;
}

static expand_cb *
str2fn_exp(const char *name,
           void       *arg,
           char      **error)
{
    return cli_expand_cb;
}

static int
incstr(cligen_handle h,
       cg_var       *cv)
{
    char *str;
    int   i;

    if (cv_type_get(cv) != CGV_STRING)
        return 0;
    str = cv_string_get(cv);
    for (i = 0; i < (int)strlen(str); i++)
        str[i]++;
    return 0;
}

static translate_cb_t *
str2fn_trans(const char *name,
             void       *arg,
             char      **error)
{
    if (strcmp(name, "incstr") == 0)
        return incstr;
    return NULL;
}

static int
history_expanded_cb(cligen_handle h,
                     const char   *cmd,
                     const char   *cmd_expanded,
                     void         *arg)
{
    if (strcmp(cmd, cmd_expanded) != 0 && g_verbose)
        cligen_output(stderr, "# Expanded: '%s' -> '%s'\n", cmd, cmd_expanded);
    return 0;
}

/*! One-time setup: locate and read tutorial.cli (or CLIGEN_FUZZ_SPEC)
 */
int
LLVMFuzzerInitialize(int    *argc,
                     char ***argv)
{
    const char *envpath;
    const char *verbose;
    size_t      i;

    (void)argc;
    (void)argv;
    if ((verbose = getenv("FUZZ_SYSTEM_VERBOSE")) != NULL && *verbose != '\0' && strcmp(verbose, "0") != 0)
        g_verbose = 1;
    if ((envpath = getenv("CLIGEN_FUZZ_SPEC")) != NULL){
        if ((g_spec = read_file(envpath)) != NULL)
            return 0;
        fprintf(stderr, "fuzz_system: CLIGEN_FUZZ_SPEC=%s: cannot read\n", envpath);
    }
    for (i = 0; spec_candidates[i] != NULL; i++){
        if ((g_spec = read_file(spec_candidates[i])) != NULL)
            return 0;
    }
    fprintf(stderr,
            "fuzz_system: cannot find tutorial.cli.\n"
            "Run from fuzz/libfuzzer/ (the normal case), or point at a spec\n"
            "explicitly with: CLIGEN_FUZZ_SPEC=/path/to/tutorial.cli %s ...\n",
            (argv && *argv && (*argv)[0]) ? (*argv)[0] : "fuzz_system");
    abort();
    return -1; /* not reached */
}

/*! Parse the cached spec into a fresh handle and bind callbacks to every tree
 *
 * @retval  0  OK, ACTIVE_TREE is now the active parse tree in h
 * @retval -1  Error (spec failed to parse or bind -- should not happen since
 *             the same text is validated once already in practice)
 */
static int
setup_session(cligen_handle h)
{
    int      retval = -1;
    cvec    *globals = NULL;
    pt_head *ph;
    parse_tree *pt;
    char    *str;

    if ((globals = cvec_new(0)) == NULL)
        goto done;
    if (clispec_parse_str(h, g_spec, "tutorial.cli", NULL, NULL, globals) < 0)
        goto done;
    ph = NULL;
    while ((ph = cligen_ph_each(h, ph)) != NULL) {
        pt = cligen_ph_parsetree_get(ph);
        if (cligen_callbackv_str2fn(pt, str2fn, NULL) < 0)
            goto done;
        if (cligen_expand_str2fn(pt, str2fn_exp, NULL) < 0)
            goto done;
        if (cligen_translate_str2fn(pt, str2fn_trans, NULL) < 0)
            goto done;
    }
    if (cligen_ph_active_set_byname(h, ACTIVE_TREE) < 0)
        goto done;
    if ((str = cvec_find_str(globals, "prompt")) != NULL)
        cligen_prompt_set(h, str);
    if ((str = cvec_find_str(globals, "comment")) != NULL)
        cligen_comment_set(h, *str);
    cligen_hist_fn_set(h, history_expanded_cb, NULL);
    retval = 0;
 done:
    if (globals)
        cvec_free(globals);
    return retval;
}

/*! Drive one line of user input through the real parse+match+eval pipeline
 *
 * Mirrors cliread_eval() (cligen_read.c) minus the terminal read, so that a
 * fuzzer-supplied line -- rather than a line typed at a tty -- is what gets
 * matched and, on a match, actually executed via cligen_eval().
 */
static void
run_line(cligen_handle h,
         char         *line)
{
    parse_tree    *pt;
    cg_obj        *matchobj = NULL;
    cvec          *cvv = NULL;
    cligen_result  result = CG_NOMATCH;
    char          *reason = NULL;

    if ((pt = cligen_pt_active_get(h)) == NULL)
        return;
    if (cliread_parse(h, line, pt, &matchobj, &cvv, &result, &reason) < 0)
        goto done;
    if (result == CG_MATCH){
        (void)hist_expand_callback(h, line, cvv);
        (void)cligen_eval(h, matchobj, cvv);
    }
 done:
    if (matchobj)
        co_free(matchobj, 0);
    if (cvv)
        cvec_free(cvv);
    if (reason)
        free(reason);
}

int
LLVMFuzzerTestOneInput(const uint8_t *data,
                       size_t         size)
{
    cligen_handle h = NULL;
    char         *buf = NULL;
    char         *line;
    char         *saveptr = NULL;
    int           nlines = 0;

    if ((buf = malloc(size + 1)) == NULL)
        return 0;
    memcpy(buf, data, size);
    buf[size] = '\0';

    if ((h = cligen_init()) == NULL)
        goto done;
    if (setup_session(h) < 0)
        goto done;

    line = strtok_r(buf, "\n", &saveptr);
    while (line != NULL && nlines < MAX_LINES){
        run_line(h, line);
        nlines++;
        if (cligen_exiting(h))   /* "quit" was invoked: end the session */
            break;
        line = strtok_r(NULL, "\n", &saveptr);
    }
 done:
    if (h)
        cligen_exit(h);          /* frees parse trees, cvecs, etc built into h */
    free(buf);
    return 0;
}
