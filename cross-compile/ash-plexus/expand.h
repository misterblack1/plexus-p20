/*
 * Copyright (C) 1989 by Kenneth Almquist.  All rights reserved.
 * This file is part of ash, which is distributed under the terms specified
 * by the Ash General Public License.  See the file named LICENSE.
 */

struct strlist {
      struct strlist *next;
      char *text;
};


struct arglist {
      struct strlist *list;
      struct strlist **lastp;
};



#ifdef __STDC__
void expandarg(union node *, struct arglist *, int);
void expandhere(union node *, int);
int patmatch(char *, char *);
void rmescapes(char *);
int casematch(union node *, char *);
#else
void expandarg();
void expandhere();
int patmatch();
void rmescapes();
int casematch();
#endif
