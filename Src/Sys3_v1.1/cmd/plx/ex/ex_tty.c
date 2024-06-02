
/* Copyright (c) 1979 Regents of the University of California */
#include "ex.h"
#include "ex_tty.h"

/*
 * Terminal type initialization routines,
 * and calculation of flags at entry or after
 * a shell escape which may change them.
 */
short	ospeed = -1;

gettmode()
{

#ifndef USG3TTY
	if (gtty(1, &tty) < 0)
		return;
	if (ospeed != tty.sg_ospeed)
		value(SLOWOPEN) = tty.sg_ospeed < B1200;
	ospeed = tty.sg_ospeed;
	normf = tty.sg_flags;
	UPPERCASE = (tty.sg_flags & LCASE) != 0;
	GT = (tty.sg_flags & XTABS) != XTABS && !XT;
	NONL = (tty.sg_flags & CRMOD) == 0;
#else
	if (ioctl(1, TCGETA, &tty) < 0)
		return;
	if (ospeed != tty.c_cflag & CBAUD)
		value(SLOWOPEN) = (tty.c_cflag & CBAUD) < B1200;
	ospeed = tty.c_cflag & CBAUD;
	normf = tty;
	UPPERCASE = (tty.c_iflag & IUCLC) != 0;
	GT = (tty.c_oflag & TABDLY) != TAB3 && !XT;
	NONL = (tty.c_oflag & OCRNL) == 0;
#endif
}

char *xPC;
char **sstrs[] = {
	&AL, &BC, &BT, &CD, &CE, &CL, &CM, &xCR, &DC, &DL, &DM, &DO, &ED, &EI,
	&HO, &IC, &IM, &IP, &LL, &MA,
	&ND, &xNL, &xPC, &SE, &SF, &SO, &SR, &TA, &TE, &TI, &UP, &VB, &VS, &VE
};
bool *sflags[] = {
	&AM, &BS, &DA, &DB, &EO, &HC, &HZ, &IN, &MI, &NC, &NS, &OS, &UL,
	&XB, &XN, &XT, &XX
};
setterm(type)
	char *type;
{
	char *tgoto();
	register int unknown, i;
	register int l;
	char ltcbuf[TCBUFSIZE];

	putpad(TE);
	if (type[0] == 0)
		type = "xx";
	unknown = 0;
	if (tgetent(ltcbuf, type) != 1) {
		unknown++;
		CP(ltcbuf, "uk|dumb:");
	}
	i = LINES = tgetnum("li");
	if (LINES <= 5)
		LINES = 24;
	if (LINES > 48)
		LINES = 48;
	l = LINES;
	if (ospeed < B1200)
		l = 9;	/* including the message line at the bottom */
	else if (ospeed < B2400)
		l = 17;
	aoftspace = tspace;
	zap();

	options[WINDOW].ovalue = options[WINDOW].odefault = l - 1;
	options[SCROLL].ovalue = options[SCROLL].odefault = HC ? 11 : ((l-1) / 2);
	COLUMNS = tgetnum("co");
	if (COLUMNS <= 4)
		COLUMNS = 1000;
	if (tgoto(CM, 2, 2)[0] == 'O')	/* OOPS */
		CA = 0, CM = 0;
	else
		CA = 1, costCM = strlen(tgoto(CM, 8, 10));
	PC = xPC ? xPC[0] : 0;
	aoftspace = tspace;
	CP(ttytype, longname(ltcbuf, type));
	if (i <= 0)
		LINES = 2;
	/* proper strings to change tty type */
#ifdef notdef
	/* Taken out because we don't allow it. See ex_set.c for reasons. */
	if (inopen)
		putpad(VE);
#endif
	termreset();
	gettmode();
	value(REDRAW) = AL && DL;
	value(OPTIMIZE) = !CA && !GT;
	if (unknown)
		serror("%s: Unknown terminal type", type);
}

zap()
{
	register char *namp;
	register bool **fp;
	register char ***sp;

 	namp = "ambsdadbeohchzinmincnsosulxbxnxtxx";
	fp = sflags;
	do {
		*(*fp++) = tgetflag(namp);
		namp += 2;
	} while (*namp);
	namp = "albcbtcdceclcmcrdcdldmdoedeihoicimipllmandnlpcsesfsosrtatetiupvbvsve";
	sp = sstrs;
	do {
		*(*sp++) = tgetstr(namp, &aoftspace);
		namp += 2;
	} while (*namp);
}

char *
longname(bp, def)
	register char *bp;
	char *def;
{
	register char *cp;

	while (*bp && *bp != ':' && *bp != '|')
		bp++;
	if (*bp == '|') {
		bp++;
		cp = bp;
		while (*cp && *cp != ':' && *cp != '|')
			cp++;
		*cp = 0;
		return (bp);
	}
	return (def);
}
