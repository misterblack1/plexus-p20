/* SID @(#)lprio.h	5.1 */
/* @(#)lprio.h	6.1 */
/*
 * Line Printer Type Devices I/O Control
 */

struct lprio {
	short	ind;
	short	col;
	short	line;
};

/* ioctl commands */
#define	LPR	('l'<<8)
#define	LPRGET	(LPR|01)
#define	LPRSET	(LPR|02)
#define	LPRGETV	(LPR|05)
#define	LPRSETV	(LPR|06)
