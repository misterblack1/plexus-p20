/*
 * Copyright (C) 1989 by Kenneth Almquist.  All rights reserved.
 * This file is part of ash, which is distributed under the terms specified
 * by the Ash General Public License.  See the file named LICENSE.
 */

struct stackmark {
      struct stack_block *stackp;
      char *stacknxt;
      int stacknleft;
};


extern char *stacknxt;
extern int stacknleft;
extern int sstrnleft;
extern int herefd;

#ifdef __STDC__
pointer ckmalloc(int);
pointer ckrealloc(pointer, int);
void free(pointer);		/* defined in C library */
char *savestr(char *);
pointer stalloc(int);
void stunalloc(pointer);
void setstackmark(struct stackmark *);
void popstackmark(struct stackmark *);
void growstackblock(void);
void grabstackblock(int);
char *growstackstr(void);
char *makestrspace(void);
void ungrabstackstr(char *, char *);
#else
pointer ckmalloc();
pointer ckrealloc();
void free();		/* defined in C library */
char *savestr();
pointer stalloc();
void stunalloc();
void setstackmark();
void popstackmark();
void growstackblock();
void grabstackblock();
char *growstackstr();
char *makestrspace();
void ungrabstackstr();
#endif



#define stackblock() stacknxt
#define stackblocksize() stacknleft
#define STARTSTACKSTR(p)	p = stackblock(), sstrnleft = stackblocksize()
#define STPUTC(c, p)	(--sstrnleft >= 0? (*p++ = (c)) : (p = growstackstr(), *p++ = (c)))
#define CHECKSTRSPACE(n, p)	if (sstrnleft < n) p = makestrspace(); else
#define USTPUTC(c, p)	(--sstrnleft, *p++ = (c))
#define STACKSTRNUL(p)	(sstrnleft == 0? (p = growstackstr(), *p = '\0') : (*p = '\0'))
#define STUNPUTC(p)	(++sstrnleft, --p)
#define STTOPC(p)	p[-1]
#define STADJUST(amount, p)	(p += (amount), sstrnleft -= (amount))
#define grabstackstr(p)	stalloc(stackblocksize() - sstrnleft)

#define ckfree(p)	free((pointer)(p))
