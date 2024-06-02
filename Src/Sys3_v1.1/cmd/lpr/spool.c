#include	<signal.h>
#include	<sys/types.h>
#include	<sys/stat.h>

#define	ONL	0
#define	TOSS	1
			/*GCOSA = GCOS job with ASCII data.*/
			/*GCOSB = GCOS job with BCD data.*/
#define	GCOSA	1
#define	GCOSB	2
#define	FGET	3
#define	LPR	4

#define	GCAT	1		/*flag for gcat command*/
#define	MAIL	2		/*flag for GCOS mail command.*/


#if SPTYPE == LPR

#ifndef	SPDIR
#define	SPDIR	"/usr/spool/lpd"
#endif
#ifndef	DAEMON
#define	DAEMON	"/usr/lib/lpd"
#endif
#ifndef	DAEMON2
#define	DAEMON2	"/etc/lpd"
#endif

#else

#ifndef	SPDIR
#define	SPDIR	"/usr/spool/dpd"
#endif
#ifndef	DAEMON
#define	DAEMON	"/usr/lib/dpd"
#endif
#ifndef	DAEMON2
#define	DAEMON2	"/etc/dpd"
#endif

#endif


int	INCHAR	= 0;		/*index of incremented character in
					temporary file names. */
char	version[] = "Version 8/8/79";
char	grade;
char	remote[]= "$	remote	**,onl";
char	toss[]	= "$	sysout	toss";
int	remotsw;		/*toss-output flag*/
char	*mailfile = 0;
#ifndef STOCKIII
char	wantbanner = 1;		/* user wants the banner */
#endif
char	wantmail = 0;
char	*pp	= 0;		/*recipient of mail*/
char	*identf = 0;		/*ident card info*/
int	uidf	= 0;
char	gcosid[13];		/*gcos userid*/
char	cpflag = 'l';		/*copy/link flag*/
int	rmflag	= 0;		/*remove flag*/
int	debug	= 0;
int	gcdebug	= 0;		/*GCOS debug switch*/
int	archsw = 0;		/*archive switch*/

int	argc;
char	**argv;
char	*arg;
int	nact = 0;		/*number of non-null files to process.*/
int	gsize	= 20;		/*size of current file in GCOS blocks.*/
long	usize	= 20*1200;	/*size of current file in bytes.*/
#define	CARDSIZE	83
char	idcard[CARDSIZE];	/*$ IDENT card image.*/
int	iacct, iname;		/*index of account number, user name in idcard.*/
char	cardbuf[CARDSIZE];	/*for use by callers of card() and copy().*/
FILE	*tff;		/*temporary control card file*/
FILE	*nfile();
char	*getarg();
char	*strchr();


comopt(o)		/*routine to test for common options.*/
char o;
{
	switch (o){

	case 'c':
		cpflag = 'c';
		break;

	case 'f':		/*option to set filename for mail. MRW*/
		mailfile = getarg('f');
		wantmail = 1;
		break;

	case 'i':
		identf = getarg('i');
		break;

	case 'm':
		wantmail++;
		if(arg[2])
			pp = &arg[2];
		break;
	
	case 'n':		/*new option to suppress mail. MRW*/
		wantmail = 0;
		break;

	case 'o':
		remotsw = ONL;
		break;

	case 'r':
		rmflag++;
		break;

	case 's':
		if(arg[2] < '1' || arg[2] > '3')
			goto unknown;
		grade = arg[2];
		break;

	case 't':
		if(arg[2])
			goto unknown;
		remotsw = TOSS;
		break;

	case '#':
		debug = 1;
		break;

	case 'Z':			/*GCOS debugging switch*/
		gcdebug = 1;
		break;

unknown:
	default:
		return(0);
	}
	return(1);
}


char *
getarg(c)		/*get modifier for complex options --
			    from either same or next argument. MRW
			    e.g. either "-ffile" or "-f file"*/
char	c;
{

	if(arg[2])
		return(&arg[2]);
	else if(--argc>1)
		return(arg = (++argv)[1]);
	errout("Incomplete -%c option",c);
}

#if SPTYPE != LPR

gcos1()		/*set up common initial GCOS control cards.*/
{
	if(debug)
		tff = stdout;
	else
		if((tff = nfile(tfname)) == NULL)
			errout("Can't create %s.", tfname);
	card('S', "");
	card('L', "$	sgrade	%c   %s", grade, version );
	if(ident())
		out();
	card('L', remote);
	if(remotsw == TOSS)
		card('L', toss);
}


gcos2()		/*add final control cards, and spool job.*/
{
	if(wantmail)
		card('N', mailfile);
	card('L', "$	endjob");
	if(debug)
		out();
	fclose(tff);
	if(nact) {
		dfname[INCHAR]++;
		if(link(tfname, dfname) < 0)
			errout("Cannot rename %s", tfname);
		unlink(tfname);
		execl(DAEMON, DAEMON, 0);
		execl(DAEMON2, DAEMON2, 0);
		err("Can't find %s.\nFiles left in spooling dir.", DAEMON);
		exit(1);
	}
	errout("No data; job deleted.");
}

#endif


FILE *nfile(name)		/*generate a new file name, and open file.*/
char *name;
{
	FILE *f;

	if(name[INCHAR] >= 'z')
		return(NULL);
	name[INCHAR]++;
	if(!access(name, 0) || (f = fopen(name, "w")) == NULL)
		return(NULL);
	return(f);
}

#if SPTYPE == GCOSA || SPTYPE == LPR

filargs()		/*process file arguments for dpr, gcat, fsend, lpr.*/
{
	int i;
	FILE *f;

	if(argc == 1){
		if(mailfile == 0)
			mailfile = "pipe.end";
		if(copy(stdin, mailfile, GCATSW) == -1)
			out();
		if(archsw)
			archive();
	}
	while(--argc) {
		arg = *++argv;
		if(size(arg,arg) <= 0)
			continue;
		switch(cpflag){

		case 'l':
			if(lfname[INCHAR]++ >= 'z')
				cpflag = rmflag ? 'c' : 'n';
			else if(link(arg, lfname) == 0){
				nuact(arg, arg);
				card(BF, lfname);
				card('U', lfname);
				break;
			}

		case 'n':
			if(*arg == '/' && !rmflag){
				nuact(arg, arg);
				card(BF, arg);
				break;
			}

		case 'c':
			f = fopen(arg, "r");
			if(f == NULL){
				err("Cannot open %s", arg);
				continue;
			}
			i = copy(f, arg, GCATSW);
			fclose(f);
			if(i == -1)
				continue;
			break;
		}
		if(archsw)
			archive();
		if(rmflag){
			if(unlink(arg) < 0)
				err("Cannot remove %s", arg);
		}
		if(mailfile == 0)
			mailfile = arg;
	}
}


 
copy(f, gname, spsw)
FILE	*f;
char	*gname;
int	spsw;
{
	int c;
	FILE *ff;
/* try removing limit on copy file .. see if anyone objects.
	long cnt;

	cnt = 0;
 * remove limit on copy file.*/
	if((ff = nfile(cfname)) == NULL){
		err("Too many copy files; %s not copied", gname);
		return(-1);
	}
	if(spsw == MAIL){
		fprintf(ff, cardbuf);
		spsw = 0;
	}
	while((c = getc(f)) != EOF){
		if(spsw == GCAT)
		    if(c != 0)
			errout("Bad input from %s.", gname);
		    else
			spsw = 0;
		if((putc(c, ff) == EOF) && ferror(ff)){
			err("Write error on copy of %s.", gname);
			break;
		}
/* try removing limit on copy file .. see if anyone objects.
		cnt++;
		if(cnt > MAXCOPY){
			err("Copy file %s is too large", gname);
			break;
		}
 * remove limit on copy file.*/
	}
	fclose(ff);
	if(size(gname, cfname) <= 0){
		unlink(cfname);
		return(-1);
	}
	nuact(gname, cfname);
	card(BF, cfname);
	card('U', cfname);
	return(0);
}

size(name, temp)
char	*temp, *name;
{
	struct stat stbuf;

	if(stat(temp,&stbuf) < 0){
		err("Cannot open %s", temp);
		return(-1);
	}
	if(!stbuf.st_size){
		err("File %s is empty.", name);
		return(0);
	}
	usize = stbuf.st_size;
	gsize = usize / 1200;
	gsize++;
	nact++;
	return(gsize);
}


#endif

#if SPTYPE == GCOSB

copyfile(fin, p)	/* precede every line with a letter L */
FILE	*fin;
char	*p;
{
	char *cp;

	while( fgets(cardbuf,CARDSIZE, fin) ){
		if((cp = strchr(cardbuf, '\n')) == NULL)
			errout("Line too long in file %s.", p);
		*cp = '\0';
		card('L', cardbuf);
	}
	nact++;
	if(mailfile == 0)
		mailfile = p;
}

#endif


/* VARARGS */
card(c, s, a1, a2, a3, a4)
int c;
char	*s;
{
	putc( c, tff );
	fprintf(tff, s, a1, a2, a3, a4);
	c = putc( '\n', tff );

	if(c == EOF && ferror(tff))
		errout("Error writing control file.");
}


#include	<pwd.h>
struct passwd *getpwuid();

ident()
{
	int c, i, j, n, test, jsave;
	struct passwd *b1;

	if((b1 = getpwuid(getuid())) == NULL) {
		err("Invalid user id");
		return(1);
	}
	j = 0;
#if SPTYPE != LPR
	while(c = "$	ident	"[j])
		idcard[j++] = c;
	iacct = j;
#endif

	i = 0;
	if(identf) {
		while(c = identf[i++])
			idcard[j++] = c;
		idcard[j++] = ',';
	}
#if SPTYPE != LPR
	else{
		jsave = j;		/*use either usg or pwb-style passwd. MRW*/
		while((c = b1->pw_gecos[i++]) && c != ':')
			if(c == ')')
				j = jsave;
			else
				idcard[j++] = c;
		idcard[j++] = ',';
	}
#endif

	iname = j;
	i = 0;
	if(!pp)
		pp = &idcard[j];
	while(c = b1->pw_name[i++])
		idcard[j++] = c;
	idcard[j] = '\0';

#if SPTYPE != LPR
	i = 0;
	n = 2;
	while(n--) {
		test = 0;
		while((c=idcard[i++]) && c != ',') {
			if('0' <= c && c <= '9') test = 2;
			else test = 1;
		}
		if(c == 0 || test <= n){	/*acct must be number; box anything*/
			idcard[j] = '\0';
			err("Invalid IDENT information - %s", idcard);
			return (1);
		}
	}

	if(!uidf) {
		n = 0;
		while((c = idcard[i++]) && c != ',') {
			if(n >= 12) break;
			gcosid[n++] = c;
		}
		gcosid[n++] = '\0';
	}
#endif
#ifdef STOCKIII
	card('L', idcard);
#else
	if(wantbanner) card('L',idcard);
#endif

	if(wantmail){
		card('M',pp);
		if(identf)
			card('Q', idcard);	/*mail back $IDENT card.*/
	}
	return (0);
}

setup()
{
	register i;
	int out();

	NAME = argv[0];
	INCHAR = sizeof(SPDIR) + 2;
	if(NTEMP < 1 || sizeof(Xfname[0]) < INCHAR+7){
		err("Program bug; overflow in Xfname.");
		exit(1);
	}
	strcpy(Xfname[0], SPDIR);
	strcat(Xfname[0], "/dfAXXXXX");
	Xfname[0][INCHAR] = FIRSTCHAR;
	for(i=0; i<NTEMP; i++){
		if(i>0)
			strcpy(Xfname[i], Xfname[0]);
		Xfname[i][INCHAR-2] = Xchar[i];
		mktemp(Xfname[i]);
	}
	if(signal(SIGHUP, SIG_IGN) != SIG_IGN)
		signal(SIGHUP, out);
	if(signal(SIGINT, SIG_IGN) != SIG_IGN)
		signal(SIGINT, out);
	if(signal(SIGQUIT, SIG_IGN) != SIG_IGN)
		signal(SIGQUIT, out);
	if(signal(SIGTERM, SIG_IGN) != SIG_IGN)
		signal(SIGTERM, out);
}


/* VARARGS */
err(s, a1, a2, a3)
char *s;
{
	fprintf(stderr, "%s: ", NAME);
	fprintf(stderr, s, a1, a2, a3);
	putc('\n', stderr);
}


/* VARARGS */
errout(s, a1, a2, a3)
char *s;
{
	err(s, a1, a2, a3);
	out();
}


out()
{
	register i, j;

	signal(SIGHUP, SIG_IGN);
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
	i = INCHAR;
	for(j = 0; j < NTEMP; j++)
		for(; Xfname[j][i] != FIRSTCHAR; Xfname[j][i]--) 
			unlink(Xfname[j]);
	exit(1);
}
