#
/*
 *
 *	UNIX debugger
 *
 */

#include "defs.h"


MSG		LONGFIL;
MSG		NOTOPEN;
MSG		A68BAD;
MSG		A68LNK;
MSG		BADMOD;

MAP		txtmap;
MAP		datmap;

SYMTAB		symbol;
INT		lastframe;
INT		callpc;
INT		calledpc;
CHAR		*octint = "%o";
CHAR		*hexint = "%x";
CHAR		*outdefault = "%o";

INT		infile;
INT		outfile;
CHAR		*lp;
INT		maxoff;
INT		maxpos;
INT		octal;
INT		hex;

/* symbol management */
L_INT		localval;

/* breakpoints */
BKPTR		bkpthead;

REGLIST reglist [] = {
		"fcw", fcw,
		"pc",  pc,
		"sp",  sp,
		"r14", r14,
		"r13", r13,
		"r12", r12,
		"r11", r11,
		"r10", r10,
		"r9",  r9,
		"r8",  r8,
		"r7",  r7,
		"r6",  r6,
		"r5",  r5,
		"r4",  r4,
		"r3",  r3,
		"r2",  r2,
		"r1",  r1,
		"r0",  r0,
};

REGLIST	freglist [] = {
		"rq0",	r0,
		"rq4",	r4,
		"rq8",	r8,
};

char		lastc;
POS		corhdr[];
POS		*endhdr;

INT		fcor;
STRING		errflg;
INT		signo;


L_INT		dot;
L_INT		var[];
STRING		symfil;
STRING		corfil;
INT		pid;
L_INT		adrval;
INT		adrflg;
L_INT		cntval;
INT		cntflg;

STRING		signals[] = {
		"",
		"hangup",
		"interrupt",
		"quit",
		"illegal instruction",
		"trace/BPT",
		"IOT",
		"EMT",
		"floating exception",
		"killed",
		"bus error",
		"memory fault",
		"bad system call",
		"broken pipe",
		"alarm call",
		"terminated",
};




/* general printing routines ($) */

printtrace(modif)
{
	INT		narg, i, stat, name, limit, inst;
	POS		dynam;
	REG BKPTR	bkptr;
	CHAR		hi, lo;
	INT		word;
	STRING		comptr;
	L_INT		argp, frame, link;
	SYMPTR		symp;

	IF cntflg==0 THEN cntval = -1; FI

	switch (modif) {

	    case '<':
	    case '>':
		{CHAR		file[64];
		INT		index;

		index=0;
		IF modif=='<'
		THEN	iclose();
		ELSE	oclose();
		FI
		IF rdc()!=EOR
		THEN	REP file[index++]=lastc;
			    IF index>=63 THEN error(LONGFIL); FI
			PER readchar()!=EOR DONE
			file[index]=0;
			IF modif=='<'
			THEN	infile=open(file,0);
				IF infile<0
				THEN	infile=0; error(NOTOPEN);
				FI
			ELSE	outfile=open(file,1);
				IF outfile<0
				THEN	outfile=creat(file,0644);
				ELSE	lseek(outfile,0L,2);
				FI
			FI

		FI
		lp--;
		}
		break;

	    case 'x':
		outdefault = hexint;
		break;

	    case 'o':
		outdefault = octint;
		octal = TRUE; break;

	    case 'd':
		octal = FALSE; break;

	    case 'q': case 'Q': case '%':
		done();

	    case 'w': case 'W':
		maxpos=(adrflg?adrval:MAXPOS);
		break;

	    case 's': case 'S':
		maxoff=(adrflg?adrval:MAXOFF);
		break;

	    case 'v': case 'V':
		prints("variables\n");
		FOR i=0;i<=35;i++
		DO IF var[i]
		   THEN printc((i<=9 ? '0' : 'a'-10) + i);
			printf(" = %Q\n",var[i]);
		   FI
		OD
		break;

	    case 'm': case 'M':
		printmap("? map",&txtmap);
		printmap("/ map",&datmap);
		break;

	    case 0: case '?':
		IF pid
		THEN printf("pcs id = %d\n",pid);
		ELSE prints("no process\n");
		FI
		sigprint(); flushbuf();

	    case 'r': case 'R':
		printregs();
		return;

	    case 'f': case 'F':
		printfregs();
		return;

	    case 'c': case 'C':
		frame=(adrflg?adrval:endhdr[r14])&EVEN; lastframe=0;
		callpc=(adrflg?get(frame+2,DSP):endhdr[pc]);
		calledpc = dot;
		WHILE cntval--
		DO	chkerr();
			narg = findroutine(frame,ISYM);
			for (i=6; i<26; i+=2) {
				if (callpc < i)
					break;
				inst = get(leng(callpc-i),ISP);
				if (((inst & 0xfeff) == 0x0cf5)
					|| ((inst & 0xfef0) == 0x2ef0)) {
					narg++;
					break;
				}
				if ((inst & 0xffff) == 0x5f00)
					break;
				if ((inst & 0xff0f) == 0x1f00)
					break;
			}
			printf("%.8s(", symbol.symc);
			argp = frame+4;
			IF --narg >= 0
			THEN	printf(outdefault, get(argp, DSP));
			FI
			WHILE --narg >= 0
			DO	argp += 2;
				printc(',');
				printf(outdefault, get(argp, DSP));
			OD
			printc(')');

			IF modif=='C'
				THEN
				IF word = findsym(calledpc,-ISYM)
				THEN
					prints("  +");
					printf(outdefault, word);
				FI
			FI

			printc('\n');

			IF modif=='C'
			THEN WHILE localsym(frame)
			     DO word=get(localval,DSP);
				printf("%8t%.8s:%10t", symbol.symc);
				IF errflg THEN prints("?\n"); errflg=0; ELSE printf(outdefault,word); printc('\n'); FI
			     OD
			FI

			lastframe=frame;
			frame=get(frame, DSP)&EVEN;
			calledpc = callpc;
			IF frame==0 THEN break; FI
		OD
		break;

	    /*print externals*/
	    case 'e': case 'E':
		symset();
		WHILE (symp=symget())
		DO chkerr();
		   IF (symp->symf)==043 ORF (symp->symf)==044
		   THEN printf("%.8s:%12t", symp->symc);
			printf(outdefault, get(leng(symp->symv),DSP));
			printc('\n');
		   FI
		OD
		break;

	    case 'a': case 'A':
		frame=(adrflg ? adrval : endhdr[r4]);

		WHILE cntval--
		DO chkerr();
		   stat=get(frame,DSP); dynam=get(frame+2,DSP); link=get(frame+4,DSP);
		   IF modif=='A'
		   THEN printf("%8O:%8t%-8o,%-8o,%-8o",frame,stat,dynam,link);
		   FI
		   IF stat==1 THEN break; FI
		   IF errflg THEN error(A68BAD); FI

		   IF get(link-4,ISP)!=04767
		   THEN IF get(link-2,ISP)!=04775
			THEN error(A68LNK);
			ELSE /*compute entry point of routine*/
			     prints(" ? ");
			FI
		   ELSE printf("%8t");
		        valpr(name=shorten(link)+get(link-2,ISP),ISYM);
			name=get(leng(name-2),ISP);
			printf("%8t\""); limit=8;
			REP word=get(leng(name),DSP); name += 2;
			    lo=word&LOBYTE; hi=(word>>8)&LOBYTE;
			    printc(lo); printc(hi);
			PER lo ANDF hi ANDF limit-- DONE
			printc('"');
		   FI
		   limit=4; i=6; printf("%24targs:%8t");
		   WHILE limit--
		   DO printf("%8t%o",get(frame+i,DSP)); i += 2; OD
		   printc(EOR);

		   frame=dynam;
		OD
		errflg=0;
		flushbuf();
		break;

	    /*set default c frame*/
	    /*print breakpoints*/
	    case 'b': case 'B':
		printf("breakpoints\ncount%8tbkpt%24tcommand\n");
		FOR bkptr=bkpthead; bkptr; bkptr=bkptr->nxtbkpt
		DO IF bkptr->flag
		   THEN printf("%-8.8d",bkptr->count);
			psymoff(leng(bkptr->loc),ISYM,"%24t");
			comptr=bkptr->comm;
			WHILE *comptr DO printc(*comptr++); OD
		   FI
		OD
		break;

	    default: error(BADMOD);
	}

}

printmap(s,amap)
STRING	s; MAP *amap;
{
	int file;
	file=amap->ufd;
	printf("%s%12t`%s'\n",s,(file<0 ? "-" : (file==fcor ? corfil : symfil)));
	mapout("b1",amap->b1);
	mapout("e1",amap->e1);
	mapout("f1",amap->f1);
	printf("\n");
	mapout("b2",amap->b2);
	mapout("e2",amap->e2);
	mapout("f2",amap->f2);
	printc(EOR);
}

mapout(str,val)
char *str;
long val;
{
	IF outdefault == hexint
	THEN	printf("%s = %-16X",str,val);
	ELSE	printf("%s = %-16Q",str,val);
	FI
}

printfregs()
{
	REG i;
	REG REGPTR	p;
	L_REAL *f;

	FOR p=freglist; p < &freglist[NUMFREGS]; p++
	DO
		printf("%s%8t", p->rname);
		f = (L_REAL *)&endhdr[p->roffs];
		printf("%-32.18f\n", *f);
	OD
}

printregs()
{
	REG REGPTR	p;
	INT		v;

	FOR p=reglist; p < &reglist[NUMREGS]; p++
	DO
		printf("%s%8t", p->rname);
		printf(outdefault, v=endhdr[p->roffs]);
		printf("%8t");
		valpr(v,(p->roffs==pc?ISYM:DSYM));
		printc(EOR);
	OD
	printpc();
}

getreg(regnam)
{
	REG REGPTR	p;
	REG STRING	regptr;
	CHAR		regnxt;
	CHAR		savc;
	regnxt=readchar();
	FOR p=reglist; p<&reglist[18]; p++
	DO	regptr=p->rname;
		IF (regnam == *regptr++) ANDF (regnxt == *regptr++)
		THEN	IF (*regptr == 0)
			THEN	return(p->roffs);
			ELSE	savc = regnxt;
				regnxt = readchar();
				IF (regnxt != *regptr)
				THEN	lp--;
					regnxt = savc;
				ELSE	return(p->roffs);
				FI
			FI
		FI
	OD
	lp--;
	return(0);
}

printpc()
{
	dot=endhdr[pc];
	psymoff(dot,ISYM,":%16t"); putins(chkget(dot,ISP),ISP,0);
	printc(EOR);
}

sigprint()
{
	prints(signals[signo]);
}

