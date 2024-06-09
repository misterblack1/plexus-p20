/*
 *	450	(for DASI 450/DIABLO 1620 terminals)
 *	includes handling of REV L to still work with forms tractors
 *	-f option handles fast (1200 baud) output with ETX/ACK protocol
 *	-f option errors
 *		1:	standard output is not a terminal
		2:	error when opened output terminal for read
 *		3:	output terminal did not respond to ETX
 *		4:	output terminal did not respond with ACK
 */

char x450vers[] = "@(#)450.c	1.10";

#include <fcntl.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <termio.h>

/* input sequences (TTY37 style) */
#define	ESC	033	/* escape */
#define	HFWD	'9'
#define	HREV	'8'
#define	FREV	'7'
#define	SO	016	/* shift out - enter greek */
#define	SI	017	/* shift in */

/* output specials (450/1620 style) */
#define	PLOT	006	/* ack, on/off plot mode */
#define BEL	007	/* exit plot mode */
#define	U	013
#define	D	012
#define	R	' '
#define	L	'\b'
#define LF	'\n'
#define CR	015
#define ETX	003	/* -f query */
#define ACK	006	/* -f response */

int	charcnt = 0;	/* -f count of characters output */
int	problem();	/* -f alarm procedure */
short	foption = 0;	/* -f flag */
struct termio sbuf = {	/* -f terminal settings */
	ISTRIP|IGNPAR|IXON|IXANY,
	0,
	B1200|CS7|CREAD|PARENB,
	0,
	0 };

int	nlcnt,		/* accumulated newline count */
	frevcnt;	/* accumulated reverse line-feeds */
char 	*ttydev;	/* will pt to /dev/tty?? */
int	svstmode;	/* for mesg restore */
int	restore();
struct termio svsgb;
struct	termio sgb;


main(argc, argv)
char **argv;
int argc;
{
	register c;

	if (argc > 1 && argv[1][0] == '-' && argv[1][1] == 'f') foption = 1;
	if (((int)signal(SIGINT, SIG_IGN) & 01) == 0)
		signal(SIGINT, restore);
	if (ioctl(1, TCGETA, &sgb) == 0)
		fixtty();
	setbuf(stdin, calloc(BUFSIZ,1));
	if (foption) { signal(SIGALRM,problem); putchar(ETX); }
	while ((c = getchar()) != EOF) {
		if (c == '\n') {
			if (frevcnt)
				flushrv();
			nlcnt++;
			continue;
		}
		else if (nlcnt)
			flushnl();
		else if (frevcnt && c != ESC)
			flushrv();
		if(c == SO) {
			special();
			continue;
		}
		if( c != ESC ) {
			output(c);
			continue;
		}
		c = getchar();
		if (frevcnt && c != FREV) {
			flushrv();
		}
		if (c == HREV || c == HFWD)
			pout(c);
		else if (c == FREV)
			frevcnt++;
	}
	flusher();
	if (foption) { getack(); putchar(ETX); getack(); }
	restore(0);
}

/*	fixtty: get tty status and save; remove CR-LF mapping */
fixtty()
{
	struct stat sb;
	extern char *ttyname();

	svsgb = sgb;
	if (foption) {
		if (isatty(1)) {
			ttydev = ttyname(1);
			close(1);
			if (open(ttydev,2) != 1) restore(2);
		} else restore(1);
		sgb = sbuf;
		ioctl(1,TCSETAF,&sgb);
	}
	sgb.c_oflag &= ~(ONLRET|OCRNL|ONLCR);
	ioctl(1, TCSETAW, &sgb);		/* stty nl  */
	fstat(1, &sb);
	svstmode = sb.st_mode;
	ttydev = ttyname(1);
	chmod(ttydev, 0600);		/* mesg n */
}

/*	flusher: flush accumulated newlines, reverse line feeds, buffer */
flusher()
{
	if (nlcnt)
		flushnl();
	if (frevcnt)
		flushrv();
}

/*	flushrv: flush accumulated reverse line feeds */
/*	note: expects to be out of plot mode on entry */
char frvadj[] = {U,U,U,LF,LF,LF,0};	/* forms tractor fixup */
flushrv()
{
	while (frevcnt--) {
		putx(U);
		nplot(5,'\0');		/* slow down somewhat */
	}
	putstr(frvadj);
	frevcnt = 0;
}

/*	flushnl: flush accumulated newlines (count in nlcnt) */
flushnl()
{
	putx(CR);			/* must have 1 CR; only 1 needed */
	while (nlcnt--)
		putx(LF);		/* no plot mode needed for these */
	nlcnt = 0;
}

putstr(p)
register char *p;
{
	while (*p)
		putx(*p++);
}

restore(e)
short e;
{
	output(ESC); output('4');
	if (isatty(1)) {
		sgb = svsgb;
		ioctl(1, TCSETAW, &sgb);
		chmod(ttydev,svstmode);
	}
	exit(e);
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

char	tab[]= {
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
char	*trans[]= {
	alpha,
	beta,
	delta,
	DELTA,
	epsilon,
	eta,
	gamma,
	GAMMA,
	infinity,
	integral,
	lambda,
	LAMBDA,
	mu,
	nabla,
	not,
	nu,
	omega,
	OMEGA,
	partial,
	phi,
	PHI,
	psi,
	PSI,
	pi,
	PI,
	rho,
	sigma,
	SIGMA,
	tau,
	theta,
	THETA,
	xi,
	zeta,
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
	register i,c;

	pout(PLOT);
	for (i = 0; (c = s[i]) != 0; i++)  {
		if( c<0 )
			nplot(-c,s[++i]);
		else
			putx(c);
	}
	pout(BEL);
	putx(' ');
}

nplot(n, c)
register n, c;
{
	while(n--)
		putx(c);
}

/*	pout: put out appropriate sequence for motions */
char hrvadj[]	= {ESC,'D', ESC,'U',0};
pout(type)
int type;
{
	putx(ESC);
	switch(type) {
	case HFWD:
		putx('U'); break;
	case HREV:
		putx('D');
		putstr(hrvadj);		/* up & down for forms tractor */
		break;
	case PLOT:
		putx('3'); break;	/* into plot mode */
	case BEL:			/* out of plot mode */
		putx('4'); break;
	}
}

/*	putx: add character to output; convert U to ESC LF */
putx(c)
register c;
{
	if (c == U) {
		output(ESC);
		output(LF);
	} else {
		output(c);
		}
}

/*	output:  all actual output done here */

output(c)
register c;
{
	putchar(c);
	if (foption) {
		charcnt++;
		if (charcnt > 78 && c != ESC ) {
			charcnt = 1;
			putchar(ETX);
			getack();
		}
	}
}
/*	problem:  to catch foption read alarm */
problem()
{
	restore(3);
}

getack()
{
  char dummy;
	alarm(10);
	read(1,&dummy,1);
	alarm(0);
	if (dummy != ACK) restore(4);
}
