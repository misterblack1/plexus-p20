/*LINTLIBRARY*/
#include <stdio.h>

char *ctermid(s)
	char *s;
{
	static char res[L_ctermid];
	register char *r;

	r = s==NULL? res: s;
	strcpy (r, "/dev/tty");
	return r;
}
