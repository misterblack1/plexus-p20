/* SID */
/* @(#)m68.h	5.1 4/22/86 */

/*
 * various definitions for the plexus 68000 processor board
 */

#define SAFETYZONE	0x80		/* kludge stk area */
#define	PAGESIZE	4096		/* page size */
#define	S_PAGENUM	12		/* log base 2 page size */
#define	M_PAGENUM	0xfff		/* mask off page part */
#define M_BIP		0xfff		/* byte in page offset */
/* constant in user.h MUST match NUMLOGPAGE */
#define NUMLOGPAGE	512		/* 2 MB per address space JK*/
#define MAXBANK		64		/* max # 256kb main mem banks */
#define PGPERBANK	64		/* # 4kb pgs / 256kb bank */

/*
 * MC68000 status register definitions
 */

#define S_CARRY		0		/* carry bit */
#define S_SYS		13		/* sys/user flag bit */
#define S_TRACE		15		/* trace mode bit */
#define M_FLGS		0x1f		/* mask for cc bits */
#define M_INTM		0x700		/* mask for int bits */

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
	|User  | + |Page  | + |Cdma| + |  |
	|Prom  |   |Access|   |Bdma|   |HI|
	|Cache |	      |Io  |

	a define consists of a horizontal pass thru the above
	table using one entry from each vertical row. As an
	example:
		S + L + M + LO = SLMLO = System Logical Memory
					       LO address
*/

#define ULMLO		0x0		/* user log mem */
#define ULMHI		0x800000	/* user log mem */
#define UMLO		0x900000	/* user (page) map mem */
#define UMHI		0x902000	/* user (page) map mem */

#define SLMLO		0x0		/* sys log mem */
#define SLMHI		0x780000	/* sys log mem */
#define SLBLO		0x780000	/* sys log blk dma */
#define SLBHI		0x7c0000	/* sys log blk dma */
#define SLCLO		0x7c0000	/* sys log char dma */
#define SLCHI		0x800000	/* sys log char dma */

#define SMLO		0x902000	/* sys (page) map mem */
#define SMHI		0x903e00	/* sys (page) map mem */
#define BMLO		0x903e00	/* sys (page) map blk dma */
#define BMHI		0x903f00	/* sys (page) map blk dma */
#define CMLO		0x903f00	/* sys (page) map char dma */
#define CMHI		0x904000	/* sys (page) map char dma */

#define PLMLO		0x800000	/* prom mem */
#define PLMHI		0x810000	/* prom mem */

#define CLMLO		0x980000	/* cache log mem */
#define CLMHI		0x981000	/* cache log mem */
#define CPMLO		0x9c0000	/* cache page/dirty */
#define CPMHI		0x9c1000	/* cache page/dirty */

#define MBILO		0xb00000	/* multibus i/o low */
#define MBIHI		0xb10000	/* multibus i/o hi */
#define MBMLO		0xb80000	/* multibus mem low */
#define MBMHI		0xc00000	/* multibus mem hi */

#define NLOGDMA		(((SLBHI-SLBLO)>>12)-1)
#define DMAIOPG		(CMHI-sizeof(unsigned))
#define LA_UBLK		0x77c000	/* UPAGE address */
#define LA_SYSIO	0x7ff000	/* system i/o page */

#define ICPLLO		0x7f0000	/* ICP logical lo addr */
#define ICPPLO		(CMHI-16*sizeof(unsigned)) /* ICP page map lo */

#define M_PPN		0xfff		/* mask for phys pg number */
#define S_RAM		15		/* page mapped to static ram */
#define B_RAM		0x8000		/* bit field for mapped to static ram*/

#define S_REF		31		/* bit number of referenced bit */
#define B_REF		0x80000000
#define S_DIRTY		30		/* bit number of dirty bit */
#define B_DIRTY		0x40000000
#define S_NOREAD	29		/* bit number of no read bit */
#define B_NOREAD	0x20000000	/* mask for no read bit */
#define S_NOWRITE	28		/* bit number of no write bit */
#define B_NOWRITE	0x10000000	/* mask for no write bit */
#define S_NOEXEC	27		/* bit number of no execute bit */
#define B_NOEXEC	0x8000000	/* mask for no execute bit */
#define B_FAKEEXEC	0x4000000	/* fake execute bit */
#define B_ABS		0x2000000	/* absolute bit for sysphys */
#define M_IVP		(B_NOREAD|B_NOWRITE|B_NOEXEC)	/* mask for inv page */
#define M_MID		0xff0000	/* mask for map id field */

#define logadx(page)	((page) << S_PAGENUM)
#define addrtopg(addr)	((addr) >> S_PAGENUM)
#define setmapslot(mapbase,page,val)	mapslot((mapbase),(page)) = (val)
#define mapport(mapbase,page)	((mapbase) + ((page)<<2))
#define logtophys(x) (((*(unsigned *)mapport(SMLO,addrtopg(x)))<<S_PAGENUM)|((x) & M_BIP))
#define out_local(addr,val) *(unsigned char *)(addr) = (val)
#define in_local(addr)  (*(unsigned char *)(addr))

/* dedicated logical pages and map slots for
 * 	use in mapped movement of data
 */

#define STMP1		0x903c00	/* copyseg */
#define STMP2		0x903c04	/* copyseg */
#define STMP3		0x903c08	/* clearseg */
#define STMP6		0x903c18	/* fubyte */
#define STMP7		0x903c1c	/* unused */
#define STMP8		0x903c20	/* unused */
#define STMP9		0x903c24	/* subyte */
#define STMPA		0x903c28	/* copyin */
#define STMPB		0x903c2c	/* copyin */
#define STMPC		0x903c30	/* copyout */
#define STMPD		0x903c34	/* copyout */
#define STMPE		0x903c38	/* mmread */
#define STMPF		0x903c3c	/* mmread */
#define STMPG		0x903c40	/* mmwrite */
#define STMPH		0x903c44	/* mmwrite */
#define STMPJ		0x903c48	/* fushort */
#define STMPK		0x903c4c	/* sushort */
#define STMPL		0x903c50	/* fuword */
#define STMPM		0x903c54	/* fuword */
#define STMPN		0x903c58	/* suword */
#define STMPO		0x903c5c	/* suword */
#define STMPSTATIC	0x903c60	/* static ram */

#define SLTMP1		0x700000	/* copyseg */
#define SLTMP2		0x701000	/* copyseg */
#define SLTMP3		0x702000	/* clearseg */
#define SLTMP6		0x706000	/* fubyte */
#define SLTMP7		0x707000	/* unused */
#define SLTMP8		0x708000	/* unused */
#define SLTMP9		0x709000	/* subyte */
#define SLTMPA		0x70a000	/* copyin */
#define SLTMPB		0x70b000	/* copyin */
#define SLTMPC		0x70c000	/* copyout */
#define SLTMPD		0x70d000	/* copyout */
#define SLTMPE		0x70e000	/* mmread */
#define SLTMPF		0x70f000	/* mmread */
#define SLTMPG		0x710000	/* mmwrite */
#define SLTMPH		0x711000	/* mmwrite */
#define SLTMPJ		0x712000	/* fushort */
#define SLTMPK		0x713000	/* sushort */
#define SLTMPL		0x714000	/* fuword */
#define SLTMPM		0x715000	/* fuword */
#define SLTMPN		0x716000	/* suword */
#define SLTMPO		0x717000	/* suword */
#define SLTMPSTATIC	0x718000	/* static ram */


/* cpu control registers */

#define	P_CONTROL	0xaf0001
#define B_MBUNLOCK	0x1		/* 0 -> lock MB busy */
#define B_SBE		0x2		/* 0 -> disable sbe ints */

#define	P_LEDS		0xaf0003

#define P_RESET		0xaf0005
#define BOOTRESET	1
#define MEMERRRESET	2
#define BUSERRRESET	3
#define MBRESET		4
#define DEBUGRESET	5

#define	P_MAPID		0xaf0007

/* cpu status registers */

#define	P_STATUS	0xaf0001
#define M_SWITCH	0x2		/* int switch active */
#define B_DMABUSERR	0x8
#define B_MBLOCK	0x10
#define B_BUSERR	0x20
#define B_MEMERR	0x40
#define B_MBINIT	0x80

#define	P_SWITCHES	0xaf0003
#define M_BAUDRATE	0x07		/* console baud rate */
#define M_AUTOBOOT	0x08		/* autoboot switch */
#define M_CHARIO	0x30		/* console port control */
#define B_USNORM	0x00		/* normal port control */
#define B_USAPORT	0x10		/* console port A */
#define B_USBPORT	0x20		/* console port B */
#define B_USABPORT	0x30		/* console port both A and B */
#define M_STATRAM	0x40		/* static ram used at boot */
#define M_DIAGMODE	0x80		/* diagnostic startup mode */

#define	P_BUSERR	0xaf0005
#define MBTO		1
#define MMTO		2
#define MA23		4
#define MMUE		8
#define MACC		16
#define MZOI		32		/* zone of impropriety */

#define P_DMAERR	0xaf0007

#define	P_MREGERR	0xaf0009
#define B_SBEENABLED	0x2

#define	P_MADDERR	0xaf000b

#define P_RMAPID	0xaf000d

/* 2661 EPCI addresses */

#define P_USA	0xa80000		/* port A */
#define P_USB	0xa90000		/* port B */

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


/* define memory board latch and register addresses */

#define DIAGLO	0xad0002	/* low edc diagnostic latch (bits 0-15) */
#define DIAGHI	0xad0080	/* high edc diagnostic latch (bits 16-31) */
#define ENBSBE	0xad0100	/* enable single bit error detection */
#define DSBSBE	0xad0180	/* disable single bit error detection */
#define RLATCH	0xad0202	/* read error status latch */
#define RSTERR	0xad0280	/* reset error flipflop */

/* define edc diagnostic latch values */

#define CODE01		0x0200	/* corresponds to DIAGLO */
#define CODE23		0x0300  /* corresponds to DIAGHI */
#define CORRECT		0x2000	/* turn on error correction */
#define PASSTHRU	0x4000	/* turn on pass thru mode (overrides others) */

#define	EDCNORM		0x0000	/* normal operation */
#define DIAGGEN		0x0800	/* diagnostic generate mode */
#define DIAGDET		0x1000	/* diagnostic detect/correct mode */
#define EDCINIT		0x1800	/* initialize memory to zero */

/* define RLATCH values */

#define M_SYNDROME	0x7f	/* mask for syndrome code */
#define B_0SBE		0x80	/* single bit err (active low) */
#define B_0DBE		0x100	/* double bit err */
#define M_BANK		0x600	/* ad18 and ad19 */
#define B_0EXIST	0x8000	/* this 256kb bank exists */


/* Motorola Real-Time Clock plus RAM chip - MC146818 */

#define	CALSECS		0xae0001
#define CALSECALARM	0xae0003
#define CALMINS		0xae0005
#define CALMINALARM	0xae0007
#define CALHRS		0xae0009
#define CALHRALARM	0xae000b
#define CALDAY		0xae000d
#define CALDATE		0xae000f
#define CALMONTH	0xae0011
#define CALYEAR		0xae0013

#define CALREGA		0xae0015	/* REGA: r/w register */
#define CALREGB		0xae0017	/* REGB: r/w register */
#define CALREGC		0xae0019	/* REGC: read only register */
#define CALREGD		0xae001b	/* REGD: read only register */

#define DISKLED		0x80
#define TAPELED		0x40
#define USERLED		0x20
#define CLKLED		0x10
#define SWAPLED		0x8

#define UNIXNAME	"/unix"
