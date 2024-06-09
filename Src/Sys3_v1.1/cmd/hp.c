/*
 *	hp [-e] [-m]
 *	convert nroff TTY37 output to right form for HP2640x terminals.
 *	-e indicates enhanced terminal with underline, etc.
 *	-m requests minimization of output by removing multiple newlines
 */

char hpvers[] = "@(#)hp.c	1.6";

#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "sys/ioctl.h"
#include "termio.h"
#define	ESC	033	/* escape */
#define	HFWD	'9'
#define	HREV	'8'
#define	FREV	'7'
#define	SO	016	/* shift out - enter greek */
#define	SI	017	/* shift in */
#define LF	'\n'
#define CR	015
#define	NFLAG	0200	/* hi-bit flag for non-showing chars */
#define	NMASK	0177
#define AMP	046	/* & */
#define DEE	0144	/* d */
#define	BFSIZ	1024	/* size of output buffer (bigger because of tbl weirds
				and nroff idiosyncracies */
#define ALLDELAY 0177400	/* all delay bits in termio.c_oflag */

/*	puty adds control char to output, marked by hi-bit */
#define puty(c)	putx(c|NFLAG)
#define flg(x) ((char)(x|NFLAG))   /* set hi-bit of x */
int	nlcnt,		/* accumulated newline count */
	frevcnt,	/* accumulated reverse line-feeds */
	halfpos,	/* half-line position: -1 = above, +1 = below */
	restrict = 1,	/* 0==> full terminal, 1==> no display enhancements */
	minimiz;	/* 0==> normal, 1==> remove extra newlines */
char	*ttydev;
int	svstmode;	/* for mesg restore */
int	restore();
char	*ttyname();
int	svsgflgs;
struct	termio	sgb;
int	retcode	= 2;	/* return code */
char	peekbuf[2];
char	*peekstr = peekbuf;	/* lookahead ptr */

char	outbf[BFSIZ];	/* output assembly buffer */
char	*bfnext = outbf,	/* addr of next empty byte */
	*bflast =  &outbf[BFSIZ-1];	/* addr of last usable byte */

/*	normal display enhancement strings */
char	*enunder =	"\033&dD",	/* underline */
	*ennorml =	"\033&d@",	/* normal output */
	*ensuper =	"\033&dH",	/* superscript (half) */
	*ensubsc =	"\033&dL",	/* subscript (half, underlined */
	*enrestr =	"\033&dB";	/* restricted (inverse only) */

int	restore();
main(argc, argv)
char **argv;
{
	register c;
	scanarg(argc,argv);
	if (((int)signal(SIGINT, SIG_IGN) & 01) == 0)
		signal(SIGINT, restore);
	if (ioctl(1, TCGETA, &sgb) == 0)
		fixtty();
	setbuf(stdin, calloc(BUFSIZ,1));
	while((c = getchal()) != EOF) {
		if (nlcnt && c != LF) flushnl();
		if (frevcnt && c != ESC) flushrv();
		if (c == LF) {
			if (++nlcnt == 1) flushln();
			continue;
		}
		if (c == ESC)
			escape();
		else if (c == SO)
			special();
		else if (c == '\b')
			backsp();
		else if (c == '_') {		/* C nroff */
			if ((c = getchal()) == '\b') {
				undersc();
			} else {
				putx('_');
				peekbuf[0] = c;
				peekstr = peekbuf;
			}
		}
		else
			putx(c);
	}
	flusher();
	retcode = 0;
	restore();
}

getchal()
{
	if (*peekstr)
		return(*peekstr++);
	return(getchar());
}

/*	scanarg: scan arguments and set flags; ignore unknown args */
scanarg(argc,argv)
	int argc; char **argv;
{
	register char *p;
	while ( --argc > 0) {
		p = *++argv;
		if (*p == '-') {
			++p;
			if (*p == 'e') restrict = 0;
			else if (*p == 'm') minimiz = 1;
		}
	}
	if (restrict)
		enunder = ensuper = ensubsc = enrestr;
}

/*	fixtty: get tty status and save; remove delay and CR-LF mapping */
fixtty()
{
	struct stat sb;

	svsgflgs = sgb.c_oflag;
	sgb.c_oflag &= ~(ALLDELAY|ONLRET|OCRNL|ONLCR);
	ioctl(1, TCSETAW, &sgb);	/* stty nl nl0 cr0 tab0 ff0 */
	fstat(1, &sb);
	svstmode = sb.st_mode;
	ttydev = ttyname(1);
	chmod(ttydev, 0600);	/* mesg n */
}

/*	flusher: flush accumulated newlines, reverse line feeds, buffer */
flusher()
{
	flushln();
	if (nlcnt) flushnl();
	if (frevcnt) flushrv();
	fflush(stdout);
}

/*	flushrv: flush accumulated reverse line feeds */
flushrv()
{
	while (frevcnt--) 
		putstr("\033T");
	putstr(ennorml);
	frevcnt = 0;
}

/*	flushnl: flush accumulated newlines (count in nlcnt) */
flushnl()
{
	if (minimiz != 0 && nlcnt > 2) nlcnt = 2;
	putchar(CR);
	while (nlcnt--)
		putchar(LF);
	nlcnt = 0;
}

putstr(p)
char *p;
{
	register char *pp;
	pp = p;
	while (*pp) puty(*pp++);
}

restore(){
	if (isatty(1)) {
		sgb.c_oflag = svsgflgs;
		ioctl(1, TCSETAW, &sgb);
		chmod(ttydev, svstmode);
	}
	exit(retcode);
}

char	tab[] = {
	'A','A',	/* alpha */
	'B','B',	/* beta */
	'D','W',	/* delta */
	'W','V',	/* DELTA */
	'S','E',	/* epsilon */
	'N','H',	/* eta */
	'\\','Q',	/* gamma */
	'G','+',	/* GAMMA */
	'o','<',	/* infinity - not in M37 */
	'^','\'',	/* integral */
	'L','G',	/* lambda */
	'E',';',	/* LAMBDA */
	'M','M',	/* mu */
	'[','$',	/* nabla (del) */
	'_','\\',	/* not */
	'@','N',	/* nu */
	'C','L',	/* omega */
	'Z',':',	/* OMEGA */
	']','F',	/* partial */
	'U','D',	/* phi */
	'F','.',	/* PHI */
	'V','C',	/* psi */
	'H',',',	/* PSI */
	'J','P',	/* pi */
	'P','*',	/* PI */
	'K','O',	/* rho */
	'Y','S',	/* sigma */
	'R','?',	/* SIGMA */
	'I','T',	/* tau */
	'T','R',	/* theta */
	'O','J',	/* THETA */
	'X','U',	/* xi */
	'Q','Z',	/* zeta */
	'v','Y',
	0
};

char spec1[] = {LF,SO,0};
special(){
	register int c;
	register char *p;
	puty(SO);
	while ((c = getchal()) != EOF) {
		if (c == SI) {
			puty(c);
			return;
		}
		if (c == LF) {
			peekstr = spec1;
			return;
		}
		for (p = tab; *p != 0; p += 2)
			if (c == *p) {
				c = *++p;
				break;
			}
		putx(c);
	}
}

/*	escape: handle escape sequences */
escape()
{
	register int c;
	c = getchal();
	if (frevcnt && c != FREV) flushrv();
	switch (c) {
	case FREV:
		frevcnt++;
		break;
	case HREV:
		if (halfpos == 0) {
			putstr(ensuper);
			halfpos--;
		}
		else if (halfpos > 0) {
			putstr(ennorml);
			halfpos--;
		}
		else {
			putstr("\033T"); /* roll back 1 */
			putstr(ennorml);
			halfpos = 0;
		}
		break;
	case HFWD:
		if (halfpos == 0) {
			putstr(ensubsc);
			halfpos++;
		}
		else if (halfpos < 0) {
			putstr(ennorml);
			halfpos++;
		}
		else {
			putstr("\033S");	/* roll up 1, i.e., LF w/o CR */
			putstr(ennorml);
			halfpos = 0;
		}
		break;
	case '&':
		putstr("\033&");
		puty(c = getchal());
		if (c == 'd') puty(getchal());
		break;
	case ')':
		putstr("\033)");
		puty(getchal());
		break;
default:
		puty(ESC);
		puty(getchal());
		break;
	}
}

char peeku[3];
/*	backsp: handle backspacing */
/* sequences are handled such that : ( \b is backspace )   */
/*	a\b_ -> a in inverse video    */
/*	_\ba -> a in inverse video    */
/*	c\bd -> c in inverse video    */
/* if a control char (ESC) precedes or follows '\b'  */
/* then an actual backspace movement is performed    */
backsp()
{
	register char *bftmp;
	register int c;
	char *bfhi;
	int i, j, ncnt, bscnt;
	char *p, *p1;
	p = bfnext-2;
	p1 = bfnext-4;
	/* check if last char put in buffer was escape sequence */
	/* if so, put backspace control sequence in buffer */
	/* check against "\033&d" */
	if ((*p == flg(ESC)) ||
	   (*p1++ == flg(ESC) && *p1++==flg(AMP) && *p1==flg(DEE))) {
		putstr("\033D");
		while((c=getchal())=='\b') putstr("\033D");
		/* put last char back */
		peeku[0] = c;
		peeku[1] = '\0';
		peekstr = peeku;
		return;
	}
	bscnt = 1;
	c = getchal();
	/* count backspaces */
	while (c == '\b') {
		bscnt++;
		c = getchal();
	}	
	/* read in chars until bscnt have been read */
	/* or until an ESC is read */
	ncnt = 0;
	if (c!=ESC) {
		for (ncnt=0; (ncnt<bscnt && c!=ESC); ncnt++)
			c = getchal();
		/* since there were normal chars with backsp */
		/* insert inverse video chars in buffer */
		bftmp = bfhi = bfnext + 4;
		for (i=0; i<bscnt;)
			if (((*--bftmp = *--bfnext) & NFLAG) == 0) i++;
		putstr(enunder);
		bfnext = bfhi;
		}
	/* if number of bcksp is not = to number normal chars */
	/* then some bcksp chars will be inserted as actual */
	/* bcksp movement and not inverse video for another char */
	j = bscnt - ncnt;
	if (j == 0) putstr(ennorml);
	else {
		/* move chars over and insert normal char seq */
		if (ncnt > 0) {
			bftmp = bfhi = bfnext + 4;
			for (i=0; i<j;)
				if (((*--bftmp = *--bfnext) & NFLAG) == 0) i++;
			putstr(ennorml);
			bfnext = bfhi;
		}
		/* insert backspace movement chars */
		while (j--) putstr("\033D");
	}
	/* put last char read back */
	peeku[0] = c;
	peeku[1] = '\0';
	peekstr = peeku;
}
/*	undersc: handle C nroff's (_ BS char)+ sequences
 *	assumes _ BS already found
 *	if char is a control char, put char back and break.
 */
undersc()
{
	register char c;

	putstr(enunder);
	while (1) {
		if ((c=getchal())==ESC) {
			peeku[0] = c; peeku[1] = '\0';
			peekstr = peeku;
			break;
		}
		else
			putx(c);
		if ((c = getchal()) != '_') {
			peeku[0] = c; peeku[1] = '\0';
			peekstr = peeku;
			break;
		}
		if ((c = getchal()) != '\b') {
			peeku[0] = '_'; peeku[1] = c;
			peekstr = peeku;
			break;
		}
	}
	putstr(ennorml);
}

/*	flushln: flush out accumulated line */
flushln()
{
register char c, *p;
	putx('\0');
	p = outbf;
	while (c = (*p++ & NMASK)) putchar(c);
	bfnext = outbf;
}

/*	putx: add normal (printing) char to output */
putx(c)
char c;
{
	if (bfnext <= bflast)
		*bfnext++ = c;
	else die("line too long\n");
}

die(mesg)
char *mesg;
{
	register char *p;
	char *c = "hp: ";

	write(2, c, 4);
	p = mesg;
	while (*p)
		write(2, p++, 1);
	restore();
}
