/* SID @(#)vpm.h	5.1 */
/* @(#)vpm.h	6.2 */

/* control characters */
#define	VINTR	0
#define	VQUIT	1
#define	VERASE	2
#define	VKILL	3
#define	VEOF	4
#define	VEOL	5
#define	VMIN	4
#define	VTIME	5

/* default control chars */
/*#define	CINTR	0177	/* DEL */
/*#define	CQUIT	034	/* FS, cntl | */
/*#define	CERASE	'#'
/*#define	CKILL	'@'
/*#define	CEOF	04	/* cntl d */
/*#define	CSTART	021	/* cntl q */
/*#define	CSTOP	023	/* cntl s */
/* BF 01nov84	*/
#define	TTIPRI	28
#define	TTOPRI	29

/* limits */
extern int ttlowat[], tthiwat[];

#ifdef VPMSYS
#ifdef ICP
#define TTYHOG	525
#endif
#endif

/*#ifndef VPMSYS
#ifdef ICP
#define TTYHOG	512
#else
#define	TTYHOG	256
#endif
#endif		BF 01nov84	*/
/*
#define	TTXOLO	128
#define	TTXOHI	256
BF 31oct84	*/

		/*------------------------------------------*\
		| The following have been added to implement |
		| Clear-To-Send (CTS) line protocol.         |
		| USE AT YOUR OWN RISK. PROBABLY WILL CHANGE |
		| BY RELEASE II !!!!!!!!!!!!!!!!!!!!         |
		\*------------------------------------------*/

#define ICTS	0040000		/* use CTS (active HIGH) for flow control */
#define ICTSLO	0100000		/* use inverted CTS (active low) */

/* VPM device masks */
#define TDEV(x)	(x&017)
#define BDEV(x)	((x>>4)&03)
#define IDEV(x)	((x>>6)&03)

#define VPMPRI  (PZERO+5)

/* vpm state bits */
#define VOPEN    01
#define WCLOSE   02
#define VPMERR   04
#define VPMWIP   010
#define VPMRPTA  020
#define VPMRIP   040
#define VPMECNT  0100

/* VPM error report defines */
#define HALTRCVD      0
#define ILLEGAL       1
#define FETCHERR      2
#define STACKERR      3
#define JMPERR        4
#define BUSERR        5
#define XBUFERR       6
#define RBUFERR       7
#define EXITINST      8
#define CRCERR        9
#define HANGUP        10
#define SEQUERR       11
#define CMDERR        12
#define STATERR       13
#define XMTSTERR      14
#define RCVSTERR      15
#define SETXERR       16
#define XMTERR        17
#define RCVERR        18
#define XBUFERR1      19
#define RBUFERR1      20
#define SIZERR        21
#define HANG          22
#define DRVERR        23
#define ARRAYERR      24

/* control modes */
#define	CBAUD	0000017
#define	B0	0
#define	B50	0000001
#define	B75	0000002
#define	B110	0000003
#define	B134	0000004
#define	B150	0000005
#define	B200	0000006
#define	B300	0000007
#define	B600	0000010
#define	B1200	0000011
#define	B1800	0000012
#define	B2400	0000013
#define	B4800	0000014
#define	B9600	0000015
#define	EXTA	0000016
#define	EXTB	0000017
#define	CSIZE	0000060
#define	CS5	0
#define	CS6	0000020
#define	CS7	0000040
#define	CS8	0000060
#define	CSTOPB	0000100
#define	CREAD	0000200
#define	PARENB	0000400
#define	PARODD	0001000
#define	HUPCL	0002000
#define	CLOCAL	0004000

/* line discipline 0 modes */
#define	ISIG	0000001
#define	ICANON	0000002
#define	XCASE	0000004
#define	ECHO	0000010
#define	ECHOE	0000020
#define	ECHOK	0000040
#define	ECHONL	0000100
#define	NOFLSH	0000200

/* Line discipline modes */
#define SYNC    0x01   /* means that we are using sync */
#define FHDLC   0x02   /* means that we are using hdlc */

#define	SSPEED	EXTB	/* default speed: cpu switch setting for baud rate */

/* Hardware bits */
#define	DONE	0200
#define	IENABLE	0100
#ifndef ICP		/* OVERRUN defined in ../icp/sio.h for icp */
#define	OVERRUN	040000
#endif
#define	FRERROR	020000
#define	PERROR	010000

/* Internal state */
#define	TIMEOUT	01		/* Delay timeout in progress */
#define	WOPEN	02		/* Waiting for open to complete */
#define	ISOPEN	04		/* Device is open */
#define	TBLOCK	010
#define	CARR_ON	020		/* Software copy of carrier-present */
#define	BUSY	040		/* Output in progress */
#define	OASLP	0100		/* Wakeup when output done */
#define	IASLP	0200		/* Wakeup when input done */
#define	TTSTOP	0400		/* Output stopped by ctl-s */
#define	EXTPROC	01000		/* External processing */
#define	TACT	02000
/* #define	ESC	04000	 BF 01nov84 Last char escape */
#define	TTIOW	020000
#define	TTXON	040000
#define	TTXOFF	0100000

/* Internal state */
#define ISACMD  01000           /* A unix driver command was received */

/* l_output status */
/*#define	CPRES	1	BF 31oct84	*/

/* device commands */
#define	T_OUTPUT	0
#define	T_TIME		1
#define	T_SUSPEND	2
#define	T_RESUME	3
#define	T_BLOCK		4
#define	T_UNBLOCK	5
#define	T_RFLUSH	6
#define	T_WFLUSH	7
#define	T_BREAK		8

