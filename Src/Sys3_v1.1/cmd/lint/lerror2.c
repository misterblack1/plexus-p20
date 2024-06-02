# include	<stdio.h>
# include	"messages.h"
# include	"lerror.h"
# include	"manifest"
# include	"lmanifest"
# include	"lpass2.h"
# include	<signal.h>


/*  tmpopen( )
 *
 *  open source message buffer file for writing
 *  open header message file for reading
 *    initialize header file name and count list from header message file
 *
 */

static char	ctmpname[ TMPLEN + 16 ] = "";
char		*htmpname = NULL;

static FILE	*ctmpfile = NULL;
static FILE	*htmpfile = NULL;

static HDRITEM	hdrlist[ NUMHDRS ];

tmpopen( )
{
    extern	sprintf( );
    extern int	getpid( ),
		getppid( );
    extern FILE	*fopen( );
    extern int	fread( );

    extern	lerror( ),
		catchsig( );

    sprintf( ctmpname, "%s/clint%d", TMPDIR, getpid( ) );

    catchsig( );
    if ( (ctmpfile = fopen( ctmpname, "w" )) == NULL ) {
	lerror( "cannot open message buffer file", FATAL | ERRMSG );
	return;
    }

    if ( (htmpfile = fopen( htmpname, "r" )) == NULL ) {
	lerror( "cannot open header message buffer file", CCLOSE | FATAL |
		 ERRMSG );
	return;
    }
    if ( fread((char *) hdrlist, sizeof( HDRITEM ), NUMHDRS, htmpfile) != NUMHDRS ) {
	lerror( "cannot read header message buffer file", CCLOSE | HCLOSE |
		FATAL | ERRMSG );
	return;
    }
    return;
}



/*  lerror( )
 *
 *  lint error message routine
 *  if code is [CH]CLOSE error close and unlink appropriate files
 *  if code is FATAL exit
 */

lerror( message, code )

char	*message;
int	code;

{
    extern	fprintf( ),
		fclose( ),
		unlink( );

    if ( code & ERRMSG ) {
	fprintf( stderr, "lint pass2 error: %s\n", message );
    }

    if ( code & CCLOSE ) {
	if ( ctmpfile != NULL ) {
	    fclose( ctmpfile );
	    unlink( ctmpname );
	}
    }
    if ( code & HCLOSE ) {
	if ( htmpfile != NULL ) {
	    fclose( htmpfile );
	    unlink( htmpname );
	}
    }
    if ( code & FATAL ) {
	exit( FATAL );
    }

    return;
}


/* unbuffer( )
 *
 * writes out information saved in htmpfile
 *
 */

unbuffer( )
{
    extern		fclose( ),
			fprintf( ),
			unlink( );
    extern FILE		*fopen( );
    extern int		fseek( ),
			fread( );
    extern		lerror( );


    int		i,
		j,
		stop;
    HRECORD	record;

    if (fseek( htmpfile, (long) sizeof ( hdrlist ), 0 ) != OKFSEEK ) {
	lerror( "cannot seek in header message buffer file", HCLOSE | ERRMSG );
	return;
    }

    for ( i = 0; ( i < NUMHDRS ) && ( hdrlist[ i ].hcount != 0 ); ++i ) {
	stop = hdrlist[ i ].hcount;
	printf( "\n%.14s  (as included in %.14s)\n==============\n",
		hdrlist[ i ].hname, hdrlist[ i ].sname );
	for ( j = 0; j < stop; ++j ) {
	    if ( fread( (char *) &record, HRECSZ, 1, htmpfile ) != 1 ) {
		lerror( "cannot read header message buffer file", HCLOSE |
			FATAL | ERRMSG );
		return;
	    }

	    printf( "(%d)  ", record.lineno );
	    if ( record.code & WERRTY ) {
		printf( "warning: " );
	    }
	    switch( record.code & ~( WERRTY | SIMPL ) ) {

		case DBLSTRTY:
		    printf( msgtext[ record.msgndx ],
			    record.arg1.name1, record.name2 );
		    break;

		case STRINGTY:
		    printf( msgtext[ record.msgndx ],
			     record.arg1.name1 );
		    break;

		case CHARTY:
		    printf( msgtext[ record.msgndx ],
			     record.arg1.char1 );
		    break;

		case NUMTY:
		    printf( msgtext[ record.msgndx ],
			     record.arg1.number );
		    break;

		default:
		    printf( msgtext[ record.msgndx ] );
		    break;

	    }
	printf( "\n" );
	}
    }

    fclose( htmpfile );
    unlink( htmpname );
    return;
}


/*  catchsig( )
 *
 *  prepares to field interrupts with the routine onintr( )
 * catchsig simply returns
 */


catchsig( )

{
    extern int	(*signal( ))( );
    extern		onintr( );

    if ((signal(SIGINT, SIG_IGN)) == SIG_DFL)
	signal(SIGINT, onintr);

    if ((signal(SIGHUP, SIG_IGN)) == SIG_DFL)
	signal(SIGHUP, onintr);

    if ((signal(SIGQUIT, SIG_IGN)) == SIG_DFL)
	signal(SIGQUIT, onintr);

    if ((signal(SIGPIPE, SIG_IGN)) == SIG_DFL)
	signal(SIGPIPE, onintr);

    if ((signal(SIGTERM, SIG_IGN)) == SIG_DFL)
	signal(SIGTERM, onintr);

    return;

}


/*  onintr( )
 *
 *  cleans up after an interrupt happens 
 *  ignores signals (interrupts) during its work
 */


onintr( )

{
    extern int	(*signal( ))( );
    extern	fprintf( );
    extern	lerror( );

    signal(SIGINT, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGTERM, SIG_IGN);

    fprintf( stderr, "\n" );
    lerror( "interrupt", CCLOSE | HCLOSE | FATAL );
    /* note that no message is printed */

}


static int	msg2totals[ NUM2MSGS ];
# define nextslot(x)	(( PERC2SZ * (x) ) + ( C2RECSZ * msg2totals[(x)] ))

/* VARARGS2 */
buffer( msgndx, symptr, digit )

int	msgndx;
STAB	*symptr;
int	digit;

{
    extern char		*strncpy( );
    extern int		fseek( ),
			fwrite( );
    extern		lerror( );

    extern short	msg2type[ ];
    extern int		cfno;
    extern union rec	r;

    C2RECORD		record;

    if ( ( msgndx < 0 ) || ( msgndx >= NUM2MSGS ) ) {
	lerror( "message buffering scheme flakey", CCLOSE | FATAL | ERRMSG );
	return;
    }

    if ( msg2totals[ msgndx ] < MAX2BUF ) {
	strncpy( record.name, symptr->name, LCHNM );

	switch( msg2type[ msgndx ] ) {

	    case ND2FNLN:
		record.number = digit;
		/* no break */

	    case NM2FNLN:
		record.file2 = cfno;
		record.line2 = r.l.fline;
		/* no break */

	    case NMFNLN:
		record.file1 = symptr->fno;
		record.line1 = symptr->fline;
		break;

	    default:
		break;
	}

	if ( fseek( ctmpfile, nextslot( msgndx ), 0 ) == OKFSEEK ) {
	    if ( fwrite( (char *) &record, C2RECSZ, 1, ctmpfile ) != 1 ) {
		lerror( "cannot write to message buffer file", CCLOSE | FATAL |
			 ERRMSG );
		return;
	    }
	} else {
	    lerror( "cannot seek in message buffer file", CCLOSE | FATAL |
		     ERRMSG );
	    return;
	}
    }

    ++msg2totals[ msgndx ];
    return;
}


un2buffer( )
{
    extern		fclose( ),
			printf( ),
			unlink( );
    extern FILE		*fopen( );
    extern int		fseek( ),
			fread( );
    extern		lerror( );

    extern char		*msg2text[ ];
    extern short	msg2type[ ];
    extern char		fnm[ ][ LFNM ];

    int		i,
		j,
		stop;
    int		toggle;
    enum boolean	codeflag;
    C2RECORD	record;

    fclose( ctmpfile );
    if ( (ctmpfile = fopen( ctmpname, "r" )) == NULL ) {
	lerror( "cannot open message buffer file for reading", CCLOSE | FATAL |
		 ERRMSG );
	return;
    }

    codeflag = false;

    for ( i = 0; i < ( NUM2MSGS - 1 ); ++i ) {
	if ( msg2totals[ i ] != 0 ) {
	    if ( codeflag == false ) {
		printf( "\n\n==============\n" );
		codeflag = true;

	    }
	    toggle = 0;

	    if ( fseek( ctmpfile, (PERC2SZ * i), 0 ) != OKFSEEK ) {
		lerror( "cannot seek in message buffer file", CCLOSE | FATAL |
			 ERRMSG );
		return;
	    }
	    stop = msg2totals[ i ];
	    if ( stop > MAX2BUF ) {
		stop = MAX2BUF;
	    }

	    printf( "%s\n", msg2text[ i ] );
	    for ( j = 0; j < stop; ++j ) {
		if ( fread( (char *) &record, C2RECSZ, 1, ctmpfile ) != 1 ) {
		    lerror( "cannot read message buffer file", CCLOSE | FATAL |
			     ERRMSG );
		    return;
		}
		switch( msg2type[ i ] ) {

		    case NM2FNLN:
			printf( "    %.8s   \t%.14s(%d) :: %.14s(%d)\n",
				record.name, fnm[ record.file1 ], record.line1,
				fnm[ record.file2 ], record.line2 );
			break;

		    case NMFNLN:
			printf( "    %.8s   \t%.14s(%d)\n", record.name,
				fnm[ record.file1 ], record.line1 );
			break;

		    case NMONLY:
			printf( "    %.8s", record.name );
			if ( ++toggle == 3 ) {
			    printf( "\n" );
			    toggle = 0;
			} else {
			    printf( "\t" );
			}
			break;

		    case ND2FNLN:
			printf( "    %.8s( arg %d )   \t%.14s(%d) :: %.14s(%d)\n",
			    record.name, record.number, fnm[ record.file1 ],
			    record.line1, fnm[ record.file2 ], record.line2 );
			break;

		    default:
			break;

		}
	    }
	    if ( toggle != 0 ) {
		printf( "\n" );
	    }
	    if ( stop < msg2totals[ i ] ) {
		printf( "    %d messages suppressed for lack of space\n",
			 msg2totals[ i ] - stop );
	    }
	}
    }

    fclose( ctmpfile );
    unlink( ctmpname );
    return;
}
