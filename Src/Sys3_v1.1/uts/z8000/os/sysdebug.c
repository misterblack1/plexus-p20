#include "sys/plexus.h"
#include "sys/param.h"
#include "sys/systm.h"
#include "sys/dir.h"
#include "sys/user.h"
#include "sys/proc.h"
#include "sys/reg.h"
#include "sys/seg.h"
#include "sys/usart.h"
#include "sys/trap.h"
#include "sys/utsname.h"

#define	MAXREGS	19
#define	MAXBP	10

/*
 * Offsets of the user's registers relative to
 * the saved r0. See reg.h
 */
char	dbgursgs[MAXREGS] =
{
	R0, R1, R2, R3, R4, R5, R6, R7,
	R8, R9, R10, R11, R12, R13, R14, R15,
	FCW, PCSEG, PC,
};

int dbgcd;
char dbgcmd[80];
char *dbgcmdptr;
caddr_t dbgtop;
unsigned dbgppn;
struct bpstruct {
	unsigned adx1;
	unsigned val1;
	unsigned adx2;
	unsigned val2;
#ifdef	OVKRNL
	unsigned	adxseg;
#endif
} dbgbp[MAXBP];


/*
 * trap handler for non vectored interrupt which is used to get into the 
 * resident debugger.
 */

sysdebug(dev, r15, r0, r1, r2, r3, r4, r5, r6, r7, id, fcw, pcseg, pc)
unsigned pc;
{
	register int *regp;
	register int i;
	long j;
	int savespl;
	int saveudseg;
	int savesg;
	register int adx;
	register int len;
	int mapno;
	int cnt;
	int offs;
	int unit;
	int fileno;
	int savepio;
#ifdef	OVKRNL
	int	bpseg;
#endif

#ifndef HWFP
	printf("%s: panic: idle in sysdebug\n",utsname.sysname);
	for (;;)
		idle();
#else
	savespl = spl7();
	dbgtop = (caddr_t) (2 + &pc);  /* save for debugging */
	dbgppn = in_local(P_MUBLK);
	saveudseg = udseg;
	if (dev == NVINT) {
		dbgprintf("\n*** sysdebug interrupt ***\n\n");
	} else if (dev == DBG) {
		dbgprintf("\n*** sysdebug call ***\n\n");
	} else if (dev == UNIMPL) {
		pc -= 2;
#ifndef	OVKRNL
		udseg = SISEG;
#else
		udseg = pcseg + 0x0100;	/* make seg 'I' segment */
#endif
		for (i = 0; i < MAXBP; i++) {
#ifndef	OVKRNL
			if (pc == dbgbp[i].adx1 || pc == dbgbp[i].adx2) {
#else
			if ( (udseg == dbgbp[i].adxseg || dbgbp[i].adxseg
					== SISEG ) &&
			     (pc == dbgbp[i].adx1 || pc == dbgbp[i].adx2) ) {
#endif
				break;
			}
		}
		if (i == MAXBP) {
			dbgprintf("\n*** sysdebug invalid breakpoint ***\n\n");
		} else if (pc == dbgbp[i].adx1) {
			dbgprintf("\n*** sysdebug breakpoint at ");
#ifdef	OVKRNL
			dbgprintf( "(" );
			puthex(udseg);
			dbgprintf( ")" );
#endif
			puthex(pc);
			dbgprintf(" ***\n\n");
			if (dbgbp[i].adx2) {
				suword(dbgbp[i].adx1, dbgbp[i].val1);
				suword(dbgbp[i].adx2, 0x8e00);
			} else {
#ifndef	OVKRNL
				delbp(pc);
#else
				delbp( udseg, pc );
#endif
			}
		} else {
			suword(dbgbp[i].adx2, dbgbp[i].val2);
			suword(dbgbp[i].adx1, 0x8e00);
			goto out;
		}
	} else {
		dbgprintf("\n*** sysdebug for unknown reason ***\n\n");
	}
	udseg = SDSEG;
	while (1) {
		dbgprintf("+ ");
		gets(dbgcmd);
		dbgcmdptr = dbgcmd + 1;
		switch (dbgcmd[0]) {

		case 'R':
			adx = gethex();
			puthex(in_local(adx));
			dbgprintf("\n");
			break;

		case 'I':
			adx = gethex();
			puthex(in_multibus(adx));
			dbgprintf("\n");
			break;

		case 'W':
			adx = gethex();
			i = gethex();
			out_local(adx, i);
			break;

		case 'O':
			adx = gethex();
			i = gethex();
			out_multibus(adx, i);
			break;

		case 'f':
			regp = &r0;
			if (eol())
				adx = regp[R14];
			else
				adx = gethex();
			offs = 0;
			len = 0x20;
			goto dol;
			break;

		case 'l':
			adx = gethex();
			offs = gethex();
			len = gethex();

			dol:
			while (adx) {
				if (xdisplay(adx + offs, len)) {
					break;
				}
				dbgprintf("\n");
				adx = fuword(adx);
			}
			break;

		case 'r':
			adx = gethex();
			len = gethex();
			xdisplay(adx, len);
			break;

		case 'e':
			goto out;
			break;

		case 'x':
			puthex(2 + &pc);
			dbgprintf("\n");
			regp = &r0;
			for (i = 0; i < MAXREGS; i++) {
				puthex(regp[dbgursgs[i]]);
				if ((i == 7) || (i == 15) || (i == 18)) {
					dbgprintf("\n");
				} else {
					dbgprintf(" ");
				}
			}
			break;

		case 'p':
			jptoprom();
			break;

		case 's':
			if (eol()) {
				dbgprintf("current segment: ");
				puthex(udseg);
				dbgprintf("\n");
			} else {
				udseg = gethex();
			}
			break;

		case 'm':
			mapno = gethex();
			for (i = 0; i < NUMLOGPAGE; i++) {
				puthex(i << S_PAGENUM);
				dbgprintf(" (");
				puthex(in_local(mapport(mapno, i)));
				dbgprintf("): ");
				gets(dbgcmd);
				if (dbgcmd[0] == '\177') {
					break;
				} else if (dbgcmd[0] == '\0') {
					continue;
				}
				dbgcmdptr = dbgcmd;
				out_local(mapport(mapno,i), gethex());
			}
			break;

		case 'w':
			adx = gethex();
			for(; ; adx += 2) {
				puthex(adx);
				dbgprintf(" (");
				puthex(fuword(adx));
				dbgprintf("): ");
				gets(dbgcmd);
				if (dbgcmd[0] == '\177') {
					break;
				} else if (dbgcmd[0] == '\0') {
					continue;
				}
				dbgcmdptr = dbgcmd;
				suword(adx, gethex());
			}
			break;

		case 't':
			adx = gethex();
			len = gethex();
			cnt = gethex();
			while (cnt--) {
				if (xdisplay(adx, len)) {
					break;
				}
				dbgprintf("\n");
				adx += len;
			}
			break;

		case 'b':
			if (eol()) {
				for (i = 0; i < MAXBP; i++) {
					if (dbgbp[i].adx1) {
#ifdef	OVKRNL
						puthex(dbgbp[i].adxseg);
						dbgprintf(" ");
#endif
						puthex(dbgbp[i].adx1);
						dbgprintf(" ");
						puthex(dbgbp[i].adx2);
						dbgprintf("\n");
					}
				}
			} else {
				for (i = 0; i < MAXBP; i ++) {
					if (dbgbp[i].adx1 == 0) {
						break;
					}
				}
				if (i == MAXBP) {
					dbgprintf("no more breakpoints\n");
				} else {
#ifdef	OVKRNL
					dbgbp[i].adxseg = gethex();
					if ( eol() ) {
						dbgprintf("usage: b bpseg" );
						dbgprintf(" bpadx1 [ bpadx2 ]");
						dbgprintf("\n");
						break;
					}
#endif
					dbgbp[i].adx1 = gethex();
					dbgbp[i].adx2 = gethex();
					savesg = udseg;
#ifndef	OVKRNL
					udseg = SISEG;
#else
					udseg = dbgbp[i].adxseg;
#endif
					dbgbp[i].val1 = fuword(dbgbp[i].adx1);
					dbgbp[i].val2 = fuword(dbgbp[i].adx2);
					suword(dbgbp[i].adx1, 0x8e00);
					udseg = savesg;
				}
			}
			break;

		case 'd':
			if (eol()) {
				for (i = 0; i < MAXBP; i++) {
					if (dbgbp[i].adx1) {
#ifndef	OVKRNL
						delbp(dbgbp[i].adx1);
#else
						delbp( dbgbp[i].adxseg,
							dbgbp[i].adx1 );
#endif
					}
				}
			} else {
#ifdef	OVKRNL
				bpseg = gethex();
				if ( eol() ) {
					dbgprintf( "usage: d bpseg bpadx\n");
					break;
				}
#endif
				adx = gethex();
#ifndef	OVKRNL
				delbp(adx);
#else
				delbp(bpseg,adx);
#endif
			}
			break;
		}
	}

	out:
	udseg = saveudseg;
	splx(savespl);
	return(0);
#endif
}


/*
 * get a string from the debug terminal
 */

gets(bufp)
register char *bufp;
{
	register char *lp;
	register int c;

#ifdef HWFP
	lp = bufp;
	while (1) {
		c = dbggetc() & 0177;
		switch(c) {

		case '\n':
		case '\r':
			c = '\n';
			*lp++ = '\0';
			return;
			break;

		case '\177':
			*lp++ = '\177';
			*lp++ = '\0';
			dbgputc('\n');
			return;
			break;

		case '\b':
			lp--;
			if(lp < bufp) {
				lp = bufp;
			}
			break;
			
		case '\030':
			lp = bufp;
			dbgputc('\n');
			break;

		default:
			*lp++ = c;
			break;
		}
	}
#endif
}


/*
 * prints an integer value in hex, leading digits 0 pad.
 */

puthex(value)
unsigned value;
{
	static char ptab[] = "0123456789abcdef";
	char buf[5];
	register int i;

#ifdef HWFP
	buf[4] = '\0';
	for (i = 3; i >= 0; i--) {
		buf[i] = ptab[value & 0xf];
		value = value >> 4;
	}
	dbgprintf("%s", buf + i + 1);
#endif
}


/*
 * get the next number from dbgcmd starting at dbgcmdptr. Ignores leading
 * blanks and interprets the number in hex.
 */

gethex()
{
	register c;
	register int total;
	register int sign;

#ifdef HWFP
	while (*dbgcmdptr == ' ') {
		dbgcmdptr++;
	}
	sign = 1;
	total = 0;
	while (1) {
		c = *dbgcmdptr;
		if ((c >= '0') && (c <= '9')) {
			total = (total << 4) + c - '0';
		} else if ((c >= 'a') && (c <= 'f')) {
			total = (total << 4) + c - 'a' + 10;
		} else if (c == '-') {
			sign = -1;
		} else {
			break;
		}
		dbgcmdptr++;
	}
	return(sign * total);
#endif
}

/*
 * returns non zero if there are no more parameters in the dbgcmd string.
 */

eol()
{
	while (*dbgcmdptr == ' ') {
		dbgcmdptr++;
	}
	if (*dbgcmdptr == '\0') {
		return(1);
	} else {
		return(0);
	}
}


/*
 * displays memory starting at adx for len.
 * If len is zero it means go forever.
 */

xdisplay(ad, le)
unsigned ad;
unsigned le;
{
	long inx;
	long adx;
	long len;

#ifdef HWFP
	inx = 0;
	adx = (long) ad;
	len = (le == 0) ? 100000 : (long) le;
	while (inx < len) {
		if ((inx % 16) == 0) {
			if (checkabort()) {
				return(1);
			}
			puthex((unsigned) adx);
			dbgprintf(":");
		}
		dbgprintf(" ");
		puthex(fuword((unsigned) adx));
		if (inx >= (len - 2) || (inx % 16) == 14) {
			dbgprintf("\n");
		}
		inx += 2;
		adx += 2;
	}
	return(0);
#endif
}


/*
 * check for suspend or abort. If suspend just stay in this routine.
 * If abort then return true.
 */

checkabort()
{
	register int c;

#ifdef HWFP
	if (!dbgwaitc()) {
		return(0);
	}
	c = dbggetc();
	if (c == '\023') {
		c = dbggetc();
	}
	if (c == '\177') {
		return(1);
	} else {
		return(0);
	}
#endif
}

/*
 * delete a breakpoint and repair the code.
 */

#ifndef	OVKRNL
delbp(adx)
#else
delbp(seg,adx)
register unsigned seg;
#endif
register unsigned adx;
{
	register int i;
	register int saveudseg;

#ifdef HWFP
	saveudseg = udseg;
#ifndef	OVKRNL
	udseg = SISEG;
#else
	udseg = seg;
#endif
	for (i = 0; i < MAXBP; i++) {
#ifndef	OVKRNL
		if (dbgbp[i].adx1 == adx) {
#else
		if (dbgbp[i].adx1 == adx && dbgbp[i].adxseg == seg) {
#endif
			suword(dbgbp[i].adx1, dbgbp[i].val1);
			if ( dbgbp[i].adx2 )
				suword(dbgbp[i].adx2, dbgbp[i].val2);
			dbgbp[i].adx1 = dbgbp[i].adx2 = 0;
			dbgbp[i].val1 = dbgbp[i].val2 = 0;
		}
	}
	udseg = saveudseg;
#endif
}


/*
 * put a character out to the usart
 */

dbgputc(c)
register c;
{
	putchar(c);
}

/*
 * read a character from the usart
 */

dbggetc()
{
	return(getchar());
}

/*
 * return true if there is a character ready to be input
 */

dbgwaitc()
{

	return(waitchar());
}

/*
 * sysdebug version of printf
 */

dbgprintf(fmt, x1)
register char *fmt;
unsigned x1;
{
	register c;
	register unsigned int *adx;
	register char *s;

#ifdef HWFP
	adx = &x1;
loop:
	while((c = *fmt++) != '%') {
		if(c == '\0')
			return;
		dbgputc(c);
	}
	c = *fmt++;
	if(c == 'd' || c == 'u' || c == 'o' || c == 'x')
		dbgpn((long)*adx, c=='o'? 8: (c=='x'? 16:10));
	else if(c == 's') {
		s = (char *)*adx;
		while(c = *s++)
			dbgputc(c);
	} else if (c == 'D') {
		dbgpn(*(long *)adx, 10);
		adx += (sizeof(long) / sizeof(int)) - 1;
	}
	adx++;
	goto loop;
#endif
}

/*
 * Print an unsigned integer in base b.
 */

dbgpn(n, b)
long n;
{
	register long a;

	if(a = n/b)
		dbgpn(a, b);
	dbgputc("0123456789ABCDEF"[(int)(n%b)]);
}
