/*  */
/* SID @(#)misc.h	5.1 */
/* @(#)misc.h	1.8 9/12/84 */




		/* miscellaneous defines */

#define EMPTY 0		/* used to indicate empty ptovc table entry */
#define IN_USE 1	/* indicates an object is `in use' */
#define NCF_CHANNEL 0	/* channel for communicating with NCF */
#define NCF_IN_USE 2	/* indicates channel in use and is `attach' channel */
#define DETACH 0x8000	/* indicates that nclose is to ignore check on pid */
#define FREE   0	/* indicates channel is unallocated */
#define PDLC   0	/* protocol used by ncf */
#define WAIT   1	/* if no vc block, then wait for one to become free */
#define NOWAIT 0	/* if no vc block, then return immediately with 0 */
#define DFS_PAD	64	/* size of dfs data in cb_des[1] */
#define NCFTYPE 1	/* ncf type message. should be in param.h/types.h */


#define ONCFPRI 28	/* priority for NCF open sleep */
#define RNCFPRI 28	/* priority for NCF read sleep */
#define CBWPRI  27      /* priority for CB blocks exausted wakeup */

			/* NATTACH paramater constants */
#define NOCHK 0x8000	/* do no check of ownership on NCF request */

			/* NIOCTL commands */
#define NGETSTAT 1	/* get bit vector indicating which line has msgs */
#define NGETMSG  2	/* # of messages received on this line */

#define TRUE  1
#define FALSE 0
#ifdef NULL
#undef NULL
#endif
#define NULL  0

#ifndef YES
#define YES   1
#else
#if YES != 1
int;			/* something guaranted to produce an error */
#endif
#endif

#ifndef NO
#define NO    0
#else
#if NO != 0
int;			/* something guaranted to produce an error */
#endif
#endif

/*  	NOS Beta Release Phase 1 timeout values */
#define OREQT	750	/* timeout for open request (15 seconds) */
#define ORPYT	500	/* timeout for process open reply (10 seconds) */
#define RUTT    750 	/* timeout for are you there (15 seconds) */
#define POPT	750	/* timeout for partial open recovery (15 seconds) */
#define TENSEC    250	/* timeout for retry if memory shortage (5 seconds) */

#define NET_DATA_SPACE    ((MBNETSEG)<<12) /* dfs data space for frame data */
#define SYSTEM_DATA_SPACE ((MBSDSEG)<<12)  /* sys data space for frame data */
