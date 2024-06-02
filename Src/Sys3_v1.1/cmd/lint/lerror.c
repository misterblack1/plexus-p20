# include	<stdio.h>
# include	"messages.h"
# include	"lerror.h"
# include	<signal.h>

extern int	lineno;
extern char	ftitle[ ];

/*  iscfile( name )
 *
 *  compares name with sourcename (file name from command line)
 *  if it is the same then
 *    if fileflag is false then print the source file name as a title
 *    return true
 *
 *  otherwise
 *    return false
 */

static enum boolean	fileflag = false;

enum boolean
iscfile( name )

char	*name;

{
    extern int	strncmp( );
    extern	fprintf( );
    extern char	sourcename[ ];

    if ( !strncmp( name, sourcename, LFNM ) ) {
	if ( fileflag == false ) {
	    fileflag = true;
	    fprintf( stderr, "\n%.14s\n==============\n", name );
	}
	return( true );

    } else {
	return( false );
    }
}


/*  tmpopen( )
 *
 *  open source message buffer file for writing
 *  open header message file for updating
 *    if open fails, open it for writing
 *  otherwise
 *    initialize header file name and count list from header message file
 *
 *  if opens succeed return success
 *  otherwise return failure
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
    extern	rewind( );
    extern int	fread( ),
		fwrite( );

    extern	lerror( ),
		catchsig( );

    sprintf( ctmpname, "%s/clint%d", TMPDIR, getpid( ) );

    catchsig( );
    if ( (ctmpfile = fopen( ctmpname, "w" )) == NULL ) {
	lerror( "cannot open message buffer file", ERRMSG | FATAL );
	return;
    }

    if ( (htmpfile = fopen( htmpname, "r+" )) == NULL ) {
	/* the file does not exist -- create it */
	if ( (htmpfile = fopen( htmpname, "w" )) == NULL ) {
	    lerror( "cannot open header message buffer file", CCLOSE | FATAL |
		     ERRMSG );
	    return;
	} else if (fwrite((char *) hdrlist, sizeof(HDRITEM), NUMHDRS, htmpfile)
		!= NUMHDRS ) {
	    lerror("cannot write header message buffer file", HCLOSE | CCLOSE |
		    ERRMSG | FATAL );
	    return;
	}
	/* seek past header list for writing header information */
	if ( fseek( htmpfile, (long) sizeof ( hdrlist ), 0 ) != OKFSEEK ) {
	    lerror( "cannot seek in header message buffer file", CCLOSE | HCLOSE
		    | FATAL | ERRMSG );
	    return;
	}
    } else {
	/* the file already exists -- initialize header information */
	rewind( htmpfile );
	if ( fread((char *) hdrlist, sizeof( HDRITEM ), NUMHDRS, htmpfile) != NUMHDRS ) {
	    lerror( "cannot read header message buffer file", CCLOSE | HCLOSE |
		    FATAL | ERRMSG );
	    return;
	}
	if ( fseek( htmpfile, 0L, 2 ) != OKFSEEK ) {
	    lerror( "cannot seek in header message buffer file", CCLOSE | HCLOSE
		    | FATAL | ERRMSG );
	    return;
	}
    }
    return;
}


/*  hdrclose( )
 *
 *  write header file name/count list to and close header message buffer file
 */

hdrclose( )
{
    extern		rewind( );
    extern int		fwrite( );
    extern		lerror( );

    rewind( htmpfile );
    if ( fwrite( (char *) hdrlist, sizeof( HDRITEM ), NUMHDRS, htmpfile ) != NUMHDRS ) {
	lerror( "cannot write header message buffer file", HCLOSE | ERRMSG );
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
	fprintf( stderr, "lint error: %s\n", message );
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


/*  lwerror( )
 *
 *  determines whether a particular message is to be buffered
 *  and if so, calls the appropriate buffer routine
 *    bufhdr( ) for a header file
 *    bufsource( ) for a source file
 *
 *  if not, calls werror( )
 */

/* VARARGS1 */
lwerror( msgndx, arg1, arg2 )

int	msgndx;

{
    extern char		*strip( );
    extern		werror( ),
			bufsource( ),
			bufhdr( );
    extern enum boolean	iscfile( );

    extern char		*msgtext[ ];
    extern short		msgbuf[ ];

    char		*filename;

    filename = strip( ftitle );

    if ( iscfile( filename ) == true ) {
	if ( msgbuf[ msgndx ] == 0 ) {
	    werror( msgtext[ msgndx ], arg1, arg2 );
	} else {
	    bufsource( WERRTY, msgndx, arg1, arg2 );
	}

    } else {
	bufhdr( WERRTY, filename, msgndx, arg1, arg2 );
    }

    return;
}


/* VARARGS1 */
luerror( msgndx, arg1 )

short	msgndx;

{
    extern char		*strip( );
    extern		uerror( ),
			bufsource( ),
			bufhdr( );
    extern enum boolean	iscfile( );

    extern char		*msgtext[ ];
    extern short		msgbuf[ ];

    char		*filename;

    filename = strip( ftitle );

    if ( iscfile( filename ) == true ) {
	if ( msgbuf[ msgndx ] == 0 ) {
	    uerror( msgtext[ msgndx ], arg1 );
	} else {
	    bufsource( UERRTY, msgndx, arg1 );
	}

    } else {
	bufhdr( UERRTY, filename, msgndx, arg1 );
    }

    return;
}


# define nextslot(x)	((PERMSG * ((x) - 1)) + (CRECSZ * msgtotals[(x)]))
static int	msgtotals[ NUMBUF ];

/* VARARGS2 */
bufsource( code, msgndx, arg1, arg2 )

int	code,
	msgndx;

{
    extern char		*strncpy( );
    extern int		fseek( ),
			fwrite( );
    extern		lerror( );

    extern short	msgbuf[ ],
		msgtype[ ];

    int		bufndx;
    CRECORD	record;

    bufndx = msgbuf[ msgndx ];
    if (( bufndx == 0 ) || ( bufndx >= NUMBUF )) {
	lerror( "message buffering scheme flakey", CCLOSE | HCLOSE | FATAL |
		ERRMSG );
    } else {

	if ( msgtotals[ bufndx ] < MAXBUF ) {
	    record.code = code | msgtype[ msgndx ];
	    record.lineno = lineno;

	    switch( msgtype[ msgndx ]  & ~SIMPL ) {

		case DBLSTRTY:
		    strncpy( record.name2, (char *) arg2, LCHNM );
		    /* no break */

		case STRINGTY:
		    strncpy( record.arg1.name1, (char *) arg1, LCHNM );
		    break;

		case CHARTY:
		    record.arg1.char1 = (char) arg1;
		    break;

		case NUMTY:
		    record.arg1.number = (int) arg1;
		    break;

		default:
		    break;

	    }

	    if ( fseek( ctmpfile, nextslot( bufndx ), 0 ) == OKFSEEK ) {
		if ( fwrite( (char *) &record, CRECSZ, 1, ctmpfile ) != 1 ) {
		    lerror( "cannot write to message buffer file", CCLOSE |
			    HCLOSE | FATAL | ERRMSG );
		}
	    } else {
		lerror( "cannot seek in message buffer file", CCLOSE | HCLOSE |
			FATAL | ERRMSG );
	    }
	}
	++msgtotals[ bufndx ];
    }

    return;
}


static int		curhdr = 0;
static enum boolean	activehdr = false;

/* VARARGS3 */
bufhdr( code, filename, msgndx, arg1, arg2 )

int	code;
char	*filename;
int	msgndx;

{
    extern char		*strncpy( );
    extern int		strncmp( ),
			fwrite( );
    extern		lerror( );

    extern char			sourcename[ ];
    extern short		msgtype[ ];

    int		i,
		emptyslot;
    HRECORD	record;

    if ((activehdr == false ) ||
	( strncmp( hdrlist[ curhdr ].hname, filename, LFNM ) != 0 )) {
    /* that is, if we do not have a new (active) header file
     * or if this header file is not the same as the last one
     * see if we have already seen it
     */

	activehdr = false;
	i = curhdr;
	emptyslot = curhdr;

	while( strncmp( hdrlist[ i ].hname, filename, LFNM ) != 0 ) {
	/* that is, while we haven't found a match on the filename */
	    if ( hdrlist[ i ].hname[ 0 ] == '\0' ) {
		emptyslot = i;
		i = 0;
	    } else {
		++i;
		if ( i == NUMHDRS ) {
		    i = 0;
		}
	    }
	    if ( i == curhdr ) {
		if ( hdrlist[ emptyslot ].hname[ 0 ] != '\0' ) {
		    lerror( "too many header files", ERRMSG );
		    return;
		} else {
		    activehdr = true;
		    strncpy( hdrlist[ emptyslot ].hname, filename, LFNM );
		    strncpy( hdrlist[ emptyslot ].sname, sourcename, LFNM );
		    i = emptyslot;
		    curhdr = emptyslot;
		}
	    }
	}
	if ( activehdr == false ) {
	    return;
	}
    }

    /* activehdr is true, curhdr points to current header file name, buffer */
    ++hdrlist[ curhdr ].hcount;
    record.msgndx = msgndx;
    record.code = code | msgtype[ msgndx ];
    record.lineno = lineno;

    switch( msgtype[ msgndx ]  & ~SIMPL ) {

	case DBLSTRTY:
	    strncpy( record.name2, (char *) arg2, LCHNM );
	    /* no break */

	case STRINGTY:
	    strncpy( record.arg1.name1, (char *) arg1, LCHNM );
	    break;

	case CHARTY:
	    record.arg1.char1 = (char) arg1;
	    break;

	case NUMTY:
	    record.arg1.number = (int) arg1;
	    break;

	default:
	    break;

    }

    if ( fwrite( (char *) &record, HRECSZ, 1, htmpfile ) != 1 ) {
	lerror( "cannot write to header message buffer file", CCLOSE | HCLOSE |
		FATAL | ERRMSG );
    }

    return;
}


/* unbuffer( )
 *
 * writes out information saved in ctmpfile
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

    extern char		*outmsg[ ],
			*outformat[ ];

    int		i,
		j,
		stop;
    int		perline,
		toggle;
    enum boolean	codeflag;
    CRECORD	record;

    fclose( ctmpfile );
    if ( (ctmpfile = fopen( ctmpname, "r" )) == NULL ) {
	lerror( "cannot open source buffer file for reading", CCLOSE | FATAL |
		 ERRMSG );
	return;
    }

    for ( i = 1; i < NUMBUF; ++i ) {
	if ( msgtotals[ i ] != 0 ) {
	    codeflag = false;

	    if ( fseek( ctmpfile, (PERMSG * (i - 1)), 0 ) != OKFSEEK ) {
		lerror( "cannot seek in source message buffer file", CCLOSE |
			FATAL | ERRMSG );
		return;
	    }
	    stop = msgtotals[ i ];
	    if ( stop > MAXBUF ) {
		stop = MAXBUF;
	    }
	    for ( j = 0; j < stop; ++j ) {
		if ( fread( (char *) &record, CRECSZ, 1, ctmpfile ) != 1 ) {
		    lerror( "cannot read source message buffer file", CCLOSE |
			    FATAL | ERRMSG );
		    return;
		}

		if ( codeflag == false ) {
		    if ( record.code & WERRTY ) {
			fprintf( stderr, "warning: " );
		    }
		    perline = 1;
		    toggle = 0;
		    if ( record.code & SIMPL ) {
			perline = 2;
		    } else if ( !( record.code & ~WERRTY ) ) {
			/* PLAINTY */
			perline = 3;
		    }
		    fprintf( stderr, "%s\n", outmsg[ i ] );
		    codeflag = true;
		}
		fprintf( stderr, "    (%d)  ", record.lineno );
		switch( record.code & ~( WERRTY | SIMPL ) ) {

		    case DBLSTRTY:
			fprintf( stderr, outformat[ i ], record.arg1.name1,
				 record.name2 );
			break;

		    case STRINGTY:
			fprintf( stderr, outformat[ i ], record.arg1.name1 );
			break;

		    case CHARTY:
			fprintf( stderr, outformat[ i ], record.arg1.char1 );
			break;

		    case NUMTY:
			fprintf( stderr, outformat[ i ], record.arg1.number );
			break;

		    default:
			fprintf( stderr, outformat[ i ] );
			break;

		}
		if ( ++toggle == perline ) {
		    fprintf( stderr, "\n" );
		    toggle = 0;
		} else {
		    fprintf( stderr, "\t" );
		}
	    }
	    if ( toggle != 0 ) {
		fprintf( stderr, "\n" );
	    }
	    if ( stop < msgtotals[ i ] ) {
		fprintf( stderr, "    %d messages suppressed for lack of space\n",
			 msgtotals[ i ] - stop );
	    }
	}
    }

    fclose( ctmpfile );
    unlink( ctmpname );
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
    /* note that no error message is printed */

}
