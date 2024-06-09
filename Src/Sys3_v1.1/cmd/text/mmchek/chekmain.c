	/* This Program checks for many of the errors
	 * which can be committed in PWB/MM input including some tbl & eqn.
	 *
	 * It is intended to be faster to save both user and processor
	 * time.
	 *
	 * Savings of 15:1 have been measured on files of 20,000 char
	 * X 1000 lines with only a few errors to print.
	 *
	 *
	 * It also avoids garbled output from mixing formatted results
	 * with error messages although there are other ways to do this.
	 *
	 * Another advantage is that it makes explicit statements about
	 * mistakes which cause output to be lost.
	 *
	 *
	 * The errors are those recognized from the relevant
	 * documents on the macros (mm 8.2) and Tbl and Eqn.
	 *
	 * In fact the Eqn checking code was lifted directly
	 * from the checkeq program.
	 *
	 * A couple of minor additional eqn checks are made in the
	 * Lex part of this program.
	 *
	 * The remainder of the Lex checks for nesting,
	 * missing, and out of sequence errors of other commonly
	 * used macros.
	 *
	 * There is no claim to completeness and the error messages
	 * are cast differently from those of PWB/MM.
	 *
	 * The need for this program may well diminish with the
	 * error diagnostics accompanying version 2.0.
	 *
	 * Programmed by t. b. merrick - 6/4/79
	 * MV 1F49 X6646 soon ? to be changed.
	 */

char xxxvers[] = "@(#)mmchek.src	1.1";

#include <stdio.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/stat.h>

main(argc, argv) 

char **argv ; 
int argc ;

{
extern int yylineno ;
extern FILE *yyin ;
FILE *fin ;

struct stat stbuf ;
	if (argc == 1)
		{ yyin = stdin;
		  yylineno = 1 ;
		  yylex() ;
		}
	else
	while (--argc > 0) {
		stat(*++argv, &stbuf) ;

		if ( stbuf.st_mode &  S_IFDIR ) {
			continue ;
		}

		if ((fin = fopen(*argv, "r")) == NULL) {
			printf("Can't open %s\n", *argv) ;
			continue ; 
		}
		printf("%s:\n", *argv) ;
		yyin = fin ;
		yylineno = 1 ;
		yylex() ;
		fclose(fin) ;
	}
}
