/*
 * Each buffer in the pool is usually doubly linked into 2 lists:
 * the device with which it is currently associated (always)
 * and also on a list of blocks available for allocation
 * for other use (usually).
 * The latter list is kept in last-used order, and the two
 * lists are doubly linked to make it easy to remove
 * a buffer from one list when it was found by
 * looking through the other.
 * A buffer is on the available list, and is liable
 * to be reassigned to another disk block, if and only
 * if it is not marked BUSY.  When a buffer is busy, the
 * available-list pointers can be used for other purposes.
 * Most drivers use the forward ptr as a link in their I/O active queue.
 * A buffer header contains all the information required to perform I/O.
 * Most of the routines which manipulate these things are in bio.c.
 */
struct buf
{
	int	b_flags;		/* see defines below */
	struct	buf *b_forw;		/* headed by d_tab of conf.c */
	struct	buf *b_back;		/*  "  */
	struct	buf *av_forw;		/* position on free list, */
	struct	buf *av_back;		/*     if not BUSY*/
	dev_t	b_dev;			/* major+minor device name */
	unsigned b_bcount;		/* transfer count */
#ifdef vax
	union {
	    caddr_t b_addr;		/* low order core address */
	    int	*b_words;		/* words for clearing */
	    struct filsys *b_filsys;	/* superblocks */
	    struct dinode *b_dino;	/* ilist */
	    daddr_t *b_daddr;		/* indirect block */
	} b_un;

#define	paddr(X)	(paddr_t)(X->b_un.b_addr)

#else
	paddr_t	b_paddr;		/* physical address */
#define	paddr(X)	X->b_paddr
#endif
	daddr_t	b_blkno;		/* block # on device */
	char	b_error;		/* returned after I/O */
	unsigned int b_resid;		/* words not transferred after error */
#ifdef vax
	struct  proc  *b_proc;		/* process doing physical or swap I/O */
#endif
};

extern struct buf buf[];		/* The buffer pool itself */
extern struct buf bfreelist;		/* head of available list */
#ifdef vax
extern char buffers[][BSIZE];
#else
extern char sabuf[][BSIZE];
#endif

#ifdef pdp11
long	bigetl();
paddr_t	bufbase;
#endif

paddr_t	ubmaddr();

/*
 * These flags are kept in b_flags.
 */
#define	B_WRITE	0	/* non-read pseudo-flag */
#define	B_READ	01	/* read when I/O occurs */
#define	B_DONE	02	/* transaction finished */
#define	B_ERROR	04	/* transaction aborted */
#define	B_BUSY	010	/* not on av_forw/back list */
#define	B_PHYS	020	/* Physical IO potentially using UNIBUS map */
#define	B_MAP	040	/* This block has the UNIBUS map allocated */
#define	B_WANTED 0100	/* issue wakeup when BUSY goes off */
#define	B_AGE	0200	/* delayed write for correct aging */
#define	B_ASYNC	0400	/* don't wait for I/O completion */
#define	B_DELWRI 01000	/* don't write till block leaves available list */
#define	B_OPEN	02000	/* open routine called */
#define	B_STALE 04000
#define	B_UAREA	0100000	/* add u-area to a swap operation */

/*
 * Fast access to buffers in cache by hashing.
 */

#define	bhash(d,b)	((struct buf *)&hbuf[((int)d+(int)b)&v.v_hmask])

struct hbuf
{
	int	b_flags;
	struct	buf *b_forw;
	struct	buf *b_back;
};

extern struct hbuf hbuf[];
