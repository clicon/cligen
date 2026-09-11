#
# Makefile for CLIgen libFuzzer harnesses
#
# ***** BEGIN LICENSE BLOCK *****
#
# Copyright (C) 2026 Olof Hagsand
#
# This file is part of CLIgen.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# ***** END LICENSE BLOCK *****
#
# Usage (from this directory):
#   make              - build all harnesses and seed corpora
#   make fuzz_cv      - build a single harness
#   make run_spec     - run fuzz_spec in the background (nohup)
#   make run_cv       - run fuzz_cv  in the background
#   make run_input    - run fuzz_input in the background
#   make run_system   - run fuzz_system in the background
#   make clean        - remove compiled harnesses
#
# The generated parser sources (cligen_parse.tab.c, lex.cligen_parse.c) must
# exist in the repo root before building; run './configure && make' there first.

TARGETS = fuzz_spec fuzz_cv fuzz_input fuzz_system
ARTIFACTS = artifacts_spec artifacts_cv artifacts_input artifacts_system

.PHONY: all clean run_spec run_cv run_input run_system

all: $(TARGETS)

clean:
	rm -rf $(TARGETS) $(ARTIFACTS)

$(TARGETS):
	./build.sh $@

$(ARTIFACTS):
	mkdir $@

run_spec: artifacts_spec fuzz_spec
	./fuzz_spec corpus_spec -dict=cligen.dict \
	    -fork=4 -ignore_crashes=1 -ignore_timeouts=1 -ignore_ooms=1 \
	    -max_len=4096 -rss_limit_mb=2048 -timeout=10 \
	    -artifact_prefix=artifacts_spec/

run_cv: artifacts_cv fuzz_cv
	./fuzz_cv corpus_cv -dict=cligen.dict \
	    -fork=4 -ignore_crashes=1 -ignore_timeouts=1 -ignore_ooms=1 \
	    -max_len=128 -rss_limit_mb=2048 -timeout=10 \
	    -artifact_prefix=artifacts_cv/

run_input: artifacts_input fuzz_input
	./fuzz_input corpus_input -dict=cligen.dict \
	    -fork=4 -ignore_crashes=1 -ignore_timeouts=1 -ignore_ooms=1 \
	    -max_len=512 -rss_limit_mb=2048 -timeout=10 \
	    -artifact_prefix=artifacts_input/

run_system: artifacts_system fuzz_system
	./fuzz_system corpus_system -dict=tutorial.dict \
	    -fork=4 -ignore_crashes=1 -ignore_timeouts=1 -ignore_ooms=1 \
	    -max_len=1024 -rss_limit_mb=2048 -timeout=10 \
	    -artifact_prefix=artifacts_system/
