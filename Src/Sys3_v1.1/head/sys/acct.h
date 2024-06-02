/*
 * Accounting structures
 */

typedef	ushort comp_t;		/* "floating point" */
		/* 13-bit fraction, 3-bit exponent  */

struct	acct
{
	char	ac_flag;		/* Accounting flag */
	char	ac_stat;		/* Exit status */
	ushort	ac_uid;			/* Accounting user ID */
	ushort	ac_gid;			/* Accounting group ID */
	dev_t	ac_tty;			/* control typewriter */
	time_t	ac_btime;		/* Beginning time */
	comp_t	ac_utime;		/* acctng user time in clock ticks */
	comp_t	ac_stime;		/* acctng system time in clock ticks */
	comp_t	ac_etime;		/* acctng elapsed time in clock ticks */
	comp_t	ac_mem;			/* memory usage */
	comp_t	ac_io;			/* chars transferred */
	comp_t	ac_rw;			/* blocks read or written */
	char	ac_comm[8];		/* command name */
};	

extern	struct	acct	acctbuf;
extern	struct	inode	*acctp;		/* inode of accounting file */

#define	AFORK	01		/* has executed fork, but no exec */
#define	ASU	02		/* used super-user privileges */
#define	ACCTF	0300		/* record type: 00 = acct */
