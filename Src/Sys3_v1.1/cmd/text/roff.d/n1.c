/*
 *	roff.src  -  v 1.115 of 5/8/80
 *
 *	This is the first file of the nroff/troff program (n1.c).
 *
 */




#ifdef NROFF
char ntversion[] = "@(#)nroff:	1.115";
#else
char ntversion[] = "@(#)troff:	1.115";
#endif

#ifndef unix
char ntNunix[] = "@(#)	non-unix";
#define INCORE
#define SMALL
#define tso
#endif

#ifdef SMALL
char ntSMALL[] = "@(#)	SMALL";
#define NDIAGS
#define NOCOMPACT
#endif

#ifdef INCORE
char ntINC[] = "@(#)	incore";
#define NOCOMPACT
#endif

#ifdef NDIAGS
char ntDIAGS[] = "@(#)	NDIAGS";
#endif

#ifdef NOCOMPACT
char ntNOCOMPACT[] = "@(#)	NOCOMPACT";
#endif



int version = 1115;		/* nroff/troff version tag */

#ifdef unix
#include <sys/types.h>
#include <sys/stat.h>
#endif
#include "tdef.hd"
#include "strs.hd"
#ifndef INCORE
#include "uns.hd"
#endif
#ifdef NROFF
#include "tw.hd"
extern struct ttable t;
#endif
#ifdef unix
#include <setjmp.h>
jmp_buf sjbuf;
#include "sys/ioctl.h"
#include "termio.h"
#endif
/*
troff1.c

consume options, initialization, main loop,
input routines, escape function calling
*/

extern struct s *frame, *stk, *nxf;
extern struct s *ejl;
extern struct tmpfaddr ip;
#ifndef INCORE
extern struct envblock eblock;		/* environment block */
#else
extern struct envblock eblock[NEV];	/* incore environments */
extern char *malloc();
extern int *argsp;
extern int maclev;
#endif
extern struct d d[NDI], *dip;
extern struct datablock dblock;		/* compactable data area */


#ifndef SMALL
extern int fork(), pipe(), fcntl();
extern int **argpp;		/* pointers to request arguments */
#endif

#ifdef ebcdic
extern char atoe[], etoa[];	/* ascii to ebcdic and vice versa */
extern char *fname();
#endif
#ifdef tso
char hibuf[NSO][IBUFSZ];	/* input buffers during .so */
int heibuf[NSO];		/* end pointers for hibuf */
int hibufp[NSO];		/* current pos pointers for hibuf */
#endif
extern int ev;
extern int bdtab[];
extern getfont();
extern char *mktemp();
extern char *ttyname();
#ifndef INCORE
extern char *setbrk();
#endif
extern char *ttyname();
extern catch(), fpecatch(), kcatch();
extern int cd;
extern int vflag;
extern int dfact;
extern int tch[];
extern int *cstk[], cstkl;
extern int ch_CMASK;
extern long atoi0();
extern int ndone;
extern int stdi;
extern int waitf;
extern int nofeed;
extern int quiet;
extern filedes ptid;
extern int ascii;
extern int npn;
extern int xflg;
extern int stop;
extern char ibuf[IBUFSZ];
extern char xbuf[IBUFSZ];
extern char *ibufp;
extern char *xbufp;
extern char *eibuf;
extern char *xeibuf;
extern int cbuf[NC];
extern int nx;
extern int mflg;
extern int ch;
extern int pto;
extern int pfrom;
extern int cps;
extern int suffid;
extern char suftab[];
extern int ibf;
extern filedes ttyod;
#ifdef unix
extern struct termio ttys;
#endif
extern int iflg;
extern int init;
extern int rargc;
extern char **argp;
extern int lgf;
extern int copyf;
extern int eschar;
extern int cwidth;
extern int nlflg;
extern int donef;
extern int nflush;
extern int nfo;
extern filedes ifile;
extern int fc;
extern int padc;
extern int raw;
extern struct	{
	char buf[NS];
		}  nextf[NSN];
extern char newf[];
extern int nfi;
#ifdef NROFF
extern char termtab[];
extern int tti;
#endif
extern filedes ifl[NSO];
extern int ifi;
extern int flss;
extern char ptname[];
extern int print;
extern int nonumb;
extern int pnlist[];
extern int *pnp;
extern int trap;
extern int tflg;
extern int ejf;
extern int gflag;
extern int oline[];
extern int *olinep;
extern int dpn;
extern int noscale;
extern char *unlkp;
extern int level;
extern int ttysave;
extern int dotT;
extern int tabch, ldrch;
extern no_out;
#ifndef NROFF
extern char codetab[];
extern int lg;
extern char fontfile[];
extern int ffi;		/* index into fontfile string (see t6.c) */
#else
extern int eqflg;
extern int hflg;
#endif
int nnextf = 0;		/* index into nextf */
struct tmpfaddr ipl[NSO];
long offl[NSO];
long ioff;
char *ttyp;
int ms[] = {31,28,31,30,31,30,31,31,30,31,30,31};

#ifndef SMALL
int unixp;		/* pointer into unix call buffer */
int unixpt=0;		/* top of unix buffer */
int unixch=0;		/* channel for unix reads */
#endif

#ifndef NROFF
int acctf;
#endif
int did_mesg = 0;

/*	definitions for compacted macros	*/

#ifndef NOCOMPACT

extern char cmpctf[], cmpctuf[];

int cmpcti;		/* pointer into cmpctt string */
int compact;		/* compact flag */
char cname[10] = "x.";	/* string name for compacted output */
int cnamei = 2;		/* pointer to end of cname string */

#endif
main(argc, argv, envp)
int argc;
char **argv, **envp;
{
	register char *p, *q;
	register i;
#ifndef NOCOMPACT
	int sargc;		/* hold argc, argv for -c */
	char **sargv;
	int tversion;

	sargc = argc;
	sargv = argv;
#endif

#ifdef ebcdic
	cargs(argc, argv);	/* convert args to ascii */
#endif
	setsignals();
	init1(argv[0][0]);

#ifdef NROFF
	for (tti = -1; termtab[++tti]; ) ;	/* find end of string */
	termtab[tti] = '3';
	termtab[tti+1] = '7';		/* tab37 is default terminal table */
#ifdef tso
	termtab[tti+2] = '.';	/* fudge name (to tabnn.t) */
	termtab[tti+3] = 't';
#endif
#else
	for (ffi = -1; fontfile[++ffi]; ) ;	/* find end of string */
#endif

#ifndef NOCOMPACT
	for (cmpcti = -1; cmpctf[++cmpcti]; ) ;	/* end of comp. macr. names */
#endif

	for (nfi = -1; nextf[0].buf[++nfi]; ) ;	/* find end of string */

	while(--argc > 0 && (++argv)[0][0]=='-')
		switch(argv[0][1]){

		case 0:
			goto start;
		case 'i':
			stdi++;
			continue;
		case 'q':
			quiet++;
#ifdef unix
			if(ioctl(0, TCGETA, &ttys) >= 0)
				ttysave = ttys.c_oflag;
#endif
			continue;
		case 'n':
			npn = cnum(&argv[0][2]);
			continue;
		case 'p':
			xflg = 0;
			cps = cnum(&argv[0][2]);
			continue;
		case 's':
			if(!(stop = cnum(&argv[0][2])))stop++;
			continue;
		case 'r':
			vlist[findr(argv[0][2])] = cnum(&argv[0][3]);
			continue;
		case 'c':	/* read compacted macros */
#ifndef NOCOMPACT
			if (mflg) goto regmac;	/* use -m if not first package */
				else mflg++;
			p = &cmpctf[cmpcti];
			q = &argv[0][2];
			while (*p++ = *q++) ;	/* make compacted file name */

			if ((i = open(cmpctf,0)) < 0)
				goto regless;	/* data area */
			if ((read(i,&tversion,sizeof(version)) != sizeof(version)) ||
			    (tversion != version ))
				goto regless;	/* wrong version of macros */
			if ((read(i,&dblock,sizeof(struct datablock))) <
				sizeof(struct datablock))	{
				prstr("error reading data area\n");
				exit(1);	}

			cmpctf[cmpcti-2] = 't';		/* now tmp file */
			if ((i = open(cmpctf,0)) < 0)	{
				prstr("can't find compacted tmp file\n");
				exit(1);	}
			Mcp(i, ibf);		/* copy tmp file */
			close(i);

			p = nextf[nnextf++].buf;	/* save name of uncomp. area */
			q = cmpctuf;
			while (*p++ = *q++) ;
			p--;			/* point to uncompacted segment */
			q = &argv[0][2];	/* package name */
			while (*p++ = *q++) ;	/* stow it */

			for (sargc-=argc; ((--sargc>0)&&((++sargv)[0][0])); )
			    if (sargv[0][1] == 'r')	/* re-eval nr settings */
				vlist[findr(sargv[0][2])] = cnum(&sargv[0][3]);

			continue;

		case 'k':
			p = &cname[cnamei];	/* save name to compact into */
			q = &argv[0][2];
			while (*p++ = *q++) ;
			compact++;
			continue;

		regless:	mflg--;	/* fall into -m */
#endif
		case 'm':
		regmac:
			if (mflg++ >= NSN) ertoomp();
			p = &nextf[nnextf++].buf[nfi];
			q = &argv[0][2];
			while((*p++ = *q++) != 0);
			continue;
		case 'o':
			getpn(&argv[0][2]);
			continue;
#ifdef NROFF
		case 'h':
			hflg++;
			continue;
		case 'z':
			no_out++;
			continue;
		case 'e':
			eqflg++;
			continue;
		case 'T':
			p = &termtab[tti];
			q = &argv[0][2];
			if(!((*q) & 0177))continue;
			while((*p++ = *q++) != 0);
#ifdef tso
			*p++ = '.';	/* fudge name on tso */
			*p++ = 't';
#endif
			dotT++;
			continue;
		case 'u':
			bdtab[2] = cnum(&argv[0][2]);	/* set emboldening */
			if ((bdtab[2]<0) || (bdtab[2]>50)) bdtab[2]=0;
			continue;
#endif
#ifndef NROFF
		case 'z':
			no_out++;
		case 'a':
			ascii = 1;
			nofeed++;
		case 't':
#ifndef tso
			ptid = 1;
#endif
			continue;
		case 'w':
			waitf = 1;
			continue;
		case 'f':
			nofeed++;
			continue;
		case 'x':
			xflg = 0;
			continue;
		case 'b':
#ifdef unix
			if(open(ptname,1) < 0)prstr("Busy.\n");
			    else
#endif
				prstr("Available.\n");
			done3(0);
		case 'g':
			stop = gflag = 1;
#ifdef unix
			ptid = 1;
#endif
			dpn = 0;
			continue;
		case 'T':
			ffi -= 2;		/* overwrite 'ft' */
			for (p = &argv[0][2]; (fontfile[ffi] = *p++); ffi++);
			fontfile[ffi++] = '/';	/* build new path */
			fontfile[ffi++] = 'f';
			fontfile[ffi++] = 't';
#ifdef tso
			fontfile[ffi+1] = '.';	/* fudge name on tso */
			fontfile[ffi+2] = 'f';
#endif
			fontfile[ffi] = 'R';  getfont(0,1);	/* get default fonts */
			fontfile[ffi] = 'I';  getfont(1,1);
			fontfile[ffi] = 'B';  getfont(2,1);
			fontfile[ffi] = 'S';  getfont(3,1);
			continue;
#endif
		default:
			prstr("Unknown option: ");
			aprstr(argv[0]);
			prstr("\n");
			ferrex();
	}
start:
	argp = argv;
	rargc = argc;
	nnextf = 0;
	init2();
#ifdef unix
	setjmp(sjbuf);
#endif
#ifdef tso
	setexit();
#endif
loop:
	copyf = lgf = nb = nflush = nlflg = 0;
	if(ip.b && (rbf0(&ip)==0) && ejf
#ifndef INCORE
		&& (frame->pframe <= ejl)
#else
		&& ((maclev-1) <= (int)ejl)
#endif
				)	{
		nflush++;
		trap = 0;
		eject((struct s *)0);
		goto loop;
	}
	i = getch();
	if(pendt)goto lablt;
	if(ch_CMASK == XPAR){
		copyf++;
		tflg++;
		for(;ch_CMASK != '\n';)pchar(getch());
		tflg = 0;
		copyf--;
		goto loop;
	}
	if((ch_CMASK == cc) || (ch_CMASK == c2)){
		if(ch_CMASK == c2)nb++;
		copyf++;
		do i = getch();
		    while ((ch_CMASK == ' ') || (ch_CMASK == '\t'));
		ch = i;
		copyf--;
		control(getrq(),1);
		flushi();
		goto loop;
	}
lablt:
	ch = i;
	text();
	goto loop;
}
setsignals()
{
#ifdef unix
	signal(SIGHUP,catch);
	if (signal(SIGINT,SIG_IGN) == SIG_IGN){
		signal(SIGHUP,SIG_IGN);
		signal(SIGINT,SIG_IGN);
		signal(SIGQUIT,SIG_IGN);	}
	    else signal(SIGINT,catch);
	signal(SIGFPE,fpecatch);
	signal(SIGPIPE,catch);
	signal(SIGTERM,kcatch);
#endif
}
catch(){
/*
	prstr("Interrupt\n");
*/
	done3(01);
}
fpecatch(){
	prstrfl("Floating Exception.\n");
#ifdef unix
	signal(SIGFPE,fpecatch);
#endif
}
kcatch(){
#ifdef unix
	signal(SIGTERM,SIG_IGN);
#endif
	done3(01);
}
#ifndef NROFF
#ifndef SMALL
acctg() {
	static char *acct_file = "/usr/adm/tracct";
	acctf = open(acct_file,1);
	setuid(getuid());
}
#endif
#endif
init1(a)
char a;
{
	register char *p;
	register i;

#ifndef NROFF
#ifndef SMALL
	acctg();/*open troff actg file while mode 4755*/
#endif
#endif
#ifndef INCORE
	if((suffid=open(suftab,0)) < 0) errcos();
	read(suffid, sufind.chr, sizeof(sufind));

	p = mktemp("/tmp/taXXXXX");
	if(a == 'a')p = &p[5];
	if((close(creat(p, 0600))) < 0){
		prstr("Cannot create temp file.\n");
		exit(-1);
	}
	ibf = open(p, 2);
#endif
	for(i=256; --i;)trtab[i]=i;
	trtab[UNPAD] = ' ';
	mchbits();
#ifndef INCORE
	if(a != 'a')unlkp = p;
#endif
}
init2()
{
	register i,j;

#ifdef unix
	ttyod = 2;
	if(((ttyp=ttyname(j=0)) != (char *)0) ||
	   ((ttyp=ttyname(j=1)) != (char *)0) ||
	   ((ttyp=ttyname(j=2)) != (char *)0)
	  );else
#endif
		ttyp = "notty";
#ifdef tso
	ttyod = stdout;
#endif
	iflg = j;
	if(ascii)mesg(0);

#ifdef unix
	if (!ptid && !waitf
#ifndef NOCOMPACT
		&& !compact
#endif
				)	{
		if((ptid = open(ptname,1)) < 0){
			prstr("Typesetter busy.\n");
			done3(-2);
		}
	}
#endif
#ifdef tso
	if ((ptid = fopen("OUTPUT", "w,BINARY")) == (FILE *)-1) {
		prstr("can't create OUTPUT");
		exit(1);	}
#endif
	ptinit();
	for(i=NEV; i--;)
#ifndef INCORE
		write(ibf, (char *)&eblock, sizeof(struct envblock));
#else
	    if (i)
		{ char *p, *q;
		    for (p=(char *)&eblock[i],q=(char *)&eblock[0],j=0; (j<sizeof(struct envblock)); j++)
			*p++ = *q++;	}
#endif
	olinep = oline;
	ibufp = eibuf = ibuf;
	v_hp = init = 0;
	ioff = 0;
	v_nl = -1;
	cvtime();
#ifndef INCORE
	frame = stk = (struct s *)setbrk(DELTA);
#else
	frame = stk = (struct s *)malloc(sizeof(struct s));
					/* incore version */
#endif
	dip = &d[0];
#ifndef INCORE
	nxf = frame + 1;
#else
	nxf = (struct s *)malloc(sizeof(struct s));
#endif
	nx = mflg;
}
cvtime(){

	long tt;
	register i;

	time(&tt);
	tt -= 3600*ZONE;	/*5hrs for EST*/
	v_dy = (tt/86400L) + 1;
	v_dw = (v_dy + 3)%7 + 1;
	for(v_yr=70;; v_yr++){
		if((v_yr)%4)ms[1]=28;else ms[1]=29;
		for(i=0;i<12;){
			if(v_dy<=ms[i]){
				v_mo = i+1;
				return;
			}
			v_dy -= ms[i++];
		}
	}
}
cnum(a)
char *a;
{
	register i;

	ibufp = a;
	for (eibuf = a; *eibuf++; ) ;
	i = atoi();
	ch = 0;
	return(i);
}
mesg(f)
int f;
{
#ifdef unix
	static int mode;
	struct stat statb;

	if(!f){
		stat(ttyp,&statb);
		mode = statb.st_mode;
		chmod(ttyp,mode & ~022);
		did_mesg = 1;
	}else{
		if (did_mesg) chmod(ttyp,mode);
	}
#endif
}
prstrfl(s)
char *s;
{
	flusho();
	prstr(s);
}
prstr(s)
char *s;
{
	register i;
	register char *j;

#ifdef unix
	j = s;
	for(i=0;*s;i++)s++;
	write(ttyod,j,i);
#endif
#ifdef ebcdic
	while (i = *s++)	{
		if (putc(i, ttyod) ==  EOF)
			exit(1);
		if (etoa[i] == '\n')
			fflush(ttyod);	}
#endif
}
#ifdef ebcdic
aprstrfl(s)
char *s;
{
	flusho();
	aprstr(s);
}
aprstr(s)
char *s;
{	register i;
	register char *j;

	while (i = *s++)	{
		if (putc(atoe[i], ttyod) == EOF)
			exit(1);
		if (i == '\n')
			fflush(ttyod);	}
}
#endif
control(a,b)
int a,b;
{
	register i,j;

	i = a;
	if((i == 0) || ((j = frmname(i)) == -1))return(0);
	if (nametab[j].ename & MMASK)	{
		nxf->nargs = 0;
		if(b)collect();
		flushi();
		return(pushi((filep)nametab[j].vv.val));	}
	else	{
		if(!b)return(0);
		return ((*nametab[j].vv.f)(0));	}
}

getrq(){
	register i,j;

	if(((i=getach()) == 0) ||
	   ((j=getach()) == 0))goto rtn;
	i = PAIR(i,j);
rtn:
	return(i);
}
getch(){
	register int i, j, k;

	level++;
g0:
	if(ch){
		if (((ch_CMASK = (i = ch) & CMASK)) == '\n')nlflg++;
		ch = 0;
		level--;
		return(i);
	}

	if(nlflg){
		level--;
		return(ch_CMASK = '\n');
	}

	if((k = (i = getch0()) & CMASK) != ESC){
		if(i & MOT)goto g2;
		if(k == FLSS){
			copyf++; raw++;
			i = getch0();
				/* sign extend */
			if (i & 0100000) i |= (int)~0177777;
			if(!fi)flss = i;
			copyf--; raw--;
			goto g0;
		}
		if(!copyf){
#ifndef NROFF
			if((k == 'f') && lg && !lgf){
				i = getlg(i);
				goto g2;
			}
#endif
			if((k == fc) || (k == tabch) || (k == ldrch)){
				if((i=setfield(k)) == 0)goto g0; else goto g2;
			}
			if(k == 010){
				i = makem(-width(' ' | chbits));
				goto g2;
			}
		}
		goto g2;
	}
	k = (j = getch0()) & CMASK;
	if(j & MOT){
		i = j;
		goto g2;
	}
	switch(k){

		case '\n':	/*concealed newline*/
			goto g0;
		case 'n':	/*number register*/
			setn();
			goto g0;
		case '*':	/*string indicator*/
			setstr();
			goto g0;
		case '$':	/*argument indicator*/
			getch();
			if (((i = ch_CMASK - '0') > 0) && (i <= 9) && (i <= frame->nargs))
#ifndef INCORE
				setap(*((int **)frame + i-1 + (sizeof(struct s)/sizeof(int **))));
#else
				setap((int *)*(argsp + i - 1));
#endif
			goto g0;
		case '{':	/*LEFT*/
			i = LEFT;
			goto gx;
		case '}':	/*RIGHT*/
			i = RIGHT;
			goto gx;
		case '"':	/*comment*/
			while(((i=getch0()) & CMASK ) != '\n');
			goto g2;
		case ESC:	/*double backslash*/
			i = eschar;
			goto gx;
		case 'e':	/*printable version of current eschar*/
			i = PRESC;
			goto gx;
		case ' ':	/*unpaddable space*/
			i = UNPAD;
			goto gx;
		case '|':	/*narrow space*/
			i = NARSP;
			goto gx;
		case '^':	/*half of narrow space*/
			i = HNSP;
			goto gx;
		case '\'':	/*\(aa*/
			i = 0222;
			goto gx;
		case '`':	/*\(ga*/
			i = 0223;
			goto gx;
		case '_':	/*\(ul*/
			i = 0224;
			goto gx;
		case '-':	/*current font minus*/
			i = 0210;
			goto gx;
		case '&':	/*filler*/
			i = FILLER;
			goto gx;
		case 'c':	/*to be continued*/
			i = CONT;
			goto gx;
		case ':':	/* lem's character */
			i = COLON;
			goto gx;
		case '!':	/*transparent indicator*/
			i = XPAR;
			goto gx;
		case 't':	/*tab*/
			i = '\t';
			goto g2;
		case 'a':	/*leader (SOH)*/
			i = LEADER;
			goto g2;
		case '%':	/*ohc*/
			i = OHC;
			goto g2;
		case 'g':	/* return format of a number reg */
			setaf();
			goto g0;
		case '.':	/*.*/
			i = '.';
		gx:
			i = (j & ~CMASK) | i;
			goto g2;
	}
	if(!copyf)
		switch(k){

			case 'p':	/*spread*/
				spread++;
				goto g0;
			case '(':	/*special char name*/
				if((i=setch()) == 0)goto g0;
				break;
			case 's':	/*size indicator*/
				setps();
				goto g0;
			case 'f':	/*font indicator*/
				setfont(0);
				goto g0;
			case 'w':	/*width function*/
				setwd();
				goto g0;
			case 'v':	/*vert mot*/
				dfact = lss;
				vflag++;
				if (i = mot()) break;
				goto g0;
			case 'h': 	/*horiz mot*/
#ifdef NROFF
				dfact = EM;
#endif
#ifndef NROFF
				dfact = 6 * (pts & 077);
#endif
				if (i = mot()) break;
				goto g0;
			case 'z':	/*zero with char*/
				if (!((i = getch()) & MOT)) i |= ZBIT;
				break;
			case 'l':	/*hor line*/
				setline();
				goto g0;
			case 'L':	/*vert line*/
				setvline();
				goto g0;
			case 'b':	/*bracket*/
				setbra();
				goto g0;
			case 'o':	/*overstrike*/
				setov();
				goto g0;
			case 'k':	/*mark hor place*/
				if((i=findr(getsn())) == -1)goto g0;
				vlist[i] = v_hp;
				goto g0;
			case 'j':	/*mark output hor place*/
				if(!(i=getach()))goto g0;
				i = (i<<BYTE) | JREG;
				break;
			case '0':	/*number space*/
				i = makem(width('0' | chbits));
				break;
			case 'x':	/*extra line space*/
				if(i = xlss())break;
				goto g0;
			case 'u':	/*half em up*/
			case 'r':	/*full em up*/
			case 'd':	/*half em down*/
				i = sethl(k);
				break;
			default:
				i = j;
		}
	else{
		setch0(j);
		i = eschar;
	}
g2:
	if((ch_CMASK = (i & CMASK)) == '\n'){
		nlflg++;
		v_hp = 0;
		if (!ip.b) cd++;
	}
	if(!--level){
		j = width(i);
		v_hp += j;
		cwidth = j;
	}
	return(i);
}
char ifilt[32] = {0,001,002,003,0,005,006,007,010,
		  011,012,0,0,0,0,0,0,
		  0,0,0,0,0,0,0,0,
		  0,0,033};
getch0(){
	register int i, j;

again:
	if (cstkl)	{	/* characters in stack? */
		while ((i = *cstk[cstkl]++) == 0)	{
		    cstk[cstkl--] = 0;	/* that string is depleted */
		    while (cstkl && !cstk[cstkl])
				cstkl--;	/* find next in stack */
		    if (!cstkl) break;	}	/* out if stack empty */
		if (cstkl >= RP)  return (i);
			else if (i) goto g5;	}
ipagain:
	if (ip.b)		/* input from tty or tmp file */
		i =
#ifndef SMALL
			(ip.b == (filep)-2) ? rdunix() :	/* read from unix */
#endif
			(ip.b == (filep)-1) ? rdtty() :		/* read from tty */
					      rbf();		/* read from tmp */
	    else	{
		if(donef || ndone)done(0);
		if(nx || (ibufp >= eibuf)){
			if(nfo)goto g1;
		g0:
			if (nextfile())	{
				if (ip.b) goto ipagain;
				if (ibufp < eibuf) goto g2;	}
		g1:
			nx = 0;
#ifdef unix
			if((j=read(ifile,ibuf,IBUFSZ)) <= 0)goto g0;
#endif
#ifdef tso
			if ((j=fread(ibuf,1,IBUFSZ,ifile))<=0) goto g0;
#endif
			ibufp = ibuf;
			eibuf = ibuf + j;	}
	g2:
#ifndef ebcdic
		i = *ibufp++ & 0177;
#else
		i = etoa[*ibufp++] & 0177;
#endif
		ioff++;
		if(i >= 040)goto g4; else i = ifilt[i];	}
g5:
	if(raw)return(i);
	if((i & CMASK) == IMP)goto again;
	if((i == 0) && !init)goto again;
g4:
	if((copyf == 0) && ((i & ~BMASK) == 0) && ((i & CMASK) < 0370))
#ifndef NROFF
		if(spbits && (i>31) && ((codetab[i-32] & 0200))) i |= spbits;
		else
#endif
		i |= chbits;
	if((i & CMASK) == eschar)i = (i & ~CMASK) | ESC;
	return(i);
}
nextfile(){
	register char *p;

n0:
#ifdef unix
	if(ifile)close(ifile);
#endif
#ifdef tso
	if (ifile) fclose(ifile);
#endif
	if(nnextf < mflg){
		p = nextf[nnextf++].buf;
		goto n1;	}
	    else
		if (mflg == nnextf)  nnextf++;
	if(ifi > 0){
		if(popf())goto n0; /*popf error*/
		return(1); /*popf ok*/
	}
	if(rargc-- <= 0) {
		if((nfo -= mflg) && !stdi)done(0);
		nfo++;
		cd = stdi = mflg = 0;
		ifile = (filedes)0;
		ioff = 0;
		return(0);	}
	p = (argp++)[0];
n1:
	if((p[0] == '-') && (p[1] == 0)){
		ifile = (filedes)0;
	}else
#ifdef unix
	      if((ifile=open(p,0)) < 0)
#endif
#ifdef tso
		if ((ifile=fopen(fname(p),"r")) == NULL)
#endif
					{
		if (nnextf <= mflg)	{
			prstr("Non-existent macro file (");
			aprstr(&nextf[nnextf-1].buf[nfi]);
			prstr(")");	}
		    else	{
			prstr("cannot open file ");
			aprstr(p);	}
		prstr("\n");
		nfo -= mflg;
		nx = nnextf;
		done(02);
	}
	nfo++;
	cd = 0;
	ioff = 0;
	return(0);
}
popf(){
	register i;
	register char *p, *q;

	ioff = offl[--ifi];
	cptmpfaddr(ip,ipl[ifi]);
	if((ifile = ifl[ifi]) == (filedes)0){
		p = xbuf;
		q = ibuf;
		ibufp = xbufp;
		eibuf = xeibuf;
		while(q < eibuf)*q++ = *p++;
		return(0);
	}
#ifdef unix
	if((lseek(ifile,(long)(ioff & ~(IBUFSZ-1)),0) < (long)0) ||
	   ((i = read(ifile,ibuf,IBUFSZ)) < 0))return(1);
	eibuf = ibuf + i;
	ibufp = ibuf;
	if(ttyname(ifile) == (char *)0)
		ibufp = ibuf + (int)(ioff & (IBUFSZ-1));
#endif
#ifdef tso
	eibuf = heibuf[ifi];		/* restore buffers and pointers */
	ibufp = hibufp[ifi];
	if (ibufp >= eibuf) return (1);
	for (p=ibuf,q=hibuf[ifi]; p<=eibuf; )
		*p++ = *q++;
#endif
	return(0);
}
flushi(){
	if(nflush)return;
	ch = 0;
	if (cstkl == CH0)	{
		if (tch[0] == '\n') nlflg++;
		do cstkl--;
		    while (cstkl && !cstk[cstkl]);	}
	copyf++;
	while(!nlflg){
		if(donef && (frame == stk))break;
		getch();
	}
	copyf--;
	v_hp = 0;
}
getach(){
	register i;

	lgf++;
	if(((i = getch()) & (MOT | 0200)) ||
	    (ch_CMASK == ' ') ||
	    (ch_CMASK == '\n'))	{
			ch = i;
			i = 0;
	}
	lgf--;
	return(i & 0177);
}
getname(){
	register int i, k;

	lgf++;
	for(k=0; k < (NS-1); k++){
		i = getch();
		if ((ch_CMASK <= ' ') || (ch_CMASK > 0176)) break;
		newf[k] = ch_CMASK;
	}
	newf[k] = 0;
	ch = i;
	lgf--;
	return(newf[0]);
}
casenx(){
	lgf++;
	skip();
	getname();
	nx++;
	nextfile();
	nlflg++;
	cstkl = pendt = ip.b = 0;
	cstk[CH0] = cstk[AP] = (int *)0;
	frame = stk;
#ifndef INCORE
	nxf = frame + 1;
#else
	nxf = (struct s *)malloc(sizeof(struct s));
#endif
}
caseso()
{
	register filedes i;
	register char *p, *q;

	lgf++;
	newf[0] = 0;
	if(skip() || !getname()
#ifdef unix
			|| ((i=open(newf,0)) <0)
#endif
#ifdef tso
			|| ((i=fopen(fname(newf),"r")) == NULL)
#endif
					|| (ifi >= NSO)) {
		prstr("can't open file ");
		aprstr(newf);
		prstr("\n");
		done(02);
	}
	flushi();
#ifdef tso
	for (p=ibuf,q=hibuf[ifi]; p<eibuf; )
		*q++ = *p++;
	heibuf[ifi] = eibuf;	/* save buffer and pointers */
	hibufp[ifi] = ibufp;
#endif
	ifl[ifi] = ifile;
	ifile = i;
	offl[ifi] = ioff;
	ioff = 0;
	cptmpfaddr(ipl[ifi],ip);
	ip.b = 0;
	nx++;
	nflush++;
	if(!ifl[ifi++]){
		p = ibuf;
		q = xbuf;
		xbufp = ibufp;
		xeibuf = eibuf;
		while(p < eibuf)*q++ = *p++;
	}
}
getpn(a)
char *a;
{
	register i, neg;
	long atoi1();

	if((*a & 0177) == 0)return;
	neg = 0;
	ibufp = a;
	for (eibuf = a; *eibuf++; ) ;
	noscale++;
	while((i = getch() & CMASK) != 0)switch(i){
		case '+':
		case ',':
			continue;
		case '-':
			neg = MOT;
			goto d2;
		default:
			ch = i;
		d2:
			i = atoi1();
			if(nonumb)goto fini;
			else{
				*pnp++ = i | neg;
				neg = 0;
				if(pnp >= &pnlist[NPN-2]){
					prstr("Too many page numbers\n");
					done3(-3);
				}
			}
		}
fini:
	if(neg)*pnp++ = -2;
	*pnp = -1;
	ch = noscale = print = 0;
	pnp = pnlist;
	if(*pnp != -1)chkpn();
}

/*	compacted macros support routines.	*/


#ifndef NOCOMPACT

Mcp(oldp, newp)		/* copy file on oldp to file on newp */
int oldp, newp;
{	int n;
	char BUF[BSIZE];		/* copy buffer */

	while ((n = read(oldp, BUF, BSIZE)) > 0)

	    if (write(newp, BUF, n) != n)	{

		prstr("tmp file write error\n");
		exit(1);	}
}

#endif

caseco()		/* perform .co request */
{
#ifndef NOCOMPACT
	int i;

	if (!compact)  return(0);
	cname[0] = 'd';		/* data file first */
	if ((i = creat(cname, 0666)) < 0)	{
		prstr("can't create data file\n");
		exit(1);	}

	write(i, &version, sizeof(version));	/* write current version tag */
	write(i, &dblock, sizeof(struct datablock));	/* write data area */
	close(i);		/* done with data area */

	cname[0] = 't';		/* now the tmp file */
	lseek(ibf, (long)(ev*sizeof(struct envblock)), 0);	/* write curr env */
	write(ibf, (char *)&eblock, sizeof(struct envblock));
	lseek(ibf, (long)0, 0);	/* rewind */
	if ((i = creat(cname, 0666)) < 0)	{
		prstr("can't create tmp file\n");
		exit(1);	}
	Mcp(ibf, i);		/* copy tmp file */
	unlink(unlkp);		/* remove old tmp file */

	prstr("Compaction completed\n");
	exit(1);
#endif
}

/* error message routines */

ertoomp() {prstr("Too many macro packages.\n"); exit(-1); }

#ifndef INCORE
errcos()  {prstr("Cannot open suftab.\n"); exit(-1); }
#endif

ferrex()	{
#ifdef unix
#ifndef INCORE
		  unlink(unlkp);
#endif
#endif
		  exit(1); }
caseunix()	/* read output of command sent to unix */
{
#ifndef SMALL
	int fildes[2];		/* file pointers for pipe */
	register int i, j;
	char argbuf[15*ARGLEN];	/* hold arguments sent to unix */
	char *argp[20];		/* pointers to arguments */
	register int *p;

	nxf->nargs = 0;
	collect();		/* get request arguments */
	flushi();		/* flush input */
	pipe(fildes);		/* open pipe */

	if (fork() == 0)	{	/* child only code */

		close(1);		/* close standard output */
		if (fcntl(fildes[1],0,1) != 1)	{
			prstr("can't setup command env\n");
			exit(1);	}
		close(fildes[1]);	/* close old pipe channel */

		j = 0;
		argpp -= nxf->nargs;	/* point to args */

		for (i=0; i < nxf->nargs; i++)	{
		    argp[i] = &argbuf[j];	/* point to next string */
		    for (p = *argpp++; argbuf[j++] = (char)*p++; ) ;	}

		argp[nxf->nargs] = 0;	/* null after last pointer */
		close(0);		/* close standard input */
		execvp(argp[0], argp);	/* call unix program */
		prstr("Can't execute ");
		prstr(argp[0]);
		prstr("\n");
		exit(1);	}

	    else	{	/* parent only code */

		close(fildes[1]);	/* close write side */
		unixch = fildes[0];	/* channel for unix reads */
		pushi((filep)-2); }	/* mark unix read */
}

rdunix()
{
	static char unixb[BSIZE];	/* unix read buffer */

	if (unixp >= unixpt)	{	/* read a buffer */

	    if ((unixpt = read(unixch,unixb,BSIZE)) <= 0)	{
		close(unixch);
		popi();		/* end of file - terminate unix read */
		unixch = 0;
		return getch0();	}

	      else unixp = 0;	}

	return unixb[unixp++];
#endif
}

#ifdef ebcdic

char *fname(s)
char *s;
{	static char fnamebuf[NS];
	register char *p;

	for (p = fnamebuf; *s; s++)
		if (*s == '/')
			p = fnamebuf;
		    else
			*p++ = atoe[*s];
	*p++ = 0;
	return (fnamebuf);
}

cargs(rgc, rgv)
int rgc;
char **rgv;
{	char *trgv;

	for ( ; (rgc-- > 0); rgv++)

		for (trgv = *rgv; (*trgv); trgv++)

			*trgv = etoa[*trgv];
}

#endif

#ifdef tso
abs(i)
int i;
{
	if (i >= 0) return i;
		else return -i;
}
#endif
