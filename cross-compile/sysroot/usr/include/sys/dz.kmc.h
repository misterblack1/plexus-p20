/* SID @(#)dz.kmc.h	5.1 */
/* @(#)dz.kmc.h	6.1 */
/*
 * Op codes for commands to the KMC
 */
#define	XBUFIN	0	/* Transmit a buffer */
#define	SFLAG	1	/* Pass Flag Bits */
#define	BASEIN	2	/* Pass Address of DZ */
#define	RBUFIN	3	/* Pass Read Buffer */
#define	CMDIN	4	/* Execute Sub Command */
/*
 * Sub command bits for CMDIN
 */
#define	IFLUSH	(1<<0)	/* Flush input */
#define	OFLUSH	(1<<1)	/* Flush output */
#define	OSPND	(1<<2)	/* Suspend output */
#define	ORSME	(1<<3)	/* Resume output */
#define	SCHAR	(1<<4)	/* Send character in csr6 */
#define	SETTMR	(1<<5)	/* Set kmc input timer to value in csr7 */
#define	SBRK	(1<<6)	/* Send break */
/*
 * Report codes for reports from the KMC
 */
#define	XBUFOUT	0	/* Output Complete */
#define	RBUFOUT	1	/* Input Complete */
#define	XPRSOUT	2	/* Express Character Sent */
#define	RBRK	3	/* Break Detected */
#define	COCHG	4	/* Not Implemented */
#define	ERROUT	5	/* Error Report */
/*
 * Read completion code bits
 */
#define	ICBRK	(1<<0)	/* Read Terminated by Break */
#define	ICFLU	(1<<1)	/* Read Terminated by Flush Sub Command */
#define	ICEIB	(1<<2)	/* Exceptional Input Buffer */
#define	ICLOST	(1<<3)	/* Input Data Lost */
/*
 * Write completion code bits
 */
#define	OCFLU	(1<<0)	/* Output Terminated by Flush Sub Command */
 

/*
 * Structure for the dzb ioctl DZBATTACH
 */

struct dzk {
		unsigned short kmc_mdno;
		unsigned short dzb_mdno;
	    };

#define	TIOC	('T'<<8)
#define DZBATTACH (TIOC|33)
