char *
scan(s,t)
char *s,*t;
{
	register char c,*p,*q;
	char *p0;
	if ((c=(*s))==0) return (t);
	for (p=t;*p;p++)
	{
		if (*p!=c) continue;
		p0=p; 
		q=s;
		while ((c=(*++q))!=0 && *++p!=0)
		{
			if (*p!=c && c!='.') break;
		};
		if (c==0) return (p0);
		p=p0; 
		c=(*s);
	};
	return ((char *)0);
};
