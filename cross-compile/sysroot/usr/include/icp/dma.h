
/* SID @(#)dma.h	5.1 */
/* @(#)dma.h	1.0 11/14/84 */

/*
 * definitions for the intel 8237-2 dma chip.
 */

/* command register */

#define	LOWDREQ		(1<<6)		/* dreq active low */
#define	HIGHDACK	(1<<7)		/* dack active high */
#define	EXTENDEDWRITE	(1<<5)		/* extended writes */

/* command type port address offsets */

#define	RSR		0		/* read status register */
#define	WCR		0		/* write command register */
#define	WRR		1		/* write request register */
#define	WSMRB		2		/* write single mask register bit */
#define	WMR		3		/* write mode register */
#define	CBPFF		4		/* clear byte pointer flip flop */
#define	RTR		5		/* read temp register */
#define	MCLR		5		/* master clear */
#define	WAMRB		7		/* write all mask register bits */

/* mode register */

#define	MCHAN		3		/* channel mask */
#define	SCHAN		0		/* channel field shift */
#define	READXFER	(2<<2)		/* memory to i/o device xfer */
#define	AUTOENAB	(1<<4)		/* auto initialize after eop */
#define	ADXDEC		(1<<5)		/* decrement address */
#define	DMNDMODE	(0<<6)		/* demand mode type xfer */
#define	SNGLMODE	(1<<6)		/* single mode type xfer */
#define	BLKMODE		(2<<6)		/* block mode type xfer */
#define	CASCADEMODE	(3<<6)		/* cascade mode */

/* request register */

#define	SETREQ		(1<<2)		/* set request bit */
#define	RESETREQ	(0<<2)		/* reset request bit */

/* mask register */

#define	SETMSK		(1<<2)		/* set mask bit */
#define	RESETMSK	(0<<2)		/* reset mask bit */

/* status register */

#define	MREQ		0xf0		/* mask for pending request status */
#define	SREQ		4		/* field shift for pending request */
#define	MTC		0x0f		/* mask for tc status */
#define	STC		0		/* field shift for tc status */

#define DMAWRITE	1		/* dma write in progress */
#define DMAECHO		2		/* dma write for echo in progress */
#define DMADELAY	3		/* dma delay in progress */
#define DMAWRCR		4		/* dma write cr in progress */
