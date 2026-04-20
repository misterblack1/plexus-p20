/* SID */
/* @(#)param.h	5.3 5/29/86 */

/*
 * fundamental variables
 * don't change too often
 */
#define BIGBLK	1		/* enables 1k byte blocks */
#define	NOFILE	20		/* max open files per process */
#define	MAXPID	30000		/* max process id */
#define	MAXUID	60000		/* max user id */
#define	MAXLINK	1000		/* max links */

#define MAXMEM (NUMLOGPAGE)	/* max core per process */
#define SSIZE	1		/* initial stk size (clicks) JK */
#define SINCR	1		/* stk inc size (clicks) */
#define USIZE	1		/* size of userb blk (clicks) */
#define USRSTACK (NUMLOGPAGE*PAGESIZE)	/* start of user stack */
#define NSWB	3		/* size of swap bh pool */

#define	CANBSIZ	256		/* max size of typewriter line	*/
#define HZ	16		/* ticks/sec of the clock */
#define MSGBUFS 256		/* Characters saved from error messages */
#define	NCARGS	5120		/* # characters in exec arglist */
#define NFLOCKS	100		/* number of lock table entries */

/*
 * priorities
 * probably should not be
 * altered too much
 */

#define	PSWP	0
#define	PINOD	10
#define	PRIBIO	20
#define	PZERO	25
#define	NZERO	20
#define	PPIPE	26
#define	PWAIT	30
#define	PSLEP	40
#define	PUSER	50
#define	PIDLE	127

/*
 * signals
 * dont change
 */

#define	NSIG	20
/*
 * No more than 32 signals (1-32) because they are
 * stored in bits in a long.
 */
#define	SIGHUP	1	/* hangup */
#define	SIGINT	2	/* interrupt (rubout) */
#define	SIGQUIT	3	/* quit (FS) */
#define	SIGILL	4	/* illegal instruction */
#define	SIGTRAP	5	/* trace or breakpoint */
#define	SIGIOT	6	/* iot */
#define	SIGEMT	7	/* emt */
#define	SIGFPE	8	/* floating exception */
#define	SIGKILL	9	/* kill, uncatchable termination */
#define	SIGBUS	10	/* bus error */
#define	SIGSEGV	11	/* segmentation violation */
#define	SIGSYS	12	/* bad system call */
#define	SIGPIPE	13	/* end of pipe */
#define	SIGALRM	14	/* alarm clock */
#define	SIGTERM	15	/* Catchable termination */
#define	SIGUSR1	16	/* user defined signal 1 */
#define	SIGUSR2	17	/* user defined signal 2 */
#define	SIGCLD	18	/* child death */
#define	SIGPWR	19	/* power-fail restart */

/*
 * fundamental constants of the implementation--
 * cannot be changed easily
 */

#define	NBPW	sizeof(int)	/* number of bytes in an integer */
#ifndef BIGBLK
#define	BSIZE	512		/* size of secondary block (bytes) */
#define	NINDIR	(BSIZE/sizeof(daddr_t))
#define	BMASK	0777		/* BSIZE-1 */
#define	BSHIFT	9		/* LOG2(BSIZE) */
#define	NMASK	0177		/* NINDIR-1 */
#define	NSHIFT	7		/* LOG2(NINDIR) */
#else
#define CXSIZE	2
#define BSIZE	1024
#define NINDIR	(BSIZE/sizeof(daddr_t))
#define BMASK	01777
#define INOPB	16
#define BSHIFT	10
#define NMASK	0377
#define NSHIFT	8
#endif
#define	NULL	0
#define	CMASK	0		/* default mask for file creation */
#define	NODEV	(dev_t)(-1)
#define	ROOTINO	((ino_t)2)	/* i number of all roots */
#define	DIRSIZ	14		/* max characters per directory */

#define CLKTICK 15625		/* usec in a clk tick */

#ifdef BIGBLK
#define PGSIZE	512
#define PGSHIFT	9
#endif

/*
 * Some macros for units conversion
 */
/* Core clicks (512/2048 bytes) to segments and vice versa */

#define	ctos(x)	(x)
#define	stoc(x)	(x)

/* Core clicks (512/2048 bytes) to disk blocks */

#define	ctod(x)	((x) << 3)

/* inumber to disk address */
#ifndef BIGBLK
#define	itod(x)	(daddr_t)((((unsigned)x+15)>>3))
#else
#define itod(x)	((daddr_t)((((unsigned)(x)+2*INOPB-1)/INOPB)))
#endif

/* inumber to disk offset */
#ifndef BIGBLK
#define	itoo(x)	(int)((x+15)&07)
#else
#define itoo(x) ((int)(((x)+2*INOPB-1)%INOPB))
#endif

/* clicks to bytes */
#define	ctob(x)	((x)<<12)

/* bytes to clicks */
#define	btoc(x)	(((unsigned)(x)+4095)>>12)
#define	btoct(x)	((unsigned)(x)>>12)

#ifdef BIGBLK
#define fsbtodb(b) ((daddr_t)((daddr_t)(b)<<1))
#define dbtofsb(b) ((daddr_t)((daddr_t)(b)>>1))
#define clrnd(i)   (((i)+(CXSIZE-1))&~(CXSIZE-1))
#endif

/* major part of a device */
#define	major(x)	(int)((unsigned)x>>8)

/* minor part of a device */
#define	minor(x)	(int)(x&0377)

/* make a device number */
#define	makedev(x,y)	(dev_t)(((x)<<8) | (y))

#ifndef ASLANG
typedef struct { int r[1]; } *	physadr;
typedef	long		daddr_t;
typedef	char *		caddr_t;
typedef	unsigned short	ushort;
typedef	ushort		ino_t;
typedef short		cnt_t;
typedef	long		time_t;
typedef	int		label_t[13];
typedef	short		dev_t;
typedef	long		off_t;
typedef	long		paddr_t;
#endif


#define	lobyte(X)	(((char *)&X)[3])
#define	hibyte(X)	(((char *)&X)[0])
#define	loword(X)	(((ushort *)&X)[1])
#define	hiword(X)	(((ushort *)&X)[0])
