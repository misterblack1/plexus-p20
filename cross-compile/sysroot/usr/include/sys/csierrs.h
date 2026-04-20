/* SID @(#)csierrs.h	5.1 */
/*	csierrs.h 6.1 of 8/22/83
	@(#)csierrs.h	6.1
 */

/*
 * VPM error codes
 */
#define HALTRCVD	0
#define ILLEGAL		1
#define FETCHERR	2
#define STACKERR	3
#define JMPERR		4
#define BUSERR		5
#define XBUFERR		6
#define RBUFERR		7
#define EXITINST	8
#define CRCERR		9
#define HANGUP		10
#define	SEQUERR		11
#define	CMDERR		12
#define	STATERR		13
#define	XMTSTERR	14
#define	RCVSTERR	15
#define	SETXERR		16
#define	XMTERR		17
#define	RCVERR		18
#define	XBUFERR1	19
#define	RBUFERR1	20
#define	SIZERR		21
#define	HANG		22
#define	DRVERR		23
#define	ARRAYERR	24
#define PCDCMD		25
#define OKFAIL		26
#define NOLINE		27
#define BUFFERR		28
#define NODMKDEV	29
#define LINERR		30

/*
 *  Error codes for 3B20S X25 (tn82 and tn75) drivers
 */
#define DISCRCV		31
#define DSETERR  	32
#define PROTHALT	33
#define SOFTWERR	34
#define HARDWERR	35
