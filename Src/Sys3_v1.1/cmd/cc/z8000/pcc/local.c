/*	Plexus - Sys3 - August 1982	*/

static char c_local[] = "@(#)local.c	1.2";

# include "mfile1"

extern int eprint();
int cdebug;


/*	this file contains code which is dependent on the target machine */

/*
 *	the global 'regvar' is treated as bits going right to left
 *	corresponing to reisters r13 to r8
 */

#define ALLREGS 0x3f

nextregvar( rvar ) {
	register int rv = rvar;
	register lastreg;

	rv <<= 9;
	lastreg = MINRVAR-1;
	do {
		rv <<= 1;
		if( rv < 0 )
			break;
	} while( ++lastreg < MAXRVAR );
	return( lastreg );
}

regfit( rvar, type )
TWORD type;
{
	register rv = rvar;
	register TWORD ty = type;
	register reg;

	if( rvar == ALLREGS )
		return( 0 );
	if( ty != LONG ) {
		reg = MAXRVAR;
		do {
			if( (rv & 01) == 0 )
				return( reg );
			rv >>= 1;
			reg--;
		} while( 1 );
	} else {
		reg = MAXRVAR-1;
		do {
			if( (rv & 03) == 0 )
				return( reg );
			rv >>= 2;
		} while( (reg -= 2) >= MINRVAR );
		return( 0 );
	}
	return(0);
}

allocregvar( rvp, type )
int *rvp;
TWORD type;
{
	register rv = *rvp;
	register reg;
	register rpos;

	reg = regfit( rv, type );
	rpos = MAXRVAR - reg;
	if( type != LONG )
		*rvp = rv | (1 << rpos);
	else
		*rvp = rv | (3 << (rpos-1));
	return( reg );
}

NODE *
cast( p, t ) register NODE *p; TWORD t; {
	/* cast node p to type t */

	p = buildtree( CAST, block( NAME, NIL, NIL, t, 0, (int)t ), p );
	p->in.left->in.op = FREE;
	p->in.op = FREE;
	return( p->in.right );
	}

NODE *
clocal(p) NODE *p; {

	/* this is called to do local transformations on
	   an expression tree preparitory to its being
	   written out in intermediate code.
	*/

	/* the major essential job is rewriting the
	   automatic variables and arguments in terms of
	   REG and OREG nodes */
	/* conversion ops which are not necessary are also clobbered here */
	/* in addition, any special features (such as rewriting
	   exclusive or) are easily handled here as well */

	register struct symtab *q;
	register NODE *r;
	register o;
	register m, ml;

	switch( o = p->in.op ){

	case NAME:
		if( p->tn.rval < 0 ) { /* already processed; ignore... */
			return(p);
		}
		q = &stab[p->tn.rval];
		switch( q->sclass ){

		case AUTO:
		case PARAM:
			/* fake up a structure reference */
			r = block( REG, NIL, NIL, PTR+STRTY, 0, 0 );
			r->tn.lval = 0;
			r->tn.rval = (q->sclass==AUTO?STKREG:ARGREG);
			p = stref( block( STREF, r, p, 0, 0, 0 ) );
			break;

		case ULABEL:
		case LABEL:
		case STATIC:
			if( q->slevel == 0 ) break;
			p->tn.lval = 0;
			p->tn.rval = -q->offset;
			break;

		case REGISTER:
			p->in.op = REG;
			p->tn.lval = 0;
			p->tn.rval = q->offset;
			break;

		}
		break;
	case LT:
	case LE:
	case GT:
	case GE:
		if( ISPTR( p->in.left->in.type ) || ISPTR( p->in.right->in.type ) ){
			p->in.op += (ULT-LT);
		}
		break;

	case PCONV:
		/* do pointer conversions for char and longs */
		ml = p->in.left->in.type;
		if( ml == FLOAT )
			goto pclobber;
		if( ( ml==CHAR || ml==UCHAR || ml==LONG || ml==ULONG )
		   && p->in.left->in.op != ICON ) break;

pclobber:
		/* pointers all have the same representation;
		   the type is inherited */
		p->in.left->in.type = p->in.type;
		p->in.left->fn.cdim = p->fn.cdim;
		p->in.left->fn.csiz = p->fn.csiz;
		p->in.op = FREE;
		return( p->in.left );

	case SCONV:
		if(cdebug)fwalk(p,eprint,0);
/*
 *		m = (p->in.type == FLOAT || p->in.type == DOUBLE );
 *		ml = (p->in.left->in.type == FLOAT || p->in.left->in.type == DOUBLE );
 *		if( m != ml ) break;
 */

		/* now, look for conversions downwards */

		m = p->in.type;
		ml = p->in.left->in.type;
		if( p->in.left->in.op == ICON ){
			/* simulate the conversion here from an ICON */
			CONSZ val;
			val = p->in.left->tn.lval;
			switch( m ){ /* convert to this type */
			case CHAR:
				p->in.left->tn.lval = (char) val;
				goto clobber;
			case UCHAR:
				p->in.left->tn.lval = val & 0XFF;
				goto clobber;
			case UNSIGNED:
				p->in.left->tn.lval = val & 0XFFFFL;
				break;
			case INT:
				p->in.left->tn.lval = (int)val;
				break;
			}
		} else if ( p->in.left->in.op == FCON ) {
			/* simulate the conversion here from an FCON */
			CONSZ val;
			val = (CONSZ)p->in.left->fpn.dval;
			if ( m == FLOAT || m == DOUBLE )
				;
			else	{
				p->in.left->in.op = ICON;
				p->in.left->in.name[0] = 0;
				p->in.left->tn.rval = NONAME;
				switch( m ){ /* convert to this type */
				case CHAR:
					p->in.left->tn.lval = (char) val;
					m = INT;
					break;
				case UCHAR:
					p->in.left->tn.lval = val & 0XFF;
					m = UNSIGNED;
					break;
				case UNSIGNED:
					p->in.left->tn.lval = val & 0XFFFFL;
					break;
				case INT:
					p->in.left->tn.lval = (int)val;
						break;
				default:		/* must be long */
					p->in.left->tn.lval = val;
					break;
				}
			}
		} else {
			/* meaningful ones are conversion of int to char,
			   int to short,
			   and short to char, and unsigned version of them */
			if( m==CHAR || m==UCHAR ){
				if( ml != CHAR && ml != UCHAR ) break;
			}
			else if( m==INT || m==UNSIGNED ){
				if( ml != INT && ml != UNSIGNED ) break;
			}
			else if( m==LONG || m==ULONG ) {
				if( ml != LONG && ml != ULONG ) break;
			}
			else if( m==DOUBLE ) {
				if( ml != DOUBLE ) break;
			}
			else if( m==FLOAT ) {
				if( ml != FLOAT ) break;
			}
		}

		/* clobber conversion */
		p->in.left->in.type = m; /* inherit type */
	clobber:
		p->in.op = FREE;
		return( p->in.left );  /* conversion gets clobbered */

	case PVCONV:
	case PMCONV:
		if( p->in.right->in.op != ICON ) cerror( "bad conversion", 0);
		p->in.op = FREE;
		return( buildtree( o==PMCONV?MUL:DIV, p->in.left, p->in.right ) );

	case PLUS:
	case MINUS:
	case LS:
	case MUL:
		/* optimize address calculations with long indexes */
		if( ISPTR( p->in.type ) || ISARY( p->in.type ) ) {
			if( p->in.left->in.type==LONG || p->in.left->in.type==ULONG )
				p->in.left = cast( p->in.left, INT );
			if( p->in.right->in.type==LONG || p->in.right->in.type==ULONG )
				p->in.right = cast( p->in.right, INT );
		}
		break;

	}

	return(p);
}

andable( p ) NODE *p; {
	return(1);  /* all names can have & taken on them */
	}

cendarg(){ /* at the end of the arguments of a ftn, set the automatic offset */
	autooff = AUTOINIT;
	}

cisreg( t ) TWORD t; { /* is an automatic variable of type t OK for a register variable */

	if( t==INT || t==UNSIGNED || ISPTR(t) || t==LONG ) return(1);
	return(0);
	}

NODE *
offcon( off, t, d, s ) OFFSZ off; TWORD t; {

	/* return a node, for structure references, which is suitable for
	   being added to a pointer of type t, in order to be off bits offset
	   into a structure */

	register NODE *p;

	/* t, d, and s are the type, dimension offset, and sizeoffset */
	/* in general they  are necessary for offcon, but not on H'well */

	p = bcon(0);
	p->tn.lval = off/SZCHAR;
	return(p);

	}

static inwd	/* current bit offsed in word */;
static word	/* word being built from fields */;

incode( p, sz ) register NODE *p; {

	/* generate initialization code for assigning a constant c
		to a field of width sz */
	/* we assume that the proper alignment has been obtained */
	/* inoff is updated to have the proper final value */
	/* we also assume sz  < SZINT */

	if((sz+inwd) > SZINT) cerror("incode: field > int");
	word |= ( p->tn.lval & ((1L<<sz)-1) ) << (16-inwd-sz) ;
	inwd += sz;
	inoff += sz;
	if(inoff%SZINT == 0) {
		printf( "	.word	0%o\n", word);
		word = inwd = 0;
		}
	}

fincode( d, sz ) double d; {
	/* output code to initialize space of size sz to the value d */
	/* the proper alignment has been obtained */
	/* inoff is updated to have the proper final value */
	/* on the target machine, write it out in octal! */

	register int *mi;
	float f;

	if( sz==SZDOUBLE ) {
		mi = (int *)&d;
		printf( "	.word	0x%x, 0x%x, 0x%x, 0x%x\n",
			mi[0], mi[1], mi[2], mi[3] );
	} else {
		f = d;
		mi = (int *)&f;
		printf( "	.word	0x%x, 0x%x\n", mi[0], mi[1] );
	}
	inoff += sz;
}

cinit( p, sz ) NODE *p; {
	/* arrange for the initialization of p into a space of
	size sz */
	/* the proper alignment has been opbtained */
	/* inoff is updated to have the proper final value */
	ecode( p );
	inoff += sz;
	}

vfdzero( n ){ /* define n bits of zeros in a vfd */

	if( n <= 0 ) return;

	inwd += n;
	inoff += n;
	if( inoff%ALINT ==0 ) {
		printf( "	.word	0%o\n", word );
		word = inwd = 0;
		}
	}


char *
exname( p ) char *p; {
	/* make a name look like an external name in the local machine */

	static char text[NCHNAM+1];

	register i;

	text[0] = '_';
	for( i=1; *p&&i<NCHNAM; ++i ){
		text[i] = *p++;
		}

	text[i] = '\0';
	text[NCHNAM] = '\0';  /* truncate */

	return( text );
	}

ctype( type ) TWORD type; { /* map types which are not defined on the local machine */
	switch( BTYPE(type) ){
	case SHORT:
		MODTYPE(type,INT);
		break;
	case USHORT:
		MODTYPE(type,UNSIGNED);
		}
	return( type );
	}

noinit() { /* curid is a variable which is defined but
	is not initialized (and not a function );
	This routine returns the storage class for an uninitialized declaration */

	return(EXTERN);

	}

commdec( id ){ /* make a common declaration for id, if reasonable */
	register struct symtab *q;
	OFFSZ off;

	q = &stab[id];
	printf( "	.comm	%s,", exname( q->sname ) );
	off = tsize( q->stype, q->dimoff, q->sizoff );
	printf( CONFMT, off/SZCHAR );
	printf( "\n" );
	}

isitlong( cb, ce ){ /* is lastcon to be long or short */
	/* cb is the first character of the representation, ce the last */

	if( ce == 'l' || ce == 'L' ||
		lastcon >= (1L << (SZINT-1) ) ) return (1);
	return(0);
	}


isitfloat( s ) char *s; {
	double atof();
	dcon = atof(s);
	return( FCON );
	}

ecode( p ) NODE *p; {

	/* walk the tree and write out the nodes.. */

	if( nerrors ) return;
/* CCF mods for two pass operation */
#ifdef ONEPASS
	p2tree( p );
	p2compile( p );
#endif
#ifndef ONEPASS
printf(".%d\t%s\n",lineno,ftitle);
prtree(p);
#endif
	}

