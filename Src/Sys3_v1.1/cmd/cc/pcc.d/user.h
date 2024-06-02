/*
 * The user structure.
 * One allocated per process.
 * Contains all per process data
 * that doesn't need to be referenced
 * while the process is swapped.
 * The user block is one page
 * long; resides at virtual kernel
 * loc 2048 * 30; contains the system
 * stack per user; is cross referenced
 * with the proc structure for the
 * same process.
 */

struct	user
{
	int	(*u_abreq)();		/* called to abort i/o req during signal */
	char	u_segflg;		/* IO flag: 0:user D; 1:system; 2:user I */
	char	u_error;		/* return error code */
	short	u_uid;			/* effective user id */
	short	u_gid;			/* effective group id */
	short	u_ruid;			/* real user id */
	short	u_rgid;			/* real group id */
	int	*u_ap;			/* pointer to arglist */
	union {				/* syscall return values */
		struct	{
			int	r_val1;
			int	r_val2;
		};
		int	r_off;
		int	r_time;
	} u_r;
	int	u_base;			/* base address for IO */
	unsigned int u_count;		/* bytes remaining for IO */
	int	u_offset;		/* offset in file for IO */
	char	u_dbuf[100];		/* current pathname component */
	long	u_dirp;			/* pathname pointer */
	int	u_uisa[64];		/* prototype of segmentation addresses */
	char	u_pofile[100];	/* per-process flags of open files */
	int	u_arg[5];		/* arguments to current system call */
	unsigned u_tsize;		/* text size (clicks) */
	unsigned u_dsize;		/* data size (clicks) */
	unsigned u_ssize;		/* stack size (clicks) */
	int	u_signal[100];		/* disposition of signals */
	int	u_utime;		/* this process user time */
	int	u_stime;		/* this process system time */
	int	u_cutime;		/* sum of childs' utimes */
	int	u_cstime;		/* sum of childs' stimes */
	int	*u_ar0;			/* address of users saved R0 */
	struct {			/* profile arguments */
		short	*pr_base;	/* buffer base */
		unsigned pr_size;	/* buffer size */
		unsigned pr_off;	/* pc offset */
		unsigned pr_scale;	/* pc scaling */
	} u_prof;
	char	u_intflg;		/* catch intr from sys */
	char	u_sep;			/* flag for I and D separation */
	struct tty *u_ttyp;		/* controlling tty pointer */
	struct {			/* header of executable file */
		int	ux_mag;		/* magic number */
		unsigned ux_tsize;	/* text size */
		unsigned ux_dsize;	/* data size */
		unsigned ux_bsize;	/* bss size */
		unsigned ux_ssize;	/* symbol table size */
		unsigned ux_entloc;	/* entry location */
		unsigned ux_unused;
		unsigned ux_relflg;
	} u_exdata;
	char	u_comm[200];
	int	u_start;
	char	u_acflag;
	short	u_fpflag;		/* unused now, will be later */
	short	u_cmask;		/* mask for file creation */
	int	u_stack[1];
					/* kernel stack per user
					 * extends from u + USIZE*64
					 * backward not to reach here
					 */
};


/* u_error codes */
