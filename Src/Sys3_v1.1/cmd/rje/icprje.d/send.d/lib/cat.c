char	*
cat(buf,s1,s2)
char	*buf,*s1,*s2;
{
	register char	*p,*q;
	p = buf;
	q = s1;
	while (*p++=(*q++));
	p--;
	q = s2;
	while (*p++=(*q++));
	p--;
	*p=0;
	return (p);
};
