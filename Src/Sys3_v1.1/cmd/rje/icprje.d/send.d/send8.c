#
/* send8.c */


/* This information is proprietary and is the property of Bell
   Telephone Laboratories, Incorporated.  Its reproduction or
   disclosure to others, either orally or in writing, is pro-
   hibited without written permission of Bell Laboratories. */


#define IDMOD send8

#define IDSTR "~|^`send8.c 1.11 3/30/76

#include "send.h"

char *stab[] = {
	2,"a2","1,10,16,40,72",
	2,"c2","1,6,10,14,49",
	2,"c3","1,6,10,14,18,22,26,30,34,38,42,46,50,54,58,62,67",
	1,"a","1,10,16,36,72",
	1,"c","1,8,12,16,20,55",
	1,"f","1,7,11,15,19,23",
	1,"p","1,5,9,13,17,21,25,29,33,37,41,45,49,53,57,61",
	1,"s","1,10,55",
	1,"u","1,12,20,44",
	0,0,0};

stfm(fmt)
struct format *fmt;
{
	register struct format *f;
	f=fmt;
	f->ffd=f->ffe=f->ffm=0;
	f->ffs=LNL-1;
	f->fft=(-8);
};

exfm(n,s,fmt)
char *s;
struct format *fmt;
{
	int m,*tp;
	char *px,*p0,*p1;
	register int i;
	register char *p;
	register struct format *f;
	px=(p=s)+n;
	p0=p1=0;
	for (;p<px;p++)
	{
		if (*p=='<' && *(p+1)==':')
		{
			p0=p+2; 
			p++; 
			continue;
		};
		if (*p==':' && *(p+1)=='>')
		{
			if (p0) {
				p1=p++; 
				break;
			};
		};
	};
	if (p>=px || p1==0) return (0);
	stfm(f=fmt);
	for (p=p0;p<p1;)
		switch (*p++)
		{
		default: 
			goto BAD;
		case ' ': 
			continue;
		case 'd': 
			f->ffd=1; 
			continue;
		case 'e': 
			f->ffe=1; 
			continue;
		case 'm': 
			p=(char *)exnm(p,&f->ffm); 
			continue;
		case 's': 
			p=(char *)exnm(p,&f->ffs); 
			continue;
		case 't':
			if (*p!='-')
			{
				if (p=(char *)tlst(f,p)) continue;
				goto BAD;
			};
			if ((i=(*++p))<'0' || i>'9')
			{
				for (tp=(int *)stab;i=(*tp);tp += 3)
				{
					if (mtch(i,*(tp+1),p)) break;
				};
				if (i==0) goto BAD;
				tlst(f,*(tp+2));
				p += i; 
				continue;
			};
			p=(char *)exnm(p,&m);
			i=m;
			if (i==8 || i==4 || i==2 || i==1)
			{
				f->fft=(-i); 
				continue;
			};
			for (i=0;i<NTB;i++) f->ftab[i]=i*m;
			f->fft=i; 
			continue;
		};
	if (f->ffs==(LNL-1)) f->ffs -= f->ffm;
	if (f->ffs>0 && f->ffm>=0 && (f->ffs+f->ffm)<LNL) return (1);
BAD: 
	stfm(f); 
	return (-1);
};

exnm(s,xp)
char *s;
int *xp;
{
	register int c,n;
	register char *p;
	p=s;
	for (n=0;(c=(*p)-'0')>=0 && c<=9;p++) n=10*n+c;
	*xp=n;
	return ((int)p);
};

tlst(fmt,s)
struct format *fmt;
char *s;
{
	int m;
	register int i;
	register char *p,*q;
	i=0;
	q=fmt->ftab;
	for (p=s;;p++)
	{
		p=(char *)exnm(p,&m);
		if (m>1)
		{
			if (i>=NTB) return (0);
			i++; 
			*q++=m-1;
		};
		if (*p!=',') break;
	};
	fmt->fft=i;
	return ((int)p);
};

cpfm(fm0,fm1)
struct format *fm0,*fm1;
{
	register int i;
	register struct format *f0,*f1;
	f0=fm0; 
	f1=fm1;
	f1->ffd=f0->ffd;
	f1->ffe=f0->ffe;
	f1->ffm=f0->ffm;
	f1->ffs=f0->ffs;
	f1->fft=f0->fft;
	for (i=0;i<NTB;i++) f1->ftab[i]=f0->ftab[i];
};

/*end*/
