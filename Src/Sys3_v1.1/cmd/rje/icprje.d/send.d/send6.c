#
/* send6.c */


/* This information is proprietary and is the property of Bell
   Telephone Laboratories, Incorporated.  Its reproduction or
   disclosure to others, either orally or in writing, is pro-
   hibited without written permission of Bell Laboratories. */


#define IDMOD send6

#define IDSTR "~|^`send6.c 1.8 4/22/76

#include "send.h"

tab(n,s)
char *s;
{
	int d;
	char *px;
	register int t;
	register char *p,*q;
	px=(p=s)+n;
	t=0;
	for (q=s;p<px;p++)
	{
		if (*p==' ') {
			t++; 
			continue;
		};
		if (t>0)
		{
			while (t>1)
			{
				d=8-(7&((p-t)-s));
				if (t<d) break;
				*q++=(d==1? ' ':'\t');
				t -= d;
			};
			for (;t>0;t--) *q++=' ';
		};
		*q++=(*p);
	};
	return (q-s);
};

index(kw0,kw1)
struct keywd *kw0,*kw1;
{
	register int i;
	register struct keywd *pk;
	for (i=0;i<256;i++) kwx[i]=0;
	for (pk=kw0;pk<kw1;pk=pk->nx)
	if (pk->nk>0) {
		i = *(pk->kp);
		pk->lk = kwx[i];
		kwx[i] = pk;
	}
}

fold(f)
{
	register int i,m;
	if (code==BCD)
	{
		m=(f? 0277:0377);
		for (i='a';i<='z';i++)
			trt[i]=m&trt[0137&i];
	};
	if (code==ASC)
	{
		m=(f? 0177:0137);
		for (i=0140;i<=0176;i++)
			trt[i]=m&i;
	};
	if (code==SIG)
	{
		m=(f? 0177:0137);
		for (i=0140;i<=0176;i++)
			trt[i]=m&i;
		trt['!'] = ']';
		trt['|'] = '!';
		trt['~'] = '^';
		trt['`'] = '[';
		trt['^'] = trt[']'] = trt['['] = trt['}'] = trt['{'] = trt['\\'] = 0;
	}
}

ascii(usflg) int usflg;
{
	register int i,m;
	if (usflg) code = SIG;
	else code = ASC;
	m=(trt['A']==trt['a']);
	for (i=0;i<256;i++)
	{
		if (trt[i]) trt[i]=i;
	};
	if (m) fold(0);
	for (i=0;i<256;i++) rtr[i]=i;
	if (usflg==1) {
		rtr[']'] = '!';
		rtr['!'] = '|';
		rtr['^'] = '~';
		rtr['['] = '`';
		code = ASC;
	}
}

trace(ctx,c)
struct context *ctx;
{
	register int i;
	i=ctx->lvl;
	if (i<0 || !ctx->flg[F't']) return;
	prc(c);
	while (--i>=0) prc(' ');
	prf("%s\n",ctx->nam);
};

shex(ctx)
struct context *ctx;
{
	int m;
	register int i;
	i=(wait(&m)!=(-1)? m:0177400);
	if (i==0) return (0);
	prf("%qcode %d",ss,0377&(i>>8));
	if (i &= 0377) prf("%qtype %d",ss,i);
	return (diag(ctx,"shell exit",ss,592));
};

diag(ctx,s,t,n)
struct context *ctx;
char *s,*t;
{
	if (ctx->flg[F'y']) return (0);
	error=1;
	prf("Line %d of %s, %s",ctx->lno,ctx->nam,s);
	if (t) prf(" %s",t);
	prf(".\n");
	return (1);
};

mktmp(s,n)
char *s;
{
	register int m;
	register char *p,*q;
	for (q=s;*q;q++);
	p=(char *)prf("%q%d",q,getpid());
	if (n) prf("%q.%d",p,n);
	if ((m=creat(p=s,0444))<0)
	{
		unlink(p);
		if ((m=creat(p,0444))<0) unlink(p);
	};
	if (m<0) *q=0;
	return (m);
};

tfork()
{
	register int i,t;
	for (t=0;t<25;t++)
	{
		if ((i=fork())!=(-1)) return (i);
		sleep(2);
	};
	return (-1);
};

xpipe(v)
int v[];
{
	return (pipe(v));
};

/*end*/
