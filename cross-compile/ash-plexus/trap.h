/*
 * Copyright (C) 1989 by Kenneth Almquist.  All rights reserved.
 * This file is part of ash, which is distributed under the terms specified
 * by the Ash General Public License.  See the file named LICENSE.
 */

extern int sigpending;

#ifdef __STDC__
void clear_traps(void);
int setsignal(int);
void ignoresig(int);
void dotrap(void);
void setinteractive(int);
void exitshell(int);
#else
void clear_traps();
int setsignal();
void ignoresig();
void dotrap();
void setinteractive();
void exitshell();
#endif
