/* SID @(#)lapbtr.h	5.1 */
/*	lapbtr.h 6.1 of 8/22/83
	@(#)lapbtr.h	6.1
 */

#define TRACE(X,Y,Z)	{U = X|Y; trace(U,Z);}
#define TSEQ	0
#define TSTART_T1	040
#define TSTOP_T1	0100
#define TXMT	0140
#define TRCV	0200
#define TNOBUF	0240
#define TOSTATE	0300
#define TNSTATE	0340
#define TDSCFRM	020
