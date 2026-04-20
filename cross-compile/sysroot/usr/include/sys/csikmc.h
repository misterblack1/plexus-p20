
/* SID @(#)csikmc.h	5.1 */
/*	csikmc.h 6.1 of 8/22/83
	@(#)csikmc.h	6.1
 */

/* Reports passed from interpreter */
#define	RRTNXBUF	0
#define	RRTNRBUF	1
#define	RTRACE		2
#define	ERRTERM		3
#define RTNSNAP		4
#define RTNER1		5
#define	RTNER2		6
#define	RTNSRPT		7
#define STARTUP		8
#define RTNACK		9
#define ITRACE		10
#define BASEACK		11
#define RTNSTOP		12

#ifndef PLEXUS
#define OKCHECK		15
#endif


/* Commands from the driver to the interpreter */
#define XBUFINCMD	0
#define RBUFINCMD	1
#define IRUNCMD		2
#define HALTCMD		3
#define SCRIPTCMD	4
#define GETECMD		5
#define BASEIN		6
#define OKCMD		15



/* interpreter options */
#define X25MODE		(01<<0)
#define HWLOOP 		(01<<1)
#define ADRSWTCH	(01<<2)
#define DSRMODE		(01<<3)

