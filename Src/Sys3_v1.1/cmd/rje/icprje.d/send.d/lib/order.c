int *order(ss,sx)
int *ss,*sx;
{
	register int t,*p,*q;
	int rt,*rp;
	for (p=ss;p<sx;)
	{
		for (q=p;q<sx;q++)
		{
			if ((t=(*p))>*q) {
				*p=(*q); 
				*q=t; 
				break;
			};
		};
		if (q>=sx) p++;
	};
	rp=ss; 
	rt=0;
	p=0; 
	q=ss;
	while (p<q)
	{
		p=q; 
		t=(*p);
		while (*q==t)
		{
			if (++t>999) t=0;
			if (++q>=sx) q=ss;
		};
		if ((t -= (*p))<0) t += 1000;
		if (t>rt) {
			rt=t; 
			rp=p;
		};
	};
	return (rp);
};
