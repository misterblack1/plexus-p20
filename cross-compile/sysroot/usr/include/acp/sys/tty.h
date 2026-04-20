/* SID */
/* @(#)tty.h	5.4 9/21/86 */

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
	|	       sctl.tp_un as does t_pgrp in tty.          |
	|	       See si.c for the reason in siopen().	  |
	\*-------------------------------------------------------*/


#define	NCC	8

/*
 *  Define hdlcflags in tty struct
 */
#define HENDFRM 0x01
#define FRMER	0x02
#define XTFRM	0x04
#define XTCTL	0x08
#define XFMRPT	0x10
/*
 *	Define PIO high water mark
 */
#define	PIOHWATER	1020

/*
 *  Define the number of transmit and receive buffers
 */
#define	NXB	4
#define	NRB	4
/*
 *  Define the size of a transmit-buffer-table entry
 */
#define LXBTE	8
/*
 *  Define the size of a receive-buffer-table entry
 */
#define LRBTE	12
/*
 *  Define the elements of a receive-buffer-table entry
 *  (The elements of a transmit-buffer-table entry are
 *  the same except that the RCOUNT and RCTL fields are missing.)
 */
#define	LINK	0
#define	STATE	1
#define	DESC	2
#define	RCOUNT	5
#define	TCOUNT	5
#define	RCTL	7

#define	RXCRC	0x08	/* Recieve  CRC checking */
#define	TXCRC	0x01	/* Transmit CRC checking */
#define HDLCMODE 0x20   /* Set HDLC mode on SIO chip */

struct tty {
	struct	clist t_rawq;	/* raw input queue 		[ 0-11]*/
	struct	clist t_canq;	/* canonical queue 		[12-23]*/
	struct	clist t_outq;	/* output queue    		[24-35]*/
	struct cblock *t_buf;	/* buffer pointer  		[36-39]*/
	int	(* t_proc)();	/* routine for device functions [40-43]*/
	ushort	t_iflag;	/* input modes     		[44-45]*/
	ushort	t_oflag;	/* output modes    		[46-47]*/
	ushort	t_cflag;	/* control modes   		[48-49]*/
	ushort	t_lflag;	/* line discipline modes 	[50-51]*/
	short	t_state;	/* internal state  		[52-53]*/
	short	t_pgrp;		/* process group name 		[54-55]*/
	dev_t	t_dev;		/* device number 		[56-57]*/
	short	t_numo;		/* number of opens		[58-59]*/
	char	t_siglock;	/* signal locked flag 		[   60]*/
	char	t_line;		/* line discipline 		[   61]*/
	int	t_delct;	/* delimiter count 		[62-65]*/
	char	t_col;		/* current column 		[   66]*/
	char	t_row;		/* current row 			[   67]*/
	unsigned char	t_cc[NCC]; /* settable control chars 	[68-75]*/
	unsigned int	t_dmacc;   /* # of output chars to dma 	[76-79]*/

/*  VPM specific           */

	struct  clist v_devq;	/* queue output from protocol 	[80-91]*/
	struct cblock *v_devbuf;  /* buffer pointer to v_devq 	[92-95]*/
	struct  clist v_dupqoutq; /*			       [96-107]*/
        struct cblock *v_outqbuf; /* buffer pointer to t_outq [108-111]*/
	struct cblock *v_outqptr; /*			      [112-115]*/
	int     v_cfirst;	  /*			      [116-119]*/
	struct  clist v_rcvholdq; /* queue formed by protocol [120-131]*/
        int     p_xeomstate;      /* xeom state following flg [132-135]*/
        unsigned int     pc;	  /* VPM program counter      [136-139]*/
	unsigned int     ac;	  /* VPM accumulator storage  [140-143]*/
        int     stacktop;	  /*			      [144-147]*/
	int     stacknext;	  /*			      [148-151]*/
        int     stackactive;      /* used by the time out     [152-155]*/
	int     v_timerid;        /* current active timer id  [156-159]*/
        int     v_timertpc;       /* svd pgm cnter for timeout */
        int     v_tim2id;         /* id for timer (not timeout) */
        int     crcaddr;          /* sved crc loc--relative to 0 */
        char    synchar;          /* defined sync char */
        char    v_cmdar[4];       /* 4 byte cmd from unix driver */
	char    v_errs[8];        /* 8 byte err cnt area for VPM */
        char    v_xbfname[3];     /* 3 byte xmit buffer descriptor bytes */
        char    v_rbfname[3];     /* 3 byte recv buffer descriptor bytes */
        int     v_xbufin;         /* flag to indicate receipt of xmit buffer */
	ushort	v_iflag;	  /*  VPM input modes */
	ushort	hdlcflags;	  /* various bit settings  */
	char	*inframep;	  /* input buffer ptr for input frame */
	char	*infpnxt;	  /* input next byte ptr for input frame */
	char	curinbuf;	  /* current index of 'rb' array for input */
	char	curoutbuf;	  /* current index of 'xb' array for output */
	char	*xb[NXB];	  /* xmit buffer pointers */
	char	*rb[NRB];	  /* recieve buffer pointers */
	char	xbt[NXB][LXBTE];  /* Transmit-buffer-table */
	char	rbt[NRB][LRBTE];  /* Receive-buffer-table  */
	unsigned char	oxbt[8];  /* Used to translate sequence numbers into */
/* 				     oxbt entry numbers    */
	char	xbgq[NXB];	  /* Transmit-buffer-get queue */
/*					(used by getxfrm  */
	char	xboq[NXB];	  /* Transmit-buffer-open queue */
	char	xbrq[NXB];	  /* Transmit-buffer-return q (xbufout q) */
	char	rbrq[NRB];	  /* Recieve-buffer-return queue */
	char	erfq[NRB];	  /* empty-recieve-frame queue */
	char	exfq[NXB];	  /* empty-transmit-frame queue */
	char	cxfq[NXB];	  /* completed-transmit-frame queue */
	char	crfq[NXB];	  /* completed-recieve-frame queue */
	char	xmtac[5];	  /* xmit control frame bytes */
	char	rcvac[5];	  /* recieve control frame bytes */
	char	xmtstate;	  /* hdlc xmit states */
	char	nxctl;		  /* number of ctl frame bytes */
	char	rnrc;		  /* number of recieve ctl frame bytes */
	char	rcvbuf;	  	  /* current "just got" rcvfrm buffer */
};

/*
 * The structure of a clist block
 */
#define CLSIZE  64
struct cblock {
	struct cblock *c_next;
	int 	c_first;
	int 	c_last;
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
#define	VEOL2	6
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

#define TTYHOG	525
#define TTYDMA	550

#define	TTXOLO	128
#define	TTXOHI	256

/* input modes */
#define	IGNBRK	0x0001
#define	BRKINT	0x0002

#define	IGNPAR	0x0004
#define	PARMRK	0x0008
#define	INPCK	0x0010
#define	ISTRIP	0x0020

#define	INLCR	0x0040
#define	IGNCR	0x0080
#define	ICRNL	0x0100
#define	IUCLC	0x0200
#define	IXON	0x0400
#define	IXANY	0x0800
#define	IXOFF	0x1000

		/*------------------------------------------*\
		| The following have been added to implement |
		| Clear-To-Send (CTS) line protocol.         |
		| USE AT YOUR OWN RISK. PROBABLY WILL CHANGE |
		| BY RELEASE II !!!!!!!!!!!!!!!!!!!!         |
		\*------------------------------------------*/

#define ICTS	0x1000		/* use CTS (active HIGH) for flow control */
#define ICTSLO	0x8000		/* use inverted CTS (active low) */

/* output modes */
#define	OPOST	0x0001		/* Postprocess output */
#define	OLCUC	0x0002		/* Map lower case to upper on output */
#define	ONLCR	0x0004		/* Map NL to CR-NL on output */
#define	OCRNL	0x0008		/* Map CR to NL on output */
#define	ONOCR	0x0010		/* No CR output at column 0 */
#define	ONLRET	0x0020		/* NL performs CR function */
#define	OFILL	0x0040		/* Use fill characters for delay */
#define	OFDEL	0x0080		/* Fill is DEL, else NULL */
#define	NLDLY	0x0100		/* Select new-line delays */
#define	NL0	0
#define	NL1	0x0100
#define	CRDLY	0x0600		/* Select carriage-return delays */
#define	CR0	0
#define	CR1	0x0200
#define	CR2	0x0400
#define	CR3	0x0600
#define	TABDLY	0x1800		/* Select horizontal-tab delays */
#define	TAB0	0
#define	TAB1	0x0800
#define	TAB2	0x1000
#define	TAB3	0x1800		/* Expand tabs to spaces */
#define	BSDLY	0x2000		/* Select backspace delays */
#define	BS0	0
#define	BS1	0x2000
#define	VTDLY	0x4000		/* Select vertical-tab delays */
#define	VT0	0
#define	VT1	0x4000
#define	FFDLY	0x8000		/* Select form-feed delays */
#define	FF0	0
#define	FF1	0x8000

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
#define VBUSERR       5
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
#define	CBAUD	0x000f
#define	B0	0
#define	B50	0x0001
#define	B75	0x0002
#define	B110	0x0003
#define	B134	0x0004
#define	B150	0x0005
#define	B200	0x0006
#define	B300	0x0007
#define	B600	0x0008
#define	B1200	0x0009
#define	B1800	0x000a
#define	B2400	0x000b
#define	B4800	0x000c
#define	B9600	0x000d
#define	EXTA	0x000e
#define	EXTB	0x000f
#define	CSIZE	0x0030
#define	CS5	0
#define	CS6	0x0010
#define	CS7	0x0020
#define	CS8	0x0030
#define	CSTOPB	0x0040
#define	CREAD	0x0080
#define	PARENB	0x0100
#define	PARODD	0x0200
#define	HUPCL	0x0400
#define	CLOCAL	0x0800
#define CRTS	0x1000
#define CDTR	0x2000

/* line discipline 0 modes */
#define	ISIG	0x0001
#define	ICANON	0x0002
#define	XCASE	0x0004
#define	ECHO	0x0008
#define	ECHOE	0x0010
#define	ECHOK	0x0020
#define	ECHONL	0x0040
#define	NOFLSH	0x0080

/* Line discipline modes */
#define SYNC    0x01   /* means that we are using sync */
#define FHDLC   0x02   /* means that we are using hdlc */

#define	SSPEED	B9600	/* default speed */

/* Hardware bits */
#define	DONE	0200
#define	IENABLE	0100
#define	OVERRUN	0x2000	/* 020000 */
#define	FRERROR	0x4000	/* 040000 */
#define	PERROR	0x1000 	/* 010000 */

/* Internal state */
#define	TIMEOUT	0x0001		/* Delay timeout in progress */
#define	WOPEN	0x0002		/* Waiting for open to complete */
#define	ISOPEN	0x0004		/* Device is open */
#define	TBLOCK	0x0008
#define	CARR_ON	0x0010		/* Software copy of carrier-present */
#define	BUSY	0x0020		/* Output in progress */
#define	OASLP	0x0040		/* Wakeup when output done */
#define	IASLP	0x0080		/* Wakeup when input done */
#define	TTSTOP	0x0100		/* Output stopped by ctl-s */
#define	EXTPROC	0x0200		/* External processing */
#define	TACT	0x0400
#define	ESC	0x0800		/* Last char escape */
#define	RTO	0x1000
#define	TTIOW	0x2000
#define	TTXON	0x4000
#define	TTXOFF	0x8000

/* Internal state */
#define ISACMD  0x0200           /* A unix driver command was received */

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
