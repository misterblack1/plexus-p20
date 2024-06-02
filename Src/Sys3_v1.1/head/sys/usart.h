/*
 * 2661 usart defines
 */

/* cmd fields */

#define	B_RTS		0x20		/* request to send */
#define	B_RERR		0x10		/* reset pe, fe, and or errors */
#define B_BRK		0x08		/* force break */
#define	B_RXEN		0x04		/* receive enable */
#define	B_DTR		0x02		/* data terminal ready */
#define	B_TXEN		0x01		/* transmit enable */

/* status fields */

#define	B_TXRDY		0x01		/* thr is empty */
#define	B_RXRDY		0x02		/* rhr is full */
#define	B_DSCHG		0x04		/* change in dsr or dcd or tsr empty */
#define	B_PE		0x08		/* parity error */
#define	B_OR		0x10		/* overrun */
#define	B_FE		0x20		/* framing error */
#define	B_DCD		0x40		/* data carrier detect */
#define	B_DSR		0x80		/* data set ready */

/* mode1 fields */

#define	BAUDX1		0x01		/* baud rate times 1 */
#define	ONESB		0x40		/* one stop bit */
#define	TWOSB		0xc0		/* two stop bits */
#define	BITS8		0x0c		/* 8 bits per character */
#define	BITS7		0x08		/* 7 bits per character */
#define	BITS6		0x04		/* 6 bits per character */
#define	PENABLE		0x10		/* parity enable */
#define	EVENPAR		0x20		/* even parity */

/* mode2 fields */

#define	II1x1x		0x30		/* TxC=I Rxc=I pin9=1x pin25=1x */
#define	BR50		0x00		/* baud rate */
#define	BR75		0x01
#define	BR110		0x02
#define	BR134		0x03
#define	BR150		0x04
#define	BR200		0x05
#define	BR300		0x06
#define	BR600		0x07
#define	BR1050		0x08
#define	BR1200		0x09
#define	BR1800		0x0a
#define	BR2000		0x0b
#define	BR2400		0x0c
#define	BR4800		0x0d
#define	BR9600		0x0e
#define	BR19200		0x0f
