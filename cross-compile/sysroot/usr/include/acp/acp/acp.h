/* SID */
/* @(#)acp.h	5.1 4/22/86 */

#define	MACKL	4		/* size of ack command block in bytes */
#define	MSIGL	10		/* size of signal command block in bytes */
#define MWUPL	8		/* size of wakeup command block in bytes */

#define MFLSHL	8		/* size of flush command block in bytes */
#define	PQSIZE	100		/* size of processor interrupt queue sizes */

/* The following relate to VPM */
#define	MREPTL  14		/* size of report command block in bytes */
#define MERTL   14              /* size of err term report in bytes */
#define MTRCEL  14              /* size of trace report in bytes */
#define MSNAPL  14              /* size of snap report in bytes */
#define MSTRUPL 14              /* size of startup report in bytes */
#define MRTNXL  14              /* size of return xmit buffer report in bytes*/
#define MRTNRL  14              /* size of return recv buffer report in bytes */

/* 			command definitions 			*/
/* The following command definitions relate to ICP */
#define ICPACK	1		/* ICP acknowledge interrupt command */
#define	ICPWUP	2		/* ICP wake-up interrupt command */
#define	ICPSIG	3		/* ICP signal interrupt command */
#define ICPFLSH 4		/* ICP flush write command */

/* The following command definitions relate to VPM (ICP/ACP) */
#define VPMRPTI 5               /* VPM report from ICP/ACP */
#define VPMERT  6               /* VPM error termination from ICP/ACP */
#define VPMTRC  7               /* VPM trace command from ICP/ACP */
#define VPMSNAP 8               /* VPM snap command from ICP/ACP */
#define VPMSTUP 9               /* VPM startup report from ICP/ACP */
#define VPMRTNX 10              /* VPM return xmit buffer from ICP/ACP */
#define VPMRTNR 11              /* VPM return recv buffer from ICP/ACP */

/* The following command definitions relate to ACP */
#define	ACPOPEN	12		/* ACP generated user 'open' of a device */
#define	ACPREAD	13		/* ACP generated user 'read' from a device */
#define	ACPWRITE 14		/* ACP generated user 'write' to a device */
#define	ACPIOCTL 15		/* ACP generated user 'ioctl' to a device */
#define	ACPCLOSE 16		/* ACP generated user 'close' of a device */
/* 				   commands 20 and 21 are hdlc commands	*/

#define	ACPPRI	28		/* ACP wake-up priority */
#define ACPINT	02		/* ACP interrupt */
#define ACPIRST	04		/* ACP reset interrupt */

#define	ICPPRI	28		/* ICP wake-up priority */
#define ICPINT	02		/* ICP interrupt */
#define ICPIRST	04		/* ICP reset interrupt */

#define	SQSIZE	9		/* number of entries in interrupt queues */

/*
 *  IBSIZE used to be 10 for the icp, but acp increased the scb struct.  see
 *  note in pbif.c:dqpbiq about si_cblen, etc.
 */

#define IBSIZE	40		/* maximum number of words in command block */
#define OOK	1		/* ok to do another open  */
#define	ROK	2		/* ok to do another read */
#define WOK	4		/* ok to do another write */
#define IOK	8		/* ok to do another ioctl */
#define COK	16		/* ok to do another close */
#define INCLEAN		128	/* in clean process for this unit */
 
/*
 * Multibus main memory access constants.
 */

#define	MBIIN	01		/* mbmore interrupt in state */
#define	MBIIMID	06		/* interrupt input in middle of transfer */
#define	MBIOMID	07		/* interrupt output in middle of transfer */
#define MBIOUT	02		/* mbmore interrupt out state */
#define MBMAX	128		/* maximum bytes that can be xferred */
#define MBMLAST 03		/* mbmore last of block state */
#define MBMMDL	04		/* mbmore middle of block state */
#define	MBQSIZE	10		/* number of entries in multibus queue */
#define MBSTART	05		/* mbmore start transfer state */

/*
 *  ACP Write Register Addresses
 */
#define WREG_STAT	0xB00000	/* acp status register address */
#define WREG_WKUP	0xB00002	/* acp wakeup address */
#define WREG_CNTL	0xB00004	/* acp control register */
#define WREG_MBDATA	0xB00006	/* MB data register */

/*
 *  ACP Read Register Addresses
 */
#define RREG_STAT	0xB00000	/* acp status register */
#define RREG_ADDR	0xB00002	/* cntl store addr register */
#define RREG_ERROR	0xB00004	/* acp error register */
#define RREG_MBDATA	0xB00006	/* MB data register */

/*
 *  BASE address for start of SIO registers
 */
#define	SIOBASE		0xa00000

/* for main.c dh.c */
#define	NUMSPE	110
#ifndef ASLANG
struct	sp	{
	struct	sp	*sp_lnk;
	unsigned char	sp_un;
	unsigned char 	sp_cmd;
};
#endif
