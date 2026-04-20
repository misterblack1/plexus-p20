
/* SID @(#)sio.h	5.1 */
/* @(#)sio.h	1.0 11/14/84 */

/*
 * Defines for the z80 sio. Only those parts used for async mode are
 * presently defined.
 */

/* write register 0 */

#define	SCMD		3		/* field adx of command */
#define	NULLCMD		(0<<SCMD)	/* nop */
#define	SENDABORT	(1<<SCMD)	/* send abort */
#define	RESETEXT	(2<<SCMD)	/* reset external/status interrupts */
#define	CHANRESET	(3<<SCMD)	/* channel reset */
#define	ENABLEINT	(4<<SCMD)	/* enable interrupt on next receive */
#define	RESETXMIT	(5<<SCMD)	/* reset xmit interrupt pending */
#define	ERRORRESET	(6<<SCMD)	/* error reset */
#define	RTNFROMINT	(7<<SCMD)	/* return from interrupt */
#define RSTXMTUNDR	(3<<6)		/* reset xmit underrun--sync mode */

#define	SPTR		0		/* field adx of register pointer */
#define	WR0		(0<<SPTR)	/* write register 0 */
#define	WR1		(1<<SPTR)
#define	WR2		(2<<SPTR)
#define	WR3		(3<<SPTR)
#define	WR4		(4<<SPTR)
#define	WR5		(5<<SPTR)
#define	WR6		(6<<SPTR)
#define	WR7		(7<<SPTR)
#define	RR0		(0<<SPTR)	/* read register 0 */
#define	RR1		(1<<SPTR)
#define	RR2		(2<<SPTR)

/* write register 1 */

#define	RINTFIRST	(1<<3)		/* rec int on first only */
#define	RINTALL		(2<<3)		/* rec int on all with parity chk */
#define	RINTALLNOPE	(3<<3)		/* rec int on all with no parity chk */
#define	STATAV		(1<<2)		/* status affects interrupt vector */
#define	XMITINTEN	(1<<1)		/* xmit interrupts enable */
#define	EXTINTEN	(1<<0)		/* external/status interrupts enable */
#define	RDYONXMIT	(6<<5)		/* wave the ready line on xmit */

/* write register 2 */

/* used for interrupt vector */

/* write register 3 */

#define	RBITS8		(3<<6)		/* eight bits per char receive */
#define	RBITS7		(1<<6)		/* seven bits per char receive */
#define	RBITS6		(2<<6)		/* six bits per char receive */
#define	AUTOEN		(1<<5)		/* auto enables */
#define	REN		(1<<0)		/* receiver enable */
#define HUNT            (1<<4)          /* enter hunt mode */
#define SYNCINHIB       (1<<1)          /* Sync char load inhibit */

/* write register 4 */

#define	CLK1X		(0<<6)		/* clk times 1 */
#define	CLK16X		(1<<6)		/* clk times 16 */
#define	CLK32X		(2<<6)		/* clk times 32 */
#define	CLK64X		(3<<6)		/* clk times 64 */
#define SYNC16          (1<<4)          /* Sync mode = 16 bit sync char */
#define	SB1		(1<<2)		/* one stop bit */
#define	SB15		(2<<2)		/* one and one half stop bits */
#define	SB2		(3<<2)		/* two stop bits */
#define	PEVEN		(1<<1)		/* even parity */
#define	PEN		(1<<0)		/* parity enable */

/* write register 5 */

#define	DTR		(1<<7)		/* data terminal ready */
#define	TBITS8		(3<<5)		/* eight bits per char xmit */
#define	TBITS7		(1<<5)		/* seven bits per char xmit */
#define	TBITS6		(2<<5)		/* six bits per char xmit */
#define	SENDBRK		(1<<4)		/* send break */
#define	TEN		(1<<3)		/* xmit enable */
#define	RTS		(1<<1)		/* request to send */

/* read register 0 */

#define	BRK		(1<<7)		/* break/abort */
#define	CTS		(1<<5)		/* clear to send */
#define	DCD		(1<<3)		/* carrier detect */
#define	TXRDY		(1<<2)		/* tx buffer empty */
#define	INTPENDING	(1<<1)		/* interrupt pending */
#define	RXRDY		(1<<0)		/* receive char available */

/* read register 1 */

#define ENDFRAME	(1<<7)		/* SDLC/HDLC end of frame */
#define	FRAMEERR	(1<<6)		/* framing error */
#define	OVERRUN		(1<<5)		/* overrun error */
#define	PARERR		(1<<4)		/* parity error */
#define	ALLSENT		(1<<0)		/* all sent */

/* read register 2 */

/* for main.c dh.c */
#define	NUMSPE	110
struct	sp	{
	struct	sp	*sp_lnk;
	unsigned char	sp_un;
	unsigned char 	sp_cmd;
};
