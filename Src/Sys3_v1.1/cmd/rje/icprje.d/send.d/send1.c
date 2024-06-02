#
/* send1.c */


/* This information is proprietary and is the property of Bell
   Telephone Laboratories, Incorporated.  Its reproduction or
   disclosure to others, either orally or in writing, is pro-
   hibited without written permission of Bell Laboratories. */


#define IDMOD send1

#define IDSTR "~|^`send1.c 1.20 4/22/76

#include	"send.h"
#include	"sys/param.h"
#include	"sys/stat.h"

abt()
{
	if (!abort) {
		abort=1; 
		dotrp();
	};
};

sig1()
{
	signal(1,1); 
	prf("Hang-up.\n"); 
	abt();
};

sig2()
{
	signal(2,1); 
	prf("Interrupt.\n"); 
	abt();
};

sig3()
{
	signal(3,1); 
	prf("Quit signal.\n"); 
	abt();
};

sigx(x)
{
	signal(x,1); 
	prf("Signal #%d.\n",x); 
	abt();
};

struct context icx,zcx;

struct format ifm;

struct trap itr;

main(ac,av,ev)
char *av[];
char *ev[];
{
	extern int errno;
	int idv,ino;
	int gth,eof,*aeof;
	struct	stat	st;
	register int t;
	register char *p,*p0;
	char	minp[12], crdcnt[12];
	extern	long	min;
	extern	char		*home;
	char	*argv[6];
	char	env[LNX];
	aeof=(&eof);
	tty=(-1);
	inibf(&obf,1);
	inibf(&dbf,2);
	st.st_dev = 0;
	st.st_ino = 0;
	st.st_mode = 0;
	if (fstat(0,&st)<0) open("/dev/null",0);
	if (isatty(0)) {
		tin=1; 
		tty=0;
	};
	if (fstat(1,&st)<0) dup(0);
	idv=st.st_dev; 
	ino=st.st_ino;
	if (isatty(1)) {
		obf.bb=1; 
		tty=1;
	};
	if (obf.bb==0 && lseek(1,0L,1)<0 && errno==ESPIPE) obf.bb=1;
	if (fstat(2,&st)<0) dup(1);
	if (st.st_dev==idv && st.st_ino==ino) oed=1;
	if (isatty(2)) {
		dbf.bb=1; 
		tty=2;
	};
	code=BCD;
	maxcol=80;
	iniky();
	stfm(&ifm);
	icx.dsp=(&ifm);
	icx.ocx=(&zcx);
	zcx.kw0=icx.kw0=ikw;
	zcx.kw1=icx.kw1=ikw->nx;
	icx.nam=(ac>0? av[0]:"send");
	icx.lvl=(-1);
	p0=p=icx.nam;
	while (t=(*p++))
	{
		if (t=='/') p0=p;
	};
	gth=mtch(4,"gath",p0);
	gth |= mtch(4,"gath",p0+1);
	usage((gth? "gath":"send"));
	if (gth)
		icx.flg[F'g']=icx.flg[F'l']=icx.flg[F'q']=1;
	else
		umask(0);	/* Reset umask for send fix */
	eof=0;
	ac--; 
	av++;
	itr.str = tchain;
	tchain = &itr;
	setjmp(itr.save);
	if (!signal(1,1)) signal(1,sig1);
	if (!signal(2,1)) signal(2,sig2);
	if (!signal(3,1)) signal(3,sig3);
	for(t=4;t<NSIG;t++) {
		if(t == SIGCLD)
			continue;
		signal(t,sigx);
	}
	while (!eof && !abort)
	{
		if (*ev!=0) {
			p = (char *)prf("%q\\$%s",env,*ev++);
			icx.flg[F'd']=1;
			doarg(&icx,p-env,env,aeof);
			icx.flg[F'd']=0;
			continue;
		}
		if (--ac<0)
		{
			if (!didf) doarg(&icx,1,"-",aeof);
			break;
		};
		p0=(*av++);
		for (p=p0;*p;p++);
		doarg(&icx,p-p0,p0,aeof);
	};
	if (icx.sfd) doarg(&icx,0,0);
	flush(&obf);
	if (tmpf) {
		flush(&qbf); 
		close(qbf.fd);
	};
	if (abort) {
		prf("Aborted.\n"); 
		goto X;
	};
	if (!gth || qat) prf("%L cards.\n",cnt);
	if (!qat) {
		flush(&dbf); 
		exit((error? 1:0));
	};
	if (!cue) goto X;
	if (error)
	{
		prf("Errors detected. Send anyway?  ");
		ss[0]=0;
		if (read(2,ss,1)==1)
		{
			ss[1]=ss[0];
			while (ss[1]!='\n' && read(2,ss+1,1)==1);
		};
		if ((0137&ss[0])!='Y') goto X;
	};
	flush(&dbf);
	for (t=1;t<13;t++) signal(t,1);
	prf("%q%L",minp,min);
	/* Fix up temp file name */

	strcpy(tmpf,strrchr(tmpf,'/')+1);
	ltoa(cnt,crdcnt);
	argv[0] = "qer";
	argv[1] = home;
	argv[2] = tmpf;
	argv[3] = crdcnt;
	argv[4] = msglvl;
	argv[5] = 0;
	execv(qer,argv);
	prf("Cannot execute queuer, %s.\n",qer);
X: 
	flush(&dbf);
	if (tmpf) unlink(tmpf);
	exit(1);
};

ltoa(n,buf)
long n;
char *buf;
{
	char tbuf[12];
	register char *ptr;

	ptr = tbuf;
	while(n > 0) {
		*ptr++ = (n % 10) + '0';
		n = n / 10;
	}
	while(--ptr >= tbuf)
		*buf++ = *ptr;
	*buf = '\0';
}

usage(s)
char *s;
{
	int u,*au;
	char *px;
	register int t,d;
	register char *p;
	au=(&u);
	p="/usr/pw/usg";
	t=getuid();
	t += 2; 
	t <<= 1;
	if (t>=512) return;
	prf("%q%s/%s",ss,p,s);
	if ((d=open(ss,2))>=0)
	{
		if (lseek(d,(long)t,0)<0 || read(d,au,2)!=2) u=0;
		u++;
		if (lseek(d,(long)t,0)>=0) write(d,au,2);
		close(d); 
		return;
	};
	if ((d=creat(ss,0666))<0) return;
	px=(p=tbf.bf)+512;
	time(p); 
	p += 4;
	while (p<px) *p++=0;
	tbf.bf[t]=1;
	write(d,tbf.bf,512);
	close(d);
};

int set14;

sig14()
{
	signal(14,sig14);
	if (set14) alarm(1);
};

getty()
{
	static int x;
	register int t;
	if (x==0)
	{
		x++;
		sig14();
		alarm(set14=10);
		t=open("/dev/tty",2);
		alarm(set14=0);
		signal(14,sigx);
		if (t>=0) tty=t;
	};
	return (tty);
};

/*end*/
