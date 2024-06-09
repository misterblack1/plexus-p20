/*
** Editor
*/

/*
**	The assembly code for ed.c should be
**	altered making the .data's for the
**	following array .text's so that it
**	can be shared (a la the Shell).
*/


char	*msgtab[] =
{
	"write or open on pipe failed",			/*  0 */
	"warning: expecting `w'",			/*  1 */
	"mark not lower case",				/*  2 */
	"cannot open input file",			/*  3 */
	"PWB spec problem",				/*  4 */
	"nothing to undo",				/*  5 */
	"restricted shell",				/*  6 */
	"cannot create output file",			/*  7 */
	"filesystem out of space!",			/*  8 */
	"cannot open file",				/*  9 */
	"cannot link",					/* 10 */
	"Range endpoint too large",			/* 11 */
	"unknown command",				/* 12 */
	"search string not found",			/* 13 */
	"-",						/* 14 */
	"line out of range",				/* 15 */
	"bad number",					/* 16 */
	"bad range",					/* 17 */
	"Illegal address count",			/* 18 */
	"incomplete global expression",			/* 19 */
	"illegal suffix",				/* 20 */
	"illegal or missing filename",			/* 21 */
	"no space after command",			/* 22 */
	"fork failed - try again",			/* 23 */
	"maximum of 64 characters in file names",	/* 24 */
	"`\\digit' out of range",			/* 25 */
	"interrupt",					/* 26 */
	"line too long",				/* 27 */
	"illegal character in input file",		/* 28 */
	"write error",					/* 29 */
	"out of memory for append",			/* 30 */
	"temp file too big",				/* 31 */
	"I/O error on temp file",			/* 32 */
	"multiple globals not allowed",			/* 33 */
	"global too long",				/* 34 */
	"no match",					/* 35 */
	"illegal or missing delimiter",			/* 36 */
	"-",						/* 37 */
	"replacement string too long",			/* 38 */
	"illegal move destination",			/* 39 */
	"-",						/* 40 */
	"no remembered search string",			/* 41 */
	"'\\( \\)' imbalance",				/* 42 */
	"Too many `\\(' s",				/* 43 */
	"more than 2 numbers given",			/* 44 */
	"'\\}' expected",				/* 45 */
	"first number exceeds second",			/* 46 */
	"incomplete substitute",			/* 47 */
	"newline unexpected",				/* 48 */
	"'[ ]' imbalance",				/* 49 */
	"regular expression overflow",			/* 50 */
	"regular expression error",			/* 51 */
	"command expected",				/* 52 */
	"a, i, or c not allowed in G",			/* 53 */
	"end of line expected",				/* 54 */
	"no remembered replacement string",		/* 55 */
	"no remembered command",			/* 56 */
	"illegal redirection",				/* 57 */
	"possible concurrent update",			/* 58 */
	"that command confuses yed",			/* 59 */
	"the x command has become X (upper case)",	/* 60 */
	0
};

/*
 * Define some macros for the regular expression 
 * routines to use for input and error stuff.
 */

#define INIT		extern int peekc;
#define GETC()		getchr()
#define UNGETC(c)	(peekc = c)
#define PEEKC()		(peekc = getchr())
#define RETURN(c)	return
#define ERROR(c)	error1(c)


#include "regexp.h"
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termio.h>
#ifndef	RESEARCH
#include <ustat.h>
#endif
#include <setjmp.h>

#define	PUTM()	if(xcode >= 0) puts(msgtab[xcode])
#define	FNSIZE	64
#define	LBSIZE	512
#define ESIZE	256
#define	GBSIZE	256
#define	EOF	-1
#define	KSIZE	9

#define	READ	0
#define	WRITE	1

#define PRNT	02

int	Xqt = 0;
int	lastc;
char	savedfile[FNSIZE];
char	file[FNSIZE];
char	funny[LBSIZE];
char	linebuf[LBSIZE];
char	expbuf[ESIZE];
char	rhsbuf[LBSIZE];
struct lin	{int cur;
		int sav;
		};
typedef struct lin *LINE;
LINE zero;
LINE dot;
LINE dol;
LINE endcore;
LINE fendcore;
LINE addr1;
LINE addr2;
LINE savdol, savdot;
int	globflg;
int	initflg;
char	genbuf[LBSIZE];
long	count;
char	*nextip;
char	*linebp;
int	ninbuf;
int	peekc;
int	io;
int	(*oldhup)();
int	(*oldquit)(), (*oldpipe)();
int	vflag = 1;
int	yflag;
int	xflag;
int	xtflag;
int	kflag;
int	hflag;
int	xcode = -1;
char	key[KSIZE + 1];
char	crbuf[512];
char	perm[768];
char	tperm[768];
int	col;
char	*globp;
int	tfile = -1;
int	tline;
char	*tfname;
char	*locs;
char	ibuff[512];
int	iblock = -1;
char	obuff[512];
int	oblock = -1;
int	ichanged;
int	nleft;
int	names[26];
int	anymarks;
int	subnewa;
int	fchange;
int	nline;
int	fflg, shflg;
char	prompt = '*';
int	rflg;
int	readflg;
int	eflg;
int	ncflg;
int	listn;
int	listf;
int	pflag;
long 	savtime;
char *name = "SHELL";
char *rshell = "/bin/rsh";
char *val;

char *getenv();
LINE address();
char	*getline();
char	*getblock();
char	*place();
int	(*signal())();
char	*mktemp();
char	*sbrk();
struct stat	Fl, Tf;
#ifndef RESEARCH
struct ustat	U;
int	Short = 0;
int	oldmask; /* No umask while writing */
#endif
jmp_buf	savej;

#ifdef	NULLS
int	nulls;	/* Null count */
#endif
#ifdef	PWB
long	ccount;

struct	Fspec	{
	char	Ftabs[22];
	char	Fdel;
	unsigned char	Flim;
	char	Fmov;
	char	Ffill;
};
struct Fspec fss;
#endif

int errcnt = 0;

onpipe()
{
	error(0);
}

main(argc, argv)
char **argv;
{
	register char *p1, *p2;
	extern int onintr(), quit(), onhup();
	int (*oldintr)();

	oldquit = signal(SIGQUIT, SIG_IGN);
	oldhup = signal(SIGHUP, SIG_IGN);
	oldintr = signal(SIGINT, SIG_IGN);
	oldpipe = signal(SIGPIPE, onpipe);
	if (((int)signal(SIGTERM, SIG_IGN)&01) == 0)
		signal(SIGTERM, quit);
	p1 = *argv;
	while(*p1++);
	while(--p1 >= *argv)
		if(*p1 == '/')
			break;
	*argv = p1 + 1;
	/* if SHELL set in environment and is /bin/rsh, set rflg */
	val = getenv(name);
	if (strcmp(val, rshell) == 0)
		rflg++;
	if (**argv == 'r')
		rflg++;
	argv++;
	while (argc > 1 && **argv=='-') {
		switch((*argv)[1]) {

		case '\0':
			vflag = 0;
			break;

		case 'q':
			signal(SIGQUIT, SIG_DFL);
			vflag = 1;
			break;

		case 'x':
			xflag = 1;
			break;
		case 'y':
			yflag = 03;
			break;
		}
		argv++;
		argc--;
	}
	if(xflag){
		getkey();
		kflag = crinit(key, perm);
	}

	if (argc>1) {
		p1 = *argv;
		if(strlen(p1) >= FNSIZE) {
			puts("file name too long");
			exit(2);
		}
		p2 = savedfile;
		while (*p2++ = *p1++);
		globp = "r";
		fflg++;
	}
	else 	/* editing with no file so set savtime to 0 */
		savtime = 0;
	eflg++;
	fendcore = (LINE )sbrk(0);
	tfname = mktemp("/tmp/eXXXXX");
	init();
	if (((int)oldintr&01) == 0)
		signal(SIGINT, onintr);
	if (((int)oldhup&01) == 0)
		signal(SIGHUP, onhup);
	setjmp(savej);
	commands();
	quit();
}

commands()
{
	int getfile(), gettty();
	register LINE a1;
	register c;
	register char *p1, *p2;
	int n;

	for (;;) {
	nodelim = 0;
	if ( pflag ) {
		pflag = 0;
		addr1 = addr2 = dot;
		goto print;
	}
	if (shflg && globp==0)
		write(1,&prompt,1);
	addr1 = 0;
	addr2 = 0;
	if((c=getchr()) == ',') {
		addr1 = zero + 1;
		addr2 = dol;
		c = getchr();
		goto swch;
	} else if(c == ';') {
		addr1 = dot;
		addr2 = dol;
		c = getchr();
		goto swch;
	} else
		peekc = c;
	do {
		addr1 = addr2;
		if ((a1 = address())==0) {
			c = getchr();
			break;
		}
		addr2 = a1;
		if ((c=getchr()) == ';') {
			c = ',';
			dot = a1;
		}
	} while (c==',');
	if (addr1==0)
		addr1 = addr2;
swch:
	switch(c) {

	case 'a':
		setdot();
		newline();
		if (!globflg) save();
		append(gettty, addr2);
		continue;

	case 'c':
		delete();
		append(gettty, addr1-1);
		continue;

	case 'd':
		delete();
		continue;

	case 'E':
		fchange = 0;
		c = 'e';
	case 'e':
		fflg++;
		setnoaddr();
		if (vflag && fchange) {
			fchange = 0;
			error(1);
		}
		filename(c);
		eflg++;
		init();
		addr2 = zero;
		goto caseread;

	case 'f':
		setnoaddr();
		filename(c);
		if (!ncflg)  /* there is a filename */
			getime();
		else
			ncflg--;
		puts(savedfile);
		continue;

	case 'g':
		global(1);
		continue;
	case 'G':
		globaln(1);
		continue;

	case 'h':
		newline();
		setnoaddr();
		PUTM();
		continue;

	case 'H':
		newline();
		setnoaddr();
		if(!hflag) {
			hflag = 1;
			PUTM();
		}
		else
			hflag = 0;
		continue;

	case 'i':
		setdot();
		nonzero();
		newline();
		if (!globflg) save();
		append(gettty, addr2-1);
		if (dot == addr2-1)
			dot += 1;
		continue;


	case 'j':
		if (addr2==0) {
			addr1 = dot;
			addr2 = dot+1;
		}
		setdot();
		newline();
		nonzero();
		if (!globflg) save();
		join();
		continue;

	case 'k':
		if ((c = getchr()) < 'a' || c > 'z')
			error(2);
		newline();
		setdot();
		nonzero();
		names[c-'a'] = addr2->cur & ~01;
		anymarks |= 01;
		continue;

	case 'm':
		move(0);
		continue;

	case '\n':
		if (addr2==0)
			addr2 = dot+1;
		addr1 = addr2;
		goto print;

	case 'n':
		listn++;
		newline();
		goto print;

	case 'l':
		listf++;
	case 'p':
		newline();
	print:
		setdot();
		nonzero();
		a1 = addr1;
		do {
			if (listn) {
				count = a1 - zero;
				putd();
				putchr('\t');
			}
			puts(getline(a1++->cur));
		}
		while (a1 <= addr2);
		dot = addr2;
		listn = 0;
		listf = 0;
		continue;

	case 'Q':
		fchange = 0;
	case 'q':
		setnoaddr();
		newline();
		quit();

	case 'r':
		filename(c);
	caseread:
		readflg = 1;
		if ((io = eopen(file, 0)) < 0) {
			lastc = '\n';
			/* if first entering editor and file does not exist */
			/* set saved access time to 0 */
			if (eflg) {
				savtime = 0;
				eflg  = 0;
			}
			error(3);
		}
		/* get last mod time of file */
		/* eflg - entered editor with ed or e  */
		if (eflg) {
			eflg = 0;
			getime();
		}
		setall();
		ninbuf = 0;
		n = zero != dol;
#ifdef NULLS
		nulls = 0;
#endif
		if (!globflg && (c == 'r')) save();
		append(getfile, addr2);
		exfile();
		readflg = 0;
		fchange = n;
		continue;

	case 's':
		setdot();
		nonzero();
		if (!globflg) save();
		substitute(globp!=0);
		continue;

	case 't':
		move(1);
		continue;

	case 'u':
		setdot();
		newline();
		if (!initflg) undo();
		else error(5);
		continue;

	case 'v':
		global(0);
		continue;
	case 'V':
		globaln(0);
		continue;

	case 'w':
		setall();
		nonzero();
		filename(c);
		if(Xqt) {
			io = eopen(file, 1);
			n = 1;	/* set n so newtime will not execute */
		} else {
			fstat(tfile,&Tf);
			if(stat(file, &Fl) < 0) {
				if((io = creat(file, 0666)) < 0)
					error(7);
				fstat(io, &Fl);
				Fl.st_mtime = 0;
				close(io);
			}
			else {
#ifndef	RESEARCH
				oldmask = umask(0);
#endif
			}
#ifndef RESEARCH
			ustat(Fl.st_dev, &U);
			if(!Short && U.f_tfree < ((Tf.st_size>>9) + 100)) {
				Short = 1;
				error(8);
			}
			Short = 0;
#endif
			if (Fl.st_nlink == 1 && (Fl.st_mode & S_IFMT) == S_IFREG) {
				if (close(open(file,1)) < 0)
					error(9);
				p1 = savedfile;
				p2 = file;
				if (!(n=strcmp(p1,p2)))
					chktime();
				mkfunny();
				if ((io = creat(funny,Fl.st_mode)) >= 0) {
					chown(funny,Fl.st_uid,Fl.st_gid);
					putfile();
					exfile();
					unlink(file);
					if (link(funny,file))
						error(10);
					unlink(funny);
					/* if filenames are the same */
					if (!n)
						newtime();
					/* check if entire buffer was written */
					fchange = (addr1==zero+1 && addr2==dol)?0:fchange;
					continue;
				}
			}
			else   n = 1;	/* set n so newtime will not execute*/
			if((io = creat(file, 0666)) < 0)
				error(7);
		}
		putfile();
		exfile();
		if (!n) newtime();
		fchange = (addr1==zero+1 && addr2==dol)?0:fchange;
		continue;

	case 'X':
		setnoaddr();
		newline();
		xflag = 1;
		getkey();
		kflag = crinit(key, perm);
		continue;


	case '=':
		setall();
		newline();
		count = (addr2-zero)&077777;
		putd();
		putchr('\n');
		continue;

	case '!':
		unixcom();
		continue;

	case EOF:
		return;

	case 'P':
		if (yflag)
			error(59);
		setnoaddr();
		newline();
		if (shflg)
			shflg = 0;
		else
			shflg++;
		continue;
	}
	if (c == 'x')
		error(60);
	else
		error(12);
	error(12);
	}
}

LINE 
address()
{
	register minus, c;
	register LINE a1;
	int n, relerr;

	minus = 0;
	a1 = 0;
	for (;;) {
		c = getchr();
		if ('0'<=c && c<='9') {
			n = 0;
			do {
				n *= 10;
				n += c - '0';
			} while ((c = getchr())>='0' && c<='9');
			peekc = c;
			if (a1==0)
				a1 = zero;
			if (minus<0)
				n = -n;
			a1 += n;
			minus = 0;
			continue;
		}
		relerr = 0;
		if (a1 || minus)
			relerr++;
		switch(c) {
		case ' ':
		case '\t':
			continue;
	
		case '+':
			minus++;
			if (a1==0)
				a1 = dot;
			continue;

		case '-':
		case '^':
			minus--;
			if (a1==0)
				a1 = dot;
			continue;
	
		case '?':
		case '/':
			compile((char *) 0, expbuf, &expbuf[ESIZE], c);
			a1 = dot;
			for (;;) {
				if (c=='/') {
					a1++;
					if (a1 > dol)
						a1 = zero;
				} else {
					a1--;
					if (a1 < zero)
						a1 = dol;
				}
				if (execute(0, a1))
					break;
				if (a1==dot)
					error(13);
			}
			break;
	
		case '$':
			a1 = dol;
			break;
	
		case '.':
			a1 = dot;
			break;

		case '\'':
			if ((c = getchr()) < 'a' || c > 'z')
				error(2);
			for (a1=zero; a1<=dol; a1++)
				if (names[c-'a'] == (a1->cur & ~01))
					break;
			break;
	
		case 'y' & 037:
			if(yflag) {
				newline();
				setnoaddr();
				yflag ^= 01;
				continue;
			}

		default:
			peekc = c;
			if (a1==0)
				return(0);
			a1 += minus;
			if (a1<zero || a1>dol)
				error(15);
			return(a1);
		}
		if (relerr)
			error(16);
	}
}

setdot()
{
	if (addr2 == 0)
		addr1 = addr2 = dot;
	if (addr1 > addr2)
		error(17);
}

setall()
{
	if (addr2==0) {
		addr1 = zero+1;
		addr2 = dol;
		if (dol==zero)
			addr1 = zero;
	}
	setdot();
}

setnoaddr()
{
	if (addr2)
		error(18);
}

nonzero()
{
	if (addr1<=zero || addr2>dol)
		error(15);
}

newline()
{
	register c;

	c = getchr();
	if ( c == 'p' || c == 'l' || c == 'n' ) {
		pflag++;
		if ( c == 'l') listf++;
		if ( c == 'n') listn++;
		c = getchr();
	}
	if ( c != '\n')
		error(20);
}

filename(comm)
{
	register char *p1, *p2;
	register c;
	register i = 0;

	count = 0;
	c = getchr();
	if (c=='\n' || c==EOF) {
		p1 = savedfile;
		if (*p1==0 && comm!='f')
			error(21);
		/* ncflg set means do not get mod time of file */
		/* since no filename followed f */
		if (comm == 'f')
			ncflg++;
		p2 = file;
		while (*p2++ = *p1++);
		red(savedfile);
		return;
	}
	if (c!=' ')
		error(22);
	while ((c = getchr()) == ' ');
	if(c == '!')
		++Xqt, c = getchr();
	if (c=='\n')
		error(21);
	p1 = file;
	do {
		if(++i >= FNSIZE)
			error(24);
		*p1++ = c;
		if(c==EOF || (c==' ' && !Xqt))
			error(21);
	} while ((c = getchr()) != '\n');
	*p1++ = 0;
	if(Xqt)
		if (comm=='f') {
			--Xqt;
			error(57);
		}
		else
			return;
	if (savedfile[0]==0 || comm=='e' || comm=='f') {
		p1 = savedfile;
		p2 = file;
		while (*p1++ = *p2++);
	}
	red(file);
}

exfile()
{
#ifdef NULLS
	register c;
#endif

#ifndef RESEARCH
	if(oldmask) {
		umask(oldmask);
		oldmask = 0;
	}
#endif
	eclose(io);
	io = -1;
	if (vflag) {
		putd();
		putchr('\n');
#ifdef NULLS
		if(nulls) {
			c = count;
			count = nulls;
			nulls = 0;
			putd();
			puts(" nulls replaced by '\\0'");
			count = c;
		}
#endif
	}
}

onintr()
{
	signal(SIGINT, onintr);
	putchr('\n');
	lastc = '\n';
	if (*funny) unlink(funny); /* remove tmp file */
	/* if interruped a read, only part of file may be in buffer */
	if ( readflg ) {
		puts ("\007read may be incomplete - beware!\007");
		fchange = 0;
	}
	error(26);
}

onhup()
{
	signal(SIGINT, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	/* if there are lines in file and file was */
	/* not written since last update, save in ed.hup */
	if (dol > zero && fchange == 1) {
		addr1 = zero+1;
		addr2 = dol;
		io = creat("ed.hup", 0666);
		if (io > 0)
			putfile();
	}
	fchange = 0;
	quit();
}

error(code)
register code;
{
	register c;

	readflg = 0;
	++errcnt;
	listf = listn = 0;
	pflag = 0;
#ifndef RESEARCH
	if(oldmask) {
		umask(oldmask);
		oldmask = 0;
	}
#endif
#ifdef NULLS	/* Not really nulls, but close enough */
	/* This is a bug because of buffering */
	if(code == 28) /* illegal char. */
		putd();
#endif
	putchr('?');
	if(code == 3)	/* Cant open file */
		puts(file);
	else
		putchr('\n');
	count = 0;
	lseek(0, (long)0, 2);
	if (globp)
		lastc = '\n';
	globp = 0;
	peekc = lastc;
	if(lastc)
		while ((c = getchr()) != '\n' && c != EOF);
	if (io > 0) {
		eclose(io);
		io = -1;
	}
	xcode = code;
	if(hflag)
		PUTM();
	longjmp(savej, 1);
}

getchr()
{
	char c;
	if (lastc=peekc) {
		peekc = 0;
		return(lastc);
	}
	if (globp) {
		if ((lastc = *globp++) != 0)
			return(lastc);
		globp = 0;
		return(EOF);
	}
	if (read(0, &c, 1) <= 0)
		return(lastc = EOF);
	lastc = c&0177;
	return(lastc);
}

gettty()
{
	register c;
	register char *gf;
	register char *p;

	p = linebuf;
	gf = globp;
	while ((c = getchr()) != '\n') {
		if (c==EOF) {
			if (gf)
				peekc = c;
			return(c);
		}
		if ((c &= 0177) == 0)
			continue;
		*p++ = c;
		if (p >= &linebuf[LBSIZE-2])
			error(27);
	}
	*p++ = 0;
	if (linebuf[0]=='.' && linebuf[1]==0)
		return(EOF);
	if (linebuf[0]=='\\' && linebuf[1]=='.' && linebuf[2]==0) {
		linebuf[0] = '.';
		linebuf[1] = 0;
	}
	return(0);
}

getfile()
{
	register c;
	register char *lp, *fp;
	int crflag;

	crflag = 0;
	lp = linebuf;
	fp = nextip;
	do {
		if (--ninbuf < 0) {
			if ((ninbuf = read(io, genbuf, LBSIZE)-1) < 0)
				return(EOF);
			fp = genbuf;
			while(fp < &genbuf[ninbuf])
				if(*fp++ & 0200) crflag = 1;
			if(crflag){
				if(kflag){
					crblock(perm, genbuf, ninbuf+1, count);
				}else error(28);
			}
			fp = genbuf;
			while(fp < &genbuf[ninbuf]){
				if(*fp++ & 0200) error(28);
			}
			fp = genbuf;
		}
		if (lp >= &linebuf[LBSIZE]) {
			lastc = '\n';
			error(27);
		}
		if ((*lp++ = c = *fp++ & 0177) == 0) {
#ifdef NULLS
			lp[-1] = '\\';
			*lp++ = '0';
			nulls++;
#else
			lp--;
			continue;
#endif
		}
		count++;
	} while (c != '\n');
	*--lp = 0;
	nextip = fp;
#ifdef PWB
	if (fss.Ffill && fss.Flim && lenchk(linebuf,&fss) < 0) {
		write(1,"line too long: lno = ",21);
		ccount = count;
		count = (++dot-zero)&077777;
		dot--;
		putd();
		count = ccount;
		putchr('\n');
	}
#endif
	return(0);
}

putfile()
{
	int n;
	LINE a1;
	register char *fp, *lp;
	register nib;

	nib = 512;
	fp = genbuf;
	a1 = addr1;
	do {
		lp = getline(a1++->cur);
#ifdef PWB
		if (fss.Ffill && fss.Flim && lenchk(linebuf,&fss) < 0) {
			write(1,"line too long: lno = ",21);
			ccount = count;
			count = (++dot-zero)&077777;
			dot--;
			putd();
			count = ccount;
			putchr('\n');
		}
#endif
		for (;;) {
			if (--nib < 0) {
				n = fp-genbuf;
				if(kflag)
					crblock(perm, genbuf, n, count-n);
				if(write(io, genbuf, n) != n)
					error(29);
				nib = 511;
				fp = genbuf;
			}
			count++;
			if ((*fp++ = *lp++) == 0) {
				fp[-1] = '\n';
				break;
			}
		}
	} while (a1 <= addr2);
	n = fp-genbuf;
	if(kflag)
		crblock(perm, genbuf, n, count-n);
	if(write(io, genbuf, n) != n)
		error(29);
}

append(f, a)
LINE a;
int (*f)();
{
	register LINE a1, a2, rdot;
	int tl;

	nline = 0;
	dot = a;
	while ((*f)() == 0) {
		if (dol >= endcore) {
			if ((int)sbrk(512*sizeof(struct lin)) == -1) {
				lastc = '\n';
				error(30);
			}
			endcore += 512;
		}
		tl = putline();
		nline++;
		a1 = ++dol;
		a2 = a1+1;
		rdot = ++dot;
		while (a1 > rdot)
			(--a2)->cur = (--a1)->cur;
		rdot->cur = tl;
	}
}

unixcom()
{
	register (*savint)(), pid, rpid;
	int retcode;
	static char savcmd[LBSIZE];	/* last command */
	char curcmd[LBSIZE];		/* current command */
	char *psavcmd, *pcurcmd, *psavedfile;
	register c, endflg=1, shflg=0;

	setnoaddr();
	if(rflg)
		error(6);
	pcurcmd = curcmd;
	/* read command til end */
	/* a '!' found in beginning of command is replaced with the saved command.
	   a '%' found in command is replaced with the current filename */
	c=getchr();
	if (c == '!') {
		if (savcmd[0]==0) 
			error(56);
		else {
			psavcmd = savcmd;
			while (*pcurcmd++ = *psavcmd++);
			--pcurcmd;
			shflg = 1;
		}
	}
	else UNGETC(c);  /* put c back */
	while (endflg==1) {
		while ((c=getchr()) != '\n' && c != '%' && c != '\\')
			*pcurcmd++ = c;
		if (c=='%') { 
			if (savedfile[0]==0)
				error(21);
			else {
				psavedfile = savedfile;
				while(*pcurcmd++ = *psavedfile++);
				--pcurcmd;
				shflg = 1;
			}
		}
		else if (c == '\\') {
			c = getchr();
			if (c != '%')
				*pcurcmd++ = '\\';
			*pcurcmd++ = c;
		}
		else
			/* end of command hit */
			endflg = 0;
	}
	*pcurcmd++ = 0;
	if (shflg == 1)
		puts(curcmd);
	/* save command */
	strcpy(savcmd,curcmd);

	if ((pid = fork()) == 0) {
		signal(SIGHUP, oldhup);
		signal(SIGQUIT, oldquit);
		execlp("/bin/sh", "sh", "-c", curcmd, (char *) 0);
		exit(0100);
	}
	savint = signal(SIGINT, SIG_IGN);
	while ((rpid = wait(&retcode)) != pid && rpid != -1);
	signal(SIGINT, savint);
	if (vflag) puts("!");
}

quit()
{
	if (vflag && fchange) {
		fchange = 0;
		error(1);
	}
	unlink(tfname);
	exit(errcnt? 2: 0);
}

delete()
{
	setdot();
	newline();
	nonzero();
	if (!globflg) save();
	rdelete(addr1, addr2);
}

rdelete(ad1, ad2)
LINE ad1, ad2;
{
	register LINE a1, a2, a3;

	a1 = ad1;
	a2 = ad2+1;
	a3 = dol;
	dol -= a2 - a1;
	do
		a1++->cur = a2++->cur;
	while (a2 <= a3);
	a1 = ad1;
	if (a1 > dol)
		a1 = dol;
	dot = a1;
	fchange = 1;
}

gdelete()
{
	register LINE a1, a2, a3;

	a3 = dol;
	for (a1=zero+1; (a1->cur&01)==0; a1++)
		if (a1>=a3)
			return;
	for (a2=a1+1; a2<=a3;) {
		if (a2->cur&01) {
			a2++;
			dot = a1;
		} else
			a1++->cur = a2++->cur;
	}
	dol = a1-1;
	if (dot>dol)
		dot = dol;
	fchange = 1;
}

char *
getline(tl)
{
	register char *bp, *lp;
	register nl;

	lp = linebuf;
	bp = getblock(tl, READ);
	nl = nleft;
	tl &= ~0377;
	while (*lp++ = *bp++)
		if (--nl == 0) {
			bp = getblock(tl+=0400, READ);
			nl = nleft;
		}
	return(linebuf);
}

putline()
{
	register char *bp, *lp;
	register nl;
	int tl;

	fchange = 1;
	lp = linebuf;
	tl = tline;
	bp = getblock(tl, WRITE);
	nl = nleft;
	tl &= ~0377;
	while (*bp = *lp++) {
		if (*bp++ == '\n') {
			*--bp = 0;
			linebp = lp;
			break;
		}
		if (--nl == 0) {
			bp = getblock(tl+=0400, WRITE);
			nl = nleft;
		}
	}
	nl = tline;
	tline += (((lp-linebuf)+03)>>1)&077776;
	return(nl);
}

char *
getblock(atl, iof)
{
	extern read(), write();
	register bno, off;
	register char *p1, *p2;
	register int n;
	
	bno = (atl>>8)&0377;
	off = (atl<<1)&0774;
	if (bno >= 255) {
		lastc = '\n';
		error(31);
	}
	nleft = 512 - off;
	if (bno==iblock) {
		ichanged |= iof;
		return(ibuff+off);
	}
	if (bno==oblock)
		return(obuff+off);
	if (iof==READ) {
		if (ichanged) {
			if(xtflag)
				crblock(tperm, ibuff, 512, (long)0);
			blkio(iblock, ibuff, write);
		}
		ichanged = 0;
		iblock = bno;
		blkio(bno, ibuff, read);
		if(xtflag)
			crblock(tperm, ibuff, 512, (long)0);
		return(ibuff+off);
	}
	if (oblock>=0) {
		if(xtflag) {
			p1 = obuff;
			p2 = crbuf;
			n = 512;
			while(n--)
				*p2++ = *p1++;
			crblock(tperm, crbuf, 512, (long)0);
			blkio(oblock, crbuf, write);
		} else
			blkio(oblock, obuff, write);
	}
	oblock = bno;
	return(obuff+off);
}

blkio(b, buf, iofcn)
char *buf;
int (*iofcn)();
{
	lseek(tfile, (long)b<<9, 0);
	if ((*iofcn)(tfile, buf, 512) != 512) {
		error(32);
	}
}

init()
{
	register *markp;
	int omask;

	close(tfile);
	tline = 2;
	for (markp = names; markp < &names[26]; )
		*markp++ = 0;
	subnewa = 0;
	anymarks = 0;
	iblock = -1;
	oblock = -1;
	ichanged = 0;
	initflg = 1;
	omask = umask(0);
	close(creat(tfname, 0600));
	umask(omask);
	tfile = open(tfname, 2);
	if(xflag) {
		xtflag = 1;
		makekey(key, tperm);
	}
	brk((char *)fendcore);
	dot = zero = dol = savdot = savdol = fendcore;
	endcore = fendcore - sizeof(struct lin);
}

global(k)
{
	register char *gp;
	register c;
	register LINE a1;
	char globuf[GBSIZE];

	if (globp)
		error(33);
	setall();
	nonzero();
	if ((c=getchr())=='\n')
		error(19);
	save();
	compile((char *) 0, expbuf, &expbuf[ESIZE], c);
	gp = globuf;
	while ((c = getchr()) != '\n') {
		if (c==EOF)
			error(19);
		if (c=='\\') {
			c = getchr();
			if (c!='\n')
				*gp++ = '\\';
		}
		*gp++ = c;
		if (gp >= &globuf[GBSIZE-2])
			error(34);
	}
	if (gp == globuf)
		*gp++ = 'p';
	*gp++ = '\n';
	*gp++ = 0;
	for (a1=zero; a1<=dol; a1++) {
		a1->cur &= ~01;
		if (a1>=addr1 && a1<=addr2 && execute(0, a1)==k)
			a1->cur |= 01;
	}
	/*
	 * Special case: g/.../d (avoid n^2 algorithm)
	 */
	if (globuf[0]=='d' && globuf[1]=='\n' && globuf[2]=='\0') {
		gdelete();
		return;
	}
	for (a1=zero; a1<=dol; a1++) {
		if (a1->cur & 01) {
			a1->cur &= ~01;
			dot = a1;
			globp = globuf;
			globflg = 1;
			commands();
			globflg = 0;
			a1 = zero;
		}
	}
}

join()
{
	register char *gp, *lp;
	register LINE a1;

	if (addr1 == addr2) return;
	gp = genbuf;
	for (a1=addr1; a1<=addr2; a1++) {
		lp = getline(a1->cur);
		while (*gp = *lp++)
			if (gp++ >= &genbuf[LBSIZE-2])
				error(27);
	}
	lp = linebuf;
	gp = genbuf;
	while (*lp++ = *gp++);
	addr1->cur = putline();
	if (addr1<addr2)
		rdelete(addr1+1, addr2);
	dot = addr1;
}

substitute(inglob)
{
	register gsubf, nl;
	register LINE a1;
	int *markp;
	int getsub();

	gsubf = compsub();
	for (a1 = addr1; a1 <= addr2; a1++) {
		if (execute(0, a1)==0)
			continue;
		inglob |= 01;
		dosub();
		if (gsubf) {
			while (*loc2) {
				if (execute(1, (LINE )0)==0)
					break;
				dosub();
			}
		}
		subnewa = putline();
		a1->cur &= ~01;
		if (anymarks) {
			for (markp = names; markp < &names[26]; markp++)
				if (*markp == a1->cur)
					*markp = subnewa;
		}
		a1->cur = subnewa;
		append(getsub, a1);
		nl = nline;
		a1 += nl;
		addr2 += nl;
	}
	if (inglob==0)
		error(35);
}

compsub()
{
	register seof, c;
	register char *p;
	static char remem[LBSIZE]={-1};

	if ((seof = getchr()) == '\n' || seof == ' ')
		error(36);
	compile((char *) 0, expbuf, &expbuf[ESIZE], seof);
	p = rhsbuf;
	for (;;) {
		c = getchr();
		if (c=='\\')
			c = getchr() | 0200;
		if (c=='\n') {
			if (nodelim == 1) {
				nodelim = 0;
				error(36);
			}
			if (globp && globp[0])
				c |= 0200;	/* insert '\' */
			else {
				UNGETC(c);
				pflag++;
				break;
			}
		}
		if (c==seof)
			break;
		*p++ = c;
		if (p >= &rhsbuf[LBSIZE])
			error(38);
	}
	*p++ = 0;
	if(rhsbuf[0] == '%' && rhsbuf[1] == 0)
		(remem[0]!=-1) ? strcpy(rhsbuf, remem) : error(55);
	else
		strcpy(remem, rhsbuf);
	if ((peekc = getchr()) == 'g') {
		peekc = 0;
		newline();
		return(1);
	}
	newline();
	return(0);
}

getsub()
{
	register char *p1, *p2;

	p1 = linebuf;
	if ((p2 = linebp) == 0)
		return(EOF);
	while (*p1++ = *p2++);
	linebp = 0;
	return(0);
}

dosub()
{
	register char *lp, *sp, *rp;
	int c;

	lp = linebuf;
	sp = genbuf;
	rp = rhsbuf;
	while (lp < loc1)
		*sp++ = *lp++;
	while (c = *rp++&0377) {
		if (c=='&') {
			sp = place(sp, loc1, loc2);
			continue;
		} else if(c & 0200) {
			c &= 0177;
			if(c >= '1' && c < nbra + '1') {
				sp = place(sp, braslist[c-'1'], braelist[c-'1']);
				continue;
			}
		}
		*sp++ = c;
		if (sp >= &genbuf[LBSIZE])
			error(27);
	}
	lp = loc2;
	loc2 = sp - genbuf + linebuf;
	while (*sp++ = *lp++)
		if (sp >= &genbuf[LBSIZE])
			error(27);
	lp = linebuf;
	sp = genbuf;
	while (*lp++ = *sp++);
}

char *
place(sp, l1, l2)
register char *sp, *l1, *l2;
{

	while (l1 < l2) {
		*sp++ = *l1++;
		if (sp >= &genbuf[LBSIZE])
			error(27);
	}
	return(sp);
}

move(cflag)
{
	register LINE adt, ad1, ad2;
	int getcopy();

	setdot();
	nonzero();
	if ((adt = address())==0)
		error(39);
	newline();
	if (!globflg) save();
	if (cflag) {
		ad1 = dol;
		append(getcopy, ad1++);
		ad2 = dol;
	} else {
		ad2 = addr2;
		for (ad1 = addr1; ad1 <= ad2;)
			ad1++->cur &= ~01;
		ad1 = addr1;
	}
	ad2++;
	if (adt<ad1) {
		dot = adt + (ad2-ad1);
		if ((++adt)==ad1)
			return;
		reverse(adt, ad1);
		reverse(ad1, ad2);
		reverse(adt, ad2);
	} else if (adt >= ad2) {
		dot = adt++;
		reverse(ad1, ad2);
		reverse(ad2, adt);
		reverse(ad1, adt);
	} else
		error(39);
	fchange = 1;
}

reverse(a1, a2)
register LINE a1, a2;
{
	register int t;

	for (;;) {
		t = (--a2)->cur;
		if (a2 <= a1)
			return;
		a2->cur = a1->cur;
		a1++->cur = t;
	}
}

getcopy()
{

	if (addr1 > addr2)
		return(EOF);
	getline(addr1++->cur);
	return(0);
}


error1(code)
{
	expbuf[0] = 0;
	nbra = 0;
	error(code);
}

execute(gf, addr)
LINE addr;
{
	register char *p1, *p2, c;

	for (c=0; c<NBRA; c++) {
		braslist[c] = 0;
		braelist[c] = 0;
	}
	if (gf) {
		if (circf)
			return(0);
		p1 = linebuf;
		p2 = genbuf;
		while (*p1++ = *p2++);
		locs = p1 = loc2;
	} else {
		if (addr==zero)
			return(0);
		p1 = getline(addr->cur);
		locs = 0;
	}
	return(step(p1, expbuf));
}


putd()
{
	register r;

	r = (int)(count%10);
	count /= 10;
	if (count)
		putd();
	putchr(r + '0');
}

puts(sp)
register char *sp;
{
#ifdef PWB
	int sz,i;
	if (fss.Ffill && (listf == 0)) {
		if ((i = expnd(sp,funny,&sz,&fss)) == -1) {
			write(1,funny,fss.Flim & 0377); putchr('\n');
			write(1,"too long",8);
		}
		else
			write(1,funny,sz);
		putchr('\n');
		if (i == -2) write(1,"tab count\n",10);
		return(0);
	}
#endif
	col = 0;
	while (*sp)
		putchr(*sp++);
	putchr('\n');
}

char	line[70];
char	*linp = line;

putchr(ac)
{
	register char *lp;
	register c;
	short len;

	lp = linp;
	c = ac;
	if ( listf ) {
		col++;
		if (col >= 72) {
			col = 0;
			*lp++ = '\\';
			*lp++ = '\n';
		}
		if (c=='\t') {
			c = '>';
			goto esc;
		}
		if (c=='\b') {
			c = '<';
		esc:
			*lp++ = '-';
			*lp++ = '\b';
			*lp++ = c;
			goto out;
		}
		if (c<' ' && c!= '\n') {
			*lp++ = '\\';
			*lp++ = (c>>3)+'0';
			*lp++ = (c&07)+'0';
			col += 2;
			goto out;
		}
	}
	*lp++ = c;
out:
	if(c == '\n' || lp >= &line[64]) {
		linp = line;
		len = lp - line;
		if(yflag & 01)
			write(1, &len, sizeof(len));
		write(1, line, len);
		return;
	}
	linp = lp;
}
crblock(permp, buf, nchar, startn)
char	*permp;
char	*buf;
long	startn;
{
	register char	*p1;
	register int n1, n2;
	register char	*t1, *t2, *t3;

	t1 = permp;
	t2 = &permp[256];
	t3 = &permp[512];

	n1 = (int)(startn&0377);
	n2 = (int)((startn>>8)&0377);
	p1 = buf;
	while(nchar--) {
		*p1 = t2[(t3[(t1[(*p1+n1)&0377]+n2)&0377]-n2)&0377]-n1;
		n1++;
		if(n1==256){
			n1 = 0;
			n2++;
			if(n2==256) n2 = 0;
		}
		p1++;
	}
}

getkey()
{
	struct termio b;
	int save;
	int (*signal())();
	int (*sig)();
	char *p;
	int c;

	sig = signal(SIGINT, SIG_IGN);
	ioctl(0, TCGETA, &b);
	save = b.c_lflag;
	b.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL);
	ioctl(0, TCSETA, &b);
	write(1, "Enter file encryption key: ", 27);
	p = key;
	while(((c=getchr()) != EOF) && (c!='\n')) {
		if(p < &key[KSIZE])
			*p++ = c;
	}
	*p = 0;
	write(1, "\n", 1);
	b.c_lflag = save;
	ioctl(0, TCSETA, &b);
	signal(SIGINT, sig);
	/* this line used to say "return(key[0] != 0);" */
}

/*
 * Besides initializing the encryption machine, this routine
 * returns 0 if the key is null, and 1 if it is non-null.
 */
crinit(keyp, permp)
char	*keyp, *permp;
{
	register char *t1, *t2, *t3;
	int ic, i, j, k, temp, pf[2];
	unsigned random;
	char buf[13];
	long seed;

	if (yflag)
		error(59);
	t1 = permp;
	t2 = &permp[256];
	t3 = &permp[512];

	if (*keyp == 0)
		return(0);

	strncpy(buf, keyp, 8);
	while (*keyp)
		*keyp++ = '\0';
	buf[8] = buf[0];
	buf[9] = buf[1];
	if(pipe(pf) < 0)
		error(0);
	i = fork();
	if(i == -1)
		error(23);
	if(i == 0) {
		close(0);
		close(1);
		dup(pf[0]);
		dup(pf[1]);
		execl("/usr/lib/makekey", "-", (char *) 0, (char *) 0, (char *) 0);
		execl("/lib/makekey", "-", (char *) 0, (char *) 0, (char *) 0);
		exit(2);
	}
	write(pf[1], buf, 10);
	if (wait((int *) 0)== -1 || read(pf[0], buf, 13)!=13) {
		puts("crypt: cannot generate key");
		exit(2);
	}
	close(pf[0]);
	close(pf[1]);
	seed = 123;
	for (i=0; i<13; i++)
		seed = seed*buf[i] + i;
	for(i=0;i<256;i++) {
		t1[i] = i;
		t3[i] = 0;
	}
	for(i=0;i<256;i++) {
		seed = 5*seed + buf[i%13];
		random = (int)(seed % 65521);
		k = 256-1 -i;
		ic = (random&0377)%(k+1);
		random >>= 8;
		temp = t1[k];
		t1[k] = t1[ic];
		t1[ic] = temp;
		if(t3[k]!=0) continue;
		ic = (random&0377) % k;
		while(t3[ic]!=0) ic = (ic+1) % k;
		t3[k] = ic;
		t3[ic] = k;
	}
	for(i=0;i<256;i++)
		t2[t1[i]&0377] = i;
	return(1);
}

makekey(a, b)
char *a, *b;
{
	register int i;
	long gorp;
	char temp[KSIZE + 1];

	for(i = 0; i < KSIZE; i++)
		temp[i] = *a++;
	time(&gorp);
	gorp += getpid();

	for(i = 0; i < 4; i++)
		temp[i] ^= (char)((gorp>>(8*i))&0377);

	i = crinit(temp, b);
}
globaln(k)
{
	register char *gp;
	register c;
	register LINE a1;
	int  nfirst, pr;
	char globuf[GBSIZE];

	if (yflag)
		error(59);
	if (globp)
		error(33);
	setall();
	nonzero();
	if ((c=getchr())=='\n')
		error(19);
	save();
	compile((char *) 0, expbuf, &expbuf[ESIZE], c);
	for (a1=zero; a1<=dol; a1++) {
		a1->cur &= ~01;
		if (a1>=addr1 && a1<=addr2 && execute(0, a1)==k)
			a1->cur |= 01;
	}
	nfirst = 0;
	newline();
	for (a1=zero; a1<=dol; a1++) {
		if (a1->cur & 01) {
			a1->cur &= ~01;
			dot = a1;
			puts(getline(a1->cur));
			if ((c=getchr()) == EOF)
				error(52);
			if(c=='a' || c=='i' || c=='c')
				error(53);
			c &= 0177;
			if (c == '\n') {
				a1 = zero;
				continue;
			}
			if (c != '&') {
				gp = globuf;
				*gp++ = c;
				while ((c = getchr()) != '\n') {
					if (c=='\\') {
						c = getchr();
						if (c!='\n')
							*gp++ = '\\';
					}
					*gp++ = c;
					if (gp >= &globuf[GBSIZE-2])
						error(34);
				}
				*gp++ = '\n';
				*gp++ = 0;
				nfirst = 1;
			}
			else
				if ((c=getchr()) != '\n')
					error(54);
			globp = globuf;
			if (nfirst) {
				globflg = 1;
				commands();
				globflg = 0;
			}
			else error(56);
			globp = 0;
			a1 = zero;
		}
	}
}
eopen(string, rw)
char *string;
{
#define w_or_r(a,b) (rw?a:b)
	int pf[2];
	int i;
	int io;

	if (rflg) {	/* restricted shell */
		if (Xqt) {
			Xqt = 0;
			error(6);
		}
	}
	if(!Xqt) {
		if((io=open(string, rw)) >= 0) {
			if (fflg) {
				read(io,funny,LBSIZE);
				if (fspec(funny,&fss,0) < 0) {
					fss.Ffill = 0;
					fflg = 0;
					error(4);
				}
				lseek(io,0L,0);
			}
		}
		fflg = 0;
		return(io);
	}
	if(pipe(pf) < 0)
xerr:		error(0);
	if((i = fork()) == 0) {
		signal(SIGHUP, oldhup);
		signal(SIGQUIT, oldquit);
		signal(SIGPIPE, oldpipe);
		signal(SIGINT, (int (*)()) 0);
		close(w_or_r(pf[1], pf[0]));
		close(w_or_r(0, 1));
		dup(w_or_r(pf[0], pf[1]));
		close(w_or_r(pf[0], pf[1]));
		execlp("/bin/sh", "sh", "-c", string, (char *) 0);
		exit(1);
	}
	if(i == -1)
		goto xerr;
	close(w_or_r(pf[0], pf[1]));
	return w_or_r(pf[1], pf[0]);
}

eclose(f)
{
	close(f);
	if(Xqt)
		Xqt = 0, wait((int *) 0);
}
mkfunny()
{
	register char *p, *p1, *p2;

	p2 = p1 = funny;
	p = file;
	while(*p)
		p++;
	while(*--p  == '/')	/* delete trailing slashes */
		*p = '\0';
	p = file;
	while (*p1++ = *p)
		if (*p++ == '/') p2 = p1;
	p1 = &tfname[6];
	*p2 = '\007';	/* add unprintable char to make funny a unique name */
	while (p1 <= &tfname[11])
		*++p2 = *p1++;
}

getime() /* get modified time of file and save */
{
	if (stat(file,&Fl) < 0)
		savtime = 0;
	else
		savtime = Fl.st_mtime;
}

chktime() /* check saved mod time against current mod time */
{
	if (savtime != 0 && Fl.st_mtime != 0) {
		if (savtime != Fl.st_mtime)
			error(58);
	}
}

newtime() /* get new mod time and save */
{
	stat(file,&Fl);
	savtime = Fl.st_mtime;
}

red(op) /* restricted - check for '/' in name */
        /* and delete trailing '/' */
char *op;
{
	register char *p;

	p = op;
	while(*p)
		if(*p++ == '/'&& rflg) {
			*op = 0;
			error(6);
		}
	/* delete trailing '/' */
	while(p > op) {
		if (*--p == '/')
			*p = '\0';
		else break;
	}
}

#ifdef PWB

char *fsp, fsprtn;

fspec(line,f,up)
char line[];
struct Fspec *f;
int up;
{
	struct termio arg;
	register int havespec, n;

	if(!up) clear(f);

	havespec = fsprtn = 0;
	for(fsp=line; *fsp && *fsp != '\n'; fsp++)
		switch(*fsp) {

			case '<':       if(havespec) return(-1);
					if(*(fsp+1) == ':') {
						havespec = 1;
						clear(f);
						if(!ioctl(1, TCGETA, &arg) &&
							((arg.c_oflag&TAB3) == TAB3))
						  f->Ffill = 1;
						fsp++;
						continue;
					}

			case ' ':       continue;

			case 's':       if(havespec && (n=numb()) >= 0)
						f->Flim = n;
					continue;

			case 't':       if(havespec) targ(f);
					continue;

			case 'd':       continue;

			case 'm':       if(havespec)  n = numb();
					continue;

			case 'e':       continue;
			case ':':       if(!havespec) continue;
					if(*(fsp+1) != '>') fsprtn = -1;
					return(fsprtn);

			default:	if(!havespec) continue;
					return(-1);
		}
	return(1);
}


numb()
{
	register int n;

	n = 0;
	while(*++fsp >= '0' && *fsp <= '9')
		n = 10*n + *fsp-'0';
	fsp--;
	return(n);
}


targ(f)
struct Fspec *f;
{
	register int n;

	if(*++fsp == '-') {
		if(*(fsp+1) >= '0' && *(fsp+1) <= '9') tincr(numb(),f);
		else tstd(f);
		return;
	}
	if(*fsp >= '0' && *fsp <= '9') {
		tlist(f);
		return;
	}
	fsprtn = -1;
	fsp--;
	return;
}


tincr(n,f)
int n;
struct Fspec *f;
{
	register int l, i;

	l = 1;
	for(i=0; i<20; i++)
		f->Ftabs[i] = l += n;
	f->Ftabs[i] = 0;
}


tstd(f)
struct Fspec *f;
{
	char std[3];

	std[0] = *++fsp;
	if (*(fsp+1) >= '0' && *(fsp+1) <= '9')  {
						std[1] = *++fsp;
						std[2] = '\0';
	}
	else std[1] = '\0';
	fsprtn = stdtab(std,f->Ftabs);
	return;
}


tlist(f)
struct Fspec *f;
{
	register int n, last, i;

	fsp--;
	last = i = 0;

	do {
		if((n=numb()) <= last || i >= 20) {
			fsprtn = -1;
			return;
		}
		f->Ftabs[i++] = last = n;
	} while(*++fsp == ',');

	f->Ftabs[i] = 0;
	fsp--;
}


expnd(line,buf,sz,f)
char line[], buf[];
int *sz;
struct Fspec *f;
{
	register char *l, *t;
	register int b;

	l = line - 1;
	b = 1;
	t = f->Ftabs;
	fsprtn = 0;

	while(*++l && *l != '\n' && b < 511) {
		if(*l == '\t') {
			while(*t && b >= *t) t++;
			if (*t == 0) fsprtn = -2;
			do buf[b-1] = ' '; while(++b < *t);
		}
		else buf[b++ - 1] = *l;
	}

	buf[b] = '\0';
	*sz = b;
	if(*l != '\0' && *l != '\n') {
		buf[b-1] = '\n';
		return(-1);
	}
	buf[b-1] = *l;
	if(f->Flim && b-1 > f->Flim) return(-1);
	return(fsprtn);
}


clear(f)
struct Fspec *f;
{
	f->Ftabs[0] = f->Fdel = f->Fmov = f->Ffill = 0;
	f->Flim = 0;
}
lenchk(line,f)
char line[];
struct Fspec *f;
{
	register char *l, *t;
	register int b;

	l = line - 1;
	b = 1;
	t = f->Ftabs;

	while(*++l && *l != '\n' && b < 511) {
		if(*l == '\t') {
			while(*t && b >= *t) t++;
			while(++b < *t);
		}
		else b++;
	}

	if((*l!='\0'&&*l!='\n') || (f->Flim&&b-1>f->Flim))
		return(-1);
	return(0);
}
#define NTABS 21

/*      stdtabs: standard tabs table
	format: option code letter(s), null, tabs, null */
char stdtabs[] = {
'a',    0,1,10,16,36,72,0,      		/* IBM 370 Assembler */
'a','2',0,1,10,16,40,72,0,      		/* IBM Assembler alternative*/
'c',    0,1,8,12,16,20,55,0,    		/* COBOL, normal */
'c','2',0,1,6,10,14,49,0,       		/* COBOL, crunched*/
'c','3',0,1,6,10,14,18,22,26,30,34,38,42,46,50,54,58,62,67,0,
'f',    0,1,7,11,15,19,23,0,    		/* FORTRAN */
'p',    0,1,5,9,13,17,21,25,29,33,37,41,45,49,53,57,61,0, /* PL/I */
's',    0,1,10,55,0,    			/* SNOBOL */
'u',    0,1,12,20,44,0, 			/* UNIVAC ASM */
0};
/*      stdtab: return tab list for any "canned" tab option.
	entry: option points to null-terminated option string
		tabvect points to vector to be filled in
	exit: return(0) if legal, tabvect filled, ending with zero
		return(-1) if unknown option
*/

stdtab(option,tabvect)
char option[], tabvect[NTABS];
{
	char *scan;
	tabvect[0] = 0;
	scan = stdtabs;
	while (*scan)
		{
		if (strequal(&scan,option))
			{strcopy(scan,tabvect);break;}
		else while(*scan++);    /* skip over tab specs */
		}
/*      later: look up code in /etc/something */
	return(tabvect[0]?0:-1);
}

/*      strequal: checks strings for equality
	entry: scan1 points to scan pointer, str points to string
	exit: return(1) if equal, return(0) if not
		*scan1 is advanced to next nonzero byte after null
*/
strequal(scan1,str)
char **scan1, *str;
{
	char c, *scan;
	scan = *scan1;
	while ((c = *scan++) == *str && c) str++;
	*scan1 = scan;
	if (c == 0 && *str == 0) return(1);
	if (c) while(*scan++);
	*scan1 = scan;
	return(0);
}

/*      strcopy: copy source to destination */

strcopy(source,dest)
char *source, *dest;
{
	while (*dest++ = *source++);
	return;
}
#endif

/* This is called before a buffer modifying command so that the */
/* current array of line ptrs is saved in sav and dot and dol are saved */
save() {
	LINE i;

	savdot = dot;
	savdol = dol;
	for (i=zero+1; i<=dol; i++)
		i->sav = i->cur;
	initflg = 0;
}

/* The undo command calls this to restore the previous ptr array sav */
/* and swap with cur - dot and dol are swapped also. This allows user to */
/* undo an undo */
undo() {
	int tmp;
	LINE i, tmpdot, tmpdol;

	tmpdot = dot; dot = savdot; savdot = tmpdot;
	tmpdol = dol; dol = savdol; savdol = tmpdol;
	/* swap arrays using the greater of dol or savdol as upper limit */
	for (i=zero+1; i<=((dol>savdol) ? dol : savdol); i++) {
		tmp = i->cur;
		i->cur = i->sav;
		i->sav = tmp;
	}
}
