/*LINTLIBRARY*/
#include <stdio.h>

char *tmpnam(s)
char	*s;
{
	static char str[L_tmpnam];
	static char seed[] = { 'a', 'a', 'a', '\0' };
	register char *p, *q;

	p = s==NULL? str: s;
	strcpy(p, "/usr/tmp/");
	strcat(p, seed);
	strcat(p, "XXXXXX");

	q = seed;
	while(*q == 'z')
		*q++ = 'a';
	++*q;

	mktemp(p);
	return(p);
}
