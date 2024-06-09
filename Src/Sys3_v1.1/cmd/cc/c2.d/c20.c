#
/*
 *	 C object code improver
 */

#include "c2.h"
char *ccstrlook();

struct optab optab[] = {
	"jr",	JR,
	"jr",	CJR,
	"jpr",	JP,
	"jpr",	CJP,
	".globl",EROU,
	"ld",	LD,
	"clr",	CLR,
	"com",	COM,
	"inc",	INC,
	"dec",	DEC,
	"neg",	NEG,
	"test",	TST,
	"exts",	EXTS,
	"cp",	CP,
	"add",	ADD,
	"sub",	SUB,
	"bit",	BIT,
	"set",	SET,
	"res",	RES,
	"and",	AND,
	"or",	OR,
	"mult",	MUL,
	"div",	DIV,
	"xor",	XOR,
	"sda",	SDA,
	"sdl",	SDL,
	"lda",	LDA,
	"ldm",	LDM,
	"ldk",	LDK,
	"ldir",	LDIR,
	"push",	PUSH,
	"pop",	POP,
	"tcc",	TCC,
	"sla",	SLA,
	"sll",	SLL,
	"sra",	SRA,
	"srl",	SRL,
	".text",TEXT,
	".data",DATA,
	".bss",	BSS,
	".even",EVEN,
	".ascii",ASCII,
	".word",WORD,
	".byte",DBYTE,
	"faddd",ADDF,
	"fsubd",SUBF,
	"fdivd",DIVF,
	"fmuld",MULF,
	"fcpd",	CPF,
	"fnegd",NEGF,
	"djnz",	SOB,
	"call",	CALL,
	".end",	END,
	0,	0};

struct optab ccodes[] = {
	"z,",	JEQ,
	"eq,",	JEQ,
	"nz,",	JNE,
	"ne,",	JNE,
	"le,",	JLE,
	"ge,",	JGE,
	"lt,",	JLT,
	"gt,",	JGT,
	"ult,",	JLO,
	"ugt,",	JHI,
	"ule,",	JLOS,
	"uge,",	JHIS,
	"pl,",	JPL,
	"mi,",	JMI,
	0,	0
};


char	revbr[] = { JNE, JEQ, JGT, JLT, JGE, JLE,
			JHIS, JLOS, JHI, JLO, JMI, JPL };
int	isn	= 20000;
int	lastseg	= -1;
int	mydebug;

main(argc, argv)
char **argv;
{
	register int niter, maxiter, isend;
	int nflag;

	if (argc>1 && argv[1][0]=='+') {
		argc--;
		argv++;
		debug++;
	}
	nflag = 0;
	if (argc>1 && argv[1][0]=='-') {
		argc--;
		argv++;
		nflag++;
	}
	if (argc>1 && argv[1][0]=='!'){
		argc--;
		argv++;
		mydebug++;
	}
	if (argc>1) {
		if (freopen(argv[1], "r", stdin) == NULL) {
			fprintf(stderr, "C2: can't find %s\n", argv[1]);
			exit(1);
		}
	}
	if (argc>2) {
		if (freopen(argv[2], "w", stdout) == NULL) {
			fprintf(stderr, "C2: can't create %s\n", argv[2]);
			exit(1);
		}
	}
	lasta = firstr = lastr = sbrk(2);
	maxiter = 0;
	opsetup();
	do {
		isend = input();
		movedat();
		niter = 0;
		do {
			refcount();
			do {
				iterate();
				clearreg();
				niter++;
			} while (nchange);
			comjump();
			rmove();
		} while (nchange || jumpsw());
		while( shortjump() );
		output();
		if (niter > maxiter)
			maxiter = niter;
		lasta = firstr;
	} while (isend);
	if (nflag) {
		fprintf(stderr, "%d iterations\n", maxiter);
		fprintf(stderr, "%d jumps to jumps\n", nbrbr);
		fprintf(stderr, "%d inst. after jumps\n", iaftbr);
		fprintf(stderr, "%d jumps to .+2\n", njp1);
		fprintf(stderr, "%d jump relatives\n", njr);
		fprintf(stderr, "%d redundant labels\n", nrlab);
		fprintf(stderr, "%d cross-jumps\n", nxjump);
		fprintf(stderr, "%d code motions\n", ncmot);
		fprintf(stderr, "%d branches reversed\n", nrevbr);
		fprintf(stderr, "%d redundant loads\n", redunm);
		fprintf(stderr, "%d short constants\n", nshortc);
		fprintf(stderr, "%d simplified addresses\n", nsaddr);
		fprintf(stderr, "%d simple compares to 0\n", ncomp0);
		fprintf(stderr, "%d loops inverted\n", loopiv);
		fprintf(stderr, "%d redundant jumps\n", nredunj);
		fprintf(stderr, "%d common seqs before jmp's\n", ncomj);
		fprintf(stderr, "%d skips over jumps\n", nskip);
		fprintf(stderr, "%d djnz's added\n", nsob);
		fprintf(stderr, "%d redundant tst's\n", nrtst);
		fprintf(stderr, "%d shits converted\n", nshft);
		fprintf(stderr, "%d literals eliminated\n", nlit);
		fprintf(stderr, "%dK core\n", (((int)lastr+01777)>>10)&077);
	}
	exit(0);
}

input()
{
	register struct node *p, *lastp;
	register int oper;

	lastp = &first;
	for (;;) {
		oper = getline();
		switch (oper&0377) {
	
		case LABEL:
			p = (struct node *)alloc(sizeof first);
			if (line[0] == 'L') {
				p->op = LABEL;
				p->subop = 0;
				p->labno = getnum(line+1);
				p->code = 0;
			} else {
				p->op = DLABEL;
				p->subop = 0;
				p->labno = 0;
				p->code = copy(1, line);
			}
			break;

		case JP:
		case CJP:
		case JR:
		case CJR:
		case JSW:
			p = (struct node *)alloc(sizeof first);
			p->op = oper&0377;
			p->subop = oper>>8;
			if (*curlp=='L' && (p->labno = getnum(curlp+1)))
				p->code = 0;
			else {
				p->labno = 0;
				p->code = copy(1, curlp);
				trunc(p);
			}
#ifndef SMALL
			whregs(p);
#endif
			break;

		default:
			p = (struct node *)alloc(sizeof first);
			p->op = oper&0377;
			p->subop = oper>>8;
			p->labno = 0;
			p->code = copy(1, curlp);
			trunc(p);
			break;

		}
		p->forw = 0;
		p->back = lastp;
		lastp->forw = p;
		lastp = p;
		p->ref = 0;
		if( (p->op == WORD || p->op == DBYTE || p->op == ASCII)
		   && p->back->op == LABEL )
			p->back->op = NLABEL;
#ifndef SMALL
		whregs(p);
#endif
		if (oper==EROU)
			return(1);
		if (oper==END)
			return(0);
	}
}

getline()
{
	register char *lp;
	register c;
	register quote = 0;

	lp = line;
	while( (c = getchar())==' ' || c=='\t' || c=='\n' || c=='/') {
		if( c != '/' )
			continue;
		if( getchar() == '/' )
			c = '\n';
		else
			c = '/';
		while( getchar() != c );
	}
	do {
		if(c=='"')
			quote++;
		if (c==':' && !quote) {
			*lp++ = 0;
			return(LABEL);
		}
		if (c=='\n') {
			*lp++ = 0;
			return(oplook());
		}
		if (lp >= &line[LSIZE-2]) {
			fprintf(stderr, "C2: Sorry, input line too long\n");
			exit(1);
		}
		*lp++ = c;
	} while ((c = getchar()) != EOF);
	*lp++ = 0;
	return(END);
}

getnum(ap)
char *ap;
{
	register char *p;
	register n, c;

	p = ap;
	n = 0;
	while ((c = *p++) >= '0' && c <= '9')
		n = n*10 + c - '0';
	if (*--p != 0)
		return(0);
	return(n);
}

output()
{
	register struct node *t;
	register struct optab *oper;
	register int byte;

	t = &first;
	while (t = t->forw)  {
	if( mydebug )
		printf("/*%d*/",ilen(t));
	switch (t->op) {

	case END:
		return;

	case LABEL:
	case NLABEL:
		printf("L%d:\n", t->labno);
		continue;

	case DLABEL:
		printf("%s:\n", t->code);
		continue;

	case TEXT:
	case DATA:
	case BSS:
		lastseg = t->op;

	default:
		if( ((byte = t->subop) == BYTE) || (byte == LONG) )
			t->subop = 0;
		for (oper = optab; oper->opstring!=0; oper++) 
			if ((oper->opcode&0377) == t->op ) {
				printf("\t%s", oper->opstring);
				if (byte==BYTE)
					printf("b");
				if( byte==LONG )
					printf("l");
				break;
			}
		if( t->op == CJR || t->op == CJP || t->op == TCC ) {
			printf( "\t%s", ccstrlook(t->subop) );
		}
		if (t->code) {
			if( t->op != CJR && t->op != CJP && t->op != TCC )
				printf("\t");
			printf("%s\n", t->code);
		} else if (t->op==JP || t->op==CJP
		   || t->op==JR || t->op==CJR) {
			if( t->op!=CJP && t->op!=CJR) printf("\t");
			printf("L%d\n", t->labno);
		} else
			printf("\n");
		continue;

	case JSW:
		printf("\t.word\tL%d\n", t->labno);
		continue;

	case SOB:
		printf("\tdjnz\t%s", t->code);
		if (t->labno)
			printf(",L%d", t->labno);
		printf("\n");
		continue;

	case 0:
		if (t->code)
			printf("%s", t->code);
		printf("\n");
		continue;
	}
	}
}

char *
copy(na, ap)
char *ap;
{
	register char *p, *np;
	char *onp;
	register n;

	p = ap;
	n = 0;
	if (*p==0)
		return(0);
	do
		n++;
	while (*p++);
	if (na>1) {
		p = (&ap)[1];
		while (*p++)
			n++;
	}
	onp = np = alloc(n);
	p = ap;
	while (*np++ = *p++)
		;
	if (na>1) {
		p = (&ap)[1];
		np--;
		while (*np++ = *p++);
	}
	return(onp);
}

trunc(p)
struct node *p;
{
register char *ap;

	if(p->op == 0 || p->op >= TEXT || (ap = p->code) == 0)
		return;
	do {
		if( *ap == ' ' || *ap == '\t' ) {
			*ap = '\0';
			return;
		}
	} while ( *ap != '\0' && *ap++ != '\n' );
}

opsetup()
{
	register struct optab *optp, **ophp;
	register char *p;

	for (optp = optab; p = optp->opstring; optp++) {
		ophp = &ophash[(((p[0]<<3)+(p[1]<<1)/*+p[2]*/)&077777) % OPHS];
		while (*ophp++)
			if (ophp > &ophash[OPHS])
				ophp = ophash;
		*--ophp = optp;
	}
}

oplook()
{
	register struct optab *optp;
	register char *lp, *np;
	register char *tempnp,*templp;
	int wlength;
	static char tmpop[32];
	char tempcc[32];
	struct optab **ophp;

	if (line[0]=='\0') {
		curlp = line;
		return(0);
	}
	np = tmpop;
	for (lp = line; *lp && *lp!=' ' && *lp!='\t';)
		*np++ = *lp++;
	*np++ = 0;
	while( *lp == '\t' || *lp == ' ' ) ++lp;
	curlp = lp;
	ophp = &ophash[(((tmpop[0]<<3)+(tmpop[1]<<1)/*+tmpop[2]*/)&077777) % OPHS];
	while (optp = *ophp) {
		np = optp->opstring;
		lp = tmpop;
		while (*lp == *np++)
			if (*lp++ == 0) {
				if( optp->opcode != WORD ) {
					if( (wlength = optp->opcode) == JP
					    || wlength == JR
					    || wlength == TCC ) {
						lp = curlp;
						np = tempcc;
						while( *lp!=0 && *lp!=',' ) {
							*np++ = *lp++;
						}
						*np = *lp;
						if( wlength == JR )
							wlength = JP;
						if( *lp != ',' )
							return( wlength );
						curlp = ++lp;
						*++np = 0;
						if( wlength == JP )
							wlength = CJP;
						return( wlength
						   |(cclook(tempcc)<<8) );
					}	
					return(optp->opcode);
				}
				if( *(lp = curlp) != 'L' )
					return(WORD);
				while(*++lp)
					if( *lp<'0' || *lp>'9' )
						return( WORD );
				return( JSW );
			}
		wlength = *lp++;
		if( *lp++==0 && *--np==0 ) {
			if( wlength == 'b' )
				return(optp->opcode + (BYTE<<8));
			if( wlength == 'l' )
				return(optp->opcode + (LONG<<8));
		}
		ophp++;
		if (ophp >= &ophash[OPHS])
			ophp = ophash;
	}
	curlp = line;
	return(0);
}

cclook(s)
char *s;
{
	register char *as = s;
	register struct optab *p;
	register char *os;

	for( p = ccodes; p->opstring; p++ ) {
		as = s;
		os = p->opstring;
		do {
			if( *as != *os++ ) break;
		} while( *as++ );
		if( *--as == 0 ) return( p->opcode );
	}
}

char *
ccstrlook( opc ) {
	register struct optab *p;

	for( p = ccodes; p->opstring; p++ ) {
		if(p->opcode == opc)
			return(p->opstring);
	}
}

refcount()
{
	register struct node *p, *lp;
	static struct node *labhash[LABHS];
	register struct node **hp, *tp;

	for (hp = labhash; hp < &labhash[LABHS];)
		*hp++ = 0;
	for (p = first.forw; p!=0; p = p->forw)
		if (p->op==LABEL) {
			labhash[p->labno % LABHS] = p;
			p->refc = 0;
		}
	for (p = first.forw; p!=0; p = p->forw) {
		if (p->op==JP || p->op==CJP || p->op==JSW) {
			p->ref = 0;
			lp = labhash[p->labno % LABHS];
			if (lp==0 || p->labno!=lp->labno)
			for (lp = first.forw; lp!=0; lp = lp->forw) {
				if (lp->op==LABEL && p->labno==lp->labno)
					break;
			}
			if (lp) {
				tp = nonlab(lp)->back;
				if (tp!=lp) {
					p->labno = tp->labno;
					lp = tp;
				}
				p->ref = lp;
				lp->refc++;
			}
		}
	}
	for (p = first.forw; p!=0; p = p->forw)
		if (p->op==LABEL && p->refc==0
		 && (lp = nonlab(p))->op && lp->op!=JSW)
			decref(p);
}

iterate()
{
	register struct node *p, *rp, *p1;

	nchange = 0;
	for (p = first.forw; p!=0; p = p->forw) {
		if ((p->op==JP||p->op==CJP||p->op==JSW) && p->ref) {
			rp = nonlab(p->ref);
			if (rp->op==JP && rp->labno && p->labno!=rp->labno) {
				nbrbr++;
				p->labno = rp->labno;
				decref(p->ref);
				rp->ref->refc++;
				p->ref = rp->ref;
				nchange++;
			}
		}
		if (p->op==CJP && (p1 = p->forw)->op==JP) {
			rp = p->ref;
			do
				rp = rp->back;
			while (rp->op==LABEL);
			if (rp==p1) {
				decref(p->ref);
				p->ref = p1->ref;
				p->labno = p1->labno;
				p->code = p1->code;
				p1->forw->back = p;
				p->forw = p1->forw;
				p->subop = revbr[p->subop];
				nchange++;
				nskip++;
			}
		}
		if (p->op==JP) {
			while (p->forw && p->forw->op!=LABEL
				&& p->forw->op!=DLABEL
				&& p->forw->op!=EROU && p->forw->op!=END
				&& p->forw->op!=0 && p->forw->op!=DATA) {
				nchange++;
				iaftbr++;
				if (p->forw->ref)
					decref(p->forw->ref);
				p->forw = p->forw->forw;
				p->forw->back = p;
			}
			rp = p->forw;
			while (rp && rp->op==LABEL) {
				if (p->ref == rp) {
					p->back->forw = p->forw;
					p->forw->back = p->back;
					p = p->back;
					decref(rp);
					nchange++;
					njp1++;
					break;
				}
				rp = rp->forw;
			}
		}
		if(p->op == CJP && (rp = p->forw) && rp->op == LABEL) {
			if(p->ref == rp) {
				p->back->forw = p->forw;
				p->forw->back = p->back;
				p = p->back;
				decref(rp);
				nchange++;
				njp1++;
				break;
			}
		}
		if (p->op==JP) {
			xjump(p);
			p = codemove(p);
		}
	}
}

xjump(p1)
register struct node *p1;
{
	register struct node *p2, *p3;

	if ((p2 = p1->ref)==0)
		return;
	for (;;) {
		while ((p1 = p1->back) && p1->op==LABEL);
		while ((p2 = p2->back) && p2->op==LABEL);
		if (!equop(p1, p2) || p1==p2)
			return;
		p3 = insertl(p2);
		p1->op = JP;
		p1->subop = 0;
		p1->ref = p3;
		p1->labno = p3->labno;
		p1->code = 0;
		nxjump++;
		nchange++;
	}
}

struct node *
insertl(oldp)
register struct node *oldp;
{
	register struct node *lp;

	if (oldp->op == LABEL) {
		oldp->refc++;
		return(oldp);
	}
	if (oldp->back->op == LABEL) {
		oldp = oldp->back;
		oldp->refc++;
		return(oldp);
	}
	lp = (struct node *)alloc(sizeof first);
	lp->op = LABEL;
	lp->subop = 0;
	lp->labno = isn++;
	lp->ref = 0;
	lp->code = 0;
	lp->refc = 1;
	lp->back = oldp->back;
	lp->forw = oldp;
	oldp->back->forw = lp;
	oldp->back = lp;
	return(lp);
}

struct node *
codemove(p)
struct node *p;
{
	register struct node *p1, *p2, *p3;
	struct node *t, *tl;
	int n;

	p1 = p;
	if (p1->op!=JP || (p2 = p1->ref)==0)
		return(p1);
	while (p2->op == LABEL)
		if ((p2 = p2->back) == 0)
			return(p1);
	if (p2->op!=JP && p2->op!=JR)
		goto ivloop;
	p2 = p2->forw;
	p3 = p1->ref;
	while (p3) {
		if (p3->op==JP || p3->op==JR) {
			if (p1==p3)
				return(p1);
			ncmot++;
			nchange++;
			p1->back->forw = p2;
			p1->forw->back = p3;
			p2->back->forw = p3->forw;
			p3->forw->back = p2->back;
			p2->back = p1->back;
			p3->forw = p1->forw;
			decref(p1->ref);
			return(p2);
		} else
			p3 = p3->forw;
	}
	return(p1);
ivloop:
	if (p1->forw->op!=LABEL)
		return(p1);
	p3 = p2 = p2->forw;
	n = 16;
	do {
		if ((p3 = p3->forw) == 0 || p3==p1 || --n==0)
			return(p1);
	} while (p3->op!=CJP || p3->labno!=p1->forw->labno);
	do 
		if ((p1 = p1->back) == 0)
			return(p);
	while (p1!=p3);
	p1 = p;
	tl = insertl(p1);
	p3->subop = revbr[p3->subop];
	decref(p3->ref);
	p2->back->forw = p1;
	p3->forw->back = p1;
	p1->back->forw = p2;
	p1->forw->back = p3;
	t = p1->back;
	p1->back = p2->back;
	p2->back = t;
	t = p1->forw;
	p1->forw = p3->forw;
	p3->forw = t;
	p2 = insertl(p1->forw);
	p3->labno = p2->labno;
	p3->ref = p2;
	decref(tl);
	if (tl->refc<=0)
		nrlab--;
	loopiv++;
	nchange++;
	return(p3);
}

comjump()
{
	register struct node *p1, *p2, *p3;

	for (p1 = first.forw; p1!=0; p1 = p1->forw)
		if (p1->op==JP && (p2 = p1->ref) && p2->refc > 1)
			for (p3 = p1->forw; p3!=0; p3 = p3->forw)
				if (p3->op==JP && p3->ref == p2)
					backjmp(p1, p3);
}

backjmp(ap1, ap2)
struct node *ap1, *ap2;
{
	register struct node *p1, *p2, *p3;

	p1 = ap1;
	p2 = ap2;
	for(;;) {
		while ((p1 = p1->back) && p1->op==LABEL);
		p2 = p2->back;
		if (equop(p1, p2)) {
			p3 = insertl(p1);
			p2->back->forw = p2->forw;
			p2->forw->back = p2->back;
			p2 = p2->forw;
			decref(p2->ref);
			p2->labno = p3->labno;
			p2->ref = p3;
			nchange++;
			ncomj++;
		} else
			return;
	}
}

printnodes(){
register struct node *p;

	for( p= &first;p;p=p->forw){
		printanode( p );
	}
}
printanode(p)
register struct node *p;
{
	fprintf(stderr, "node = %x\n",p);
	fprintf(stderr, "op = %d, subop = %d\n",p->op,p->subop);
	fprintf(stderr, "back = %x, reference = %x\n",p->back,p->ref);
	fprintf(stderr,"label no. = %d, code = %s\n",p->labno,p->code);
	fprintf(stderr, "reference count = %d\n",p->refc);
	fprintf(stderr, "instr length = %d\n\n", ilen(p) );
}
