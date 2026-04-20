/* SID @(#)uba.h	5.1 */
/* @(#)uba.h	6.1 */
extern struct uba *ubavad[];

/*
 * Macros to rename some unibus routines to provide
 * upward compatibility for existing user-written drivers.
 * (Drivers default to unibus adapter 0 (nexus 3).)
 */
#define ubmalloc(X,Y)	mubmalloc(X,Y,ubaindex(3))
#define ubmdev(X,Y)	mubmdev(X,Y,ubaindex(3))
#define ubmaddr(X,Y)	mubmaddr(X,Y,ubaindex(3))
#define ubmvad(X,Y)	mubmvad(X,Y,ubaindex(3))

/* bdpr VAX-11/750 only */
#define BDPERR	0x80000000	/* IOR of next two bits */
#define BDPNXM	0x40000000	/* Non-eXsistant Memory, write 1 to clear */
#define BDPUCE	0x20000000	/* UnCorrectable Error, write 1 to clear */
#define BDPURGE	1		/* Purge buffer data path */

/* Unibus adapter codes in device addresses: */
#define MAXUBA	4		/* 4 unibus adapters on 11/780; 1 on 11/750 */
#define UBA0	00000000	/* bits 18 & 19 encode uba number */
#define UBA1	01000000	/* bits 18 & 19 encode uba number */
#define UBA2	02000000	/* bits 18 & 19 encode uba number */
#define UBA3	03000000	/* bits 18 & 19 encode uba number */
#define UBANUM(x)	(((x)&03000000)>>18)
#define UBADEV(x)	((x)&0777777)
#define UNEX780(x)	(0x20000000+((x)*0x2000))
#define UMEM780(x)	(0x2007E000+((x)*0x40000))

/* Configuration Register */
#define	CFGFLT	0xfc000000	/* SBI fault bits */
#define	ADPDN	0x00800000	/* Adapter Power Down */
#define	ADPUP	0x00400000	/* Adapter Power Up */
#define	UBINIT	0x00040000	/* UNIBUS INIT is asserted */
#define	UBPDN	0x00020000	/* UNIBUS Power Down */
#define	UBIC	0x00010000	/* UNIBUS Initialization */
 
/* Control Register */
 
#define	UBAIE	0x7c	/* normal IEs */
#define	UPF	0x2	/* cause UNIBUS Power Fail */
#define	ADINIT	0x1	/* Adapter Init */
 
/* Status Register */
#define	RDTO	0x400	/* UNIBUS to SBI Read Data Timeout */
#define	RDS	0x200	/* Read Data Substitute */
#define	CRD	0x100	/* Corrected Read Data */
#define	CXTER	0x80	/* Command Transmit Error */
#define	CXTMO	0x40	/* Command Transmit Timeout */
#define	DPPE	0x20	/* Data Path Parity Error */
#define	IVMR	0x10	/* Invalid Map Register */
#define	MRPF	0x8	/* Map Register Parity Failure */
#define	LEB	0x4	/* Lost Error */
#define	UBSTO	0x2	/* UNIBUS Select Timeout */
#define	UBSSTO	0x1	/* UNIBUS Slave Sync Timeout */
 
/* BR Receive Vector register */
#define	AIRI	0x80000000	/* Adapter Interrupt Request */
#define	DIV	0x0000ffff	/* Device Interrupt Vector Field */
 
/* Data Path Register */
#define	BNE	0x80000000	/* Buffer Not Empty - Purge */
#define	BTE	0x40000000	/* Buffer Transfer Error */
#define	DPF	0x20000000	/* DP Function (RO) */
#define	BUBA	0x0000ffff	/* Buffered UNIBUS Address */
#define	NDP780	16		/* Number of Data Paths VAX-11/780 */
#define	NDP750	4		/* Number of Data Paths VAX-11/750 */
 
/* Map Register */
 
#define	MRV	0x80000000	/* Map Register Valid */
#define	LWAE	0x04000000	/* LongWord Access Enable */
#define	BO	0x02000000	/* Byte Offset Bit */
#define	DPDF	0x01e00000	/* Data Path Designator Field */
#define	SBIPFN	0x001fffff	/* SBI Page Address Field */
#define	UAMSIZ	40

struct uba_device {
	int	uba_addr;
	int	uba_num;
};

struct uba {
	union {
		struct r780 {
		    int	csr,
			cr,
			sr,
			dcr,
			fmer,
			fubar,
			pad[2],
			brsvr[4],
			brrvr[4],
			dpr[16];
		} r780;
		struct r750 {
		    int	bdp[4],
			dsr[4];
		} r750;
		int	fill[512];
	} ireg;
	int	map[496];
};

#define	_ub8	ireg.r780
#define	_ub5	ireg.r750
#define	ubacsr	_ub8.csr
#define	ubacr	_ub8.cr
#define	ubasr	_ub8.sr
#define	ubadcr	_ub8.dcr
#define	ubafmer	_ub8.fmer
#define	ubafubar	_ub8.fubar
#define	ubabrsvr	_ub8.brsvr
#define	ubabrrvr	_ub8.brrvr
#define	ubadpr	_ub8.dpr

#define	ubabdp	_ub5.bdp
#define	ubadsr	_ub5.dsr

#ifndef PLEXUS		/* BF 02nov84	*/
#define	ubmdata(X)	(paddr_t)((paddr_t)X - ubcoffs)
#define	ubmrev(L, H)	(((((int)H&03)<<16)|((int)L&0xffff))+(int)ubcoffs)
#else
#define UBA_DEV 0
#define ubmdata(X)	(paddr_t)((unsigned)X)
#define ubmrev(L, H)	((((int)H&03)<<16)|((int)L&0xffff))
#endif

#define	umdbase(X)	(X & 0x1ff)
#define	umdnseg(X)	((X >> 9) & 0x1ff)
#define	umdbdp(X)	((X >> 18) & 0xf)
#define	umduba(X)	((X >> 22) & 0x3)
/*
extern paddr_t ubcoffs;
BF 02nov84	*/
