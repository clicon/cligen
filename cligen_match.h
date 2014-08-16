/*
  CLI generator match functions, used in runtime checks.

  Copyright (C) 2001-2014 Olof Hagsand

  This file is part of CLIgen.

  CLIgen is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  CLIgen is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with CLIgen; see the file COPYING.
*/


#ifndef _CLIGEN_MATCH_H
#define _CLIGEN_MATCH_H

/* Number of allowed matchings */
#define MATCHVECTORLEN 1024

/*
 * Function Prototypes
 */
int match_pattern(cligen_handle h, char *, parse_tree , int, int, pt_vec *, 
		  int **, int *, cvec *, char **);
int match_pattern_exact(cligen_handle h, char *, parse_tree, int exact, cvec *, cg_obj **);
int command_levels(char *string);
int extract_substring(char *string0, int level, char **sp);
int extract_substring_rest(char *string0, int level, char **sp);
int match_complete(cligen_handle h, char *string0, parse_tree pt, int maxlen, cvec *cvec);

#endif /* _CLIGEN_MATCH_H */



