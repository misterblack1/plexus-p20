/*	Plexus - Sys3 - August 1982	*/

static char code_c[] = "@(#)code.c	1.1";

# include <stdio.h>
# include <signal.h>

# include "mfile1"

int csavlab, enterlab;
int cretlab;
extern int lineswitch;
int proflag;
int strftn = 0;	/* is the current function one which returns a value */
FILE *tmpfile;
FILE *outfile = stdout;

branch( n ){
	/* output a branch to label n */
	/* exception is an ordinary function branching to retlab: then,return */
	if( n == retlab && !strftn ){
#ifdef ONEPASS
		printf( "	jpr	L%d\n", cretlab );
#else
		printf( "	jpr	cret\n" );
#endif
	}
	else printf( "	jpr	L%d\n", n );
}

int lastloc = PROG;

defalign(n) {
	/* cause the alignment to become a multiple of n */
	n /= SZCHAR;
	if( lastloc != PROG && n > 1 ) printf( "	.even\n" );
	}

locctr( l ){
	register temp;
	/* l is PROG, ADATA, DATA, STRNG, ISTRNG, STAB, or BSS */

	if( l == lastloc ) return(l);
	temp = lastloc;
	lastloc = l;
	switch( l ){

	case PROG:
		outfile = stdout;
		printf( "	.text\n" );
		break;

	case DATA:
	case ADATA:
		outfile = stdout;
		if( temp != DATA && temp != ADATA )
			printf( "	.data\n" );
		break;

	case BSS:
		outfile = stdout;
		if( temp != BSS )
			printf( "	.bss\n" );
		break;

	case STRNG:
	case ISTRNG:
		outfile = tmpfile;
		break;

	case STAB:
		cerror( "locctr: STAB unused" );
		break;

	default:
		cerror( "illegal location counter" );
		}

	return( temp );
	}

deflab( n ){
	/* output something to define the current position as label n */
	fprintf( outfile, "L%d:\n", n );
	}

int crslab = 10;

getlab(){
	/* return a number usable for a label */
	return( ++crslab );
	}

efcode(){
	/* code for the end of a function */

	if( strftn ){  /* copy output (in r7) to caller */
		register struct symtab *p;
		register int stlab;
		register int count;
		int size;

		p = &stab[curftn];

		deflab( retlab );

		stlab = getlab();
		printf( "	ld	r6,$L%d\n", stlab );
		size = tsize( DECREF(p->stype), p->dimoff, p->sizoff ) / SZCHAR;
		count = size/2;
		printf( "	ld	r5,$%d\n", count );
		printf( "	ldir	*r6,*r7,r5\n" );
		printf( "	ld	r7,$L%d\n", stlab );
		printf( "	.bss\nL%d:	.=.+%d\n	.text\n", stlab, size );
		/* turn off strftn flag, so return sequence will be generated */
		strftn = 0;
		}
#ifndef ONEPASS
		deflab(cretlab);
		printf( "	jpr	cret\n" );
#endif
/* CCF mods for two pass operation */
#ifdef ONEPASS
	p2bend();
#endif
#ifndef ONEPASS
printf("]...\n");
#endif
	}


bfcode( a, n ) OFFSZ int a[]; {
	/* code for the beginning of a function; a is an array of
		indices in stab for the arguments; n is the number */
	register i;
	register temp;
	register struct symtab *p;
	OFFSZ off;

	locctr( PROG );
	p = &stab[curftn];
	defnam( p );
	printf( "~~%.8s:\n", p->sname );
	temp = p->stype;
	temp = DECREF(temp);
	strftn = (temp==STRTY) || (temp==UNIONTY);

	for(i = 0;i<paramno;i++){
		p = &stab[paramstk[i]];
		printf( "\t~%.8s = %ld\n", p->sname, p->offset/SZCHAR );
	}

	retlab = getlab();
	/* routine prolog */

	/* adjust stack for autos */
	/* do adjust at routine exit, depend on optimizer */
	enterlab = getlab();
	csavlab = getlab();
	cretlab = getlab();
	branch(csavlab);
	deflab(enterlab);

	off = ARGINIT;

	for( i=0; i<n; ++i ){
		p = &stab[a[i]];
		if( p->sclass == REGISTER ){
			temp = p->offset;  /* save register number */
			p->sclass = PARAM;  /* forget that it is a register */
			p->offset = NOOFFSET;
			oalloc( p, &off );
			if( p->stype == LONG || p->stype == ULONG ) {
				printf( "	ldl	rr%d,%ld(r14)\n",
				    temp, p->offset/SZCHAR );
			} else {
				printf( "	ld	r%d,%ld(r14)\n",
				    temp, p->offset/SZCHAR );
			}
			p->offset = temp;  /* remember register number */
			p->sclass = REGISTER;/*remember that it is a register*/
			}
		else {
			if( oalloc( p, &off ) ) cerror( "bad argument" );
			}

		}
	}

bccode(){ /* called just before the first executable statment */
		/* by now, the automatics and register variables are allocated */
	SETOFF( autooff, SZINT );
	/* set aside store area offset */
/* CCF mods for two pass operation */
#ifdef ONEPASS
	p2bbeg( autooff, regvar );
#endif
#ifndef ONEPASS
printf("[%d\t%ld\t%d\t\n",ftnno,autooff,regvar);
#endif
	}

ejobcode( flag ){
	/* called just before final exit */
	/* flag is 1 if errors, 0 if none */
	}

aobeg(){
	/* called before removing automatics from stab */
	}

aocode(p) struct symtab *p; {
	/* called when automatic p removed from stab */
	}

aoend(){
	/* called after removing all automatics from stab */
	}

defnam( p ) register struct symtab *p; {
	/* define the current location as the name p->sname */

	if( p->sclass == EXTDEF )
		printf( "	.globl	%.8s\n",  exname(p->sname) );
	else if( p->slevel <= 1 )
		printf( "	.globl\n"); /* for the optimizer */
	if( p->sclass == STATIC && p->slevel>1 ) deflab( (int) p->offset );
	else printf( "%s:\n", exname( p->sname ) );

	}

fixdef( p ) register struct symtab *p; {

	if( p->slevel <= 1 ) return;
	if( p->sclass == AUTO )
		printf( "	~%.8s = %ld\n", p->sname, p->offset/SZCHAR );
	else if( p->sclass == REGISTER )
		printf( "	~%.8s = r%ld\n", p->sname, p->offset );
/*
	else if( p->sclass == STATIC )
		printf( "	~%.8s = L%ld\n", p->sname, p->offset );
*/
}

/*
bycode( t, i ){
	i &= 0xf;
	if( t < 0 ) {
		if( i != 0 ) fprintf( outfile, "\n" );
	} else {
		if( i == 0 ) fprintf( outfile, "	.byte	" );
		else fprintf( outfile, "," );
		fprintf( outfile, "%d", t );
		if( i == 0xf ) fprintf( outfile, "\n" );
	}
}
*/
bycode( t, i ){
	/* put byte i+1 in a string */

	i &= 31;
	if( t < 0 ){ /* end of the string */
		if( i != 0 ) fprintf( outfile, "\"\n" );
		}

	else { /* stash byte t into string */
		if( i == 0 )  fprintf( outfile, "	.ascii	\"" );
		outchar( t );
		if( i == 31 ) fprintf( outfile, "\"\n");
		}
	}

outchar( ch ) {

	if (((ch >= ' ') && (ch <= '~')) && (ch != '\\') && (ch != '\"'))
		fprintf(outfile,"%c",ch);
	else	switch (ch) {
		case '\\':
			fprintf(outfile,"\\\\");
			break;
		case '\n':
			fprintf(outfile,"\\n");
			break;
		case '\r':
			fprintf(outfile,"\\r");
			break;
		case '\b':
			fprintf(outfile,"\\b");
			break;
		case '\t':
			fprintf(outfile,"\\t");
			break;
		case '\f':
			fprintf(outfile,"\\f");
			break;
		case '"':
			fprintf(outfile,"\\\"");
			break;
		default:
			if (ch != 0)
				fprintf(outfile,"\\%03.3o",ch);
			else
				fprintf(outfile,"\\0");
		}
	}
zecode( n ){
	/* n integer words of zeros */
	OFFSZ temp;
	register i;

	if( n <= 0 ) return;
	printf( "	. = . + %d\n", 2*n );
/*
	printf( "	.word	0" );
	i = 1;
	while (i++ < n) {
		if( i%8 == 0 )
			printf( "\n	.word	0");
		else
			printf( ", 0" );
	}
	printf( "\n" );
*/
	temp = n;
	inoff += temp*SZINT;
}

fldal( t ) unsigned t; { /* return the alignment of field of type t */
	uerror( "illegal field type" );
	return( ALINT );
	}

fldty( p ) struct symtab *p; { /* fix up type of field p */
	;
	}

where(c){ /* print location of error  */
	/* c is either 'u', 'c', or 'w' */
	fprintf( stderr, "%s, line %d: ", ftitle, lineno );
	}

char *tmpname = "/tmp/pcXXXXXX";

main( argc, argv ) char *argv[]; {
	int dexit();
	register int c;
	register int i;
	int r;

	for( i=1; i<argc; ++i )
		if( argv[i][0] == '-' && argv[i][1] == 'X' && argv[i][2] == 'p' ) {
			proflag = 1;
			}

	mktemp(tmpname);
	if(signal( SIGHUP, SIG_IGN) != SIG_IGN) signal(SIGHUP, dexit);
	if(signal( SIGINT, SIG_IGN) != SIG_IGN) signal(SIGINT, dexit);
	if(signal( SIGTERM, SIG_IGN) != SIG_IGN) signal(SIGTERM, dexit);
	tmpfile = fopen( tmpname, "w" );

	r = mainp1( argc, argv );

	tmpfile = freopen( tmpname, "r", tmpfile );
	if( tmpfile != NULL )
		while((c=getc(tmpfile)) != EOF )
			putchar(c);
	else cerror( "Lost temp file" );
	unlink(tmpname);
	return( r );
	}

dexit( v ) {
	unlink(tmpname);
	exit(1);
	}

genswitch(p,n) register struct sw *p;{
	/*	p points to an array of structures, each consisting
		of a constant value and a label.
		The first is >=0 if there is a default label;
		its value is the label number
		The entries p[1] to p[n] are the nontrivial cases
		*/
	register i;
	register CONSZ j, range;
	register dlab, swlab;

	range = p[n].sval-p[1].sval;
	dlab = p->slab >= 0 ? p->slab : getlab();

	if( range>0 && range <= 3*n && n>=4 ){ /* implement a direct switch */

		printf( "	ld	r6,$L%d\n", swlab = getlab() );
		if (lineswitch) {
printf( "	ld	r5,4(r6)" );	/* r5 = default code address */
printf( "	cp	r7,2(r6)" );		
printf( "	jpr	gt,*r5" );	/* out of case range on high end */
printf( "	sub	r7,*r6" );	
printf( "	jpr	lt,*r5" );	/* out of case range on low end */
printf( "	add	r6,r7" );	
printf( "	add	r6,r7" );	/* r6 =index into case code addresses */
printf( "	ld	r5,6(r6)" );	/* r5 = case code address */
printf( "	jpr	*r5" );		/* jump to case */
		} else
			printf( "	jpr	switch1\n" );

		/* output table */

		locctr( ADATA );
		defalign( ALPOINT );
		deflab( swlab );

		printf( "	.word	%ld\n", p[1].sval );
		printf( "	.word	%ld\n", p[n].sval );
		if( p->slab < 0 )
			printf( "	.word	L%d\n", dlab );
		else
			printf( "	.word	L%d\n", p[0].slab );

		for( i=1,j=p[1].sval; i<=n; ++j ){
			printf( "	.word	L%d\n", ( j == p[i].sval ) ?
				p[i++].slab : dlab );
			}

		locctr( PROG );

		if( p->slab< 0 ) deflab( dlab );
		return;

		}

	/* debugging code */

	if( n >= 4 ) {
		printf( "	ld	r6,$L%d\n", swlab=getlab() );
		if (lineswitch) {
printf( "	ld	r5,*r6" );	/* r5 = number of cases + 1 */
printf( "	ld	r4,r5" );	/* r4 = number of cases + 1 */
printf( "	inc	r6,$2" );	/* r6 = ptr to case value list */
printf( "	cpir	r7,*r6,r5,eq" );	/* look for correct case value */
printf( "	add	r6,r4" );		
printf( "	add	r6,r4" );	/* r6=address of correct code address */
printf( "	ld	r5,-4(r6)" );	/* r5 = correct case code address */
printf( "	jpr	*r5" );		/* jump to case */
		} else 
			printf( "	jpr	switch2\n" );
		locctr( ADATA );
		defalign( ALPOINT );
		deflab( swlab );
		printf( "	.word	%d\n", n+1);
		for (i=1; i<=n; i++)
			printf( "	.word	%ld\n", j = p[i].sval );
		for (i=1; i<=n; i++)
			printf( "	.word	L%d\n", p[i].slab );
		printf( "	.word	L%d\n", dlab );
		locctr( PROG );
		if( p->slab < 0 ) deflab( dlab );
		return;
		}

	/* simple switch code */

	for( i=1; i<=n; ++i ){
		/* already in r7 */

		printf( "	cp	r7,$" );
		printf( CONFMT, p[i].sval );
		printf( "\n	jpr	eq,L%d\n", p[i].slab );
		}

	if( p->slab>=0 ) branch( p->slab );
	}
