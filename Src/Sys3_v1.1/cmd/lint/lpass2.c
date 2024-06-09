# include "manifest"
# include "lerror.h"
# include "lmanifest"
# include "lpass2.h"

# define USED 01
# define VUSED 02
# define EUSED 04
# define RVAL 010
# define VARARGS 0100

# define NSZ 1024
# define TYSZ 2500
# define FSZ 150

STAB stab[NSZ];
STAB *find();

STYPE tary[TYSZ];
STYPE *tget();

char fnm[FSZ][LFNM];

int tfree;  /* used to allocate types */
int ffree;  /* used to save filenames */

struct ty atyp[50];
	/* r is where all the input ends up */
union rec r;

/* 28 feb 80  reverse sense of hflag */
int hflag = 1;
int pflag = 0;
/* 28 feb 80  reverse the sense of the xflag */
int xflag = 1;
int uflag = 1;
int ddddd = 0;

int cfno;  /* current file number */

main( argc, argv ) char *argv[]; {
	register char *p;
	char		*ifilename;
	extern char	*htmpname;


	ifilename = NULL;
	for ( --argc, ++argv; argc > 0; --argc, ++argv ) {
	    if ( **argv == '-' ) {
		p = ++*argv;
		if ( *p == 'H' ) {
		    /* we have here the name of the header buffer file */
		    htmpname = ++p;
		} else if ( *p == 'T' ) {
		    /* we have here the name of the intermediate file */
		    ifilename = ++p;
		} else {
		    for( 	; *p; ++p ){
			switch( *p ){

			case 'h':
				/* 28 feb 80 reverse sense of hflag */
				hflag = 0;
				break;

			case 'p':
				pflag = 1;
				break;

			case 'x':
				/* 28 feb 80  reverse sense of xflag */
				xflag = 0;
				break;

			case 'X':
				ddddd = 1;
				break;

			case 'u':
				uflag = 0;
				break;

			}
		    }
		}
	    }
	}

    tmpopen( );
    unbuffer( );
    if ( !freopen( ifilename, "r", stdin ) ){
		lerror( "cannot open intermediate file", FATAL | CCLOSE | 
			 ERRMSG );
		}

	mloop( LDI|LIB );
	rewind( stdin );
	mloop( LDC|LDX );
	rewind( stdin );
	mloop( LRV|LUV|LUE|LUM );
	cleanup();
	un2buffer( );
	return(0);
	}

mloop( m ){
	/* do the main loop */
	register STAB *q;

	while( lread(m) ){
		q = find();
		if( q->decflag ) chkcompat(q);
		else setuse(q);
		}
	}

lread(m){ /* read a line into r.l */

	register n;

	for(;;) {
		if( fread( (char *)&r, sizeof(r), 1, stdin ) <= 0 ) return(0);
		if( r.l.decflag & LFN ){
			/* new filename */
			setfno( r.f.fn );
			continue;
			}

		n = r.l.nargs;
		if( n<0 ) n = -n;
		if( n ){
			fread( (char *)atyp, sizeof(ATYPE), n, stdin );
			}
		if( ( r.l.decflag & m ) ) return( 1 );
		}
	}

setfno( s ) char *s; {
	/* look up current file names */
	/* first, strip backwards to the beginning or to the first / */
	int i;

	/* now look up s */
	for( i=0; i<ffree; ++i ){
		if( !strncmp( s, fnm[i], LFNM ) ){
			cfno = i;
			return;
			}
		}
	/* make a new entry */
	strncpy( fnm[ffree], s, LFNM );
	cfno = ffree++;
	}

STAB *
find(){
	/* for this to work, NSZ should be a power of 2 */
	register h=0;
	{	register char *p, *q;
		for( h=0,p=r.l.name,q=p+LCHNM; *p&&p<q; ++p) {
			h = (h<<1)+ *p;
			if( h>=NSZ ){
				h = (h+1)&(NSZ-1);
				}
			}
		}
	{	register STAB *p, *q;
		for( p=q= &stab[h]; q->decflag; ){
			/* this call to strncmp should be taken out... */
			if( !strncmp( r.l.name, q->name, LCHNM)) return(q);
			if( ++q >= &stab[NSZ] ) q = stab;
			if( q == p ) lerror( "too many names defined", CCLOSE |
					      FATAL | ERRMSG );
			}
		strncpy( q->name, r.l.name, LCHNM );
		return( q );
		}
	}

STYPE *
tget(){
	if( tfree >= TYSZ ){
		lerror( "too many types needed", CCLOSE | FATAL | ERRMSG );
		}
	return( &tary[tfree++] );
	}

chkcompat(q) STAB *q; {
	/* are the types, etc. in r.l and q compatible */
	register int i;
	STYPE *qq;

	setuse(q);

	/* argument check */

	if( q->decflag & (LDI|LIB|LUV|LUE) ){
		if( r.l.decflag & (LUV|LIB|LUE) ){
			if( q->nargs != r.l.nargs ){
				if( !(q->use&VARARGS) ){
					/* "%.8s: variable # of args." */
					buffer( 7, q );
					}
				if( r.l.nargs > q->nargs ) r.l.nargs = q->nargs;
				if( !(q->decflag & (LDI|LIB) ) ) {
					q->nargs = r.l.nargs;
					q->use |= VARARGS;
					}
				}
			for( i=0,qq=q->symty.next; i<r.l.nargs; ++i,qq=qq->next){
				if( chktype( &qq->t, &atyp[i] ) ){
					/* "%.8s, arg. %d used inconsistently" */
					buffer( 6, q, i+1 );
					}
				}
			}
		}

	if( (q->decflag&(LDI|LIB|LUV)) && r.l.decflag==LUV ){
		if( chktype( &r.l.type, &q->symty.t ) ){
			/* "%.8s value used inconsistently" */
			buffer( 4, q );
			}
		}

	/* check for multiple declaration */

	if( (q->decflag&LDI) && (r.l.decflag&(LDI|LIB)) ){
		/* "%.8s multiply declared" */
		buffer( 3, q );
		}

	/* do a bit of checking of definitions and uses... */

	if( (q->decflag & (LDI|LIB|LDX|LDC|LUM)) && (r.l.decflag & (LDX|LDC|LUM)) && q->symty.t.aty != r.l.type.aty ){
		/* "%.8s value declared inconsistently" */
		buffer( 5, q );
		}

	/* better not call functions which are declared to be structure or union returning */

	if( (q->decflag & (LDI|LIB|LDX|LDC)) && (r.l.decflag & LUE) && q->symty.t.aty != r.l.type.aty ){
		/* only matters if the function returns union or structure */
		TWORD ty;
		ty = q->symty.t.aty;
		if( ISFTN(ty) && ((ty = DECREF(ty))==STRTY || ty==UNIONTY ) ){
			/* "%.8s function value type must be declared before use" */
			buffer( 8, q );
			}
		}

	if( pflag && q->decflag==LDX && r.l.decflag == LUM && !ISFTN(q->symty.t.aty) ){
		/* make the external declaration go away */
		/* in effect, it was used without being defined */
		}
	}

	/* messages for defintion/use */
int	mess[2][2] = {
	12,
	0,
	1,
	2
};

lastone(q) STAB *q; {

	register nu, nd, uses;

	if( ddddd ) pst(q);

	nu = nd = 0;
	uses = q->use;

	if( !(uses&USED) && q->decflag != LIB ) {
		if( strncmp(q->name,"main",7) )
			nu = 1;
		}

	if( !ISFTN(q->symty.t.aty) ){
		switch( q->decflag ){

		case LIB:
			nu = nd = 0;  /* don't complain about uses on libraries */
			break;
		case LDX:
			if( !xflag ) break;
		case LUV:
		case LUE:
/* 01/04/80 */	case LUV | LUE:
		case LUM:
			nd = 1;
			}
		}
	if( uflag && ( nu || nd ) ) buffer( mess[nu][nd], q );

	if( (uses&(RVAL+EUSED)) == (RVAL+EUSED) ){
	    if ( uses & VUSED ) {
		/* "%.8s returns value which is sometimes ignored\n" */
		buffer( 11, q );
	    } else {
		/* "%.8s returns value which is always ignored\n" */
		buffer( 10, q );
	    }
		}

	if( (uses&(RVAL+VUSED)) == (VUSED) && (q->decflag&(LDI|LIB)) ){
		/* "%.8s value is used, but none returned\n" */
		buffer( 9, q );
		}
	}

cleanup(){ /* call lastone and die gracefully */
	STAB *q;
	for( q=stab; q< &stab[NSZ]; ++q ){
		if( q->decflag ) lastone(q);
		}
	return;
	}

setuse(q) STAB *q; { /* check new type to ensure that it is used */

	if( !q->decflag ){ /* new one */
		q->decflag = r.l.decflag;
		q->symty.t = r.l.type;
		if( r.l.nargs < 0 ){
			q->nargs = -r.l.nargs;
			q->use = VARARGS;
			}
		else {
			q->nargs = r.l.nargs;
			q->use = 0;
			}
		q->fline = r.l.fline;
		q->fno = cfno;
		if( q->nargs ){
			int i;
			STYPE *qq;
			for( i=0,qq= &q->symty; i<q->nargs; ++i,qq=qq->next ){
				qq->next = tget();
				qq->next->t = atyp[i];
				}
			}
		}

	switch( r.l.decflag ){

	case LRV:
		q->use |= RVAL;
		return;
	case LUV:
		q->use |= VUSED+USED;
		return;
	case LUE:
		q->use |= EUSED+USED;
		return;
/* 01/04/80 */	case LUV | LUE:
	case LUM:
		q->use |= USED;
		return;

		}
	}

chktype( pt1, pt2 ) register ATYPE *pt1, *pt2; {
	TWORD t;

	/* check the two type words to see if they are compatible */
	/* for the moment, enums are turned into ints, and should be checked as such */
	if( pt1->aty == ENUMTY ) pt1->aty =  INT;
	if( pt2->aty == ENUMTY ) pt2->aty = INT;

	if( (t=BTYPE(pt1->aty)==STRTY) || t==UNIONTY ){
		return( pt1->aty!=pt2->aty || pt1->extra!=pt2->extra );
		}

	if( pt2->extra ){ /* constant passed in */
		if( pt1->aty == UNSIGNED && pt2->aty == INT ) return( 0 );
		else if( pt1->aty == ULONG && pt2->aty == LONG ) return( 0 );
		}
	else if( pt1->extra ){ /* for symmetry */
		if( pt2->aty == UNSIGNED && pt1->aty == INT ) return( 0 );
		else if( pt2->aty == ULONG && pt1->aty == LONG ) return( 0 );
		}

	return( pt1->aty != pt2->aty );
	}

struct tb { int m; char * nm };
ptb( v, tp ) struct tb *tp; {
	/* print a value from the table */
	int flag;
	flag = 0;
	for( ; tp->m; ++tp ){
		if( v&tp->m ){
			if( flag++ ) putchar( '|' );
			printf( "%s", tp->nm );
			}
		}
	}

pst( q ) STAB *q; {
	/* give a debugging output for q */
	static struct tb dfs[] = {
		LDI, "LDI",
		LIB, "LIB",
		LDC, "LDC",
		LDX, "LDX",
		LRV, "LRV",
		LUV, "LUV",
		LUE, "LUE",
		LUM, "LUM",
		0, "" };

	static struct tb us[] = {
		USED, "USED",
		VUSED, "VUSED",
		EUSED, "EUSED",
		RVAL, "RVAL",
		VARARGS, "VARARGS",
		0,	0,
		};

	printf( "%.8s (", q->name );
	ptb( q->decflag, dfs );
	printf( "), use= " );
	ptb( q->use, us );
	printf( ", line %d, nargs=%d\n", q->fline, q->nargs );
	}

