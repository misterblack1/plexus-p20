/* SID */
/* @(#)m68.h	5.3 10/7/86 */

/*
 * various definitions for the plexus 68000 ACP processor board
 */

#define SAFETYZONE	0x80		/* kludge stk area */
#define	PAGESIZE	4096		/* page size */
#define	S_PAGENUM	12		/* log base 2 page size */
#define	M_PAGENUM	0x1ff		/* mask off page part */
#define STARTADDR	0x2000		/* link load starting address */
#define M_BIP		0xfff		/* byte in page offset */
/* constant in user.h MUST match NUMLOGPAGE */
#define NUMLOGPAGE	512		/* max address space */
#define NUMPAGHALFMB	128		/* 512k address space */
#define NUMPAGTWOMB	512		/* 2 MB address space */

/*
 * MC68000 status register definitions
 */

#define S_CARRY		0		/* carry bit */
#define S_SYS		13		/* sys/user flag bit */
#define S_TRACE		15		/* trace mode bit */
#define M_FLGS		0x1f		/* mask for cc bits */
#define M_INTM		0x700		/* mask for int bits */
#define M_INTL		0x300		/* lowest to set int bits (level 3) */
#define STOP_I		0x2300		/* stop 68k with approp. ints enable */

#define B_CARRY		(1<< S_CARRY)
#define B_SYS		(1 << S_SYS)	/* sys bit */
#define B_TRACE		(1 << S_TRACE)	/* trace bit */

/*
	somewhat regular definitions for the address spaces
	on the plexus 68000 processor board which include such
	areas as system/user logical memory, system/user-mem/dma
	maps, prom space, etc.

	the layout of the defines words is:

	|Multibus|
	|System|   |Log   |   |Mem |   |LO|
	|User  | + |Page  | + | dma| + |  |
	|Prom  |   |Access|   |    |   |HI|

	a define consists of a horizontal pass thru the above
	table using one entry from each vertical row. As an
	example:
		S + L + M + LO = SLMLO = System Logical Memory
					       LO address
*/

#define ULMLO		0x0		/* user log mem */
#define ULMHI		0x200000	/* user log mem 2 MB memory */

#define UMLO		0x900800	/* user 0 (page) map mem */
#define UMHI		0x900c00	/* user 0 (page) map mem */

#define U1MLO		0x900c00	/* user 1 (page) map mem */
#define U1MHI		0x901000	/* user 1 (page) map mem */

#define SLMLO		0x0		/* sys log mem */
#define SLMHI		0x200000	/* sys log mem 2 MB memory */

#define SLDLO		0x0		/* sys log dma */
#define SLDHI		0x200000	/* sys log dma 2 MB memory */

#define SMLO		0x900000	/* sys (page) map mem */
#define SMHI		0x900400	/* sys (page) map mem */

#define DMLO		0x900400	/* sys (page) map dma */
#define DMHI		0x900800	/* sys (page) map dma */

#define PLMLO		0x800000	/* prom mem */
#define PLMHI		0x810000	/* prom mem */

#define NVRLO		0x880000	/* system non-volatile mem */
#define NVRHI		0x880800	/* system non-volatile mem */

#define MBILO		0xe00000	/* multibus i/o low */
#define MBIHI		0xf00000	/* multibus i/o hi */

#define MBMLO		0xd00000	/* multibus mem low */
#define MBMHI		0xe00000	/* multibus mem hi */

#define	DMATEXT		0xc40000	/* dma instruction space */
#define	DMADATA		0xc80000	/* dma instruction space */

#define DMAIOPG		(DMHI-sizeof(unsigned short))

#define M_PPN		0x1ff		/* mask for phys pg number */

#define MS_READ		12		/* bit number of read bit */
#define MB_READ		0x1000		/* mask for read bit */

#define MS_WRITE	13		/* bit number of write bit */
#define MB_WRITE	0x2000		/* mask for write bit */

#define MS_EXEC		14		/* bit number of execute bit */
#define MB_EXEC		0x4000		/* mask for execute bit */

#define B_FAKEEXEC	0x4000		/* fake execute bit */
#define M_IVP		0		/* mask for inv page */

/*
 *	LA_SYSTK and LA_UBLK are determined as follows:
 *
 *	(top of logical memory address) minus (size of one page) = LA_SYSTK
 *				     or
 *	 (0x200000) minus 0x1000 = LA_SYSTK = 0x1ff000 for 2 MB byte memories
 *
 *
 *	LA_SYSTK minus (size of two pages) = LA_UBLK
 *			or
 *	0x1ff000    minus 0x2000 = LA_UBLK = 0x1fd000 for 2 MB byte memories
 */
#define	LA_SYSTK	0x01ff000	/* system stack address */
#define	LA_UBLK		0x01fd000	/* UPAGE address */

#define logadx(page)	((page) << S_PAGENUM)
#define addrtopg(addr)	((addr) >> S_PAGENUM)
#define setmapslot(mapbase,page,val)	mapslot((mapbase),(page)) = (val)
#define mapport(mapbase,page)	((mapbase) + ((page)<<1))
#define logtophys(x) (((*(unsigned *)mapport(SMLO,addrtopg(x)))<<S_PAGENUM)|((x) & M_BIP))

/* 
 * out_local and in_local are for word transfers,
 * bout_local and bin_local are for byte transfers (sios, for example )
 */

#define out_local(addr,val) *(unsigned short *)(addr) = (val)
#define in_local(addr)  (*(unsigned short *)(addr))
#define bout_local(addr,val) *(unsigned char *)(addr) = (val)
#define bin_local(addr) (*(unsigned char *)(addr))

/*
 *  BASE address for SIO registers
 */
#define	SIOBASE		0xa00000
#define	TURNON		0		/* turn on modem lines */
#define	TURNOFF		1		/* turn off modem lines */

/* dedicated logical pages and map slots for
 * 	use in mapped movement of data
 */

#define STMP1		0x9003f8	/* copyseg */
#define STMP2		0x9003f6	/* copyseg */
#define STMP3		0x9003f4	/* clearseg */
#define STMP6		0x9003f2	/* fubyte */
#define STMP9		0x9003f0	/* subyte */
#define STMPA		0x9003ee	/* copyin */
#define STMPB		0x9003ec	/* copyin */
#define STMPC		0x9003ea	/* copyout */
#define STMPD		0x9003e8	/* copyout */
#define STMPJ		0x9003e6	/* fushort */
#define STMPK		0x9003e4	/* sushort */
#define STMPL		0x9003e2	/* fuword */
/* #define STMPM		0x9003e0	/* fuword */
/* #define STMPN		0x9003de	/* suword */
/* #define STMPO		0x9003dc	/* suword */

/*	slots of smalloc space in logical system memory 	*/
#define DMASLOT1	0x9003e0	/* startdma */
#define DMASLOT2	0x9003de	/* startdma */
#define DMASLOT3	0x9003dc	/* startdma */
#define DMASLOT4	0x9003da	/* startdma */
#define DMASLOT5	0x9003d8	/* startdma */
#define DMASLOT6	0x9003d6	/* startdma */
#define DMASLOT7	0x9003d4	/* startdma */
#define DMASLOT8	0x9003d2	/* startdma */
#define DMASLOT9	0x9003d0	/* startdma */

/*	slots of smalloc space in logical dma memory 		*/
#define DMBFSL1		0x9007e0	/* dma map page for DMABUF */
#define DMBFSL2		0x9007de	/* dma map page for DMABUF */
#define DMBFSL3		0x9007dc	/* dma map page for DMABUF */
#define DMBFSL4		0x9007da	/* dma map page for DMABUF */
#define DMBFSL5		0x9007d8	/* dma map page for DMABUF */
#define DMBFSL6		0x9007d6	/* dma map page for DMABUF */
#define DMBFSL7		0x9007d4	/* dma map page for DMABUF */
#define DMBFSL8		0x9007d2	/* dma map page for DMABUF */
#define DMBFSL9		0x9007d0	/* dma map page for DMABUF */

#define SLTMP1		0x1fc000 	/* copyseg */
#define SLTMP2		0x1fb000 	/* copyseg */
#define SLTMP3		0x1fa000	/* clearseg */
#define SLTMP6		0x1f9000	/* fubyte */
#define SLTMP9		0x1f8000	/* subyte */
#define SLTMPA		0x1f7000	/* copyin */
#define SLTMPB		0x1f6000	/* copyin */
#define SLTMPC		0x1f5000 	/* copyout */
#define SLTMPD		0x1f4000 	/* copyout */
#define SLTMPJ		0x1f3000	/* fushort */
#define SLTMPK		0x1f2000	/* sushort */
#define SLTMPL		0x1f1000	/* fuword */
/* #define SLTMPM		0x1f0000	/* fuword */
/* #define SLTMPN		0x1ef000	/* suword */
/* #define SLTMPO		0x1ee000	/* suword */
/*	DMABUF		0x1f0000	   startdma */
/*	DMABUF		0x1ef000	   startdma */
/*	DMABUF		0x1ee000	   startdma */
/*      DMABUF		0x1ed000	   startdma */
/*      DMABUF		0x1ec000	   startdma */
/*      DMABUF		0x1eb000	   startdma */
/*      DMABUF		0x1ea000	   startdma */
/*      DMABUF		0x1e9000	   startdma */
#define DMABUF		0x1e8000	/* startdma DMABUF starts here and 
					   goes for nine pages */

#define NUMTMPSLOTS	21
#define NUMDMASLOTS	9

/*
 *	defines for WREG_STAT
 */
#define CLKLED		0x01
#define USERLED		0x02
#define W0_EMM		0x400		/* enable memory map control line */

/*
 *	defines for RREG_STAT
 */
#define R0_CHATTN	0x8000		/* Unix to ACP channel attention */
#define R0_MBIRQ	0x4000		/* ACP to multibus interrupt bit */
#define R0_DMAATTN	0x2000		/* ACP interrupt TO the DMA */
#define R0_CPUATTN	0x1000		/* ACP interrupt FROM the DMA */
#define R0_BOOTSW	0x0080		/* ACP boot/diag enable switch */
#define R0_ENDBGSW	0x0040		/* ACP Switch to enable debugger */

/*
 *	defines for WREG_CNTL
 */
#define W4_SDMATTN	0x0080		/* set dma attn interrupt request */
#define W4_SMBIRQ	0x0040		/* set mb interrupt request */
#define W4_CHIACK	0x0020		/* channel attention Iack */
#define	W4_RTCIACK	0x0010		/* real time clock Iack from CPU */
#define W4_DMAIACK	0x0008		/* dma interrupt Iack */
#define W4_DMASS	0x0004		/* dma single step */
#define W4_DMARUN	0x0002		/* dma run */
#define W4_DMARST	0x0001		/* dma reset */

/* following are write registers to control the Multi-bus interrupts */
#define RESET_BD	0xA00210 	/* write-- reset RESET.BD interrupt */
#define RST_CH_ATN	0xA00212 	/* write-- reset CH.ATTN interrupt */
#define	RST_MBIACK	0xA00214 	/* write-- reset MBIACK */
#define	SET_MB_INT	0xA00216 	/* write-- set MULTIBUS interrupt */
		 			/* A write to addr 0xA00216 will assert
					 * an interrupt on the multibus.  The 
					 * level of ther interrupt is 
					 * determined by what has been written 
					 * into to the interrupt levels of 
					 * 0xB00000.
					 */

/*
 *	bit defines for RREG_ERROR
 */
#define R4_FC2		0x8000		/* function code 2 */
#define R4_FC1		0x4000		/* function code 1 */
#define R4_FC0		0x2000		/* function code 0 */
#define R4_WRITE	0x1000		/* write signal status */ 
#define R4_DMA		0x0800		/* DMA signal status */ 
#define R4_MBTO		0x0400		/* BUS timeout */
#define R4_PARHIGH	0x0200		/* parity high line mask */
#define R4_PARLOW	0x0100		/* parity low  line mask */
#define R4_ADDRMSK	0x00ff		/* Most significant 8 bits of addr  */
					/* which caused bus or parity error */
#define W0_REL		0x0080		/* reset error latch control line   */

#define MAXUNITS	16		/* max number of usarts in ACP */

/* ACP console (port 0) address */

#define P_USA	0xa00000		/* port 0 */

/*
 *  the following also points to the console in order to
 *  not change the logic in the <us> driver
 */

#define P_USB	0xa00000		/* port 0 */

#define P_USCTL		0x5
#define P_USCMD		0x7
#define P_USSTAT	0x3
#define P_USDATA	0x1

/* define stack offsets off of sp@ (w/o link) for subroutines */

#define ARG1		4
#define ARG2		8
#define ARG3		12
#define ARG4		16
#define ARG5		18
