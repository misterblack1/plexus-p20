
/* SID @(#)pio.h	5.1 */
/* @(#)pio.h	1.0 11/14/84 */

/*
 * Defines for the z80 pio.
 */

#define	PMODE		(15<<0)		/* identifies a mode ctrl word */
#define	PICW		(7<<0)		/* identifies an interrupt ctrl word */

/* mode word */

#define	POUTMODE	(0<<6)		/* output mode */
#define	PINMODE		(1<<6)		/* input mode */
#define	PBIMODE		(2<<6)		/* bidirectional mode */
#define	PCTRLMODE	(3<<6)		/* control mode */

/* interrupt control word */

#define	PINTEN		(1<<7)		/* enable interrupts */
#define	PANDOR		(1<<6)		/* and/or */
#define	PHIGHLOW	(1<<5)		/* high/low */
#define	PMSK		(1<<4)		/* mask follows */
 
struct pios {
	int	pb_busy;
	char	*pb_buf;
	int	pb_len;
};
