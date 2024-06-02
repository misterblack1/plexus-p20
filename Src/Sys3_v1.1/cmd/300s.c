/*
 *	300s [+12] [-n] [-dt,l,c]
 *	for DTC/DASI 300/S
 *	+12 	12-pitch, 6 lines/inch (needs to be faked on this terminal
 *	-n	# increments for half-line spacing
 *	-dt,l,c	delays for tab,  line char
 *
 */

char *xxxvers = "@(#)300s.c	1.6";
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "sys/ioctl.h"
#include "termio.h"
/* input sequences (TTY37 style) */
#define	ESC	033	/* escape */
#define	HFWD	'9'
#define	HREV	'8'
#define	FREV	'7'
#define	SO	016	/* shift out - enter greek */
#define	SI	017	/* shift in */

/* output specials (300 style) */
#define	PLOT	006	/* ack, on/off plot mode */
#define BEL	007     /* exit plot mode */
#define	U	032
#define	D	012
#define	R	' '
#define	L	'\b'
#define LF	'\n'
#define CR	'\r'
#define TABVAL	4	/* approx equivalent of blanks per tab */
#define FLIPMODE	intrmod ^= 01

int	nlcnt,		/* accumulated newline count */
	frevcnt,	/* accumulated reverse line-feeds */
	pitch12,	/* 1==> 12-pitch&6lines/inch, 0==>10-pitch */
	fullsiz,	/* # increments for full line */
	halfsiz = 4;	/* # increments for halfline */
int	tabcnt,		/* accumulated tabs in 1 line */
	charcnt,	/* # chars in line */
	nblcnt;		/* nonblanks contiguous */
int	delay[3] = {3,90,20};
			/* tab limit, characters/line, contiguous lim*/
char 	*ttydev;
int	svstmode;	/* for mesg restore */
int	restore();
int	svsgflgs;
struct	termio	sgb;
char	*scanptr;	/* side-effect of getnum() */

int	intrmod = 1,	/* internal mode, 1==> text, 0 ==> plot */
	extrmod =1;	/* external, same */


main(argc, argv)
char **argv;
{
	register c;

	scanarg(argc, argv);
	if (((int)signal(SIGINT, SIG_IGN) & 01) == 0)
		signal(SIGINT, restore);
	if (ioctl(1, TCGETA, &sgb) == 0)
		fixtty();
	setbuf(stdin, calloc(BUFSIZ,1));
	while ((c = getchar()) != EOF) {
		if (intrmod) {
			if (c == '\n') {
				if (frevcnt)
					flushrv();
				nlcnt++;
				continue;
			} else if (nlcnt)
				flushnl();
			else if (frevcnt && c != ESC)
				flushrv();
		}
		if (c == PLOT) {
			FLIPMODE;
			continue;
		}
		if (c == SO) {
			special();
			continue;
		}
		if (c != ESC) {
			charcnt++;
			if (c == ' ')
				nblcnt = 0;
			else if (c == '\t') {
				charcnt += TABVAL;
				nblcnt = 0;
				if (++tabcnt >= delay[0]) {
					putx('\0');
					tabcnt = 0;
					if (delay[0] == 0)	/* 2 nulls per tab */
						putx('\0');
				}
			} else if (c == '\b')
				charcnt--;
			else if (++nblcnt >= delay[2]) {
				nblcnt = 0;
				putx('\0');
				if (delay[2] == 0)
					putx('\0');
			}
			if (intrmod != extrmod) {
				if (intrmod == 1)
					putchar(ESC);	/* 300s uses ESC PLOT to get out */
				putchar(PLOT);		/* flip real state */
				extrmod = intrmod;
			}
			putchar(c);
			continue;
		}
		FLIPMODE;
		c = getchar();
		if (frevcnt && c != FREV) {
			FLIPMODE;
			flushrv();
			FLIPMODE;
		}
		if (c == HREV)
			nplot(halfsiz,U);
		else if (c == HFWD)
			nplot(halfsiz,D);
		else if (c == FREV)
			frevcnt++;
		FLIPMODE;
	}
	flusher();
	restore();
}


/*	scanarg: scan arguments and set flags; ignore unknown args */
scanarg(argc, argv)
char **argv;
{
	register char *p;

	while (--argc > 0) {
		p = *++argv;
		if (strcmp(p, "+12") == 0)
			pitch12 = 1;
		else if (*p++ == '-') {
			if (*p == 'd')
				getdelay(++p);
			else if (*p > '0' && *p <= '9')
				halfsiz = *p - '0';
		}
	}
	fullsiz = pitch12 ? 6 : 8;
}

/*	getdelay: scan fields of delay arg */
getdelay(p)
register char *p;
{
	register i;

	for (i = 0; i <= 2; i++) {
		if (*p == ',') {
			p++;
			continue;
		}
		if (*p == '\0')
			break;
		delay[i] = getnum(p);
		p = scanptr+1;
		if (*scanptr != ',')
			break;
	}
}

/*	fixtty: get tty status and save; remove CR-LF mapping */
fixtty()
{
	struct stat sb;

	svsgflgs = sgb.c_oflag;
	sgb.c_oflag &= ~(ONLRET|OCRNL|ONLCR);
	ioctl(1, TCSETAW, &sgb);		/* stty nl  */
	fstat(1, &sb);
	svstmode = sb.st_mode;
	ttydev = (char*)ttyname(1);
	chmod(ttydev, 0600);		/* mesg n */
}

getnum(p)
register char *p;
{
	register i = 0 ;

	while (*p >= '0' && *p <= '9')
		i = 10*i + *p++ - '0';
	scanptr = p;
	return(i);
}

/*	flusher: flush accumulated newlines, reverse line feeds, buffer */
flusher()
{
	if (nlcnt)
		flushnl();
	if (frevcnt)
		flushrv();
	fflush(stdout);
}

/*	flushrv: flush accumulated reverse line feeds */
/*	note: expects to be out of plot mode on entry */
char frv1[] = {U,PLOT,U,U,PLOT,0};	/* 1 FREV leftover */
char frv2[] = {U,U,PLOT,U,U,U,U,PLOT,0};	/* 2 of them */
char frvadj[] = {U,U,U,LF,LF,LF,0};	/* forms tractor fixup */
flushrv()
{
	register numleft;

	if (pitch12) {
		numleft = frevcnt % 3;
		frevcnt = 4 * (frevcnt / 3);
	} else
		numleft = 0;
	while (frevcnt--) {
		putx(U);
		nplot(5,'\0');		/* slow down somewhat */
	}
	if (numleft == 1)
		putstr(frv1);
	else if (numleft == 2)
		putstr(frv2);
	putstr(frvadj);
	frevcnt = 0;
}

/*	flushnl: flush accumulated newlines (count in nlcnt) */
char nl1[] = {LF, PLOT, LF, LF, PLOT, 0}; /* 12pitch: 1 nl */
char nl2[] = {LF, LF, PLOT, LF, LF, LF, LF, PLOT, 0}; /* 2 nls */
flushnl()
{
	register numleft;

	if (pitch12) {
		numleft = nlcnt % 3;
		nlcnt = 4 * (nlcnt/3);
	} else
		numleft = 0;
	putx(CR);	/* must have 1 CR; only 1 needed */
	while (nlcnt--)
		putx(LF);	/* no plot mode needed for these */
	if (numleft == 1)
		putstr(nl1);
	else if (numleft == 2)
		putstr(nl2);
	if (charcnt > delay[1])
		nplot(1 + charcnt/20,'\0');
	nlcnt = charcnt = nblcnt = tabcnt = 0;
}

putstr(p)
register char *p;
{
	while (*p)
		putx(*p++);
}

restore()
{
	putchar(ESC);
	putchar(PLOT);
	if (isatty(1)) {
		sgb.c_oflag = svsgflgs;
		ioctl(1, TCSETAW, &sgb);
		chmod(ttydev, svstmode);
	}
	exit(0);
}

char alpha[]	= {L,'c',R,R,'(',L,0};
char beta[]	= {'B',L,L,D,D,'|',R,R,U,U,0};
char delta[]	= {'o',U,U,'<',D,D,0};
char DELTA[]	= {L,L,'/',-3,D,'-',-4,R,'-',-3,U,'\\',L,L,0};
char epsilon[]	= {'<','-',0};
char eta[]	= {'n',R,R,D,D,'|',L,L,U,U,0};
char gamma[]	= {')',R,'/',L,0};
char GAMMA[]	= {L,L,'|',R,R,-3,U,'-',-3,D,R,R,'`',L,L,0};
char infinity[]	= {L,L,'c',-4,R,'o',L,L,0};
char integral[]	= {'|','\'',R,R,'`',-3,L,-6,D,'\'',L,'`',R,R,-6,U,0};
char lambda[]	= {'\\',-4,D,L,'\'',D,L,'\'',-5,U,R,R,0};
char LAMBDA[]	= {L,L,'/',-4,R,'\\',L,L,0};
char mu[]	= {'u',L,L,',',R,R,0};
char nabla[]	= {L,L,'\\',-3,U,'-',-4,R,'-',-3,D,'/',L,L,0};
char not[]	= {'-',-2,R,U,',',D,-2,L,0};
char nu[]	= {L,'(',-3,R,'/',L,L,0};
char omega[]	= {L,'u',-3,R,'u',L,L,0};
char OMEGA[]	= {'O',D,D,L,'-',R,R,'-',L,U,U,0};
char partial[]	= {'o',R,D,'`',L,U,'`',L,U,'`',R,D,0};
char phi[]	= {'o','/',0};
char PHI[]	= {'o','[',']',0};
char psi[]	= {'/','-',D,D,R,R,'\'',-4,L,'\'',R,R,U,U,0};
char PSI[]	= {'[',']','-',D,D,R,R,'\'',-4,L,'`',R,R,U,U,0};
char pi[]	= {U,'-',-3,D,'"',D,'"',-3,U,0};
char PI[]	= {L,L,'[',']',-4,R,'[',']',L,L,-3,U,'-',-3,D,0};
char rho[]	= {'o',L,L,D,D,'|',U,U,R,R,0};
char sigma[]	= {'o',D,R,R,'~',U,L,L,0};
char SIGMA[]	= {'>',-2,D,'-',-5,U,'-',D,D,0};
char tau[]	= {'t',D,R,R,'~',L,L,L,'~',R,U,0};
char theta[]	= {'O','-',0};
char THETA[]	= {'O','=',0};
char xi[]	= {'c',R,D,',',L,-3,U,'c',L,D,'`',R,D,0};
char zeta[]	= {'c',R,D,',',L,-3,U,'<',D,D,0};

char	tab[] = {
	'A',	/* alpha */
	'B',	/* beta */
	'D',	/* delta */
	'W',	/* DELTA */
	'S',	/* epsilon */
	'N',	/* eta */
	'\\',	/* gamma */
	'G',	/* GAMMA */
	'o',	/* infinity - not in M37 */
	'^',	/* integral */
	'L',	/* lambda */
	'E',	/* LAMBDA */
	'M',	/* mu */
	'[',	/* nabla (del) */
	'_',	/* not */
	'@',	/* nu */
	'C',	/* omega */
	'Z',	/* OMEGA */
	']',	/* partial */
	'U',	/* phi */
	'F',	/* PHI */
	'V',	/* psi */
	'H',	/* PSI */
	'J',	/* pi */
	'P',	/* PI */
	'K',	/* rho */
	'Y',	/* sigma */
	'R',	/* SIGMA */
	'I',	/* tau */
	'T',	/* theta */
	'O',	/* THETA */
	'X',	/* xi */
	'Q',	/* zeta */
	0
};
int	trans[] = {
	(int)alpha,
	(int)beta,
	(int)delta,
	(int)DELTA,
	(int)epsilon,
	(int)eta,
	(int)gamma,
	(int)GAMMA,
	(int)infinity,
	(int)integral,
	(int)lambda,
	(int)LAMBDA,
	(int)mu,
	(int)nabla,
	(int)not,
	(int)nu,
	(int)omega,
	(int)OMEGA,
	(int)partial,
	(int)phi,
	(int)PHI,
	(int)psi,
	(int)PSI,
	(int)pi,
	(int)PI,
	(int)rho,
	(int)sigma,
	(int)SIGMA,
	(int)tau,
	(int)theta,
	(int)THETA,
	(int)xi,
	(int)zeta,
	0
};

special()
{
	register c, i;

   loop:
	if ((c = getchar()) == SI || c < 0)
		return;
	for (i = 0; tab[i] != 0; i++)
		if (c == tab[i]) {
			plot(trans[i]);
			goto loop;
		}
	putx(c);
	goto loop;
}

plot(s)
register char *s;
{
	register i, c;

	FLIPMODE;
	for (i = 0; (c = s[i]) != 0; i++) {
		if (c < 0)
			nplot(-c, s[++i]);
		else
			putx(c);
	}
	FLIPMODE;
	putx(' ');
}

nplot(n, c)
register n, c;
{
	while(n--)
		putx(c);
}

/*	putx: add ordinary (not PLOT or BEL) character to output, adding
	extra PLOT when needed to flip state. */
putx(c)
register c;
{
	if (intrmod != extrmod) {
		if (intrmod == 1)
			putchar(ESC);	/* 300s uses ESC PLOT to get out, not flipflop */
		putchar(PLOT);
		extrmod = intrmod;
	}
	putchar(c);
}
