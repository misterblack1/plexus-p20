/*
 * general TTY subroutines
 */
#include "sys/param.h"
#include "sys/systm.h"
#include "sys/dir.h"
#include "sys/user.h"
#include "sys/tty.h"
#include "sys/ttold.h"
#include "sys/proc.h"
#include "sys/file.h"
#include "sys/conf.h"
#include "sys/ioctl.h"
#include "sys/sysinfo.h"

/*
 * tty low and high water marks
 * high < TTYHOG
 */
int	tthiwat[16] = {
	0, 60, 60, 60,
	60, 60, 60, 120,
	120, 180, 180, 240,
	240, 240, 100, 100,
};
int	ttlowat[16] = {
	0, 20, 20, 20,
	20, 20, 20, 40,
	40, 60, 60, 80,
	80, 80, 50, 50,
};

char	ttcchar[NCC] = {
	CINTR,
	CQUIT,
	CERASE,
	CKILL,
	CEOF,
	0,
	0,
	0
};

/* null clist header */
struct clist ttnulq;

/* canon buffer */
char	canonb[CANBSIZ];
/*
 * Input mapping table-- if an entry is non-zero, when the
 * corresponding character is typed preceded by "\" the escape
 * sequence is replaced by the table value.  Mostly used for
 * upper-case only terminals.
 */
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

/*
 * routine called on first teletype open.
 * establishes a process group for distribution
 * of quits and interrupts from the tty.
 */
ttopen(tp)
register struct tty *tp;
{
	register struct proc *pp;

	pp = u.u_procp;
	if ((pp->p_pid == pp->p_pgrp)
	 && (u.u_ttyp == NULL)
	 && (tp->t_pgrp == 0)) {
		u.u_ttyp = tp;
		tp->t_pgrp = pp->p_pgrp;
	}
	tp->t_state &= ~WOPEN;
	tp->t_state |= ISOPEN;
}

ttclose(tp)
register struct tty *tp;
{
	spl5();
	(*tp->t_proc)(tp, T_RESUME);
	spl0();
	ttyflush(tp, FREAD);
	ttywait(tp);
	tp->t_state &= ~(ISOPEN|WOPEN);
}

/*
 * common ioctl tty code
 */
ttiocom(tp, cmd, arg, mode)
register struct tty *tp;
{
	register flag;
	register struct termio cb;
	register struct sgttyb tb;

	switch(cmd) {
	case IOCTYPE:
		u.u_rval1 = TIOC;
		break;

	/* dummy entry. ICP ver. clears procees group with tty */
	case CLRPGRP:
		break;

	case TCSETAW:
	case TCSETAF:
		ttywait(tp);
		if (cmd == TCSETAF)
			ttyflush(tp, (FREAD|FWRITE));
	case TCSETA:
		if (copyin(arg, &cb, sizeof cb)) {
			u.u_error = EFAULT;
			break;
		}
		if (tp->t_line != cb.c_line) {
			if (cb.c_line < 0 || cb.c_line >= linecnt) {
				u.u_error = EINVAL;
				break;
			}
			(*linesw[tp->t_line].l_ioctl)(tp, LDCLOSE, 0, mode);
		}
		flag = tp->t_lflag;
		tp->t_iflag = cb.c_iflag;
		tp->t_oflag = cb.c_oflag;
		tp->t_cflag = cb.c_cflag;
		tp->t_lflag = cb.c_lflag;
		bcopy(cb.c_xcc, tp->t_cc, NCC);
		if (tp->t_line != cb.c_line) {
			tp->t_line = cb.c_line;
			(*linesw[tp->t_line].l_ioctl)(tp, LDOPEN, 0, mode);
		} else if (tp->t_lflag != flag) {
			(*linesw[tp->t_line].l_ioctl)(tp, LDCHG, flag, mode);
		}
		return(1);

	case TCGETA:
		cb.c_iflag = tp->t_iflag;
		cb.c_oflag = tp->t_oflag;
		cb.c_cflag = tp->t_cflag;
		cb.c_lflag = tp->t_lflag;
		cb.c_line = tp->t_line;
		bcopy(tp->t_cc, cb.c_xcc, NCC);
		if (copyout(&cb, arg, sizeof cb))
			u.u_error = EFAULT;
		break;

	case TCSBRK:
		ttywait(tp);
		if (arg == 0)
			(*tp->t_proc)(tp, T_BREAK);
		break;

	case TCXONC:
		switch (arg) {
		case 0:
			(*tp->t_proc)(tp, T_SUSPEND);
			break;
		case 1:
			(*tp->t_proc)(tp, T_RESUME);
			break;
		case 2:
			(*tp->t_proc)(tp, T_BLOCK);
			break;
		case 3:
			(*tp->t_proc)(tp, T_UNBLOCK);
			break;
		default:
			u.u_error = EINVAL;
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
			u.u_error = EINVAL;
		}
		break;

/* conversion aide only */
	case TIOCSETP:
		ttywait(tp);
		ttyflush(tp, (FREAD|FWRITE));
	case TIOCSETN:
		if (copyin(arg, &tb, sizeof(tb))) {
			u.u_error = EFAULT;
			break;
		}
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
#ifdef V6COMPAT			
			tp->t_cflag |= HUPCL;
#else
			tp->t_iflag |= IXOFF;
#endif
#ifdef V6COMPAT
		if (flag&O_XTABS)
			tp->t_oflag |= TAB3;
#endif
		/*--------------------------------------------------*\
		| O_TBDELAY & O_ONAL = 02000|004000 = 006000 = XTABS |
		|      TAB1 & TAB2   = 04000|010000 = 014000 = TAB3  |
		\*--------------------------------------------------*/
		if (flag&O_TBDELAY)
			tp->t_oflag |= TAB1;
		if (flag&O_NOAL)
#ifdef V6COMPAT
			tp->t_lflag |= ECHOK;
#else
			tp->t_oflag |= TAB2;
#endif

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
			if ((flag&O_CR2) == O_CR2)
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
#ifdef V6COMPAT
			tp->t_iflag |= BRKINT|IGNPAR|ISTRIP|IXON|IXANY|ICRNL;
#else
			tp->t_iflag |= BRKINT|IGNPAR|ISTRIP|IXON;
#endif
			tp->t_oflag |= OPOST;
#ifdef V6COMPAT
			tp->t_cflag |= CS7|PARENB;
#else
		/* This next line was CS7|PARENB in original system3 source, */
		/*  but since v7 turned on parity, used 7 bits, and either */
		/*  odd or even parity was allowed, it is equivalent to */
		/*  eight bits */
			tp->t_cflag |= CS8;
#endif
#ifdef V6COMPAT
			tp->t_lflag |= ICANON|ISIG;
#else
			tp->t_lflag |= ISIG;
#endif
#ifndef V6COMPAT
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
#endif
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
#ifdef V6COMPAT
		if (tp->t_cflag&HUPCL)
#else
		if (tp->t_lflag&IXOFF)
#endif
			flag |= O_HUPCL;
		if (!(tp->t_lflag&ICANON)
#ifndef V6COMPAT
			&& (tp->t_iflag&(ICRNL|IUCLC) == 0)
			&& (tp->t_cc[VTIME] == 1)
			&& (tp->t_cc[VMIN] == 6)
#endif
			)
			flag |= O_RAW;
#ifndef V6COMPAT
		if (!(tp->t_lflag&ICANON) &&
			(tp->t_cc[VTIME] == 1) &&
			(tp->t_cc[VMIN] == 1))
			flag |= O_XTABS;
#endif
		if (tp->t_lflag&XCASE)
			flag |= O_LCASE;
		if (tp->t_lflag&ECHO)
			flag |= O_ECHO;
#ifdef V6COMPAT
		if (!(tp->t_lflag&ECHOK))
			flag |= O_NOAL;
#endif
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
#ifdef V6COMPAT
		if ((tp->t_oflag&TABDLY)==TAB3)
			flag |= O_XTABS;
#endif
		/*--------------------------------------------------*\
		|      TAB1 & TAB2   = 04000|010000 = 014000 = TAB3  |
		| O_TBDELAY & O_ONAL = 02000|004000 = 006000 = XTABS |
		\*--------------------------------------------------*/
		if (tp->t_oflag&TAB1)
			flag |= O_TBDELAY;
#ifndef V6COMPAT
		if (tp->t_oflag&TAB2)
			flag |= O_NOAL;
#endif

		if (tp->t_oflag&FFDLY)
			flag |= O_VTDELAY;
		if (tp->t_oflag&BSDLY)
			flag |= O_BSDELAY;
		tb.sg_flags = flag;
		if (copyout(&tb, arg, sizeof(tb)))
			u.u_error = EFAULT;
		break;

	default:
		if ((cmd&IOCTYPE) == LDIOC)
			(*linesw[tp->t_line].l_ioctl)(tp, cmd, arg, mode);
		else
			u.u_error = EINVAL;
		break;
	}
	return(0);
}

ttinit(tp)
register struct tty *tp;
{
	tp->t_line = 0;
	tp->t_iflag = 0;
	tp->t_oflag = 0;
	tp->t_cflag = SSPEED|CS8|CREAD|HUPCL;
	tp->t_lflag = 0;
	bcopy(ttcchar, tp->t_cc, NCC);
}

ttywait(tp)
register struct tty *tp;
{
	spl5();
	while (tp->t_outq.c_cc || (tp->t_state&(BUSY|TIMEOUT))) {
		tp->t_state |= TTIOW;
		sleep((caddr_t)&tp->t_oflag, TTOPRI);
	}
	spl0();
	delay(HZ/15);
}

/*
 * flush TTY queues
 */
ttyflush(tp, cmd)
register struct tty *tp;
{
	register struct cblock *cp;
	register s;

	if (cmd&FWRITE) {
		while ((cp = getcb(&tp->t_outq)) != NULL)
			putcf(cp);
		(*tp->t_proc)(tp, T_WFLUSH);
		if (tp->t_state&OASLP) {
			tp->t_state &= ~OASLP;
			wakeup((caddr_t)&tp->t_outq);
		}
		if (tp->t_state&TTIOW) {
			tp->t_state &= ~TTIOW;
			wakeup((caddr_t)&tp->t_oflag);
		}
	}
	if (cmd&FREAD) {
		while ((cp = getcb(&tp->t_canq)) != NULL)
			putcf(cp);
		s = spl5();
		while ((cp = getcb(&tp->t_rawq)) != NULL)
			putcf(cp);
		tp->t_delct = 0;
		splx(s);
		(*tp->t_proc)(tp, T_RFLUSH);
		if (tp->t_state&IASLP) {
			tp->t_state &= ~IASLP;
			wakeup((caddr_t)&tp->t_rawq);
		}
	}
}

/*
 * Transfer raw input list to canonical list,
 * doing erase-kill processing and handling escapes.
 */
canon(tp)
register struct tty *tp;
{
	register char *bp;
	register c, esc;

	spl5();
	if (tp->t_rawq.c_cc == 0)
		tp->t_delct = 0;
	while (tp->t_delct == 0) {
		if ((tp->t_state&CARR_ON)==0) {
			spl0();
			return;
		}
		tp->t_state |= IASLP;
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
	sysinfo.canch += c;
	bp = canonb;
/* faster copy ? */
	while (c--)
		putc(*bp++, &tp->t_canq);
	return;
}

#ifndef	OVKRNL
/* note: Anything changed between these #if statements must be changed
	 in iotimo.c also.
*/
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

/*
 * Called from device's read routine after it has
 * calculated the tty-structure given as argument.
 */
ttread(tp)
register struct tty *tp;
{
	register struct clist *tq;

	tq = &tp->t_canq;
	if (tq->c_cc == 0)
		if (u.u_fmode&FNDELAY) {
			if((tp->t_rawq).c_cc)
				canon(tp);
			else
				return;
		}
		else
			canon(tp);
	while (u.u_count!=0 && u.u_error==0) {
		if (u.u_count >= CLSIZE) {
			register n;
			register struct cblock *cp;

			cp = getcb(tq);
			if (cp == NULL)
				break;
			n = min(u.u_count, cp->c_last - cp->c_first);
			if (copyout(&cp->c_data[cp->c_first], u.u_base, n))
				u.u_error = EFAULT;
			putcf(cp);
			u.u_base += n;
			u.u_count -= n;
		} else {
			register c;

			if ((c = getc(tq)) < 0)
				break;
			if (subyte(u.u_base++, c))
				u.u_error = EFAULT;
			u.u_count--;
		}
	}
	if (tp->t_state&TBLOCK) {
		if (tp->t_rawq.c_cc<TTXOLO) {
			(*tp->t_proc)(tp, T_UNBLOCK);
		}
	}
}

/*
 * Called from device's write routine after it has
 * calculated the tty-structure given as argument.
 */
ttwrite(tp)
register struct tty *tp;
{

	if ((tp->t_state&CARR_ON)==0)
		return;
	while (u.u_count) {
		spl5();
		while (tp->t_outq.c_cc > tthiwat[tp->t_cflag&CBAUD]) {
			(*tp->t_proc)(tp, T_OUTPUT);
			tp->t_state |= OASLP;
			sleep((caddr_t)&tp->t_outq, TTOPRI);
		}
		spl0();
		if (u.u_count >= (CLSIZE/2)) {
			register n;
			register struct cblock *cp;

			if ((cp = getcf()) == NULL)
				break;
			n = min(u.u_count, cp->c_last);
			if (copyin(u.u_base, cp->c_data, n)) {
				u.u_error = EFAULT;
				putcf(cp);
				break;
			}
			u.u_base += n;
			u.u_count -= n;
			cp->c_last = n;
			ttxput(tp, cp, n);
		} else {
			register c;

			c = fubyte(u.u_base++);
			if (c<0) {
				u.u_error = EFAULT;
				break;
			}
			u.u_count--;
			ttxput(tp, c, 0);
		}
	}
	spl5();
	(*tp->t_proc)(tp, T_OUTPUT);
	spl0();
}
