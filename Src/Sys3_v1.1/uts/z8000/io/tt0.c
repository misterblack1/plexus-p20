/*
 * Line discipline 0
 */

#include "sys/param.h"
#include "sys/systm.h"
#include "sys/dir.h"
#include "sys/user.h"
#include "sys/file.h"
#include "sys/tty.h"
#include "sys/ioctl.h"
#include "sys/sysinfo.h"

extern char partab[];

/*
 * Place character(s) on raw TTY input queue, putting in delimiters
 * and waking up top half as needed.
 * Also echo if required.
 */
ttin(tp, ucp, ncode)
register struct tty *tp;
union {
	ushort	ch;
	struct cblock *ptr;
} ucp;
{
	register c;
	register flg;
	register unsigned char *cp;
#ifdef	OVKRNL
/* note: Anything changed in between these #if statements must be changed
	 in iotimo.c also.
*/
	extern	tttimeo();
#endif

	flg = tp->t_iflag;
	switch (ncode) {
	case 0:
		ncode++;
		c = ucp.ch;
		if (c&PERROR && !(flg&INPCK))
			c &= ~PERROR;
		if (c&(FRERROR|PERROR|OVERRUN)) {
			if ((c&0377) == 0) {
				if (flg&IGNBRK)
					return;
				if (flg&BRKINT) {
					signal(tp->t_pgrp, SIGINT);
					ttyflush(tp, (FREAD|FWRITE));
					return;
				}
			} else {
				if (flg&IGNPAR)
					return;
			}
			if (flg&PARMRK) {
				ttin(tp, 0377, 1);
				ttin(tp, 0, 1);
			} else
				c = 0;
			c |= 0400;
		} else {
			if (flg&ISTRIP)
				c &= 0177;
			else {
				c &= 0377;
				if (c == 0377 && flg&PARMRK)
					if (putc(0377, &tp->t_rawq))
						return;
			}
		}
		if (flg&IXON) {
			if (tp->t_state&TTSTOP) {
				if (c == CSTART || flg&IXANY)
					(*tp->t_proc)(tp, T_RESUME);
			} else {
				if (c == CSTOP)
					(*tp->t_proc)(tp, T_SUSPEND);
			}
			if (c == CSTART || c == CSTOP)
				return;
		}
		if (c == '\n' && flg&INLCR)
			c = '\r';
		else if (c == '\r')
			if (flg&IGNCR)
				return;
			else if (flg&ICRNL)
				c = '\n';
		if (flg&IUCLC && 'A' <= c && c <= 'Z')
			c += 'a' - 'A';
		ucp.ch = c;
	case 1:
		if (putc(ucp.ch, &tp->t_rawq))
			return;
		sysinfo.rawch++;
		cp = (unsigned char *)&lobyte(ucp.ch);
		break;
	
	default:
		putcb(ucp.ptr, &tp->t_rawq);
		sysinfo.rawch += ncode;
		cp = (unsigned char *)&ucp.ptr->c_data[ucp.ptr->c_first];
		break;
	}
	if (tp->t_rawq.c_cc > TTXOHI) {
		if (flg&IXOFF && !(tp->t_state&TBLOCK))
			(*tp->t_proc)(tp, T_BLOCK);
		if (tp->t_rawq.c_cc > TTYHOG) {
			ttyflush(tp, FREAD);
			return;
		}
	}
	if (tp->t_lflag) while (ncode--) {
		c = *cp++;
		flg = tp->t_lflag;
		if (flg&ISIG) {
			if (c == tp->t_cc[VINTR]) {
				signal(tp->t_pgrp, SIGINT);
				if (!(flg&NOFLSH))
					ttyflush(tp, (FREAD|FWRITE));
				continue;
			}
			if (c == tp->t_cc[VQUIT]) {
				signal(tp->t_pgrp, SIGQUIT);
				if (!(flg&NOFLSH))
					ttyflush(tp, (FREAD|FWRITE));
				continue;
			}
		}
		if (flg&ICANON) {
			if (c == '\n') {
				if (flg&ECHONL)
					flg |= ECHO;
				tp->t_delct++;
			} else if (c == tp->t_cc[VEOL])
				tp->t_delct++;
			if (!(tp->t_state&ESC)) {
				if (c == '\\')
					tp->t_state |= ESC;
				if (c == tp->t_cc[VERASE] && flg&ECHOE) {
					if (flg&ECHO)
						ttxput(tp, '\b', 0);
					flg |= ECHO;
					ttxput(tp, ' ', 0);
					c = '\b';
				} else if (c == tp->t_cc[VKILL] && flg&ECHOK) {
					if (flg&ECHO)
						ttxput(tp, c, 0);
					flg |= ECHO;
					c = '\n';
				} else if (c == tp->t_cc[VEOF]) {
					flg &= ~ECHO;
					tp->t_delct++;
				}
			} else {
				if (c != '\\' || (flg&XCASE))
					tp->t_state &= ~ESC;
			}
		}
		if (flg&ECHO) {
			ttxput(tp, c, 0);
			(*tp->t_proc)(tp, T_OUTPUT);
		}
	}
	if (!(flg&ICANON)) {
		tp->t_state &= ~RTO;
		if (tp->t_rawq.c_cc >= tp->t_cc[VMIN])
			tp->t_delct = 1;
		else if (tp->t_cc[VTIME]) {
			if (!(tp->t_state&TACT))
				tttimeo(tp);
		}
	}
	if (tp->t_delct && (tp->t_state&IASLP)) {
		tp->t_state &= ~IASLP;
		wakeup((caddr_t)&tp->t_rawq);
	}
}

/*
 * Put character(s) on TTY output queue, adding delays,
 * expanding tabs, and handling the CR/NL bit.
 * It is called both from the top half for output, and from
 * interrupt level for echoing.
 */
ttxput(tp, ucp, ncode)
register struct tty *tp;
union {
	ushort	ch;
	struct cblock *ptr;
} ucp;
{
	register c;
	register flg;
	register unsigned char *cp;
	register char *colp;
	int ctype;
	register int cs;
	struct cblock *scf;

	flg = tp->t_oflag;
/* The following condition does not exist in the usart or icp
	if (tp->t_state&EXTPROC)
		flg &= ~OPOST;
*/
	if (ncode == 0) {
		if (tp->t_outq.c_cc >= TTYHOG)
			return;
		ncode++;
		if (!(flg&OPOST)) {
			sysinfo.outch++;
			putc(ucp.ch, &tp->t_outq);
			return;
		}
		cp = (unsigned char *)&lobyte(ucp.ch);
		scf = NULL;
	} else {
		if (!(flg&OPOST)) {
			sysinfo.outch += ncode;
			putcb(ucp.ptr, &tp->t_outq);
			return;
		}
		cp = (unsigned char *)&ucp.ptr->c_data[ucp.ptr->c_first];
		scf = ucp.ptr;
	}
	while (ncode--) {
		c = *cp++;
		if (c >= 0200) {
	/* slp5-0 */
			if (c == 0200)
				putc(0200, &tp->t_outq);
			sysinfo.outch++;
			putc(c, &tp->t_outq);
			continue;
		}
		/*
		 * Generate escapes for upper-case-only terminals.
		 */
		if (tp->t_lflag&XCASE) {
			colp = "({)}!|^~'`\\";
			while(*colp++)
				if (c == *colp++) {
					ttxput(tp, '\\', 0);
					c = colp[-2];
					break;
				}
			if ('A' <= c && c <= 'Z')
				ttxput(tp, '\\', 0);
		}
		if (flg&OLCUC && 'a' <= c && c <= 'z')
			c += 'A' - 'a';
		cs = c;
		/*
		 * Calculate delays.
		 * The numbers here represent clock ticks
		 * and are not necessarily optimal for all terminals.
		 * The delays are indicated by characters above 0200.
		 */
		ctype = partab[c];
		colp = &tp->t_col;
		c = 0;
		switch (ctype&077) {
	
		case 0:	/* ordinary */
			(*colp)++;
	
		case 1:	/* non-printing */
			break;
	
		case 2:	/* backspace */
			if (flg&BSDLY)
				c = 2;
			if (*colp)
				(*colp)--;
			break;
	
		case 3:	/* line feed */
			if (flg&ONLRET)
				goto cr;
			if (flg&ONLCR) {
				if (!(flg&ONOCR && *colp==0)) {
					sysinfo.outch++;
					putc('\r', &tp->t_outq);
				}
				goto cr;
			}
		nl:
			if (flg&NLDLY)
				c = 5;
			break;
	
		case 4:	/* tab */
			c = 8 - ((*colp)&07);
			*colp += c;
			ctype = flg&TABDLY;
			if (ctype == TAB0) {
				c = 0;
			} else if (ctype == TAB1) {
				if (c < 5)
					c = 0;
			} else if (ctype == TAB2) {
				c = 2;
			} else if (ctype == TAB3) {
				sysinfo.outch += c;
				do
					putc(' ', &tp->t_outq);
				while (--c);
				continue;
			}
			break;
	
		case 5:	/* vertical tab */
			if (flg&VTDLY)
				c = 0177;
			break;
	
		case 6:	/* carriage return */
			if (flg&OCRNL) {
				cs = '\n';
				goto nl;
			}
			if (flg&ONOCR && *colp == 0)
				continue;
		cr:
			ctype = flg&CRDLY;
			if (ctype == CR1) {
				if (*colp)
					c = max((*colp>>4) + 3, 6);
			} else if (ctype == CR2) {
				c = 5;
			} else if (ctype == CR3) {
				c = 9;
			}
			*colp = 0;
			break;
	
		case 7:	/* form feed */
			if (flg&FFDLY)
				c = 0177;
			break;
		}
		sysinfo.outch++;
		putc(cs, &tp->t_outq);
		if (c) {
			if ((c < 32) && flg&OFILL) {
				if (flg&OFDEL)
					cs = 0177;
				else
					cs = 0;
				putc(cs, &tp->t_outq);
				if (c > 3)
					putc(cs, &tp->t_outq);
			} else {
				putc(0200, &tp->t_outq);
				putc(c|0200, &tp->t_outq);
			}
		}
	}
	if (scf != NULL)
		putcf(scf);
}

/*
 * Get next function from output queue.
 * Called from xmit interrupt complete.
 */
ttout(tp)
register struct tty *tp;
{
	register c, retval;
	extern ttrstrt();

	if (tp->t_state&(TIMEOUT|TTSTOP|BUSY))
		return(0);
	if (tp->t_state&TTIOW && tp->t_outq.c_cc==0) {
		tp->t_state &= ~TTIOW;
		wakeup((caddr_t)&tp->t_oflag);
	}
	retval = 0;
	while ((c=getc(&tp->t_outq)) >= 0) {
		retval = CPRES;
		if (tp->t_oflag&OPOST && c == 0200) {
			if ((c = getc(&tp->t_outq)) < 0)
				break;
			if (c > 0200) {
				tp->t_state |= TIMEOUT;
				timeout(ttrstrt, tp, (c&0177)+6);
				return(0);
			}
		}
		*((char *)&tp->t_buf) = c;
		break;
	}
	if (tp->t_state&OASLP && tp->t_outq.c_cc<=ttlowat[tp->t_cflag&CBAUD]) {
		tp->t_state &= ~OASLP;
		wakeup((caddr_t)&tp->t_outq);
	}
	return(retval);
}

#ifndef	OVKRNL
/* note: Anything changed in between these #if statements must be changed
	 in iotimo.c also.
*/
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
#endif

/*
 * I/O control interface
 */
ttioctl(tp, cmd, arg, mode)
register struct tty *tp;
{
	ushort	chg;

	switch(cmd) {
	case LDCHG:
		chg = tp->t_lflag^arg;
		if (!(chg&ICANON))
			break;
		spl5();
		if (tp->t_canq.c_cc) {
			if (tp->t_rawq.c_cc) {
				tp->t_canq.c_cc += tp->t_rawq.c_cc;
				tp->t_canq.c_cl->c_next = tp->t_rawq.c_cf;
				tp->t_canq.c_cl = tp->t_rawq.c_cl;
			}
			tp->t_rawq = tp->t_canq;
			tp->t_canq = ttnulq;
		}
		tp->t_delct = tp->t_rawq.c_cc;
		spl0();
		break;

	default:
		break;
	}
}
