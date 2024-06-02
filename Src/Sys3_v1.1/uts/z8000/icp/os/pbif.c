/*
 * This file contains SIOC system interface routines for processing commands
 * between the SIOC and the processor board.
 */

#include "sys/param.h"
#include "icp/sioccomm.h"	/* icp specific */
#include "icp/icp.h"	/* icp specific */
#include "icp/pbsioc.h"	/* icp specific */

#ifdef COUNT
extern int profile[];
#endif
#ifdef WAITING
extern int profile[];
#endif
extern	struct	pctl	pctl;		/* pb control structure */
#ifndef	VPMSYS
char	poqcnt = 0;
char	piqcnt = 0;
#endif

/*
 * msignal - queues a signal command to the main processor.
 */

msignal( mpgrp, msig, unit )
{
	register struct scb *p;
	int	s;

#ifdef COUNT
	profile[45]++;
#endif

	s = spl6();
	for (;;) {
		/* If the queue is not full, queue the signal command and
		 * call pbomore to get the output queue processor running 
		 * if it isn't already.  If the queue is full, wait on the
		 * count of entries in the queue.  The output queue processor
		 * will wake us up when an entry is taken out of the queue.
		 */
		if ( pctl.p_oq.pq_cnt < PQSIZE ) {
			p = &pctl.p_oq.pq_e[pctl.p_oq.pq_in];
			p->si_cblen = MSIGL;
			p->si_cmd = SIOCSIG;
			p->si_un.gdc3.si_mpgrp = mpgrp;
			p->si_un.gdc3.si_msig = msig;
			p->si_un.gdc3.si_unit = unit;
			if ( ++pctl.p_oq.pq_in == PQSIZE )
				pctl.p_oq.pq_in = 0;
			pctl.p_oq.pq_cnt++;
			splx(s);
			pbomore();
			break;
		} else {
#ifdef WAITING
			profile[10]++;
#endif
#ifndef	VPMSYS
			poqcnt++;
#endif
			sleep( (caddr_t)&pctl.p_oq.pq_cnt, SIOCPRI );
		}
	}
	splx( s );
}

/*
 * mreport - queues a report command to the main processor.
 */

#ifdef VPMSYS
mreport( type, unit, rpt, len )
register char rpt[];
{
	register struct scb *p;
	int	s;

#ifdef COUNT
	profile[45]++;
#endif

	s = spl6();
		/* If the queue is not full, queue the report command and
		 * call pbomore to get the output queue processor running 
		 * if it isn't already.  If the queue is full, return a value
		 * of "1".  The caller must re-call "mreport".
		 */
		if ( pctl.p_oq.pq_cnt < PQSIZE ) {
			p = &pctl.p_oq.pq_e[pctl.p_oq.pq_in];
			p->si_cblen = len;
			p->si_cmd = type;
			p->si_un.gdc5.si_parm[0] = unit;
			p->si_un.gdc5.si_parm[1] = rpt[0];
			p->si_un.gdc5.si_parm[2] = rpt[1];
			p->si_un.gdc5.si_parm[3] = rpt[2];
			p->si_un.gdc5.si_parm[4] = rpt[3];
			if ( ++pctl.p_oq.pq_in == PQSIZE )
				pctl.p_oq.pq_in = 0;
			pctl.p_oq.pq_cnt++;
			splx(s);
			pbomore();
/*
			printf("mreport:type=%x ,unit=%x\n", type, unit);
*/
	                return(0);
		} else {
#ifdef WAITING
			profile[10]++;
#endif
			splx( s );
		        return(1);
		}
}

/*
 * mrptslp - sleeps after caller calls mreport waiting
 *           for an entry in the queue.
 */

mrptslp()
{
     int s;

     s = spl6();
     sleep( (caddr_t)&pctl.p_oq.pq_cnt, SIOCPRI );
     splx( s );
}
#endif

/*
 * mwakeup - queues a wakeup command to the main processor.
 */

#ifdef SEQ
mwakeup( chan ,seq)
#else
mwakeup( chan )
#endif
caddr_t	chan;
{

	register struct scb *p;
	int	s;

#ifdef COUNT
	profile[46]++;
#endif

	s = spl6();
	for (;;) {
		/* If the queue is not full, put the wakeup command into it
		 * and call pbomore to start the output queue processor if
		 * it is not already running.  If the queue is full, sleep on
		 * the number of entries in the queue.  The output queue 
		 * processor will wake us up when an entry is taken from the
		 * queue.
		 */
		if ( pctl.p_oq.pq_cnt < PQSIZE ) {
			p = &pctl.p_oq.pq_e[pctl.p_oq.pq_in];
			p->si_cblen = MWUPL;
			p->si_cmd = SIOCWUP;
			p->si_un.gdc2.si_chan = chan;
#ifdef SEQ
p->si_un.gdc2.si_seq = seq;
#endif
			if ( ++pctl.p_oq.pq_in == PQSIZE )
				pctl.p_oq.pq_in = 0;
			pctl.p_oq.pq_cnt++;
			splx(s);
			pbomore();
			break;
		} else {
#ifdef WAITING
			profile[11]++;
#endif
#ifndef	VPMSYS
			poqcnt++;
#endif
			sleep( (caddr_t)&pctl.p_oq.pq_cnt, SIOCPRI );
		}
	}
	splx( s );
}

/*
 * Queue a write buffer flush command.
 */

#ifdef SEQ
mflush( ctlu ,seq)
#else
mflush( ctlu ,seq)
#endif
{
	register struct scb *p;
	int	s;

#ifdef COUNT
	profile[47]++;
#endif

	s = spl6();
	for (;;) {
		/* If the queue is not full, queue the flush command and
		 * call pbomore to get the output queue processor running 
		 * if it isn't already.  If the queue is full, wait on the
		 * count of entries in the queue.  The output queue processor
		 * will wake us up when an entry is taken out of the queue.
		 */
		if ( pctl.p_oq.pq_cnt < PQSIZE ) {
			p = &pctl.p_oq.pq_e[pctl.p_oq.pq_in];
			p->si_un.gdc4.si_ctlu = (struct ctlunit *)ctlu;
			p->si_cblen = MFLSHL;
			p->si_cmd = SIOCFLSH;
#ifdef SEQ
p->si_un.gdc2.si_seq = seq;
#endif
			if ( ++pctl.p_oq.pq_in == PQSIZE )
				pctl.p_oq.pq_in = 0;
			pctl.p_oq.pq_cnt++;
			splx(s);
			pbomore();
			break;
		} else {
#ifdef WAITING
			profile[12]++;
#endif
#ifndef	VPMSYS
			poqcnt++;
#endif
			sleep( (caddr_t)&pctl.p_oq.pq_cnt, SIOCPRI );
		}
	}
	splx( s );
}

/*
 * dqpbiq - dequeues next entry from proceesor board input command queue and
 * moves it to the specified location.
 */

dqpbiq( p )
register struct	scbw	*p;
{
	extern	char	pbsync, pback, nxtreset;
	register struct	scbw	*q;
	register int	s, i, j;

	/*
	 * If the queue is not empty, move the next entry to the specified
	 * place.  If the queue was full before this, change the processor
	 * board sync flag and call pbomore to 'ack' the last interrupt.
	 * If the queue is empty, sleep on the count of entries in the queue.
	 * When an entry is put into the queue, wakeup will be called.
	 */
#ifdef COUNT
	profile[48]++;
#endif

	s = spl6();
	for (;;) {
		if ( pctl.p_iq.pq_cnt > 0 ) {
			q = ( struct scbw *)&pctl.p_iq.pq_e[pctl.p_iq.pq_out];
			for ( i = 0, j = 0;
			      j < pctl.p_iq.pq_e[pctl.p_iq.pq_out].si_cblen;
			      i++, j += 2 ) {
				p->cbw[i] = q->cbw[i];
			}
			if ( ++pctl.p_iq.pq_out == PQSIZE )
				pctl.p_iq.pq_out = 0;
			if ( pctl.p_iq.pq_cnt-- == PQSIZE && ( !pback && !nxtreset ) ) {
				pbsync++;
				splx(s);
				pbomore();
			}
			break;
		} else {
#ifdef WAITING
			profile[13]++;
#endif
#ifndef	VPMSYS
			piqcnt++;
#endif
			sleep( (caddr_t)&pctl.p_iq.pq_cnt, SIOCPRI );
		}
	}
	splx( s );
}
