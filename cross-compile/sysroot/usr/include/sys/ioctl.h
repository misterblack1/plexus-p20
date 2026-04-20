/* SID @(#)ioctl.h	5.2  */
/* @(#)ioctl.h	1.0 11/10/84 */

#define	IOCTYPE	0xff00

#define	TIOC	('T'<<8)
#define	TCGETA	(TIOC|1)
#define	TCSETA	(TIOC|2)
#define	TCSETAW	(TIOC|3)
#define	TCSETAF	(TIOC|4)
#define	TCSBRK	(TIOC|5)
#define	TCXONC	(TIOC|6)
#define	TCFLSH	(TIOC|7)
#ifdef	PROFILE		/*>>>>> Profiling for ICP <<<<<*/
#define TCGETP	(TIOC|64)
#endif

/* Plexus specific ioctl calls */
#define PIOC	('P'<<8)	/* set process group of tty line to 0 */
#define CLRPGRP (PIOC|1)

#define	TCDSET	(TIOC|32)

#ifndef ICP		/*>>>>> Not used by ICP <<<<<*/
#define	LDIOC	('D'<<8)
#define	LDOPEN	(LDIOC|0)
#define	LDCLOSE	(LDIOC|1)
#define	LDCHG	(LDIOC|2)
#endif

#define	tIOC	('t'<<8)
#define	TIOCGETP	(tIOC|8)
#define	TIOCSETP	(tIOC|9)
#define	TIOCSETN	(tIOC|10)

#ifndef ICP		/*>>>>> Not used by ICP <<<<<*/
#define	LIOC	('l'<<8)
#define	LIOCGETP	(LIOC|1)
#define	LIOCSETP	(LIOC|2)
#define	LIOCGETS	(LIOC|5)
#define	LIOCSETS	(LIOC|6)

#define	DIOC	('d'<<8)
#define	DIOCGETC	(DIOC|1)
#define	DIOCGETB	(DIOC|2)
#define	DIOCSETE	(DIOC|3)
#endif

#define	VPM	('V'<<8)
#define	VPMCMD	(VPM|8)
#define	VPMERRS	(VPM|9)
#define	VPMRPT	(VPM|10)
#define	RUNCMD 	(VPM|11)
#define HLTCMD  (VPM|12)
#define VPMTRCO (VPM|16)

/*
 * Some tape operation I/O control stuff
 */
#define	MTIOC		('M'<<8)
#define	MTIOCTOP	(MTIOC|1)		/* Tape operation */
#define	MTIOCGET	(MTIOC|2)		/* Return configuration */
#define	MTIOCCON	(MTIOC|3)		/* Set configuration */
#define	MTWEOF		(MTIOC|4)		/* Write EOF */
#define	MTFSF		(MTIOC|5)		/* Forward space filemark */
#define	MTBSF		(MTIOC|6)		/* Backward space filemark */
#define	MTFSR		(MTIOC|7)		/* Forward space record */
#define	MTBSR		(MTIOC|8)		/* Backward space record */
#define	MTREW		(MTIOC|9)		/* Rewind drive */
#define	MTOFFL		(MTIOC|10)		/* ??? */
#define	MTNOP		(MTIOC|11)		/* No Operation */
#define	MTRST		(MTIOC|12)		/* Reset drive command */
