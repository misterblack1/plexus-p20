/* SID */
/* @(#)sio68564.h	5.1 4/22/86 */

/*
 * Defines for the Mostek MK68564 sio. Only those parts used for async mode are
 * presently defined.
 */

/* internal register offsets */

/* channel A */

#define	CMDREGA		0		/* offset of command register */
#define MODECTLA	2		/* offset of mode control register */
#define INTCTLA		4		/* offset of interrupt control reg */
#define SYNC1A		6		/* sync word register 1 */
#define	SYNC2A		8		/* sync word register 2 */
#define RCVCTLA		10		/* receive control register */
#define XMTCTLA		12		/* transmitter control register */
#define	STAT0A		14		/* status register 0 */
#define STAT1A		16		/* status register 1 */
#define DATARGA		18		/* data register */
#define TCREGA		20		/* time constant register */
#define	BRGCTLA		22		/* baud rate generator */
#define VECTRGA		24		/* vector register same for channel B */



/* channel B */

#define	CMDREGB		32		/* offset of command register */
#define MODECTLB	34		/* offset of mode control register */
#define INTCTLB		36		/* offset of interrupt control reg */
#define SYNC1B		38		/* sync word register 1 */
#define	SYNC2B		40		/* sync word register 2 */
#define RCVCTLB		42		/* receive control register */
#define XMTCTLB		44		/* transmitter control register */
#define	STAT0B		46		/* status register 0 */
#define STAT1B		48		/* status register 1 */
#define DATARGB		50		/* data register */
#define TCREGB		52		/* time constant register */
#define	BRGCTLB		54		/* baud rate generator */
#define VECTRGB		56		/* vector register same for channel B */


/* relative offsets for each channel */

#define	CMDREG		0		/* offset of command register */
#define MODECTL		2		/* offset of mode control register */
#define INTCTL		4		/* offset of interrupt control reg */
#define SYNC1		6		/* sync word register 1 */
#define	SYNC2		8		/* sync word register 2 */
#define RCVCTL		10		/* receive control register */
#define XMTCTL		12		/* transmitter control register */
#define	STAT0		14		/* status register 0 */
#define STAT1		16		/* status register 1 */
#define DATARG		18		/* data register */
#define TCREG		20		/* time constant register */
#define	BRGCTL		22		/* baud rate generator */
#define VECTRG		24		/* vector register same for channel B */


/* command register */

#define	SCMD		3		/* field adx of command */
#define	NULLCMD		0		/* nop */
#define	SENDABORT	8		/* send abort */
#define	RESETEXT	0x10		/* reset external/status interrupts */
#define	CHANRESET	0x18		/* channel reset */
#define	ENABLEINT	0x20		/* enable interrupt on next receive */
#define	RESETXMIT	0x28		/* reset xmit interrupt pending */
#define	ERRORRESET	0x30		/* error reset */
#define	RSTRCVCRC	0x40		/* reset receive crc checker */
#define RSTXMTCRC	0x80		/* reset xmit crc generator */
#define RSTXMTUNDR	0xc0		/* reset xmit underrun--sync mode */
#define LOOP		1		/* enable loop mode */

/* interrupt control register */

#define	RINTFIRST	0x8		/* rec int on first only */
#define	RINTALL		0x10		/* rec int on all with parity chk */
#define	RINTALLNOPE	0x18		/* rec int on all with no parity chk */
#define	STATAV		0x4		/* status affects interrupt vector */
#define	XMITINTEN	0x2		/* xmit interrupts enable */
#define	EXTINTEN	0x1		/* external/status interrupts enable */
#define	RXRDYEN		0x20		/* Rx ready enable */
#define TXRDYEN		0x40		/* Tx ready enable */
#define CRC16		0x80		/* select crc 16 polynomial */

/* receive control register */

#define	RBITS8		0xc0		/* eight bits per char receive */
#define	RBITS7		0x80		/* seven bits per char receive */
#define	RBITS6		0x40		/* six bits per char receive */
#define	AUTOEN		0x20		/* auto enables */
#define	REN		1		/* receiver enable */
#define HUNT            0x10		/* enter hunt mode */
#define SYNCINHIB       2	        /* Sync char load inhibit */
#define ADDSRCH		0x4		/* address search mode */
#define RXCRCEN		0x8		/* receive crc enable */

/* mode control register */

#define	CLK1X		0		/* clk times 1 */
#define	CLK16X		0x40		/* clk times 16 */
#define	CLK32X		0x80		/* clk times 32 */
#define	CLK64X		0xc0		/* clk times 64 */
#define SYNC16          0x10		/* Sync mode = 16 bit sync char */
#define	SDLC		0x20		/* sdlc mode flag */
#define EXTSYNC		0x30		/* external sync mode */
#define	SB1		0x4		/* one stop bit */
#define	SB15		0x8		/* one and one half stop bits */
#define	SB2		0xc		/* two stop bits */
#define	PEVEN		0x2		/* even parity */
#define	PEN		1		/* parity enable */

/* transmitter control register */

#define	DTR		0x4		/* data terminal ready */
#define	TBITS8		0xc0		/* eight bits per char xmit */
#define	TBITS7		0x80		/* seven bits per char xmit */
#define	TBITS6		0x40		/* six bits per char xmit */
#define	SENDBRK		0x10		/* send break */
#define	TEN		1		/* xmit enable */
#define	RTS		2		/* request to send */
#define TXCRCEN		8		/* enable transmit crc generation */
#define TXAUTOEN	0x20		/* transmit auto enable */

/* baud rate generator */

#define	RCINT		0x8		/* receive clock internal */
#define	TCINT		0x4		/* transmit clock internal */
#define DIV64		0x2		/* divide by 64 */
#define	BRGEN		0x1		/* enable baud rate generator */

/* status register 0 (read only) */

#define	BRK		0x80		/* break/abort */
#define TXUNDERRUN	0x40		/* transmit underrun */
#define	CTS		0x20		/* clear to send */
#define HUNTS		0x10		/* hunt or sync */
#define	DCD		0x8		/* carrier detect */
#define	B_TXRDY		0x4		/* tx buffer empty */
#define	INTPENDING	0x2		/* interrupt pending */
#define	B_RXRDY		0x1		/* receive char available */

/* status register 1 (read only) */

#define ENDFRM		0x80		/* end of frame */
#define	FRAMEERR	0x40		/* framing error */
#define	OVRUN		0x20		/* overrun error */
#define	PARERR		0x10		/* parity error */
#define	ALLSENT		0x1		/* all sent */

/* general definitions */

#define	SIO_CH_SPC	32	/* number of bytes between channels on chip */	

#ifndef ASLANG
struct usart {			/* mnemonics taken from the mostek manual */
	unsigned char 
		cmdreg,
		even0,		/* even'n' are place keepers, since regs are */
		modectl,	/* a byte wide */
		even1,
		intctl,
		even2,
		sync1,
		even3,
		sync2,
		even4,
		rcvctl,
		even5,
		xmtctl,
		even6,
		stat0,
		even7,
		stat1,
		even8,
		datarg,
		even9,
		tcreg,
		evena,
		brgctl,
		evenb,
		vectrg;
};

/* macros for polling scheme */

# define busy( rtp ) 		(( rtp->usp->stat0 & B_TXRDY) == 0 )
# define r_outchar( rtp, c ) 	( rtp->usp->datarg = c)
# define outchar( rtp, c ) 	( rtp->usp->datarg = c)

# define r_inchar( rtp )	\
	( (rtp->usp->stat0&B_RXRDY) ? (rtp->usp->datarg) : -1L)
# define inchar( rtp )	\
	( (rtp->usp->stat0&B_RXRDY) ? (rtp->usp->datarg) : -1L)
#endif
