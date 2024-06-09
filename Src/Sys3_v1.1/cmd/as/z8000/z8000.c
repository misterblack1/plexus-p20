#
/************************************************************************/
/*   This module is the controlling routine for the entire Z8000	*/
/*	assembler.  All symbols that are global to the entire		*/
/*	program are contained in this module.				*/
/************************************************************************/


#include <stdio.h>
#include "common.h"
#include "error.h"
#include "ifile.h"
#include "symtab.h"
#include "token.h"
#include "types.h"

#include "extern1.h"
#include "extern2.h"
#include "extern.h"

char *outfile = "a.out";
char *tmpfile = "/usr/tmp/as????????";
extern catcher ();


/************************************************************************/
/*   This is the main routine for the Z8000 assembler.  It checks the	*/
/*	number of arguments, attempts to open the source file,		*/
/*	scans the arguments, calls the initialization routine,		*/
/*	calls pass1, calls pass2, and then calls terminate.		*/
/************************************************************************/

main (argc, argv)

register int argc;
register char **argv;

{

	abortflag = listopt = margin = narrow = FALSE;
	p1trace = p2trace = split = uflag = verbose = xref = zcc = FALSE;

	if (argc < 2)
		faterror(USAGE);

	if (signal(1,catcher) == 1)
		signal(1,1);
	if (signal(2,catcher) == 1)
		signal(2,1);
	if (signal(3,catcher) == 1)
		signal(3,1);

	while (*++argv != 0) {

		if (**argv == '-') {
			if (*argv[1] == 0) {
				inbuf = stdin;
				listopt = 0;
			}
			while (*(++(*argv))) {

				switch(**argv) {

				case 'a':
					abortflag = TRUE;
					continue;

				case 'f':
					forceobject++;
					continue;

				case 'i':
 					ifile = *++argv;
					if(checkfile(ifile)==FAILURE) {
						ifile = 0;
						faterror(OVERWRIT,*argv);
					}
					if (ifile == 0)
						faterror(USAGE);
					break;

				case 'l':
					listopt = TRUE;
					continue;

				case 'm':
					margin = TRUE;
					continue;

				case 'n':
					narrow = TRUE;
					continue;

				case 'o':
					outfile = *++argv;
					if(checkfile(outfile)==FAILURE) {
						outfile = 0;
						faterror(OVERWRIT,*argv);
					}
					if (outfile == 0)
						faterror(USAGE);
					break;

				case 't':
					p1trace = TRUE;
					p2trace = TRUE;
					continue;

				case 'u':
					uflag = TRUE;
					continue;

				case 'v':
					verbose = TRUE;
					continue;

				case 'x':
					xref = TRUE;
					printf("\nxref not implemented\n");
					continue;

				case 'z':
					zcc = TRUE;
					continue;

				case '1':
					p1trace = TRUE;
					continue;

				case '2':
					p2trace = TRUE;
					continue;


				default:
					break;
				}
			break;
			}
		}
		else {
			srcfile = *argv;

			if ((inbuf = fopen(srcfile,"r")) == NULL)
				faterror(NOOPEN,srcfile);
		}
	}

	if (inbuf == stdin)
		listopt = 0;

	init();

	pass1();

	pass2();

	terminate();

	if (xref == TRUE)
		xreference();

	if (abortflag == TRUE)
		abort();

	if (numerrors)
		exit(1);
	else
		exit(0);
}


/************************************************************************/
/*	check filename, won't allow .c and.s                            */
/************************************************************************/
 
checkfile (fptr)
 
register char *fptr;
{
	do {
	} while (*fptr++ != 0)  ;
	fptr--;
	fptr--;
	fptr--;
	if (*fptr++ == '.')  {
		if ((*fptr=='s')||(*fptr=='c'))
			return(FAILURE);
	}
	return(SUCCESS);
}
 

/************************************************************************/
/*   Faterror reports a fatal error and exits.				*/
/************************************************************************/

faterror (errno, symbol)

int errno;
unsigned symbol;

{
	fprintf(stderr,"as:\t");
	fprintf(stderr,errmsg[errno],symbol);
	catcher();
}


/************************************************************************/
/*   Printdev2 prints the message to the terminal.			*/
/************************************************************************/

printdev2 (a, b, c, d, e, f, g)

int a,b,c,d,e,f,g;

{
	register unsigned out, err;

	out = ttyname(fileno(stdout));
	err = ttyname(fileno(stderr));

	if (out != 0)
		if (!strcmp(out,err) && listopt)
			return;
	fprintf(stderr,a,b,c,d,e,f,g);
}

/************************************************************************/
/*   Putcheck does a putc with a check for out of space.		*/
/************************************************************************/

putcheck (val, fp)

int val;
FILE *fp;
{
	putc (val, fp);
	if (ferror(fp))
		faterror(NOSPACE);
}


/************************************************************************/
/*  Catcher catches the interrupt and quit signals, deletes tmp files	*/
/*	and terminates gracefully.					*/
/************************************************************************/

catcher () {
	*extenptr = IFILE;
	if (ifile) unlink (ifile);
	if (outfile) unlink (outfile);
	*extenptr = DATAFILE;
	unlink (tmpfile);
	*extenptr = TEXTREL;
	unlink (tmpfile);
	*extenptr = DATAREL;
	unlink (tmpfile);

	if (abortflag == TRUE)
		abort();
	else
		exit(2);
}

xreference () {
	;
}

