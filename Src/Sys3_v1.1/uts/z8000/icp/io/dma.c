#include "sys/param.h"
#include "sys/tty.h"
#include "icp/sioc.h"	/* icp specific */
#include "icp/dma.h"	/* icp specific */
#ifndef VPMSYS
#include "icp/pio.h"	/* icp specific */
#endif
#ifdef VPMSYS
#include "icp/sio.h"
extern int wr5sav[];
#endif

#ifdef COUNT
extern int profile[];
#endif
extern char sio_xon,sio_xoff;
extern	struct	tty	ttytab[];
extern struct	pios	pio;
#ifdef MDP
int	dmawakeup = 0;
int	dmasleep = 0;
#endif

	/*------------------*\
	| dma initialization |
	\*------------------*/

dmainit()
{
	register chan;

	/* init dma 0 (units 8-11) */

	out_local(pdmac[8] | WCR, 0 );
	out_local(pdmac[8] | WMR, CASCADEMODE | (0 << SCHAN));
	out_local(pdmac[8] | WSMRB, RESETMSK | 0 );
	out_local(pdmac[8] | WMR, CASCADEMODE | (1 << SCHAN));
	out_local(pdmac[8] | WSMRB, RESETMSK | 1 );
	out_local(pdmac[8] | WMR, READXFER | SNGLMODE | (2 << SCHAN));

	/* init dma 1 (units 0-3) */

	out_local(pdmac[0] | WCR, LOWDREQ);
	for (chan = 0; chan < 4; chan++) {
		out_local(pdmac[0] | WMR, READXFER | SNGLMODE | (chan<<SCHAN));
	}

	/* init dma 2 (units 4-7) */

	out_local(pdmac[4] | WCR, LOWDREQ);
	for (chan = 0; chan < 4; chan++) {
		out_local(pdmac[4] | WMR, READXFER | SNGLMODE | (chan<<SCHAN));
	}
}

#ifdef	MDP
struct {
	unsigned	dt_start;
	unsigned	dt_int;
	unsigned	dt_complete;
} dt[8];
#endif

/*
 * start a dma transfer
 */


startdma(unit, adx, cnt)
register char *adx;
register unit,cnt;
{

#ifdef	VPMSYS
	register pac,pax,paa;
#else
	register pac,pax,paa,s;
#endif
#ifdef COUNT
	profile[13]++;
#endif

#ifdef	MDP
	dt[unit].dt_start++;
#endif
	pac = pdmac[unit];
	pax = pdmax[unit];
	paa = pdmaa[unit];
#ifndef	VPMSYS
	s = spl7();
#endif
	out_local(pac | CBPFF, 0);	/* clear byte pointer */
	out_local(paa, adx);	/* lower byte of address */
	out_local(paa, (int)adx >> 8);	/* upper byte of address */
	cnt--;
	out_local(pax, cnt);	/* lower byte of count */
	out_local(pax, cnt >> 8);	/* upper byte of count */
	out_local(pac | WSMRB, RESETMSK | MCHAN & unit); /* start */
#ifndef VPMSYS
	splx(s);
#endif
}

#ifdef TDEBUG
struct help {
	char portid;
	char numsent;
	int ttystate;
	char *curcblk;
	char *nxtcblk;
	int firstsnt;
	int lastsnt;
} tdata[100];

int tdcnt = 0;
#endif

#ifndef VPMSYS
unsigned	dmado = 0;
unsigned	dmarun = 0;
dma( r0, r1, r2, r3, r4, r5, r6, r7, id, fcw, pc )
register	id;
{

	register unit;
	register i;
	register status;
	/* output interrupt acknowledgement (Intel 8237 doesn't send one) */

	putreti();

	/* is it the Paralled port that finished? */

	if (id == IDDMA0END) {
		status = ( in_local(pdmac[8] | RSR ) & 0xff );
		if ( (status >> 2) & 1 ) {
			pio.pb_busy = 0;
			wakeup( (caddr_t)&pio );
		}
		return;
	} else if (id == IDDMA1END) {
		unit = 0;
	} else {
		unit = 4;
	}

	/* get the status of the interrpting dma */

	status = (in_local(pdmac[unit] | RSR) & 0xf);

	dmado |= (status << unit);
#ifdef MDP
	for ( i = 0; i < 4; i++, unit++ ) {
		if ( !(status & (1<<i))) continue;
		dt[unit].dt_int++;
	}
#endif
	if ( !dmarun ) {
#ifdef	MDP
		dmawakeup++;
#endif
		wakeup( (caddr_t)&dmarun);
	}
}
dmadaemon()
{
	register	unit, s, umask, dhit;
	unit = 0;
	umask = 1;
	while ( 1 ) {
		s = spl7();
		while ( !dmado ) {
			dmarun = 0;
#ifdef MDP
			dmasleep++;
#endif
			sleep( (caddr_t)&dmarun, PINOD );
		}
		dmarun++;
		dhit = 0;
		if ( dmado & umask ) {
			dhit++;
			dmado &= ~umask;
		}
		splx(s);
		if ( dhit )
			xdma(unit);
		umask <<= 1;
		if ( ++unit >= 8 ) {
			unit = 0;
			umask =1;
		}
	}
}
xdma( unit )
register unit;
{
	register s;
	register struct	tty *tp;


#ifdef	MDP
	dt[unit].dt_complete++;
#endif
	tp = &ttytab[unit];


	/* tp->t_numo is > 0 if cblock data transfer */

	if (tp->t_numo) {
		/* adjust start index in cblock. Note that cblock may
		   may not yet be exhausted (embedded delay flag) */
		tp->t_buf->c_first += tp->t_numo;
	}
	/* send a start character; we've cleared the backlog */
	if (tp->t_state&TTXON) {
		/* assertion: BUSY set because of previous activity */
		tp->t_state &= ~TTXON;
		/* # char =0. Not cblock transfer */
		tp->t_numo = 0;
		s = spl7();
		startdma(unit,&sio_xon,1);
		splx(s);
		return;
	}

	/* send a stop char; we're choking */
	if (tp->t_state&TTXOFF) {
		/* assertion: BUSY set because of previous activity */
		tp->t_state &= ~TTXOFF;
		/* # char =0. Not cblock transfer */
		tp->t_numo = 0;
		s = spl7();
		startdma(unit,&sio_xoff,1);
		splx(s);
		return;
	}
	/* This cblock dma transfer done. Output next cblock */
	tp->t_state &= ~BUSY;
	s = spl7();
	sioproc(tp, T_OUTPUT);
	splx(s);
}
#else

	/*---------------------------------*\
	| End of DMA transmission interrupt |
	\*---------------------------------*/

dma(r0, r1, r2, r3, r4, r5, r6, r7, id, fcw, pc)
{
	register unit;
	register i;
	register status;
	register struct	tty *tp;
#ifdef VPMSYS
	register struct cblock *cp;
	int j;
#endif
#ifdef TDEBUG
	register struct help *ptdata;
#endif

#ifdef COUNT
	profile[14]++;
#endif

	/* output interrupt acknowledgement (Intel 8237 doesn't send one) */

	putreti();

	/* is it the Paralled port that finished? */

	if (id == IDDMA0END) {
#ifndef VPMSYS
		status = ( in_local(pdmac[8] | RSR ) & 0xff );
		if ( (status >> 2) & 1 ) {
			pio.pb_busy = 0;
			wakeup( (caddr_t)&pio );
		}
#endif
		return;
	} else if (id == IDDMA1END) {
		unit = 0;
	} else {
		unit = 4;
	}

	/* get the status of the interrpting dma */

	status = (in_local(pdmac[unit] | RSR) & 0xff);

	/* for each channel of the device, handle the interrupt */

	for (i = 0; i < 4; i++, unit++) {

		/* determine if this channel finished with dma */

		if (!(status & (1<<i))) continue;

		/* This channel has completed a dma transfer */

		tp = &ttytab[unit];

#ifdef TDEBUG
	ptdata = &tdata[tdcnt];
	if(++tdcnt >= 100) tdcnt = 0;
	ptdata->portid = i;
	ptdata->numsent = tp->t_numo;
	ptdata->ttystate = tp->t_state;
	ptdata->curcblk = tp->t_buf;
	ptdata->nxtcblk = tp->t_outq->c_next;
	ptdata->firstsnt = tp->t_buf->c_first;
	ptdata->lastsnt = tp->t_buf->c_last;
#endif

		/* tp->t_numo is > 0 if cblock data transfer */

		if (tp->t_numo) {
			/* adjust start index in cblock. Note that cblock may
			   may not yet be exhausted (embedded delay flag) */
#ifdef VPMSYS
			tp->v_devbuf->c_first += tp->t_numo;
#else
			tp->t_buf->c_first += tp->t_numo;
#endif
		}
#ifndef VPMSYS
		/* send a start character; we've cleared the backlog */
		if (tp->t_state&TTXON) {
			/* assertion: BUSY set because of previous activity */
			tp->t_state &= ~TTXON;
			/* # char =0. Not cblock transfer */
			tp->t_numo = 0;
			startdma(unit,&sio_xon,1);
			continue;
		}

		/* send a stop char; we're choking */
		if (tp->t_state&TTXOFF) {
			/* assertion: BUSY set because of previous activity */
			tp->t_state &= ~TTXOFF;
			/* # char =0. Not cblock transfer */
			tp->t_numo = 0;
			startdma(unit,&sio_xoff,1);
			continue;
		}
#endif
		/* This cblock dma transfer done. Output next cblock */
		tp->t_state &= ~BUSY;
		sioproc(tp, T_OUTPUT);
#ifdef VPMSYS
#ifndef ASYNC
		if((tp->t_state & BUSY) == 0) {

			for (j=0; j<500; j++) {
			}

			while ((cp = getcb(&tp->v_rcvholdq)) != NULL)
				putcf(cp);

			while ((cp = getcb(&tp->t_rawq)) != NULL)
				putcb(cp);

			/* ******************************************** */
			/*						*/
			/*	  SYNCHRONOUS TRANSMISSION ONLY		*/
			/*						*/
			/* Request To Send (RTS) is taken down here.    */
			/* It is assumed that there are 'mark' chars 	*/
			/* following the message so the taking down of  */
			/* RTS does not cause the loss of any message   */
			/* chars.                                       */
			/*						*/
			/* ******************************************** */

			out_local(psioc[unit],NULLCMD | WR5);
			out_local(psioc[unit],wr5sav[unit]=
				(wr5sav[unit] & ~(TEN|RTS) | TBITS8));
		}
#endif
#endif
	}
}
#endif
