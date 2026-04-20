/*
 * Copyright (C) 1989 by Kenneth Almquist.  All rights reserved.
 * This file is part of ash, which is distributed under the terms specified
 * by the Ash General Public License.  See the file named LICENSE.
 */

#ifndef SYSV
#define strchr mystrchr
#endif

#ifdef __STDC__
void scopyn(const char *, char *, int);
char *strchr(const char *, int);
void mybcopy(const pointer, pointer, int);
int prefix(const char *, const char *);
int number(const char *);
int is_number(const char *);
int strcmp(const char *, const char *);	/* from C library */
char *strcpy(char *, const char *);	/* from C library */
int strlen(const char *);		/* from C library */
char *strcat(char *, const char *);	/* from C library */
#else
void scopyn();
char *strchr();
void mybcopy();
int prefix();
int number();
int is_number();
int strcmp();
char *strcpy();
int strlen();
char *strcat();
#endif

#define equal(s1, s2)	(strcmp(s1, s2) == 0)
#define scopy(s1, s2)	((void)strcpy(s2, s1))
#define bcopy(src, dst, n)	mybcopy((pointer)(src), (pointer)(dst), n)
