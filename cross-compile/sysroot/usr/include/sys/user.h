/* @(#)user.h	5.3 SID */
/* @(#)user.h	6.1 */
#include "sys/pcb.h"
/*
 * The user structure.
 * One allocated per process.
 * Contains all per process data that doesn't need to be referenced
 * while the process is swapped.
 * The user block is USIZE*click bytes long; resides at virtual kernel
 * loc 0x7ffff800(vax), 0140000(11/70), floating(11/34);
 * contains the system stack per user; is cross referenced
 * with the proc structure for the same process.
 */
 
struct	user
{
#ifdef vax
	struct pcb u_pcb;		/* pcb, save area when switching */
#endif

	label_t	u_rsav;			/* save info when exchanging stacks */
	/* the u_abreq entry is added from the SYS3 port */
	int	(*u_abreq)();	/* called to abort i/o req during signal */
	label_t	u_qsav;			/* label variable for quits and interrupts */
	label_t	u_ssav;			/* label variable for swapping */
	char	u_segflg;		/* IO flag: 0:user D; 1:system; 2:user I */
	char	u_error;		/* return error code */
	ushort	u_uid;			/* effective user id */
	ushort	u_gid;			/* effective group id */
	ushort	u_ruid;			/* real user id */
	ushort	u_rgid;			/* real group id */
	struct proc *u_procp;		/* pointer to proc structure */
	int	*u_ap;			/* pointer to arglist */
	union {				/* syscall return values */
		struct	{
			int	r_val1;
			int	r_val2;
		}r_reg;
		off_t	r_off;
		time_t	r_time;
	} u_r;
	caddr_t	u_base;			/* base address for IO */
	unsigned u_count;		/* bytes remaining for IO */
	off_t	u_offset;		/* offset in file for IO */
	short	u_fmode;		/* file mode for IO */
	ushort	u_pbsize;		/* bytes in block for IO */
	ushort	u_pboff;		/* offset in block for IO */
	dev_t	u_pbdev;		/* real device for IO */
	daddr_t	u_rablock;		/* read ahead block addr */
	short	u_errcnt;		/* syscall error count */
	struct inode *u_cdir;		/* current directory of process */
	struct inode *u_rdir;		/* root directory of process */
	caddr_t	u_dirp;			/* pathname pointer */
	struct direct u_dent;		/* current directory entry */
	struct inode *u_pdir;		/* inode of parent directory of dirp */
	struct file *u_ofile[NOFILE];	/* pointers to file structures of open files */
	char	u_pofile[NOFILE];	/* per-process flags of open files */
	int	u_arg[10];		/* arguments to current system call */
	unsigned u_tsize;		/* text size (clicks) */
	unsigned u_dsize;		/* data size (clicks) */
	unsigned u_ssize;		/* stack size (clicks) */
	int	u_signal[NSIG];		/* disposition of signals */
	time_t	u_utime;		/* this process user time */
	time_t	u_stime;		/* this process system time */
	time_t	u_cutime;		/* sum of childs' utimes */
	time_t	u_cstime;		/* sum of childs' stimes */
	int	*u_ar0;			/* address of users saved R0 */
	struct u_sprof {		/* profile arguments */
		short	*pr_base;	/* buffer base */
		unsigned pr_size;	/* buffer size */
		unsigned pr_off;	/* pc offset */
		unsigned pr_scale;	/* pc scaling */
	} u_prof;
	char	u_intflg;		/* catch intr from sys */
	char	u_sep;			/* flag for I and D separation */
	short	*u_ttyp;		/* pointer to pgrp in "tty" struct */
	dev_t	u_ttyd;			/* controlling tty dev */
	struct {			/* header of executable file */
		unsigned ux_mag;	/* magic number */
		unsigned ux_tsize;	/* text size */
		unsigned ux_dsize;	/* data size */
		unsigned ux_bsize;	/* bss size */
		unsigned ux_ssize;	/* symbol table size */
		unsigned ux_rtsize;	/* text relocation size */
		unsigned ux_rdsize;	/* data relocation size */
		unsigned ux_entloc;	/* entry location */
	} u_exdata;
#define	ux_tstart	ux_rtsize
	char	u_comm[DIRSIZ];
	time_t	u_start;
	time_t	u_ticks;
	long	u_mem;
	long	u_ior;
	long	u_iow;
	long	u_iosw;
	long	u_ioch;
	char	u_acflag;
	short	u_cmask;		/* mask for file creation */
	daddr_t	u_limit;		/* maximum write address */
	short	u_lock;			/* process/text locking flags */

	int	u_dfsvc;		/* virtual circuit to remote system */
	int	u_dfspvc;		/* process virtual circuit to remote */
	ushort	u_rofile[NOFILE];	/* remote file pointer ordinals */
	struct {
		unsigned u_dfssis;	/* remote sister vc's */
		unsigned u_dfsid;	/* remote mount id */
	} u_sis[NOFILE];
	struct	dqp	*u_mntpro;	/* ptr to dfs process entry */

	char    u_fpstack[320];		/* floating point state stack */
	int	u_stack[1];
					/* kernel stack per user
					 * extends from u + USIZE*64
					 * backward not to reach here
					 */
};
extern struct user u;

#define	u_rval1	u_r.r_reg.r_val1
#define	u_rval2	u_r.r_reg.r_val2
#define	u_roff	u_r.r_off
#define	u_rtime	u_r.r_time

/* ioflag values: Read/Write, User/Kernel, Ins/Data */
#define	U_WUD	0
#define	U_RUD	1
#define	U_WKD	2
#define	U_RKD	3
#define	U_WUI	4
#define	U_RUI	5

#define	EXCLOSE	01
