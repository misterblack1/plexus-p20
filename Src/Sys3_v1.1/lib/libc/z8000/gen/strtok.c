/*LINTLIBRARY*/
/*
 * uses strpbrk and strspn to break string into tokens on
 * sequentially subsequent calls.  returns NULL at end.
 */

#define	NULL	(char *) 0

char *
strtok(string, sepset)
char	*string, *sepset;
{
	int	strspn();
	char	*strpbrk();
	register char	*p, *q, *r;
	static	char	*savept;

	if(string == NULL)
		p = savept;
	else
		p = string;
	if(p == 0)
		return(NULL);
	q = p + strspn(p, sepset);
	if(*q == '\0')
		return(NULL);
	if((r = strpbrk(q, sepset)) == NULL)
		savept = 0;
	else {
		*r = '\0';
		p = r + strspn(r, sepset);
		savept = (p > r)? ++p: ++r;
	}
	return(q);
}
