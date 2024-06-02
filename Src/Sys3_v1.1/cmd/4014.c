/*
 * optimize output for Tek 4014
 */

char x4014vers[] = "@(#)4014.c	1.5";

#include <stdio.h>
#include <signal.h>
#include "sys/ioctl.h"
#include "termio.h"

#define MAXY 3071
#define LINE 47
#define INCH 282
#define XOFF 248
#define US 037
#define GS 035
#define ESC 033
#define CR 015
#define FF 014
#define SO 016
#define SI 017
#define VALID	01777
#define HALF	5
#define HALFMASK	037

int	pl	= 66*LINE;
int	yyll	= -1;
char	obuf[BUFSIZ];
int	xx = XOFF;
int	xoff = XOFF;
int	coff = 0;
int	ncol = 0;
int	maxcol = 1;
int	yy = MAXY;
int	ohy = -1;
int	ohx = -1;
int	oxb = -1;
int	oly = -1;
int	olx = -1;
int	alpha;
int	ry;
int	erase = 1;
int	tflg;
FILE	*ttyin;

main(argc, argv)
int argc;
char **argv;
{
	register i, j;
	register c;
	char *argptr;
	extern ex();
	extern char *optarg;
	extern int optind;

	while ((c = getopt(argc, argv, "tnp:c:")) != EOF)
		switch(c) {
			case 'p':
				argptr = optarg;
				if (i = tekpoints(argptr)) {
					pl = i;
					yyll = MAXY + 1 - pl;
				}
				break;
			case 'n':
				erase = 0;
				break;

			case 't':
				tflg ++;
				break;

			case 'c':
				argptr = optarg;
				if (i = atoi(argptr)) {
					maxcol = i;
					xx = xoff = 0;
					coff = 4096/i;
				}
				break;

			case '?':
				fprintf(stderr, "usage: 4014 [ -tn ] [ -cN ] [ -pL ] [ file ]\n",(char *)NULL);
				exit(1);
		}
	if ((ttyin = fopen("/dev/tty", "r")) != NULL)
		setbuf(ttyin, (char *)NULL);
	if ( optind < argc ) {
		if (freopen(argv[optind], "r", stdin) == NULL) {
			fprintf(stderr, "4014: cannot open %s\n", argv[optind]);
			exit(1);
		}
	}
	signal(SIGINT, ex);
	setbuf(stdout, obuf);
	ncol = maxcol;
	init();
	while ((i = getchar()) != EOF) {
		switch(i) {

		case FF:
			yy = 0;
		case '\n':
			xx = xoff;
			yy -= LINE;
			alpha = 0;
			if (yy < yyll) {
				ncol++;
				yy = 0;
				sendpt(0);
				putchar(US);
				fflush(stdout);
				if (ncol >= maxcol)
					kwait();
				init();
			}
			continue;

		case CR:
			xx = xoff;
			alpha = 0;
			continue;

		case ' ':
			xx += 31;
			alpha = 0;
			continue;

		case '\t': /*tabstops at 8*31=248*/
			j = ((xx-xoff)/248) + 1;
			xx += j*248 - (xx-xoff);
			alpha = 0;
			continue;

		case '\b':
			xx -= 31;
			alpha = 0;
			continue;

		case ESC:
			switch(i = getchar()) {
			case '7':
				yy += LINE;
				alpha = 0;
				continue;
			case '8':
				yy += (LINE + ry)/2;
				ry = (LINE + ry)%2;
				alpha = 0;
				continue;
			case '9':
				yy -= (LINE - ry)/2;
				ry = -(LINE - ry)%2;
				alpha = 0;
				continue;
			default:
				continue;
			}

		default:
			sendpt(alpha);
			if (alpha==0) {
				putchar(US);
				alpha = 1;
			}
			putchar(i);
			if (i>' ')
				xx += 31;
			continue;
		}
	}
	xx = xoff;
	yy = 0;
	sendpt(0);
	putchar(US);
	kwait();
	ex();
}

init()
{
	static flags;
	ohx = oxb = olx = ohy = oly = -1;
	if (ncol >= maxcol) {
		ncol = 0;
		if (maxcol > 1)
			xoff = 0;
		else
			xoff = XOFF;
	} else
		xoff += coff;
	xx = xoff;
	yy = MAXY;
	if (ncol==0 && erase)
		fputs("\033\014\033;", stdout);
	sendpt(!erase);
	if(!erase && !flags && isatty(fileno(stdout))) {
		struct termio statb;

		ioctl(fileno(ttyin), TCGETA, &statb);
		flags = statb.c_lflag;
		statb.c_lflag = flags & ~(ICANON|ECHO|ECHOE|ECHOK|ECHONL);
		ioctl(fileno(ttyin), TCSETAW, &statb);
		fputs("\033\005", stdout);
		fflush(stdout);
		getc(ttyin);
		xx = (getc(ttyin) << HALF) & VALID;
		xx |= getc(ttyin) & HALFMASK;
		yy = (getc(ttyin) << HALF) & VALID;
		yy |= getc(ttyin) & HALFMASK;
		xx <<= 2, yy <<= 2;
		statb.c_lflag = flags;
		ioctl(fileno(ttyin), TCSETAW, &statb);
	}
}

ex()
{
	yy = MAXY;
	xx = 0;
	fputs("\033;\037", stdout);
	sendpt(1);
	exit(0);
}

kwait()
{
	register c;

	fflush(stdout);
	if (ttyin==NULL || tflg )
		return;
	while ((c=getc(ttyin))!='\n') {
		if (c=='!') {
			execom();
			printf("!\n");
			fflush(stdout);
			continue;
		}
		if (c==EOF)
			ex();
	}
}

execom()
{
	int (*si)(), (*sq)();

	if (fork() != 0) {
		si = signal(SIGINT, SIG_IGN);
		sq = signal(SIGQUIT, SIG_IGN);
		wait((int *)NULL);
		signal(SIGINT, si);
		signal(SIGQUIT, sq);
		return;
	}
	if (isatty(fileno(stdin)) == 0) {
		if (freopen("/dev/tty", "r", stdin)==NULL)
			freopen("/dev/null", "r", stdin);
	}
	execl("/bin/sh", "sh", "-t", 0);
}

sendpt(a)
{
	register zz;
	int hy,xb,ly,hx,lx;

	if (a)
		return;
	if ((zz = yy) < 0)
		zz = 0;
	hy = ((zz>>7) & 037);
	xb = ((xx & 03) + ((zz<<2) & 014) & 017);
	ly = ((zz>>2) & 037);
	hx = ((xx>>7) & 037);
	lx = ((xx>>2) & 037);
	putchar(GS);
	if (hy != ohy)
		putchar(hy | 040);
	if (xb != oxb)
		putchar(xb | 0140);
	if ((ly != oly) || (hx != ohx) || (xb != oxb))
		putchar(ly | 0140);
	if (hx != ohx)
		putchar(hx | 040);
	putchar(lx | 0100);
	ohy = hy;
	oxb = xb;
	oly = ly;
	ohx = hx;
	olx = lx;
	alpha = 0;
}
tekpoints(str)
register char *str;
{
	register c,s = 0;
	while((c = *str++) && ( c >= '0' && c <= '9'))
		s = 10 * s + c - '0';
	switch(c) {
	case 'l':
	case '\0':
		return(s * LINE);
	case 'i':
		return(s*INCH);
	default:
		fprintf(stderr,"illegal scale factor: %s\n", str-1);
		exit(1);
	}
}
