# Security Policy

## Supported Versions

CLIgen's `master` branch is continuously tested by CI. CLIgen is released in
lockstep with [Clixon](https://github.com/clicon/clixon), roughly every third
month (see [Releases](https://github.com/clicon/cligen/releases)). Security
fixes are made against `master` and included in the next release; only the
latest release and `master` are supported.

## Reporting a Vulnerability

Please report security vulnerabilities privately by email to
**info@clicon.org**, rather than filing a public GitHub issue.

Include, if possible:
- A description of the vulnerability and its potential impact.
- Steps to reproduce, or a minimal example (eg a CLIgen spec file and input)
  that triggers it.
- The affected component (the parser, the expand/callback/translate
  resolution, the parse-tree/cvec runtime, etc.) and version or commit.

We will acknowledge your report, investigate, and work with you on a fix and
disclosure timeline before any public announcement.
