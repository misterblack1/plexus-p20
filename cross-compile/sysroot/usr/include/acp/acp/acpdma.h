/* SID */
/* @(#)acpdma.h	5.4 10/7/86 */

/*
 *  This file contains defines and structs for the startdma control block 
 */

#define NUMPORTS 16	/* number of ports on an acp */

/*
 *  number of ticks for timeout routine to wait before expiring on
 *  dma accept control block interrupt
 */

#define ACCTO 30	/* 3 seconds */

/*
 *  register defines for acp-to-dma and dma-to-acp
 */
/*
 *  read-only registers defines
 */
#define DMASTART 0xc00020	/* control store start address */

/*
 *	bit defines for RREG_STAT register 
 */
#define SCHATTN	0x8000
#define SMBIRQ	0x4000
#define SDMAATTN 0x2000		/* dma attention */
#define SCPUATTN 0x1000
#define SPWRFAIL 0x0800

/*
 *  bit defines for WREG_CNTL relative to DMA
 */
#define DMARST	0x0001		/* clear all resettable DMA regs when zero */
#define DMARUN	0x0002		/* enable DMA to run (when set) */
#define DMASTEP	0x0004		/* enable single step when toggled if dma
					halted */
#define DMAIACK	0x0008		/* clear int from dma, allow dma to acp ints */
#define RTCIACK 0x0010		/* clear rtc ints, allow rtc ints */
#define CHATIACK 0x0020		/* clear the channel attention from CPU */
#define SETMBIRQ 0x0040		/* toggle (only when 0) to int the mb */
#define DMAATTN	0x0080		/* (rqt to dma) interrupts dma when toggled */

/*
 *  defines for max size of CBARRAY, the array which keeps track of the
 *  control blocks for the requests that the ACP has made of the DMA, and
 *  the hard addresses, in ACP memory, of where the ACP and the DMA put
 *  the addresses of the control blocks for the DMA ATTN and ACP ATTN
 *  interrupts.
 */

#define CBMAX	300		/* max entries in CBARRAY */
#define CBADDR	0x1000		/* first of 4 bytes for ptr to ctl blk */
#define RTNADDR 0x1004		/* first of 4 bytes for ptr to rtn ctl blk */
#define ACCSEMA	0x1008		/* first of 2 bytes for accept semaphore */
#define DMAONE	0x100A		/* set at init time */
#define SETONE	0x3784		/* DMAONE value */

#define CTLBLKINIT	0xFFFFF000	/* init value for ctlblk's */

/*
 *  Defines for the dma control block 
 */
/*
 *  Command Types
 */

#define DMAIMB	0x00		/* DMA IN from Multibus */
#define DMAOMB  0x01		/* DMA OUT to Multibus */
#define DMAISIO	0x02		/* DMA IN from SIO */
#define DMAOSIO 0x03		/* DMA OUT to SIO */
#define DMAOPIO 0x04		/* DMA OUT to PIO */
#define DMASRD	0x05		/* DMA read scratch ram into 68k memory */
#define DMASWR	0x06		/* DMA write scratch ram from 68k memory */
#define DMACRC  0x07		/* "new" DMA CRC table pointer */
#define DMASYNC 0x08		/* "new" DMA SYNC state table pointer */
#define DMACLOS 0x09		/* Close SIO line */
#define DMAFLUSH 0x0A		/* Flush output for sio line */
#define DMAREOPEN 0x0B		/* re-open sio line, no CFIFO sent */
#define DMAOPEN	0x0C		/* Open an sio line, CFIFO sent also */

/* 
 *  char counts for some command types
 */

#define SINITSZ	13		/* initialize SIO char cnt */
#define PINITSZ	13		/* initialize PIO char cnt */

/*
 *  bit flags in the check flags field
 */

#define ASYNCM	0x0000		/* async mode for this line */
#define SYNCM	0x0008		/* sync mode for this line */
#define HDLCM	0x0010		/* hdlc mode for this line */

#define CKRAW	0x0020		/* check for raw mode for this line */
#define CKSCB	0x0040		/* check if this was an scb read in */
#define CKECHO	0x0080		/* check if echo required 1=yes */
#define CKMBINT	0x0100		/* 1= mb int required after mb out completed */
#define NORMALMB 0x0000		/* Regular MB flag setting */

#define IOCTL	0x8000		/* output in CONTROL mode */
#define SIOREGS	0x0000		/* ioctl cmd - init sio chip registers */
#define SIOREGCNT	32	/* size of sio register output buffer */
#define CKCTLQS	0x0001		/* check for control-q / control-s */
#define CLOSDMA	0x0002		/* close the designated sio channel */
#define OPENDMA	0x0003		/* Re-Open the designated sio channel */
#define CKMODE	0x0018		/* field is used to define mode of line */

/*
 *  struct for SIO register contiguous area in ACP memory
 */

struct sioregs {
	unsigned char	reg_num;
	unsigned char	reg_val;
};

/*
 *  struct for 3-byte ACP RAM area containing the state following flag
 *  and two bytes of crc
 */

struct statecrc {
	char	state;		/* 0 state following flag */
	char	crc1;		/* 1 crc-16 byte 1 */
	char	crc2;		/* 2 crc-16 byte 2 */
};

/*
 *  Struct of the acp-generated dma control block (ACP to DMA)
 */

struct dmactlb {
	struct dmactlb	*nextctlb;	/* 0:3 ptr to next dma ctl block */
	char	line;			/* 4 line number */
	char	cmdtype;		/* 5 command type */
	char	*fromaddr;		/* 6:9 "from" address */
	char	*toaddr;		/* 10:13 "to" address */
	short	donecnt;		/* 14:15 read completion count */
	short	charcnt;		/* 16:17 char count for this dma rqt 
						 (words for multibus?) */
	short	flags;			/* 18:19 flags to check */
/* RETURN BLOCK DATA ITEMS */
	char	rtntype;		/* 20    type of dma interrupt */
	char	rtnstat;		/* 21    SIO/PIO status byte */
	short	rtnccnt;		/* 22:23 char count actually dma'd */
};

/*
 *  DMA interrupt (to ACP) types
 */

#define SIOITX	0x00		/* SIO input timer exhausted */
#define SIOICX	0x01		/* SIO input count exhausted */
#define SIOSTAT	0x02		/* SIO status change */
#define SIOOCX	0x03		/* SIO output count exhausted */
#define PIOSTAT 0x04		/* PIO status change */
#define PIOOCX	0x05		/* PIO output count exhausted */
#define MBICX	0x06		/* Multibus input count exhausted */
#define MBOCX	0x07		/* Multibus output count exhausted */
#define SIOCTL  0x08		/* SIO IOCTL completed */
/*              0x09		   Reserved */
#define CTLQSR	0x0a		/* control-q or control-s received */
#define CBACCDN	0x0b		/* ctl blk accepted and operation completed */
#define SYNCST	0x0c		/* SYNC rcv/xmt ext status change */
#define BADCMD	0x0d		/* unknown acp-to-dma command type */
#define DELRCV	0x0e		/* <del> received */
#define BRKRCV	0x0f		/* <break> received */
#define CTLCHRCV 0x10		/* control character received */
#define SIOCLOS	0x11		/* DMA SIO Close completed */
#define SIOROPN	0x12		/* DMA SIO Re-Open completed */
#define SRDDMA	0x13		/* DMA scratch ram read completed */
#define SWTDMA	0x14		/* DMA scratch ram write completed */

/*
 *  max number of control blocks that may be on the dma queue at one
 *  time.  Not sure what this number should be, but we will start at
 *  what the icp used, plus a few more, since the acp supports a few
 *  more lines.  The icp value was MBQSIZE (10).
 */

#define DMAQSIZE 500

/*
 *  struct of acp-to-dma control block queue
 */

struct dmaq {
	struct dmactlb *head;	/* pointer to first on queue */
	struct dmactlb *tail;	/* pointer to last on queue */
	short cnt;		/* count of control blocks in queue */
};

/*
 *	Should only need 16 of these since we can now have only 1 read
 *	return control block from the dma per line (i.e. the CFIFO ctlblk).
 *	But we'll say 32 for now just to give us some padding.
 */
#define READQSZ 32		/* max # of indices to queue to dmadaemon */

/*
 *	The following defines are for offsets into the CFIFO used for input
 *	These macro values are added to the dma ctlblk "toaddr" to get
 *	the corresponding data values.
 *		out index:	byte offset of first char to pull out
 *		in  index:	byte offset of next avail slot for a char
 *		char cnt :	number of "good" chars in CFIFO
 *
 *	NOTE:  a "char" is really a byte of status and a byte of char data.
 *	       equaling a short word.
 */
#define CFIFOOUT	0	/* out index (short) */
#define CFIFOIN		2	/* in  index (short) */
#define CFIFOCCNT	4	/* char cnt  (short) */
#define CFIFOSTRT	6	/* start of char data */
#define CFIFOSIZE	1024	/* size of char fifo */


/*
 *	This structure is never used but defined to aid cdb with viewing
 *	the dma scratch ram.  Therefore, it must agree with the dma ram
 *	definitions to work correctly.
 */
struct dma_sram {
	short	oaflag;			/* output activity flag */
	short	xmting;			/* transmitting activity flag */
	short 	mbaflag;		/* multibus activity flag */
	short	pioaflag;		/* pio activity flag */
	short	lnopen;			/* line open flag */
	short	oflush;			/* output flush flag */
	short	filler[2];		/* filler to align next var */
	struct dmactlb *siooftb[16];	/* output first pointers */
	struct dmactlb *siooltb[16];	/* output last  pointers */
	char	*siooutb[16];		/* output buffer pointers */
	short	siooutc[16];		/* output counts */
	short	siooutx[16];		/* output even/odd flags */
	short	sioowrd[16];		/* current output words */
	short	txsched[16];		/* transmit scheduling flags */
	short	sioochk[16];		/* output control flags */
	short	ctlzany[16];		/* ixany flags */
	struct dmactlb *sioincb[16];	/* input  first pointers */
	short	sioinip[16];		/* input interrupt in process flags */
	char	*fifoadr[16];		/* cfifo addresses */
	short	fifosz[16];		/* cfifo sizes */
	short	siocmd[16];		/* cmd register offsets */
	short  	siomode[16];		/* mode control register offsets */
	short	siointc[16];		/* intr control register offsets */
	short	sio1syn[16];		/* sync char 1 register offsets */
	short  	sio2syn[16];		/* sync char 2 register offsets */
	short 	siorxc[16];		/* rcvr control register offsets */
	short	siotxc[16];		/* xmit control register offsets */
	short	sio0st[16];		/* status 0 register offsets */
	short	sio1st[16];		/* status 1 register offsets */
	short	siodata[16];		/* data register offsets */
	short	sioclr[16];		/* clear intr register offsets */
	short	siotcrg[16];		/* time constant register offsets */
	short	siobrg[16];		/* brg constant register offsets */
	short 	siovect[16];		/* vect number register offsets */
	struct dmactlb *piooftb;	/* pio first pointer */
	struct dmactlb *piooltb;	/* pio last pointer */
	char 	*piooutb;		/* pio output buffer pointer */
	short	piooutc;		/* pio output count */
	short	piooutx;		/* pio output even/odd flag */
	short	piosch;			/* pio output scheduling flag */
	short	pioowrd;		/* current pio output word */
	struct dmactlb *dmaftb;		/* multibus first pointer */
	struct dmactlb *dmaltb;		/* multibus last pointer */
	char 	*dmato;			/* multibus "to" pointer */
	char	*dmafm;			/* multibus "from" pointer */
	short	dmac;			/* multibus output count */
	short 	dmambc;			/* multibus char count (saved) */
	short	mbtemp;			/* multibus routine temp storage */
	short	mbcbchk;		/* multibus output control flag */
	struct dmactlb *rbaqh;		/* return block queue head pointer */
	struct dmactlb *rbaqt;		/* return block queue tail pointer */
	short	rbaqc;			/* return block queue entry count */
	short	bittab[16];		/* bit flag array */
	short	mctrl;			/* mctrl save area */
	short	sema;			/* bad semaphore save area */
	struct dmactlb *tmpoftb;	/* temporary first pointer */
	struct dmactlb *tmpoltb;	/* temporary last pointer */
};

