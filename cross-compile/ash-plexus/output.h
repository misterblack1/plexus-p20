/*
 * Copyright (C) 1989 by Kenneth Almquist.  All rights reserved.
 * This file is part of ash, which is distributed under the terms specified
 * by the Ash General Public License.  See the file named LICENSE.
 */

#ifndef OUTPUT_INCL

struct output {
      char *nextc;
      int nleft;
      char *buf;
      int bufsize;
      short fd;
      short flags;
};

extern struct output output;
extern struct output errout;
extern struct output memout;
extern struct output *out1;
extern struct output *out2;


#ifdef __STDC__
void outstr(char *, struct output *);
void out1str(char *);
void out2str(char *);
void outfmt(struct output *, char *, ...);
void out1fmt(char *, ...);
void fmtstr(char *, int, char *, ...);
/* void doformat(struct output *, char *, va_list); */
void doformat();
void emptyoutbuf(struct output *);
void flushall(void);
void flushout(struct output *);
void freestdout(void);
int xwrite(int, char *, int);
int xioctl(int, int, int);
#else
void outstr();
void out1str();
void out2str();
void outfmt();
void out1fmt();
void fmtstr();
/* void doformat(); */
void doformat();
void emptyoutbuf();
void flushall();
void flushout();
void freestdout();
int xwrite();
int xioctl();
#endif

#define outc(c, file)	(--(file)->nleft < 0? (emptyoutbuf(file), *(file)->nextc++ = (c)) : (*(file)->nextc++ = (c)))
#define out1c(c)	outc(c, out1);
#define out2c(c)	outc(c, out2);

#define OUTPUT_INCL
#endif
