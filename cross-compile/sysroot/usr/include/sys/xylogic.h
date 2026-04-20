
/*
 * Revision Level 5.1
 * Last Delta     4/22/86 23:50:05
 * File Name      xylogic.h
 */


	/* xylogic registers (pg. 7) */
#ifndef PIRATE
#define XY_LO_REL	0xee40	/* low  order byte of relocation register */
#define XY_HI_REL	0xee41	/* high order byte of relocation register */
#define XY_LO_ADDR	0xee42	/* low  order byte of address register */
#define XY_HI_ADDR	0xee43	/* high order byte of address register */
#define XY_CSR		0xee44	/* controller status register */
#define XY_RESET_REG	0xee45	/* read from this reg resets controller*/
#else
/* low  order byte of relocation register */
#define XY_LO_REL(x)	(0xee40+(0x10*(x)))
/* high order byte of relocation register */
#define XY_HI_REL(x)	(0xee41+(0x10*(x)))
/* low  order byte of address register */
#define XY_LO_ADDR(x)	(0xee42+(0x10*(x)))
/* high order byte of address register */
#define XY_HI_ADDR(x)	(0xee43+(0x10*(x)))
/* controller status register */
#define XY_CSR(x)	(0xee44+(0x10*(x)))
/* read from this reg resets controller*/
#define XY_RESET_REG(x)	(0xee45+(0x10*(x)))
#endif


	/* controller status register */

#define	XY_DRDY		0x1	/* drive ready           (R)   */
#define	XY_AACK		0x2	/* attention acknowledge (R)   */
#define	XY_AREQ		0x4	/* attention request     (R/W) */
#define	XY_ADRM		0x8	/* addressing mode       (R)   */
#define	XY_IPND		0x10	/* interrupt pending     (R/W) */
#define	XY_DERR		0x20	/* double error          (R)   */
#define	XY_ERR		0x40	/* general error bit     (R/W) */
#define	XY_GBSY		0x80	/* go/busy bit           (R/W) */

	/* unix configuration data */

/*#define XY0		6	/* major #. move to elog.h for real driver */

	/* command block structure (pg 10) */
	/* read using byte mode DMA (pg 5) */

struct iopb {
	unsigned char  imode;		/* interrupt mode */
	unsigned char  comm;		/* disk command */
	unsigned short stat;		/* status word */
	unsigned char  drive;		/* drive type, unit select */
	unsigned char  throt;		/* throttle */
	unsigned char  sect;		/* sector address */
	unsigned char  head;		/* head address */
	unsigned short cyl;		/* cylinder address */
	unsigned short scnt;		/* sector count */
	unsigned short data;		/* data address */
	unsigned short datar;		/* data relocation */
	unsigned char  res;		/* reserved */
	unsigned char  hdofst;		/* head offset */
	unsigned short niop;		/* next iopb address */
	unsigned char  eccml;		/* ecc mask pattern low */
	unsigned char  eccmh;		/* ecc mask pattern high */
	unsigned short ecca;		/* exx bit address */
};

	/* command byte (IOPB byte 0) */

#define	XY_NOP		0x0		/* no operation */
#define XY_WRITE	0x1		/* write to disk */
#define	XY_READ		0x2		/* read from disk */
#define	XY_WTH		0x3		/* write track headers */
#define	XY_RTH		0x4		/* read track headers */
#define	XY_SEEK		0x5		/* seek */
#define	XY_RSET		0x6		/* drive reset */
#define	XY_FORMAT	0x7		/* write format */
#define	XY_RHDE		0x8		/* read H-D-E */
#define XY_STATUS	0x9		/* read drive status */
#define	XY_WHDE		0xa		/* write H-D-E */
#define	XY_SET		0xb		/* set drive size */
#define XY_TEST		0xc		/* self test */
#define XY_RESERVED	0xd		/* reserved */
#define	XY_BUFFIL	0xe		/* buffer fill */
#define	XY_BUFDMP	0xf		/* buffer dump */

#define	XY_IEN		0x10		/* interrupt enable */
#define	XY_CHEN		0x20		/* chain enable */
#define	XY_RELO		0x40		/* relocate enable */
#define	XY_AUD		0x80		/* automatic update */

	/* Interrupt mode/funstion modification (IOPB byte 1) */

#define	XY_ECC0		0x0		/* ecc correction mode 0 */
#define	XY_ECC1		0x1		/* ecc correction mode 1 */
#define	XY_ECC2		0x2		/* ecc correction mode 2 */
#define	XY_ECC3		0x3		/* ecc correction mode 3 */
#define	XY_EEF		0x4		/* enable extended function */
#define	XY_ASR		0x8		/* automatic seek retry */
#define	XY_HDP		0x10		/* hold dual port */
#define	XY_IERR		0x20		/* Interrupt on error */
#define	XY_IEI		0x40		/* interrupt on each IOPB */
					/* 0x80 reserved */

	/* status byte 1 (IOPB byte 2) */

#define	XY_DONE		0x1		/* operation done */
#define	XY_CTYP		0x1c		/* mask for controller type */
#define	XY_ERRS		0x80		/* error summary */

	/* status byte 2 (IOPB byte 3) defined as needed */

	/* Throttle (IOPB byte 4) */

#define	XY_BW		0x80		/* transfer mode byte/word */

/* ioctl commands to turn on/off profiling */
#define XYONPRF  1			/* turn on profiling */
#define XYOFFPRF 2			/* turn off profiling */
#define XYZPRF   3			/* zero profiling tables */

#define xydtoc(x)	(minor(x)/(XYLOGVOL*XYUNITS))
#define xyutoc(x)	(minor(x)/XYUNITS)
#define xydtou(x)	((minor(x)%XYLOGVOL)%XYUNITS)
