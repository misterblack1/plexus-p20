/* SID @(#)csi.h	5.1 */
/* @(#)csi.h	6.1 */
/*	csi.h 1.2 of 2/10/82
 */


/*	CSI linked list		*/

struct csillist {
	int		l_ql;		/* Queue length */
	struct csibd	*l_first;	/* Pointer to first item */
	struct csibd	*l_last;	/* Pointer to last item */
};

/*	CSI Start flags structure	*/

struct csiflags {
	char	options;
	char	intopts;
	char	window;
	char	timer0;
	char	timer1;
};

/*	CSI stat structure	*/

struct csiinfo {
	char	options;
	char	intopts;
	char	window;
	char	timer0;
	char	timer1;
	dev_t	mdev;
	short	devtype;
};

/*	Report structures	*/

#define CMDSZ	4

struct solrpt {
	short		retcode;
	struct csibd	*bdp;
};

struct unsolrpt {
	short		code;
	int		excode;
};

struct trrpt {
	short		scloc;
	char		info[CMDSZ];
};

union csirpt {
	struct solrpt	*solp;
	struct unsolrpt	*unsolp;
	struct trrpt	*trp;
};

/*	Unsolicited codes	*/

#define CSINONAPP	-1

/*	Error return codes	*/

#define INVCSI	-1			/* Invalid CSI type specified */
#define BADDEV	-2			/* Invalid device or sub-device */
#define ATTCH	-3			/* The device is already attached */
#define NOIDEV	-4			/* Invalid or no interface index */
#define STERR	-5			/* Start or stop error */

/*	Report Types	*/

#define CSISTART	1
#define CSISTOP		2
#define CSIRXBUF	3
#define CSIRRBUF	4
#define CSITERM		5
#define CSIERR		6
#define CSISRPT		7
#define CSITRACE	8
#define CSICMDACK       9
#define CSICLEANED	10


/* csiinfo options */
#define ADRB	01
#define FAST	02
#define ANSWER	04



/*	Status of transmit buffer	*/

#define CSIXMT	0
#define CSINXMT	1
#define CSIMAYBE	2

/*	Status of receive buffer	*/

#define CSIRCV	0
#define CSIEMPTY	1


#define CSIEQL(IDEV)	csiemptq(IDEV, (struct csibd *)NULL)
#define CSIXQL(IDEV)	csixmtq(IDEV, (struct csibd *)NULL)

#define CSISLP	(PZERO+5)	/* sleep priority*/


#define CSISOLP 	arg->solp
#define CSIUNSOLP	arg->unsolp
#define CSITRP		arg->trp
