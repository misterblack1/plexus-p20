rjetab(s,l,n,lx,nx)
char *s,*l[],lx[];
{
	register char *p,**q;
	register int t;
	char *px,**qx,f[40];
	cat(f,"/usr/rje/",s);
	t=open(f,0);
	nx=read(t,lx,nx-1);
	close(t);
	if (nx<=0) return (0);
	px=(p=lx)+nx;
	qx=(q=l)+n;
	while (p<px && q<qx)
	{
		*q++=p;
		while (p<px && *p!='\t' && *p!='\n') p++;
		*p++=0;
	};
	return (q-l);
};

