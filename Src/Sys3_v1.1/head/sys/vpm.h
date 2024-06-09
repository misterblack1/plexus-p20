/*
 * Common definitions for vpm driver and vpm interpreter
 */
/*
 * Commands from the driver to the interpreter
 */
#define	XBUFINCMD	0
#define	RBUFINCMD	1
#define	RUNCMD		2
#define	HALTCMD		3
#define	OKCMD		4
#define	SCRIPTCMD	5
#define	GETECMD		6
/*
 * Reports passed from the interpreter to the driver
 */
#define	RRTNXBUF	0
#define	RRTNRBUF	1
#define	RTRACE		2
#define	ERRTERM		3
#define	RTNOK		4
#define	RTNSNAP		5
#define	RTNERCNT	6
#define	RTNSRPT		7
#define	STARTUP		8
#define	RTNACK		9
/*
 * Define error codes for process-termination signal
 */
#define	ERRMASK		0377
#define	HALTRCVD	0
