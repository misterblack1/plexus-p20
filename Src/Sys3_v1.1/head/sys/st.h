#define NKMC	8
#define FAIL	1
#define CHNUM	4		/* number of channels (end stations) */
#define STHBUF	(6 * CHNUM)	/* number of buffer headers */
#define BLKSZ	256
#define	STNDSCH	1		/* number of special channels */
#define	NSTCHQ	8		/* size of sync channel queues */
#define	STOBSZ	8000		/* space for internal messages */
#define	STIBSZ	8000		/* space for internal messages */
#define	STMAP	100		/* maps for allocation of above */
#ifdef vax
#define round(X)	((X+sizeof(int)-1) & ~(sizeof(int)-1))
#else
#define ptoc(X)		(unsigned)((X + 63) >> 6)
#endif
struct	stbhdr {
#ifdef vax
	ushort		sm_count;	/* char count in buffer */
	ushort		sm_locl;	/* lower 16 bits of UNIBUS address */
#else
	unsigned	sm_count;	/* char count in buffer */
	unsigned	sm_locl;	/* lower 16 bits of UNIBUS address */
#endif
	char		sm_locu;	/* upper 2 bits of UNIBUS address */
	char		sm_type;	/* communications cell */
	char		sm_sta;		/* station identification */
	char		sm_dev;		/* device identification */
	struct stbhdr	*sm_nxt;
#ifdef vax
	unsigned	sm_loc;		/* physical offset of data */
#else
	paddr_t		sm_loc;		/* physical location of data */
#endif
	unsigned	sm_size;	/* size of buffer */
};
struct	stmsghdr {
	int		s_max;		/* max length of queue */
	int		s_act;		/* number of current queue members */
	struct stbhdr	*s_last;	/* pointer to last queue member */
	struct stbhdr	*s_first;	/* pointer to message queue */
};
struct	st {
	unsigned	s_ttyid;	/* terminal identifier */
	char		s_type;		/* terminal type */
	char		s_port;		/* KMC port */
	struct	stbhdr  *s_rbuf;	/* current read buffer */
	unsigned	s_roffset;	/* current offset in read buffer */
	unsigned	s_flags;	/* state of channel */
	unsigned	s_mode;		/* RAW only */
	struct stmsghdr	s_hdr;		/* channel queue header */
};
/*
 *	Status Information
 */
#define STRAW		040
#define CH_OPEN		01
#define CH_AVAIL	02
#define FBLOCK		040	/* first block */
#define LBLOCK		04	/* last block */
#define STMBSZ		256	/* maximum buffer size */
#define STRM		6
#define RRTNXBUF	0	/* VPM interrupt command */
#define RRTNRBUF	1	/* VPM interrupt command */
#define RRTNEBUF	16	/* VPM interrupt command */
#define RCVLEN		4	/* number of receive buffers */
