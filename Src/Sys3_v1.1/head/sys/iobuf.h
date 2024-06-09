/*
 * Each block device has a iobuf, which contains private state stuff
 * and 2 list heads: the b_forw/b_back list, which is doubly linked
 * and has all the buffers currently associated with that major
 * device; and the d_actf/d_actl list, which is private to the
 * device but in fact is always used for the head and tail
 * of the I/O queue for the device.
 * Various routines in bio.c look at b_forw/b_back
 * (notice they are the same as in the buf structure)
 * but the rest is private to each device driver.
 */
struct iobuf
{
	int	b_flags;		/* see buf.h */
	struct	buf *b_forw;		/* first buffer for this dev */
	struct	buf *b_back;		/* last buffer for this dev */
	struct	buf *b_actf;		/* head of I/O queue */
	struct 	buf *b_actl;		/* tail of I/O queue */
	dev_t	b_dev;			/* major+minor device name */
	char	b_active;		/* busy flag */
	char	b_errcnt;		/* error count (for recovery) */
	struct eblock	*io_erec;	/* error record */
	int	io_nreg;		/* number of registers to log on errors */
	physadr	io_addr;		/* csr address */
#ifdef vax
	physadr	io_mba;			/* mba address */
#endif
	struct	iostat	*io_stp;	/* unit I/O statistics */
	int	io_s1;			/* space for drivers to leave things */
	int	io_s2;			/* space for drivers to leave things */
};

#ifdef vax
#define tabinit(dv,stat)	{0,0,0,0,0,makedev(dv,0),0,0,0,0,0,0,stat,0,0}
#else
#define tabinit(dv,stat)	{0,0,0,0,0,makedev(dv,0),0,0,0,0,0,stat,0,0}
#endif
#define NDEVREG	(sizeof(struct device)/sizeof(int))

#define	B_ONCE	01	/* flag for once only driver operations */
#define	B_TAPE	02	/* this is a magtape (no bdwrite) */
#define	B_TIME	04	/* for timeout use */
