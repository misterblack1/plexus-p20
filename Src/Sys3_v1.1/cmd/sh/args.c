#
/*
 * UNIX shell
 *
 * S. R. Bourne
 * Bell Telephone Laboratories
 *
 */

#include	"defs.h"

PROC STRING *copyargs();
LOCAL DOLPTR	dolh;

CHAR	flagadr[10];

CHAR	flagchar[] = {
	'x',	'n',	'v',	't', STDFLG,	'i',	'e',	'r',	'k',	'u',	0
};
INT	flagval[]  = {
	execpr,	noexec,	readpr,	oneflg,	stdflg,	intflg,	errflg,	rshflg,	keyflg,	setflg,	0
};

/* ========	option handling	======== */


INT	options(argc,argv)
	STRING		*argv;
	INT		argc;
{
	REG STRING	cp;
	REG STRING	*argp=argv;
	REG STRING	flagc;
	STRING		flagp;

	IF argc>1 ANDF *argp[1]=='-'
	THEN
		IF argp[1][1] == '-'
		THEN	/* if first argument is "--" then options are not
			   to be changed	Fix for problems getting 
			   $1 starting with a "-"
			*/
			argp[1] = argp[0]; argc--;
			return(argc);
		FI
		cp = argp[1];
		IF cp[1] == '\0' THEN flags &= ~(execpr|readpr) FI
		/* Step along 'flagchar[]' looking for matches.
		   'sicr' are not legal with 'set' command.
		*/
		WHILE *++cp
		DO	flagc=flagchar;

			WHILE *flagc ANDF *flagc != *cp DO flagc++ OD
			IF *cp == *flagc
			THEN	IF eq(argv[0], "set") ANDF any(*cp, "sicr")
				THEN failed(argv[1], badopt);
				ELSE flags |= flagval[flagc-flagchar];
				FI
			ELIF *cp=='c' ANDF argc>2 ANDF comdiv==0
			THEN	comdiv=argp[2];
				argp[1]=argp[0]; argp++; argc--;
			ELSE	failed(argv[1],badopt);
			FI
		OD
		argp[1]=argp[0]; argc--;
	ELIF	argc >1 ANDF *argp[1]=='+' /*	unset flags x, k, t, n, v, e, u	*/
	THEN	cp = argp[1];
		WHILE *++cp
		DO
			flagc = flagchar;
			WHILE *flagc ANDF *flagc != *cp DO flagc++ OD
				/*	step through flags	*/

			IF !any(*cp, "csir") ANDF *cp == *flagc
			THEN
				IF (flags&flagval[flagc-flagchar])
				/*	only turn off if already on	*/
				THEN
					flags &= ~(flagval[flagc-flagchar])
				FI
			FI
		OD
		argp[1]=argp[0]; argc--;
	FI
	/* set up $- */
/*
	IF argc < 2 THEN flags |= stdflg FI
*/
	flagc=flagchar;
	flagp=flagadr;
	WHILE *flagc
	DO IF flags&flagval[flagc-flagchar]
	   THEN *flagp++ = *flagc;
	   FI
	   flagc++;
	OD
	*flagp++=0;

	return(argc);
}

VOID	setargs(argi)
	STRING		argi[];

	/*	sets up positional parameters 	*/
{
	/* count args */
	REG STRING	*argp=argi;
	REG INT		argn=0;

	WHILE Rcheat(*argp++) != ENDARGS DO argn++ OD

	/* free old ones unless on for loop chain */
	freeargs(dolh);
	dolh=(DOLPTR)(copyargs(argi, argn));
	assnum(&dolladr,dolc=argn-1);
}

freeargs(blk)
	DOLPTR		blk;
{
	REG STRING	*argp;
	REG DOLPTR	argr=0;
	REG DOLPTR	argblk;

	IF argblk=blk
	THEN	argr = argblk->dolnxt;
		IF (--argblk->doluse)==0
		THEN	FOR argp=(STRING *)(argblk->dolarg); Rcheat(*argp)!=ENDARGS; argp++
			DO free(*argp) OD
			free(argblk);
		FI
	FI
	return((int) argr);
}

LOCAL STRING *	copyargs(from, n)
	STRING		from[];
{
	REG STRING *	np=(STRING *) alloc(sizeof(STRING*)*n+3*BYTESPERWORD);
	REG STRING *	fp=from;
	REG STRING *	pp=np;

	((DOLPTR) np)->doluse=1;	/* use count */
	np=(STRING *) ((DOLPTR) np)->dolarg;
	dolv=np;

	WHILE n--
	DO *np++ = make(*fp++) OD
	*np++ = ENDARGS;
	return(pp);
}

clearup()
{
	/* force `for' $* lists to go away */
	WHILE argfor=(DOLPTR) freeargs(argfor) DONE

	/* clean up io files */
	WHILE pop() DONE
}

DOLPTR	useargs()
{
	IF dolh
	THEN	dolh->doluse++;
		dolh->dolnxt=argfor;
		return(argfor=dolh);
	ELSE	return(0);
	FI
}
