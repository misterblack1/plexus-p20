/*
 * A clist structure is the head of a linked list queue of characters.
 * The routines getc* and putc* manipulate these structures.
 */
struct clist {
	int	c_cc;		/* character count */
	struct cblock *c_cf;	/* pointer to first */
	struct cblock *c_cl;	/* pointer to last */
};

/*
 * A tty structure is needed for each UNIX character device that
 * is used for normal terminal IO.
 */

	/*-------------------------------------------------------*\
	| IMPORTANT!!! sc_pgrp corresponds to the same offset in  |
	|	       sctl.tp_un as does t_pgrp in tty. This is  |
	|	       a hideous kludge for system 3. See si.c    |
	|	       for the reason in siopen().		  |
	\*-------------------------------------------------------*/

#define	NCC	8
struct tty {
	struct	clist t_rawq;	/* raw input queue [0 -5]*/
	struct	clist t_canq;	/* canonical queue [6 -11]*/
	struct	clist t_outq;	/* output queue    [12-17]*/
	struct cblock *t_buf;	/* buffer pointer  [18-19]*/
	int	(* t_proc)();	/* routine for device functions [20-21]*/
	ushort	t_iflag;	/* input modes     [22-23]*/
	ushort	t_oflag;	/* output modes    [24-25]*/
	ushort	t_cflag;	/* control modes   [26-27]*/
	ushort	t_lflag;	/* line discipline modes [28-29]*/
	short	t_state;	/* internal state  [30-31]*/
	short	t_pgrp;		/* process group name [32-33]*/
#ifdef ICP
	dev_t	t_dev;		/* device number */
	short	t_numo;		/* # char sent to dma to be outputted */
	char	t_siglock;	/* signal locked flag */
#endif
	char	t_line;		/* line discipline */
	char	t_delct;	/* delimiter count */
	char	t_col;		/* current column */
	char	t_row;		/* current row */
	unsigned char	t_cc[NCC];	/* settable control chars */
};

/*
 * The structure of a clist block
 */
#ifdef ICP
#define CLSIZE  64
#else
#define	CLSIZE	24
#endif
struct cblock {
	struct cblock *c_next;
	char	c_first;
	char	c_last;
	char	c_data[CLSIZE];
};

extern struct cblock cfree[];
extern struct cblock * getcb();
extern struct cblock * getcf();
extern struct clist ttnulq;

struct chead {
	struct cblock *c_next;
	int	c_size;
};
extern struct chead cfreelist;

struct inter {
	int	cnt;
};

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
#define	CINTR	0177	/* DEL */
#define	CQUIT	034	/* FS, cntl | */
#define	CERASE	'#'
#define	CKILL	'@'
#define	CEOF	04	/* cntl d */
#define	CSTART	021	/* cntl q */
#define	CSTOP	023	/* cntl s */

#define	TTIPRI	28
#define	TTOPRI	29

/* limits */
extern int ttlowat[], tthiwat[];
#ifdef ICP
#define TTYHOG	512
#else
#define	TTYHOG	256
#endif
#define	TTXOLO	60
#define	TTXOHI	180

/* input modes */
#define	IGNBRK	0000001
#define	BRKINT	0000002
#define	IGNPAR	0000004
#define	PARMRK	0000010
#define	INPCK	0000020
#define	ISTRIP	0000040
#define	INLCR	0000100
#define	IGNCR	0000200
#define	ICRNL	0000400
#define	IUCLC	0001000
#define	IXON	0002000
#define	IXANY	0004000
#define	IXOFF	0010000

		/*------------------------------------------*\
		| The following have been added to implement |
		| Clear-To-Send (CTS) line protocol.         |
		| USE AT YOUR OWN RISK. PROBABLY WILL CHANGE |
		| BY RELEASE II !!!!!!!!!!!!!!!!!!!!         |
		\*------------------------------------------*/

#define ICTS	0040000		/* use CTS (active HIGH) for flow control */
#define ICTSLO	0100000		/* use inverted CTS (active low) */

/* output modes */
#define	OPOST	0000001		/* Postprocess output */
#define	OLCUC	0000002		/* Map lower case to upper on output */
#define	ONLCR	0000004		/* Map NL to CR-NL on output */
#define	OCRNL	0000010		/* Map CR to NL on output */
#define	ONOCR	0000020		/* No CR output at column 0 */
#define	ONLRET	0000040		/* NL performs CR function */
#define	OFILL	0000100		/* Use fill characters for delay */
#define	OFDEL	0000200		/* Fill is DEL, else NULL */
#define	NLDLY	0000400		/* Select new-line delays */
#define	NL0	0
#define	NL1	0000400
#define	CRDLY	0003000		/* Select carriage-return delays */
#define	CR0	0
#define	CR1	0001000
#define	CR2	0002000
#define	CR3	0003000
#define	TABDLY	0014000		/* Select horizontal-tab delays */
#define	TAB0	0
#define	TAB1	0004000
#define	TAB2	0010000
#define	TAB3	0014000		/* Expand tabs to spaces */
#define	BSDLY	0020000		/* Select backspace delays */
#define	BS0	0
#define	BS1	0020000
#define	VTDLY	0040000		/* Select vertical-tab delays */
#define	VT0	0
#define	VT1	0040000
#define	FFDLY	0100000		/* Select form-feed delays */
#define	FF0	0
#define	FF1	0100000

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
#define	ESC	04000		/* Last char escape */
#define	RTO	010000
#define	TTIOW	020000
#define	TTXON	040000
#define	TTXOFF	0100000

/* l_output status */
#define	CPRES	1

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

/*
 * Ioctl control packet
 */
struct termio {
	ushort	c_iflag;	/* input modes */
	ushort	c_oflag;	/* output modes */
	ushort	c_cflag;	/* control modes */
	ushort	c_lflag;	/* line discipline modes */
	char	c_line;		/* line discipline */
	unsigned char	c_xcc[NCC];	/* control chars */
};
