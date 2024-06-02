/*
 * "True" major device numbers. These correspond
 * to standard positions in the configuration
 * table, but are used for error logging
 * purposes only.
 */

#define RK0	0
#define RP0	1
#define RF0	2
#define TM0	3
#define TC0	4
#define HP0	5
#define HT0	6
#define HS0	7
#define RL0	8

/*
 * IO statistics are kept for each physical unit of each
 * block device (within the driver). Primary purpose is
 * to establish a guesstimate of error rates during
 * error logging.
 */

struct iostat {
	long	io_ops;		/* number of read/writes */
	long	io_misc;	/* number of "other" operations */
	ushort io_unlog;	/* number of unlogged errors */
};
