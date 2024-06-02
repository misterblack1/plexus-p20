	/*--------------------*\
	| ICP handler software |
	\*--------------------*/


#include "sys/plexus.h"
#include "sys/param.h"
#include "sys/dir.h"
#include "sys/ioctl.h"
#include "sys/proc.h"
#include "sys/tty.h"
#include "sys/ttold.h"
#include "sys/conf.h"
#include "sys/user.h"
#ifdef VPMSYS
#include "sys/vpmd.h"
#endif
#ifdef VPR
#include "sys/vpr.h"
#endif

#include "icp/sioc.h"		/* icp specific */
#include "icp/siocunix.h"	/* icp specific */
#include "icp/siocprom.h"	/* icp specific */
#include "icp/sioccomm.h"	/* icp specific */
#include "icp/icp.h"		/* icp specific */
#include "icp/pbsioc.h"		/* icp specific */

/*
 * constants
 */

#define	CON		0
#define MBSDADX(x)	((((long) MBSDSEG) << 16) | (unsigned) (x))
	/* ICP STATES */
#define NONE		0	/* no state. initializtion value */
#define	DNLD		1	/* downloading icp */
#define	RUN		2	/* icp running */
#define IDLE		3	/* icp waiting to down load */
#define SELFTEST	4	/* icp doing self test (after reset) */
#ifndef	OVKRNL
/* note: anything changed between these #if statments must be changed in
	 iotimo.c also
*/
#define TESTDONE	5	/* self test done. intermediate state */
#endif

#define	DNLDBUFSIZE	128
#define AOPEN	1
#define ACLOSE	2
#define AREAD 	3
#define AWRITE 	4
#define APWRITE	5
#define AIOCTL	6
#define DNLDOPEN	1
#define STDOPEN		2
#define DNLDCLOSE	3
#define STDCLOSE	4
#define DNLDREAD	5
#define STDREAD		6
#define DNLDWRITE	7
#define STDWRITE	8
#define STDPWRITE	9
#define STDIOCTL	10

#define TRUE	1
#define FALSE	0

/*
 * global variables
 */
#ifdef PROFILE
int icpprof[650];	/* icp profile holding area */
#endif

#ifdef VPMSYS
short word1, word2;
#endif

extern  int	diagswits;
extern	int	dbug;
unsigned dnldjmpadx;
char dnldbuf[DNLDBUFSIZE];
extern	char	siocbuf[];
#ifdef SEQ
int numsent;
#endif

/*
 * icp configuration vector
 */

char legal_icp[NUMICP];
/*int  numusr;			/* # of allowed users < numusr*/

/*
 * SIOC control structure.
 */

#define	PBASE0	0xf800
#define PBASE1	0xf900
#define PBASE2 	0xfa00
#define PBASE3	0xfb00
#define PBASE4	0xfc00
extern struct sctl sctl[];

#ifdef VPMSYS
extern vpmtrint();
extern struct vpmbd vpmbd[];
int lastclunit;
#endif

/*
 * open a unit
 */

#ifdef VPMSYS
siopen(index,unit, flag, minounit, vpmoricp)
#else
siopen(index,unit, flag)
#endif
int index;
int unit;
int flag;
#ifdef VPMSYS
int minounit;
int vpmoricp;
#endif
{
	register struct proc *pp;
	register struct openlcb *lcb;
	register struct openrtn *rtn;
	register struct scb *scb;
	int	i;
	long	j;
	register struct sctltp *ctltp;
#ifdef VPMSYS
	register struct vpmb *vp;
#endif
	register struct	sctl	*ctl;
	struct	cmd	*pblk;
#ifndef	OVKRNL
/* note: anything changed between these #if statments must be changed in
	 iotimo.c also
*/
	int	siocfail();
	int	selftestdone();
#else
	extern	int	siocfail();
	extern	int	selftestdone();
#endif
#ifdef VPMSYS
	int 	ec;

	/* In VPM the minor unit no. contains other than */
	/* just the line #                               */

	if (vpmoricp) {
		unit = TDEV(minounit);   /* redo unit using the last 4 bits */
		ctl = &sctl[IDEV(minounit)];
	} else
#endif
		ctl = &sctl[index];

	ctltp = &ctl->tp_un[unit];
	if (unit == DNLDUNIT) {
		spl7();
		/* if unit is idle, then prod it awake. Wait for self test */
		if(ctl->sc_state == IDLE || ctl->sc_state == RUN) {
			out_multibus(ctl->sc_port, RESET);
			ctl->sc_state = SELFTEST;
			timeout(selftestdone, (caddr_t) ctl, 1000);
		}
		/* wait for self test to complete */
		if(ctl->sc_state == SELFTEST) {
			sleep((caddr_t) ctl, SIOCPRI);
		}
		/* if we're already down loading, then device is busy */
		if (ctl->sc_state == DNLD) {
			u.u_error = EBUSY;
			return;
		}
		ctl->sc_state = DNLD;
		spl0();

		dnldjmpadx = 0;
		pblk = (struct cmd *)ctl->sc_paddr;
		pblk->c_gate = 0;
		pblk->c_flags = diagswits & 07;
		ctl->sc_intseq = ctl->sc_toseq = 0x8000;
		setabort( ctl, unit, AOPEN, DNLDOPEN );
		timeout( siocfail, ctl, 50);
		out_multibus( ctl->sc_port, ATTN );
		docmd( pblk );
		spl7();
		if ( pblk->c_gate != 0 ){
			u.u_error = EIO;
			ctl->sc_state = IDLE;
		} else ctl->sc_intseq++;
		rsetabort( ctl, unit, AOPEN );
		spl0();
	} else if (unit == PIOUNIT) {
		/*---------------------------------------------*\
		| if the icp is not running, then it is illegal |
		| to attempt to open the parallel port at this  |
		| time.						|
		\*---------------------------------------------*/
		if ( ctl->sc_state != RUN )
			u.u_error = EBUSY;
		/*
		 * for now we will do nothing until we see what the
		 * driver really looks like.  Write will be allowed
		 * only.
		 */
		 
	} else if ( ctl->sc_state == RUN ) {
#ifdef VPMSYS
		if(vpmoricp) {
		     /* VPM check to see if protocol is o.k. */
		     /* vp = ICP# times 4 plus protocol #    */
		     vp = &vpmb[IDEV(minounit) * 4 + BDEV(minounit)];

		     if(vp->vb_state&VPMERR) {
				u.u_error = EIO;
				vp->vb_state = 0;
				return;
                     }

		     if (ec = vpmstart(minounit, vpmtrint)) {
		          vpmsave('p', minounit, ec, 0);
/*
			printf("SIOPEN:ec=%x\n", ec);
			printf("SIOPEN:vp->vb_howmany=%x\n", vp->vb_howmany);
			printf("SIOPEN:vp->vb_state=%x\n", vp->vb_state);
*/
			  vp->vb_howmany = 0;
			  vp->vb_state = 0;
		          u.u_error = ec;
		          return;
                     }
		}
#endif
		spl7();
		while ( (ctltp->sc_tstat&OOK)==0 || (ctltp->sc_tstat&COK)==0) {
			sleep( (caddr_t)ctltp, SIOCPRI );
		}
		setabort( ctl, unit, AOPEN, STDOPEN );
		ctltp->sc_tstat &= ~OOK;
		spl0();
		scb = (struct scb *) smalloc(sizeof(struct scb));
		ctltp->sc_oscbbuf = (char *)scb;
		lcb = (struct openlcb *)scb->si_un.gdc5.si_parm;
		rtn = (struct openrtn *) smalloc( sizeof(struct openrtn) );
		ctltp->sc_ortnbuf = (char *)rtn;
		pp = u.u_procp;
		scb->si_cmd = DOPEN;
		scb->si_cblen = sizeof(struct scb);
#ifdef VPMSYS
		if(vpmoricp) {
		     lcb->lo_dev = minounit;
		} else {
#endif
		     lcb->lo_dev = unit;
#ifdef VPMSYS
                }
#endif
		lcb->lo_procp = pp;
		lcb->lo_spdidx = diagswits & 07;
		lcb->lo_uid = u.u_uid;
		lcb->lo_pid = pp->p_pid;
		lcb->lo_pgrp = pp->p_pgrp;
		lcb->lo_flag = flag;
		lcb->lo_ttyp = u.u_ttyp;
		lcb->lo_rtnadx = MBSDADX(rtn);
		lcb->lo_wuadx = (int) rtn;
		rtn->ro_gate = 0xff;
		qsqo(scb, ctl, SIOCPRI);
		spl6();
		while (rtn->ro_gate != 0) {
			sleep((caddr_t) rtn, SIOCPRI);
		}
		spl0();
		if((pp->p_pid == pp->p_pgrp) &&
		   (u.u_ttyp == NULL)        &&
		   (ctltp->sc_pgrp == 0)) {
			u.u_ttyp = (struct tty *) ctltp;
			/*
			 * Returned value may be superfluous since
			 * it originally came from lcb->lo_pgrp
			 */
			u.u_ttyp->t_pgrp = rtn->ro_pgrp;
		}
		u.u_error = rtn->ro_error;
		clrbufs( ctl, unit, AOPEN );
		spl7();
		ctltp->sc_tstat |= OOK;
		rsetabort( ctl, unit, AOPEN );
#ifdef VPMSYS
		if (vpmoricp) {
			if(vp->vb_howmany == 0) {
		     		vp->vb_howmany++;
		     		vp->vb_state |= OPEN;
		     		vp->vb_device = minounit;
		     		vpmsave('o', minounit, vp->vb_state, 0);
                	}
		}
#endif
		spl0();
		wakeup( (caddr_t)ctltp );
	} else u.u_error = EBUSY;

}


/*
 * close a unit 
 */

#ifdef VPMSYS
siclose(index, unit, mincunit, vpmoricp)
#else
siclose(index, unit)
#endif
int index;
int unit;
#ifdef VPMSYS
int mincunit;
int vpmoricp;
#endif
{
	register struct closelcb *lcb;
	register struct closertn *rtn;
	register struct scb *scb;
	long	i;
	register struct sctltp *ctltp;
	register struct	sctl	*ctl;
	register struct	cmd	*pblk;
#ifdef VPMSYS
	register struct vpmb *vp;
#endif
#ifndef	OVKRNL
/* note: anything changed between these #if statments must be changed in
	 iotimo.c also
*/
	int	siocfail(), closefail();
#else
	extern	int	siocfail(), closefail();
#endif
#ifdef VPMSYS
	/* In VPM the minor unit no. contains other than */
	/* just the line #                               */

	if (vpmoricp) {
		unit = TDEV(mincunit);   /* redo unit using the last 4 bits */
		ctl = &sctl[IDEV(mincunit)];
	} else
#endif
		ctl = &sctl[index];

	ctltp = &ctl->tp_un[unit];
	if (unit == DNLDUNIT) {
		if (dnldjmpadx != 0) {
			setabort( ctl, unit, ACLOSE, DNLDCLOSE );
			pblk = (struct cmd *)ctl->sc_paddr;
			pblk->c_command = JMP;
			pblk->c_flags = CLRINT;
			pblk->c_lcladx = dnldjmpadx;
			docmd( pblk );
			pblk->c_command = MEMPRMS;
			pblk->c_mbadx = (long)siocbuf | (long)0xf0000;
			pblk->c_len = BUFSIZE;
			pblk->c_gate = 0xff;
			for(i = 0; (i < 400000) && (pblk->c_gate != 0); i++ ) {
			}
			if ( pblk->c_gate != 0 ) {
				u.u_error = EIO;
				ctl->sc_state = IDLE;
				rsetabort( ctl, unit, ACLOSE );
				return;
			}
			/*
			ctl->sc_intseq = ctl->sc_toseq = 0x8000;
			timeout( siocfail, ctl, 50);
			docmd( pblk );
			spl7();
			if ( pblk->c_gate != 0 ){
				u.u_error = EIO;
				ctl->sc_state = IDLE;
				ctl->sc_intseq = ctl->sc_toseq = 0;
				rsetabort( ctl, unit, ACLOSE );
				spl0();
				return;
			}
			 */
			ctl->sc_intseq = ctl->sc_toseq = 0;
			rsetabort( ctl, unit, ACLOSE );
			/*
			spl0();
			 */
			ctl->sc_state = RUN;
			dnldjmpadx = 0;
		} else ctl->sc_state = IDLE;
	} else if (unit == PIOUNIT) {
		/* 
		 * Do nothing until we have the real driver.
		 */

	} else if ( ctl->sc_state == RUN ) {
		/*-------------------------------------------------------*\
		| If the open for this line is partially open then there  |
		| is the danger of an infinite loop occuring in the event |
		| of a signal to the openning process. It is placed on    |
		| run queue. When it is "swtched" in, the signal is       |
		| is processed. As part of this recovery action, closep   |
		| is called. Then we sleep to wait for the icp processing |
		| to complete. When the icp returns control, the close    |
		| process is reawaken and placed on the run queue. Since  |
		| there is an outstanding signal, this whole sequence is  |
		| repeated and any actions such as timeouts etc are also  |
		| repeated.						  |
		|							  |
		| What we do here is to keep a count of the number of     |
		| times we have attempted to close this line and have     |
		| looped at the sleep. The second time arround, we do     |
		| nothing and just finish the close with out sleeping,    |
		| the fact that we're entering the close the second time  |
		| indicates that the icp has already completed its close  |
		\*-------------------------------------------------------*/
		if(ctltp->sc_lstcl++)
			goto done;

		spl7();
		/*
		 * if "timeout ignored" or "idle timeout" continue.
		 * We want to wait until all timeouts on this line are
		 * finished.
		 */
		while ( ctltp->sc_timstate > 0) {
			sleep( (caddr_t) ctltp, SIOCPRI );
		}
/* added */	ctltp->sc_tstat &= ~COK;
		spl0();
#ifdef VPMSYS
		lastclunit = mincunit;
#endif
		setabort( ctl, unit, ACLOSE, STDCLOSE );
		scb = (struct scb *) smalloc(sizeof(struct scb));
		ctltp->sc_cscbbuf = (char *) scb;
		lcb = (struct closelcb *) scb->si_un.gdc5.si_parm;
		rtn = (struct closertn *) smalloc(sizeof(struct closertn));
		ctltp->sc_crtnbuf = (char *)rtn;
		scb->si_cmd = DCLOSE;
		scb->si_cblen = sizeof(struct scb);
#ifdef VPMSYS
		if(vpmoricp) {
		     lcb->lc_dev = mincunit;
		} else {
#endif
		     lcb->lc_dev = unit;
#ifdef VPMSYS
                }
#endif
		lcb->lc_procp = u.u_procp;
		lcb->lc_pid = u.u_procp->p_pid;
		lcb->lc_rtnadx = MBSDADX(rtn);
		lcb->lc_wuadx = (int) rtn;
		rtn->rc_gate = 0xff;
		ctltp->sc_buf = 0;
		clrbufs( ctl, unit, AREAD );
#ifdef VPMSYS
		lcb->lc_vpmhalt = 0;

		if (vpmoricp) {
			/* vp = ICP# times 4 plus protocol #    */
			vp = &vpmb[IDEV(mincunit) * 4 + BDEV(mincunit)];

			vp->vb_howmany--;

			if((vp->vb_howmany) < 0) 
		          vp->vb_howmany = 0;

			if(vp->vb_howmany ==0) {
				lcb->lc_vpmhalt = 1;
				vpmsave('t',lcb->lc_dev, vp->vb_state <<8, 0);
			}
		}
#endif
		qsqo(scb, ctl, SIOCPRI);
/*		timeout( closefail, rtn, 100 );*/
#ifdef VPMSYS
		if(vpmoricp) {
			timeout( closefail, rtn, 300 );
		}
#endif
		spl6();
		while (rtn->rc_gate != 0) {
			sleep((caddr_t) rtn, SIOCPRI);
		}
/*
		Time out removed hence cleanup software no longer needed
		if ( !(rtn->rc_error) ) {
			rtn->rc_gate = 0xff;
			while (rtn->rc_gate != 0 ) {
				sleep( (caddr_t)rtn, SIOCPRI );
			}
			rtn->rc_error = 0;
		}
*/
	done:
		rsetabort( ctl, unit, ACLOSE );
/* added */	ctltp->sc_tstat |= COK;
		spl0();
		u.u_error = ctltp->sc_crtnbuf->rc_error;
		/*
		 * reset copy of c_lflag (from icp) to 0. this is its
		 * default value. Any ioctl calls will reset this so
		 * that it corresponds to the values in the icp.
		 */
		ctltp->sc_lflag = 0;
		/*
		 * reset count of # of retries on this close.
		 */
		ctltp->sc_lstcl = 0;
		clrbufs( ctl, unit, ACLOSE );
#ifdef VPMSYS
		if (vpmoricp) {
		     /* vp = ICP# times 4 plus protocol #    */
		     vp = &vpmb[IDEV(mincunit) * 4 + BDEV(mincunit)];

		     vp->vb_state |= WCLOSE;
		     vpmsave('c', mincunit, vp->vb_state, 0);

		     /* VPM check to see if protocol is o.k. */
		     if(vp->vb_state&VPMERR) {
			u.u_error = EIO;
			vp->vb_state = 0;
			return;
		     }
		     vp->vb_state = 0;
                }
#endif
	} else u.u_error = EBUSY;

}


/*
 * read a unit
 */

#ifdef VPMSYS
siread(index, unit, minrunit, vpmoricp)
#else
siread(index, unit)
#endif
int index;
int unit;
#ifdef VPMSYS
int minrunit;
int vpmoricp;
#endif
{

	int cnt;
	register len;
	register char *bp;
	register struct readlcb *lcb;
	register struct readrtn *rtn;
	register struct scb *scb;
	register char *buf;
	int flag;
	register struct sctltp *ctltp;
#ifdef VPMSYS
	register struct vpmb *vp;
#endif
	struct	sctl	*ctl;
	struct cmd	*pblk;
#ifdef VPMSYS
	struct vpmbd    *dp;
#endif
	int	usrmore, adrflag, cntflag;
#ifdef VPMSYS
        char arg[4];
        int i;

	/* In VPM the minor unit no. contains other than */
	/* just the line #                               */

	if(vpmoricp) {
		unit = TDEV(minrunit);   /* redo unit using the last 4 bits */
		ctl = &sctl[IDEV(minrunit)];
	} else
#endif
		ctl = &sctl[index];

	ctltp = &ctl->tp_un[unit];
	if (unit == DNLDUNIT) {
		setabort( ctl, unit, AREAD, DNLDREAD );
		pblk = (struct cmd *)ctl->sc_paddr;
		pblk->c_command = RBLK;
		pblk->c_flags = CLRINT;
		pblk->c_mbadx = MBSDADX(dnldbuf);
		pblk->c_lcladx = u.u_offset;
		len = u.u_count;
		if ( (pblk->c_lcladx + len) > (unsigned)0xc000 ) {
			u.u_error = ENXIO;
			return;
		}
		adrflag = cntflag = 0;
		if ( pblk->c_lcladx & 1 ) {
			pblk->c_lcladx--;
			len++;
			adrflag++;
		}
		if ( len & 1 ) {
			len++;
			cntflag++;
		}
		while (len) {
			cnt = min(len, DNLDBUFSIZE);
			pblk->c_len = cnt;
			docmd( pblk );
			len -= cnt;
			pblk->c_lcladx += cnt;
			if ( adrflag ) {
				bp = &dnldbuf[1];
				adrflag = 0;
				cnt--;
			} else bp = dnldbuf;
			if ( !len && cntflag ) {
				cnt--;
				cntflag = 0;
			}
			for (; cnt; cnt--) {
				passc(*bp++);
			}
		}
		rsetabort( ctl, unit, AREAD );
	} else if ( ctl->sc_state == RUN ) {
#ifdef VPMSYS
/*
		printf("SIREAD\n");
*/
	        /* VPM check to see if protocol is o.k. */
	        /* vp = ICP# times 4 plus protocol #    */
		if(vpmoricp) {
	             vp = &vpmb[IDEV(minrunit) * 4 + BDEV(minrunit)];
	             if(vp->vb_state&VPMERR) {
	                  u.u_error = EIO;
	                  return;
                     }
		}
#endif
		flag = TRUE;  /* only first icp request of several, do canon */
		while ( 1 ) {
			spl7();
			while ( (ctltp->sc_tstat & ROK) == 0 ) {
				sleep( (caddr_t)ctltp, SIOCPRI );
			}
#ifdef VPMSYS
			if(vpmoricp) {
	                     if(vp->vb_state&VPMERR) {
	                          u.u_error = EIO;
				  vp->vb_state = 0;
				  spl0();
	                          return;
                             }
			}
#endif
			ctltp->sc_tstat &= ~ROK;
			setabort( ctl, unit, AREAD, STDREAD );
			spl0();
			if ( !(ctltp->sc_buf) ) {
				scb = (struct scb *) smalloc(sizeof(struct scb));
				ctltp->sc_rscbbuf = (char *)scb;
				lcb = (struct readlcb *) scb->si_un.gdc5.si_parm;
				rtn = (struct readrtn *) smalloc(sizeof(struct readrtn));
				ctltp->sc_rrtnbuf = (char *)rtn;
#ifdef VPMSYS
				buf = (char *) smalloc(VRBUFSIZE);
#else
				buf = (char *) smalloc(RBUFSIZE);
#endif
				ctltp->sc_rbufs = (char *)buf;
				scb->si_cmd = DREAD;
				scb->si_cblen = sizeof(struct scb);
#ifdef VPMSYS
				if(vpmoricp) {
				     lcb->lr_dev = minrunit;
				} else {
#endif
				     lcb->lr_dev = unit;
#ifdef VPMSYS
                                }
#endif
				lcb->lr_procp = u.u_procp;
				lcb->lr_pid = u.u_procp->p_pid;
				lcb->lr_fmode = u.u_fmode;
				lcb->lr_bufadx = MBSDADX(buf);
				lcb->lr_rtnadx = MBSDADX(rtn);
				lcb->lr_wuadx = (int) rtn;
				while (1) {
					lcb->lr_flag = flag;
					rtn->rr_gate = 0xff;
					qsqo(scb, ctl, SIOCPRI);
#ifdef VPMSYS
					/* If VPM send an I/O cntl to the ICP*/
					/* in order to send 3 bytes          */
					/* related to the protocol.          */

					if(vpmoricp) {
						dp = &vpmbd[IDEV(minrunit)];
						lcb->lr_rbufin[0] = dp->d_type;
						lcb->lr_rbufin[1] = dp->d_sta;
						lcb->lr_rbufin[2] = dp->d_dev;
					}
#endif
					spl6();
					while (rtn->rr_gate != 0) {
						sleep((caddr_t) rtn, SIOCPRI);
					}
					spl0();
					flag = FALSE;
					ctltp->sc_error = rtn->rr_error;
					if (rtn->rr_error == EIO ) {
						goto out;
					}
#ifdef VPMSYS
					if(vpmoricp) {
	                                     if(vp->vb_state&VPMERR) {
	                                          u.u_error = EIO;
	                                          goto out;
                                             }
                                        /* If VPM, update dp's 3 bytes */
                                        /* from the first three bytes  */
                                        /* of the read buffer.         */

                                        dp->d_type = *buf++;
                                        dp->d_sta  = *buf++;
                                        dp->d_dev  = *buf++;
                                        *buf++;
                                        rtn->rr_count -= 4;

                                        }
#endif
					for(bp=buf, len=rtn->rr_count; len;  ) {
						if( passc(*bp++) == (-1) ) {
							if ( --len ) {
								ctltp->sc_buf = bp;
								ctltp->sc_len = len;
								goto out;
							} else goto out;
						} else len--;
					}
					if(rtn->rr_error == NULL) {
						break;
					}
				}
				out:
#ifdef VPMSYS
				word1 = rtn->rr_count;
#endif
				smfree( sizeof(struct scb), scb );
				smfree( sizeof(struct readrtn), rtn );
				ctltp->sc_rscbbuf = 0;
				ctltp->sc_rrtnbuf = 0;
				if ( !ctltp->sc_buf ) {
#ifdef VPMSYS
					smfree( VRBUFSIZE , ctltp->sc_rbufs );
#else
					smfree( RBUFSIZE, buf );
#endif
					ctltp->sc_rbufs = 0;
				}
				break;
			} else {
				for(bp=ctltp->sc_buf, len=ctltp->sc_len; len;) {
					if ((usrmore = passc(*bp++)) == (-1)) {
						if ( --len ) {
							ctltp->sc_buf = bp;
							ctltp->sc_len = len;
							break;
						} else break;
					} else len--;
				}
#ifdef VPMSYS
				word1 = ctltp->sc_len;
#endif
				if ( !len ) {
#ifdef VPMSYS
					smfree( VRBUFSIZE, ctltp->sc_rbufs );
#else
					smfree( RBUFSIZE, ctltp->sc_rbufs );
#endif
					ctltp->sc_buf = 0;
					ctltp->sc_rbufs = 0;
					if ( ctltp->sc_error == EFBIG && usrmore != (-1) ) {
						ctltp->sc_tstat |= ROK;
						continue;
					} else break;
				} else break;
			}
		}
#ifdef VPMSYS
		if(vpmoricp) {
		     vpmsave('r', minrunit, word1, 0);
                }
#endif
		spl7();
		ctltp->sc_tstat |= ROK;
		rsetabort( ctl, unit, AREAD );
		spl0();
		wakeup( (caddr_t)ctltp );
	} else {
		u.u_error = EBUSY;
		return;
	}


}


/*
 * write to a unit
 */

#ifdef VPMSYS
siwrite(index, unit, minwunit, vpmoricp)
#else
siwrite(index, unit)
#endif
int index;
int unit;
#ifdef VPMSYS
int minwunit;
int vpmoricp;
#endif
{

	int cnt;
	register len;
	register char *bp;
	register struct writelcb *lcb;
	register struct writertn *rtn;
	register struct sctltp *ctltp;
	register struct scb *scb;
	register char *buf;
	register c;
#ifdef VPMSYS
	register struct vpmb *vp;
#endif
	int	scnt;
	struct	sctl	*ctl;
	struct	cmd	*pblk;
#ifdef VPMSYS
	struct vpmbd    *dp;
#endif
	int	i;
#ifndef	OVKRNL
/* note: anything changed between these #if statments must be changed in
	 iotimo.c also
*/
	int	siwflsh();
#else
	extern	int	siwflsh();
#endif
#ifdef VPMSYS
	int wbufsize;
        char arg[4];

	/* In VPM the minor unit no. contains other than */
	/* just the line #                               */

	if(vpmoricp) {
		unit = TDEV(minwunit);   /* redo unit using the last 4 bits */
		ctl = &sctl[IDEV(minwunit)];
	} else
#endif
		ctl = &sctl[index];

	ctltp = &ctl->tp_un[unit];
	if (unit == DNLDUNIT ) {
		if ( ctl->sc_state != DNLD ) {
			u.u_error = ENXIO;
			return;
		}
		setabort( ctl, unit, AWRITE, DNLDWRITE );
		pblk = (struct cmd *)ctl->sc_paddr;
		pblk->c_command = WBLK;
		pblk->c_flags = CLRINT;
		pblk->c_mbadx = MBSDADX(dnldbuf);
		pblk->c_lcladx = u.u_offset;
		if ( dnldjmpadx == 0 )
			dnldjmpadx = u.u_offset;
		len = u.u_count;
		while (len) {
			scnt = cnt = min(len, DNLDBUFSIZE);
			pblk->c_len = cnt;
			for (bp = dnldbuf; cnt; cnt--) {
				*bp++ = cpass();
			}
			docmd( pblk );
			pblk->c_lcladx += scnt;
			len -= scnt;
		}
		rsetabort( ctl, unit, AWRITE );
	} else if (unit == PIOUNIT) {
		spl7();
		while ( ctl->sc_notpok ) 
			sleep( (caddr_t)&ctl->sc_notpok, SIOCPRI );
		ctl->sc_notpok++;
		spl0();
		setabort( ctl, unit, APWRITE, STDPWRITE );
		scb = (struct scb *) smalloc(sizeof(struct scb));
		ctl->sc_pscbbuf = (char *) scb;
		lcb = (struct writelcb *) scb->si_un.gdc5.si_parm;
		rtn = (struct writertn *) smalloc(sizeof(struct writertn));
		ctl->sc_prtnbuf = (char *) rtn;
#ifdef VPR
		buf = (char *) smalloc(4*WBUFSIZE);
#else
		buf = (char *) smalloc(WBUFSIZE);
#endif
		ctl->sc_pbufs = (char *) buf;
		scb->si_cmd = DWRITE;
		scb->si_cblen = sizeof(struct scb);
		lcb->lw_dev = unit;
		lcb->lw_procp = u.u_procp;
		lcb->lw_pid = u.u_procp->p_pid;
		lcb->lw_ctlu = 0;
		lcb->lw_bufadx = MBSDADX(buf);
		lcb->lw_rtnadx = MBSDADX(rtn);
		lcb->lw_wuadx = (int) rtn;
		while (u.u_count) {
#ifdef VPR
			len = min(4*WBUFSIZE,u.u_count);
			if(copyin(u.u_base, buf, len) == -1) {
				u.u_error = EFAULT;
				goto out1;
			}
			u.u_count  -= len;
			u.u_base   += len;
			u.u_offset += len;
#else
			for (bp = buf, len = 0; len < WBUFSIZE; len++) {
				if((c = cpass()) < 0) {
					break;
				}
				*bp++ = c;
			}
#endif
			lcb->lw_count = len;
			rtn->rw_gate = 0xff;
			qsqo(scb, ctl, SIOCPRI);
			spl6();
			while (rtn->rw_gate != 0) {
				sleep((caddr_t) rtn, SIOCPRI);
			}
			spl0();
			if (rtn->rw_error == EIO ) {
				u.u_count += len;
				u.u_offset -= len;
				u.u_base -= len;
				goto out1;
			}
		}

		out1:
		clrbufs( ctl, unit, APWRITE );
		rsetabort( ctl, unit, APWRITE );
		ctl->sc_notpok = 0;
		wakeup( (caddr_t)&ctl->sc_notpok );
	} else if ( ctl->sc_state == RUN ) {
#ifdef VPMSYS
	        /* VPM check to see if protocol is o.k. */
	        /* vp = ICP# times 4 plus protocol #    */
		if(vpmoricp) {
	             vp = &vpmb[IDEV(minwunit) * 4 + BDEV(minwunit)];
	             if(vp->vb_state&VPMERR) {
			  vp->vb_state = 0;
	                  u.u_error = EIO;
			  spl0();
	                  return;
                     }
		}
#endif
		spl7();
		while ( (ctltp->sc_tstat & WOK) == 0 ) {
			sleep( (caddr_t)ctltp, SIOCPRI );
		}
#ifdef VPMSYS
		if(vpmoricp) {
	             if(vp->vb_state&VPMERR) {
			  vp->vb_state = 0;
	                  u.u_error = EIO;
	                  return;
                     }
                     vp->vb_state |= VPMWIP;
		}
#endif
		setabort( ctl, unit, AWRITE, STDWRITE );
		ctltp->sc_tstat &= ~WOK;
		spl0();
		if ( !ctltp->sc_wbufs ) {
			scb = (struct scb *) smalloc(sizeof(struct scb));
			ctltp->sc_wscbbuf = (char *) scb;
			lcb = (struct writelcb *) scb->si_un.gdc5.si_parm;
			rtn = (struct writertn *) 
					smalloc(sizeof(struct writertn));
			ctltp->sc_wrtnbuf = (char *) rtn;
#ifdef VPMSYS
			buf = (char *) smalloc(VWBUFSIZE);
#else
			buf = (char *) smalloc(WBUFSIZE);
#endif
			ctltp->sc_wbufs = (char *) buf;
			scb->si_cmd = DWRITE;
			scb->si_cblen = sizeof(struct scb);
#ifdef VPMSYS
			if(vpmoricp) {
			        lcb->lw_dev = minwunit;
                        } else {
#endif
				lcb->lw_dev = unit;
#ifdef VPMSYS
                        }
#endif
			lcb->lw_procp = u.u_procp;
			lcb->lw_pid = u.u_procp->p_pid;
			lcb->lw_ctlu = 0;
			lcb->lw_bufadx = MBSDADX(buf);
			lcb->lw_rtnadx = MBSDADX(rtn);
			lcb->lw_wuadx = (int) rtn;
			ctltp->sc_bspot = 0;
		} else {
			scb = (struct scb *)ctltp->sc_wscbbuf;
			lcb = (struct writelcb *)scb->si_un.gdc5.si_parm;
			rtn = (struct writertn *) ctltp->sc_wrtnbuf;
			buf = (char *)( (int)ctltp->sc_wbufs
						+ ctltp->sc_bspot );
		}
		/*
		trcwrt( 0, unit, u.u_count, u.u_base, scb, lcb, rtn, buf,
				ctltp->sc_bspot, 0 );
		*/
		while (u.u_count) {
			i = min( (WBUFSIZE - ctltp->sc_bspot),
					u.u_count );
			/*
			trcwrt( 1, unit, u.u_count, u.u_base, scb, lcb,
					rtn, buf, ctltp->sc_bspot,
					i );
			*/
#ifdef VPMSYS
                        if(vpmoricp) {
			     i = min( ((VWBUFSIZE-4) - ctltp->sc_bspot),
					u.u_count );
                             dp = &vpmbd[IDEV(minwunit)];

                             *buf++ = dp->d_type;
                             *buf++ = dp->d_sta;
                             *buf++ = dp->d_dev;
                             *buf++;
                        }
#endif
			if ( cpyinb( u.u_base, buf, i, u.u_segflg ) != 0 ) {
				u.u_error = EFAULT;
				/*
				printf( "cpyinb error %x,%x,%x,%x\n",
					u.u_base, buf, i, u.u_segflg );
				printf( "call Monte or take full dumps!\n" );
				debug();
				*/
				/* if any pending timeout, then disable
				 * it. We may want to change this in the
				 * future to allow the timeout to finish. We
				 * would then leave the timeout state alone and
				 * change the place where we jump to prevent
				 * the buffer from being flushed.
				 */
				/* if a timeout, then we want to inhibit it */
				spl7();
				if(ctltp->sc_timstate)
					ctltp->sc_timstate = -1;
				spl0();
				goto out2;
			}
#ifdef VPMSYS
			if(vpmoricp) {
			     word1 = i;
			     vpmsave('w', minwunit, word1, 0);
                        }
#endif
			ctltp->sc_bspot += i;
#ifdef VPMSYS
			if(vpmoricp) {
				wbufsize = VWBUFSIZE;
			} else {
				wbufsize = WBUFSIZE;
			}
			if ((ctltp->sc_bspot == wbufsize) || (vpmoricp != 0)) {
#else
			if ( ctltp->sc_bspot == WBUFSIZE ) {
#endif
				/*
				 * buffer has been filled. disable any
				 * outsanding timeout event.
				 */
				spl7();
				if(ctltp->sc_timstate)
					ctltp->sc_timstate = -1;
				spl0();
#ifdef VPMSYS
				if(vpmoricp) {
					lcb->lw_count = i+4;
				} else {
#endif
					lcb->lw_count = WBUFSIZE;
#ifdef VPMSYS
				}
#endif
				/* tell icp that this is a normal write */
				lcb->lw_ctlu = 0;
#ifdef SEQ
				lcb->lw_seq = ++numsent;
#endif
				rtn->rw_gate = 0xff;
				qsqo(scb, ctl, SIOCPRI);
				spl6();
				while (rtn->rw_gate != 0) {
					sleep((caddr_t) rtn, SIOCPRI);
				}
				spl0();
				if (rtn->rw_error != NULL ) {
					u.u_error = rtn->rw_error;
					/*
					printf( "error return for write!\n" );
					printf( "call monte or take full" ); 
					printf( "dump!\n" );
					debug();
					*/
					ctltp->sc_bspot = 0;
					goto out2;
				}
#ifdef VPMSYS
				if(vpmoricp) {
	                             if(vp->vb_state&VPMERR) {
	                                  u.u_error = EIO;
	                                  goto out2;
                                     }

                                ctltp->sc_timstate = 0;

				}
#endif
				u.u_count -= i;
				u.u_offset += i;
				u.u_base += i;
				ctltp->sc_bspot = 0;
				buf = (char *)ctltp->sc_wbufs;
			} else {
				u.u_count -= i;
				u.u_offset += i;
				u.u_base += i;
				spl7();
				/* if no outstanding timeout event */
				if(ctltp->sc_timstate == 0) {
					ctltp->cu_un.cu_unit = unit;
					ctltp->cu_un.cu_ctl  = ctl;
					timeout( siwflsh, &ctltp->cu_un, 2);
					/* there is now an outstanding timeout
					   event */
					ctltp->sc_timstate = 1;
				}
				/*
				 * if we've already retried timeout once, then
				 * force flush when timeout occurs i.e. don't
				 * reinstate timeout. Otherwise when timeout
				 * occurs, we want to wait a little longer so
				 * retimeout at end of current event but do
				 * no flushing at that time.
				 */
				else if(ctltp->sc_timstate != 3)
					ctltp->sc_timstate = 2;
				spl0();
			}
		}
#ifdef VPMSYS
		/* The VPM protocol script controls when */
		/* the write is "o.k.".                  */

		if(vpmoricp) {
                	spl6();
			while((vp->vb_state&VPMWIP) != 0) {

		     		sleep((caddr_t)&vp->vb_state, SIOCPRI);

                	}
                	spl0();
		}
#endif
		out2:
#ifdef VPMSYS
		if(vpmoricp) {
                	vp->vb_state &= ~VPMWIP;
		}
#endif
		rsetabort( ctl, unit, AWRITE );
		/* clean up buffers if "ignore timeout" or "idle" */
		if(ctltp->sc_timstate <= 0) {
			clrbufs( ctl, unit, AWRITE );
		}
		ctltp->sc_tstat |= WOK;
		wakeup( (caddr_t)ctltp );
	} else u.u_error = EBUSY;
}

/*
 * i/o control
 */

#ifdef VPMSYS
siioctl(index, unit, cmd, arg, mode, minsiunt, vpmoricp)
#else
siioctl(index, unit, cmd, arg, mode)
#endif
int index;
int unit;
int cmd;
int arg;
int mode;
#ifdef VPMSYS
int minsiunt;
int vpmoricp;
#endif
{

	register struct ioctllcb *lcb;
	register struct ioctlrtn *rtn;
	register struct scb *scb;
	register char *p;
	register psize;
	register struct sctltp *ctltp;
#ifdef VPMSYS
	register struct vpmb *vp;
#endif
	struct sctl	*ctl;
#ifdef VPMSYS
        char cmdar[4], *cp;

	/* In VPM the minor unit no. contains other than */
	/* just the line #                               */

	if(vpmoricp) {
		unit = TDEV(minsiunt);   /* redo unit using the last 4 bits */
		vp = &vpmb[IDEV(minsiunt) * 4 + BDEV(minsiunt)];
		ctl = &sctl[IDEV(minsiunt)];
        } else
#endif
		ctl = &sctl[index];

	ctltp = &ctl->tp_un[unit];
#ifdef VPR
	if ( ctl->sc_state == RUN || unit == PIOUNIT) {
#else
	if ( ctl->sc_state == RUN ) {
#endif
		spl6();
		/* dont change state if outstanding timeout event */
		while ((ctltp->sc_tstat&IOK) == 0 || (ctltp->sc_timstate>0)) {
			sleep( (caddr_t)ctltp, SIOCPRI );
		}
		setabort( ctl, unit, AIOCTL, STDIOCTL );
		ctltp->sc_tstat &= ~IOK;
		spl0();
		scb = (struct scb *) smalloc(sizeof(struct scb));
		ctltp->sc_iscbbuf = (char *)scb;
		lcb = (struct ioctllcb *) scb->si_un.gdc5.si_parm;
		rtn = (struct ioctlrtn *) smalloc(sizeof(struct ioctlrtn));
		ctltp->sc_irtnbuf = (char *)rtn;
		scb->si_cmd = DIOCTL;
		scb->si_cblen = sizeof(struct scb);
#ifdef VPMSYS
		if(vpmoricp) {
		        lcb->li_dev = minsiunt;
                } else {
#endif
			lcb->li_dev = unit;
#ifdef VPMSYS
		}
#endif
		lcb->li_procp = u.u_procp;
		lcb->li_pid = u.u_procp->p_pid;
		lcb->li_com = cmd;
		lcb->li_arg = arg;
		lcb->li_rtnadx = MBSDADX(rtn);
		lcb->li_wuadx = (int) rtn;
		psize = 0;
		switch (cmd) {

#ifdef VPR
		case VSETSTATE:
			p = (char *) smalloc(psize = sizeof(struct vprio));
			ctltp->sc_ibuf = p;
			ctltp->sc_ibuflen = psize;
			if (copyin((char *)arg, (caddr_t) p, psize) ) {
				u.u_error = EFAULT;
				goto out;
			}
			lcb->li_parmadx = MBSDADX(p);
			break;
#endif
		case TIOCSETP:
		case TIOCSETN:
			p = (char *) smalloc(psize = sizeof(struct sgttyb));
			ctltp->sc_ibuf = p;
			ctltp->sc_ibuflen = psize;
			if (copyin((char *)arg, (caddr_t) p, psize) ) {
				u.u_error = EFAULT;
				goto out;
			}
			lcb->li_parmadx = MBSDADX(p);
			break;

		case TIOCGETP:
			p = (char *) smalloc(psize = sizeof(struct sgttyb));
			ctltp->sc_ibuf = p;
			ctltp->sc_ibuflen = psize;
			lcb->li_parmadx = MBSDADX(p);
			break;

		case TCSETA:
		case TCSETAW:
		case TCSETAF:
			p = (char *) smalloc(psize = sizeof(struct termio));
			ctltp->sc_ibuf = p;
			ctltp->sc_ibuflen = psize;
			if (copyin((char *)arg, (caddr_t) p, psize) ) {
				u.u_error = EFAULT;
				goto out;
			}
			lcb->li_parmadx = MBSDADX(p);
			break;

		case TCGETA:
			p = (char *) smalloc(psize = sizeof(struct termio));
			ctltp->sc_ibuf = p;
			ctltp->sc_ibuflen = psize;
			lcb->li_parmadx = MBSDADX(p);
			break;

#ifdef PROFILE		/* request to get profile data from icp */
		case TCGETP:
			lcb->li_parmadx = MBSDADX(icpprof);
			break;
#endif
#ifdef VPMSYS
		case VPMCMD:
                vpmcmd:
			copyin((char *) arg, cmdar, sizeof(cmdar));
			lcb->li_parmadx = MBSDADX(cmdar);
			break;

		case VPMERRS:
			break;
		case VPMRPT:
			if(vp->vb_state&VPMRPTA) {
			     vp->vb_state &= ~VPMRPTA;
			     cp = vp->vb_rpt;
			     copyout(cp, (char *) arg, 4);
			     u.u_rval1 = 1;
                        }
                     goto out;
		
		case RUNCMD:
		case HLTCMD:
			break;
#endif
		}
		switch (cmd) {

		case TCSETAF:
		case TCFLSH:
		case TIOCSETP:
			if ( ctltp->sc_tstat & ROK ) {
				ctltp->sc_buf = 0;
				clrbufs( ctl, unit, AREAD );
			}
		}
		rtn->ri_gate = 0xff;
		qsqo(scb, ctl, SIOCPRI);
		spl6();
		while (rtn->ri_gate != 0) {
			sleep((caddr_t) rtn, SIOCPRI);
		}
		spl0();
#ifdef VPMSYS
		if(cmd == RUNCMD || cmd == HLTCMD )
			goto out;
#endif
		if (rtn->ri_error != NULL) {
			u.u_error = rtn->ri_error;
			goto out;
		}
#ifdef PROFILE		/*>>>>> ICP profile data request */
		if(cmd == TCGETP)
			if (copyout(icpprof, (char *)arg, sizeof(icpprof)))
				u.u_error = EFAULT;
#endif
		if(cmd == TCGETA || cmd == TIOCGETP)
			if (copyout((caddr_t) p, (char *)arg, psize))
				u.u_error = EFAULT;
#ifdef VPMSYS
		if(cmd == VPMERRS) {
			cp = vp->vb_errs;
			if (copyout(cp, (char *)arg, 8))
				u.u_error = EFAULT;
		}
#endif
		out:
		smfree(sizeof(struct scb), scb);
		smfree(sizeof(struct ioctlrtn), rtn);
		ctltp->sc_iscbbuf = 0;
		ctltp->sc_irtnbuf = 0;
		if (psize) {
			smfree(psize, p);
			ctltp->sc_ibuf = 0;
		}
		rsetabort( ctl, unit, AIOCTL );
		spl7();
		ctltp->sc_tstat |= IOK;
		spl0();
		wakeup( (caddr_t)ctltp );
	} else u.u_error = EBUSY;

	return( u.u_error ? 1 : 0 );
}

setabort( ctl, unit, func, spot )
register struct sctl	*ctl;
{
	register struct sctltp *ctltp;
	int	siabort();

	ctltp = &ctl->tp_un[unit];
	u.u_abreq = siabort;

	switch ( func ) {

	case AOPEN:
		ctltp->sc_oprocp = u.u_procp;
		ctltp->sc_opid = u.u_procp->p_pid;
		ctltp->sc_ospot = spot;
		break;

	case ACLOSE:
		ctltp->sc_cprocp = u.u_procp;
		ctltp->sc_cpid = u.u_procp->p_pid;
		ctltp->sc_cspot = spot;
		break;

	case AREAD:
		ctltp->sc_rprocp = u.u_procp;
		ctltp->sc_rpid = u.u_procp->p_pid;
		ctltp->sc_rspot = spot;
		break;

	case AWRITE:
		ctltp->sc_wprocp = u.u_procp;
		ctltp->sc_wpid = u.u_procp->p_pid;
		ctltp->sc_wspot = spot;
		break;

	case APWRITE:
		ctl->sc_pprocp = u.u_procp;
		ctl->sc_ppid = u.u_procp->p_pid;
		ctl->sc_pspot = spot;
		break;

	case AIOCTL:
		ctltp->sc_iprocp = u.u_procp;
		ctltp->sc_ipid = u.u_procp->p_pid;
		ctltp->sc_ispot = spot;
		break;
	}

}

rsetabort( ctl, unit, func )
register struct sctl	*ctl;
{

	register struct sctltp *ctltp;
	ctltp = &ctl->tp_un[unit];
	u.u_abreq = 0;

	switch ( func ) {

	case AOPEN:
		ctltp->sc_oprocp = 0;
		ctltp->sc_opid = 0;
		ctltp->sc_ospot = 0;
		break;

	case ACLOSE:
		ctltp->sc_cprocp = 0;
		ctltp->sc_cpid = 0;
		ctltp->sc_cspot = 0;
		break;

	case AREAD:
		ctltp->sc_rprocp = 0;
		ctltp->sc_rpid = 0;
		ctltp->sc_rspot = 0;
		break;

	case AWRITE:
		ctltp->sc_wprocp = 0;
		ctltp->sc_wpid = 0;
		ctltp->sc_wspot = 0;
		break;

	case APWRITE:
		ctl->sc_pprocp = 0;
		ctl->sc_ppid = 0;
		ctl->sc_pspot = 0;
		break;

	case AIOCTL:
		ctltp->sc_iprocp = 0;
		ctltp->sc_ipid = 0;
		ctltp->sc_ispot = 0;
		break;
	}

}

#ifndef	OVKRNL
/* note: anything changed between these #if statments must be changed in
	 iotimo.c also
*/
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
#endif

#ifdef VPMSYS
srchtp( ctl , j)
#else
srchtp( ctl )
#endif
register struct sctl *ctl;
#ifdef VPMSYS
register int j;
#endif
{
	register struct proc	*procp;
	register int	pid, i;

	procp = u.u_procp;
	pid = u.u_procp->p_pid;
	for ( i =0; i < 8; i++ ) {
		       if ( procp == ctl->tp_un[i].sc_oprocp && 
		            pid == ctl->tp_un[i].sc_opid ) {
				soabt( ctl, i );
#ifdef VPMSYS
				vpmflags(i, j);
#endif
				return(1);
		} else if ( procp == ctl->tp_un[i].sc_cprocp &&
			    pid == ctl->tp_un[i].sc_cpid ) {
				scabt( ctl, i );
#ifdef VPMSYS
				vpmflags(i, j);
#endif
				return(1);
		} else if ( procp == ctl->tp_un[i].sc_rprocp &&
			    pid == ctl->tp_un[i].sc_rpid ) {
				srabt( ctl, i );
#ifdef VPMSYS
				vpmflags(i, j);
#endif
				return(1);
		} else if ( procp == ctl->tp_un[i].sc_wprocp &&
			    pid == ctl->tp_un[i].sc_wpid ) {
				swabt( ctl, i );
#ifdef VPMSYS
				vpmflags(i, j);
#endif
				return(1);
		} else if ( procp == ctl->sc_pprocp &&
			    pid == ctl->sc_ppid ) {
				spabt( ctl, i );
				return(1);
		} else if ( procp == ctl->tp_un[i].sc_iprocp &&
			    pid == ctl->tp_un[i].sc_ipid ) {
				siabt( ctl, i );
#ifdef VPMSYS
				vpmflags(i, j);
#endif
				return(1);
		}
	}
	return(0);
}
#ifdef VPMSYS
vpmflags(i, j)
int i, j;
{
	register struct vpmb *vp;
	int k;

	for (k=0; k<VPMBS*(j+1); k++) {
		vp = &vpmb[k];
		if(TDEV(vp->vb_device) == i) {
			vp->vb_state = 0;
			vp->vb_howmany = 0;
		}
	}
}
#endif
soabt( ctl, unit )
register struct sctl *ctl;
{
	int	s;

	switch ( ctl->tp_un[unit].sc_ospot ) {

	case DNLDOPEN:
		s = spl7();
		ctl->sc_intseq = ctl->sc_toseq = 0;
		ctl->sc_state = IDLE;
		splx( s );
		break;

	case STDOPEN:
		siclean( ctl, unit );
		clrbufs( ctl, unit, AOPEN );
		ctl->tp_un[unit].sc_tstat |= OOK;
		wakeup( (caddr_t)&ctl->tp_un[unit] );
		break;

	}

	rsetabort( ctl, unit, AOPEN );
}

scabt( ctl, unit )
register struct sctl *ctl;
{
	int	s;

	switch( ctl->tp_un[unit].sc_cspot ) {

	case DNLDCLOSE:
		s = spl7();
		ctl->sc_intseq = ctl->sc_toseq = 0;
		ctl->sc_state = IDLE;
		splx( s );
		break;

	case STDCLOSE:
/* causes problem with second pass through close due to partial open
/* added	s = spl7();
/* added	while((ctl->tp_un[unit].sc_tstat&COK) == 0)
/* added		sleep((caddr_t) ctl->tp_un[unit], SIOCPRI);
/* added	splx(s);
*/
		siclean( ctl, unit );	
		clrbufs( ctl, unit, ACLOSE );
/* added */	ctl->tp_un[unit].sc_tstat |= COK;
/* added	wakeup( (caddr_t)&ctl->tp_un[unit] ); */
		break;

	}

	rsetabort( ctl, unit, ACLOSE );
}

srabt( ctl, unit )
register struct sctl *ctl;
{

	register struct sctltp *ctltp;
	ctltp = &ctl->tp_un[unit];
	switch ( ctltp->sc_rspot ) {

	case STDREAD:
		siclean( ctl, unit );
		clrbufs( ctl, unit, AREAD );
		ctltp->sc_tstat |= ROK;
		wakeup( (caddr_t) ctltp );
		break;

	}

	rsetabort( ctl, unit, AREAD );

}

swabt( ctl, unit )
register struct sctl *ctl;
{

	register struct sctltp *ctltp;
	ctltp = &ctl->tp_un[unit];
	switch ( ctltp->sc_wspot ) {

	case STDWRITE:
		siclean( ctl, unit );
		clrbufs( ctl, unit, AWRITE );
		ctltp->sc_tstat |= WOK;
		wakeup( (caddr_t)ctltp );
		break;

	}

	rsetabort( ctl, unit, AWRITE );

}

spabt( ctl, unit )
register struct sctl *ctl;
{

	switch ( ctl->sc_pspot ) {

	case STDPWRITE:
		siclean( ctl, unit );
		clrbufs( ctl, unit, APWRITE );
		ctl->sc_notpok = 0;
		wakeup( (caddr_t)&ctl->sc_notpok );
		break;

	}

	rsetabort( ctl, unit, APWRITE );

}

siabt( ctl, unit )
register struct sctl *ctl;
{

	register struct sctltp *ctltp;
	ctltp = &ctl->tp_un[unit];
	switch ( ctltp->sc_ispot ) {

	case STDIOCTL:
		siclean( ctl, unit );
		clrbufs( ctl, unit, AIOCTL );
		ctltp->sc_tstat |= IOK;
		wakeup( (caddr_t)ctltp );
		break;

	}

	rsetabort( ctl, unit, AIOCTL );

}

siclean( ctl, unit )
struct sctl	*ctl;
{
	register struct sctltp *ctltp;
	register struct	scb *scb;
	register struct	cleanlcb *clcb;

	ctltp = &ctl->tp_un[unit];
	ctltp->sc_tstat |= INCLEAN;
	scb = (struct scb *)smalloc( sizeof( struct scb ) );
	scb->si_cmd = DCLEAN;
	scb->si_cblen = sizeof(struct scb);
	clcb = (struct cleanlcb *)scb->si_un.gdc5.si_parm;
	clcb->clr_procp = u.u_procp;
	clcb->clr_pid = u.u_procp->p_pid;
	clcb->clr_dev = unit;
	clcb->clr_wuadx = (int) scb;
	spl7();
	qsqo( scb, ctl, (PZERO-10) );
	sleep( (caddr_t)scb, (PZERO - 10 ) );
	spl0();
	ctltp->sc_tstat &= ~INCLEAN;
	smfree( sizeof(struct scb), scb );

}

clrbufs( ctl, unit, func )
register struct sctl *ctl;
{

	register struct sctltp *ctltp;
	register s;

	ctltp = &ctl->tp_un[unit];
	switch ( func ) {

	case AOPEN:

		if ( ctltp->sc_oscbbuf != 0 ) {
			smfree( sizeof(struct scb), ctltp->sc_oscbbuf );
			ctltp->sc_oscbbuf = 0;
		}
		if ( ctltp->sc_ortnbuf != 0 ) {
			smfree( sizeof(struct openrtn), ctltp->sc_ortnbuf );
			ctltp->sc_ortnbuf = 0;
		}
		break;
	
	case ACLOSE:

		if ( ctltp->sc_cscbbuf != 0 ) {
			smfree( sizeof(struct scb), ctltp->sc_cscbbuf );
			ctltp->sc_cscbbuf = 0;
		}
		if ( ctltp->sc_crtnbuf != 0 ) {
			smfree( sizeof(struct closertn), ctltp->sc_crtnbuf );
			ctltp->sc_crtnbuf = 0;
		}
		break;

	case AREAD:

		if ( ctltp->sc_rscbbuf != 0 ) {
			smfree( sizeof(struct scb), ctltp->sc_rscbbuf );
			ctltp->sc_rscbbuf = 0;
		}
		if ( ctltp->sc_rrtnbuf != 0 ) {
			smfree( sizeof(struct readrtn), ctltp->sc_rrtnbuf );
			ctltp->sc_rrtnbuf = 0;
		}
		if ( ctltp->sc_rbufs != 0 ) {
#ifdef VPMSYS
			smfree( VRBUFSIZE, ctltp->sc_rbufs );
#else
			smfree( RBUFSIZE, ctltp->sc_rbufs );
#endif
			ctltp->sc_rbufs = 0;
		}
		break;

	case AWRITE:
		/* disable any pending write flush */
/* added */	s = spl7();
/* added */	if(ctltp->sc_timstate) ctltp->sc_timstate = -1;
/* added */	splx(s);

		if ( ctltp->sc_wscbbuf != 0 ) {
			smfree( sizeof(struct scb), ctltp->sc_wscbbuf );
			ctltp->sc_wscbbuf = 0;
		}
		if ( ctltp->sc_wrtnbuf != 0 ) {
			smfree( sizeof(struct writertn), ctltp->sc_wrtnbuf );
			ctltp->sc_wrtnbuf = 0;
		}
		if ( ctltp->sc_wbufs != 0 ) {
#ifdef VPMSYS
			smfree( VWBUFSIZE, ctltp->sc_wbufs );
#else
			smfree( WBUFSIZE, ctltp->sc_wbufs );
#endif
			ctltp->sc_wbufs = 0;
		}
		break;
			
	case APWRITE:

		if ( ctl->sc_pscbbuf != 0 ) {
			smfree( sizeof(struct scb), ctl->sc_pscbbuf );
			ctl->sc_pscbbuf = 0;
		}
		if ( ctl->sc_prtnbuf != 0 ) {
			smfree( sizeof(struct writertn), ctl->sc_prtnbuf );
			ctl->sc_prtnbuf = 0;
		}
		if ( ctl->sc_pbufs != 0 ) {
#ifdef VPR
			smfree( 4*WBUFSIZE, ctl->sc_pbufs );
#else
			smfree( WBUFSIZE, ctl->sc_pbufs );
#endif
			ctl->sc_pbufs = 0;
		}
		break;
			
	case AIOCTL:

		if ( ctltp->sc_iscbbuf != 0 ) {
			smfree( sizeof(struct scb), ctltp->sc_iscbbuf );
			ctltp->sc_iscbbuf = 0;
		}
		if ( ctltp->sc_irtnbuf != 0 ) {
			smfree( sizeof(struct ioctlrtn), ctltp->sc_irtnbuf );
			ctltp->sc_irtnbuf = 0;
		}
		if ( ctltp->sc_ibuf != 0 ) {
			smfree( ctltp->sc_ibuflen, ctltp->sc_ibuf );
			ctltp->sc_ibuf = 0;
		}
		break;

	}


}

/*
 * do a command to the polling command buffer
 */

docmd( pblk )
register struct	cmd	*pblk;
{
	
	pblk->c_gate = 0xff;
	spl6();
	while ( pblk->c_gate == 0xff ) {
		sleep( (caddr_t) pblk, SIOCPRI );
	}
	spl0();
}

/*
 * Open failed routine for recognizing SIOC load failures.
 */

#ifndef	OVKRNL
/* note: anything changed between these #if statments must be changed in
	 iotimo.c also
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
#endif

/*
 * SIOC-0 controller interrupt
 */

siint0()
{

	if (sctl[0].sc_state == RUN) {
		sioci(&sctl[0]);
	} else if (sctl[0].sc_state == DNLD) {
		out_multibus(PSIOC0, CLRINT);
		wakeup((caddr_t) sctl[0].sc_paddr );
	} else {
	}
}


/*
 * SIOC-1 controller interrupt
 */

siint1()
{

	if (sctl[1].sc_state == RUN) {
		sioci(&sctl[1]);
	} else if (sctl[1].sc_state == DNLD) {
		out_multibus(PSIOC1, CLRINT);
		wakeup((caddr_t) sctl[1].sc_paddr );
	} else {
	}
}


/*
 * SIOC-2 controller interrupt
 */

siint2()
{

	if (sctl[2].sc_state == RUN) {
		sioci(&sctl[2]);
	} else if (sctl[2].sc_state == DNLD) {
		out_multibus(PSIOC2, CLRINT);
		wakeup((caddr_t) sctl[2].sc_paddr );
	} else {
	}
}


/*
 * SIOC-3 controller interrupt
 */

siint3()
{

	if (sctl[3].sc_state == RUN) {
		sioci(&sctl[3]);
	} else if (sctl[3].sc_state == DNLD) {
		out_multibus(PSIOC3, CLRINT);
		wakeup((caddr_t) sctl[3].sc_paddr );
	} else {
	}
}


/*
 * SIOC-4 controller interrupt
 */

siint4()
{

	if (sctl[4].sc_state == RUN) {
		sioci(&sctl[4]);
	} else if (sctl[4].sc_state == DNLD) {
		out_multibus(PSIOC4, CLRINT);
		wakeup((caddr_t) sctl[4].sc_paddr );
	} else {
	}
}


#ifndef	OVKRNL
/* note: anything changed between these #if statments must be changed in
	 iotimo.c also
*/
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
#endif

siinit()
{
	register int	i;
	register int	j;
	register int	icp;
#ifndef	OVKRNL
/* note: anything changed between these #if statments must be changed in
	 iotimo.c also
*/
	int	selftestdone();
	int	gochanatn();
#else
	extern	int	selftestdone();
	extern	int	gochanatn();
#endif
	extern	mbusto;
	extern	initstat;

#if NUMICP >= 1
	sctl[0].sc_iaddr = (int *) PBASE0 + (sizeof(struct cmd)/2);
#endif
#if NUMICP >= 2
	sctl[1].sc_iaddr = (int *) PBASE1 + (sizeof(struct cmd)/2);
#endif
#if NUMICP >= 3
	sctl[2].sc_iaddr = (int *) PBASE2 + (sizeof(struct cmd)/2);
#endif
#if NUMICP >= 4
	sctl[3].sc_iaddr = (int *) PBASE3 + (sizeof(struct cmd)/2);
#endif
#if NUMICP >= 5
	sctl[4].sc_iaddr = (int *) PBASE4 + (sizeof(struct cmd)/2);
#endif

#if NUMICP >= 1
	sctl[0].sc_oaddr = (int *) sctl[0].sc_iaddr + (sizeof(struct scb)/2);
#endif
#if NUMICP >= 2
	sctl[1].sc_oaddr = (int *) sctl[1].sc_iaddr + (sizeof(struct scb)/2);
#endif
#if NUMICP >= 3
	sctl[2].sc_oaddr = (int *) sctl[2].sc_iaddr + (sizeof(struct scb)/2);
#endif
#if NUMICP >= 4
	sctl[3].sc_oaddr = (int *) sctl[3].sc_iaddr + (sizeof(struct scb)/2);
#endif
#if NUMICP >= 5
	sctl[4].sc_oaddr = (int *) sctl[4].sc_iaddr + (sizeof(struct scb)/2);
#endif

#if NUMICP >= 1
	sctl[0].sc_paddr = (int *) PBASE0;
#endif
#if NUMICP >= 2
	sctl[1].sc_paddr = (int *) PBASE1;
#endif
#if NUMICP >= 3
	sctl[2].sc_paddr = (int *) PBASE2;
#endif
#if NUMICP >= 4
	sctl[3].sc_paddr = (int *) PBASE3;
#endif
#if NUMICP >= 5
	sctl[4].sc_paddr = (int *) PBASE4;
#endif

#if NUMICP >= 1
	sctl[0].sc_port = (int *)PSIOC0;
#endif
#if NUMICP >= 2
	sctl[1].sc_port = (int *)PSIOC1;
#endif
#if NUMICP >= 3
	sctl[2].sc_port = (int *)PSIOC2;
#endif
#if NUMICP >= 4
	sctl[3].sc_port = (int *)PSIOC3;
#endif
#if NUMICP >= 5
	sctl[4].sc_port = (int *)PSIOC4;
#endif

	for (j=0; j<NUMICP; j++)
		sctl[j].sc_state = IDLE;

	for (j=0; j<NUMICP; j++) {
		for (i=0; i<10; i++)
/* altered */		sctl[j].tp_un[i].sc_tstat = ROK|WOK|IOK|OOK|COK;
	}

	/* cause trap.c to increment mbusto on multibus timeout */
	initstate = 1;
	/* setup vector of configured icps */
	for(icp=0; icp<NUMICP; icp++) {
		/* reset timeout flag */
		mbusto = 0;
		/* try to reset icp */
		out_multibus(sctl[icp].sc_port, RESET);
		/* check if there has been a multibus time out */
		legal_icp[icp] = !mbusto;
		/* icp was found?? */
		if(!mbusto) {
/*			printf("icp%d found\n",icp); */
			/* wait for self test to complete */
			sctl[icp].sc_state = SELFTEST;
			timeout(selftestdone, &sctl[icp], 1000);
		}
	}
	/* reenable normal multibus timeout handling */
	initstate = 0;


	/* Enable multibus interrupts for each of the discovered ICPs */

#if NUMICP >= 1
	if(legal_icp[0]) {
		out_local( P_SPIC1, (in_local( P_SPIC1 ) & (~SIINT0)) );
		timeout( gochanatn, &sctl[0], 1000 );
	}
#endif

#if NUMICP >= 2
	if(legal_icp[1]) {
		out_local( P_SPIC1, (in_local( P_SPIC1 ) & (~SIINT1)) );
		timeout( gochanatn, &sctl[1], 1000 );
	}
#endif

#if NUMICP >= 3
	if(legal_icp[2]) {
		out_local( P_SPIC1, (in_local( P_SPIC1 ) & (~SIINT2)) );
		timeout( gochanatn, &sctl[2], 1000 );
	}
#endif

#if NUMICP >= 4
	if(legal_icp[3]) {
		out_local( P_SPIC1, (in_local( P_SPIC1 ) & (~SIINT3)) );
		timeout( gochanatn, &sctl[3], 1000 );
	}
#endif

#if NUMICP >= 5
	if(legal_icp[4]) {
		out_local( P_SPIC1, (in_local( P_SPIC1 ) & (~SIINT4)) );
		timeout( gochanatn, &sctl[4], 1000 );
	}
#endif

}

#ifndef	OVKRNL
/* note: anything changed between these #if statments must be changed in
	 iotimo.c also
*/
selftestdone(ctl)
struct sctl *ctl;
{
	ctl->sc_state = TESTDONE;
	wakeup((caddr_t) ctl);
}
#endif


#if CON
/*
 * putchar - prints a character on the console.
 *
 * Since the console is driven by the SIOC, this routine just puts a 
 * character is a special communications area in the multibus
 * accessable memory.  The SIOC periodically looks at this area and
 * sends the character to the console.
 */


char	*msgbufp = msgbuf;		/* next saved printf character */

putchar( c )
register	c;
{
	long	timo;
	register	struct	cmd	*pblk;

	pblk = (struct cmd *)sctl0.sc_paddr;
	/*
	 * put character into msgbuf in case it doesn't get to the console.
	 */
	if ( c != '\0' && c != '\r' && c != 0177 ) {
		*msgbufp++ = c;
		if (msgbufp >= &msgbuf[MSGBUFS]) {
			msgbufp = msgbuf;
		}
	}
	/*
	 * try to wait for last character to be accepted by the SIOC.
	 * after wait, try to pass it anyway.
	 */
	timo = 300000;
	while (pblk->c_gate != 0) {
		if (--timo == 0) {
			break;
		}
	}
	if (c == 0) {
		return;
	}
	pblk->c_char = c;
	pblk->c_command = WCHAR;
	pblk->c_flags = 0;
	pblk->c_gate = 0xff;
	if (c == '\n') {
		putchar( '\r' );
	}
	putchar( 0 );
}

getchar()
{
}
/*

	register struct	cmd	*pblk;

	pblk = (struct cmd *)sctl0.sc_paddr;

	while ( pblk->c_gate != 0 ) {
	}

	pblk->c_command = RCHAR;
	pblk->c_flags = 0;
	pblk->c_char = 0;
	pblk->c_gate = 0xff;

	while ( pblk->c_gate != 0 ) {
	}

	return( pblk->c_char );

}

 */
charrdy()
{
}
/*

	struct	cmd	*pblk;

	pblk = (struct cmd *)sctl0.sc_paddr;

	while ( pblk->c_gate != 0 ) {
	}

	pblk->c_command = CRDY;
	pblk->c_flags = 0;
	pblk->c_gate = 0xff;

	while ( pblk->c_gate != 0 ) {
	}

	return( pblk->c_flags );
}
 */
#endif

#ifndef	OVKRNL
/* note: anything changed between these #if statments must be changed in
	 iotimo.c also
*/
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
		timeout(siwflsh, ctlu, 2);
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
		timeout(siwflsh, ctlu, 2);
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
			timeout( siwflsh, ctlu, 2 );
		}
	}
}
#endif
