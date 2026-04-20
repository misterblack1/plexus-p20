/*
 * Copyright (C) 1989 by Kenneth Almquist.  All rights reserved.
 * This file is part of ash, which is distributed under the terms specified
 * by the Ash General Public License.  See the file named LICENSE.
 */

/* PEOF (the end of file marker) is defined in syntax.h */

/*
 * The input line number.  Input.c just defines this variable, and saves
 * and restores it when files are pushed and popped.  The user of this
 * package must set its value.
 */
extern int plinno;
extern int parsenleft;		/* number of characters left in input buffer */
extern char *parsenextc;	/* next character in input buffer */


#ifdef __STDC__
char *pfgets(char *, int);
int pgetc(void);
int preadbuffer(void);
void pungetc(void);
void ppushback(char *, int);
void setinputfile(char *, int);
void setinputfd(int, int);
void setinputstring(char *, int);
void popfile(void);
void popallfiles(void);
void closescript(void);
#else
char *pfgets();
int pgetc();
int preadbuffer();
void pungetc();
void ppushback();
void setinputfile();
void setinputfd();
void setinputstring();
void popfile();
void popallfiles();
void closescript();
#endif

#define pgetc_macro()	(--parsenleft >= 0? *parsenextc++ : preadbuffer())
