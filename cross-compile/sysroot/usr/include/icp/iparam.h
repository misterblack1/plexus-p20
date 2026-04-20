/* 
 * SID @(#)iparam.h	5.1
 */

/*
 * fundamental variables
 * don't change too often
 */
#define BIGBLK	1		/* enables 1k byte blocks */
#define	NOFILE	20		/* max open files per process */
#define	MAXPID	30000		/* max process id */
#define	MAXUID	60000		/* max user id */
#define	MAXLINK	1000		/* max links */

#ifdef z8000
#define	MAXMEM	(64)		/* max core per process */
#define	SSIZE	1		/* initial stack size (*2048 bytes) */
#define	SINCR	1		/* increment of stack (*2048 bytes) */
#define	USIZE	1		/* size of user block (*2048 bytes) */
#define	USRSTACK 0		/* Start of user stack */
#define	NSWB	3		/* size of swap pool */
#endif
#ifdef m68
#define MAXMEM (NUMLOGPAGE)	/* max core per process */
#define SSIZE	1		/* initial stk size (clicks) JK */
#define SINCR	1		/* stk inc size (clicks) */
#define USIZE	3		/* size of userb blk (clicks) */
#define USRSTACK (NUMLOGPAGE*PAGESIZE)	/* start of user stack */
#define NSWB	3		/* size of swap bh pool */
#endif

#define	CANBSIZ	256		/* max size of typewriter line	*/
#ifdef z8000
#define	HZ	50		/* Ticks/second of the clock */
#endif
#ifdef m68
#define HZ	64		/* ticks/sec of the clock */
#endif
#define MSGBUFS 256		/* Characters saved from error messages */
#define	NCARGS	5120		/* # characters in exec arglist */
#ifndef OVKRNL
#define NFLOCKS	100		/* number of lock table entries */
#else
#define NFLOCKS 50		/* less for overlay kernel, data space */
#endif

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

#ifdef PNETVT
#define	NSIG	21
#else
#define	NSIG	20
#endif
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
#ifdef PNETVT
#define SIGVT	20	/* flush out data in rbuf */
#endif

/*
 * fundamental constants of the implementation--
 * cannot be changed easily
 */

#define	NBPW	sizeof(int)	/* number of bytes in an integer */
#ifndef BIGBLK
#define	BSIZE	512		/* size of secondary block (bytes) */
#define	NINDIR	(BSIZE/sizeof(daddr_t))
#define	BMASK	0777		/* BSIZE-1 */
#define	INOPB	8		/* inodes per block */
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
#define	NICFREE	50		/* number of superblock free blocks */
#define	NULL	0
#define	CMASK	0		/* default mask for file creation */
/* #define	CDLIMIT	(1L<<11)	moved to conf.c   */
#define	NODEV	(dev_t)(-1)
#define	ROOTINO	((ino_t)2)	/* i number of all roots */
#define	SUPERB	((daddr_t)1)	/* block number of the super block */
#define	DIRSIZ	14		/* max characters per directory */
#define	NICINOD	100		/* number of superblock inodes */
#ifdef z8000
#define	CLKTICK	20000		/* microseconds in a  clock tick */
#endif
#ifdef m68
#define CLKTICK 15625		/* usec in a clk tick */
#endif
#ifdef BIGBLK
#define PGSIZE	512
#define PGSHIFT	9
#endif

/*
 * Some macros for units conversion
 */
/* Core clicks (512/2048 bytes) to segments and vice versa */
#ifdef m68
#define	ctos(x)	(x)
#define	stoc(x)	(x)
#else
#define	ctos(x)	(x)
#define	stoc(x)	(x)
#endif

/* Core clicks (512/2048 bytes) to disk blocks */
#ifdef m68
#define	ctod(x)	((x) << 3)
#else
#define	ctod(x)	((x) << 2)
#endif

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
#ifdef m68
#define	ctob(x)	((x)<<12)
#else
#define	ctob(x)	((x)<<11)
#endif

/* bytes to clicks */
#ifdef m68
#define	btoc(x)	(((unsigned)(x)+4095)>>12)
#define	btoct(x)	((unsigned)(x)>>12)
#else
#define	btoc(x)	(((unsigned) (x) >> 11) + (((x) & 2047) != 0))
#define	btoct(x)	((unsigned)(x)>>11)
#endif

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
#ifdef m68
typedef short		cnt_t;
#else
typedef char		cnt_t;
#endif
typedef	long		time_t;
#ifndef	OVKRNL
#ifdef m68
#ifdef ICP
typedef	int		label_t[9];
#else
typedef	int		label_t[13];	/* d2-d7 a2-a7 pc */
#endif
#else
typedef	int		label_t[9];
#endif
#else
typedef int		label_t[10];
#endif
typedef	short		dev_t;
typedef	long		off_t;
typedef	long		paddr_t;
#endif


#ifdef z8000
#define	lobyte(X)	(((char *)&X)[1])
#endif
#ifdef m68
#define	lobyte(X)	(((char *)&X)[3])
#endif
#define	hibyte(X)	(((char *)&X)[0])
#define	loword(X)	(((ushort *)&X)[1])
#define	hiword(X)	(((ushort *)&X)[0])

	/*----------------------*\
	|>>>>> ICP specific <<<<<|
	\*----------------------*/

#ifdef ICP
#ifdef DEBUG
#define NCLIST	15		/* DEBUG # of free cblocks */
#else
#ifdef PROFILE
#define NCLIST	50		/* max total CLIST size */
#else
#ifdef VPR
#define NCLIST	55		/* VPR needed some space */
#else
#define NCLIST	36		/* max total CLIST size */
#endif
#endif
#endif
#ifdef DEBUG
#define NPROC	10		/* DEBUG # of process entries */
#else
#define NPROC	15		/* max # of processes */
#endif
#define NCALL	10		/* max # simultaneous callouts */
#ifdef VPMSYS
#define	STKSIZE	218		/* number of words in a ICP process stack */
#else
#define	STKSIZE	220		/* number of words in a ICP process stack */
#endif
#endif

	/*----------------------*\
	|>>>>> VPM specific <<<<<|
	\*----------------------*/

#define VPMNCLIST	40	/* max total CLIST size */
#define VPMNPROC	6	/* max # of processes */
