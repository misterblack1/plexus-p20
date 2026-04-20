
/* SID @(#)proc.h	5.1 */
/* @(#)proc.h	1.0 11/14/84 */

/*
 * One structure allocated per active
 * process. It contains all data needed
 * about the process.
 */
struct	proc {
	char	p_stat;
	char	p_pri;		/* priority, negative is high */
	caddr_t	p_wchan;	/* event process is awaiting */
#ifdef z8000
	struct	proc	*p_procp;	/* procp for job processor process */
	int	p_pid;		/* process id for job processor process */
#endif
#ifdef m68
	long p_procp;		/* procp for job processor process */
	long p_pid;		/* process id for job processor process */
#endif
	char	*p_wrbuf;	/* write buffer address */
/*	int	p_wrbsz;	/* write buffer size */
	struct proc *p_link;	/* linked list of running processes */
	label_t	p_rsav;		/* save area for restart info */
	int	p_stk[STKSIZE];	/* stack for process */
};

extern struct proc proc[];	/* the proc table itself */
extern struct proc *curproc;	/* pointer to the executing process */

/* stat codes */
#define	SSLEEP	1		/* awaiting an event */
#define	SWAIT	2		/* (abandoned state) */
#define	SRUN	3		/* running */
#define	SIDL	4		/* intermediate state in process creation */
#define	SZOMB	5		/* intermediate state in process termination */
#define	SSTOP	6		/* process being traced */
