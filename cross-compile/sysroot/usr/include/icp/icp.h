
/* SID @(#)icp.h	5.1 */
/* @(#)icp.h	1.0 11/14/84 */

#define	MACKL	4		/* size of ack command block in bytes */
#define	MSIGL	10		/* size of signal command block in bytes */
#ifdef z8000
#ifdef SEQ
#define MWUPL	8		/* echo sequence # back to host */
#else
#define MWUPL	6		/* size of wakeup command block in bytes */
#endif
#ifdef SEQ
#define MFLSHL	8		/* echo sequence # back to host */
#else
#define MFLSHL	6		/* size of flush command block in bytes */
#endif
#endif
#ifdef m68
#ifdef SEQ
#define MWUPL	10		/* echo sequence # back to host */
#else
#define MWUPL	8		/* size of wakeup command block in bytes */
#endif
#ifdef SEQ
#define MFLSHL	10		/* echo sequence # back to host */
#else
#define MFLSHL	8		/* size of flush command block in bytes */
#endif
#endif
#define	PQSIZE	10		/* size of processor interrupt queue sizes */

/* The following relate to VPM */
#define	MREPTL  14		/* size of report command block in bytes */
#define MERTL   14              /* size of err term report in bytes */
#define MTRCEL  14              /* size of trace report in bytes */
#define MSNAPL  14              /* size of snap report in bytes */
#define MSTRUPL 14              /* size of startup report in bytes */
#define MRTNXL  14              /* size of return xmit buffer report in bytes*/
#define MRTNRL  14              /* size of return recv buffer report in bytes */

/* command definitions */
#define SIOCACK	01		/* SIOC acknowledge interrupt command */
#define	SIOCWUP	02		/* SIOC wake-up interrupt command */
#define	SIOCSIG	03		/* SIOC signal interrupt command */
#define SIOCFLSH 04		/* SIOC flush write command */

/* The following relate to VPM */
#define VPMRPTI 5               /* VPM report from icp */
#define VPMERT  6               /* VPM error termination from icp */
#define VPMTRC  7               /* VPM trace command from icp */
#define VPMSNAP 8               /* VPM snap command from icp */
#define VPMSTUP 9               /* VPM startup report from icp */
#define VPMRTNX 10              /* VPM return xmit buffer from ICP */
#define VPMRTNR 11              /* VPM return recv buffer from ICP */

#define	SIOCPRI	28		/* SIOC wake-up priority */
#define SIOCINT	02		/* SIOC interrupt */
#define SIOCIRST	04	/* SIOC reset interrupt */
#define	SQSIZE	9		/* number of entries in interrupt queues */
#define IBSIZE	10		/* maximum number of words in command block */
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
