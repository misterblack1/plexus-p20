mtch(n,s,t)
char *s,*t;
{
	register int i;
	register char *p,*q;
	i=n;
	p=s; 
	q=t;
	while (--i>=0)
	{
		if (*p++!=(*q++)) return (0);
	};
	return (1);
};
