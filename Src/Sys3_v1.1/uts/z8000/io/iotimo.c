#ifdef	OVKRNL
#include "sys/plexus.h"
#include "sys/param.h"
#include "sys/dir.h"
#include "sys/tty.h"
#include "sys/ttold.h"
#include "sys/user.h"
#include "icp/sioc.h"		/* icp specific */
#include "icp/siocunix.h"	/* icp specific */
#include "icp/siocprom.h"	/* icp specific */
#include "icp/sioccomm.h"	/* icp specific */
#include "icp/icp.h"		/* icp specific */
#include "icp/pbsioc.h"		/* icp specific */
#include "sys/sysinfo.h"
/*
 * Open failed routine for recognizing SIOC load failures.
 */

siocfail( ctl )
struct	sctl	*ctl;
{
	if ( ctl->sc_toseq == ctl->sc_intseq == 0x8000 ) {
		wakeup( (caddr_t) ctl->sc_paddr );
	}
}
 
/*
 * Close fail routine.
 */

closefail( rtn )
register struct closertn	*rtn;
{
#ifndef VPMSYS
	if ( rtn->rc_gate != 0 ) {
		rtn->rc_error = EIO;
		rtn->rc_gate = 0;
		wakeup( (caddr_t) rtn );
	}
#else
	rtn->rc_error = 0;
	rtn->rc_gate = 0;
	wakeup( (caddr_t) rtn );
#endif
}
long	tochanatn;

gochanatn( ctl )
register struct sctl	*ctl;
{
	if ( ctl->sc_oqrun ) {
		if ( ctl->sc_toseq == ctl->sc_intseq ) {
			out_multibus( ctl->sc_port, SIOCINT );
			tochanatn++;
		}	
	}
	ctl->sc_toseq = ctl->sc_intseq;
	timeout( gochanatn, ctl, 50 );
}
#define	TESTDONE	5	/* selftest done state */
selftestdone(ctl)
struct sctl *ctl;
{
	ctl->sc_state = TESTDONE;
	wakeup((caddr_t) ctl);
}
long	siwfent, siwfrwait;
extern	long	flshsent;

siwflsh( ctlu )
struct	ctlunit	*ctlu;
{
	struct	sctl 	*ctl;
	struct	scb	*scb;
	struct	writelcb	*lcb;
	int	unit;
	register struct sctltp *ctltp;
	register struct	sql *sqe;
	register struct scbw	*ca;
	register	i, j, len;
	int	out;

	ctl = ctlu->cu_ctl;
	unit = ctlu->cu_unit;
	ctltp = &ctl->tp_un[unit];
	/* if data go out on line, then wait a bit and try again */
	if((ctltp->sc_tstat & WOK) == 0) {
		timeout(siwflsh, ctlu, 4);
		return;
	}
	/* if "ignore timeout", then reset state to "idle" and return */
	else if(ctltp->sc_timstate == -1) {
		ctltp->sc_timstate = 0;
		return;
	}
	/* if "initiate second timeout", then do so and return. */
	else if(ctltp->sc_timstate == 2) {
		ctltp->sc_timstate = 1;
		timeout(siwflsh, ctlu, 4);
		return;
	}
	/* if "idle", then there are problems */
	else if(ctltp->sc_timstate == 0) {
		printf("unexpected write flush\n");
		return;
	}
	/* if "timeout pending" or "final timeout", send buffer */
	else if((ctltp->sc_timstate==1) || (ctltp->sc_timstate == 3)) {
		if ( ctl->s_oq.sq_cnt < SQSIZE ) {
			flshsent++;
			/*
			 * set semiphore so that no one else can enter
			 * siwrite critical region. Note that all changes
			 * of timeout state occure there, or here in siwflsh
			 * or in sioci() in unix/sioc.c. We let the return
			 * from the icp to reset the timeout state to "idle".
			 * we do this so that siioctl will sleep until the
			 * return. ( the state will be > 0).
			 */
			ctltp->sc_tstat &= ~WOK;
			scb = (struct scb *)ctltp->sc_wscbbuf;
			lcb = (struct writelcb *)scb->si_un.gdc5.si_parm;
			/*
			 * the icp checks lw_ctlu and returns a specal signal
			 * if the write is from a timeout. When the main
			 * processor is interrupted, this special signal
			 * (SIOCFLSH) will cause the release of the buffers
			 */
			lcb->lw_ctlu = (int)ctlu;
			lcb->lw_count = ctltp->sc_bspot;
			/* flush is made to be nobody so no one can abort it */
			lcb->lw_procp = 0;
			lcb->lw_pid = 0;
#ifdef SEQ
			lcb->lw_seq = ++numsent;
#endif
			ctl->s_oq.sq_e[ctl->s_oq.sq_in].sq_addr =
					(int *)scb;
			if ( ++ctl->s_oq.sq_in == SQSIZE )
				ctl->s_oq.sq_in = 0;
			ctl->s_oq.sq_cnt++;
			/*
			if ( !(ctltp->sc_wbufs) ) {
				printf( "flshing w/o a buffer-take dump!\n" );
				debug();
			}
			trcwrt( 2, unit, ctltp->sc_bspot, 0,
				 scb, lcb, 0, 0, 0, 0 );
			*/
			if ( !ctl->sc_oqrun ) {
				out = ctl->s_oq.sq_out;
				sqe = &ctl->s_oq.sq_e[out];
				len = *(sqe->sq_addr);
				ca = (struct scbw *)ctl->sc_oaddr;
				if ( len & 1 )
					len++;
				for ( i = 0, j = 0; j < len; i++, j += 2 ) 
					ca->cbw[i] = *(sqe->sq_addr)++;
				if ( ++ctl->s_oq.sq_out == SQSIZE )
					ctl->s_oq.sq_out = 0;
				ctl->sc_intseq++;
				ctl->sc_oqrun++;
				ctl->s_oq.sq_cnt--;
				out_multibus( ctl->sc_port, SIOCINT );
			}
		}
		else  {
			timeout( siwflsh, ctlu, 4 );
		}
	}
}
extern	struct	sctl	sctl[];
siabort()
{
	register int i;

	for(i=0;i<NUMICP;i++) {
#ifdef VPMSYS
		if ( srchtp( &sctl[i], i ) )
#else
		if ( srchtp( &sctl[i] ) ) 
#endif
			return;
	}
}
tttimeo(tp)
register struct tty *tp;
{
	tp->t_state &= ~TACT;
	if (tp->t_iflag&ICANON || tp->t_cc[VTIME] == 0)
		return;
	if (tp->t_rawq.c_cc == 0)
		return;
	if (tp->t_state&RTO) {
		tp->t_delct = 1;
		if (tp->t_state&IASLP) {
			tp->t_state &= ~IASLP;
			wakeup((caddr_t)&tp->t_rawq);
		}
	} else {
		tp->t_state |= RTO|TACT;
		timeout(tttimeo, tp, tp->t_cc[VTIME]*(HZ/10));
	}
}
/*
 * Restart typewriter output following a delay timeout.
 * The name of the routine is passed to the timeout
 * subroutine and it is called during a clock interrupt.
 */
ttrstrt(tp)
register struct tty *tp;
{

	(*tp->t_proc)(tp, T_TIME);
}
#endif
