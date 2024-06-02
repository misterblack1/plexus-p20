static char	sccsid[] = "@(#)tty.c	4.9";

# include "saio.h"
# include <sgtty.h>
#include "sys/plexus.h"
#define PSIOC0	0xf800

struct cmd {
	int	sioc_gate;
	int	sioc_command;
	int	sioc_flags;
	int	sioc_char;
	long	sioc_mbadx;
	int	sioc_lcladx;
	int	sioc_len;
};

/* command types */

#define	WBLK	0		/* write block from mb to sioc */
#define	RBLK	1		/* write block from sioc to mb */
#define	JMP	2		/* set sioc pc */
#define	WCHAR	3		/* write character to console */
#define	RCHAR	4		/* read character from console */

/* flags bits */

#define	COMPINT	1		/* sioc causes mb interrupt on completion */

#define SIOC0	((struct cmd *) (PSIOC0))

#define P_USARTDATA	0x7fa0
#define P_USARTSTATUS	0x7fa1
#define B_TXRDY		0x01
#define B_RXRDY		0x02
#define B_DCD		(1 << 6)
#define PIOBCNTRL	0x7f8b
#define PIOBDATA	0x7f89

#define XON	021
#define XOFF	023

int dnld;
int nowait;

# define TTYHOG 256

#ifdef STOCKIII
struct sgttyb _ttstat = { B300, B300, '#', '@', CRMOD|ECHO|XTABS };
#else
struct sgttyb _ttstat = { EXTB, EXTB, '#', '@', CRMOD|ECHO|XTABS };
#endif

#ifdef STOCKIII
static struct {
#else
struct {
#endif
	char	*t_fptr;
	char	*t_lptr;
	char	t_buf[TTYHOG];
} _tt = { _tt.t_buf, _tt.t_buf };

static
_ttputc(c)
register c; {
	if(xttread() == XOFF)
		while(xttread() != XON);
	xttputc(c);
}
static 
xttread() {
	char ch;

	nowait++;
	ch = _ttread();
	nowait = 0;
	return(ch);
}

static
xttputc (c)
register c; {

	register s;
	register unsigned timo;

	register int i;

	timo = 60000;
	/*
	 * Try waiting for the console tty to come ready,
	 * otherwise give up after a reasonable time.
	 */

	if ((in_local(P_USARTSTATUS)&B_DCD) == 0) {
		ledson(SILED);
		while(SIOC0->sioc_gate != 0) {
			if(--timo == 0)
				break;
		}
		ledsoff(SILED);
	}
	else
		while((in_local(P_USARTSTATUS)&B_TXRDY)==0) {
			if(--timo == 0)
				break;
		}

	if(c == -1)
		c = 0;
	else if(c == 0)
		return;

	if ((in_local(P_USARTSTATUS)&B_DCD) == 0) {
		SIOC0->sioc_command = WCHAR;
		SIOC0->sioc_flags = 0;
		SIOC0->sioc_char = c;
		SIOC0->sioc_gate = 0xff;
	}
	else
		out_local(P_USARTDATA,c);

	xttputc(0);
}

#ifndef STOCKIII
short mcc;
#endif
static
_ttcanon (c)
char c; {
	register lcc, flag;
#ifdef STOCKIII
	static mcc;
#endif
	flag = _ttstat.sg_flags;

	switch (c) {

	case '\0':
		return;

	case '\b':
		if (mcc <= 0)
			return;
		_ttputc ('\b');
		--mcc;
		return;
	
	case '\t':
		if (flag & XTABS) {
			lcc = (mcc + 8) & ~7;
			while (mcc++ < lcc)
				_ttputc (' ');
		} else
			_ttputc ('\t');
		return;
	
	case '\n':
		_ttputc ('\n');
		if ((flag & CRMOD) == 0)
			return;
	case '\r':
		_ttputc ('\r');
		mcc = 0;
		return;
	}

	if (c >= ' ' && c < 0177) {
		_ttputc (c);
		++mcc;
	}
}

_ttread (buf, n)
char *buf; {
	register c, cnt, flag;

	if ((cnt = _tt.t_lptr - _tt.t_fptr) == 0 || nowait) {
		flag = _ttstat.sg_flags;
		for (;;) {
			if ((in_local(P_USARTSTATUS)&B_DCD)==0) {
				ledson(SILED);
				while(SIOC0->sioc_gate != 0)
					if (nowait)
						return(-1);
				ledsoff(SILED);
				SIOC0->sioc_command = RCHAR;
				SIOC0->sioc_flags = 0;
				SIOC0->sioc_gate = 0xff;
				ledson(SILED);
				while(SIOC0->sioc_gate != 0);
				ledsoff(SILED);
				c = SIOC0->sioc_char & 0177;
			}
			else {
				while((in_local(P_USARTSTATUS)&B_RXRDY)==0)
					if (nowait)
						return(-1);
				c = in_local(P_USARTDATA) & 0177;
			}
		
			if (c == 0177) {	/* abort on a del */
				_ttcanon('\n');
#ifdef V7
				_exit(127);
#else
				exit(127);
#endif
			}
		
			if (dnld) {
				out_local(PIOBDATA,c&0x0f);
				out_local(PIOBCNTRL,0xcf);
				out_local(PIOBCNTRL,0);
			}

			if (nowait) return(c);

			if (c == '\r' && (flag & CRMOD))
				c = '\n';
			if (flag & ECHO)
				_ttcanon (c);

			if (flag & LCASE) {
				if (c>='A' && c<='Z')
					c -= 'A' - 'a';
				if (_tt.t_lptr > _tt.t_fptr && _tt.t_lptr[-1] == '\\') {
					_tt.t_lptr--;
					if (c>='a' && c<='z')
						c += 'A' - 'a';
					else
						switch (c) {
						case '(':
							c = '{';
							break;
						case ')':
							c = '}';
							break;
						case '!':
							c = '|';
							break;
						case '^':
							c = '~';
							break;
						case '\'':
							c = '`';
							break;
						}
				}
			}
			cnt = _tt.t_lptr - _tt.t_fptr;
			if ((flag & RAW) == 0
			    && (_tt.t_lptr[-1] != '\\' || _tt.t_lptr == _tt.t_fptr)) {
#ifdef STOCKIII
				if (c == _ttstat.sg_erase) {
#else
				if (c == _ttstat.sg_erase ||
				    c == 0x08) {	/* back space */
#endif
					_tt.t_lptr--;
					if (_tt.t_lptr < _tt.t_fptr)
						_tt.t_lptr = _tt.t_fptr;
					continue;
				}
#ifdef STOCKIII
				if (c == _ttstat.sg_kill) {
#else
				if (c == _ttstat.sg_kill || 
				    c == 0x018) { /* ctl - A */
#endif
					_tt.t_lptr = _tt.t_fptr;
					if (flag & ECHO)
						_ttcanon ('\n');
					continue;
				}
				if (c == 04)
					break;
			}
			*_tt.t_lptr++ = c;
			++cnt;
			if (flag & RAW)
				if (cnt >= n)
					break;
				else;
			else
				if (c == '\n')
					break;
			if (_tt.t_lptr < &_tt.t_buf[TTYHOG-1])
				continue;
			if (_tt.t_fptr == _tt.t_buf) {
				_tt.t_buf[TTYHOG-1] = '\n';
				_ttcanon ('\n');
				break;
			}
			strncpy (_tt.t_buf, _tt.t_fptr, cnt);
			_tt.t_lptr = &_tt.t_buf[cnt];
			_tt.t_fptr = _tt.t_buf;
		}
	}
	cnt = cnt>n ? n : cnt;
	strncpy (buf, _tt.t_fptr, cnt);
	if (n < cnt)
		_tt.t_fptr += cnt;
	else
		_tt.t_fptr = _tt.t_lptr = _tt.t_buf;
	return (cnt);
}

_ttwrite (buf, n)
char *buf; {
	register cnt;
	register char *ptr;

	ptr = buf;

	for (cnt=0; cnt<n; cnt++)
		_ttcanon (*ptr++);

	return (n);
}

_prs (str)
char *str; {

	_ttwrite (str, strlen (str));
}
