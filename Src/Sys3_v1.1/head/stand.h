#define SCCSID @(#)saio.h	4.2

/*
 * Header file for standalone package
 */

#include <errno.h>
#include <sys/param.h>
#include <sys/inode.h>

/*
 * I/O block flags
 */

#define F_READ	01
#define F_WRITE	02
#define F_ALLOC	04
#define F_FILE	010

/*
 * Request codes -- must be
 * the same as an F_XXX above
 */

#define	READ	1
#define	WRITE	2

/*
 * Buffer sizes
 */

#define BLKSIZ	512
#define NAMSIZ	60

/*
 * devsw table --
 * initialized in conf.c
 */

struct devsw {
	int	(*dv_strategy)();
	int	(*dv_open)();
	int	(*dv_close)();
} _devsw[];

/*
 * dtab table -- entries
 * are created by MKNOD
 */

#define NDEV	6

struct dtab {
	char		*dt_name;
	struct devsw	*dt_devp;
	int		dt_unit;
	daddr_t		dt_boff;
} _dtab[NDEV];

/*
 * mtab table -- entries
 * are created by mount
 */

#define NMOUNT	4

struct mtab {
	char		*mt_name;
	struct dtab	*mt_dp;
} _mtab[NMOUNT];

/*
 * I/O block: includes an inode,
 * cells for the use of seek, etc,
 * and a buffer.
 */

#define NFILES	6

struct iob {
	char		i_flgs;
	struct inode	i_ino;
	time_t		i_atime;
	time_t		i_mtime;
	time_t		i_ctime;
	struct dtab	*i_dp;
	off_t		i_offset;
	daddr_t		i_bn;
	char		*i_ma;
	int		i_cc;
	char		i_buf[512];
} _iobuf[NFILES];

#ifdef pdp11
/*
 * Set to which 32Kw segment the code is physically running in.
 * Must be set by the user's main (or thereabouts).
 */

int	segflag;
#endif

/*
 * Set to the error type of the routine that
 * last returned an error -- may be read by perror.
 */

int	errno;
