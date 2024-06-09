	/*  uust.h 1.1  10/2/79  15:04:39  */
#ifdef UUSTAT
#define US_RRS(a,b)	us_rrs(a,b)
#define US_CRS(a)	us_crs(a)
#define US_SST(a)	us_sst(a)
#define	USRF(flag)	Usrf |= flag
#else
#define US_RRS(a,b)	 
#define US_CRS(a)	 
#define US_SST(a)	 
#define	USRF(flag)
#endif
 
#define L_stat		"/usr/lib/uucp/L_stat"	/* system status */
#define R_stat		"/usr/lib/uucp/R_stat"	/* request status */
#define LCKLSTAT	"/usr/spool/uucp/LCK.LSTAT" /* L_stat lock */
#define LCKRSTAT	"/usr/spool/uucp/LCK.RSTAT" /* R_stat lock */
#define 	NAME7	8

struct	us_ssf {	/* L_stat format */
	 char	sysname[NAME7];	/* system name */
	 time_t	sti;			/* status time */
	 short	sstat;			/* system status */
};
 
struct	us_rsf {	/* R_stat format */
	 short	jobn;		/* job # */
	 char	user[NAME7];	/* login user id */
	 char	rmt[NAME7];	/* remote system name */
	 time_t	qtime;		/* time the command is queued */
	 time_t	stime;		/* status time */
	 short	ustat;		/* job status */
};
 
 
/***	system status flags 	***/
#define	us_s_ok		0	/* conversation succeeded */
#define	us_s_sys	1	/* bad system */
#define	us_s_time	2	/* wrong time to call */
#define	us_s_lock	3	/* system locked */
#define us_s_dev	4	/* no device available */
#define	us_s_dial	5	/* dial failed */
#define	us_s_login	6	/* login failed */
#define	us_s_hand	7	/* handshake failed */
#define	us_s_start	8	/* startup failed */
#define	us_s_gress	9	/* conversation in progress */
#define	us_s_cf		10	/* conversation failed */
#define us_s_cok	11	/* call succeeded */
 
 
/***	request status flags	***/
#define	USR_CFAIL	01	/* copy fail */
#define	USR_LOCACC	02	/* local access to file denied */
#define	USR_RMTACC	04	/* remote access to file denied */
#define	USR_BADUUCP	010	/* bad uucp command */
#define	USR_RNOTMP	020	/* remote can't create temp file */
#define USR_RMTCP	040	/* can't copy to remote directory */
#define USR_LOCCP	0100	/* can't copy to local directory */
#define USR_LNOTMP	0200	/* local can't create temp file */
#define	USR_XUUCP	0400	/* can't execute uucp */
#define	USR_COK		01000	/* copy (partially) succeeded */
#define	USR_COMP	02000	/* copy completed, job deleted */
#define	USR_QUEUE	04000	/* job is queued */
 
/*** define USR flag ***/
extern short Usrf;	/* declaration in uucpdefs.c */
 
