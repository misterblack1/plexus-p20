/* SID */
/* @(#)systm.h	5.2 8/11/86 */

/*
 * Random set of variables used by more than one routine.
 */
struct proc *runq;		/* head of linked list of running processes */
time_t	lbolt;			/* time in HZ since last boot */
time_t	time;			/* time in sec from 1970 */
long	sysmem;			/* size of system memory */
int	initstate;		/* if nonzero, mbtimeout is not generated */
int	mbusto;			/* incremented by one if mbtimout occurs */
int	mpid;			/* generic for unique process id's */
char	runin;			/* scheduling flag */
char	runout;			/* scheduling flag */
char	runrun;			/* scheduling flag */
char	curpri;			/* more scheduling */
int	maxmem;			/* actual max memory per process */
int	physmem;		/* physical memory on this CPU */
extern char	regloc[];	/* locs of saved user registers (trap.c) */
char	msgbuf[MSGBUFS];	/* saved "printf" characters */
int	icode[];		/* user init code */
int	szicode;		/* its size */
int	dcode[];		/* execme code */
int	szdcode;		/* its size */
int	mcode[];		/* execdp code */
int	szmcode;		/* its size */
int	pcode[];		/* temp user exit code */
int	szpcode;		/* its size */
int	phypage;		/* first phys page number avail to unix */
int	udseg;			/* data map for current user process */
int	uiseg;			/* code map for current user process */
int	diagswits;		/* diagnostic switch settings */
int	leds;			/* current state of leds */
int	ecccnt;			/* counter for ecc logging rate */
int	parityerror;		/* number of memory parity errors */
unsigned short memsize;		/* non-vol RAM passed value 0=512k,1=2MB */
int	pwr_cnt, pwr_act;
int	(*pwr_clr[])();
dev_t getmdev();
daddr_t	bmap();
struct buf *alloc();
struct buf *getblk();
struct buf *getablk();
struct buf *bread();
struct buf *breada();
struct filsys *getfs();
struct file *getf();
struct file *falloc();
int	uchar();
long	gettod();

/*
 * Structure of the system-entry table
 */
extern struct sysent {
	char	sy_narg;		/* total number of arguments */
	char	sy_nrarg;		/* number of args in registers */
	int	(*sy_call)();		/* handler */
} sysent[];

/*
 * Structure used for PLEXUS style kernel profiling
 */
 extern	struct profbuf {
	long k_norm;
	long k_sys;
	long k_sysvion;
	int  profsf;
} profbuf;

