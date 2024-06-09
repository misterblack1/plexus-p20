#include	"trek.h"

#ifdef TRACE
tTf()
{
	return(0);
}
#endif

bmove(f, t, n)
register char *f, *t;
register int n;
{
	do
		*t++ = *f++;
	while(--n);
	return(t);
}

syserr()
{
	printf("Trek System Error!\n");
	printf("Aborting mission.\n");
	abort();
}

randmsg(s, n)
char *s;
{
	if(!rflg && !ranf(n)) {
		printf("%s\n", s);
		return(1);
	}
	return(0);
}
