/*
 * This file contains the system routines that handle the processor board
 * channel attention interrupt.
 */

#include "sys/param.h"
#include "icp/sioc.h"	/* icp specific */
#include "icp/sioccomm.h"	/* icp specific */
#include "icp/icp.h"	/* icp specific */
#include "icp/pbsioc.h"	/* icp specific */

#ifdef COUNT
extern int profile[];
#endif
#ifdef TRACE
int trace();
#endif

struct	pctl	pctl;			/* proc. board I/F control table */

char	pborun;				/* output processor running flag */
char	pback;				/* send processor board ack command */
char	pbsync = 1;			/* proc. board sync , init. in sync */
char	nxtreset;			/* indicates that next reset for ack */
#ifndef	VPMSYS
int	pbflag;
long	pintcnt;
#else
long	intcnt;
#endif
long	catncnt;
long	rsetcnt;
long	intissued;
long	missedrset;
long	extrachan;
#ifndef VPMSYS
long	extrarset;
#endif
long	dupscnt;
int	pvalsave;
int	bpval1;
int	bpval2;
int	bpval3;
int	xpval;

/*
 * pbint - processes the channel attention interrupt from the processor 
 *         board.
 */

pbint()
{

	extern	int	mbicnt, *mbito;
	extern	unsigned	pbmbgo, pbogo, pbdrun;
	extern	long	mbifrom;
	extern	char	mbictl, mbiin;
#ifndef VPMSYS
	register	pval, s;
#else
	int	pval;
#endif
#ifdef DEBUG
	extern long debugc;
#endif;
#ifdef COUNT
	profile[49]++;
#endif


	/*
	 * reset the ctc interrupt so that another one can come thru
	 */

#ifdef DEBUG
	debugc++;
#endif
#ifdef	VPMSYS
	intcnt++;
#else
	pintcnt++;
#endif

	/* 
	 * read the channel attention port.  If bit ATTN is true, then
	 * this is a interrupt from the processor board.  Otherwise it is
	 * a reset interrupt.
	 */
#ifdef	VPMSYS
	bpval3 = bpval2;
	bpval2 = bpval1;
	pval = bpval1 = in_local( PCA );
#else
	pval = in_local( PCA );
#endif
	putreti();
#ifdef	VPMSYS
	if ( (intissued > rsetcnt) && !(pval & CLRINT) && !pbsync ) {
#else
	if ( !pbsync && !(pval & CLRINT) && (intissued > rsetcnt) ) {
#endif
		pval = CLRINT;
		missedrset++;
	}
#ifdef TRACE
trace(11,1,pval,(int)intissued,(int)rsetcnt,pbsync,(int)missedrset,nxtreset,
	pborun, (int)catncnt, mbiin, pctl.p_iq.pq_in);
#endif
	if ( pval & CLRINT ) {
		/*
		 * Process the reset interrupt.  If the output process is 
		 * running (and it should be to get a reset interrupt) call
		 * pbomore to see if there are any more output commands to
		 * send.
		 */
#ifdef	VPMSYS
		rsetcnt++;
		if ( nxtreset ) {
			nxtreset--;
			if ( pctl.p_iq.pq_cnt != PQSIZE )
				pbsync++;
		}
		if ( pborun > 1 ) {
			panic( "pborun" );
		}
		if ( pborun-- )
			pbomore();
		else {
			panic( "pb reset" );
			pctl.pc_rbad++;
		}
#else
		if ( pborun ) {
			rsetcnt++;
			if ( nxtreset ) {
				nxtreset--;
				if ( pctl.p_iq.pq_cnt != PQSIZE )
					pbsync++;
			}
		}
		if ( pborun ) {
			pborun = 0;
			pbomore();
		} else {
			extrarset++;
		}
#endif
	}
	if ( pval & ATTN ) {
		/*
		 * Process the processor board interrupt.  If the pb is in 
		 * sync (each int. requires explicite 'ack' from the SIOC 
		 * before the next int. can be issued), change the sync and
		 * set up multibus main memory access read request to the next
		 * entry in the input command queue.  There is always room, we
		 * don't 'ack' the previous int. unless there is.  If we are
		 * not in sync we are in trouble, issue message and count.
		 */
		catncnt++;
#ifdef	VPMSYS
		if ( (rsetcnt != intissued) && !pbsync ) {
#else
		if ( !pbsync && (rsetcnt != intissued) ) {
#endif
			panic( "CPU" );
		}
		if ( pbsync ) {
			pbsync--;
			mbicnt = sizeof(struct scb);
			mbito = ( int *)&pctl.p_iq.pq_e[pctl.p_iq.pq_in];
			if ( ++pctl.p_iq.pq_in == PQSIZE )
				pctl.p_iq.pq_in = 0;
			mbifrom = (long )pctl.pc_iaddr;
			mbiin++;
#ifdef	VPMSYS
			mbstart();
#else
			pbmbgo++;
			if ( !pbdrun )
				wakeup( (caddr_t)&pbdrun );
#endif
		} else {
			extrachan++;
#ifdef	VPMSYS
			xpval = pval;
#endif
			pctl.pc_ibad++;
		}
	}
#ifdef	VPMSYS
	if ( ((pval & CLRINT) && (pval & ATTN)) || 
				( !(pval & CLRINT) && !(pval & ATTN) ) ) {
		dupscnt++;
	}
	pvalsave = pval;
#endif
#ifdef TRACE
trace(11,2,pval,(int)intissued,(int)rsetcnt,pbsync,(int)missedrset,nxtreset,
	pborun, (int)catncnt, mbiin, pctl.p_iq.pq_in);
#endif
}
#ifndef	VPMSYS
unsigned	pbogo = 0;
unsigned	pbdrun = 0;
unsigned	pbmbgo = 0;

pbdaemon()
{
	register	pbo, pbmb;
	register	s;

	while ( 1 ) {
		s = spl7();
		while ( !pbogo && !pbmbgo ) {
			pbdrun = 0;
			sleep( (caddr_t)&pbdrun, PINOD-1 );
		}
		pbdrun++;
		pbo = pbogo;
		pbogo = 0;
		pbmb = pbmbgo;
		pbmbgo = 0;
		splx(s);
		if ( pbo )
			xpbomore();
		if ( pbmb )
			mbstart();
	}
}

pbomore()
{
	extern	unsigned	pbogo;
	register	s;

	s = spl7();
	pbogo++;
	if( !pbdrun )
		wakeup( (caddr_t)&pbdrun );
	splx(s);

}

#endif

/*
 * pbomore - see what output command processing has to be done.
 */

struct	scb	pboack = { MACKL, SIOCACK };

#ifdef	VPMSYS
pbomore()
#else
xpbomore()
#endif
{
	extern	int	mbocnt, *mbofrom;
	extern	long	mboto;
	extern	char	mboctl, mbiout;
	int	s;

	/* Only do something if the output processor is not running and
	 * there is either a processor board 'ack', or something in the
	 * output command, to send.
	 */
#ifdef COUNT
	profile[50]++;
#endif

	s = spl7();
#ifdef TRACE
trace(10,3,pborun,pback,pctl.p_oq.pq_cnt,mbiout,mbocnt,(int)mboto,mbofrom,
	(int)intissued,(int)rsetcnt);
#endif
	while ( !pborun && ( pback || pctl.p_oq.pq_cnt > 0 ) ) {
		if ( pback && pctl.p_iq.pq_cnt != PQSIZE ) {
			/* Set up a multibus access request for the 'ack' to
			 * the processor board and call mbstart to get the
			 * multibus function running if it isn't already.
			 */
			pborun++;
#ifdef	VPMSYS
			splx(s);
#endif
			mbocnt = MACKL;
			mboto = (long )pctl.pc_oaddr;
			mbofrom = ( int *)&pboack;
			mbiout++;
#ifndef	VPMSYS
			splx(s);
#endif
			mbstart();
			break;
		} else  {
			/* Set up a multibus access request for the next entry
			 * in the output queue and call mbstart to get the 
			 * multibus access function running if it isn't running
			 * already.
			 */
			pborun++;
#ifdef	VPMSYS
			splx(s);
#endif
			mbocnt = pctl.p_oq.pq_e[pctl.p_oq.pq_out].si_cblen;
			mboto = (long )pctl.pc_oaddr;
			mbofrom = ( int *)&pctl.p_oq.pq_e[pctl.p_oq.pq_out];
			if ( ++pctl.p_oq.pq_out == PQSIZE )
				pctl.p_oq.pq_out = 0;
			mbiout++;
#ifndef	VPMSYS
			splx(s);
#endif
			mbstart();
			break;
		}
	}
#ifdef TRACE
trace(10,4,pborun,pback,pctl.p_oq.pq_cnt,mbiout,mbocnt,(int)mboto,mbofrom,
	(int)intissued,(int)rsetcnt);
#endif
	splx(s);
}

#ifdef DEBUG
	/*
	 * DEBUG software. Catches missed mutltibus interrupts by
	 * Host. Goes to system debug and then issues missed interrupt
	 */

long lastintissued;
long lastrsetcnt;
mbimissed()
{
    if(intissued != rsetcnt) {
	if((intissued == lastintissued) && (rsetcnt == lastrsetcnt)) {
	    sysdebug();
	    out_local(PCMD,cmdsav|INT796);
	}
	lastintissued = intissued;
	lastrsetcnt = rsetcnt;
    }
#ifdef VPMSYS
    timeout(mbimissed,0,50, 2005);
#else
    timeout(mbimissed,0,50);
#endif
}
#endif
