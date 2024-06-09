#ifdef WAITING
extern int profile[350];
#define PROFILE
#endif
#ifdef COUNT
extern int profile[650];
#define PROFILE
#endif
#include "sys/param.h"
#include "sys/dir.h"
#include "sys/user.h"
#include "sys/tty.h"
#include "sys/ttold.h"
#include "sys/file.h"
#include "sys/conf.h"
#include "sys/ioctl.h"
#include "icp/sio.h"	/* icp specific */
#include "icp/icpinfo.h" /* icp specific */
#ifdef VPMSYS
#include "icp/icp.h"     /* icp specific */
extern int devicearray[];
extern int thisicp;
#endif
#ifdef VPR
#include "sys/vpr.h"
#endif

#ifndef VPMSYS
extern int needcblock;

	/*-----------------*\
        | null clist header |
	\*-----------------*/

struct clist ttnulq;

	/*------------*\
        | canon buffer |
	\*------------*/

char	canonb[CANBSIZ];

	/*---------------------------*\
	| version and release numbers |
	\*---------------------------*/

char icpver = 1;
char icprel = 2;

	/*------------------------------------------------------------*\
	| Input mapping table-- if an entry is non-zero, when the      |
	| corresponding character is typed preceded by "\" the escape  |
	| sequence is replaced by the table value.  Mostly used for    |
	| upper-case only terminals.				       |
	\*------------------------------------------------------------*/

char	maptab[] = {
	000,000,000,000,000,000,000,000,
	000,000,000,000,000,000,000,000,
	000,000,000,000,000,000,000,000,
	000,000,000,000,000,000,000,000,
	000,'|',000,000,000,000,000,'`',
	'{','}',000,000,000,000,000,000,
	000,000,000,000,000,000,000,000,
	000,000,000,000,000,000,000,000,
	000,000,000,000,000,000,000,000,
	000,000,000,000,000,000,000,000,
	000,000,000,000,000,000,000,000,
	000,000,000,000,000,000,'~',000,
	000,'A','B','C','D','E','F','G',
	'H','I','J','K','L','M','N','O',
	'P','Q','R','S','T','U','V','W',
	'X','Y','Z',000,000,000,000,000,
};
#endif

	/*-------------------------------------------------------------*\
	| TTY IOCTL code. Someday may be incorporated with sio software |
	\*-------------------------------------------------------------*/

#ifdef VPMSYS
ttiocom(tp, cmd, arg, lcbbuf,minordev, error)
#else
ttiocom(tp, cmd, arg, lcbbuf, error)
#endif
register struct tty *tp;
int *error;
long lcbbuf;
{
	register flag;
	register struct termio cb;
	register struct sgttyb tb;

#ifdef VPR
	struct vprio vflag;
	extern int ppiod[];
#endif
#ifdef VPMSYS
	char *vptr;
	int unit;
        char array[4];
#endif
#ifdef PROFILE
	char *ptr;
	int cnt;
	extern int profile[650];
#endif
#ifdef COUNT
	profile[21]++;
#endif

	switch(cmd) {
	/*
	 * Couldn't find any reference to IOCTYPE. Nor was any use
	 * discovered in any of the utility software under cmd. For
	 * the time being, it is being commented out.

		case IOCTYPE:
			u.u_rval1 = TIOC;
			break;

	 */

	/* clear process group entry in ttytab entry for this line */
	case CLRPGRP:
		tp->t_pgrp = 0;
		break;	

#ifndef VPMSYS
#ifdef VPR
	case VSETSTATE:
		mvfrommb((caddr_t) &vflag, lcbbuf, 2);
		if(vflag.vpr_odata & VPLOT)
			out_local(ppiod[0], 0);
		if(vflag.vpr_odata & VPRINT)
			out_local(ppiod[0], 1);
		break;
#endif
	case TCSETAW:
	case TCSETAF:
		ttywait(tp);
		if (cmd == TCSETAF)
			ttyflush(tp, (FREAD|FWRITE));
	case TCSETA:
		mvfrommb((caddr_t) &cb, lcbbuf, sizeof(cb));
		if (tp->t_line != cb.c_line) {
			if (cb.c_line < 0 || cb.c_line >= linecnt) {
				*error = EINVAL;
				break;
			}
		}
		tp->t_iflag = cb.c_iflag;
		tp->t_oflag = cb.c_oflag;
		tp->t_cflag = cb.c_cflag;
		tp->t_lflag = cb.c_lflag;
		bcopy(cb.c_xcc, tp->t_cc, NCC);	/*c_xcc for v7 C compiler */
		/*
		 * linecnt is fixed at 0 (system III) hence only legal
		 * line descpline is 0 and t_line will always equal
		 * input line descipline.
		 *
		 * if (tp->t_line != cb.c_line) {
		 *	tp->t_line = cb.c_line;
		 * }
		 */
		return(1);

	case TCGETA:
		cb.c_iflag = tp->t_iflag;
		cb.c_oflag = tp->t_oflag;
		cb.c_cflag = tp->t_cflag;
		cb.c_lflag = tp->t_lflag;
		cb.c_line = tp->t_line;
		bcopy(tp->t_cc, cb.c_xcc, NCC); /*c_xcc for v7 C compiler */
		mvtomb(lcbbuf,(caddr_t) &cb, sizeof(cb));
		break;

	case TCSBRK:
		ttywait(tp);
		if (arg == 0)
			sioproc(tp, T_BREAK);
		break;

	case TCXONC:
		switch (arg) {
		case 0:
			sioproc(tp, T_SUSPEND);
			break;
		case 1:
			sioproc(tp, T_RESUME);
			break;
		case 2:
			sioproc(tp, T_BLOCK);
			break;
		case 3:
			sioproc(tp, T_UNBLOCK);
			break;
		default:
			*error = EINVAL;
		}
		break;

	case TCFLSH:
		switch (arg) {
		case 0:
		case 1:
		case 2:
			ttyflush(tp, (arg - FOPEN)&(FREAD|FWRITE));
			break;

		default:
			*error = EINVAL;
		}
		break;

/* conversion aide only */
	case TIOCSETP:
		ttywait(tp);
		ttyflush(tp, (FREAD|FWRITE));
	case TIOCSETN:
		mvfrommb((caddr_t) &tb, lcbbuf, sizeof(tb));
		tp->t_iflag = 0;
		tp->t_oflag = 0;
		tp->t_lflag = 0;
		tp->t_cflag = (tb.sg_ispeed&CBAUD)|CREAD;
		if ((tb.sg_ispeed&CBAUD)==B110)
			tp->t_cflag |= CSTOPB;
		tp->t_cc[VERASE] = tb.sg_erase;
		tp->t_cc[VKILL] = tb.sg_kill;
		flag = tb.sg_flags;
		if (flag&O_HUPCL)
/*			tp->t_cflag |= HUPCL;	/* V6-S3 compatibility */
			tp->t_iflag |= IXOFF;	/* V7-S3 compatibility */
/*
 * V6-S3 compatibility 
 * if (flag&O_XTABS)
 *	tp->t_oflag |= TAB3;
 */

		/*--------------------------------------------------*\
		| O_TBDELAY & O_ONAL = 02000|004000 = 006000 = XTABS |
		|      TAB1 & TAB2   = 04000|010000 = 014000 = TAB3  |
		\*--------------------------------------------------*/

		if (flag&O_TBDELAY)
			tp->t_oflag |= TAB1;
		if (flag&O_NOAL)
/*			tp->t_lflag |= ECHOK;	/* V6-S3 compatibility */
			tp->t_oflag |= TAB2;	/* V7-S3 compatibility */

		if (flag&O_LCASE) {
			tp->t_iflag |= IUCLC;
			tp->t_oflag |= OLCUC;
			tp->t_lflag |= XCASE;
		}
		if (flag&O_ECHO)
			tp->t_lflag |= ECHO;
		if (flag&O_CRMOD) {
			tp->t_iflag |= ICRNL;
			tp->t_oflag |= ONLCR;
			if ((flag&O_CR1) == O_CR1)
				tp->t_oflag |= CR1;
			if ((flag&O_CR2) == O_CR1)
				tp->t_oflag |= CR2;
		} else {
			tp->t_oflag |= (ONLCR|ONLRET);
			if ((flag&O_NL1) == O_NL1)
				tp->t_oflag |= CR1;
			if ((flag&O_NL2) == O_NL2)
				tp->t_oflag |= CR2;
		}
		if (flag&O_RAW) {
			tp->t_cc[VTIME] = 1;
			tp->t_cc[VMIN] = 6;
			tp->t_iflag &= ~(ICRNL|IUCLC);
			tp->t_cflag |= CS8;
		} else {
			tp->t_cc[VEOF] = CEOF;
			tp->t_cc[VEOL] = 0;
			tp->t_iflag |= BRKINT|IGNPAR|ISTRIP|IXON;
/*			tp->t_iflag |= BRKINT|IGNPAR|ISTRIP|IXON|IXANY|ICRNL;*/
			tp->t_oflag |= OPOST;
/* CCF -- This next line should turn on parity to be consistent with
	  original system3 source */
			tp->t_cflag |= CS8;
			tp->t_lflag |= ISIG;
			/* adjusted for V7 compatibility (CBREAK) */
			if (flag&O_XTABS) {
			    tp->t_cc[VTIME] = 1;
			    tp->t_cc[VMIN] = 1;
			} else {
			    tp->t_lflag |= ICANON|ECHOK;
			    tp->t_cc[VTIME] = 0;
			    tp->t_cc[VMIN] = 4;
			}
			tp->t_iflag |= INPCK;
			if (flag&O_EVENP)
				tp->t_cflag |= PARENB;
			if (flag&O_ODDP)
				tp->t_cflag |= (PARENB|PARODD);
		}
		if (flag&O_VTDELAY)
			tp->t_oflag |= FFDLY;
		if (flag&O_BSDELAY)
			tp->t_oflag |= BSDLY;
		return(1);

	case TIOCGETP:
		tb.sg_ispeed = tp->t_cflag&CBAUD;
		tb.sg_ospeed = tb.sg_ispeed;
		tb.sg_erase = tp->t_cc[VERASE];
		tb.sg_kill = tp->t_cc[VKILL];
		flag = 0;
		if (tp->t_iflag&IXOFF)
			flag |= O_HUPCL;
		if (!(tp->t_lflag&ICANON)   && (tp->t_iflag&(ICRNL|IUCLC) == 0)
		   &&(tp->t_cc[VTIME] == 1) && (tp->t_cc[VMIN] == 6))
			flag |= O_RAW;
		if (!(tp->t_lflag&ICANON)   &&
		     (tp->t_cc[VTIME] == 1) && (tp->t_cc[VMIN] == 1))
			flag |= O_XTABS;
		if (tp->t_lflag&XCASE)
			flag |= O_LCASE;
		if (tp->t_lflag&ECHO)
			flag |= O_ECHO;
		if (tp->t_cflag&PARODD)
			flag |= O_ODDP;
		else if (tp->t_cflag&PARENB)
			flag |= O_EVENP;
		if (tp->t_oflag&ONLCR) {
			flag |= O_CRMOD;
			if (tp->t_oflag&CR1)
				flag |= O_CR1;
			if (tp->t_oflag&CR2)
				flag |= O_CR2;
		} else {
			if (tp->t_oflag&CR1)
				flag |= O_NL1;
			if (tp->t_oflag&CR2)
				flag |= O_NL2;
		}
		/*--------------------------------------------------*\
		|      TAB1 & TAB2   = 04000|010000 = 014000 = TAB3  |
		| O_TBDELAY & O_ONAL = 02000|004000 = 006000 = XTABS |
		\*--------------------------------------------------*/
		if (tp->t_oflag&TAB1)
			flag |= O_TBDELAY;
		if (tp->t_oflag&TAB2)
			flag |= O_NOAL;

		if (tp->t_oflag&FFDLY)
			flag |= O_VTDELAY;
		if (tp->t_oflag&BSDLY)
			flag |= O_BSDELAY;
		tb.sg_flags = flag;
		mvtomb(lcbbuf,(caddr_t) &tb, sizeof(tb));
		break;

#ifdef PROFILE
	case TCGETP:
		ptr = (char *) profile;
		for(cnt=sizeof(profile); cnt>0; cnt-=64) {
			if(cnt>64)
				mvtomb(lcbbuf,(caddr_t) ptr, 64);
			else
				mvtomb(lcbbuf,(caddr_t) ptr, cnt);
			ptr += 64;
			lcbbuf += 64;
		}
		break;
#endif
#endif

#ifdef VPMSYS
	case VPMCMD:
		tp->t_state |= ISACMD;
		vptr = (char *) tp->v_cmdar;
		mvfrommb((caddr_t) vptr, lcbbuf, 4);
		break;

        case VPMERRS:
		vptr = (char *) tp->v_errs;
		mvtomb(lcbbuf, (caddr_t) vptr, 8);
		break;

        case VPMRPT:
		/* should never get here */
		break;

        case RUNCMD:
		unit = TDEV(minordev);
		devicearray[unit] = BDEV(minordev);
                thisicp = IDEV(minordev);
		tp->pc = 0;

		array[0] = 0;
		array[1] = 0;
		array[2] = 0;
		array[3] = 0;

		mreport(VPMSTUP, minordev, array, MSTRUPL);

		break;

        case HLTCMD:
		unit = TDEV(minordev);
		devicearray[unit] = 4;

		if(tp->v_timerid) {
		     cantimer(tp->v_timerid);
                     tp->v_timerid = 0;
                }
		break;
#endif
	default:
		*error = EINVAL;
		break;
	}
	return(0);
}

	/*-------------------------------------------------*\
	| wait for complete to flush and timeouts to finish |
	\*-------------------------------------------------*/

ttywait(tp)
register struct tty *tp;
{
#ifdef COUNT
	profile[22]++;
#endif

	spl4();

#ifdef VPMSYS
	while (tp->t_outq.c_cc || (tp->t_state&(BUSY))) {
#else
	while (tp->t_outq.c_cc || (tp->t_state&(BUSY|TIMEOUT))) {
#endif
#ifdef WAITING
		profile[3]++;
#endif
		tp->t_state |= TTIOW;
		sleep((caddr_t)&tp->t_oflag, TTOPRI);
	}
	spl0();
	delay(HZ/15);
}

	/*------------------------------------------------------------------*\
	| Flush the TTY queues: either output, or input raw and canonical Qs |
	\*------------------------------------------------------------------*/

ttyflush(tp, cmd)
register struct tty *tp;
{
	register struct cblock *cp;
	register s;

#ifdef COUNT
	profile[23]++;
#endif

	if (cmd&FWRITE) {
		while ((cp = getcb(&tp->t_outq)) != NULL) {
			putcf(cp);
#ifndef VPMSYS
			if(needcblock) {
				needcblock = 0;
				wakeup((caddr_t) &needcblock);
			}
#endif
		}
#ifndef VPMSYS
		sioproc(tp, T_WFLUSH);
#endif
		if (tp->t_state&OASLP) {
			tp->t_state &= ~OASLP;
			wakeup((caddr_t)&tp->t_outq);
		}
		if (tp->t_state&TTIOW) {
			tp->t_state &= ~TTIOW;
			wakeup((caddr_t)&tp->t_oflag);
		}
#ifdef VPMSYS
		wakeup((caddr_t) &cfreelist.c_next);
#endif
	}
	if (cmd&FREAD) {
		while ((cp = getcb(&tp->t_canq)) != NULL) {
			putcf(cp);
#ifndef VPMSYS
			if(needcblock) {
				needcblock = 0;
				wakeup((caddr_t) &needcblock);
			}
#endif
		}
#ifdef VPMSYS
		while ((cp = getcb(&tp->v_rcvholdq)) != NULL)
			putcf(cp);
#endif
		s = spl4();
		while ((cp = getcb(&tp->t_rawq)) != NULL) {
			putcf(cp);
#ifndef VPMSYS
			if(needcblock) {
				needcblock = 0;
				wakeup((caddr_t) &needcblock);
			}
#endif
		}
#ifndef VPMSYS
		tp->t_delct = 0;
#endif
		splx(s);
#ifndef VPMSYS
		sioproc(tp, T_RFLUSH);
		if (tp->t_state&IASLP) {
			tp->t_state &= ~IASLP;
			wakeup((caddr_t)&tp->t_rawq);
		}
#endif
	}
}
#ifndef VPMSYS
	/*-------------------------------------------------*\
	| Transfer raw input list to canonical list,	    |
	| doing erase-kill processing and handling escapes. |
	\*-------------------------------------------------*/

canon(tp)
register struct tty *tp;
{
	register char *bp;
	register c, esc;
#ifndef	VPMSYS
	extern	tttimeo();
#endif

#ifdef COUNT
	profile[24]++;
#endif

	spl4();
	if (tp->t_rawq.c_cc == 0)
		tp->t_delct = 0;
	while (tp->t_delct == 0) {
		if ((tp->t_state&CARR_ON)==0) {
			spl0();
			return;
		}
		if (!(tp->t_iflag&ICANON)) {
			tp->t_state &= ~RTO;
			if (tp->t_rawq.c_cc >= tp->t_cc[VMIN]) {
				tp->t_delct = 1;
				tp->t_state &= ~EXTPROC;
				continue;
			} else if ( (tp->t_cc[VTIME] > 1) ||
				    (tp->t_rawq.c_cc && tp->t_cc[VTIME])) {
				tp->t_state |= EXTPROC;
				if (!(tp->t_state&TACT))
					tttimeo(tp);
			}
		}
		tp->t_state |= IASLP;
#ifdef WAITING
		profile[4]++;
#endif
		sleep((caddr_t)&tp->t_rawq, TTIPRI);
	}
	if (!(tp->t_lflag&ICANON)) {
		tp->t_canq = tp->t_rawq;
		tp->t_rawq = ttnulq;
		tp->t_delct = 0;
		spl0();
		return;
	}
	spl0();
	bp = canonb;
	esc = 0;
	while ((c=getc(&tp->t_rawq)) >= 0) {
		if (!esc) {
			if (c == '\\') {
				esc++;
			} else if (c == tp->t_cc[VERASE]) {
				if (bp > canonb)
					bp--;
				continue;
			} else if (c == tp->t_cc[VKILL]) {
				bp = canonb;
				continue;
			} else if (c == tp->t_cc[VEOF]) {
				break;
			}
		} else {
			esc = 0;
			if (c == tp->t_cc[VERASE] ||
			    c == tp->t_cc[VKILL] ||
			    c == tp->t_cc[VEOF])
				bp--;
			else if (tp->t_lflag&XCASE) {
				if ((c < 0200) && maptab[c]) {
					bp--;
					c = maptab[c];
				} else if (c == '\\')
					continue;
			} else if (c == '\\')
				esc++;
		}
		*bp++ = c;
		if (c == '\n' || c == tp->t_cc[VEOL])
			break;
		if (bp >= &canonb[CANBSIZ])
			bp--;
	}
	tp->t_delct--;
	c = bp - canonb;
	canch += c;
	bp = canonb;
/* faster copy ? */
	while (c--)
		putc(*bp++, &tp->t_canq);
	return;
}

	/*-----------------------------------------------------*\
	| Restart typewriter output following a delay timeout.  |
 	| The name of the routine is passed to the timeout      |
 	| subroutine and it is called during a clock interrupt. |
	\*-----------------------------------------------------*/

ttrstrt(tp)
register struct tty *tp;
{

#ifdef COUNT
	profile[25]++;
#endif

	sioproc(tp, T_TIME);
}

	/*-----------------------------------------------------*\
	| Restart typewriter output following a break (TCSBRK). |
 	| The name of the routine is passed to the timeout      |
 	| subroutine and it is called during a clock interrupt. |
	\*-----------------------------------------------------*/

endbreak(tp)
register struct tty *tp;
{

	extern int wr5sav[];
	extern int psioc[];
	extern struct tty ttytab[];
#ifdef COUNT
	profile[26]++;
#endif

	/* turn off break from this channel */
	out_local(psioc[tp-ttytab],NULLCMD|WR5);
	out_local(psioc[tp-ttytab],wr5sav[tp-ttytab] &= ~SENDBRK);
	sioproc(tp, T_TIME);
}
#endif
