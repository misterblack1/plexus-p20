/*
 * This source contains defines which are related to the z8001
 * and the plexus memory management system. There are a few
 * defines having to do with the plexus implementation in other
 * header files but the majority are in this source.
 */

/*
 * The folowing determines the maximum number of logins on the
 * system by controlling the number of ttys open for both FREAD
 * and FWRITE simultaneoulsy.
 */

/*
 * The following define variables used by `csv' and `cret'
 */

#define	SAFETYZONELEN	0x80		/* size of safety zone */
#define	FIRSTSAVEREG	r8		/* first reg of C reg variables */
#define	NUMSAVEREG	6		/* number of C reg variables */
#define LENSAVEREG	2*NUMSAVEREG	/* # bytes in C reg variables */


/* All commands that access the disk copy of the opsys should
 * use following:
 */

#define UNIXNAME "/sys3"

/*
 * These defines have to do with the way that a logical address
 * space of 64k bytes is split into pages. 
 */

#define	PAGESIZE	0x800		/* size of a logical page in bytes */
#define	S_PAGENUM	11		/* starting bit of logical page num */
#define	M_PAGENUM	0x1f		/* mask for logical page num field */
#define	B_PAGENUM	(M_PAGENUM << S_PAGENUM)	/* page num field */
#define	M_BIP		0x7ff		/* mask for byte in logical page */
#define	NUMLOGPAGE	32		/* logical pages per address space */
#define	FIRSTPPN	0x200		/* first physical page number */


/*
 * z8001 flags and control word (fcw)
 */

#define	S_CARRY	7			/* carry bit */
#define	S_FLGS	2			/* lsb of flags */
#define	S_SYS	14			/* sys/normal mode flag bit */
#define	S_NVI	11			/* non vectored interrupt enable */
#define	S_VI	12			/* vectored interrupt enable */
#define	S_SEG	15			/* segmented mode enable */
#define	S_SINGL	0			/* fake for single step */

#define	M_FLGS	077			/* mask for flags */

#define	B_SYS	(1 << S_SYS)		/* sys bit */
#define	B_NVI	(1 << S_NVI)		/* nvi bit */
#define	B_VI	(1 << S_VI)		/* vi bit */
#define	B_SEG	(1 << S_SEG)		/* seg bit */
#define	B_SINGL	(1 << S_SINGL)		/* single step fake bit */
#define	B_CARRY	(1 << S_CARRY)		/* carry bit */


/*
 * definition of the various segment numbers.
 */

#define	PRMDSEG	(0 << 8)		/* prom data seg */
#define	PRMISEG	(1 << 8)		/* prom code seg */
#define	SDSEG	(2 << 8)		/* system data seg 0 */
#define	SISEG	(3 << 8)		/* system code seg 1 */
#define	SDSEG1 	(4 << 8)		/* system data seg 2 */
#define	SISEG1	(5 << 8)		/* system code seg 3 */
#define	SDSEG2	(6 << 8)		/* system data seg 4 */
#define	SISEG2	(7 << 8)		/* system code seg 5 */
#define	SDSEG3	(8 << 8)		/* system code seg 6 */
#define	SISEG3	(9 << 8)		/* system data seg 7 */
#define	SDSEG4	(10 << 8)		/* system code seg 8 */
#define	SISEG4	(11 << 8)		/* system data seg 9 */
#define PROFSEG (11 << 8)		/* profiling segment */
#define BFSEG	(12 << 8)		/* system buffer seg 0 */
#define BFSEG1	(13 << 8)		/* system buffer seg 1 */
#define BFSEG2	(14 << 8)		/* system buffer seg 2 */
#define BFSEG3	(15 << 8)		/* system buffer seg 3 */
#define	UDSEG	(16 << 8)		/* user data seg 0 */
#define	UISEG	(17 << 8)		/* user code seg 0 */
#define	UDSEG1	(18 << 8)		/* user data seg 1 */
#define	UISEG1  (19 << 8)		/* user code seg 1 */
#define	UDSEG2	(20 << 8)		/* user data seg 2 */
#define	UISEG2	(21 << 8)		/* user code seg 2 */
#define	UDSEG3	(22 << 8)		/* user data seg 3 */
#define	UISEG3	(23 << 8)		/* user code seg 3 */
#define	UDSEG4	(24 << 8)		/* user data seg 4 */
#define	UISEG4	(25 << 8)		/* user code seg 4 */
#define	XDSEG	(26 << 8)		/* temp data seg */
#define	XISEG	(27 << 8)		/* temp code seg */
#define	MBSEG	(28 << 8)		/* multibus I/O seg 0 */
#define	MBSEG1	(29 << 8)		/* multibus I/O seg 1 */
#define	MBSEG2	(30 << 8)		/* multibus I/O seg 2 */
#define	BADSEG	(31 << 8)		/* seg with all pages invalid */
#define	LIOSEG	(127 << 8)		/* cover for local i/o space */
#define	MINSEG	(0 << 8)		/* min seg in map */
#define	MAXSEG	(31 << 8)		/* max seg in map */
#define	SEGINC	(1 << 8)		/* seg increment */
#define	SEGTOMAP(seg)	((seg) >> 8)	/* xlate seg to map number */
#define	MAPTOSEG(map)	((map) << 8)	/* xlate map to seg number */
#define	LONGSEGADX	(0x8000)	/* used to make z8001 segmented adx */
#define ADDRMASK	(0xff00ffff)	/* used to clear garbage bits in adx */


/*
 * conversions between various forms of addressing
 */

/*
 * mapport(seg, page)
 *
 * returns the port address of the mapper word that is used to map the
 * `page'th logical page in the `seg'th segment. `seg' is in the z8001
 * format for a segment number (seg in upper byte).
 *
 * logadx(page)
 *
 * returns the logical 16 bit address for the first byte in the `page'th
 * logical page in an address space.
 *
 * segtoaddr(seg)
 *
 * returns segment number shifted up to high word
 */

#define	mapport(seg, page)	(P_M0 + ((((seg) >> 3) + (page)) << 1))
#define	logadx(page)	(page << S_PAGENUM)
#define segtoaddr(seg)	(seg << 16)
#define addrtoseg(addr)	(addr >> 16)

#define	P_M0		0x8000		/* base port address of mapper */
#define	P_MUBLK		mapport(SDSEG, 30)
#define	P_MSYSIO	mapport(SDSEG, 31)
#define	P_MXD0		mapport(XDSEG, 0)
#define	P_MXI0		mapport(XISEG, 0)
#define	P_MUD0		mapport(UDSEG, 0)
#define	P_MUI0		mapport(UISEG, 0)
#define	P_MMB0		mapport(MBSEG, 0)
#define P_MPROF0	mapport(PROFSEG,0)
#define	LA_UBLK		logadx(30)
#define	LA_SYSIO	logadx(31)


/*
 * field definitions for words in the mapping ram
 */

#define	M_PPN		0x0fff		/* mask for physical page number */
#define	S_RO		14		/* bit number of read only flg */
#define	S_IP		15		/* bit number of invalid page flg */
#define	S_EX		13		/* fake bit number of execute flg */
#define	B_RO		(1 << S_RO)	/* bit field for read only flg */
#define	B_IP		(1 << S_IP)	/* bit field for invalid page flg */
#define	B_EX		(1 << S_EX)	/* bit field for execute flg */


/*
 * multibus mapper interface
 */

#define	MBSDSEG		0xf		/* multibus seg which is mapped to
					   the system data seg */
#define	MBIOSEG		0xe		/* multibus seg which is mapped to
					   the multibus map seg */
#define MBBFSEG		0xd		/* multibus seg which is mapped to
					   the virtual buffer cache seg */
#define MBBFSEG1	0xc		/* second multibus seg for vbufs */
#define MBBFSEG2	0xb		/* third multibus seg for vbufs */
#define MBBFSEG3	0xa		/* fourth multibus seg for vbufs */

#define	P_MB0		0xff88		/* lowest port address of multibus
					   mapper ports */
#define	MINMBSEG	8		/* min mb seg number */
#define	MAXMBSEG	15		/* max mb seg number */
#define	MBSEGINC	1		/* mb seg increment */
#define NUMBUFMAPS	4		/* number of buffer segments */
#define NUMMBIOMAPS	3		/* number of multibus io segments */

/*
 * mb_mapport(seg)
 *
 * returns the port address of the word which maps the `seg'th multibus
 * memory segment. Multibus memory segments are numbered 0-15 but only
 * 8-15 actually have mapper slots since 0-7 reference normal multibus
 * system memory.
 */

#define	mb_mapport(s)	(P_MB0 + (s & 7))


/*
 * definitions for various sys call instructions
 */

#define	MINSCINST	0x7f00		/* numerically lowest legal sc */
#define	MAXSCINST	0x7f3f		/* numerically highest legal sc */
#define	BPTINST		0x7fff		/* breakpoint instruction */
#define	INDSCINST	0x7f00		/* inderect system call */
#define	M_SCCODE	0x3f		/* mask for sys call code */


/*
 * 8259 interrupt controllers
 *
 * Note that all data to/from the 8259s is shifted left 1 bit because
 * the data bus for the chips is hooked to d8 - d1 of the z8001 data bus
 * so that the interrupt ids generated by the 8259 will always be even.
 */

#define	MICW1		(0x19 << 1)	/* initialization for master */
#define	MICW2		(0x00 << 1)
#define	MICW3		(0x80 << 1)
#define	MICW4		(0x01 << 1)
#define	SICW1		(0x19 << 1)	/* initialization for slave */
#define	SICW2		(0x08 << 1)
#define	SICW3		(0x07 << 1)
#define	SICW4		(0x01 << 1)

#define	MPICMSK		(0x75 << 1)	/* initial master pic int mask */
#define	SPICMSK		(0xff << 1)	/* initial slave  pic int mask */
#define	NSEOI		(0x20 << 1)	/* non specific end of int command */
#define	READIR		(0x0a << 1)	/* read ir reg from pic */
#define	S_PICSS		(5 + 1)		/* bit number of ir for single step */
#define	CLKINT		(0x04 << 1)	/* bit field to mask clock int */
#define	TTYINT		(0x40 << 1)	/* bit field to mask lcltty int */
#define	SSINT		(0x20 << 1)	/* bit field to mask snglstep int */

#define	P_MPIC0		0x7f90		/* port num for master 8259 a0 = 0 */
#define	P_MPIC1		0x7f91		/* port num for master 8259 a0 = 1 */
#define	P_SPIC0		0x7f98		/* slave a0 = 0 */
#define	P_SPIC1		0x7f99		/* slave a0 = 1 */


/*
 * single step logic
 */

#define	P_SSINT		0x7fb2		/* port num for reset of single step 
					   interrupt request */


/*
 * ecc controller
 */

#define	P_ECC		0xff80		/* ecc chip 16 bit control port */
#define	ECCON		0x2000		/* ecc mode to check and correct */
#define	ECCOFF		0x4000		/* ecc mode to pass thru */


/*
 * memory request error status
 */

#define	P_MRE0		0xff90		/* status word 0 */
#define	P_MRE1		0xff98		/* status word 1 */
#define	P_MRE2		0xffa0		/* status word 2 */
#define	P_MRE3		0xffa8		/* status word 3 */
#define	P_MRINT		0xffa0		/* mem request int reset port */

#define	S_MBERR		0		/* multibus access caused error */
#define	S_Z8KERR	1		/* z8000 access caused error */
#define	S_ROERR		2		/* read only error */
#define	S_PIERR		3		/* page invalid error */
#define	S_ECCERR	7		/* ecc error of some kind */
#define	S_MECCERR	8		/* multi bit ecc error */
#define	S_PFLT		9		/* error caused by protect violation */
#define	S_SYNDROME	10		/* syndrome field */

#define	M_SYNDROME	0x3f		/* syndrome field mask */

#define	B_MBERR		(1 << S_MBERR)
#define	B_Z8KERR	(1 << S_Z8KERR)
#define	B_ROERR		(1 << S_ROERR)
#define	B_PIERR		(1 << S_PIERR)
#define	B_ECCERR	(1 << S_ECCERR)
#define	B_MECCERR	(1 << S_MECCERR)
#define	B_PFLT		(1 << S_PFLT)


/*
 * multibus control
 */

#define	P_MBTOINT	0x7fb3		/* multibus time out int reset */
#define	P_MBUNLOCK	0xff98		/* unlocks multibus access to memory */
#define	P_MBLOCK	0xff90		/* locks multibus access to memory */


/*
 * system control pio port
 */

#define	P_PIOACTL	0x7f8a		/* port a control */
#define	P_PIOADAT	0x7f88		/* port a data */
#define	P_PIOBCTL	0x7f8b		/* port b control */
#define	P_PIOBDAT	0x7f89		/* port b data */

#define	S_LEDS		0		/* first bit in leds field */
#define	S_TODDATA	0		/* first bit in tod data field */
#define	S_TODADX	4		/* first bit in tod adx field */

#define	M_LEDS		0xf		/* mask for leds field */
#define	M_TODDATA	0xf		/* mask for time of day data */
#define	M_TODADX	0xf		/* mask for time of day adx */

#define	B_RESETOFF	0x01		/* reset cpu (0 true) */
#define	B_LOKBUS	0x02		/* lock multibus out of memory */
#define	B_MAPOFF	0x04		/* mapping (0 true) */
#define	B_ECCOFF	0x08		/* ecc checking (0 true) */
#define	B_PIOHOLD	0x10		/* tod hold bit */
#define	B_PIOREAD	0x20		/* tod read bit */
#define	B_PIOWRITE	0x40		/* tod write bit */
#define	B_ALLOWRESET	0x80		/* allows cpu reset (1 true) */

#define	PIOACTL		0xcf		/* initialization for pio */
#define	PIOAWRT		0x00		/* port a, all outputs */
#define	PIOBCTL		0xcf		/* port b in control mode */
#define	PIOBWRT		0x00		/* port b, normally all outputs */
#define	PIOBRD		0x0f		/* port b, bits 0-3 inputs, this is
					   used for accessing the tod clock */

#define DISKLED		0x01		/* led indicating wait for disk */
#define TAPELED		0x02		/* led indicating wait for tape */
#define USERLED		0x04		/* led indicating user running */
#define SWAPLED		0x08		/* led indicating swap occurrence */
#define CLKLED		0x08		/* led indicating clock tick */


/*
 * real time clock
 */

#define	P_CLKINT	0x7fb1		/* real time clock int reset */
#define	P_CTC0		0x7f80		/* ctc port 0 */
#define	P_CTC1		0x7f81		/* ctc port 1 */

#define	CTC0CTL		0x07		/* control word for ctc 0 */
#define	CTC0TIM		125		/* count word for ctc 0 */
#define	CTC1CTL		0x47		/* control word for ctc 1 */
#define	CTC1TIM		50		/* count word for ctc 1 */


/*
 * floating point processor chip interface
 */

#define	P_FPPDATA	0x7fc0		/* data port for fpp */
#define	P_FPPCMD	0x7fc1		/* cmd port for fpp */


/*
 * system diagnostic switches
 */

#define	P_SWIT		0x7fa8		/* switches port */


/*
 * diagnositc usart
 */

#define	P_USDATA	0x7fa0		/* usart data */
#define	P_USSTAT	0x7fa1		/* usart status */
#define	P_USCTL		0x7fa2		/* usart control */
#define	P_USCMD		0x7fa3		/* usart command */

/* profiling defines and variables */

#define N_PROFREGSAVED	3		/* # of registers saved, starting */
					/* with r1, on entry to t_prof    */
#define PROFSF		1		/* profiling scaling factor. 
					 * determines granularity of pc
					 * and number of pages used:
					 *  0 -- every other pc, 32 pages
					 *  1 -- every 2^2 pc, 16 pages
					 *  2 -- every 2^3 pc, 8 pages
					 *  up to
					 *  5 -- every 2^6 pc, 1 page
					 */
