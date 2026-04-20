
/* SID @(#)ctc.h	5.1 */
/* @(#)ctc.h	1.0 11/14/84 */

/*
 * Defines for the z80 ctc.
 */

/* channel control word */

#define	CINTEN		(1<<7)		/* interrupt enable */
#define	CCTR		(1<<6)		/* counter mode */
#define	CCLK256X	(1<<5)		/* clk times 256 */
#define	CUPCLK		(1<<4)		/* clk on rising edge */
#define	TRGSTART	(1<<3)		/* clk/trg pulse starts timer */
#define	CTIME		(1<<2)		/* time const to follow */
#define	CRESET		(1<<1)		/* reset timer */
#define	CCTRL		(1<<0)		/* control word */
