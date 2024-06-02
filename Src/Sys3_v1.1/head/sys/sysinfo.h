struct sysinfo {
	time_t	cpu[3];
#define	CPU_IDLE	0
#define	CPU_USER	1
#define	CPU_KERNAL	2
	time_t	wait[3];
#define	W_IO	0
#define	W_SWAP	1
#define	W_PIO	2
	long	bread;
	long	bwrite;
	long	lread;
	long	lwrite;
	long	swapin;
	long	swapout;
	long	pswitch;
	long	qswitch;
	long	idle;
	long	preempt;
	long	syscall;
	long	iget;
	long	namei;
	long	dirblk;
	long	readch;
	long	writech;
	long	rcvint;
	long	xmtint;
	long	mdmint;
	long	rawch;
	long	canch;
	long	outch;
};

extern struct sysinfo sysinfo;

struct syswait {
	short	iowait;
	short	swap;
	short	physio;
};

extern struct syswait syswait;
