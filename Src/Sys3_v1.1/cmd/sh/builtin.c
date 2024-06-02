#include        "defs.h"
extern int      exitval;

builtin(xbuiltin, argn, com)

/*
	builtin commands are those that Bourne did not intend
	to be part of his shell.
	Redirection of i/o, or rather the lack of it, is still a
	problem..
*/
register int    argn;
register char   *com[];
{
        switch(xbuiltin) {
                case TEST:      /* test expression */
                        exitval = test(argn,com);
                        break;
        }
}
bfailed(s1, s2, s3) 
	/*	fake diagnostics to continue to look like original
		test(1) diagnostics
	*/
	STRING s1;
	STRING s2;
	STRING s3;
{
	prp(); prs(s1);
	IF s2
	THEN	prs(colon); prs(s2); prs(s3);
	FI
	newline(); exitsh(ERROR);
}
