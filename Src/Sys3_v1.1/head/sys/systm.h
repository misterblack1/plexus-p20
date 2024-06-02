/*
 * Random set of variables used by more than one routine.
 */
struct inode *rootdir;		/* pointer to inode of root directory */
struct proc *runq;		/* head of linked list of running processes */
short	cputype;		/* type of cpu = z8000, m68000 */
time_t	lbolt;			/* time in HZ since last boot */
time_t	time;			/* time in sec from 1970 */
int	ctc0tim;		/* variable that determines speed of z8000 */
int	initstate;		/* if nonzero, mbtimeout is not generated */
int	mbusto;			/* incremented by one if mbtimout occurs */
int	numuser;		/* number of ttys with O_RDWR open (= logins) */

int	mpid;			/* generic for unique process id's */
char	runin;			/* scheduling flag */
char	runout;			/* scheduling flag */
char	runrun;			/* scheduling flag */
char	curpri;			/* more scheduling */
int	maxmem;			/* actual max memory per process */
int	physmem;		/* physical memory on this CPU */
daddr_t	swplo;			/* block number of swap space */
int	nswap;			/* size of swap space */
int	numdisks;		/* total number of disks with init info */
int	updlock;		/* lock for sync */
daddr_t	rablock;		/* block to be read ahead */
#ifdef pdp11
char	regloc[];		/* locs of saved user registers (trap.c) */
#endif
char	msgbuf[MSGBUFS];	/* saved "printf" characters */
dev_t	rootdev;		/* device of the root */
dev_t	swapdev;		/* swapping device */
dev_t	pipedev;		/* pipe device */
dev_t	dumpdev;		/* dump device */
int	icode[];		/* user init code */
int	szicode;		/* its size */
int	phypage;		/* first phys page number avail to unix */
int	udseg;			/* data map for current user process */
int	uiseg;			/* code map for current user process */
int	diagswits;		/* diagnostic switch settings */
int	ecccnt;			/* counter for ecc logging rate */
int	blkacty;		/* active block devices */
int	pwr_cnt, pwr_act;
int	(*pwr_clr[])();
dev_t getmdev();
daddr_t	bmap();
struct inode *ialloc();
struct inode *iget();
struct inode *owner();
struct inode *maknode();
struct inode *namei();
struct buf *alloc();
struct buf *getblk();
#ifdef vax
struct buf *geteblk();
#else
struct buf *getablk();
#endif
struct buf *bread();
struct buf *breada();
struct filsys *getfs();
struct file *getf();
struct file *falloc();
int	uchar();
long	gettod();
/*
 * Instrumentation
 */
int	dk_busy;
long	dk_time[32];
long	dk_numb[3];
long	dk_wds[3];
long	tk_nin;
long	tk_nout;

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

