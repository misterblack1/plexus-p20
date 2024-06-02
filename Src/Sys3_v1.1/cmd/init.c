#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <utmp.h>
#include <sys/plexus.h>

#define TTYS	"/etc/inittab"
#define RC	"/etc/rc"
#define SH	"/bin/sh"
#define DIAG	"/dev/console"
#define ERRACT	"/bin/sh < /dev/console > /dev/console 2>&1"

#define INIT 1
#define NOPROC	0
#define NOCMD	-1
#define NOIND	-1
#define CHILD	0
#define FORKERR	-1

#define ASIZE	256
#define TABSIZ	100
#define RCLIM	300

#define STx	1
#define ST1	4
#define ST2	5
#define ST3	6
#define ST4	7
#define ST5	8
#define ST6	10
#define ST7	11
#define ST8	12
#define ST9	13

struct initbl {
	int in_proc;
	char in_id[2];
	int in_cmd;
};
struct initbl itab[TABSIZ];

char	lio[]   = "/dev/liomem";
char state = '1';
char a0[] = "INIT     ";
char lstate = '0';
int statecnt [10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int ST [10] = {0, ST1, ST2, ST3, ST4, ST5, ST6, ST7, ST8, ST9};
int chst(), rckill();
int sigs [4] = {SIGINT, SIGQUIT, SIGALRM, SIGTERM};
int rdstate = 1;
int xstate = 0;
int idlim = 0;
int rcid;
unsigned csize = ASIZE;
char *start, *malloc(), *realloc();
unsigned nextcmd = 0;	/* offset from start */

main(argc, argv)
int argc;
char **argv;
{
	int i;

	if(argc > 1) {
		if((*argv[1] > '9') || (*argv[1] < '1')) {
			fputs("init: invalid state\n", stderr);
			exit(2);
		}

		if(kill(INIT, ST[*argv[1]-'0'])) {
			fputs("init: permission denied\n", stderr);
			exit(2);
		}
		exit(0);
	}

	if(getpid() != INIT) {
		fputs("usage: init state\n", stderr);
		exit(2);
	}

	signal(STx, chst);
	signal(SIGPWR, chst);
	ignsigs();

	for(i=0;i<=10;i++)
		close(i);

	while ((start=malloc(csize)) == NULL) err(2, "no memory, malloc failed", "");
	if(autoboot() != 0) state = '8';

	for(;;) {
		a0[5] = state;
		strcpy(*argv, a0);
		xstate = 0;
		while(rdstate) {
			rdstate = 0;
			readttys();
			statecnt[state-'0']++;
			runrc();
			xstate = 0;
		}

		invttys();
		waitttys();
	}
}

fit(str)
char *str;
{
/* returns offset from start where string stored. */
	int len, rstr;
	char *ostart;

	len = strlen(str) + 1;
	if((len+nextcmd) > csize) {
		csize += ASIZE;
		ostart = start;
		if ((start=realloc(start, csize)) == NULL) {
			start = ostart;
			csize -= ASIZE;
			err(0, "no memory, malloc failed", "");
			return(NOCMD);
		}
	}

	strcpy(start+nextcmd, str);
	rstr = nextcmd;
	nextcmd += len;
	return(rstr);
}

invttys()
{
	int i;

	for(i=0;i<idlim;i++)
		if((itab[i].in_proc == NOPROC) && (itab[i].in_cmd != NOCMD)) {
			if(*((itab[i].in_cmd)+start) == ':') {

				itab[i].in_proc = iexec(start+(itab[i].in_cmd)+2);
				itab[i].in_cmd = NOCMD;
			}
			else itab[i].in_proc = iexec(start+(itab[i].in_cmd));
		}
}

iexec(s)
char *s;
{
	int pid, i;
	char rs[256];

	strcpy(rs, "exec ");
	strcat(rs, s);
	switch(pid=fork()) {
	case FORKERR:
		err(0, "fork failed for:", s);
		return(NOPROC);
	case CHILD:
		for(i=0;i<=10;i++)
			close(i);
		open("/", 0);
		dup(0);
		dup(0);
		rsetsigs();
		setpgrp();
		execl(SH, "sh", "-c", rs, 0);
		err(1, "exec failed for:", s);
	default:
		return(pid);
	}
}

waitttys()
{
	int status;
	int pnum;
	int deadind;

	while(((pnum=wait(&status))==-1) && (rdstate==0) && (xstate==0))
		err(2, "no processes running", "");

	deadind=lookproc(pnum);
	if(deadind!=NOIND)
		itab[deadind].in_proc=NOPROC;
	return(pnum);
}

runrc()
{
	char rcstring[100], iabuf[5];
	int i;
	char blank = ' ', eos = '\0';

	strcpy(rcstring, "exec ");
	strcat(rcstring, RC);
	i=strlen(rcstring);
	rcstring[i++] = blank;
	rcstring[i++] = state;
	if(xstate==1)
		rcstring[i++] = 'x';
	rcstring[i++] = blank;
	rcstring[i++] = eos;
	itoa(statecnt[state-'0']-1, iabuf);
	strcat(rcstring, iabuf);
	i = strlen(rcstring);
	rcstring[i++] = blank;
	rcstring[i++] = lstate;
	rcstring[i++] = eos;

	while((rcid=fork())==FORKERR) {
		err(0, "fork failed for: RC", "");
		sleep(2);
	}

	if(rcid==CHILD) {
		for(i=0;i<=10;i++)
			close(i);

		open("/", 0);
		dup(0);
		dup(0);

		rsetsigs();
		setpgrp();
		execl(SH, "RC", "-c", rcstring, 0);
		err(1, "exec failed for: RC", "");
	}

	setsigs();
	if(xstate==0) {
		signal(SIGALRM, rckill);
		alarm(RCLIM);
	}
	while((waitttys()!=rcid) && (rdstate==0));
	if(xstate==0) {
		alarm(0);
		signal(SIGALRM, SIG_IGN);
	}
}

readttys()
{
	FILE *tfile;
	char ocmd[258], icmd[256], iflags[5], *ifp;
	int itp, i, tset, istate, holdc;
	char *fp;
	char id[2];

	for(i=0;i<idlim;i++)
		itab[i].in_cmd = NOCMD;

	nextcmd=0;

	while((tfile = fopen(TTYS, "r"))==NULL)
		err(2, "inittab open failed", "");
	rdstate = 0;
	for(;;) {
		if((istate=getc(tfile))==EOF)
			break;
		else if(istate=='\n')
			continue;
		else if((istate != state) || (getc(tfile)!=':')) {
			while(((holdc=getc(tfile))!=EOF) && (holdc!='\n'))
				;
			if(holdc==EOF)
				break;
			else continue;
		}

		if((int)(id[0]=getc(tfile))==EOF)
			break;
		else if(id[0]=='\n')
			continue;
		if((int)(id[1]=getc(tfile))==EOF)
			break;
		else if(id[1]=='\n')
			continue;

		if(getc(tfile)!=':') {
			while(((holdc=getc(tfile))!=EOF) && (holdc!='\n'))
				;
			if(holdc==EOF)
				break;
			else continue;
		}

		ifp=iflags;
		while(((*ifp=getc(tfile))!=':') && ((int)*ifp!=EOF) && (*ifp!='\n'))
			ifp++;
		if((int)*ifp==EOF)
			break;
		else if(*ifp=='\n')
			continue;
		else *ifp = '\0';

		ifp=icmd;
		while(((int)(*ifp=getc(tfile))!=EOF) && (*ifp!='\n'))
			ifp++;
		*ifp = '\0';

		itp = lookid(id);
		if(strchr(iflags, 'o') || (*icmd == '\0'))
			;
		else if(strchr(iflags, 'c'))
			itab[itp].in_cmd = fit(icmd);
		else {
			ocmd[0] = ':';
			ocmd[1] = ' ';
			ocmd[2] = '\0';
			strcat(ocmd, icmd);
			itab[itp].in_cmd = fit(ocmd);
		}

		tset=0;
		for(fp=iflags;*fp;fp++) switch(*fp) {
		case 't':
			if((itab[itp].in_proc) > NOPROC) {
				/* killpgrp */
				kill(-(itab[itp].in_proc), 15);
				tset=1;
			}
			break;
		case 'k':
			if(itab[itp].in_proc > NOPROC) {
				/* killpgrp */
				if(tset) sleep(1);
				kill(-(itab[itp].in_proc), 9);
			}
			break;
		}
	}
	fclose(tfile);
}

lookproc(proc)
int proc;
{
	int i;

	for(i=0;i<idlim;i++)
		if(itab[i].in_proc==proc) return(i);
	return(NOIND);
}

lookid(id)
char id[2];
{
	int i;

	for(i=0;i<idlim;i++)
		if((itab[i].in_id[0]==id[0]) && (itab[i].in_id[1]==id[1])) return(i);

	itab[idlim].in_id[0]=id[0];
	itab[idlim].in_id[1]=id[1];
	itab[idlim].in_proc=NOPROC;
	itab[idlim].in_cmd = NOCMD;

	return(idlim++);
}

chst(sno)
{
	int i;

	ignsigs();
	if (sno == STx || sno == SIGPWR) {
		signal(STx, chst);
		signal(SIGPWR, chst);
		xstate = 1;
		runrc();
		return;
	}

	lstate = state;
	for(i=1;ST[i] != sno;i++) ;
	state = i + '0';
	rdstate = 1;
}

rckill()
{
	char itoabuf[6];

	itoa(RCLIM, itoabuf);
	err(0, "RC over time limit of:", itoabuf);
	kill(-rcid, 9);
	xstate = 1;
}

ignsigs()
{
	int i;

	for(i=0;i<=3;i++)
		signal(sigs[i], SIG_IGN);

	for(i=1;i<=9;i++)
		signal(ST[i], SIG_IGN);
}

setsigs()
{
	int i;

	for(i=0;i<=3;i++)
		signal(sigs[i], SIG_IGN);

	for(i=1;i<=9;i++)
		signal(ST[i], chst);
}

rsetsigs()
{
	int i;

	for(i=0;i<=3;i++)
		signal(sigs[i], SIG_DFL);

	for(i=1;i<=9;i++)
		signal(ST[i], SIG_DFL);
}

err(act, msg1, msg2)
int act;
char *msg1, *msg2;
{
	FILE *errfile;
	int i;

	if((errfile=fopen(DIAG, "a")) != NULL) {
		setbuf(errfile, (char *)NULL);
		fputs("init: ", errfile);
		fputs(msg1, errfile);
		fputs(msg2, errfile);
		putc('\n', errfile);
	}

	switch(act) {
	case 0:
		fclose(errfile);
		return;
	case 1:
		exit(2);
	case 2:
		if(errfile != NULL) {
			fputs("trying default shell\n", errfile);
			fputs(ERRACT, errfile);
			fputs("\n", errfile);
		}

		if((i=iexec(ERRACT))==NOPROC)
			err(1, "exec failed for: final default shell", "");
		rdstate = 0;
		setsigs();
		while((waitttys()!=i) && (rdstate==0))
			;
		if(rdstate && (errfile != NULL))
			fputs("WARNING: default shell not terminated\n", errfile);
		fclose(errfile);
		return;
	}
}

itoa(n, s)
char *s;
int n;
{
	int i=0, c, j, sign;

	if((sign=n) < 0)
		n = -n;

	do {
		s[i++] = n % 10 + '0';
	} while((n /= 10) > 0);

	if(sign < 0)
		s[i++] = '-';
	s[i] = '\0';

	for(i=0, j=strlen(s)-1;i<j;i++, j--) {
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}
/* If P_SWIT is 0x08, return 1; otherwise, return 0; */
autoboot()
{
	register pid;
	register liofn;
	int status;
	int swit;
	static int boot = 0;

	if (boot) {
		return(0);
	}
	boot = 1;
	if ((liofn = open(lio, 0)) < 0) {
		return(0);
	}
	lseek(liofn, (long) P_SWIT, 0);
	read(liofn, &swit, sizeof(swit));
	close(liofn);
	if (swit & 0x08) {
		return(0);
	}
	return(1);
}
