#
/*
 *
 *	UNIX debugger
 *
 */

#include "defs.h"


MSG		NOFORK;
MSG		ENDPCS;
MSG		BADWAIT;

CHAR		*lp;
STRING		sigint;
INT		sigqit;

/* breakpoints */
BKPTR		bkpthead;

REGLIST		reglist[];

CHAR		lastc;
POS		corhdr[];
POS		*endhdr;

INT		fcor;
INT		fsym;
STRING		errflg;
INT		errno;
INT		signo;

L_INT		dot;
STRING		symfil;
INT		wtflag;
INT		pid;
L_INT		expv;
INT		adrflg;
L_INT		loopcnt;





/* service routines for sub process control */

getsig(sig)
{	return(expr(0) ? shorten(expv) : sig);
}

INT		userpc=1;

runpcs(runmode, execsig)
{
	INT		rc;
	REG BKPTR	bkpt;
	INT		oldpc;
	IF adrflg
	THEN userpc=shorten(dot); ptrace(WUREGS,pid,ctob(USIZE)+2*pc,userpc);
	FI
	setbp();
	printf("%s: running\n", symfil);

	WHILE (loopcnt--)>0
	DO
#ifdef DEBUG
		printf("\ncontinue %d %d\n",userpc,execsig);
#endif
		stty(0,&usrtty);
		IF runmode != SINGLE
		THEN
			ptrace(runmode,pid,1,execsig);
			bpwait(); chkerr();
		ELSE
			oldpc = adrflg ? userpc : endhdr[pc];
			REP
				ptrace(runmode,pid,1,execsig);
				bpwait(); chkerr();
			PER oldpc == ptrace(RUREGS,pid,ctob(USIZE)+2*pc,0)
			DONE
		FI
		readregs();

		/*look for bkpt*/
		IF signo==0 ANDF (bkpt=scanbkpt(endhdr[pc]-2))
		THEN /*stopped at bkpt*/
		     userpc=endhdr[pc]=bkpt->loc;
		     ptrace(WUREGS,pid,ctob(USIZE)+2*pc,userpc);
		     IF bkpt->flag==BKPTEXEC
			ORF ((bkpt->flag=BKPTEXEC, command(bkpt->comm,':')) ANDF --bkpt->count)
		     THEN execbkpt(bkpt); execsig=0;
			IF runmode != SINGLE THEN loopcnt++; ELSE readregs(); FI
			  userpc=1;
		     ELSE bkpt->count=bkpt->initcnt;
			  rc=1;
		     FI
		ELSE rc=0; execsig=signo; userpc=1;
		FI
	OD
	return(rc);
}

endpcs()
{
	REG BKPTR	bkptr;
	IF pid
	THEN ptrace(EXIT,pid,0,0); pid=0; userpc=1;
	     FOR bkptr=bkpthead; bkptr; bkptr=bkptr->nxtbkpt
	     DO IF bkptr->flag
		THEN bkptr->flag=BKPTSET;
		FI
	     OD
	FI
}

setup()
{
	close(fsym); fsym = -1;
	IF (pid = fork()) == 0
	THEN ptrace(SETTRC,0,0,0);
	     signal(SIGINT,sigint); signal(SIGQUIT,sigqit);
	     doexec(); exit(0);
	ELIF pid == -1
	THEN error(NOFORK);
	ELSE bpwait(); readregs(); lp[0]=EOR; lp[1]=0;
	     fsym=open(symfil,wtflag);
	     IF errflg
	     THEN printf("%s: cannot execute\n",symfil);
		  endpcs(); error(0);
	     FI
	FI
}

execbkpt(bkptr)
BKPTR	bkptr;
{	INT		bkptloc;
	INT		oldpc;
#ifdef DEBUG
	printf("exbkpt: %d\n",bkptr->count);
#endif
	bkptloc = bkptr->loc;
	ptrace(WIUSER,pid,bkptloc,bkptr->ins);
	stty(0,&usrtty);
	oldpc = bkptloc;
	REP
		ptrace(SINGLE,pid,1,0);
		bpwait(); chkerr();
	PER oldpc == ptrace(RUREGS,pid,ctob(USIZE)+2*pc,0)
	DONE
	ptrace(WIUSER,pid,bkptloc,BPT);
	bkptr->flag=BKPTSET;
}


doexec()
{
	STRING		argl[MAXARG];
	CHAR		args[LINSIZ];
	STRING		p, *ap, filnam;
	ap=argl; p=args;
	*ap++=symfil;
	REP	IF rdc()==EOR THEN break; FI
		*ap = p;
		WHILE lastc!=EOR ANDF lastc!=SP ANDF lastc!=TB DO *p++=lastc; readchar(); OD
		*p++=0; filnam = *ap+1;
		IF **ap=='<'
		THEN	close(0);
			IF open(filnam,0)<0
			THEN	printf("%s: cannot open\n",filnam); exit(0);
			FI
		ELIF **ap=='>'
		THEN	close(1);
			IF creat(filnam,0666)<0
			THEN	printf("%s: cannot create\n",filnam); exit(0);
			FI
		ELSE	ap++;
		FI
	PER lastc!=EOR DONE
	*ap++=0;
	execv(symfil, argl);
}

BKPTR	scanbkpt(adr)
{
	REG BKPTR	bkptr;
	FOR bkptr=bkpthead; bkptr; bkptr=bkptr->nxtbkpt
	DO IF bkptr->flag ANDF bkptr->loc==adr
	   THEN break;
	   FI
	OD
	return(bkptr);
}

delbp()
{
	REG INT		a;
	REG BKPTR	bkptr;
	FOR bkptr=bkpthead; bkptr; bkptr=bkptr->nxtbkpt
	DO IF bkptr->flag
	   THEN a=bkptr->loc;
		ptrace(WIUSER,pid,a,bkptr->ins);
	   FI
	OD
}

setbp()
{
	REG INT		a;
	REG BKPTR	bkptr;

	FOR bkptr=bkpthead; bkptr; bkptr=bkptr->nxtbkpt
	DO IF bkptr->flag
	   THEN a = bkptr->loc;
		bkptr->ins = ptrace(RIUSER, pid, a, 0);
		ptrace(WIUSER, pid, a, BPT);
		IF errno
		THEN prints("cannot set breakpoint: ");
		     psymoff(leng(bkptr->loc),ISYM,"\n");
		FI
	   FI
	OD
}

bpwait()
{
	REG INT w;
	INT stat;

	signal(SIGINT, 1);
	WHILE (w = wait(&stat))!=pid ANDF w != -1 DONE
	signal(SIGINT,sigint);
	gtty(0,&usrtty);
	stty(0,&adbtty);
	IF w == -1
	THEN pid=0;
	     errflg=BADWAIT;
	ELIF (stat & 0177) != 0177
	THEN IF signo = stat&0177
	     THEN sigprint();
	     FI
	     IF stat&0200
	     THEN prints(" - core dumped");
		  close(fcor);
		  setcor();
	     FI
	     pid=0;
	     errflg=ENDPCS;
	ELSE signo = stat>>8;
	     IF signo!=SIGTRAP
	     THEN sigprint();
	     ELSE signo=0;
	     FI
	     flushbuf();
	FI
}

readregs()
{
	/*get REG values from pcs*/
	REG i;
	FOR i=0; i<NUMREGS; i++
	DO endhdr[reglist[i].roffs] =
		    ptrace(RUREGS, pid, ctob(USIZE)+2*reglist[i].roffs, 0);
	OD

	/* REALing poINT		*/
/*
	FOR i=FROFF; i<FRLEN+FROFF; i++
	DO corhdr[i] = ptrace(RUREGS,pid,i,0); OD
*/
}


