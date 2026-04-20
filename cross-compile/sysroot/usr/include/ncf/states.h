/*  */
/* SID @(#)states.h	5.1 */
/* @(#)states.h	1.4 8/9/84 */



	/*-----------------*\
	| connection states |
	\*-----------------*/

#define OPENREQUEST 1		/* open request to foreign host outstanding */
#define OPENING     2		/* awaiting rply to open (foreign ack/nak) */
#define OPEN        3		/* connection is open for communication */
#define CLOSING     4		/* connection is in process of being closed */
#define AOPENING    5		/* close during OPENING state. */
