
/* SID @(#)sioc.h	5.1 */
/* @(#)sioc.h	1.0 11/14/84 */

/*
 * Global defines which are used to describe the sioc 
 * hardware.
 */

/*
 * z8002 processor fcw 
 */

#define	SYS	(1<<14)		/* system mode */
#define	VIE	(1<<12)		/* vectored interrupt enable */
#define	NVIE	(1<<11)		/* non vecotred interrupt enable */

/*
 * z8002 refresh counter
 */

#define	RFRESH	0x9e00		/* proper value for the refresh control wd */

/*
 * The next to define's are used throughout the si driver to determine 
 * if code is compiled for the additional SIOC's.  Setting these
 * defines to 0 will cause exclusion of code assoicated to each SIOC.
 * Setting them non-zero causes inclusion of said code.
 */

#define	SIOC1DEF	0		/* SIOC1 */
#define SIOC2DEF	0		/* SIOC2 */
#define SIOC3DEF	0		/* SIOC3 */
#define SIOC4DEF	0		/* SIOC4 */


/*
 * SIOC interrupt assignments.
 */

#define SIINT0	( 0x08 << 1 )
#define SIINT1	( 0x04 << 1 )
#define SIINT2	( 0x10 << 1 )
#define SIINT3  ( 0x40 << 1 )
#define SIINT4  ( 0x01 << 1 )

/*
 * sioc multibus port addresses set into address select jumpers and switches
 */

#define PSIOC(k) (0xff7f-0x10*k)
#define	PSIOC0	0xff7f		/* address for sioc 0 */
#define	PSIOC1	0xff6f		/* address for sioc 1 */
#define	PSIOC2	0xff5f		/* address for sioc 2 */
#define PSIOC3	0xff4f		/* address for sioc 3 */
#define PSIOC4	0xff3f		/* address for sioc 4 */

/*
 * misc i/o ports
 */

#define	PCA	0x40d0		/* channel attention port */
#define	CLRINT	(1<<2)		/* clear sioc board interrupt */
#define	ATTN	(1<<1)		/* attn bit in channel attention port */
#define	RESET	(1<<0)		/* reset the sioc board */

#define	PWUA	0x4080		/* wake up address switches and jumpers */
#define	PRETI	0x40f0		/* reti port */
#define	P796MSB	0x40c0		/* 6 most sig bits of 796 bus address */

#define	PSTAT	0x40a0		/* status latch address */
#define	MSWIT	0xf0		/* mask for diagnostic switches */
#define	SSWIT	4		/* lsb of diagnostic switches field */
#define	PERR	(1<<3)		/* parity error in ram */

#define	PCMD	0x40b0		/* command latch */
#define	SLEDS	4		/* lsb of leds field */
#define	MLEDS	0xf0		/* mask for leds field */
#define	INT796	(1<<0)		/* set 796 bus interrupt line */
#define	RESPERR	(1<<1)		/* reset parity error */
#define	PERREN	(1<<2)		/* parity error enable */
#define	PIOREQ	(1<<3)		/* sets the pio dma request latch */
#define	CMDINIT	(15<<SLEDS)	/* initial value for cmd latch
					   leds off, parity check off,
					   796 bus int off */

#define P796ARBINT	0x4013	/* ctc port address for 796 bus grant */
#define P796TIMER	0x4020	/* ctc port address for 796 timer */
#define P796CATTN	0x4003	/* ctc port address for channel attention */

#define	PSIO0CMD	0x4042	/* sio port 0 command address */
#define	PSIO0DATA	0x4040	/* sio port 0 data address */
#define	PSIO0CTC	0x4000	/* ctc baud clock for sio 0 */

/*
 * memory layout
 */

#define	RAM		0x4000		/* start of ram memory */
#define	MBUS		0xc000		/* start of multibus memory */
#define	MBADRADD	(long)0xc000	/* starting address of multibus area */
#define	MBADRMSK	(long)0x3fff	/* mask for lower 14 bits */
#define MBADRSPACE	0x4000		/* size of sioc multibus adx space */
#define	MBMSBMSK	(long)0x7f	/* multibus mem msb adx mask */
#define	MBMSBSHFT	(long)13	/* multibus mem msb adx shift */

/* 
 * declarations for c code.
 */

#ifndef ASSM
extern int pdmac[];
extern int pdmaa[];
extern int pdmax[];
extern int ppioc[];
extern int ppiod[];
extern int psioc[];
extern int psiod[];
extern int pctc[];
extern int cmdsav;
extern int defspeed;
#endif


/*
 * interrupt ids for various ctc interrupts.
 */

#define	IDCHANATTN	70	/* channel attention interrupt */
#define	ID796BUSGRANT	78	/* 796 bus grant */
#define	IDRTCLK		92	/* real time clock */
#define	IDDMA0END	84	/* dma 0 end */
#define	IDDMA1END	86	/* dma 1 end */
#define	IDDMA2END	94	/* dma 2 end */


/*
 * fields in the flags param of sioparam
 */

#define	RAW	040
#define	ODDP	0100
#define	EVENP	0200
