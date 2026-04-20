/* SID @(#)trace.h	5.1 */
/*	trace.h 6.1 of 8/22/83
	@(#)trace.h	6.1
 */

/*
 * Per trace structure
 */
struct trace {
	struct 	clist tr_outq;
	short	tr_state;
	short	tr_chbits;
	short	tr_rcnt;
	unsigned char	tr_chno;
	char	tr_ct;
};
