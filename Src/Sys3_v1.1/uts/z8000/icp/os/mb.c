/*
 * This file contains code that handles all accesses to main memory through
 * the multibus.
 */

#include "sys/param.h"
#ifndef	VPMSYS
#include "sys/plexus.h"
#endif
#include "icp/sioc.h"	/* icp specific */
#include "icp/sioccomm.h"	/* icp specific */
#include "icp/icp.h"	/* icp specific */
#include "icp/pbsioc.h"	/* icp specific */

#ifdef COUNT
extern int profile[];
#endif
#ifdef WAITING
extern int profile[];
#endif
#ifdef TRACE
extern long rsetcnt;
int trace();
#endif
extern	long	intissued;
long	ackcnt;

/*
 * multibus main memory access request queue.
 */

struct	mbq	mbq;			/* multibus main memory access queue */

/*
 * communications block between mbmore and mbint.
 */

int	mbcnt;				/* number of words in transfer */
/* changed */	char	*mbfrom;	/* from address (LSB 16) */
/* changed */ 	char	*mbto;		/* to address (LSB 16) */
int	mbflag;				/* flag indicating called from i_mb */
#ifndef	VPMSYS
extern	int	pbflag;			/* pb interrupt occurred flag */
extern	pbint();
unsigned	pbcall;
unsigned	pbnocall;
char	mbqcnt;				/* mb queue entries needed flag */
extern	char	poqcnt, piqcnt;
#endif

/*
 * operation flag and state.
 */

char	mbrun;				/* multibus access running flag */
char	mbstate;			/* state of multibus activity */

/*
 * communications block between mbmore and pbint.  
 */

int	mbicnt;				/* number of words in transfer */
long	mbifrom;			/* 20 bit main memory address */
char	mbiin;				/* communications block filled flag */
/* changed */ char	*mbito;			/* SIOC memory address */

/*
 * communications block bewteen mbmore and pbomore.
 */

char	mbiout;				/* communications block filled flag */
int	mbocnt;				/* number of words in transfer */
/* changed */ char	*mbofrom;		/* SIOC address */
long	mboto;				/* 20 bit main memory address */

/*
 * mbmore - see what multibus main memory access is required.
 */

extern	char	pback, nxtreset;
extern	struct	scb	pboack;
extern	struct	pctl	pctl;
char	smb_out;			/* last queue entry processed */
int	smbcnt;				/* saved byte count for mult. xfer */
#ifndef	VPMSYS
unsigned mbgo = 0;
unsigned mbdrun = 0;
unsigned intsoff = 0;
#endif
#ifdef	MDP
long	nvicnt;
#endif
#ifdef	VPMSYS
mbmore()
#else
mbmore( r0, r1, r2, r3, r4, r5, r6, r7, id, fcw, pc )
register	fcw;
#endif
{

#ifndef	VPMSYS
	register	spri;
	register struct mq *mbqp;
#endif
#ifdef COUNT
	profile[39]++;
#endif

#ifndef	VPMSYS
	if ( mbflag ) {
#ifdef	MDP
		nvicnt++;
#endif
		if ( fcw & B_VI ) {
			spri = spl0();
			splx(spri);
		} else intsoff++;
	}
#endif
	/*
	 * reset interrupt chain and turn on interrupts
	 */

#ifdef TRACE
trace(11,5,mbstate,mbiout,mbiin,(int)mboto,mbocnt,pback,nxtreset,(int)ackcnt,
	(int)intissued,(int)rsetcnt);
#endif

	/*
	 * mbstate always tells us what to do.  Switch on it.
	 */
#ifndef	VPMSYS
	mbbegin:
#endif
	switch ( mbstate ) {

	/*
	 * Start the next multibus main memory access.  The priority order
	 * is output commands, input commands, queued multibus access 
	 * requests.
	 */
	case MBSTART:
		if ( mbiout ) {
			if ( mboto != pctl.pc_oaddr ) {
				panic( "MB out error!\n" );
			}
			if ( (((long)mboto & MBADRMSK) + mbocnt) 
					> (long)RAM ) {
				mbcnt = (char)(((long)RAM - 
					( (long)mboto & MBADRMSK )) >> 1);
				mbstate = MBIOMID;
			} else {
				mbcnt = ( mbocnt >> 1 );
				mbstate = MBIOUT;
				mbiout--;
			}
			mbto = (char *)
				(( (long)mboto & MBADRMSK ) | MBADRADD );
			mbfrom = mbofrom;
#ifdef TRACE
trace(11,6,mbstate,mbiout,mbiin,(int)mboto,mbocnt,pback,nxtreset,(int)ackcnt,
	(int)intissued,(int)rsetcnt);
#endif
			out_local( P796MSB, 
		  	  (int)((((long)mboto >> MBMSBSHFT) & MBMSBMSK) | 1) );
			mres();
			break;
		}
		if ( mbiin ) {
			if ( mbifrom != pctl.pc_iaddr ) {
				panic( "MB in error!\n" );
			}
			if ( (((long)mbifrom & MBADRMSK) + mbicnt) 
					> (long)RAM ) {
				mbcnt = (((long)RAM - 
					((long)mbifrom & MBADRMSK)) >> 1 );
				mbstate = MBIIMID;
			} else {
				mbcnt = ( mbicnt >> 1 );
				mbstate = MBIIN;
				mbiin--;
			}
			mbto = mbito;
			mbfrom = (char *)
				(( (long)mbifrom & MBADRMSK ) | MBADRADD );
#ifdef TRACE
trace(11,7,mbstate,mbiout,mbiin,(int)mboto,mbocnt,pback,nxtreset,(int)ackcnt,
	(int)intissued,(int)rsetcnt);
#endif
			out_local( P796MSB, 
			  (int)((((long)mbifrom >> MBMSBSHFT) & MBMSBMSK)|1));
			mres();
			break;
		}
		if ( mbq.mb_cnt > 0 ) {
#ifndef	VPMSYS
			mbqp = &mbq.mb_q[mbq.mb_out];
			if ( mbqp->mb_bcnt <= MBMAX ) {
#else
			if ( mbq.mb_q[mbq.mb_out].mb_bcnt <= MBMAX ) {
#endif
				mbstate = MBMLAST;
#ifdef	VPMSYS
				mbcnt = ( mbq.mb_q[mbq.mb_out].mb_bcnt >> 1);
#else
				mbcnt = ( mbqp->mb_bcnt >> 1);
#endif
			} else {
				mbstate = MBMMDL;
				mbcnt = ( MBMAX >> 1);
#ifdef	VPMSYS
				smbcnt = mbq.mb_q[mbq.mb_out].mb_bcnt;
#else
				smbcnt = mbqp->mb_bcnt;
#endif
			}
#ifdef	VPMSYS
			mbto = (char *)mbq.mb_q[mbq.mb_out].mb_to;
			mbfrom = (char *)mbq.mb_q[mbq.mb_out].mb_from;
			out_local( P796MSB, mbq.mb_q[mbq.mb_out].mb_ctl );
#else
			mbto = (char *)mbqp->mb_to;
			mbfrom = (char *)mbqp->mb_from;
			out_local( P796MSB, mbqp->mb_ctl );
#endif
#ifdef TRACE
trace(11,8,mbstate,mbiout,mbiin,(int)mboto,mbocnt,pback,nxtreset,(int)ackcnt,
	(int)intissued,(int)rsetcnt);
#endif
#ifdef	VPMSYS
#else
#endif
			smb_out = mbq.mb_out;
			if ( ++mbq.mb_out == MBQSIZE )
				mbq.mb_out = 0;
			mbq.mb_cnt--;
			mres();
#ifdef	VPMSYS
			wakeup ( (caddr_t)&mbq.mb_cnt );
#else
			if ( mbqcnt ) {
				mbqcnt = 0;
				wakeup ( (caddr_t)&mbq.mb_cnt );
			}
#endif
			break;
		} else {
			mbrun--;
			break;
		}

	/* Just completed part of copying output command to comm. area. */
	case MBIOMID:
		(char *)mbofrom += (mbcnt << 1);
		mboto += (long)(mbcnt << 1);
		mbocnt -= (mbcnt << 1);
		mbstate = MBSTART;
#ifdef	VPMSYS
		mbmore();
#else
		goto mbbegin;
#endif
		break;

	/* Just completed copying output command to communications area. */
	case MBIOUT:
		if ( mbfrom == (char *)&pboack ) {
			pback--;
			nxtreset++;
			ackcnt++;
		}
		else
			pctl.p_oq.pq_cnt--;
#ifdef	VPMSYS
		wakeup( (caddr_t)&pctl.p_oq.pq_cnt );
#else
		if ( poqcnt ) {
			poqcnt = 0;
			wakeup( (caddr_t)&pctl.p_oq.pq_cnt );
		}
#endif
#ifdef TRACE
trace(11,9,mbstate,mbiout,mbiin,(int)mboto,mbocnt,pback,nxtreset,(int)ackcnt,
	(int)intissued,(int)rsetcnt);
#endif
		out_local( PCMD, cmdsav | INT796 );	/* send int to main */
		intissued++;
		mbstate = MBSTART;
#ifdef	VPMSYS
		mbmore();
#else
		goto mbbegin;
#endif
		break;

	/* Just completed part of moving a command block to the input queue. */
	case MBIIMID:
		mbifrom += (long)(mbcnt << 1);
		(char *)mbito += (mbcnt << 1);
		mbicnt -= (mbcnt << 1);
		mbstate = MBSTART;
#ifdef	VPMSYS
		mbmore();
#else
		goto mbbegin;
#endif
		break;

	/* Just completed moving a command block to the input queue. */
	case MBIIN:
		pctl.p_iq.pq_cnt++;
#ifdef	VPMSYS
		wakeup( (caddr_t)&pctl.p_iq.pq_cnt );
#else
		if ( piqcnt ) {
			piqcnt = 0;
			wakeup( (caddr_t)&pctl.p_iq.pq_cnt );
		}
#endif
		pback++;
		pbomore();
		mbstate = MBSTART;
#ifdef	VPMSYS
		mbmore();
#else
		goto mbbegin;
#endif
		break;

	/* Just completed partial transfer of a block. */
	case MBMMDL:
		smbcnt -= MBMAX;
		if ( smbcnt <= MBMAX ) {
			mbstate = MBMLAST;
			mbcnt = ( smbcnt >> 1);
		} else {
			mbcnt = ( MBMAX >> 1 );
		}
/* changed */	mbto += MBMAX;
/* changed */	mbfrom += MBMAX;
		mres();
		break;

	/* Just completed the last part of block transfer */
	case MBMLAST:
#ifdef	VPMSYS
		mbq.mb_q[smb_out].mb_amb++;
		wakeup( (caddr_t)&mbq.mb_q[smb_out].mb_amb );
#else
		mbqp = &mbq.mb_q[smb_out];
		mbqp->mb_amb++;
		wakeup( (caddr_t)&mbqp->mb_amb );
#endif
		mbstate = MBSTART;
#ifdef	VPMSYS
		mbmore();
#else
		goto mbbegin;
#endif
		break;
	}
#ifndef	VPMSYS
	if ( mbflag ) {
		mbflag = 0;
		if ( fcw & B_VI ) {
			spri = spl0();
			splx(spri);
		}
	}
#endif
}

/*
 * mbstart - starts the multibus main memory access code if it is not 
 * already running.
 */

mbstart()
{

	/*
	 * If the multibus main memory access function is not runnning,
	 * start it.
	 */
	int	s;

#ifdef COUNT
	profile[40]++;
#endif

	s = spl7();
	if ( !mbrun ) {
		mbrun++;
		splx(s);
		mbstate = MBSTART;
		mbmore();
	} else splx(s);
}

/*
 * qmbq - queues an entry on the multibus main memory access queue.
 */

struct mq *qmbq( cnt, to, from, ctl )
int	*to, *from;
char	ctl;
{

	register struct	mq	*p;
	int	savint;

#ifdef COUNT
	profile[41]++;
#endif

	for (;;) {
		savint = spl6();
		if ( mbq.mb_cnt < MBQSIZE ) {
			p = &mbq.mb_q[mbq.mb_in];
			p->mb_amb = 0;
			p->mb_bcnt = cnt;
			p->mb_to = to;
			p->mb_from = from;
			p->mb_ctl = ctl;
			if ( ++mbq.mb_in == MBQSIZE )
				mbq.mb_in = 0;
			mbq.mb_cnt++;
			break;
		} else {
#ifdef WAITING
			profile[7]++;
#endif
#ifndef	VPMSYS
			mbqcnt++;
#endif
			sleep( (caddr_t)&mbq.mb_cnt, SIOCPRI );
		}
	}
	splx( savint );
	mbstart();
	return( p );
}

/*
 * mvtomb - moves from SIOC local memory to main memory.
 */

mvtomb( mbx, lclx, cnt )
register long	mbx;
register char	*lclx;
register cnt;
{

	register struct	mq	*p;
	register int	scnt;
	int savint;
 
	/*
	chkit( mbx );
	*/
#ifdef COUNT
	profile[42]++;
#endif

	if ( !cnt )
		return;
	if ( (( mbx & MBADRMSK ) + cnt ) > (long)RAM ) {
		scnt = ((long)RAM - ( mbx & MBADRMSK ));
		cnt -= scnt;
		p = qmbq( scnt, (int *)((mbx & MBADRMSK) | MBADRADD), lclx,
			  (char)((( mbx >> MBMSBSHFT) & MBMSBMSK) | 1 ) );
		mbx += scnt;
		lclx += scnt;
		p = qmbq( cnt , (int *)((mbx & MBADRMSK) | MBADRADD), lclx,
			  (char)((( mbx >> MBMSBSHFT) & MBMSBMSK) | 1) );
	} else {
		p = qmbq( cnt, ( int *)( ( mbx & MBADRMSK ) + MBADRADD ), lclx,
			  ( char )( ( mbx >> MBMSBSHFT ) & MBMSBMSK | 1 ) );
	}
	savint = spl6();
	for (;;) {
		if ( p->mb_amb == 0 ) {
#ifdef WAITING
			profile[8]++;
#endif
			sleep( (caddr_t)&p->mb_amb, SIOCPRI );
		} else break;
	}
	splx( savint );
}

/*
 * mvfrommb - moves from main memory to SIOC local memory.
 */

mvfrommb( lclx, mbx, cnt )
register long	mbx;
register char	*lclx;
register cnt;
{
	register struct	mq	*p;
	register int	scnt;
	int savint;

	/*
	chkit( mbx );
	*/
#ifdef COUNT
	profile[43]++;
#endif

	if ( !cnt )
		return;
	if ( (( mbx & MBADRMSK ) + cnt ) > (long)RAM ) {
		scnt = ((long)RAM - ( mbx & MBADRMSK ));
		cnt -= scnt;
		p = qmbq( scnt, lclx, (int *)((mbx & MBADRMSK) | MBADRADD),
			  (char)((( mbx >> MBMSBSHFT) & MBMSBMSK) | 1) );
		mbx += scnt;
		lclx += scnt;
		p = qmbq( cnt, lclx, (int *)((mbx & MBADRMSK) | MBADRADD),
			  (char)((( mbx >> MBMSBSHFT) & MBMSBMSK) | 1) );
	} else {
		p = qmbq( cnt, lclx, ( int *)( ( mbx & MBADRMSK ) + MBADRADD ),
	  		  ( char )( ( mbx >> MBMSBSHFT ) & MBMSBMSK | 1 ) );
	}
	savint = spl6();
	for (;;) {
		if ( p->mb_amb == 0 ) {
#ifdef WAITING
			profile[9]++;
#endif
			sleep( (caddr_t)&p->mb_amb, SIOCPRI );
		} else break;
	}
	splx( savint );
}
/* Debug software for mbfrom & mbto */
/*
chkit( mbx )
long mbx;
{

#ifdef COUNT
	profile[44]++;
#endif

	if ( mbx < pctl.pc_min || mbx >= pctl.pc_max )
		if ( mbx != pctl.pc_paddr && mbx != (pctl.pc_paddr + 2) ) {
			panic( "MB mem range err!\n" );
		}
}
