/* SID */
/* @(#)proc.h	5.2 8/5/86 */

/*
 * One structure allocated per active process. It contains all data needed
 * about the process while the process may be swapped out.
 * Other per process data (user.h) is swapped with the process.
 */

struct	proc {
	char	p_stat;
	char	p_flag;
	char	p_pri;		/* priority, negative is high */
	char	p_time;		/* resident time for scheduling */
	char	p_cpu;		/* cpu usage for scheduling */
	char	p_nice;		/* nice for cpu usage */
	long	p_sig;		/* signals pending to this process */
	ushort	p_uid;		/* real user id */
	short	p_pgrp;		/* name of process group leader */
	short	p_pid;		/* unique process id */
	short	p_ppid;		/* process id of parent */
	unsigned short	p_addr;	/* address of swappable image */
	short	p_size;		/* size of swappable image (clicks) */
	caddr_t p_wchan;	/* event process is awaiting */
	struct text *p_textp;	/* pointer to text structure */
	struct proc *p_link;	/* linked list of running processes */
	int	p_clktim;	/* time to alarm clock signal */
	long	p_jobprocp;	/* job processor's process pointer */
	long	p_jobpid;	/* job processor's process id */
	char	*p_rbuf;	/* read buffer address */
	int	p_rdbsz;	/* read buffer size */
	char	*p_wbuf;	/* write buffer address */
	int	p_wrbsz;	/* write buffer size */

	short	p_adma;		/* DMA activity flag */
	char	*p_oprtn;	/* DMA open return buffer address*/
	int	p_ortnsz;	/* DMA open return buffer size */
	char	*p_rrtn;	/* DMA read return buffer address*/
	int	p_rrtnsz;	/* DMA read return buffer size */
	char	*p_wrtn;	/* DMA write return buffer address*/
	int	p_wrtnsz;	/* DMA write return buffer size */
	char	*p_crtn;	/* DMA close return buffer address*/
	int	p_crtnsz;	/* DMA close return buffer size */
	char	*p_irtn;	/* DMA ioctl return buffer address*/
	int	p_irtnsz;	/* DMA ioctl return buffer size */
	char	*p_lcbp;	/* DMA lcb buffer address*/
	int	p_lcbsz;	/* DMA lcb buffer size */

	char	p_name[14];	/* process name */
};

extern struct proc proc[];	/* the proc table itself */

/* dma activity flags */
#define A_FRMMB	0x01		/* mb input active */
#define A_TOMB	0x02		/* mb output active */
#define A_ISIO	0x04		/* sio input active */
#define A_OSIO	0x08		/* sio output active */
#define A_CLOS	0x10		/* line close active */
#define A_OPIO	0x20		/* pio output active */
#define A_CLEAN	0x40		/* Clean occurred, don't do mb i/f */

/* stat codes */
#define	SSLEEP	1		/* awaiting an event */
#define	SWAIT	2		/* (abandoned state) */
#define	SRUN	3		/* running */
#define	SIDL	4		/* intermediate state in process creation */
#define	SZOMB	5		/* intermediate state in process termination */
#define	SSTOP	6		/* process being traced */

/* flag codes */
#define	SLOAD	01		/* in core */
#define	SSYS	02		/* scheduling process */
#define	SLOCK	04		/* process cannot be swapped */
#define	SSWAP	010		/* process is being swapped out */
#define	STRC	020		/* process is being traced */
#define	SWTED	040		/* another tracing flag */
#define	STEXT	0100		/* text pointer valid */
#define	SSPART	0200		/* process is partially swapped out */
#define SULOCK	0200		/* process is locked in core */

/*
 * parallel proc structure
 * to replace part with times
 * to be passed to parent process
 * in ZOMBIE state.
 */
struct	xproc {
	char	xp_stat;
	char	xp_flag;
	char	xp_pri;		/* priority, negative is high */
	char	xp_time;	/* resident time for scheduling */
	char	xp_cpu;		/* cpu usage for scheduling */
	char	xp_nice;	/* nice for cpu usage */
	long	xp_sig;		/* signals pending to this process */
	ushort	xp_uid;		/* real user id */
	short	xp_pgrp;	/* name of process group leader */
	short	xp_pid;		/* unique process id */
	short	xp_ppid;	/* process id of parent */
	short	xp_xstat;	/* Exit status for wait */
	time_t	xp_utime;	/* user time, this proc */
	time_t	xp_stime;	/* system time, this proc */
};
