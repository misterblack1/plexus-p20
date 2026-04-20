/* SID @(#)mtpr.h	5.4 */
/*
 *	processor register numbers
 */
  

#ifdef LUNDELL

/* define the location of the EPROM in system address space */
#define P_EPROM		0x00800000

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
#endif  /* LUNDELL */

#ifdef SCHROEDER

/* define the location of the EPROM in system address space */
#define P_EPROM		0x00800000

/* cpu control registers (writable) */
#define P_RSTBOOT	0xa00010	/* reset boot condition */
#define P_RSTPWRF	0xa00011	/* reset power fail condition */
#define P_RSTMB		0xa00012	/* reset multi-bus (start TINIT) */
#define P_RSTSWITCH	0xa00013	/* reset switch interrupt */
#define P_RSTERROR	0xa00014	/* reset error */
#define P_RSTFPP	0xa00015	/* reset floating point processor */
#define P_RSTSIO	0xa00016	/* reset SIO's */

#define	P_CONTROL	0xa00022
#define B_CACHE0EN	0x01		/* 1 says enable cache 0 */
#define B_CACHE1EN	0x02		/* 1 says enable cache 1 */
#define B_MBUNLOCK	0x08		/* 0 -> lock MB busy */
#define B_FRCMISS0	0x10		/* 1 says force misses in cache 0 */
#define B_FRCMISS1	0x20		/* 1 says force misses in cache 1 */

#define	P_LEDS		0xa00023

#define P_USRMULTI	0xa00026	/* controls user access to multi-bus */
#define B_UMIOENAB	0x01		/* enable user access to multi-bus io space */
#define B_UMMEMENAB	0x02		/* enable user access to multi-bus mem space */

#define	P_MAPID		0xa00027

/* cpu status registers (read only) */

#define	P_STATUS	0xa00020
#define B_PWRFAIL	0x01	/* 1 says reset by power restoration */
#define B_BOOTING	0x02	/* 1 says booting, addr bit 23 forced on */
#define B_MBINIT	0x04	/* 1 says multi-bus initialize active */
#define B_MBLOCK	0x08	/* 1 says multi-bus BUSY locked */
#define B_ERRORR	0x10	/* 1 says information in error register */
#define M_SWITCH	0x40	/* 1 says int switch active */
#define B_PFW		0x80	/* 1 says power fail warning */

#define	P_SWITCHES	0xa00021
#define M_BAUDRATE	0x07		/* console baud rate */
#define M_AUTOBOOT	0x08		/* autoboot switch */
#define M_CHARIO	0x30		/* console port control */
#define B_USNORM	0x00		/* normal port control */
#define B_USAPORT	0x10		/* console port A */
#define B_USBPORT	0x20		/* console port B */
#define B_USABPORT	0x30		/* console port both A and B */
#define M_STATRAM	0x40		/* static ram used at boot */
#define M_DIAGMODE	0x80		/* diagnostic startup mode */

#define P_RMAPID	0xa00027	/* readable current map id */

#define P_ERRORREG	0xa00028	/* error reason register (32 bits) */
#define B_ETIMEOUT	0x80000000	/* timeout error */
#define B_EMBACC	0x20000000	/* was multi-bus access */
#define B_ETRBZY	0x10000000	/* transmit busy -- says ETIMEOUT was for MB */
#define B_EMBERR	0x08000000	/* multi-bit main memory error */
#define B_ESBERR	0x04000000	/* single-bit main memory error */
#define B_EPAERR	0x02000000	/* page access error */
#define B_EIDERR	0x01000000	/* page user ID not equal error */
#define B_EDMACY	0x00800000	/* was a DMA cycle */
#define M_EFC		0x00700000	/* 68k bus operation FC code */
#define B_EBOOTING	0x00080000	/* booting enabled, bit 23 of addr forced on */
#define B_EREAD		0x00040000	/* read operation */
#define M_ESIZ		0x00030000	/* 68k bus operation size code */
#define B_ECMISS	0x00008000	/* this cache operation missed */
#define B_EWBACK	0x00004000	/* error was on cache write back operation */
#define M_EPMA		0x00003fff	/* page number of physical page address */

#define P_ERRVADD	0xa0002c	/* virtual address of error */

/* 2661 EPCI addresses */
#define P_USA	0xb00000		/* port A */
#define P_USB	0xb80000		/* port B */

#define P_USCTL		0x2
#define P_USCMD		0x3
#define P_USSTAT	0x1
#define P_USDATA	0x0

/* define memory board latch and register addresses */
#define P_PHYSMEM	0x20000000	/* sys addr to direct phys mem */
#define P_PHYSMEMC	0x30000000	/* sys addr to direct phys mem stat/control */
#define DIAGLO	0x002		/* low edc diagnostic latch (bits 0-15) */
#define DIAGHI	0x080		/* high edc diagnostic latch (bits 16-31) */
#define ENBSBE	0x100		/* enable single bit error detection */
#define DSBSBE	0x180		/* disable single bit error detection */
#define RLATCH	0x202		/* read error status latch */
#define RSTERR	0x280		/* reset error flipflop */

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
#define	CALSECS		0xa80000
#define CALSECALARM	0xa80001
#define CALMINS		0xa80002
#define CALMINALARM	0xa80003
#define CALHRS		0xa80004
#define CALHRALARM	0xa80005
#define CALDAY		0xa80006
#define CALDATE		0xa80007
#define CALMONTH	0xa80008
#define CALYEAR		0xa80009

#define CALREGA		0xa8000a	/* REGA: r/w register */
#define CALREGB		0xa8000b	/* REGB: r/w register */
#define CALREGC		0xa8000c	/* REGC: read only register */
#define CALREGD		0xa8000d	/* REGD: read only register */

#define DISKLED		0x80
#define TAPELED		0x40
#define USERLED		0x20
#define CLKLED		0x10
#define SWAPLED		0x8
#endif  /* SCHROEDER */

#ifdef ROBIN

/* define the location of the EPROM in system address space */
#define P_EPROM		0x00800000

	/* output register addresses for the robin */

#define O_RSEL 0xE00000	/* reset selection , see below */
#define O_SC_C 0xE00006	/* scsi byte count */
#define O_SC_P	0xE0000A	/* scsi pointer register */
#define O_SC_R	0xE0000E	/* scsi register */
#define O_LEDS	0xE00010	/* led register */
#define O_MISC 0xE00016	/* misc. functions */
#define O_KILL 0xE00018	/* kill job / dma cpu */
#define O_TRCE	0xE0001A	/* rce/ tce for usarts */
#define O_INTE	0xE0001C	/* interupt register */
#define O_MAPID 0xe0001E	/* user id register */

	/* defines for O_RSEL  reset register */

#define	R_MULTERR	0xe00020  /* reset multibus interface error flag */
#define	R_SCSI_PFLG	0xe00040  /* reset scsi parity error flag */
#define	R_JOBINT	0xe00060  /* reset job processor software int */
#define	S_JOBINT	0xe00080  /* set job processor software int */
#define	R_DMAINT	0xe000a0  /* reset dma processor software int */
#define	S_DMAINT	0xe000c0  /* set dma processor int */
#define	R_CINTJ		0xe000e0  /* reset job clock interrupt */
#define R_CINTD		0xe00100  /* reset dma clock int */
#define	R_JBERR		0xe00120  /* reset job bus error flag */
#define	R_DBERR		0xe00140  /* reset dma bus error flag */
#define	R_MPERR		0xe00160  /* reset memory parity err flag SET ON RESET*/
#define	R_SWINT		0xe00180  /* reset switch interrupt */
#define	R_SCSIBERR	0xe001a0  /* reset scsi bus error flag */

	/* input register addresses for the robin */

#define I_PERR1 0xE00000	/* parity error latch */
#define I_PERR2 0xE00002	/* parity error latch */
#define I_MBERR 0xE00004	/* latches address on multibus error */
#define I_SC_C 0xE00006	/* scsi byte count */
#define I_SC_P	0xE0000A	/* scsi pointer register */
#define I_SC_R	0xE0000E	/* scsi register */
#define I_LEDS	0xE00010	/* led register */
#define I_USRT 0xE00012	/* usart register */
#define I_ERR	0xE00014	/* misc functoins */
#define I_MISC 0xE00016	/* misc. functions */
#define I_KILL 0xE00018	/* kill job / dma cpu */
#define I_TRCE	0xE0001A	/* rce/ tce for usarts */
#define I_INTE	0xE0001C	/* interupt register */
#define I_USER 0xE0001E	/* user number */

	/* bits in I_ERR */

#define B_MBTO	0x20

	/* scsi bits in register O_SC_R */

#define IOPTR  0x8000
#define MSGPTR 0x4000
#define CDPTR  0x2000
#define S_DRAM 0x1000
#define SC_RST 0x0800
#define SC_SEL	0x0400
#define SC_BSY	0x0200
#define ARBIT	0x0100
#define SCSIREQ 0x080
#define SCSIMSG 0x040
#define SCSIRST 0x020
#define SCSIIO	0x0010
#define SCSICD	0x0008
#define SCSIATN 0x004
#define SCSIACK 0x002
#define AUTO	0x0001


	/* scsi data buffers */

#define SCDBUF0 0xA70000
#define SCDBUF1 0xA70001
#define SCDBUF2 0xA70002
#define SCDBUF3 0xA70003
#define MY_ID	0x08


	/* defines for inter processor communication */

#define DZERO 0x8000
#define JZERO 0x4000
#define MAPENBL 0x0100
#define STARTJOB 0x0002
#define KILLJOB 0x0000
#define KILLDMA 0x0001

	/* defines for which cpu handles the int . A 1 in a bit says
	   the job cpu gets it */

#define CENJOB	0x04		/* Ints to clock are seperately enalbled */
#define CENDMA	0x08

	/* defines for miscellaneous register */

#define	B_UINTEN	0x1	/* enable ups interrupt */
#define	B_TINTEN	0x2	/* enable temperature interrupt */
#define B_CINTJEN	0x4	/* enable job's clock interrupt */
#define B_CINTDEN	0x8	/* enable dma's clock interrupt */
#define B_RESMB		0x10	/* reset multibus ACTIVE LOW */
#define B_HOLDMBUS	0x20	/* hold multibus */
#define B_DIAGUART	0x40	/* disable output to ttys */
#define	B_TBUSY		0x80	/* READ only */
#define B_ENMAP		0x100	/* enable mapping (active low) */
#define B_DISMAP	0x100	/* disables map (active hi ) */
#define	B_DIAGMB	0x200	/* put multibus into diagnostic mode */
#define B_DIAGPESC	0x400	/* force parity scsi parity error */
#define	B_DIAGPH	0x800	/* force parity error low byte */
#define B_DIAGPL	0x1000	/* force parity error hi byte */
#define B_SCSIDL	0x2000	/* enable diag latch (ACTIVE LOW) */
#define B_BOOTJOB	0x4000	/* force job's A23 high (ACTIVE LOW ) */
#define B_BOOTDMA	0x8000	/* force dma's A23 high (ACTIVE LOW ) */

/* non kill bits in I_KILL */

#define	B_INTDMA 0x4		/* dma processor interrupt */
#define B_INTJOB 0x8		/* job processor interrupt */
#define B_JOBP	0x80		/* on if we are the job processor */


/* locations of the various interrupt vectors */

#define I_MBUS_INT	0xf00003
#define I_SCSI_INT	0xf00009
#define I_PANIC_INT	0xf0000f

/*  68564 usart adresses */

#define P_U0	0xa00021		/* port 0 */
#define P_U1	0xa00001		/* port 1 */
#define P_U2	0xa10021		/* port 2 */
#define P_U3	0xa10001		/* port 3 */
#define P_U4	0xa20021		/* port 4 */
#define P_U5	0xa20001		/* port 5 */
#define P_U6	0xa30001		/* port 6 */
#define P_U7	0xa30021		/* port 7 */

/* Motorola Real-Time Clock plus RAM chip - MC146818 */

#define	CALSECS		0xd00001
#define CALSECALARM	0xd00003
#define CALMINS		0xd00005
#define CALMINALARM	0xd00007
#define CALHRS		0xd00009
#define CALHRALARM	0xd0000b
#define CALDAY		0xd0000d
#define CALDATE		0xd0000f
#define CALMONTH	0xd00011
#define CALYEAR		0xd00013

#define CALREGA		0xd00015	/* REGA: r/w register */
#define CALREGB		0xd00017	/* REGB: r/w register */
#define CALREGC		0xd00019	/* REGC: read only register */
#define CALREGD		0xd0001b	/* REGD: read only register */

#define DISKLED		0x4
#define TAPELED		0x4
#define USERLED		0x2
#define CLKLED		0x2
#define SWAPLED		0x8
#define DMALED		0x1

#endif  /* ROBIN */

#ifdef KICKER	 

/* define the location of the EPROM in system address space */
#define P_EPROM		0x08000000

/* cpu control registers (writable) */
#define P_RSTBOOT	0xa000010	/* reset boot condition */
#define P_RSTPWRF	0xa000011	/* reset power fail condition */
#define P_RSTVME	0xa000012	/* reset VME-bus (start TINIT) */
#define P_RSTSWITCH	0xa000013	/* reset switch interrupt */
#define P_RSTERROR	0xa000014	/* reset error */
#define P_RSTFPP	0xa000015	/* reset floating point processor */
#define P_RSTSIO	0xa000016	/* reset SIO's */

/* cpu status registers (read only) */
#define	P_STATUS	0xa000020
#define	B_PWRFAIL	0x01		/* Power failure status */
#define	B_BOOTING	0x02
#define	B_VMEINIT	0x04		/* VME init bit */
#define	B_VMEBUSY	0x08		/* VME busy bit */
#define B_ERRORR	0x10		/* Error? */
#define B_SYSFAIL	0x20		/* vme sysfail */
#define	M_SWITCH	0x40		/* Switch int */
#define	B_PFW		0x80		/* Power Failure warning */

/* sense switches */
#define	P_SWITCHES	0xa000021
#define M_BAUDRATE	0x07		/* console baud rate */
#define M_AUTOBOOT	0x08		/* autoboot switch */
#define M_CHARIO	0x30		/* console port control */
#define B_USNORM	0x00		/* normal port control */
#define B_USAPORT	0x10		/* console port A */
#define B_USBPORT	0x20		/* console port B */
#define B_USABPORT	0x30		/* console port both A and B */
#define M_STATRAM	0x40		/* static ram used at boot */
#define M_DIAGMODE	0x80		/* diagnostic startup mode */

/* control registers */
#define	P_CONTROL	0xa000022
#define	B_CACHESEL	0x03
#define	B_CACHETEST	0x04		/* Cache test */
#define	B_VMEREQ	0x08		/* VME bus request */
#define	B_CACHESINGLE	0x10		/* Cache single set request */
#define B_ENSBEI	0x20		/* Enable single bit error interrupt */
#define	B_VMETEST	0x40 		/* VME test bit */

/* xmas lights */
#define	P_LEDS		0xa000023
#define DISKLED		0x80
#define TAPELED		0x40
#define USERLED		0x20
#define CLKLED		0x10
#define SWAPLED		0x8

/* user status / control register */
#define P_USC		0xa000024

/* controls user access to VME-bus */
#define P_USRVME	0xa000026	/* controls user access to VME-bus */
#define B_UVIOENAB	0x08	/* enable user access to VME-bus io space */

/* current task register */
#define	P_MAPID		0xa000026
#define P_RMAPID	0xa000026	/* readable current map id */
#define M_RMAPID	0x7ff		/* mask for map id */

/* error reason register (32 bits) */
#define P_ERRORREG	0xa000028	/* error reason register (32 bits) */
#define B_ETIMEOUT	0x80000000	/* timeout error */
#define B_ENOTCOMP	0x40000000	/* not complete */
#define B_UPDATE	0x20000000	/* not doing update */
#define B_ETRBZY	0x10000000	/* transmit busy--says ETIMEOUT was for VME */
#define B_EMBERR	0x08000000	/* multi-bit main memory error */
#define B_ESBERR	0x04000000	/* single-bit main memory error */
#define B_EPAERR	0x02000000	/* page access error */
#define B_EIDERR	0x01000000	/* page user ID not equal error */
#define B_EDMACY	0x00800000	/* was a DMA cycle */
#define M_EFC		0x00700000	/* 68k bus operation FC code */
#define B_EWBACK	0x00080000	/* not doing writeback */
#define B_EREAD		0x00040000	/* read operation */
#define M_ESIZ		0x00030000	/* 68k bus operation size code */
#define M_EPMA		0x0000ffff	/* page number of physical page address */

#define P_ERRVADD	0xa00002c	/* virtual address of error */

/* 2661 EPCI addresses */
#define P_USA	0xb000000		/* port A */
#define P_USB	0xb080000		/* port B */

#define P_USCTL		0x2
#define P_USCMD		0x3
#define P_USSTAT	0x1
#define P_USDATA	0x0

/* define memory board latch and register addresses */
#define P_PHYSMEM	0x30000000	/* sys addr to direct phys mem */
#define P_PHYSMEMC	0x30000000	/* sys addr to direct phys mem stat/control */
#define DIAGLO	0x000		/* low edc diagnostic latch (bits 0-15) */
#define DIAGHI	0x080		/* high edc diagnostic latch (bits 16-31) */
#define ENBSBE	0x100		/* enable single bit error detection */
#define DSBSBE	0x180		/* disable single bit error detection */
#define RLATCH	0x200		/* read error status latch */
#define RSTERR	0x280		/* reset error flipflop */

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
#define	CALSECS		0xa080000
#define CALSECALARM	0xa080001
#define CALMINS		0xa080002
#define CALMINALARM	0xa080003
#define CALHRS		0xa080004
#define CALHRALARM	0xa080005
#define CALDAY		0xa080006
#define CALDATE		0xa080007
#define CALMONTH	0xa080008
#define CALYEAR		0xa080009
#define CALREGA		0xa08000a	/* REGA: r/w register */
#define CALREGB		0xa08000b	/* REGB: r/w register */
#define CALREGC		0xa08000c	/* REGC: read only register */
#define CALREGD		0xa08000d	/* REGD: read only register */

#endif  /* KICKER */

#if defined(LUNDELL) || defined(SCHROEDER) || defined(ROBIN)
#define	LA_UBLK		0x77c000
#define LA_SYSIO	0x7ff000	/* i/o page address */
#ifdef SCHROEDER
#define MAXBANK		64		/* max # of 256Kb banks of ram */
#else
#define MAXBANK		64		/* max # of 256Kb banks of ram */
#endif
#else
/* KICKER */
#define	LA_UBLK		0x77c000	/* address of struct user u */
#define LA_SYSIO	0x7ff000	/* multibus i/o page address */
#define	MAXBANK		1024		/* Max. # of 256kb banks */
#define LA_VMESYSIO	0xfff000	/* VME bus i/o page address */
#endif

/* pseudo hardware regions for use by expand() and callers */
#define TREGION		0
#define DREGION		1
#define SREGION		2
