static char Sccsid[]="@(#)dname	3.1";
# include	"sys/types.h"
# include	"macros.h"

/*
	Returns directory name containing a file
	(by modifying its argument).
	Returns "." if current
	directory; handles root correctly.
	Returns its argument.
	Bugs: doesn't handle null strings correctly.
*/

char *dname(p)
char *p;
{
	register char *c;
	register int s;

	s = size(p);
	for(c = p+s-2; c > p; c--)
		if(*c == '/') {
			*c = '\0';
			return(p);
		}
	if (p[0] != '/')
		p[0] = '.';
	p[1] = 0;
	return(p);
}
