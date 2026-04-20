/*  */
/* SID @(#)buffers.h	5.1 */
/* @(#)buffers.h	1.3 5/1/83 */


	/*----------------------------------------------------*\
	| Control structure for hdlc implementation to provide |
	| reliable data transfer between hosts. One entry per  |
	| host.						       |
	\*----------------------------------------------------*/

struct pdlctab {
	char         pd_state;		/* state of PDLC for this line */
	char         pd_Istate;		/* state of PDLC (INFO_TR) */
	char	     pd_flags;		/* misc flags for control */
	char         pd_VS;		/* next seq. # to be sent */
	char         pd_VR;		/* next seq. # expectd to be received*/
	char         pd_lastNR;		/* last NR received */
	ushort       pd_retries;	/* number of retries to this host */
	int          pd_timer;		/* id of timer t1 */
	struct cb   *pd_current;	/* current cb being xmitted */
	int          pd_count;		/* count of message out to this host */
	struct cb   *pd_next;		/* next cb to be xmitted */
	struct cbque pd_waiting;	/* cb placed here if window full */
	struct cbque pd_suq;		/* s/u frames to go out for this link*/
	struct cbque pd_out;		/* Q of cb's waiting to be acked */
};
