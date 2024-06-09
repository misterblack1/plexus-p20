# include "mfile2"
/* a lot of the machine dependent parts of the second pass */

extern int tcc_ok;
extern int zflag;
extern int minrvar;
#ifdef ONEPASS
extern int enterlab, csavlab;
extern int cretlab;
extern int proflag;
extern int csavallregs;
#endif
int callins;		/* Flag indicating this is a call instruction */

int hibyte, lobyte;
int rpair;
int ldaflag;
# define BITMASK(n) ((1L<<n)-1)

lineid( l, fn ) char *fn; {
	/* identify line l and file fn */
	printf( "//	line %d, file %s\n", l, fn );
	}

eobl2(){
	OFFSZ spoff;	/* offset from stack pointer */
	char regstosave;

	spoff = maxoff;
	spoff /= SZCHAR;
	SETOFF(spoff,2);

#ifndef ONEPASS
	printf( "	.F%d = %ld\n", ftnno, spoff );
#endif
#ifdef ONEPASS
	regstosave = MAXRVAR - minrvar + '0';
	if ((regstosave == '6') || (csavallregs))
		regstosave = ' ';
	deflab(cretlab);
	printf("	jpr	cret%c\n",regstosave);
	deflab(csavlab);
	printf("	ld	r0,$%ld\n", spoff);
	printf("	call	csv%c\n",regstosave);

	if( proflag ) {
		int plab;
		plab = getlab();
		printf("	ld	r0,$L%d\n", plab);
		printf("	call	mcount\n");
		printf("	.bss\nL%d:	.=.+2\n	.text\n", plab);
	}

	branch(enterlab);
#endif

	if( fltused ) {
		fltused = 0;
/*
		printf( "	.globl	fltused\n" );
*/
		}
	}

struct hoptab { int opmask; char * opstring; } ioptab[]= {

	ASG PLUS, "faddd",
	ASG MINUS, "fsubd",
	ASG OR,	"or",
	ASG AND, "and",
	ASG ER,	"xor",
	ASG MUL, "fmuld",
	ASG DIV, "fdivd",
	ASG MOD, "fdivd",
	ASG LS,	"sll",
	ASG RS,	"srl",
	PLUS, "faddd",
	MINUS, "fsubd",
	OR,	"or",
	AND, "and",
	ER,	"xor",
	MUL, "fmuld",
	DIV, "fdivd",
	MOD, "fdivd",
	LS,	"sll",
	RS,	"srl",

	-1, ""    };

hopcode( f, o ){
	/* output the appropriate string from the above table */

	register struct hoptab *q;

	for( q = ioptab;  q->opmask>=0; ++q ){
		if( q->opmask == o ){
			printf( "%s", q->opstring );
			if( f == 'L' ) printf( "l" );
			return;
			}
		}
	cerror( "no hoptab for %s", opst[o] );
	}

char *
rnames[]= {  /* keyed to register number tokens */

	"r0", "r1",
	"r2", "r3",
	"r4", "r5",
	"r6", "r7",
	"r8", "r9", "r10",
	"r11", "r12", "r13",
	"r14", "r15",
	"r4",  "****",
	"r6",  "****",
	"r0",  "****",
	"r2",  "****",
};

char *rhnames[] = {
	"rh0",	"rh1",
	"rh2",	"rh3",
	"rh4",	"rh5",
	"rh6",	"rh7"
};

char *rlnames[] = {
	"rl0",	"rl1",
	"rl2",	"rl3",
	"rl4",	"rl5",
	"rl6",	"rl7"
};

char *rrnames[] = {
	"rr0",	"****",
	"rr2",	"****",
	"rr4",	"****",
	"rr6",	"****",
	"rr8",	"****",
	"rr10",	"****",
	"rr12",	"****",
	"rr14",	"****",
	"rr4",	"****",
	"rr6",	"****",
	"rr0",	"****",
	"rr2",	"****",
};

char *rqnames[] = {
	"rq0",	"****",
	"****",	"****",
	"rq4",	"****",
	"****",	"****",
	"rq8",	"****",
	"****",	"****",
	"rq12",	"****",
	"****",	"****",
	"rq4",	"****",
	"****",	"****",
	"rq0",	"****",
	"****",	"****",
};

int rstatus[] = {
	SAREG|STAREG, SAREG|STAREG,
	SAREG|STAREG, SAREG|STAREG,
	SAREG|STAREG, SAREG|STAREG,
	SAREG|STAREG, SAREG|STAREG,
	SAREG, SAREG, SAREG,
	SAREG, SAREG, SAREG,
	SAREG, SAREG,

	};

NODE *brnode;
int brcase;

int toff = 0; /* number of stack locations used for args */

zzzzcode( p, c ) register NODE *p; {
	register m;
	switch( c ){

	case 'A':
		ldaflag++;
		return;

	case 'P':
		m = p->left->rval;
		if (m%2 == 0)
			printf("	ld	%s,%s\n",rnames[m+1],rnames[m]);
		return;

	case 'H':
		hibyte++;
		return;

	case 'L':
		lobyte++;
		return;

	case '$':
		callins++;
		return;

	case ')':
		callins = 0;
		hibyte = lobyte = rpair = 0;
		ldaflag = 0;
		return;
	
	case 'S':
		if( (m = ispow2( p->right->lval )) < 0 ) {
			cerror("Illegal #S lval");
			return;
		}
		printf("$%d",m);
		return;

	case 'R':
		if( (m = ispow2( (~p->right->lval )&0xffff) ) < 0 ) {
			cerror("Illegal #R lval");
			return;
		}
		printf("$%d",m);
		return;

	};
	cerror( "Illegal # code(%d,%c)\n",c,c);
}

int tccop;
extern char * cctests[];
zzzcode( p, c ) NODE *p; {
	register m, n, flag;
	switch( c ){

	case 'B':	/* output b if type is byte */
			/* output l if type is long */
		if( p->type == CHAR || p->type == UCHAR ) {
			printf( "b" );
			lobyte++;
		} else if( p->type == LONG || p->type == ULONG ) {
			printf("l");
			rpair++;
		}
		return;

	case 'N':  /* logical ops, turned into 0-1 */
		/* use register given by register 1 */
		n = (int)rnames[getlr( p, '1' )->rval];
		if( flag = tcc_ok != 1 ) { /* load a 1, skip a 0 */
			printf( "	ldk	%s,$1\n", n );
			cbgen( 0, m = getlab(), 'I' );
		}
		deflab( p->label );
		printf( "\tldk	%s,$0\n", n );
		if( !flag ) {
			printf( cctests[tccop - EQ], n );
		}
		if( p->type == LONG || p->type == ULONG )
			printf( "	ldk	%s,$0\n",
				rnames[getlr( p, '1' )->rval+ 1] );
		if( flag ) {
			deflab( m );
		}
		return;

	case 'I':
	case 'F':
		cbgen( p->op, p->label, c );
		return;

	case 'A':
	case 'C':
		/* logical operators for longs
		   defer comparisons until branch occurs */

		brnode = tcopy( p );
		brcase = c;
		return;

	case 'V':
		/* sign extend or not -- assumes U1 or E1 has value in it */

		if( ISUNSIGNED(p->type) ){
			if( p->left->type == UCHAR || p->left->type == CHAR )
				expand( p, FOREFF, "#H	ldb	U1,$0#)\n" );
			expand( p, FOREFF, "	ldk	A1,$0\n");
			if( p->left->type == ULONG || p->left->type == LONG )
				expand( p, FOREFF, "	ldk	U1,$0\n");
			}
		else {
			if( p->left->type == UCHAR || p->left->type == CHAR )
				expand( p, FOREFF, "	extsb	U1\n" );
			if( p->left->type == ULONG || p->left->type == LONG )
				expand( p, FOREFF, "	extsl	Q1\n");
			else
				expand( p, FOREFF, "	exts	D1\n");
			}
		return;

		/* stack management macros */
	case '2':
		if( zflag ) goto dopush;
		if( toff++ ) printf( "push" );
		else printf( "ld" );
		return;
	dopush:
		printf( "push" ); /* drop into 'Z-' case */
	case '-':
		if( toff == 0 ) toff++; /* can't "ld" so will push */
		toff++;
		return;

	case 'L':  /* INIT for long constants */
		{
			printf( "%ld\n", p->left->lval );
			return;
		}

	case 'T':
		/* Truncate longs for type conversions:
		    LONG|ULONG -> CHAR|UCHAR|INT|UNSIGNED
		   increment offset to second word */

		m = p->type;
		p = p->left;
		switch( p->op ){
		case NAME:
		case OREG:
			if( m== CHAR || m == UCHAR )
				p->lval += 1; /* char byte */
			if ( p->type == LONG || p->type == ULONG )
				p->lval += SZINT/SZCHAR;
			return;
		case REG:
			if( p->type != LONG && p->type != ULONG ) return;
			rfree( p->rval, p->type );
			p->rval += 1;
			p->type = m;
			rbusy( p->rval, p->type );
			return;
		default:
			cerror( "Illegal ZT type conversion" );
			return;

			}

	case 'U':
		/* same as AL for exp under U* */
		if( p->left->op == UNARY MUL ) {
			adrput( getlr( p->left, 'L' ) );
			return;
			}
		cerror( "Illegal ZU" );
		/* NO RETURN */

	case 'W':	/* structure size */
		if( p->op == STASG )
			printf( "%d", p->stsize);
		else	cerror( "Not a structure" );
		return;

	case 'X':	/* structure size / 2 */
		printf("%d", p->stsize/2);
		return;

	case 'S':	/* structure oreg */
		{
		
		register NODE *l, *r;

		l = p->left;
		r = p->right;

		l->op = REG;

		expand(l, FOREFF, "AR");

		l->op = OREG;
		}
		break;

	default:
		cerror( "illegal zzzcode" );
		}
	}

rmove( rt, rs, t ) TWORD t; {
	if (t==FLOAT||t==DOUBLE) {
		printf( "	ldl	%s,%s\n", rrnames[rt], rrnames[rs] );
		printf( "	ldl	%s,%s\n", rrnames[rt+2], rrnames[rs+2] );
	}
	else if (t==LONG||t==ULONG) 
		printf("	ldl	%s,%s\n", rrnames[rt], rrnames[rs] );
	else
		printf("	ld	%s,%s\n", rnames[rt], rnames[rs] );
	}

struct respref
respref[] = {
	INTAREG|INTBREG,	INAREG|INBREG|INTAREG|INTBREG,
	INAREG|INBREG,	INAREG|INBREG|SOREG|STARREG|SNAME|SCON,
	INTEMP,	INTEMP,
	FORARG,	FORARG,
	INTAREG,	SOREG|SNAME,
	0,	0 };

setregs(){ /* set up temporary registers */
	register i;

	fregs = MINRVAR;
	if( xdebug ){
		/* -x changes number of free regs to 2, -xx to 3, etc. */
		if( (xdebug+1) < fregs ) fregs = xdebug+1;
		}
	for (i=MINRVAR; i<=MAXRVAR; i++)
		rstatus[i] = i<fregs ? SAREG|STAREG : SAREG;
	}

szty(t) TWORD t; { /* size, in words, needed to hold thing of type t */
	/* really is the number of registers to hold type t */
	switch( t ) {

	case DOUBLE:
	case FLOAT:
		return(SZDOUBLE/SZINT);

	case LONG:
	case ULONG:
		return( SZLONG/SZINT );

	default:
		return(1);

		}
	}

rewfld( p ) NODE *p; {
	return(1);
	}

callreg(p) NODE *p; {
	switch (p->type) {
	case DOUBLE:
	case FLOAT:
		return(R4);
	case LONG:
	case ULONG:
		return(R6);
	default:
		return(R7);
		}
	}

shltype( o, p ) NODE *p; {
	if( o == NAME|| o==REG || o == ICON || o == OREG ) return( 1 );
	return( 0 );
	}

flshape( p ) register NODE *p; {
	register o = p->op;
	if( o==NAME || o==REG || o==ICON || o==OREG ) return( 1 );
	return( 0 );
	}

shtemp( p ) register NODE *p; {
	if( p->op == UNARY MUL ) p = p->left;
	if( p->op == REG || p->op == OREG ) return( !istreg( p->rval ) );
	return( p->op == NAME || p->op == ICON );
	}

spsz( t, v ) TWORD t; CONSZ v; {

	/* is v the size to increment something of type t */

	if( !ISPTR(t) ) return( 0 );
	t = DECREF(t);

	if( ISPTR(t) ) return( v == 2 );

	switch( t ){

	case UCHAR:
	case CHAR:
		return( v == 1 );

	case INT:
	case UNSIGNED:
		return( v == 2 );

	case FLOAT:
		return( v == 4 );

	case DOUBLE:
		return( v == 8 );
		}

	return( 0 );
	}

shumul( p ) register NODE *p; {

	return( 0 );
	}

adrcon( val ) CONSZ val; {
	printf( CONFMT, val );
	}

conput( p ) register NODE *p; {
	switch( p->op ){

	case ICON:
		acon( p );
		return;

	case REG:
		if (hibyte)
			printf( "%s", rhnames[p->rval] );
		else if (lobyte)
			printf( "%s", rlnames[p->rval] );
		else
			printf( "%s", rnames[p->rval] );
		return;

	default:
		cerror( "illegal conput" );
		}
	}

insput( p ) NODE *p; {
	cerror( "insput" );
	}

upput( p ) NODE *p; {
	/* output the address of the second word in the
	   pair pointed to by p (for LONGs)*/
	CONSZ save;

	if( p->op == FLD ){
		p = p->left;
		}

	save = p->lval;
	switch( p->op ){

	case NAME:
		p->lval += SZINT/SZCHAR;
		acon( p );
		break;

	case ICON:
		/* addressable value of the constant */
		p->lval &= BITMASK(SZINT);
		printf( "$" );
		acon( p );
		break;

	case REG:
		if (hibyte)
			printf( "%s", rhnames[p->rval+1] );
		else if (lobyte)
			printf( "%s", rlnames[p->rval+1] );
		else
			printf( "%s", rnames[p->rval+1] );
		break;

	case OREG:
		p->lval += SZINT/SZCHAR;
		if( p->rval == R14 ){  /* in the argument region */
			if( p->name[0] != '\0' ) werror( "bad arg temp" );
			}
		if( p->lval != 0 || p->name[0] != '\0' ) {
			acon( p );
			printf( "(%s)", rnames[p->rval] );
		}
		else	printf( "*%s", rnames[p->rval] );
		break;

	default:
		cerror( "illegal upper address" );
		break;

		}
	p->lval = save;

	}

adrput( p ) register NODE *p; {
	/* output an address, with offsets, from p */

	if( p->op == FLD ){
		p = p->left;
		}
	switch( p->op ){

	case NAME:
		acon( p );
		return;

	case ICON:
		/* addressable value of the constant */
		if( szty( p->type ) == 2 && !rpair ) {
			/* print the high order value */
			CONSZ save;
			save = p->lval;
			p->lval = ( p->lval >> SZINT ) & BITMASK(SZINT);
			if (!callins)
				printf( "$" );
			acon( p );
			p->lval = save;
			return;
			}
		if (!callins)
			printf( "$" );
		acon( p );
		return;

	case REG:
		if (hibyte)
			printf( "%s", rhnames[p->rval] );
		else if (lobyte)
			printf( "%s", rlnames[p->rval] );
		else if( rpair )
			printf( "%s", rrnames[p->rval] );
		else
			printf( "%s", rnames[p->rval] );
		return;

	case OREG:
		if( p->rval == R14 ){  /* in the argument region */
			if( p->name[0] != '\0' ) werror( "bad arg temp" );
			printf( CONFMT, p->lval );
			printf( "(r14)" );
			return;
			}
		if( ldaflag || p->lval != 0 || p->name[0] != '\0' ) {
			acon( p );
			printf( "(%s)", rnames[p->rval] );
		}
		else	printf( "*%s", rnames[p->rval] );
		return;

	case UNARY MUL:
		/* STARREG found */
		printf( "*" );
		adrput( p->left );
		return;

	default:
		cerror( "illegal address" );
		return;

		}

	}

aput( p, size, offset )
register NODE *p;
register int size;
register int offset;
{
	/* output an address, with offsets, from p */

	if( p->op == FLD ){
		p = p->left;
		}
	switch( p->op ){

	case NAME:
		p->lval += offset;
		acon( p );
		p->lval -= offset;
		return;

	case ICON:
		/* addressable value of the constant */
		if (!callins)
			printf( "$" );
		acon( p );
		return;

	case REG:
		offset = p->rval + offset/2;
		if (size == 4)
			printf( "%s", rrnames[offset]);
		else if (size == 8)
			printf( "%s", rqnames[offset]);
		else if (hibyte)
			printf( "%s", rhnames[offset] );
		else if (lobyte)
			printf( "%s", rlnames[offset] );
		else
			printf( "%s", rnames[offset] );
		return;

	case OREG:
		if( p->rval == R14 ){  /* in the argument region */
			if( p->name[0] != '\0' ) werror( "bad arg temp" );
			printf( CONFMT, p->lval+offset );
			printf( "(r14)" );
			return;
			}
		if( ldaflag || (p->lval + offset) != 0 || p->name[0] != '\0' ) {
			p->lval += offset;
			acon( p );
			p->lval -= offset;
			printf( "(%s)", rnames[p->rval] );
		}
		else	printf( "*%s", rnames[p->rval] );
		return;

	case UNARY MUL:
		/* STARREG found */
		if (offset != 0) {
			printf( CONFMT, offset);
			printf( "(" );
			aput( p->left, size, 0);
			printf( ")" );
		}
		else {
			printf( "*" );
			adrput( p->left );
		}
		return;

	default:
		cerror( "illegal address" );
		return;

		}

	}

acon( p ) register NODE *p; { /* print out a constant */

	if( p->name[0] == '\0' ){	/* constant only */
		printf( CONFMT, p->lval);
		}
	else if( p->lval == 0 ) {	/* name only */
		printf( "%.8s", p->name );
		}
	else {				/* name + offset */
		printf( "%.8s+", p->name );
		printf( CONFMT, p->lval );
		}
	}

genscall( p, cookie ) register NODE *p; {
	/* structure valued call */
	return( gencall( p, cookie ) );
	}

gencall( p, cookie ) register NODE *p; {
	/* generate the call given by p */
	register temp;
	register m;

	if( p->right ) temp = argsize( p->right );
	else temp = 0;

	if( p->right ){ /* generate args */
		genargs( p->right );
		}

	if( !shltype( p->left->op, p->left ) ) {
		order( p->left, INAREG|SOREG );
		}

	p->op = UNARY CALL;
	m = match( p, INTAREG|INTBREG );
	popargs( temp );
	return(m != MDONE);
	}

popargs( size ) register size; {
	/* pop arguments from stack */

	toff -= size/2;
	if( toff == 0 && size >= 2 ) size -= 2;
	if( size == 0)
		return;
	else if (size <= 16)
		printf("	inc	r15,$%d\n", size);
	else
		printf("	add	r15,$%d\n", size);
	}

char *
ccbranches[] = {
	"	jpr	eq,L%d\n",
	"	jpr	ne,L%d\n",
	"	jpr	le,L%d\n",
	"	jpr	lt,L%d\n",
	"	jpr	ge,L%d\n",
	"	jpr	gt,L%d\n",
	"	jpr	ule,L%d\n",
	"	jpr	ult,L%d\n",
	"	jpr	uge,L%d\n",
	"	jpr	ugt,L%d\n",
	};

char *
cctests[] = {
	"	tcc	ne,%s\n",
	"	tcc	eq,%s\n",
	"	tcc	gt,%s\n",
	"	tcc	ge,%s\n",
	"	tcc	lt,%s\n",
	"	tcc	le,%s\n",
	"	tcc	ugt,%s\n",
	"	tcc	uge,%s\n",
	"	tcc	ult,%s\n",
	"	tcc	ule,%s\n",
	};

/*	long branch table

   This table, when indexed by a logical operator,
   selects a set of three logical conditions required
   to generate long comparisons and branches.  A zero
   entry indicates that no branch is required.
   E.G.:  The <= operator would generate:
	cp	AL,AR
	jpr	lt,lable	/ 1st entry LT -> lable
	jpr	gt,1f	/ 2nd entry GT -> 1f
	cp	UL,UR
	jpr	ule,lable	/ 3rd entry ULE -> lable
   1:
 */

int lbranches[][3] = {
	/*EQ*/	0,	NE,	EQ,
	/*NE*/	NE,	0,	NE,
	/*LE*/	LT,	GT,	ULE,
	/*LT*/	LT,	GT,	ULT,
	/*GE*/	GT,	LT,	UGE,
	/*GT*/	GT,	LT,	UGT,
	/*ULE*/	ULT,	UGT,	ULE,
	/*ULT*/	ULT,	UGT,	ULT,
	/*UGE*/	UGT,	ULT,	UGE,
	/*UGT*/	UGT,	ULT,	UGT,
	};

/* logical relations when compared in reverse order (cmp R,L) */
extern short revrel[] ;

cbgen( o, lab, mode ) { /*   printf conditional and unconditional branches */
	register *plb;
	int lab1f;

	tcc_ok++;
	if( o == 0 ) printf( "	jpr	L%d\n", lab );
	else	if( o > UGT ) cerror( "bad conditional branch: %s", opst[o] );
	else {
		switch( brcase ) {

		case 'A':
		case 'C':
			plb = lbranches[ o-EQ ];
			lab1f = getlab();
			expand( brnode, FORCC, brcase=='C' ? "\tcp\tAL,AR\n" : "\ttest\tAR\n" );
			if( *plb != 0 )
				printf( ccbranches[*plb-EQ], lab);
			if( *++plb != 0 )
				printf( ccbranches[*plb-EQ], lab1f);
			expand( brnode, FORCC, brcase=='C' ? "\tcp\tUL,UR\n" : "\ttest\tUR\n" );
			printf( ccbranches[*++plb-EQ], lab);
			deflab( lab1f );
			reclaim( brnode, RNULL, 0 );
			break;

		default:
			if( mode=='F' ) o = revrel[ o-EQ ];
			printf( ccbranches[o-EQ], lab );
			tccop = o;
			break;
			}

		brcase = 0;
		brnode = 0;
		}
	}

nextcook( p, cookie ) NODE *p; {
	/* we have failed to match p with cookie; try another */
	if( cookie == FORREW ) return( 0 );  /* hopeless! */
	if( !(cookie&(INTAREG|INTBREG)) ) return( INTAREG|INTBREG );
	if( !(cookie&INTEMP) && asgop(p->op) ) return( INTEMP|INAREG|INTAREG|INTBREG|INBREG );
	return( FORREW );
	}

lastchance( p, cook ) NODE *p; {
	/* forget it! */
	return(0);
	}

struct functbl {
	int fop;
	TWORD ftype;
	char *func;
	} opfunc[] = {
	MUL,		LONG,	"lmul",
	DIV,		LONG,	"ldiv",
	MOD,		LONG,	"lrem",
	ASG MUL,	LONG,	"almul",
	ASG DIV,	LONG,	"aldiv",
	ASG MOD,	LONG,	"alrem",
	MUL,		ULONG,	"lmul",
	DIV,		ULONG,	"uldiv",
	MOD,		ULONG,	"ulrem",
	ASG MUL,	ULONG,	"almul",
	ASG DIV,	ULONG,	"auldiv",
	ASG MOD,	ULONG,	"aulrem",
	0,	0,	0 };

hardops(p)  register NODE *p; {
	/* change hard to do operators into function calls.
	   for pdp11 do long * / %	*/
	register NODE *q;
	register struct functbl *f;
	register o;
	register TWORD t;

	o = p->op;
	t = p->type;
	if( t!=LONG && t!=ULONG ) return;

	for( f=opfunc; f->fop; f++ ) {
		if( o==f->fop && t==f->ftype ) goto convert;
		}
	return;

	/* need address of left node for ASG OP */
	/* WARNING - this won't work for long in a REG */
	convert:
	if( asgop( o ) ) {
		switch( p->left->op ) {

		case UNARY MUL:	/* convert to address */
			p->left->op = FREE;
			p->left = p->left->left;
			break;

		case NAME:	/* convert to ICON pointer */
			p->left->op = ICON;
			p->left->type = INCREF( p->left->type );
			break;

		case OREG:	/* convert OREG to address */
			p->left->op = REG;
			p->left->type = INCREF( p->left->type );
			if( p->left->lval != 0 ) {
				q = talloc();
				q->op = PLUS;
				q->rall = NOPREF;
				q->type = p->left->type;
				q->left = p->left;
				q->right = talloc();

				q->right->op = ICON;
				q->right->rall = NOPREF;
				q->right->type = INT;
				q->right->name[0] = '\0';
				q->right->lval = p->left->lval;
				q->right->rval = 0;

				p->left->lval = 0;
				p->left = q;
				}
			break;

		default:
			cerror( "Bad address for hard ops" );
			/* NO RETURN */

			}
		}

	/* build comma op for args to function */
	q = talloc();
	q->op = CM;
	q->rall = NOPREF;
	q->type = INT;
	q->left = p->left;
	q->right = p->right;
	p->op = CALL;
	p->right = q;

	/* put function name in left node of call */
	p->left = q = talloc();
	q->op = ICON;
	q->rall = NOPREF;
	q->type = INCREF( FTN + p->type );
	strcpy( q->name, f->func );
	q->lval = 0;
	q->rval = 0;

	return;

	}

optim2( p ) register NODE *p; {
	/* do local tree transformations and optimizations */

	register NODE *r;

#ifdef PDP11
	switch( p->op ) {

	case AND:
		/* commute L and R to eliminate compliments and constants */
		if( p->left->op==ICON || p->left->op==COMPL ) {
			r = p->left;
			p->left = p->right;
			p->right = r;
			}
	case ASG AND:
		/* change meaning of AND to ~R&L - bic on pdp11 */
		r = p->right;
		if( r->op==ICON ) { /* compliment constant */
			r->lval = ~r->lval;
			}
		else if( r->op==COMPL ) { /* ~~A => A */
			r->op = FREE;
			p->right = r->left;
			}
		else { /* insert complement node */
			p->right = talloc();
			p->right->op = COMPL;
			p->right->rall = NOPREF;
			p->right->type = r->type;
			p->right->left = r;
			p->right->right = NULL;
			}
		break;

		}
#endif
	}

myreader(p)
register NODE *p;
{
	canon( p );		/* expands r-vals for fileds */
	toff = 0;	/* stack offset swindle */
}

special( p, shape ) register NODE *p; {
	/* special shape matching routine */

	switch( shape ) {

	case SSCON:
		if( p->op == ICON && p->name[0]=='\0' &&
		    p->lval>=1 && p->lval <= 16) return( 1 );
		break;

	case SCCON:
		if( p->op == ICON && p->name[0]=='\0' &&
		    p->lval>= -128 && p->lval <= 127 ) return( 1 );
		break;

	case SICON:
		if( p->op == ICON && p->name[0]=='\0' &&
		    p->lval>= 0 && p->lval <=32767 ) return( 1 );
		break;

	case SUCCON:
		if( p->op == ICON && p->name[0]=='\0'
		    && p->lval >= 0 && p->lval <=255 ) return( 1 );
		break;

	case SCONSET:
		if( p->op == ICON && p->name[0]=='\0'
		    && p->lval >= -32768 && p->lval <= 0xffff
		    && apow2(p->lval) >= 0 )  return( 1 );
		break;

	case SCONRES:
		if( p->op == ICON && p->name[0]=='\0'
		    && p->lval >= -32768 && p->lval <= 0xffff
		    && apow2( (~ p->lval)&0xffff ) >= 0 )  return( 1 );
		break;

	default:
		cerror( "bad special shape" );

		}

	return( 0 );
	}

apow2( c )
CONSZ c;
{
	if( c <= 0 || (c&(c-1)) )
		return( -1 );
	else
		return( 0 );
}

# ifndef ONEPASS
main( argc, argv ) char *argv[]; {
	return( mainp2( argc, argv ) );
	}
# endif
/* CCF two pass code inserted from here to end */
#ifndef ONEPASS
/* CCF moved from code.c */
dexit( v ) {
/* CCF for now
	unlink(tmpname);
*/
	exit(1);
	}
/* CCF moved from code */
#ifndef ONEPASS
where(c){ /* print location of error  */
	/* c is either 'u', 'c', or 'w' */
	fprintf( stderr, "%s, line %d: ", ftitle, lineno );
	}
#endif

short revrel[] ={ EQ, NE, GE, GT, LE, LT, UGE, UGT, ULE, ULT };
#endif

extern int relwalk();
mycanon(p) {
	/* for now make sure registers are on the left side of relations */

	walkf( p, relwalk );
}

relwalk( p ) register NODE *p; {
	register o;
	register NODE *temp;

	if( logop( o = p->op ) && p->right->op == REG 
	    && o != NOT && o != ANDAND && o != OROR ) {
		temp = p->left;
		p->left = p->right;
		p->right = temp;
		p->op = revrel[o-EQ];
	}
}
