/*
 *	PCL-11 Multiplexing / Demultiplexing Driver
 *	Permits 8 two-way communications between 16 machines.
 */

/*
 *	pcl structure - one required per simultaneous pcl conversation
 */

# define PCLBSZ   512		/* size of pcl input/output buffers */

struct	pcl	{
	int	pcl_dev;	/* machine and logical channel */
	int	pcl_flag;	/* channel open, etc. */
	int	pcl_pgrp;	/* process group for signals */
	struct	pcl * pcl_ioq;	/* linked list of transmissions */
	short	pcl_hdr;	/* header for transmission */
	int	pcl_icnt;	/* characters remaining in input buffer */
	int	pcl_ioff;	/* offset of first character in input buffer */
	int	pcl_ocnt;	/* characters in output buffer */
#	ifdef pdp11
	paddr_t	pcl_ibuf;		/* input buffer location */
	paddr_t	pcl_obuf;		/* output buffer location */
#	else
	char	pcl_ibuf[PCLBSZ];	/* input buffer */
	char	pcl_obuf[PCLBSZ];	/* output buffer */
#	endif
} ;

# ifndef PCL11B_0

/*
 *	pcl_flag bit definitions
 */

# define P_OPEN   000001	/* channel in use */
# define P_ROPEN  000002	/* remote channel is open */
# define P_READ   000004	/* waiting for input data */
# define P_WRITE  000010	/* waiting for output buffer */
# define P_REOF   000020	/* eof received */
# define P_NOCTRL 000040	/* no control on channel */
# define P_RERR   000100	/* reception error */
# define P_XERR   000200	/* transmission error */
# define P_RETRY  000400	/* retry on transmission error */
# define P_RSTR   001000	/* restrict control messages */
# define P_WOPEN  010000	/* waiting for open of remote */
# define P_WASC   020000	/* remote channel has closed */

/*
 *	pcl hardware registers
 */

struct	pclhw	{
	short	pcl_tcr;	/* transmitter command register */
	short	pcl_tsr;	/* transmitter status register */
	short	pcl_tdb;	/* transmitter data buffer register */
	short	pcl_tbc;	/* transmitter byte count register */
	short	pcl_tba;	/* transmitter bus address register */
	short	pcl_mmr;	/* master - maintainance register */
	short	pcl_tcrc;	/* transmitter CRC register */
	short	pcl_pad1;	/* unused register */
	short	pcl_rcr;	/* receiver command register */
	short	pcl_rsr;	/* receiver status register */
	short	pcl_rdb;	/* receiver data buffer register */
	short	pcl_rbc;	/* receiver byte count register */
	short	pcl_rba;	/* receiver bus address register */
	short	pcl_pad2;	/* unused register */
	short	pcl_rcrc;	/* receiver CRC register */
	short	pcl_pad3;	/* unused register */
} ;

/*
 *	random tcr, tsr, rcr, rsr bit definitions
 */

# define STTXM	0000001		/* start transmission */
# define TXINIT	0000002		/* transmitter initialize */
# define EAOFF	     12		/* extended address bits offset */
# define EABITS 0600000		/* mask for extended address bits */
# define IE	0000100		/* interrupt enable */
# define DSTPCL	0017400		/* destination pcl */
# define DSTOFF	      8		/* destination bits offset */
# define SNDWD	0020000		/* send word */
# define TXNPR	0040000		/* transmit NPR */
# define RIB	0100000		/* retry if busy */
# define SORE	0000040		/* software reject */
# define TBSBSY	0000020		/* tdm bus busy */
# define SUCTXF	0000200		/* successful transfer */
# define TIMOUT	0002000		/* timeout */
# define MSTDWN	0004000		/* master down on transmission */
# define ERR	0100000		/* transmission error */
# define MASTER	0030400		/* tdm bus master */
# define RCVDAT 0000001		/* receive data */
# define RCINIT	0000002		/* receiver initialize */
# define SRCPCL	0017400		/* source pcl */
# define SRCOFF	      8		/* source bits offset */
# define RCVWD  0020000		/* receive first word */
# define RCNPR	0040000		/* receive NPR */
# define REJ	0100000		/* reject */
# define CHNOPN	0000020		/* channel open */
# define REJCOM	0000040		/* reject complete */
# define DTORDY	0000400		/* data output ready */

# define PCLRPRI (PZERO + 2)	/* allow interrupts */
# define PCLWPRI (PZERO + 2)	/* allow interrupts */

# define PCLDELAY  30		/* delay after transmission rejection */

extern	struct	pcl	pcl_pcl[];	/* communication structures */
extern	int	pcl_cnt;		/* number of channels */
extern  struct  pclhw  *pcl_addr[];	/* addresses of pcl hardware */

struct	pcl *	pclsrch();

# define pclRintr	pclxintr	/* int vectors are reversed in hw */
# define pclXintr	pclrintr	/* int vectors are reversed in hw */

/*
 *	pcl minor bit definitions:
 *		bits 0 - 2:	select 1 of 8 channels
 *		bits 3 - 6:	select 1 of 16 machines
 *		bit 7:		select 1 of 2 pcl devices
 */

# define pclchan(X)	(X & 07)		/* logical channel */
# define pclmach(X)	(((X & 0170) >> 3) + 1)	/* machine id */
# define pclpcl(X)	(X >> 7)		/* pcl id */

# ifdef pdp11
# define move(x,y,z)	pimove(x, y, z)
# else
# define move(x,y,z)	iomove((int) x, y, z)
# endif

# define pclhdr(X)	(X & 0170000)		/* type of data tranmission */

/*
 *	pcl message headers
 */

# define PCLOPEN	(1 << 12)	/* channel open request */
# define PCLEDATA	(2 << 12)	/* even byte count transmission */
# define PCLODATA	(3 << 12)	/* odd byte count transmission */
# define PCLEOF		(4 << 12)	/* zero byte count transmission */
# define PCLCLOSE	(5 << 12)	/* channel shutdown */
# define PCLCTRL	(6 << 12)	/* message for control channel */
# define PCLSIGNAL	(7 << 12)	/* send signal to remote */

/*
 *	ioctl command types
 */

# define WAIT   1			/* wait until channel opens */
# define FLAG   2			/* change certain flags */
# define SIG    3			/* send a signal across the link */
# define CTRL   4			/* send a control message */
# define RSTR	5			/* restrict control messages */

/*
 *	pcl debug structure
 */

# define PCLINDX 30

struct	pcldb	{
	int	pcl_record;		/* record identifier */
	int	pcl_cmdreg;		/* command register */
	int	pcl_statreg;		/* status register */
	int	pcl_header;		/* pcl structure header */
} ;

# endif
