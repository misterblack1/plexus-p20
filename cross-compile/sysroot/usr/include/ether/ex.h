/* SID @(#)ex.h	5.1 */
/*  */
/* @(#)ex.h	1.8 10/1/84 */

/*
 *  This file contains defines for the Excelan (EXOS/101) 
 *  ethernet controller.
 */

/*
 *  The EXOS/101 port address on the multibus.
 */

#define	P_AEXCELAN	0xff10		/* port A, board and int reset */
#define P_BEXCELAN	0xff11		/* port B, init. and status */

#define EXBERROR	(1<<0)		/* port B status, error bit */
#define EXBINT		(1<<1)		/* port B status, interrupt bit */
#define EXBRDY		(1<<3)		/* port B status, port B ready */

/*
 *  Ethernet frame header format.
 */

struct etframe {
	unsigned char ef_dest[6];
	unsigned char ef_src[6];
	ushort	ef_type;
};

#define ETHERTYPE	0x3080

/*
 *  Initialization Reply completion code field definitions.
 */

#define	EXISUCCESS	0		/* successful initialization */
#define	EXIERRPARAM	0xa0		/* failed, invalid params. */
/* 
 *  Initialization Request mode field definitions.
 */

#define	EXIMLINK	0		/* link level controller mode */
#define EXIMFEHOST	1		/* front-end mode, boot from host */
#define EXIMFENET	2		/* front-end mode, bott from net */

/*
 *  Initialization Request interrupt type filed definitions.
 */

#define EXIINTNO	0		/* no interrupt, polled completions */
#define EXIINTIO	1		/* I/O mapped interrupts */
#define EXIINTMEM	2		/* memory mapped interrupts */
#define EXIINTLEV	3		/* level interrupts on INT 0-7 */

/*
 *  Initialization Request/Reply Message Format.
 */

struct exi {
	ushort		exi_rsvd;	/* reserved area */
	unsigned char	exi_ver[4];	/* EXOS/101 version */
	unsigned char	exi_mode;	/* EXOS/101 usage mode */
	unsigned char	exi_cc;		/* completion code */
	ushort		exi_hdfo;	/* host data format option */
	ushort		exi_1arsvd;	/* reserved area */
	unsigned char	exi_ham;	/* host address mode */
	unsigned char	exi_1brsvd;	/* reserved area */
	unsigned char	exi_nmemm;	/* number of memory maps */
	unsigned char	exi_2brsvd;	/* reserved area */
	unsigned char	exi_mmap[32];	/* memeory map */
	ushort		exi_mmvblk;	/* MSB's of movable block */
	ushort		exi_lmvblk;	/* LSB's of movable block */
	unsigned char	exi_mailb;	/* number of mailboxes */
	unsigned char	exi_proc;	/* number of processes */
	unsigned char	exi_nhosts;	/* number of hosts */
	unsigned char	exi_adslots;	/* number of adress slots */
	ushort		exi_qohtoe;	/* host to EXOS/101 queue offset */
	ushort		exi_qshtoe;	/* host to EXOS/101 queue segment */
	ushort		exi_offhtoe;	/* host to EXOS/101 queue hdr addr */
	unsigned char	exi_hival;	/* host to EXOS/101 interrupt value */
	unsigned char	exi_hityp;	/* host to EXOS/101 interrupt type */
	ushort		exi_hliadrhtoe;	/* host to EXOS/101 int. addr LSB's */
	ushort		exi_hmiadrhtoe;	/* host to EXOS/101 int. addr MSB's */
	ushort		exi_qoetoh;	/* EXOS/101 to host queue offset */
	ushort		exi_qsetoh;	/* EXOS/101 to host queue segment */
	ushort		exi_offetoh;	/* EXOS/101 to host queue hdr addr */
	unsigned char	exi_eival;	/* EXOS/101 to host interrupt value */
	unsigned char	exi_eityp;	/* EXOS/101 to host interrupt type */
	ushort		exi_emiadretoh;	/* EXOS/101 to host int. adr MSB's */
	ushort		exi_eliadretoh;	/* EXOS/101 to host int. adr LSB's */
};

#define MAXEXQDATA	40
#ifndef m68
#define NUMEXQENT	16
#define NUMEXREAD	3
#define NUMEXWRITE	10
#else
#define NUMEXQENT	20
#define NUMEXREAD	7
#define NUMEXWRITE	10
#endif
#define MAXREQOUT	NUMEXREAD+NUMEXWRITE

/*
 *  Host to EXOS/101 message queue entry status field bit definitions.
 */

#define EXQOWN	 (1 << 0)	/* queue entry ownership bit */
#define EXQDONE  (1 << 1) 	/* queue entry done bit */
#define EXQOVER  (1 << 2)	/* queue entry overflow bit */

/*
 *  Host to EXOS/101 message queue entry format.
 */

struct  exqe {
	ushort	exq_link;	/* queue entry link */
	unsigned char exq_stat; /* queue entry status */
	unsigned char exq_rsvd; /* queue entry reserved field */
	ushort	exq_len;	/* queue entry data length */
	unsigned char exq_data[MAXEXQDATA];
				/* queue entry data area */
	struct etframe exq_ef;	/* ether frame header */
};

/*
 *  Host to EXOS/101 message queue format.
 */

struct	exq {
	ushort	exq_qhdr;		/* EXOS/101 queue header pointer */
	struct	exqe e[NUMEXQENT];
};

/*
 *  EXOS/101 Link Level operation request definitions.
 */

#define EXNET_MODE	8		/* read/modify the network mode */
#define	EXNET_ADDRS	9		/* read/modify network addr. slots */
#define	EXNET_RCV	10		/* enable/disable rcv on addr slots */
#define EXNET_STSTCS	11		/* read/clear network statistics */
#define EXMLBX_SEND	12		/* Ethernet transmit request */
#define	EXMLBX_RECV	13		/* Ethernet receive request */
/*
 *  General Request Mask definitions.
 */

#define	EXREAD	2			/* read request */
#define EXWRITE	1			/* write request */
#define EXRWR	3			/* read/write request */

/* 
 *  General EXOS/101 - host message header format.
 */

struct exqmh {
	ushort	mh_rsvd;		/* reserved field */
	unsigned long mh_uid;		/* user id field */
	unsigned char mh_rplyc;		/* reply code field */
	unsigned char mh_reqc;		/* request code field */
};

/*
 *  Network mode request definitions.
 */

#define	EXNMOVFL	0x4		/* overflow error on request */
#define	EXNMSHRT	0x8		/* short packet error (<64 bytes) */
#define	EXNMCRC		0x20		/* CRC error */
#define EXNMALIGN	0x10		/* packet alignment error */
#define EXNMDISCON	0		/* disconnect from network */
#define EXNMCONPF	1		/* connect, enable perfect filter */
#define EXNMCONHF	2		/* connect, hardware address filter */
#define EXNMPROM	3		/* coneect, promiscuos mode */
#define EXNMSUCCESS	0		/* network mode request success */
#define EXNMNOLINK	0xa1		/* network mode error, not link lvl */

/*
 *  Network mode request message format.
 */

struct	exnm {
	struct exqmh nmh;		/* general header */
	unsigned char nm_errm;		/* error receive mask */
	unsigned char nm_reqm;		/* request mask */
	unsigned char nm_rsvd;		/* space holder (byte swap) */
	unsigned char nm_mode;		/* network mode */
};

/*
 *  Network Address request definitions.
 */

#define	EXNASUCCESS	0		/* successfull completion */
#define EXNABADSLOT	0xd1		/* slot does not exist */
#define EXNABADADR	0xd2		/* improper address */
#define EXNANOlink	0xa1		/* failed, not in link level mode */

/*
 *  Network Address request definitions.
 */

#define	EXNRSUCCESS	0		/* successfull completion */
#define EXNRBADSLOT	0xd1		/* slot does not exist */
#define EXNRBADADR	0xd2		/* address slot empty */
#define EXNRNOlink	0xa1		/* failed, not in link level mode */

/*  
 *  Network Address request message format.
 */

struct	exna {
	struct	exqmh	nah;		/* general header */
	unsigned char na_slot;		/* address slot number */
	unsigned char na_reqm;		/* request mask */
	unsigned char na_netadr[6];	/* network address */
};
	
/*
 *  Network Address request definitions.
 */

#define	EXNRSUCCESS	0		/* successfull completion */
#define EXNRBADSLOT	0xd1		/* slot does not exist */
#define EXNRBADADR	0xd2		/* address slot empty */
#define EXNRNOLINK	0xa1		/* failed, not in link level mode */

/*  
 *  Network Receive request message format.
 */

struct	exnr {
	struct	exqmh	nrh;		/* general header */
	unsigned char nr_slot;		/* address slot number */
	unsigned char nr_reqm;		/* request mask */
};

/*
 *  Network Statistics request definitions.
 */

#define	EXNSSUCCESS	0		/* successfull completion */
#define EXNSNOLINK	0xa1		/* failed, not in link level mode */
#define EXSTATTO	600		/* exstatgo timeout value */
					/* statistics gather every n clicks */

/*  
 *  Network Statistics request message format.
 */

struct	exns {
	struct	exqmh	nsh;		/* general header */
	unsigned char ns_rsvd;		/* reserved character */
	unsigned char ns_reqm;		/* request mask */
	ushort	ns_obnum;		/* number of objects */
	ushort	ns_obindx;		/* ojbects index */
	ushort	ns_bufoff;		/* buffer offset */
	ushort	ns_bufseg;		/* buffer segment */
};

/*
 *  Network Statistics returned buffer format.
 */

struct exsdata {
	unsigned long	exns_fsne;	/* frames sent no errors */
	unsigned long	exns_faec;	/* frames aborted excess collisions */
	unsigned long	exns_rsvd;	/* reserved */
	unsigned long	exns_tdr;	/* time domain reflectometer */
	unsigned long	exns_frne;	/* frames received no errors */
	unsigned long	exns_frae;	/* frames received allignment errors */
	unsigned long	exns_frce;	/* frames received CRC errors */
	unsigned long	exns_fl;	/* frames lost */
};

/*
 *  Ethernet Transmit request definitions.
 */

#define	EXEXTSUCCESS	0		/* successfull completion */
#define EXEXT1RETRY	1		/* success, 1 retry */
#define EXEXT2RETRYS	2		/* success, 2+ retrys */
#define EXEXTERR16	0x10		/* failed, 16 collisions */
#define EXEXTDISCON	0x80		/* failed, network disconnected */
#define EXEXTNOLINK	0xa1		/* failed, not in link level mode */

/*  
 *  Ethernet Transmit request message format.
 */

struct	exext {
	struct	exqmh	exth;		/* general header */
	unsigned char ext_numblk;	/* number of data blocks */
	unsigned char ext_slot;		/* address slot */
	struct extd {
		ushort	ext_dlen;	/* data length */
		ushort	ext_doff;	/* data buffer offset */
		ushort	ext_dseg;	/* data buffer segment */
	} extd[4];
	struct	cb	*ext_cb;
	ushort	ext_flag;
};

/*
 *  Ethernet Receive request definitions.
 */

#define	EXEXRSUCCESS	0		/* successfull completion */
#define EXEXRERR16	0x10		/* failed, 16 collisions */
#define EXEXRDISCON	0x80		/* failed, network disconnected */
#define EXEXRNOLINK	0xa1		/* failed, not in link level mode */
#define EXEXROVFL	(1<<2)		/* packet overflow reply code bit */
#define EXEXRSHRT	(1<<4)		/* short packet reply code bit */
#define EXEXRCRC	(1<<5)		/* CRC error packet reply code bit */
#define EXEXRALIGN	(1<<6)		/* packet alignment reply code bit */

/*  
 *  Ethernet Receive request message format.
 */

struct	exexr {
	struct	exqmh	exrh;		/* general header */
	unsigned char exr_numblk;	/* number of data blocks */
	unsigned char exr_slot;		/* address slot */
	struct exrd {
		ushort	exr_dlen;	/* data length */
		ushort	exr_doff;	/* data buffer offset */
		ushort	exr_dseg;	/* data buffer segment */
	} exrd[4];
	struct cb *exr_cb;
};

