#
/*
 * UNIX shell
 *
 * S. R. Bourne
 * Bell Telephone Laboratories
 *
 */

#include	"defs.h"
#include	"sym.h"
#include	"timeout.h"
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<sgtty.h>
#include	"dup.h"

UFD		output = 2;
LOCAL BOOL	beenhere = FALSE;
CHAR		tmpout[20] = "/tmp/sh-";
FILEBLK		stdfile;
FILE		standin = &stdfile;
#include	<execargs.h>

PROC VOID	exfile();




main(c, v, e)
	INT		c;
	STRING		v[];
	STRING		e[];
{
	REG INT		rflag=ttyflg;
	INT		rsflag=1;	/* local restricted flag */
	STRING sim;

	/* initialise storage allocation */
	stdsigs();

	setbrk(BRKINCR);

	addblok((POS)0);


	/* set names from userenv */
	/*  'rsflag' is non-zero if SHELL variable is
	    set in environment and contains an'r' in
	    the simple file part of the value.	*/

	rsflag=getenv();

	/* a shell is also restricted if argv(0) has
	   an 'r' in its simple name
	*/
#ifndef RES /* restricted shell code */
	IF c>0 ANDF any('r', sim=(STRING) simple(*v)) THEN rflag=0 FI
#endif

	/* look for options */
	/* dolc is $#	*/
	dolc=options(c,v);

        IF dolc < 2 THEN flags |= stdflg;
                        BEGIN REG CHAR *flagc = flagadr;
                        WHILE *flagc DO flagc++ OD
                        *flagc = STDFLG;
                        END
        FI
	IF (flags&stdflg)==0
	THEN	dolc--;
	FI
	dolv=v+c-dolc; dolc--;

	/* return here for shell file execution */
	/* but not for parenthesis subshells	*/
	setjmp(subshell);


	/* number of positional parameters */
	assnum(&dolladr, dolc);	/* $# */
	cmdadr=dolv[0]; /* cmdadr is $0 */


	/* set pidname '$$' */
	assnum(&pidadr, getpid());


	/* set up temp file names */
	settmp();


	/* default internal field separators - $IFS */
	dfault(&ifsnod, sptbnl);


	IF (beenhere++)==FALSE
	THEN	/* ? profile */
		IF *(sim=(STRING) simple(cmdadr)) == '-'
		THEN	/*	system profile	*/
#ifndef RES
			IF (input=pathopen(nullstr, sysprofile)) >=0
			THEN 	exfile(rflag);	/* file exists */
			FI
#endif
			IF	(input=pathopen(nullstr, profile)) >=0
			THEN	exfile(rflag); flags &= ~ttyflg;
			FI
		FI
		IF rsflag==0 ORF rflag==0 THEN flags |= rshflg FI

		/* open input file if specified */
		IF comdiv
		THEN	estabf(comdiv); input = -1;

		ELSE	input=((flags&stdflg) ? 0 : chkopen(cmdadr));

			comdiv--;
		FI
	ELSE	*execargs=(STRING) dolv;	/* for `ps' cmd */
	FI

	exfile(0);

	done();

}

LOCAL VOID	exfile(prof)
BOOL		prof;
{
	L_INT		mailtime = 0;	/* Must not be a register variable */
	REG INT		userid;
	struct stat	statb;

	/* move input */

	IF input>0
	THEN	Ldup(input,INIO);


		input=INIO;

	FI

	/* move output to safe place */
	IF output==2
	THEN	Ldup(dup(2),OTIO);

		output=OTIO;

	FI

	userid=geteuid();

	/* decide whether interactive */
	IF (flags&intflg)
	  ORF    ((flags&oneflg)==0
	    ANDF gtty(output,&statb)==0
	    ANDF gtty(input,&statb)==0)
	THEN	dfault(&ps1nod, (userid?stdprompt:supprompt));
		dfault(&ps2nod, readmsg);
		flags |= ttyflg|prompt; ignsig(KILL);
	ELSE	flags |= prof; flags &= ~prompt;
	FI

	IF setjmp(errshell) ANDF prof
	THEN	close(input); return;
	FI

	/* error return here */
	loopcnt=breakcnt=peekc=0; iopend=0;
	IF input>=0 THEN initf(input) FI

	/* command loop */
	LOOP	tdystak(0);
		stakchk(); /* may reduce sbrk */
		exitset();
		IF (flags&prompt) ANDF standin->fstak==0 ANDF !eof
		THEN	IF mailnod.namval
			THEN	IF stat(mailnod.namval,&statb)>=0
				THEN	IF statb.st_size
					    ANDF mailtime
					    ANDF statb.st_mtime != mailtime
					THEN	prs(mailmsg)
					FI
					mailtime=statb.st_mtime;
				ELIF mailtime==0
				THEN	mailtime=1
				FI
			FI
			prs(ps1nod.namval); alarm(TIMEOUT); flags |= waiting;
		FI

		trapnote=0; peekc=readc();
		IF eof
		THEN	return;
		FI
		alarm(0); flags &= ~waiting;

		execute(cmd(NL,MTFLG),0);
		eof |= (flags&oneflg);
	POOL
}

chkpr(eor)
char eor;
{
	IF (flags&prompt) ANDF standin->fstak==0 ANDF eor==NL
	THEN	prs(ps2nod.namval);
	FI
}

settmp()
{
	itos(getpid()); serial=0;
	tmpnam=movstr(numbuf,&tmpout[TMPNAM]);
}

Ldup(fa, fb)
	REG INT		fa, fb;
{
#ifdef RES
	dup(fa|DUPFLG, fb);
	close(fa);
	ioctl(fb, FIOCLEX, 0);
#else	/*	TS lacks two-arg dup, ioctl	*/
	IF fa >= 0
	THEN	close(fb);
		fcntl(fa,0,fb); /* normal dup */
		close(fa);
		fcntl(fb,2,1)	/* autoclose for fb */
	FI
#endif
}
