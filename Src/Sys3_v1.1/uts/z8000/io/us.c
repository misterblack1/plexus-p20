#include "sys/plexus.h"
#include "sys/param.h"
#include "sys/conf.h"
#include "sys/dir.h"
#include "sys/file.h"
#include "sys/maxuser.h"
#include "sys/systm.h"
#include "sys/sysinfo.h"
#include "sys/tty.h"
#include "sys/usart.h"
#include "sys/user.h"

/*
 * This is a driver for the 2661 usart resident on the cpu board.
 * The driver is patterned after the std v7 dh.c driver.
 */

/* general constants */

#define	NUMUNITS	1		/* number of units on controller */
#define	TURNON		0		/* turn on modem lines */
#define	TURNOFF		1		/* turn off modem lines */

/* static data */


struct tty	us_tty[NUMUNITS];
char	rwstate[NUMUNITS];
int	us_cnt = NUMUNITS;
int	numuser;	/* number of active users r/w */
int	usproc();
#ifndef	OVKRNL
/* note: Anything changed between these #if statements must be changed 
	 in iotimo.c also.
*/
int	ttrstrt();
#else
extern	ttrstrt();
#endif

int	mode2[] =	/* mode 2 value for each baud rate */
{
	0,			/* hang up */
	(II1x1x | BR50   ),	/* 50 baud */
	(II1x1x | BR75   ),	/* 75 baud */
	(II1x1x | BR110  ),	/* 110 baud */
	(II1x1x | BR134  ),	/* 134.5 baud */
	(II1x1x | BR150  ),	/* 150 baud */
	(II1x1x | BR200  ),	/* 200 baud */
	(II1x1x | BR300  ),	/* 300 baud */
	(II1x1x | BR600  ),	/* 600 baud */
	(II1x1x | BR1200 ),	/* 1200 baud */
	(II1x1x | BR1800 ),	/* 1800 baud */
	(II1x1x | BR2400 ),	/* 2400 baud */
	(II1x1x | BR4800 ),	/* 4800 baud */
	(II1x1x | BR9600 ),	/* 9600 baud */
	(II1x1x | BR19200),	/* EXT A (19.2 k baud) */
	0			/* EXT B (speed from diag switches) */
};
char diagspeed[8] = 
{
	3,	/* 0 = 110 baud */
	7,	/* 1 = 300 */
	8,	/* 2 = 600 */
	9,	/* 3 = 1200 */
	11,	/* 4 = 2400 */
	12,	/* 5 = 4800 */
	13,	/* 6 = 9600 */
	14,	/* 7 = 19200 */
};


/*
 * Open a line.
 */

usopen(dev, flag)
{
	register struct tty *tp;
	extern usproc();

	if (dev >= us_cnt) {
		u.u_error = ENXIO;
		return;
	}
	tp = &us_tty[dev];
	spl5();
	if (!rwstate[dev] && ((flag & FREAD|FWRITE) == (FREAD|FWRITE))) {
		if (numuser >= MAXUSER) {
			u.u_error = EINVAL;
			return;
		}
		numuser++;
		rwstate[dev]++;
	}
	spl0();
	if ((tp->t_state&(ISOPEN|WOPEN)) == 0) {
		out_local(P_USCMD, B_RXEN);
		out_local(P_MPIC1, in_local(P_MPIC1) & (~TTYINT)); /*allow int*/
		ttinit(tp);
		tp->t_iflag |= IXON|ICRNL|ISTRIP|IGNPAR|BRKINT;
		tp->t_oflag |= OPOST|ONLCR|TAB3;
		tp->t_lflag |= ISIG|ICANON|ECHO|ECHOK;
		tp->t_proc = usproc;
		usparam(dev);
	}
	spl5();
	if ((tp->t_cflag&CLOCAL) || (usmctl(dev,TURNON) & B_DCD))
		tp->t_state |= CARR_ON;
	if(!(flag&FNDELAY))
		while ((tp->t_state & CARR_ON) == 0) {
			tp->t_state |= WOPEN;
			sleep((caddr_t) &tp->t_canq, TTIPRI);
		}
	(*linesw[tp->t_line].l_open)(tp);
	spl0();
}

usclose(dev)
{
	register struct tty *tp;

	tp = &us_tty[dev];
	(*linesw[tp->t_line].l_close)(tp);
	if (tp->t_cflag&HUPCL) {
		spl5();
		usmctl(dev, TURNOFF);
		spl0();
	}
	if(rwstate[dev])
		numuser--;
	rwstate[dev] = 0;
}

usread(dev)
{
	register struct tty *tp;

	tp = &us_tty[dev];
	(*linesw[tp->t_line].l_read)(tp);
}

uswrite(dev)
{
	register struct tty *tp;

	tp = &us_tty[dev];
	(*linesw[tp->t_line].l_write)(tp);
}

usioctl(dev, cmd, arg, mode)
register dev;
{
	switch(cmd) {
	default:
		if (ttiocom(&us_tty[dev], cmd, arg, mode))
			usparam(dev);
	}
}

usparam(dev)
{
	register struct tty *tp;
	register flags, mode1;
	register savepri;
	register speed;

	tp = &us_tty[dev];
	flags = tp->t_cflag;
	mode1 = BAUDX1;
	if (flags&CS6)
		mode1 |= BITS6;
	if (flags&CS7)
		mode1 |= BITS7;
	if (flags&CS8)
		mode1 |= BITS8;
	if (flags&PARENB) {
		mode1 |= PENABLE;
		if ((flags&PARODD) == 0)
			mode1 |= EVENPAR;
	}
	if (flags&CSTOPB)
		mode1 |= TWOSB;
	else
		mode1 |= ONESB;
	savepri = spl5();
	if ((flags&CBAUD) == 0) {
		/* Hang up line */
		if (tp->t_state&CARR_ON) {
			usmctl(dev, TURNOFF);
		}
		splx(savepri);
		return;
	}
	out_local(P_USCTL, mode1);
	speed = ((tp->t_cflag&CBAUD) != EXTB) ?
		(tp->t_cflag&CBAUD) : diagspeed[diagswits & 0x7];
	out_local(P_USCTL, mode2[speed]);
	spl0();
}

/*
 * This procedure is called when an interrupt occurs from
 * the 2661 usart. The routine queries the usart chip
 * and calls the proper interrupt routine.
 */

usint(dev)
dev_t	dev;
{
	register	status;

	status = in_local(P_USSTAT);
	if (status & B_RXRDY) {
		usrint(dev);
	} else if (status & B_TXRDY) {
		usxint(dev);
	} else {
		usmint(dev);
	}
}

usrint(dev)
{
	register struct tty *tp;
	register c;
	register status;

	sysinfo.rcvint++;
	tp = &us_tty[dev];
	status = in_local(P_USSTAT);
	c = in_local(P_USDATA) & 0377;
	if (!(tp->t_state&(ISOPEN|WOPEN))) {
		return;
	}
	if (status & B_PE) {
		out_local(P_USCMD, in_local(P_USCMD) | B_RERR);
		c |= PERROR;
	}
	if (status & B_FE) {
		out_local(P_USCMD, in_local(P_USCMD) | B_RERR);
		c |= FRERROR;
	}
	if (status & B_OR) {
		out_local(P_USCMD, in_local(P_USCMD) | B_RERR);
		c |= OVERRUN;
	}
	(*linesw[tp->t_line].l_input)(tp, c, 0);
}

/*
 * Transmitter interrupt.
 */
usxint(dev)
{
	register struct tty *tp;
	register ttybit, bar;
	short *sbar;

	sysinfo.xmtint++;
	tp = &us_tty[dev];
	if (tp->t_state&BUSY) {
		tp->t_state &= ~BUSY;
		tp->t_buf->c_first++; 
		out_local(P_USCMD, in_local(P_USCMD) & ~B_TXEN);
	}
	if (tp->t_state&TTXON) {
		tp->t_state &= ~TTXON;
		out_local(P_USDATA, CSTART);
		out_local(P_USCMD, in_local(P_USCMD) | B_TXEN);
		tp->t_state |= BUSY;
		return;
	}
	if (tp->t_state&TTXOFF) {
		tp->t_state &= ~TTXOFF;
		out_local(P_USDATA, CSTOP);
		out_local(P_USCMD, in_local(P_USCMD) | B_TXEN);
		tp->t_state |= BUSY;
		return;
	}
	usproc(tp, T_OUTPUT);
}

usproc(tp, cmd)
register struct tty *tp;
{
	register n;
	register savepri;
	register char	*cp;
	int	dev, ttybit;
	extern ttrstrt();

	dev = tp - us_tty;
	switch(cmd) {

	case T_TIME:
		out_local(P_USCMD, in_local(P_USCMD) & (~B_BRK));
		tp->t_state &= ~TIMEOUT;
		goto start;

	case T_WFLUSH:
	case T_RESUME:
		tp->t_state &= ~TTSTOP;
		goto start;

	case T_OUTPUT:
	start:
		if (tp->t_state&(TIMEOUT|TTSTOP|BUSY))
			break;
		if (tp->t_buf && tp->t_buf->c_first >= tp->t_buf->c_last) {
			putcf(tp->t_buf);
			tp->t_buf = NULL;
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
		n = 1;	/* 2661 can only handle one char at at time */
		cp = &tp->t_buf->c_data[tp->t_buf->c_first];
		if ((tp->t_oflag&OPOST) && ((*cp & 0377) == 0200)) {
			if ((tp->t_buf->c_first+1) == tp->t_buf->c_last) {
				n = getc(&tp->t_outq);
				*cp = n;
			} else {
				tp->t_buf->c_first++;
				cp++;
				n = *cp & 0377;
			}
			if (n > 0200) {
				tp->t_buf->c_first++;
				timeout(ttrstrt, tp, (n&0177)+6);
				tp->t_state |= TIMEOUT;
				goto dd;
			}
			n = 1;
		}
		out_local(P_USDATA, *cp&0377);
		out_local(P_USCMD, in_local(P_USCMD) | B_TXEN);
		tp->t_state |= BUSY;
	dd:
		if (tp->t_state&OASLP &&
		    tp->t_outq.c_cc <= ttlowat[tp->t_cflag&CBAUD]) {
			tp->t_state &= ~OASLP;
			wakeup((caddr_t)&tp->t_outq);
		}
		break;

	case T_SUSPEND:
		tp->t_state |= TTSTOP;
		break;

	case T_BLOCK:
		tp->t_state |= TBLOCK;
		tp->t_state &= ~TTXON;
		if (tp->t_state&BUSY) {
			tp->t_state |= TTXOFF;
		} else {
			tp->t_state |= BUSY;
			out_local(P_USDATA, CSTOP);
			out_local(P_USCMD, in_local(P_USCMD) | B_TXEN);
		}
		break;

	case T_RFLUSH:
		if (!(tp->t_state&TBLOCK))
			break;

	case T_UNBLOCK:
		tp->t_state &= ~(TTXOFF|TBLOCK);
		if (tp->t_state&BUSY) {
			tp->t_state |= TTXON;
		} else {
			tp->t_state |= BUSY;
			out_local(P_USDATA, CSTART);
			out_local(P_USCMD, in_local(P_USCMD) | B_TXEN);
		}
		break;

	case T_BREAK:
		out_local(P_USCMD, in_local(P_USCMD) | B_BRK);
		tp->t_state |= TIMEOUT;
		timeout(ttrstrt, tp, HZ/4);
		break;
	}
}


/*
 * This procedure is used to change modem control signals.
 */

usmctl(dev, control)
dev_t	dev;
int	control;
{
	register	savepri;

	savepri = spl5();
	switch (control) {

	case TURNON:
		out_local(P_USCMD, in_local(P_USCMD) | B_RTS|B_DTR);
		break;

	case TURNOFF:
		out_local(P_USCMD, in_local(P_USCMD) & ~(B_RTS|B_DTR));
		break;
	}
	splx(savepri);
	return(in_local(P_USSTAT));
}


/*
 * This routine handles a change in a modem control line.
 */

usmint(dev)
dev_t	dev;
{
	register struct tty	*tp;

	tp = &us_tty[dev];
	if (tp->t_cflag&CLOCAL)
		return;
	if (!(tp->t_state&(ISOPEN|WOPEN)))
		return;
	if (in_local(P_USSTAT) & B_DCD) {
		if ((tp->t_state&CARR_ON)==0) {
			wakeup(&tp->t_canq);
			tp->t_state |= CARR_ON;
		}
	} else {
		if (tp->t_state&CARR_ON) {
			if ((tp->t_state&ISOPEN)) {
				signal(tp->t_pgrp, SIGHUP);
				usmctl(dev, TURNOFF);
				ttyflush(tp, FREAD|FWRITE);
			}
			tp->t_state &= ~CARR_ON;
		}
	}
}

isclr()
{
	register dev;
	register struct tty *tp;

	dev = 0;
	tp = &us_tty[dev];
	if ((tp->t_state&(ISOPEN|WOPEN)) == 0)
		return;
	usparam(dev);
	usmctl(dev, TURNON);
	tp->t_state &= ~BUSY;
	usproc(tp, T_OUTPUT);
}

/*
 * msgbufp is a pointer to the next char in the buffer msgbuf which
 * is MSGBUFS chars long. This buffer is circular and contains the
 * most recent characters printed by putchar.
 */

char	*msgbufp = msgbuf;


/*
 * put a character out using polling mode.
 */

putchar(c)
register	c;
{
	register	timo;
	register	cmd;
	register	savepri;

	if (c != '\0' && c != '\r' && c != 0177) {
		*msgbufp++ = c;
		if (msgbufp >= &msgbuf[MSGBUFS]) {
			msgbufp = msgbuf;
		}
	}

	savepri = spl7(); /* turn off ints to protect against xmit int */
	cmd = in_local(P_USCMD);
	out_local(P_USCMD, cmd | B_TXEN);	/* make sure xmitter is on */

	timo = 30000;
	while ((in_local(P_USSTAT) & B_TXRDY) == 0) {
		if (--timo == 0) {
			break;
		}
	}
	if (c == 0) {
		return;
	}
	out_local(P_USDATA, c);
	if (c == '\n') {
		putchar('\r');
	}
	putchar(0);	/* wait for last character to complete */
	out_local(P_USCMD, cmd);	/* restore original command */
	splx(savepri);
}


/*
 * read a character from the usart
 */

getchar()
{
	register c;

	while ((in_local(P_USSTAT) & B_RXRDY) == 0) {
	}
	c = in_local(P_USDATA) & 0x7f;
	if (c == '\r') {
		c = '\n';
	}
	putchar(c);
	return(c);
}

/*
 * return true if there is a character ready to be input
 */

waitchar()
{

	return((in_local(P_USSTAT) & B_RXRDY) != 0);
}
