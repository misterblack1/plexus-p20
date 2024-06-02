#include "sys/param.h"
#include "sys/dir.h"
#include "sys/user.h"
#include "sys/tty.h"
#include "sys/file.h"
#include "icp/sioc.h"	/* icp specific */
#include "icp/siocunix.h"	/* icp specific */
#include "icp/proc.h"	/* icp specific */
#include "icp/sioccomm.h"	/* icp specific */
#include "icp/icp.h"	/* icp specific */
#include "icp/dma.h"	/* icp specific */
#include "icp/ctc.h"	/* icp specific */
#ifndef VPMSYS
#include "icp/pio.h"	/* icp specific */
#endif
#include "icp/sio.h"	/* icp specific */
#include "icp/icpinfo.h" /* icp specific */
#ifdef COUNT
extern int profile[];
#endif
#ifdef WAITING
extern int profile[];
#endif

extern char *bcopy();

#ifdef VPMSYS
extern int proto0stksize;
extern int proto1stksize;
extern int proto2stksize;
extern int proto3stksize;
extern int devicearray[];

#define NUMLINES  5        /* there is a max of 4 lines for VPM */

int	  rr0sav[NUMLINES];	/* sioext status saved for finding changes */
struct tty ttytab[NUMLINES];	/* tty control tables. One per line. */
int	wr5sav[NUMLINES];		/* save area for sio write reg #5 */
unsigned char rxmask[NUMLINES] = 0;	/* mask to remove parity bits and other
					   garbage added by sio chip */
#endif

#ifdef	MDP
#define	RTRCNT	100
	struct  {
		unsigned rt_in;
		char	rt_c[RTRCNT];
	} rt;
	unsigned	xrt;
#endif
#ifndef VPMSYS
extern	struct	pios pio;
#ifdef VPR
char piobuf[1024];		/* added 10/24/82 for faster parallel io */
#endif
int	  rr0sav[NUMSIO];	/* sioext status saved for finding changes */
struct tty ttytab[NUMSIO];	/* tty control tables. One per line. */
int	wr5sav[NUMSIO];		/* save area for sio write reg #5 */
char sio_xon = CSTART;
char sio_xoff = CSTOP;
int needcblock;			/* cfreelist exhausted; wait for free cblock */
unsigned char rxmask[NUMSIO];	/* mask to remove parity bits and other
					   garbage added by sio chip */

#define PIOUNIT	8		/* unit used for pio */
#define	BITS6	01
#define	BITS7	02
#define	TWOSB	04
#define	PENABLE	020
#define	OPAR	040

#define	IENAB	030100
#define	XINT	0100000
#define	CLRNEM	0400


#define	DONE	0200
#define	SCENABL	040
#define	BSY	020
#endif
#ifdef	MDP
long	rfint;
#endif

#define	TURNON	07	/* RTS, DTR, line enable */
#define	TURNOFF	1	/* line enable only */

#ifndef VPMSYS
	/* default sio control characters */
	/* NCC defined in tty.h */
unsigned char ttcchar[NCC] = {
		CINTR,
		CQUIT,
		CERASE,
		CKILL,
		CEOF,
		0,
		0,
		0
};
#endif
	/* speed constants for CTC and Mutiplier for SIO */

struct {
	int	timeconst;
	int	basewr4;
} speedtab[16] = {
	0, 0,		/* hangup */
	192, CLK64X,	/* 50 baud */
	128, CLK64X,	/* 75 baud */
	175, CLK32X,	/* 110 baud */
	143, CLK32X,	/* 134.5 baud */
	128, CLK32X,	/* 150 baud */
	96, CLK32X,	/* 200 baud */
	64, CLK32X,	/* 300 baud */
	32, CLK32X,	/* 600 baud */
	16, CLK32X,	/* 1200 baud */
	21, CLK16X,	/* 1800 baud */
	16, CLK16X,	/* 2400 baud */
	8, CLK16X,	/* 4800 baud */
	4, CLK16X,	/* 9600 baud */
	2, CLK16X,	/* 19200 baud */
	1, CLK16X,	/* 34800 baud */
};

	/*-----------------------------*\
 	| tty low and high water marks	|
 	| high < TTYHOG			|
 	\*-----------------------------*/

int	tthiwat[16] = {
	0, 60, 60, 60,
	60, 60, 60, 120,
#ifdef	VPMSYS
	120, 180, 180, 240,
	240, 512, 512, 512,
#else
	120, 191, 191, 191,
	191, 191, 191, 191,
#endif
};
int	ttlowat[16] = {
	0, 20, 20, 20,
	20, 20, 20, 40,
#ifdef	VPMSYS
	40, 60, 60, 80,
	80, 80, 80, 80,
#else
	40, 60, 60, 128,
	128, 128, 128, 128,
#endif
};
#ifdef VPMSYS
int thisicp;              /* our icp# (0 - 3) */
#endif
	/*------------------------------------------------------*\
	| Open a line. Also wakeup with the modem if appropriate |
	\*------------------------------------------------------*/

sioopen()
{
	register struct tty *tp;	/* pointer to status for line */
	register short unit;		/* unit number being opened */
	register struct openlcb *lcb;
	struct openrtn rtn;

#ifdef COUNT
	profile[0]++;
#endif
		/* get pointer to command data */
	lcb = (struct openlcb *) curproc->p_stk;
		/* set up default ACK */
	rtn.ro_gate = 0;
	rtn.ro_error = NULL;
#ifdef VPMSYS
		/* get vpm index */
	unit = TDEV(lcb->lo_dev);
#else
		/* get tty index */
	unit = minor(lcb->lo_dev);
#endif
		/* set the procp & pid in this process's pcb */
	setproc( lcb->lo_procp, lcb->lo_pid );

#ifdef VPMSYS
/*
	printf("SIOPEN:unit=%x\n", lcb->lo_dev);
*/
#endif
		/* extb default speed */

	defspeed = lcb->lo_spdidx;

		/* if line # out of range, then error ACK to mb */
#ifdef VPMSYS
	if (unit >= NUMLINES) {
#else
	if (unit >= NUMSIO) {
#endif
		rtn.ro_error = ENXIO;
		goto out;
	}
	tp = &ttytab[unit];

#ifdef VPMSYS
        devicearray[unit] = BDEV(lcb->lo_dev);
	thisicp = IDEV(lcb->lo_dev);

	tp->pc = 0;

	tp->stacktop = 0;
	if (devicearray[unit] == 0) tp->stacktop = proto0stksize;
	if (devicearray[unit] == 1) tp->stacktop = proto1stksize;
	if (devicearray[unit] == 2) tp->stacktop = proto2stksize;
	if (devicearray[unit] == 3) tp->stacktop = proto3stksize;
	if (tp->stacktop) {
		tp->stacknext = tp->stacktop;
	}
#endif
		/* first open of line? */

	if ((tp->t_state&(ISOPEN|WOPEN)) == 0) {
		/* set default line parameters */
#ifdef VPMSYS
		tp->p_xeomstate = 0;
#ifndef ASYNC
		        /* bit 0 = 1 means sync, otherwise async */
		        tp->t_lflag = 1;
#else
			tp->t_lflag = 0;
#endif
#endif
#ifdef VPMSYS
		if(tp->t_lflag & SYNC)
		     /* set the sync char */
			tp->synchar = 016;
#endif
		tp->t_line = 0;
		tp->t_iflag = 0;
		tp->t_oflag = 0;
		tp->t_cflag = SSPEED|CS8|CREAD|HUPCL;
#ifndef VPMSYS
		tp->t_lflag = 0;
		bcopy(ttcchar, tp->t_cc, NCC);	/* default control char */
#endif
		sioparam(unit);
	}

	spl4();
	if (tp->t_cflag&CLOCAL || siomctl(unit, TURNON)&DCD)
		tp->t_state |= CARR_ON;
		/* If open-without-delay not set, then wait for modem */

	if (!(lcb->lo_flag&FNDELAY))
		while ((tp->t_state&CARR_ON)==0) {
			tp->t_state |= WOPEN;
#ifdef WAITING
			profile[0]++;
#endif
			sleep(&tp->t_canq, TTIPRI);
		}

	/*
	 * establishes a process group for distribution
	 * of quits and interrupts from the tty.
	 */

	if ((lcb->lo_pid   == lcb->lo_pgrp)
	    && (lcb->lo_ttyp   == NULL)
	    && (tp->t_pgrp == 0) ) {
		tp->t_pgrp = lcb->lo_pgrp;
		rtn.ro_pgrp = tp->t_pgrp;
	}

	tp->t_state &= ~WOPEN;
	tp->t_state |= ISOPEN;

	spl0();

		/* return status to main processor & any necessary data */

	out:
	mvtomb(lcb->lo_rtnadx, &rtn, sizeof(rtn));
#ifdef SEQ
	mwakeup(lcb->lo_wuadx,0);	/* reflect sequence # to host */
#else
	mwakeup(lcb->lo_wuadx);
#endif
	exit();
}


	/*--------------------------------------------------*\
	| Close a line. Disconnect from modem if appropriate |
	\*--------------------------------------------------*/

sioclose()
{
	register struct tty	*tp;
	register short unit;
	register struct closelcb *lcb;
	struct closertn 	rtn;
#ifdef VPMSYS
	char clsarray[4];
#endif

#ifdef COUNT
	profile[1]++;
#endif

		/* get arguments off of stack */

	lcb = (struct closelcb *) curproc->p_stk;
	rtn.rc_gate = 0;
	rtn.rc_error = NULL;
#ifdef VPMSYS
	unit = TDEV(lcb->lc_dev);
#else
	unit = minor(lcb->lc_dev);
#endif
	setproc( lcb->lc_procp, lcb->lc_pid );

#ifdef VPMSYS
/*
	printf("SIOCLOSE:unit=%x\n", lcb->lc_dev);
*/
#endif
	tp = &ttytab[unit];

		/* on last close, clear tty process group */

	tp->t_pgrp = 0;

		/* startup output if line was suspended */
#ifndef VPMSYS
#ifdef	MDP
	rt.rt_c[rt.rt_in] = 0xf2;
	if ( ++rt.rt_in >= RTRCNT ) 
		rt.rt_in = 0;
#endif
	spl4();
	sioproc(tp, T_RESUME);
	spl0();
#endif
	ttyflush(tp, FREAD);
	ttywait(tp);

#ifdef VPMSYS
   clsagain:
	clsarray[0] = 0;
	clsarray[1] = 0;
	clsarray[2] = 0;
	clsarray[3] = 0;
	if(mreport(VPMRTNX, lcb->lc_dev, clsarray, MRTNXL)) {
	     mrptslp();
	     goto clsagain;
        }

	if(lcb->lc_vpmhalt) {
		devicearray[unit] = 4;
		if(tp->v_timerid) {
			cantimer(tp->v_timerid);
			tp->v_timerid = 0;
		}
	}
	/*  clear some flags for VPM */
	tp->t_buf = 0;
	tp->v_cfirst = 0;
#endif
	tp->t_state &= ~(ISOPEN|WOPEN);


	if (tp->t_cflag&HUPCL) {
		spl4();
		siomctl(unit, TURNOFF);
		spl0();
	}

	mvtomb(lcb->lc_rtnadx, &rtn, sizeof(rtn));
#ifdef SEQ
	mwakeup(lcb->lc_wuadx,0);	/* reflect sequence # to host */
#else
	mwakeup(lcb->lc_wuadx);
#endif
	exit();
}

	/*----------------------------------------------------*\
	| Read a buffer of data into user space (#char <= 128) |
	\*----------------------------------------------------*/

#ifdef VPMSYS
	char	buf[VRBUFSIZE];
	int	onlyone;
#endif
sioread()
{
	register n,count;
	register struct cblock *cp;
	register char	*bufp;
	int flag;
/*	register c; */
	register struct readlcb *lcb;
	struct readrtn rtn;
#ifndef VPMSYS
	char	buf[RBUFSIZE];
#endif
	register struct clist *tq;
	register struct tty *tp;
	int unit;
	struct cblock *getcb();

#ifdef COUNT
	profile[2]++;
#endif

		/* get pointer to command data */
	lcb = (struct readlcb *) curproc->p_stk;

#ifdef VPMSYS
/*
        printf("SIOREAD:unit=%x\n", lcb->lr_dev);
*/
#endif

#ifdef VPMSYS
		/* get vpm index */
	unit = TDEV(lcb->lr_dev);
#else
		/* get tty index */
	unit = minor(lcb->lr_dev);
#endif
		/* set the procp & pid in this process's pcb */
	setproc( lcb->lr_procp, lcb->lr_pid );
		/* get flag that tells us if this is first iteration */
	flag = lcb->lr_flag;
		/* set up default ACK */
	rtn.rr_gate = 0;
	rtn.rr_error = EFBIG;	/* default return status: more data to come */
	rtn.rr_count = 0;

	tp = &ttytab[unit];

		/* check if cononical empty */

	tq = &tp->t_canq;
		/*flag is artifact of multiple requests instead of one */
	if (tq->c_cc == 0 && flag)
		if (lcb->lr_fmode&FNDELAY) {
#ifndef VPMSYS
			if((tp->t_rawq).c_cc)
				canon(tp);
			else
#endif
			/* icp return status: Q empty */
			/* rtn.rr_error = EIO; */
				goto out;
		}
#ifndef VPMSYS
		else
			canon(tp);
#endif
#ifdef VPMSYS
		else {

			tp->v_rbfname[0] = lcb->lr_rbufin[0];
			tp->v_rbfname[1] = lcb->lr_rbufin[1];
			tp->v_rbfname[2] = lcb->lr_rbufin[2];

		/*  This is where the virtual protocol script  */
		/*  enters the picture.  The script actually   */
		/*  builds the canonical queue.  A sleep on    */
		/*  canonical queue is done here instead       */
		/*  of a call to "canon".                      */

#ifdef WAITING
			profile[2]++;
#endif
			sleep ((caddr_t)&tp->t_canq, TTIPRI);

			onlyone = 1;
		}
#endif
	bufp = buf;
#ifdef VPMSYS
	for(count=VRBUFSIZE;count; count -= n) {
#else
	for(count=RBUFSIZE;count; count -= n) {
#endif
		/* Will be be able to exhaust a cblock */
		if (count >= CLSIZE) {
			/* if no more cblocks, then finished */
			if ((cp=getcb(tq)) == NULL) {
				rtn.rr_error = NULL; /* no more data */
				break;
			}
			/* get number of chars in cblock */
			n = min(count, cp->c_last - cp->c_first);
			/* copy cblock worth of data in temp buffer */
			bcopy(&cp->c_data[cp->c_first], bufp, n);
			/* cblock empty; return it to the free list */
			putcf(cp);
#ifndef VPMSYS
			if(needcblock) {
				needcblock = 0;
				wakeup( (caddr_t) &needcblock);
			}
#endif
			/* adjust buffer ptr for next set of data */
			/* second bcopy so it returns updated second arg */
			bufp += n;
		} else {	/* we want only partial cblock */
			/* if no more chars, then finished */
			if ((n = getc(tq)) < 0) {
				rtn.rr_error = NULL; /* no more data */
				break;
			}
			*bufp++ = n; /* changed c to n */
			n = 1;
		}
	}
#ifndef VPMSYS
		/* 
		 * if we were blocked because of too much input, 
		 * restart the input if we're below the low water mark.
		 */

	if (tp->t_state&TBLOCK) {
		if (tp->t_rawq.c_cc<TTXOLO) {
			sioproc(tp, T_UNBLOCK);
		}
	}
#endif
		/* return block of data to main processor */
	rtn.rr_count = count = bufp - buf;
		/* # of char made even since we will use word transfer */
	mvtomb(lcb->lr_bufadx, buf, (count+1) & ~01 );
#ifdef	MDP
	for ( bufp = buf, xrt = count; xrt; xrt-- ) {
		rt.rt_c[rt.rt_in] = *bufp++;
		if ( ++rt.rt_in >= RTRCNT )
			rt.rt_in = 0;
	}
	rt.rt_c[rt.rt_in] = 0xf0;
	if ( ++rt.rt_in >= RTRCNT )
		rt.rt_in = 0;
#endif

out:
		/* send ACK back to main processor */
	mvtomb(lcb->lr_rtnadx, &rtn, sizeof(rtn));
#ifdef SEQ
	mwakeup(lcb->lr_wuadx,0);	/* reflect sequence # to host */
#else
	mwakeup(lcb->lr_wuadx);
#endif
/*
rmreport:
	if(mreport(VPMRTNR, lcb->lr_dev, tp->v_rbfname[0], MRTNRL)) {
		mrptslp();
		goto rmreport;
	}
*/
/*
	printf("SIOREAD:mwakeup");
*/
#ifdef VPMSYS
	onlyone = 0;
#endif
	exit();
}


	/*----------------------------------------------------*\
	| write a block of data from the main processor to the |
	| designated channel (PIO VPM or TTY).		       |
	\*----------------------------------------------------*/

#ifdef VPMSYS
long tcount;
extern int vpmflag;
#endif

siowrite()
{
	register struct tty	*tp;
	int unit;
	register struct writelcb *lcb;
	register count;
	struct writertn rtn;
	int	wcnt;
	char	*buf;
	register char *bufp;
	register int	n;
	int	highwater;
#ifndef VPMSYS
	register struct cblock *cp;
	struct cblock *getcf();
#endif
#ifdef VPMSYS
	struct cblock *cp,*getcf();
#endif
	char *wcopy(),*smalloc();

#ifdef COUNT
	profile[3]++;
#endif

		/* get pointer to command data */
	lcb = (struct writelcb *) curproc->p_stk;
		/* set up default ACK */
	rtn.rw_gate = 0;
	rtn.rw_error = NULL;

#ifdef VPMSYS
/*
	printf("SIOWRITE:unit=%x\n", lcb->lw_dev);
*/
#endif

#ifdef VPMSYS
		/* get vpm index */
	unit = TDEV(lcb->lw_dev);
#else
		/* get tty index */
	unit = minor(lcb->lw_dev);
#endif
		/* set the procp & pid in this process's pcb */
	setproc( lcb->lw_procp, lcb->lw_pid );

		/* is this a write to the PIO channel?? */
#ifndef VPMSYS
	if ( unit == PIOUNIT ) {
#ifdef VPR
		/* check if parallel unit is busy, if so wait for free buffer*/
		spl4();
		if(pio.pb_busy) {
			sleep( (caddr_t)&pio, SIOCPRI );
		}
		/* mark parallel port busy */
		pio.pb_busy++;
		spl0();
		/* round up # char to even value. transfer done in words */
		wcnt = (lcb->lw_count + 1) & ~01;
		/* move the data from the main processor int this icp */
		mvfrommb( piobuf, lcb->lw_bufadx, wcnt );
		/* start the pio and sleep until output complete */
		startdma(10, piobuf, lcb->lw_count);
		/* send ack message back to main processor */
		mvtomb( lcb->lw_rtnadx, &rtn, sizeof( rtn ) );
#ifdef SEQ
		mwakeup( lcb->lw_wuadx ,0);	/* reflect sequence # to host*/
#else
		mwakeup( lcb->lw_wuadx );
#endif
		goto swexit;
#else	/* else for VPR */
		/* make sure that # chars to send is legal */
		if ( lcb->lw_count > WBUFSIZE ) {
			rtn.rw_error = ENOMEM;
			goto out;
		}
		/* round up # char to even value. transfer done in words */
		wcnt = (lcb->lw_count + 1) & ~01;
		/* mark parallel port busy */
		pio.pb_busy++;
		/* get a buffer to hold the data */
		buf = smalloc();
		/*buf = (char *)smalloc( WBUFSIZE );*/
		/* set buffer address/size in current proc's control block */
		/*setbufp( buf, WBUFSIZE );*/
		setbufp( buf );
		/* move the data from the main processor int this icp */
		mvfrommb( buf, lcb->lw_bufadx, wcnt );
		/* set buf address & length for parallel port controller */
		pio.pb_buf = ( char *) buf;
		pio.pb_len = lcb->lw_count;

		/* race condition pio signal before we get to sleep */
		spl4();
		/* start the pio and sleep until output complete */
		piostart();
		while ( pio.pb_busy ) {
			/* signal from pio handler */
#ifdef WAITING
			profile[1]++;
#endif
			sleep( (caddr_t)&pio, SIOCPRI );
		}
		spl0();

		/* send ack message back to main processor */
		mvtomb( lcb->lw_rtnadx, &rtn, sizeof( rtn ) );
#ifdef SEQ
		mwakeup( lcb->lw_wuadx ,0);	/* reflect sequence # to host */
#else
		mwakeup( lcb->lw_wuadx );
#endif
		/* return the buffer back to the pool */
		smfree( buf );
		/*smfree( WBUFSIZE, buf );*/
		/* clear buffer address/size in current proc's control block */
		clrbufp();
		goto swexit;
#endif	/* end of VPR ifdef */
		/* start the pio and sleep until output complete */
	}
#endif
		/* This is a TTY port */

		/* get pointer to tty struct for this line */
	tp = &ttytab[unit];
		/* if the CARRIER is not on then do nothing and return */
	if ((tp->t_state&CARR_ON)==0)
		goto out;
		/* make sure that # chars to send is legal */
#ifdef VPMSYS
	if (lcb->lw_count > VWBUFSIZE) {
#else
	if (lcb->lw_count > WBUFSIZE) {
#endif
		rtn.rw_error = ENOMEM;
		goto out;
	}
		/* round up # char to even value. transfer done in words */
	wcnt = (lcb->lw_count + 1) & ~01;
		/* get a buffer to hold the data */
	buf = smalloc();
		/* set buffer address/size in current proc's control block */
	setbufp( buf );
		/* move the data from the main processor int this icp */
	mvfrommb( buf, lcb->lw_bufadx, wcnt );

		/* move the data into the output Q */

	highwater = tthiwat[tp->t_cflag&CBAUD];

	bufp = buf;
#ifdef VPMSYS
	if(lcb->lw_count > 4) {
	     tp->v_xbfname[0] = *bufp++;
	     tp->v_xbfname[1] = *bufp++;
	     tp->v_xbfname[2] = *bufp++;
	     bufp++;
	     lcb->lw_count -= 4;
        }
/*
        printf("write: buf+4=%x\n", bufp);
*/
	tcount += lcb->lw_count;

        tp->v_xbufin = 1;
#endif
	for(count=lcb->lw_count;count; count -= n) {

		/*
		 * if the output Q is overflowing, wait until more
		 * space is free
		 */
		spl4();
		while (tp->t_outq.c_cc > highwater) {
#ifndef VPMSYS
			sioproc(tp, T_OUTPUT);
#endif
			tp->t_state |= OASLP;
#ifdef WAITING
			profile[2]++;
#endif
			sleep((caddr_t)&tp->t_outq, TTOPRI);
		}
		spl0();

		/* if count small then use putc to conserve cblocks */
		if(count<CLSIZE/4) {
			n = 1;
#ifndef VPMSYS
			if(tp->t_oflag & OPOST) {
				/* let sioproc add CR for ONLCR only case */
				if(tp->t_oflag == (OPOST|ONLCR)) {
					putc(*bufp++, &tp->t_outq);
					outch ++;
				}
				else
					ttxput(tp, *bufp++, 0);
			}
			else {
#endif
				putc(*bufp++, &tp->t_outq);
				outch ++;
#ifndef VPMSYS
			}
#endif
		}
		else {
#ifndef VPMSYS
			/* get free block. */
			/* Null case should be handled better */
			spl4();
			while((cp=getcf()) == NULL) {
				needcblock++;
				sleep(&needcblock,TTOPRI);
			}
			spl0();
#endif
#ifdef VPMSYS
			if ((cp=getcf()) == NULL) {

			     sleep((caddr_t)&cfreelist.c_next, TTOPRI);
			}
#endif
			/* move as much as we can into the cblock */
			n = (count >= CLSIZE)? CLSIZE: count;
			bufp = wcopy(bufp, cp->c_data, n);
			/* fix cblock to show how much data is in it */
			cp->c_last = n;
#ifndef VPMSYS
			if(tp->t_oflag & OPOST) {
				/* let sioproc add CR for ONLCR only case */
				if(tp->t_oflag == (OPOST|ONLCR)) {
					putcb(cp, &tp->t_outq);
					outch += n;
				}
				else
					/* Note that ttxput will return cblock
					   to free list */
					ttxput(tp, cp, n);
			}
			/*
			 * if no post processing, place cblock directly
			 * the output Q.
			 */
			else {
#endif
				putcb(cp, &tp->t_outq);
				outch += n;
#ifndef VPMSYS
			}
			/* start the output on this line */
			spl4();
			sioproc(tp, T_OUTPUT);
			spl0();
#endif
		}
	}
#ifndef VPMSYS
	/* start the output on this line */
	spl4();
	sioproc(tp, T_OUTPUT);
	spl0();
#endif
	/* free the buffer holding the input info back to free space */

	smfree( buf );
	/* clear buffer info from proc control block */
	clrbufp();

out:
#ifdef SEQ
	rtn.rw_seq = lcb->lw_seq; /* reflect seq # to host */
#endif
	/* send ACK back to main processor */
	mvtomb(lcb->lw_rtnadx, &rtn, sizeof(rtn));
	/* Two tyes of write: mutiple full buffers, or partially full buffer */
	if ( lcb->lw_ctlu ) 
		/* partially full case */
#ifdef SEQ
		mflush(lcb->lw_ctlu, lcb->lw_seq); /* reflect seq # to host */
#else
		mflush(lcb->lw_ctlu);
#endif
	else
		/* completely full buffer case */
#ifdef SEQ
		mwakeup(lcb->lw_wuadx, lcb->lw_seq); /* reflect seq # to host */
#else
		mwakeup(lcb->lw_wuadx);
#endif

#ifdef VPMSYS
	tp->v_xbufin = 0;
        wakeup((caddr_t) &vpmflag);
#endif
	swexit:
	exit();
}


	/*----------------------------------*\
	| Alter the line attributes and mode |
	\*----------------------------------*/

sioioctl()
{
	register short unit;
	register struct ioctllcb *lcb;
	struct ioctlrtn rtn;

#ifdef COUNT
	profile[4]++;
#endif

		/* get pointer to command data */
	lcb = (struct ioctllcb *) curproc->p_stk;
		/* set up default ACK */
	rtn.ri_gate = 0;
	rtn.ri_error = NULL;
#ifdef VPMSYS
		/* get vpm index */
	unit = TDEV(lcb->li_dev);
#else
		/* get tty index */
	unit = minor(lcb->li_dev);
#endif
		/* set the procp & pid in this process's pcb */
	setproc( lcb->li_procp, lcb->li_pid );
#ifndef VPMSYS
#ifdef	MDP
	rt.rt_c[rt.rt_in] = 0xf1;
	if ( ++rt.rt_in >= RTRCNT ) 
		rt.rt_in = 0;
	rt.rt_c[rt.rt_in] = ((lcb->li_com >> 8) & 0xff);
	if ( ++rt.rt_in >= RTRCNT ) 
		rt.rt_in = 0;
	rt.rt_c[rt.rt_in] = (lcb->li_com & 0xff);
	if ( ++rt.rt_in >= RTRCNT ) 
		rt.rt_in = 0;
#endif
	if (ttiocom(&ttytab[unit], lcb->li_com, lcb->li_arg,
			lcb->li_parmadx, &rtn.ri_error))
#endif
#ifdef VPMSYS
	if (ttiocom(&ttytab[unit], lcb->li_com, lcb->li_arg,
			lcb->li_parmadx, lcb->li_dev, &rtn.ri_error))
#endif
		sioparam(unit);
	/* send ACK back to main processor */
	mvtomb(lcb->li_rtnadx, &rtn, sizeof(rtn));
#ifdef SEQ
	mwakeup(lcb->li_wuadx,0);	/* reflect sequence # to host */
#else
	mwakeup(lcb->li_wuadx);
#endif
	exit();
}


/*--------------------------------------------------------------------*\
| initialize sio and ctc baud clk parameters for character processing. |
\*--------------------------------------------------------------------*/

extern	char	deftab[8];
sioparam(dev)
dev_t	dev;
{
	register struct tty *tp;
	register speed;
	register short unit;
	register savepri;

#ifdef COUNT
	profile[5]++;
#endif
#ifdef VPMSYS
	unit = TDEV(dev);
#else
	unit = minor(dev);
#endif
	tp = &ttytab[unit];
	speed = tp->t_cflag & CBAUD;
	savepri = spl4();

	/* Hang up line request */

	if (speed == 0) {
		if(tp->t_state&CARR_ON) {
/*
 *   Assert break here if needed.
 *   See system 3 dh.c driver.
 */
			siomctl(unit, TURNOFF);
		}
		goto out;
	}

	
#ifdef VPMSYS
	if(tp->t_lflag & SYNC) {
		/* It is synchronous transmission */
		/* Setup Write register 3 */

		out_local(psioc[unit], NULLCMD | WR3);
		out_local(psioc[unit], RBITS8|HUNT|AUTOEN);

		/* set sync mode                  */
/*
		printf("WR4= %x\n",    SYNC16 );

		printf("SYNCHAR= %x\n", tp->synchar);
*/
		out_local(psioc[unit], NULLCMD | WR4);
		out_local(psioc[unit], SYNC16 );

    		/* set 1st sync char */

     		out_local(psioc[unit], NULLCMD | WR6);
     		out_local(psioc[unit], tp->synchar);
	    
     		/* set 2nd sync char */

     		out_local(psioc[unit], RESETEXT | WR7);
     		out_local(psioc[unit], tp->synchar);
        }
#endif

#ifndef VPMSYS
                /* It is asynchronous tranmission */
	        /* init baud clk for unit */

	        if ( speed == EXTB ) {
		                      speed = deftab[ defspeed];
	        }
	        out_local(pctc[unit], CCTR | CTIME | CRESET | CCTRL);
	        out_local(pctc[unit], speedtab[speed].timeconst);

		/* init sio unit */
	        /* set clock multiplier,# stop bits, parity enable */
	        /* even/odd parity */

	        out_local(psioc[unit], RESETEXT | WR4);
	        out_local(psioc[unit], speedtab[speed].basewr4 |
			               (tp->t_cflag&CSTOPB ? SB2 : SB1) |
			               (tp->t_cflag&PARENB ? PEN : 0) |
			               (tp->t_cflag&PARODD ? 0 : PEVEN));

	        /* enable SIO transmitter, bits/char */

	        out_local(psioc[unit], RESETEXT | WR5);
	        out_local(psioc[unit],wr5sav[unit]=(wr5sav[unit]&
	                           ~(TEN|TBITS6|TBITS7))| TEN |
			       (tp->t_cflag&CS6 ? TBITS6 : 0) |
			       (tp->t_cflag&CS7 ? TBITS7 : 0));

	        /* enable SIO receiver,bits/char */

	        out_local(psioc[unit], RESETEXT | WR3);
	        out_local(psioc[unit], (tp->t_cflag&CREAD  ? REN : 0) | 
			               (tp->t_cflag&CS6 ? RBITS6 : 0) |
			               (tp->t_cflag&CS7 ? RBITS7 : 0));
#endif
	/* construct mask for received characters. (removes all but data) */

	switch(tp->t_cflag&CSIZE) {
	case CS5: rxmask[unit] = 0x1f;
		break;
	case CS6: rxmask[unit] = 0x3f;
		break;
	case CS7: rxmask[unit] = 0x7f;
		break;
	case CS8: rxmask[unit] = 0xff;
		break;
	}


	/* interrupts and spice and everything nice */
#ifdef VPMSYS
	if(tp->t_lflag & SYNC) {
		out_local(psioc[unit], RESETEXT | WR1);
		out_local(psioc[unit], RINTALLNOPE | STATAV |
					EXTINTEN | RDYONXMIT);

		out_local(psioc[unit], NULLCMD | WR5);
		out_local(psioc[unit],wr5sav[unit]=(wr5sav[unit]&
					~(TEN|TBITS8)) | TBITS8);
	}
#endif

#ifndef VPMSYS
	out_local(psioc[unit], RESETEXT | WR1);
	out_local(psioc[unit], RINTALL | STATAV | EXTINTEN |RDYONXMIT);
#endif

	out:
	splx(savepri);
}

#ifdef VPMSYS
        /*-----------------------------------------------*\
	|   skip to the beginning of a new receive frame  |
        \*-----------------------------------------------*/
setrsom(tp)
register struct tty *tp;
{
	int unit;
	register s;

	unit = tp - ttytab;
	s = spl4();
	if(tp->t_lflag & SYNC) {

		/* set 1st sync char */

		out_local(psioc[unit], NULLCMD | WR6);
		out_local(psioc[unit], tp->synchar);

		/* set 2nd sync char */

		out_local(psioc[unit], NULLCMD | WR7);
		out_local(psioc[unit], tp->synchar);

		/* enable SIO receiver, bits/char, hunt mode */

		out_local(psioc[unit], ENABLEINT | WR3);
		out_local(psioc[unit], HUNT | REN | 
						RBITS8 | AUTOEN);

	}
	splx ( s );
}
#endif

	/*-------------------------------------------------------*\
	| break detect flag. 0 means previous character non-NULL, |
	| so nothing needs to be done. None 0 means previous      |
	| character was NULL. If a break had occured, then this   |
	| flag would have been cleared. If its still set, put out |
	| a 0 before processing the next input character.         |
	\*-------------------------------------------------------*/
#ifndef VPMSYS
/*
static  unsigned char breakchar = 0;
*/
	/*------------------------------------------------------*\
	| isnull timeout pending flag. This flag is set whenever |
	| isnull has been evoked via a timer. Until the timeout  |
	| no other copies are scheduled. This is an attempt to   |
	| conserve timeout space/time.				 |
	\*------------------------------------------------------*/
/*
static  unsigned char nullwaiting = 0;
*/
#endif


	/*-----------------------------------------*\
 	| sio receive character available interrupt |
 	\*-----------------------------------------*/

#ifdef	VPMSYS
siorec(r0, r1, r2, r3, r4, r5, r6, r7, id, fcw, pc)
{
	register struct tty *tp;
	register unit;
	register c;
#ifndef VPMSYS
	int isnull();
#endif

#ifdef COUNT
	profile[6]++;
#endif

	unit = (id >> 3) ^ 1;
	tp = &ttytab[unit];
	if (!(tp->t_state&(ISOPEN|WOPEN)))
		goto done;
	rcvint++;
	while ((in_local(psioc[unit])) & RXRDY) {   /* char. present */
		c = (in_local(psiod[unit]) & rxmask[unit]);
		/*
		 * The receive interrupt has priority over the external
		 * status interrupt. This means that a break will produce
		 * both interrupts with the receive interrupt occuring
		 * first. In this case we want to ignore the received
		 * character and rely on the external status interrupt to
		 * alert the icp to the break condition. We do this by
		 * delaying the processing of a NULL character. The
		 * external status interrupt will clear the flag if there
		 * was a break, thus preventing the extra character.
		 * The NULL character is ignored since the external status
		 * interrupt is pending.
		 */
#ifndef VPMSYS
		if(breakchar) {
			breakchar = 0;
			ttin(tp,0,0);
		}
		if(c == 0) {
			breakchar++;
			/*
			 * In the event that this is a real NULL, then there
			 * may be a long delay before the next character.
			 * Hence fire off a watchdog timer to force the NULL
			 * out. Wait up to 2 ticks. The clock has a higher
			 * priority than the SIO so that a 0 or 1 tick may
			 * occur before the sioext interrupt. This means
			 * that there is a delay of 1/50 to 1/25 sec for a
			 * solitary NULL.
			 */
			if(!nullwaiting) {
				nullwaiting++;
				timeout(isnull,tp,2);
			}
			continue;
		}
#endif
#ifdef VPMSYS
		if (tp >= &ttytab[NUMLINES])
			continue;
#endif
#ifdef VPMSYS
		ttin(tp, c);
#else
		ttin(tp, c, 0);
#endif
	}
done:
	out_local(psioc[unit&0xfe],RTNFROMINT);
/*	putreti();*/
}
#else
#ifdef	MDP
#define	RBSIZE	600
#else
#ifdef z8000
#define RBSIZE	2000		/* rawraw q size, must be even */
#endif
#ifdef m68
#define RBSIZE	1800
#endif
#endif
struct	recbuf {
	unsigned rc_cnt;
	unsigned rc_in;
	unsigned rc_out;
	unsigned char rc_rbuf[RBSIZE];
} rb;

unsigned	rbrun = 0;
unsigned	rbover = 0;

siorec(r0, r1, r2, r3, r4, r5, r6, r7, id, fcw, pc)
register id;
{
	register	unit, c;
	register struct tty	*tp;
	int	sioproc();

	unit = ( id >> 3) ^ 1;
#ifdef	COUNT
	profile[6]++;
#endif
	while ((in_local(psioc[unit])) & RXRDY) {
		c = (in_local(psiod[unit]) & rxmask[unit]);
		if ( c == CSTOP ) {
			tp = &ttytab[unit];
			if ( tp->t_iflag & IXON ) {
				if ( !(tp->t_state & TTSTOP) ) {
					tp->t_state |= TTSTOP;
					if ( tp->t_state & BUSY ) {
						out_local(pdmac[unit] | WSMRB,
							SETMSK|MCHAN & unit );
					}
					continue;
				}
			}
		}
		if ( rb.rc_cnt < RBSIZE ) {
			rb.rc_rbuf[rb.rc_in++] = (unsigned char ) unit;
			rb.rc_rbuf[rb.rc_in++] = (unsigned char ) c;
			rb.rc_cnt += 2;
			if ( rb.rc_in >= RBSIZE )
				rb.rc_in = 0;
			if ( !rbrun )
				wakeup( (caddr_t)&rbrun );
		}  else {
			/*
			printf( "read buffer overflow %x, %x\n", c, unit );
			*/
			rbover++;
		} 
	}
#ifdef MDP
	rfint++;
#endif
	out_local(psioc[unit&0xfe],RTNFROMINT);
}

/*
rbdaemon()
{
	register	c, unit, spri;

	while ( 1 ) {
		spri = spl4();
		while ( !rb.rc_cnt ) {
			rbrun = 0;
			sleep( (caddr_t)&rbrun, PINOD );
		}
		rbrun++;
		unit = rb.rc_rbuf[rb.rc_out++];
		c = rb.rc_rbuf[rb.rc_out++];
		rb.rc_cnt -= 2;
		splx(spri);
		if ( rb.rc_out >= RBSIZE )
			rb.rc_out = 0;
		if ( unit != 0xff )
			xsiorec( c, unit );
	}

}
*/
	/*-----------------------------------------*\
 	| sio receive character available interrupt |
 	\*-----------------------------------------*/

xsiorec( c, unit)
register c, unit;
{
	register struct tty *tp;

#ifdef COUNT
	profile[6]++;
#endif

	tp = &ttytab[unit];
	rcvint++;
	ttin(tp, c, 0);
}
#endif
#ifndef VPMSYS
static char *crlf = "\r\n";	/* carriage return line feed */
#endif
#ifndef	VPMSYS
#define	NUMSPE	110
struct	sp	{
	struct	sp	*sp_lnk;
	unsigned char	sp_un;
	unsigned char 	sp_cmd;
} spent[NUMSPE];

struct	sp	*spflist, *spwflist, *spwllist;
unsigned	sprun = 0;
unsigned	spover = 0;

spinit()
{
	register struct sp *spe;

	for ( spe = &spent[0]; spe < &spent[NUMSPE]; spe++ ) {
		spe->sp_lnk = spflist;
		spflist = spe;
	}
}

/*
spdaemon()
{
	register struct sp *spe;
	register	spri;

	while ( 1 ) {
		spri = spl4();
		while ( !spwflist ) {
			sprun = 0;
			sleep( (caddr_t)&sprun, PINOD );
		}
		sprun++;
		spe = spwflist;
		spwflist = spwflist->sp_lnk;
		if ( !spwflist )
			spwllist = NULL;
		splx(spri);
		xsioproc( &ttytab[spe->sp_un], spe->sp_cmd );
		spri = spl4();
		spe->sp_lnk = spflist;
		spflist = spe;
		splx( spri );
	}
}
*/
rbspdaem()
{
	extern	xsiorec(), xsioproc();
	register	c, unit, spri;
	register struct sp *spe;

	while ( 1 ) {
		spri = spl4();
		while ( !rb.rc_cnt && !spwflist ) {
			rbrun = 0;
			sprun = 0;
			sleep( (caddr_t)&rbrun, PINOD );
		}
		rbrun++;
		sprun++;
		if ( spwflist ) {
			spe = spwflist;
			spwflist = spwflist->sp_lnk;
			if ( !spwflist )
				spwllist = NULL;
			splx(spri);
			xsioproc( &ttytab[spe->sp_un], spe->sp_cmd );
			spri = spl4();
			spe->sp_lnk = spflist;
			spflist = spe;
			splx( spri );
		} else if ( rb.rc_cnt ) {
			unit = rb.rc_rbuf[rb.rc_out++];
			c = rb.rc_rbuf[rb.rc_out++];
			rb.rc_cnt -= 2;
			splx(spri);
			if ( rb.rc_out >= RBSIZE )
				rb.rc_out = 0;
			if ( unit != 0xff )
				xsiorec( c, unit );
		}
	}
}

sioproc( tp, cmd )
register struct	tty	*tp;
register char	cmd;
{
	register struct sp	*spe;
	register	spri;
	extern struct tty ttytab[];

	spri = spl4();
	if ( !(spe = spflist) ) {
		spover++;
		return;
	}
	spflist = spe->sp_lnk;
	spe->sp_lnk = NULL;
	spe->sp_un = tp - ttytab;
	spe->sp_cmd = cmd;
	if ( !spwflist ) {
		spwflist = spwllist = spe;
	} else {
		spwllist->sp_lnk = spe;
		spwllist = spe;
	}
	splx(spri);
	if ( !sprun )
		wakeup( (caddr_t)&rbrun );
}
#endif

	/*-------------------------------------------------------*\
	| Perform the requested device action. This is taken from |
	| the dh driver, system 3.				  |
	\*-------------------------------------------------------*/

#ifdef	VPMSYS
sioproc(tp, cmd)
#else
xsioproc(tp, cmd)
#endif
register struct tty *tp;
int cmd;
{
	register n;
#ifndef VPMSYS
	register char *p;
#endif
	/*register int c;*/
	register char	*cp;
	/*register char *colp;*/
	int	s;
	register int	dev;
#ifndef VPMSYS
	extern ttrstrt(), endbreak();
#endif
	struct cblock *getcb();

#ifdef COUNT
	profile[7]++;
#endif

	dev = tp - ttytab;
	switch(cmd) {

#ifndef VPMSYS
	case T_TIME:
		tp->t_state &= ~TIMEOUT;
		goto ostart;

	case T_RESUME:
	case T_WFLUSH:
		/*
		 * If line was busy when ^S was typed, then start up dma. If,
		 * not then dont do anything. startdma() will begin transfer.
		 * TTSTOP is incorrect here because unknown addresses in
		 * dma in that case (what ever was left there by previous dma
		 * transfer. Found this out the hard way.
		 */
		s = spl4();
		if (tp->t_state&BUSY)
			out_local(pdmac[dev] | WSMRB, RESETMSK | MCHAN&dev );
/*
		{
			out_local(psioc[dev], NULLCMD|WR5);
			out_local(psioc[dev], wr5sav[dev]|=TEN);
		}
*/
		tp->t_state &= ~TTSTOP;
		splx( s );
		goto ostart;

	case T_OUTPUT:
	ostart:
		if (tp->t_state&(TIMEOUT|TTSTOP|BUSY))
			break;
		if (tp->t_buf && tp->t_buf->c_first >= tp->t_buf->c_last) {
			putcf(tp->t_buf);
			tp->t_buf = NULL;
			if(needcblock) {
				needcblock == 0;
				wakeup((caddr_t) &needcblock);
			}
		}
		if (tp->t_buf == NULL) {
			if ((tp->t_buf = getcb(&tp->t_outq)) == NULL) {
				if (tp->t_state&TTIOW) {
					tp->t_state &= ~TTIOW;
					wakeup((caddr_t)&tp->t_oflag);
				}
				goto dd;
			}
		}
		/* n is # of char left to process in this cblock */
		n = tp->t_buf->c_last - tp->t_buf->c_first;
		cp = &tp->t_buf->c_data[tp->t_buf->c_first];
		/* if output post processing, check for delay flags */
		if (tp->t_oflag&OPOST) {
	
			/* Only processing OPOST & ONLCR */
			if (tp->t_oflag == (OPOST|ONLCR)) {

				/* search for first new line character */
				p = cp;
				/*colp = &tp->t_col;*/
				do {
					/*  isn't invisable character  */
#ifdef TDEBUG
					c = (*p++) & 0177;
					if(c >= ' ' && c < 0177)
						(*colp)++;*/
					/* advance to next tab location */
					else if(c == '\t') {
						*colp = (*colp + 8) & ~07;
					}
					/* move back a column */
					else if(c == '\b') {
						if(*colp) (*colp)--;
					}
					/* carrage return returns to col 0 */
					else if(c == '\r') {
						*colp = 0;
					}
					/* stop at new line */
					else if(c == '\n') {
						p--;
						break;
					}
#else
					if(((*p++)&0177) == '\n') {
						p--;
						break;
					}
#endif
				} while(--n);
				/* n is either entire block or # char upto but
				   not including new line character. This is so
				   when it comes to providing the delay. The
				   new line is at the beginning of the cblock.
				*/
				n = p - cp;

				if (n == 0) {
					/* move to col 0 */
					/**colp = 0;*/
					/* skip over new line character */
					tp->t_buf->c_first++;
					/* line is busy outputting CR/LF */
					tp->t_state |= BUSY;
					/* don't adjust cblock lenght
					   when done */
					tp->t_numo = 0;
					/* start the characters flowing */
					startdma( dev, crlf, 2);
					goto dd;
				}
			}
			else {
				/* search for first delay flag */
				p = cp;
				do {
					/*
					 * delay is 0200 followed by encoded
					 * time
					 */
					if (((*p++)&0377) == 0200) {
						p--;
						break;
					}
				} while(--n);
				/*
				 * n is either entire block or # char upto but
				 * not including time constant. This is so
				 * when it comes to providing the delay. The
				 * constant is at the beginning of the cblock.
				 */
				n = p - cp;
			}
		}
		if (n == 0) {
			/* delay flag is last char in cblock then use getc to
			  to load next cblock and get the encoded time value */
			if ((tp->t_buf->c_first+1) == tp->t_buf->c_last) {
				n = getc(&tp->t_outq);
				*cp = n;
			} else {
				/* skip over flag and get time value */
				tp->t_buf->c_first++;
				cp++;
				n = *cp & 0377;
			}
			/* if char following flag is really time constant */
			if (n > 0200) {
				/* skip over time constant in cblock */
				tp->t_buf->c_first++;
				/* pause for time interval */
				timeout(ttrstrt, tp, (n&0177)+6);
				tp->t_state |= TIMEOUT;
				goto dd;
			}
			/* if not time constant then just put out this char */
			n = 1;
		}
		tp->t_state |= BUSY;
		/* save number of chars being outputted. This saves us from
		   to interigate the dma and determine this at interrupt
		   complete time */
		tp->t_numo = n;
		startdma(dev, cp, n);
	dd:
		if (tp->t_state&OASLP &&
		    tp->t_outq.c_cc <= ttlowat[tp->t_cflag&CBAUD]) {
			tp->t_state &= ~OASLP;
			wakeup((caddr_t)&tp->t_outq);
		}
		break;

	case T_SUSPEND:
		s = spl4();
		tp->t_state |= TTSTOP;
		if (tp->t_state&BUSY)
			out_local( pdmac[dev] | WSMRB, SETMSK | MCHAN & dev );
/*
		{
			out_local(psioc[dev], NULLCMD|WR5);
			out_local(psioc[dev], wr5sav[dev]&= ~TEN);
		}
*/
		splx( s );
		break;

	case T_BLOCK:
		tp->t_state |= TBLOCK;
		tp->t_state &= ~TTXON;
		/* if channel busy, then defer request until dma done */
		if (tp->t_state&BUSY) {
			tp->t_state |= TTXOFF;
		} else {
			tp->t_state |= BUSY;
			tp->t_numo = 0;
			startdma(dev, &sio_xoff, 1);
		}
		break;

	case T_RFLUSH:
		if (!(tp->t_state&TBLOCK))
			break;

	case T_UNBLOCK:
		tp->t_state &= ~(TTXOFF|TBLOCK);
		/* if channel busy, then defer request until dma done */
		if (tp->t_state&BUSY) {
			tp->t_state |= TTXON;
		} else {
			tp->t_state |= BUSY;
			tp->t_numo = 0;
			startdma(dev, &sio_xon, 1);
		}
		break;

	case T_BREAK:
		/* put out break on the desired channel */
		out_local(psioc[dev],NULLCMD|WR5);
		out_local(psioc[dev],wr5sav[dev] |= SENDBRK);
		tp->t_state |= TIMEOUT;
		/* leave the break asserted for .25 sec */
		timeout(endbreak, tp, HZ/4);
		break;
	}
#endif

#ifdef VPMSYS
	case T_OUTPUT:

	   if (tp->t_state& BUSY)
			break;

	   if (tp->v_devbuf && tp->v_devbuf->c_first >= tp->v_devbuf->c_last) {
			putcf(tp->v_devbuf);
			tp->v_devbuf = NULL;
           }
                if (tp->v_devbuf == NULL) {
			if ((tp->v_devbuf = getcb(&tp->v_devq)) == NULL) {
				if (tp->t_state&TTIOW) {
					 tp->t_state &= ~TTIOW;
					 wakeup((caddr_t)&tp->t_oflag);
                                }
				goto dd;
                        }
                }
		/* n is the # of char left to process in this cblock */

		n = tp->v_devbuf->c_last - tp->v_devbuf->c_first;

		cp = &tp->v_devbuf->c_data[tp->v_devbuf->c_first];

		tp->t_state |= BUSY;

		/* save number of chars being output. This saves us from
		   to interigate the dma and determine this at interrupt
		   complete time */

		tp->t_numo = n;

		if(tp->t_lflag & SYNC) {
			out_local(psioc[dev],NULLCMD | WR5);
			out_local(psioc[dev],wr5sav[dev]=
					(wr5sav[dev] | TEN | RTS | TBITS8));
		}

		startdma(dev, cp, n);

		wakeup ((caddr_t) &vpmflag);

        dd:
		if (tp->t_state&OASLP &&
		    tp->t_outq.c_cc <= ttlowat[tp->t_cflag&CBAUD]) {
			tp->t_state &= ~OASLP;
			wakeup((caddr_t)&tp->t_outq);
                }
	break;

	}
#endif
}


	/*-------------------------------------------------------*\
	| Turn on/off "request to send" and "data terminal ready" | 
	| and return these line parameters			  |
	\*-------------------------------------------------------*/

siomctl(dev, control)
dev_t	dev;
int	control;
{
	register	unit;
#ifdef VPMSYS
	register struct tty *tp;
#endif

#ifdef COUNT
	profile[8]++;
#endif

#ifdef VPMSYS
	unit = TDEV(dev);
	tp = &ttytab[unit];
#else
	unit = minor(dev);
#endif
	switch (control) {

	case TURNON:
		out_local(psioc[unit], NULLCMD | WR5);
#ifdef VPMSYS
		if(tp->t_lflag & SYNC) {
			out_local(psioc[unit], wr5sav[unit] |= DTR);
		}
#else
		out_local(psioc[unit], wr5sav[unit] |= RTS | DTR);
#endif
		break;

	case TURNOFF:
#ifndef VPMSYS
		out_local(psioc[unit], NULLCMD | WR5);
		out_local(psioc[unit], wr5sav[unit] &= ~(RTS | DTR));
#endif
		break;
	}

	/* return current status of modem signals */

	out_local(psioc[unit],RESETEXT);
	return (in_local(psioc[unit]));
}


	/*------------------------------------------*\
	| sio special receive condition interrupt    |
	| Parity error, Framing error, Overrun error |
	\*------------------------------------------*/

#ifndef VPMSYS
unsigned	overrun = 0;
#endif
siospec(r0, r1, r2, r3, r4, r5, r6, r7, id, fcw, pc)
{
	register struct tty *tp;
	register unit;
	register rr1;
	register c;

#ifdef COUNT
	profile[9]++;
#endif

	unit = (id >> 3) ^ 1;
	tp = &ttytab[unit];
	rcvint++;
	out_local(psioc[unit], NULLCMD | RR1);
	rr1 = in_local(psioc[unit]);
	c = in_local(psiod[unit]) & rxmask[unit];
	out_local(psioc[unit], ERRORRESET);
#ifdef VPMSYS
	/* ensure that line is legal */
	if (tp >= &ttytab[NUMLINES])
#else
	/* ensure that line is legal */
	if (tp >= &ttytab[NUMSIO])
#endif
		goto out;
	/* ensure that line is active */
	if (!(tp->t_state&(ISOPEN|WOPEN)))
		goto out;

#ifdef VPMSYS
	if(tp->t_lflag & SYNC)
		/* It is synchronous transmission */
		goto out;
#endif
#ifndef VPMSYS
	/* ensure that only frame, parity, or overrun error are effective */
	if (rr1&(FRAMEERR|PARERR|OVERRUN)) {
		if ( rr1 & OVERRUN )
			overrun++;
		if (rr1&PARERR && !(tp->t_iflag&INPCK))
			rr1 &= ~PARERR;
		if (rr1&(FRAMEERR|PARERR|OVERRUN)) {
			if (tp->t_iflag&IGNPAR)
				goto out;
			if (tp->t_iflag&PARMRK) {
#ifdef VPMSYS
				ttin(tp, 0377);
				ttin(tp, 0);
#else
				ttin(tp, 0377, 1);
				ttin(tp, 0, 1);
#endif
			} else
				c = 0;
			c |= 0400;
		}
#ifdef VPMSYS
		ttin(tp, c);
#else
		ttin(tp, c, 0);
#endif
	}
#endif
out:
#ifdef	MDP
	rfint++;
#endif
	out_local(psioc[unit&0xfe],RTNFROMINT);
/*	putreti();*/
}


	/*-------------------------------------*\
	| sio external/status change interrupt  |
	| break detection and carrier on detect |
	\*-------------------------------------*/

sioext(r0, r1, r2, r3, r4, r5, r6, r7, id, fcw, pc)
{
	register struct tty *tp;
	register short unit;
	register rr0;
	register rr0change;
#ifndef VPMSYS
	register	i;
	extern	siggo();
#endif

#ifdef COUNT
	profile[10]++;
#endif

	unit = (id >> 3) ^ 1;
	tp = &ttytab[unit];
	rcvint++;
	rr0 = (in_local(psioc[unit]) & 0xff);
	rr0change = rr0 ^ rr0sav[unit];	/* 1 for each bit that changed */
	out_local(psioc[unit], RESETEXT);

#ifndef VPMSYS
	if (rr0change&BRK) {
	    if ((rr0 & BRK) == 0) {
	    	in_local(psiod[unit]);	/* throw out null at end */
		/* if there was a break, a null got into the rawraw
		   queue.  Throw out that null by making unit 0xff.
		   It must be the first character in backwards search
		   of rawraw for this unit.
		*/
		for ( i = (rb.rc_in - 2); ; i -= 2 ) {
			if ( i < 0 )
				i = RBSIZE-2;
			if ( rb.rc_rbuf[i] == unit ) {
				rb.rc_rbuf[i] = 0xff;
				break;
			}
		}
	    	/* signal & flush only !IGNBRK & BRKINT */
	    	if( !(tp->t_iflag&IGNBRK) &&  !(tp->t_siglock)) {
	   	    if(tp->t_iflag&BRKINT) {
	    		msignal(tp->t_pgrp, SIGINT, minor( tp->t_dev));
	    		tp->t_siglock++;
	    		timeout( siggo, tp, 25 );
	    		ttyflush(tp, (FREAD|FWRITE));
		    }
		    else {
			if(tp->t_iflag & PARMRK) {
			    ttin(tp, 0377, 1);
			    ttin(tp, 0, 1);
			}
			ttin(tp, 0400, 0);
		    }
		}
	    }
	}
#endif
	if ((rr0change & DCD) && !(tp->t_cflag&CLOCAL)) {
		if (rr0 & DCD) {
		    if ((tp->t_state & CARR_ON) == 0) {
			wakeup((caddr_t) &tp->t_canq);
			tp->t_state |= CARR_ON;
		    }
		}
#ifndef VPMSYS
		else {
		    if (tp->t_state & CARR_ON) {
			if(tp->t_state&ISOPEN) {
			    if ( !(tp->t_siglock) ) {
				msignal(tp->t_pgrp, SIGHUP, 
					minor( tp->t_dev) );
				tp->t_siglock++;
				timeout( siggo, tp, 25 );
				siomctl(unit, TURNOFF);
				ttyflush(tp, FREAD|FWRITE);
			    }
			}
			tp->t_state &= ~CARR_ON;
		    }
		}
#endif
	}

#ifndef VPMSYS
	/*----------------------------------------*\
	| following is not supported by system III |
	| added as action item TSO-004  22/2/82    |
	| CTS is used here as tandem control for   |
	| line control (note: XON/XOFF NOT         |
	| inhibited by use of ICTS).               |
	\*----------------------------------------*/
	if ((rr0change&CTS) && (tp->t_iflag&ICTS)) {
rr0 = (in_local(psioc[unit]) & 0xff);
		/*------------------------------*\
		| TTSTOP   CTS   ICTSLO   ACTION |
		|   1       0      0        --   |
		|   1       0      1      start  |
		|   1       1      0      start  |
                |   1       1      1        --   |
		\*------------------------------*/
		if(tp->t_state & TTSTOP) {
		    if(((rr0&CTS)==CTS) ^ ((tp->t_iflag&ICTSLO)==ICTSLO)) {
			sioproc(tp,T_RESUME);
		    }
		}
		/*------------------------------*\
		| TTSTOP   CTS   ICTSLO   ACTION |
		|   0       0      0       halt  |
		|   0       0      1        --   |
		|   0       1      0        --   |
                |   0       1      1       halt  |
		\*------------------------------*/
		else {
		    if(!(((rr0&CTS)==CTS) ^ ((tp->t_iflag&ICTSLO)==ICTSLO)) ) {
			sioproc(tp,T_SUSPEND);
		    }
		}
	}
#endif
	rr0sav[unit] = rr0;
#ifdef	MDP
	rfint++;
#endif
	out_local(psioc[unit&0xfe],RTNFROMINT);
/*	putreti();*/
}

#ifdef ZADD

	/*----------------------------------*\
	| Power fail cleanup on icp channels |
	\*----------------------------------*/

sioclr()
{
	register short dev;
	register struct tty *tp;

	for (dev = 0; dev < NUMSIO; dev++) {
		tp = &ttytab[dev];
		if ((tp->t_state&(ISOPEN|WOPEN)) == 0)
			continue;
		sioparam(dev);
		siomctl(dev, TURNON);
		tp->t_state &= ~BUSY;
		sioproc(tp, T_OUTPUT);
	}
}
#endif


	/*------------------------------------------------*\
	| clean up icp process when main process is killed |
	\*------------------------------------------------*/

sioclean()
{
	register	struct	cleanlcb *lcb;
/*	register	unit;*/
/*	register	struct	tty	*tp;*/
#ifdef VPMSYS
	char array[4];
#endif

#ifdef COUNT
	profile[11]++;
#endif

	lcb = (struct cleanlcb *) curproc->p_stk;
/*	unit = minor( lcb->clr_dev );*/
/*	tp = &ttytab[unit];*/
	delproc( lcb->clr_procp, lcb->clr_pid );
#ifdef SEQ
	mwakeup( lcb->clr_wuadx ,0);	/* reflect sequence # to host */
#else
	mwakeup( lcb->clr_wuadx );
#endif
#ifdef VPMSYS
   again:
	array[0] = 0;
	array[1] = 0;
	array[2] = 0;
	array[3] = 0;
	if(mreport(VPMRTNX, lcb->clr_dev, array, MRTNXL)) {
	     mrptslp();
	     goto again;
        }
#endif
	exit();
}

	/*------------------------------------------------------------*\
	| initialize the sios interrupt vectors and the parameters for |
	| the console sio.					       |
	\*------------------------------------------------------------*/

sioinit()
{
	register unit;

	/* set up interrupt vectors */

	for (unit = 1; unit < 8; unit += 2) {
		out_local(psioc[unit], NULLCMD | WR2);
		out_local(psioc[unit], (unit - 1) * 8);
		out_local(psioc[unit], NULLCMD | WR1);
		out_local(psioc[unit], STATAV);
	}
}

	/*-----------------------------------------------------------*\
	| This routine prints a character on the console using status |
	| wait instead of interrupts.				      |
	\*-----------------------------------------------------------*/

putchar(c)
register c;
{
	register  timo;

#ifdef COUNT
	profile[12]++;
#endif

	timo = 30000;
	while ((in_local(psioc[CONSOLE]) & TXRDY) == 0) {
		if (--timo == 0) {
			break;
		}
	}
	if (c == 0) {
		return;
	}
	out_local(psiod[CONSOLE], c);
	if (c == '\n') {
		putchar('\r');
	}
	putchar(0);	/* wait for last character to complete */
}
#ifndef VPMSYS

	/*-------------------------------------------------------*\
	| timeout event for solitary NULL encountered on input.   |
	| Nullwaiting is intended to prevent too many outstanding |
	| timeouts. Breakchar is flag that is reset when defered  |
	| NULL character has been processed.			  |
	\*-------------------------------------------------------*/

/*
isnull(unit)
register	unit;
{
	if ( breakchar & (1<<unit) ) {
		breakchar &= ~(1<<unit);
		if ( rb.rc_cnt < RBSIZE ) {
			rb.rc_rbuf[rb.rc_in++] = (unsigned char ) unit;
			rb.rc_rbuf[rb.rc_in++] = (unsigned char ) 0;
			rb.rc_cnt += 2;
			if ( rb.rc_in >= RBSIZE )
				rb.rc_in = 0;
		}
	}
	nullwaiting &= ~(1<<unit);
}
*/
#endif
