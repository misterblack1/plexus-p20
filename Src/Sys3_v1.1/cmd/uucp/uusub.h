	/*  uusub.h 3.2  11/6/79  09:37:14  */
 
#ifdef UUSUB
#define UB_SST(a)	ub_sst(a)
#else
#define UB_SST(a)	 
#endif
 
#define L_sub		"/usr/lib/uucp/L_sub"	
#define R_sub		"/usr/lib/uucp/R_sub"	
#define LCKLSUB	"/usr/spool/uucp/LCK.LSUB" /* L_sub lock */
#define LCKRSUB	"/usr/spool/uucp/LCK.RSUB" /* R_sub lock */

struct	ub_l {		/* L_sub format */
	 char	sys[10];	/* system name */
	 short	call;		/* # of calls attempted */
	 short	ok;		/* # of successful connection */
	 time_t	oktime;		/* latest good connection */
	 short	noacu;		/* # of times when ACU busy */
	 short	login;		/* # of times LOGIN fails */
	 short	nack;		/* # of times without answer */
	 short	other;		/* e.g. wrong time, lock, etc */
};
 
struct	ub_r {	/* R_sub format */
	 char	sys[10];	/* system name */
	 short	sf;		/* # of files sent */
	 long	sb;		/* # of bytes sent */
	 short	rf;		/* # of files received */
	 long	rb;		/* # of bytes received */
};
 
/*	connection status */
#define	ub_ok		0	/* call ok */
#define	ub_sys		1	/* bad system */
#define	ub_time		2	/* wrong time */
#define	ub_lock		3	/* system locked */
#define	ub_noacu	4	/* no acu */
#define	ub_nack		5	/* no answer */
#define	ub_login	6	/* login fail */
#define	ub_seq		7	/* bad sequence */
