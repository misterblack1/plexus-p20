/* SID @(#)sxt.h	5.1 */
/*	<@(#)sxt.h	6.2>	*/

/*
 **	Multiplexed channels driver header
 */

#define	SXTRACE		0		/* 1 to include tracing */

#define	MAXLINKS	32
#define	CHAN(dev)	(dev&CHANMASK)
#define	LINK(dev)	((dev>>CHANBITS)&(0xff>>CHANBITS))

#if	(MAXPCHAN*MAXLINKS) > 256
	ERROR -- product cannot be greater than minor(dev)
#endif



struct Channel
{
	struct tty	tty;		/* Virtual tty for this channel */
};

typedef struct Channel *Ch_p;

struct Link
{
	struct tty *	line;		/* Real tty for this link */
	char		controllingtty;	/* the current top dog */
	char		old;		/* Old line discipline for line */
	char		nchans;		/* Number of channels allowed */
	unsigned char	chanmask;	/* Allowable channel bits */
	char		open;		/* Channel open bits */
	char		xopen;		/* Exclusive open bits */
	char		wpending;	/* pending writes/channel */
	char		iblocked;	/* channels blocked for input */
	char		oblocked;	/* channels blocked for output*/
	char		lwchan;		/* Last channel written bit */
	char		wrcnt;		/* Number of writes on last channel written */
	dev_t		dev;		/* major and minor device # */
	struct Channel	chans[1];	/* Array of channels for this link */
};

typedef	struct Link *	Link_p;

/*
**	Ioctl args
*/

#define	SXTIOCLINK	('b'<<8)
#define	SXTIOCTRACE	(SXTIOCLINK|1)
#define	SXTIOCNOTRACE	(SXTIOCLINK|2)
#define SXTIOCSWTCH	(SXTIOCLINK|3)
#define	SXTIOCWF	(SXTIOCLINK|4)
#define SXTIOCBLK	(SXTIOCLINK|5)
#define SXTIOCUBLK	(SXTIOCLINK|6)
#define SXTIOCSTAT	(SXTIOCLINK|7)



/* the following structure is used for the SXTIOCSTAT ioctl call */
struct sxtblock
{
	char	input;		/* channels blocked on input  */
	char	output;		/* channels blocked on output */
};



#define	t_link		t_dstat		/* Use dstat in real tty for linknumber */

#define	MAXPCHAN	8			/* Maximum channel number */
#define	CHANBITS	3			/* Bits for channel number */
#define	CHANMASK	07			/* 2**CHANBITS - 1 */
#define	SXTHOG		2			/* Channel consecutive write limit */
