#ifdef vax
#define	UBA_DEV	((int)(&umemvad) - 0160000)	/* sys virt of device regs */
extern	umemvad;
extern struct uba_regs ubavad;

/* Configuration Register */
#define	CFGFLT	0xfc000000	/* SBI fault bits */
#define	ADPDN	0x00800000	/* Adapter Power Down */
#define	ADPUP	0x00400000	/* Adapter Power Up */
#define	UBINIT	0x00040000	/* UNIBUS INIT is asserted */
#define	UBPDN	0x00020000	/* UNIBUS Power Down */
#define	UBIC	0x00010000	/* UNIBUS Initialization */
 
/* Control Register */
 
#define	UBAIE	0x78	/* normal IEs */
#define	CNFIE	0x4	/* Configuration IE */
#define	UPF	0x2	/* UNIBUS Power Fail */
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
 
/* Map Register */
 
#define	MRV	0x80000000	/* Map Register Valid */
#define	BO	0x02000000	/* Byte Offset Bit */
#define	DPDB	0x01e00000	/* Data Path Designator Field */
#define	SBIPFN	0x000fffff	/* SBI Page Address Field */
#define	UAMSIZ 50

struct uba_regs {
	int uba_cnfgr,
	  uba_cr,
	  uba_sr,
	  uba_dcr,
	  uba_fmer,
	  uba_fubar,
	  pad1[2],
	  uba_brsvr[4],
	  uba_brrvr[4],
	  uba_dpr[16],
	  pad2[480],
	  uba_map[496];
};

#define	ubmdata(X)	(paddr_t)((paddr_t)X - ubcoffs)
#define	ubmrev(L, H)	(((((int)H&03)<<16)|((int)L&0xffff))+(int)ubcoffs)
extern paddr_t ubcoffs;
#else
#define	UBA_DEV	0
#define	ubmdata(X)	(paddr_t)((unsigned)X)
#define	ubmrev(L, H)	((((int)H&03)<<16)|((int)L&0xffff))
#endif
