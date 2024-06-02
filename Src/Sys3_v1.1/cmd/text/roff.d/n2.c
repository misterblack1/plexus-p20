#include "tdef.hd"
#ifdef unix
#include "sys/ioctl.h"
#include "termio.h"
#endif
#include "strs.hd"
#ifdef NROFF
#include "tw.hd"
extern struct ttable t;
#endif
#ifdef unix
#include <setjmp.h>
extern jmp_buf sjbuf;
#endif

/*
troff2.c

output, cleanup
*/

extern struct s *frame, *stk, *nxf;
extern struct tmpfaddr ip;
extern struct tmpfaddr offset;
#ifndef INCORE
extern struct envblock eblock;
#else
extern struct envblock eblock[NEV];
extern int ev;
#endif
extern struct datablock dblock;
extern struct d d[NDI], *dip;

extern char *enda;


extern int ch_CMASK;
extern char obuf[OBUFSZ];
extern char *obufp;
extern int dilev;
extern int eschar;
extern int tlss;
extern int tflg;
extern int ascii;
extern int print;
extern int waitf;
extern char ptname[];
extern filedes ptid;
extern int em;
extern int ds;
extern int mflg;
extern struct tmpfaddr woff;
extern int nflush;
extern int lgf;
extern int app;
extern int nfo;
extern int donef;
extern int nofeed;
extern int trap;
extern struct termio ttys;
extern int ttysave;
extern int quiet;
extern int ndone;
extern int lead;
extern int ralss;
extern int gflag;
extern char *unlkp;
extern char newf[];
extern int ejf;
extern int no_out;
extern int level;
extern int stop;
int toolate;
int error;
#ifndef NROFF
extern int paper;
extern int acctf;
#else
#ifndef SMALL
extern int pipeflg;
#endif
#endif
pchar(c)
int c;
{
	register j;

	if(c & MOT){pchar1(c); return;}
	switch(j = c & CMASK){
		case 0:
		case IMP:
		case RIGHT:
		case LEFT:
			return;
		case HX:
			j = (tlss>>9) | ((c&~0777)>>3);
			if(c & 040000){
				j &= ~(040000>>3);
				if(j > dip->blss)dip->blss = j;
			}else{
				if(j > dip->alss)dip->alss = j;
				ralss = dip->alss;
			}
			tlss = 0;
			return;
		case LX:
			tlss = c;
			return;
		case PRESC:
			if (dilev == 0) j = eschar;
		default:
			c = (trtab[j] & BMASK) | (c & ~CMASK);
	}
	pchar1(c);
}
pchar1(c)
int c;
{
	register j, *k;
	extern int chtab[];

	j = c & CMASK;
	if (dilev > 0) {
		wbf(c);
		cptmpfaddr(dip->op,offset);
		return;
	}
	if(!tflg && !print){
		if(j == '\n')dip->alss = dip->blss = 0;
		return;
	}
	if(no_out || (j == FILLER))return;
#ifndef NROFF
	if(ascii){
		if(c & MOT){
			oput(' ');
			return;
		}
		if(j < 0177){
			oput(c);
			return;
		}
		switch(j){
			case 0200:
			case 0210:
				oput('-');
				break;
			case 0211:
				oputs("\146\151"/*fi*/);
				break;
			case 0212:
				oputs("\146\154"/*fl*/);
				break;
			case 0213:
				oputs("\146\146"/*ff*/);
				break;
			case 0214:
				oputs("\146\146\151"/*ffi*/);
				break;
			case 0215:
				oputs("\146\146\154"/*ffl*/);
				break;
			default:
				for(k=chtab; *++k != j; k++)
					if(*k == 0)return;
				oput('\\');
				oput('(');
				oput(*--k & BMASK);
				oput(*k >> BYTE);
		}
	}else
#endif
	ptout(c);
}
oput(i)
char i;
{
	*obufp++ = i;
	if(obufp == (obuf + OBUFSZ + ascii - 1))flusho();
}
oputs(i)
char *i;
{
	while(*i != 0)oput(*i++);
}
flusho(){
	if(!ascii)*obufp++ = 0;
#ifdef unix
	if(!ptid){
		while((ptid=open(ptname,1)) < 0){
			if(++waitf <=2)prstr("Waiting for Typesetter.\n");
			sleep(15);
		}
	}
#endif
	if(no_out == 0){
		if (!toolate) {
			toolate++;
#ifdef NROFF
#ifdef unix
			if(t.bset || t.breset){
				if(ttysave == -1) {
					ioctl(1, TCGETA, &ttys);
					ttysave = ttys.c_oflag;
				}
				ttys.c_oflag &= ~t.breset;
				ttys.c_oflag |= t.bset;
				ioctl(1, TCSETAW, &ttys);
			}
#endif
			{
			char *p = t.twinit;
			while (*p++)
				;
#ifdef unix
			write(ptid, t.twinit, p-t.twinit-1);
#endif
#ifdef tso
			fwrite(t.twinit,p-t.twinit-1,1,ptid);
#endif
			}
#endif
		}
#ifdef unix
		toolate += write(ptid, obuf, obufp-obuf);
#endif
#ifdef tso
		toolate += fwrite(obuf,obufp-obuf,1,ptid);
#endif
	}
	obufp = obuf;
}
done(x) int x;{
	register i;

	error |= x;
	level = 0;
	app = ds = lgf = 0;
	if(i=em){
		donef = -1;
		em = 0;
#ifdef unix
		if(control(i,0))longjmp(sjbuf,1);
#endif
#ifdef tso
		if (control(i,0)) reset(0);
#endif
	}
	if(!nfo)done3(0);
	mflg = 0;
	dip = &d[dilev = 0];
	if (woff.b) wbt(0);
	if(pendw)getword(1);
	pendnf = 0;
	if(donef == 1)done1(0);
	donef = 1;
	ip.b = 0;
	frame = stk;
#ifndef INCORE
	nxf = frame + 1;
#else
	nxf = (struct s *)malloc(sizeof(struct s));
#endif
	if(!ejf)tbreak();
	nflush++;
	eject((struct s *)0);
#ifdef unix
	longjmp(sjbuf,1);
#endif
#ifdef tso
	reset(0);
#endif
}
done1(x) int x; {
	error |= x;
	if(v_nl){
		trap = 0;
		eject((struct s *)0);
#ifdef unix
		longjmp(sjbuf,1);
#endif
#ifdef tso
		reset(0);
#endif
	}
	if(nofeed){
		ptlead();
		flusho();
		done3(0);
	}else{
		if(!gflag)lead += TRAILER;
		done2(0);
	}
}
done2(x) int x; {
	register i;

	ptlead();
#ifndef NROFF
	if(!ascii){
		oput(T_INIT);
		oput(T_STOP);
		if(!gflag)for(i=8; i>0; i--)oput(T_PAD);
#ifndef tso
		if(stop && (ptid != 1)) prstr("Troff finished.\n");
#endif
	}
#endif
	flusho();
	done3(x);
}
done3(x) int x;{
	error |= x;
#ifdef unix
	signal(SIGINT, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
#ifndef INCORE
	unlink(unlkp);
#endif
#endif
#ifdef NROFF
	twdone();
#endif
#ifdef unix
	if(quiet){
		ttys.c_lflag |= ECHO;
		ioctl(0, TCSETAW, &ttys);
	}
#endif
	if(ascii)mesg(1);
#ifndef NROFF
#ifndef SMALL
	report();
#endif
#endif
	exit(error);
}
edone(x) int x;{
	frame = stk;
#ifndef INCORE
	nxf = frame + 1;
#else
	nxf = (struct s *)malloc(sizeof(struct s));
#endif
	ip.b = 0;
	done(x);
}

#ifndef NROFF
#ifndef SMALL
report(){
	struct {int use; int uid;} a;

	if((ptid != 1) && paper ){
		lseek(acctf,0L,2);
		a.use = paper;
		a.uid = getuid();
		write(acctf,(char *)&a,sizeof(a));
	}
}
#endif
#endif
#ifdef NROFF
casepi(){
#ifndef SMALL
	register i;
	int id[2];

	if(toolate || skip() || !getname() || (pipe(id) == -1) ||
	   ((i=fork()) == -1)){
		prstr("Pipe not created.\n");
		return;
	}
	ptid = id[1];
	if(i>0){
		close(id[0]);
		toolate++;
		pipeflg++;
		return;
	}
	close(0);
	dup(id[0]);
	close(id[1]);
	execl(newf,newf,0);
	prstr("Cannot exec: ");
	prstr(newf);
	prstr("\n");
	exit(-4);
#endif
}
#endif
