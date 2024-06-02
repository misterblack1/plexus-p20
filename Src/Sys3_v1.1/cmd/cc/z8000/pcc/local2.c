/*	Plexus - Sys3 - August 1982	*/

static char c_2local[] = "@(#)local2.c	1.1";

# include "mfile2"
/* a lot of the machine dependent parts of the second pass */

extern int tcc_ok;
extern int zflag;
#ifdef ONEPASS
extern int enterlab, csavlab;
extern int proflag;
extern int minrvar, cretlab, csavallregs;
#endif
int callins;		/* Flag indicating this is a call instruction */

int hibyte, lobyte;
int rpair, flpair;
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
	if( (regstosave == '6') || csavallregs )
		regstosave = ' ';
	deflab(cretlab);
	printf( "	jpr	cret%c\n", regstosave);
	deflab(csavlab);
	printf("	ld	r0,$%ld\n", spoff);
	printf("	call	csv%c\n", regstosave);

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
		printf( "	.globl	fltused\n" );
		}
	else printf( "	.globl\n" );	/* help the optimizer in some cases */
	}

struct hoptab { int opmask; char * opstring; } ioptab[]= {

	ASG PLUS, "fadd",
	ASG MINUS, "fsub",
	ASG OR,	"or",
	ASG AND, "and",
	ASG ER,	"xor",
	ASG MUL, "fmul",
	ASG DIV, "fdiv",
	ASG MOD, "fdiv",
	ASG LS,	"sll",
	ASG RS,	"srl",
	PLUS, "fadd",
	MINUS, "fsub",
	OR,	"or",
	AND, "and",
	ER,	"xor",
	MUL, "fmul",
	DIV, "fdiv",
	MOD, "fdiv",
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
static long jlab;

zzzzcode( p, c ) register NODE *p; {
	register m;
	switch( c ){

	case 'A':
		ldaflag++;
		return;

	case 'P':
		m = p->in.left->tn.rval;
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
		hibyte = lobyte = rpair = flpair = 0;
		ldaflag = 0;
		return;
	
	case 'S':
		if( (m = ispow2( p->in.right->tn.lval )) < 0 ) {
			cerror("Illegal #S lval");
			return;
		}
		printf("$%d",m);
		return;

	case 'R':
		if( (m = ispow2( (~p->in.right->tn.lval )&0xffff) ) < 0 ) {
			cerror("Illegal #R lval");
			return;
		}
		printf("$%d",m);
		return;

	case 'd':
		if( p->in.left->in.type == FLOAT || p->in.right->in.type == FLOAT
		   || p->in.type == FLOAT )
			flpair++;
		else
			printf( "d" );
		return;

	case 'D': /* for SCONVs */
		if( p->in.left->in.type == FLOAT || p->in.type == FLOAT ) {
			printf( "f" );
			flpair++;
		} else
			printf("d");
		return;
	
	case 'f':
		flpair++;
		return;

	case 'j':	/* get and print a label */
		jlab = getlab();
		printf("L%ld",jlab);
		return;

	case 'l':	/* target of the above label */
		printf("L%ld:",jlab);
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
		if( p->in.type == CHAR || p->in.type == UCHAR ) {
			printf( "b" );
			lobyte++;
		} else if( p->in.type == LONG || p->in.type == ULONG ) {
			printf("l");
			rpair++;
		}
		return;

	case 'N':  /* logical ops, turned into 0-1 */
		/* use register given by register 1 */
		n = (int)rnames[getlr( p, '1' )->tn.rval];
		if( flag = tcc_ok != 1 ) { /* load a 1, skip a 0 */
			printf( "	ldk	%s,$1\n", n );
			cbgen( 0, m = getlab(), 'I' );
		}
		deflab( p->bn.label );
		printf( "\tldk	%s,$0\n", n );
		if( !flag ) {
			printf( cctests[tccop - EQ], n );
		}
		if( p->in.type == LONG || p->in.type == ULONG )
			printf( "	ldk	%s,$0\n",
				rnames[getlr( p, '1' )->tn.rval+ 1] );
		if( flag ) {
			deflab( m );
		}
		return;

	case 'I':
	case 'F':
		cbgen( p->in.op, p->bn.label, c );
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

		if( ISUNSIGNED(p->in.type) ){
			if( p->in.left->in.type == UCHAR || p->in.left->in.type == CHAR )
				expand( p, FOREFF, "#H	ldb	U1,$0#)\n" );
			expand( p, FOREFF, "	ldk	A1,$0\n");
			if( p->in.left->in.type == ULONG || p->in.left->in.type == LONG )
				expand( p, FOREFF, "	ldk	U1,$0\n");
			}
		else {
			if( p->in.left->in.type == UCHAR || p->in.left->in.type == CHAR )
				expand( p, FOREFF, "	extsb	U1\n" );
			if( p->in.left->in.type == ULONG || p->in.left->in.type == LONG )
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
			printf( "%ld\n", p->in.left->tn.lval );
			return;
		}

	case 'T':
		/* Truncate longs for type conversions:
		    LONG|ULONG -> CHAR|UCHAR|INT|UNSIGNED
		   increment offset to second word */

		m = p->in.type;
		p = p->in.left;
		switch( p->in.op ){
		case NAME:
		case OREG:
			if( m== CHAR || m == UCHAR )
				p->tn.lval += 1; /* char byte */
			if ( p->in.type == LONG || p->in.type == ULONG )
				p->tn.lval += SZINT/SZCHAR;
			return;
		case REG:
			if( p->in.type != LONG && p->in.type != ULONG ) return;
			rfree( p->tn.rval, p->in.type );
			p->tn.rval += 1;
			p->in.type = m;
			rbusy( p->tn.rval, p->in.type );
			return;
		default:
			cerror( "Illegal ZT type conversion" );
			return;

			}

	case 'U':
		/* same as AL for exp under U* */
		if( p->in.left->in.op == UNARY MUL ) {
			adrput( getlr( p->in.left, 'L' ) );
			return;
			}
		cerror( "Illegal ZU" );
		/* NO RETURN */

	case 'W':	/* structure size */
		if( p->in.op == STASG || p->in.op == STARG )
			printf( "%d", p->stn.stsize);
		else	cerror( "Not a structure" );
		return;

	case 'X':	/* structure size / 2 */
		printf("%d", p->stn.stsize/2);
		return;

	case 'Y':	/* adjust SP if can't use magic cell */
		if( toff != 0 )
			printf("	dec	r15,$2\n");
		toff += (p->stn.stsize/2);
		return;

	case 'S':	/* structure oreg */
		{
		
		register NODE *l, *r;

		l = p->in.left;
		r = p->in.right;

		l->in.op = REG;

		expand(l, FOREFF, "AR");

		l->in.op = OREG;
		}
		break;

	default:
		cerror( "illegal zzzcode" );
		}
	}

rmove( rt, rs, t ) TWORD t; {
	switch( t ) {
	case DOUBLE:
		printf( "\tldl	%s,%s\n", rrnames[rt+2], rrnames[rs+2] );
	case FLOAT:
	case LONG:
	case ULONG:
		printf( "\tldl	%s,%s\n", rrnames[rt], rrnames[rs] );
		break;
	default:
		printf("\tld	%s,%s\n", rnames[rt], rnames[rs] );
	}
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
	switch (p->in.type) {
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
	register o = p->in.op;
	if( o==NAME || o==REG || o==ICON || o==OREG ) return( 1 );
	return( 0 );
	}

shtemp( p ) register NODE *p; {
	if( p->in.op == UNARY MUL ) p = p->in.left;
	if( p->in.op == REG || p->in.op == OREG ) return( !istreg( p->tn.rval ) );
	return( p->in.op == NAME || p->in.op == ICON );
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
	switch( p->in.op ){

	case ICON:
		acon( p );
		return;

	case REG:
		if (hibyte)
			printf( "%s", rhnames[p->tn.rval] );
		else if (lobyte)
			printf( "%s", rlnames[p->tn.rval] );
		else
			printf( "%s", rnames[p->tn.rval] );
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

	if( p->in.op == FLD ){
		p = p->in.left;
		}

	save = p->tn.lval;
	switch( p->in.op ){

	case NAME:
		p->tn.lval += SZINT/SZCHAR;
		acon( p );
		break;

	case ICON:
		/* addressable value of the constant */
		p->tn.lval &= BITMASK(SZINT);
		printf( "$" );
		acon( p );
		break;

	case REG:
		if (hibyte)
			printf( "%s", rhnames[p->tn.rval+1] );
		else if (lobyte)
			printf( "%s", rlnames[p->tn.rval+1] );
		else
			printf( "%s", rnames[p->tn.rval+1] );
		break;

	case OREG:
		p->tn.lval += SZINT/SZCHAR;
		if( p->tn.rval == R14 ){  /* in the argument region */
			if( p->in.name[0] != '\0' ) werror( "bad arg temp" );
			}
		if( p->tn.lval != 0 || p->in.name[0] != '\0' ) {
			acon( p );
			printf( "(%s)", rnames[p->tn.rval] );
		}
		else	printf( "*%s", rnames[p->tn.rval] );
		break;

	default:
		cerror( "illegal upper address" );
		break;

		}
	p->tn.lval = save;

	}

adrput( p ) register NODE *p; {
	/* output an address, with offsets, from p */

	if( p->in.op == FLD ){
		p = p->in.left;
		}
	switch( p->in.op ){

	case NAME:
		acon( p );
		return;

	case ICON:
		/* addressable value of the constant */
		if( szty( p->in.type ) == 2 && !rpair ) {
			/* print the high order value */
			CONSZ save;
			save = p->tn.lval;
			p->tn.lval = ( p->tn.lval >> SZINT ) & BITMASK(SZINT);
			if (!callins)
				printf( "$" );
			acon( p );
			p->tn.lval = save;
			return;
			}
		if (!callins)
			printf( "$" );
		acon( p );
		return;

	case REG:
		if (hibyte)
			printf( "%s", rhnames[p->tn.rval] );
		else if (lobyte)
			printf( "%s", rlnames[p->tn.rval] );
		else if( rpair )
			printf( "%s", rrnames[p->tn.rval] );
		else
			printf( "%s", rnames[p->tn.rval] );
		return;

	case OREG:
		if( p->tn.rval == R14 ){  /* in the argument region */
			if( p->in.name[0] != '\0' ) werror( "bad arg temp" );
			printf( CONFMT, p->tn.lval );
			printf( "(r14)" );
			return;
			}
		if( ldaflag || p->tn.lval != 0 || p->in.name[0] != '\0' ) {
			acon( p );
			printf( "(%s)", rnames[p->tn.rval] );
		}
		else	printf( "*%s", rnames[p->tn.rval] );
		return;

	case UNARY MUL:
		/* STARREG found */
		printf( "*" );
		adrput( p->in.left );
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

	if( p->in.op == FLD ){
		p = p->in.left;
		}
	switch( p->in.op ){

	case NAME:
		p->tn.lval += offset;
		acon( p );
		p->tn.lval -= offset;
		return;

	case ICON:
		/* addressable value of the constant */
		if (!callins)
			printf( "$" );
		acon( p );
		return;

	case REG:
		offset = p->tn.rval + offset/2;
		if (size == 4)
			printf( "%s", rrnames[offset]);
		else if (size == 8) {
			if( flpair )
				printf( "%s", rrnames[offset+2] );
			else
				printf( "%s", rqnames[offset]);
		} else if (hibyte)
			printf( "%s", rhnames[offset] );
		else if (lobyte)
			printf( "%s", rlnames[offset] );
		else
			printf( "%s", rnames[offset] );
		return;

	case OREG:
		if( p->tn.rval == R14 ){  /* in the argument region */
			if( p->in.name[0] != '\0' ) werror( "bad arg temp" );
			printf( CONFMT, p->tn.lval+offset );
			printf( "(r14)" );
			return;
			}
		if( ldaflag || (p->tn.lval + offset) != 0 || p->in.name[0] != '\0' ) {
			p->tn.lval += offset;
			acon( p );
			p->tn.lval -= offset;
			printf( "(%s)", rnames[p->tn.rval] );
		}
		else	printf( "*%s", rnames[p->tn.rval] );
		return;

	case UNARY MUL:
		/* STARREG found */
		if (offset != 0) {
			printf( CONFMT, offset);
			printf( "(" );
			aput( p->in.left, size, 0);
			printf( ")" );
		}
		else {
			printf( "*" );
			adrput( p->in.left );
		}
		return;

	default:
		cerror( "illegal address" );
		return;

		}

	}

acon( p ) register NODE *p; { /* print out a constant */

	if( p->in.name[0] == '\0' ){	/* constant only */
		printf( CONFMT, p->tn.lval);
		}
	else if( p->tn.lval == 0 ) {	/* name only */
		printf( "%.8s", p->in.name );
		}
	else {				/* name + offset */
		printf( "%.8s+", p->in.name );
		printf( CONFMT, p->tn.lval );
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

	if( p->in.right ) temp = argsize( p->in.right );
	else temp = 0;

	if( p->in.right ){ /* generate args */
		genargs( p->in.right );
		}

	if( !shltype( p->in.left->in.op, p->in.left ) ) {
		order( p->in.left, INAREG|SOREG );
		}

	p->in.op = UNARY CALL;
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
	if( !(cookie&INTEMP) && asgop(p->in.op) ) return( INTEMP|INAREG|INTAREG|INTBREG|INBREG );
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

	o = p->in.op;
	t = p->in.type;
	if( t!=LONG && t!=ULONG ) return;

	for( f=opfunc; f->fop; f++ ) {
		if( o==f->fop && t==f->ftype ) goto convert;
		}
	return;

	/* need address of left node for ASG OP */
	/* WARNING - this won't work for long in a REG */
	convert:
	if( asgop( o ) ) {
		switch( p->in.left->in.op ) {

		case UNARY MUL:	/* convert to address */
			p->in.left->in.op = FREE;
			p->in.left = p->in.left->in.left;
			break;

		case NAME:	/* convert to ICON pointer */
			p->in.left->in.op = ICON;
			p->in.left->in.type = INCREF( p->in.left->in.type );
			break;

		case OREG:	/* convert OREG to address */
			p->in.left->in.op = REG;
			p->in.left->in.type = INCREF( p->in.left->in.type );
			if( p->in.left->tn.lval != 0 ) {
				q = talloc();
				q->in.op = PLUS;
				q->in.rall = NOPREF;
				q->in.type = p->in.left->in.type;
				q->in.left = p->in.left;
				q->in.right = talloc();

				q->in.right->in.op = ICON;
				q->in.right->in.rall = NOPREF;
				q->in.right->in.type = INT;
				q->in.right->in.name[0] = '\0';
				q->in.right->tn.lval = p->in.left->tn.lval;
				q->in.right->tn.rval = 0;

				p->in.left->tn.lval = 0;
				p->in.left = q;
				}
			break;

		default:
			cerror( "Bad address for hard ops" );
			/* NO RETURN */

			}
		}

	/* build comma op for args to function */
	q = talloc();
	q->in.op = CM;
	q->in.rall = NOPREF;
	q->in.type = INT;
	q->in.left = p->in.left;
	q->in.right = p->in.right;
	p->in.op = CALL;
	p->in.right = q;

	/* put function name in left node of call */
	p->in.left = q = talloc();
	q->in.op = ICON;
	q->in.rall = NOPREF;
	q->in.type = INCREF( FTN + p->in.type );
	strcpy( q->in.name, f->func );
	q->tn.lval = 0;
	q->tn.rval = 0;

	return;

	}

optim2( p ) register NODE *p; {
	/* do local tree transformations and optimizations */

	register NODE *r;

#ifdef PDP11
	switch( p->in.op ) {

	case AND:
		/* commute L and R to eliminate compliments and constants */
		if( p->in.left->in.op==ICON || p->in.left->in.op==COMPL ) {
			r = p->in.left;
			p->in.left = p->in.right;
			p->in.right = r;
			}
	case ASG AND:
		/* change meaning of AND to ~R&L - bic on pdp11 */
		r = p->in.right;
		if( r->in.op==ICON ) { /* compliment constant */
			r->tn.lval = ~r->tn.lval;
			}
		else if( r->in.op==COMPL ) { /* ~~A => A */
			r->in.op = FREE;
			p->in.right = r->in.left;
			}
		else { /* insert complement node */
			p->in.right = talloc();
			p->in.right->in.op = COMPL;
			p->in.right->in.rall = NOPREF;
			p->in.right->in.type = r->in.type;
			p->in.right->in.left = r;
			p->in.right->in.right = NULL;
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
		if( p->in.op == ICON && p->in.name[0]=='\0' &&
		    p->tn.lval>=1 && p->tn.lval <= 16) return( 1 );
		break;

	case SCCON:
		if( p->in.op == ICON && p->in.name[0]=='\0' &&
		    p->tn.lval>= -128 && p->tn.lval <= 127 ) return( 1 );
		break;

	case SICON:
		if( p->in.op == ICON && p->in.name[0]=='\0' &&
		    p->tn.lval>= 0 && p->tn.lval <=32767 ) return( 1 );
		break;

	case SUCCON:
		if( p->in.op == ICON && p->in.name[0]=='\0'
		    && p->tn.lval >= 0 && p->tn.lval <=255 ) return( 1 );
		break;

	case SCONSET:
		if( p->in.op == ICON && p->in.name[0]=='\0'
		    && p->tn.lval >= -32768 && p->tn.lval <= 0xffff
		    && apow2(p->tn.lval) >= 0 )  return( 1 );
		break;

	case SCONRES:
		if( p->in.op == ICON && p->in.name[0]=='\0'
		    && p->tn.lval >= -32768 && p->tn.lval <= 0xffff
		    && apow2( (~ p->tn.lval)&0xffff ) >= 0 )  return( 1 );
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
	fprintf( stderr, "%s, line %d: ", filename, lineno );
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

	if( logop( o = p->in.op ) && p->in.right->in.op == REG 
	    && o != NOT && o != ANDAND && o != OROR ) {
		temp = p->in.left;
		p->in.left = p->in.right;
		p->in.right = temp;
		p->in.op = revrel[o-EQ];
	}
}
