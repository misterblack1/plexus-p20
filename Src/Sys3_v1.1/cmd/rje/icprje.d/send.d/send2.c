#
/* send2.c */


/* This information is proprietary and is the property of Bell
   Telephone Laboratories, Incorporated.  Its reproduction or
   disclosure to others, either orally or in writing, is pro-
   hibited without written permission of Bell Laboratories. */


#define IDMOD send2

#define IDSTR "~|^`send2.c 1.18 4/22/76

#include	"send.h"

struct iobf *ibf0;

char tt[LNX],uu[LNX];

dofile(descr,name,oldctx,ftype)
char *name;
struct context *oldctx;
{
	extern int errno;
	struct iobf *aibf,ibf;
	struct format *afmt,fmt;
	struct format *adfm,dfm;
	struct context *actx,ctx;
	struct trap *atrp,trp;
	int isy,ndo,imd,rmd,eof,err,*aeof;
	struct keywd *pk,*qk;
	int m,n,ns,nt,nu,ntb;
	char *q,*px,*pt,*pt0,*acc,cc[LNX];
	register int i,k;
	register char *p;
	aibf=(&ibf);
	afmt=(&fmt);
	adfm=(&dfm);
	actx=(&ctx);
	atrp=(&trp);
	aeof=(&eof);
	pt0=fmt.ftab;
	acc=cc;
	didf |= 1;
	if (descr<0)
	{
		diag(oldctx,"cannot open source",name,581);
		return (1);
	};
	if (descr==0)
	{
		if (ibf0) bsp0(ibf0); 
		ibf0=0;
	};
	isy=isatty(descr);
	if (ftype==TTTY || ftype==TPTY) isy=1;
	inibf(aibf,descr);
	cpfm(oldctx->dsp,afmt);
	cpfm(afmt,adfm);
	ctx.ocx=oldctx;
	px=(p=ctx.flg)+32;
	q=oldctx->flg;
	while (p<px) *p++=(*q++);
	ctx.dsp=adfm;
	ctx.kw0=oldctx->kw0;
	ctx.kw1=oldctx->kw1;
	ctx.nam=name;
	ctx.lno=0;
	ctx.lvl=oldctx->lvl+1;
	ctx.typ=ftype;
	ctx.sfd=0;
	ctx.dfl=0;
	if (ctx.flg[F'a']) doarg(actx,2,"-k",aeof);
	ndo=0;
	imd=ctx.flg[F'i'];
	rmd=ctx.flg[F'r'];
	if (rmd) stfm(afmt);
	trace(actx,'>');
	eof=0;
	trp.str = tchain;
	tchain = atrp;
	setjmp(trp.save);
	while (!eof && !abort)
	{
		if (descr==0) ibf0=aibf;
		err=0;
		ctx.lno++;
		if (isy!=0 && ctx.flg[F'p']) write(getty(),"*",1);
		if (rmd)
		{
			if ((nu=get80(aibf,uu))<=0) break;
			if (nu<80) eof=1;
			if (ctx.flg[F'g'])
			{
				nt=nu;
				for (i=0;i<nt;i++)
					tt[i]=rtr[0377&uu[i]];
			};
			goto OUT;
		};
		if ((ns=getl(aibf,ss))<0) break;
		if (ns>=LNL) {
			ns=LNL-1; 
			err |= ELL;
		};
		if (ctx.lno==1 || fmt.ffe)
		{
			if ((i=exfm(ns,ss,afmt))<0)
				diag(actx,"format spec rejected",0,582);
			if (i>0 && fmt.ffd) continue;
		};
		if (ss[0]=='~' && ns>0 && !imd && !ctx.flg[F's'])
		{
			n=ns; 
			q=ss; 
			goto CNTRL;
		};
REDO: 
		k=0;
		px=(p=ss)+ns;
		ntb=fmt.fft; 
		pt=pt0;
		while (p<px)
		{
			if (pk=kwx[i=(*p & 0377)])
			{
				n=px-p;
				for (m=0;pk>=ctx.kw0;pk=pk->lk)
				{
					i=pk->nk;
					if (i>m && i<=n && mtch(i,pk->kp,p))
					{
						m=i; 
						qk=pk;
					};
				};
				if (m>0)
				{
					p += m;
					i = (int)qk->rp;
					if ((i&~0377)==0)
					{
						tt[k]=rtr[i];
						uu[k]=i;
						if (k<LNL) k++;
						continue;
					};
					q=p; 
					p = (char *)i;
					i=k+qk->nr;
					if (i>LNL) i=LNL;
					for (m=i;k<m;k++)
					{
						i=(*p++);
						tt[k]=i;
						uu[k]=trt[i & 0377];
					};
					p=q;
					continue;
				};
				i=(*p);
			};
			p++;
			tt[k]=i;
			if (uu[k]=trt[i & 0377])
			{
				if (k<LNL) k++; 
				continue;
			};
			if (i!='\t') {
				err |= ENG; 
				continue;
			};
			if ((i=ntb)<0) i &= (k-i);
			else {
				q=p;
				for (p=pt;i>0;i--)
				{
					if (k<(0377&(*p++))) break;
				};
				m=0377&(*--p);
				ntb=i; 
				pt=p;
				p=q;
				if (i>0) i=m;
				else {
					i=k; 
					err |= ETB;
				};
			};
			if (i>LNL) i=LNL;
			for (;k<i;k++)
			{
				tt[k]=' '; 
				uu[k]=trt[' '];
			};
		};
		if (k>=LNL) {
			k=LNL-1; 
			err |= ELL;
		};
		nt=nu=k;
		if (tt[0]=='~' && nt>0 && !imd && ctx.flg[F's'])
		{
			n=nt; 
			q=tt;
CNTRL: 
			m=0;
			if (err)
			{
				m=diag(actx,"bad control line ignored",0,583);
				if (m) putl(&dbf,n,q);
			};
			if (ctx.flg[F'c'] && !m) putl(&dbf,n,q);
			if (err) continue;
			n--; 
			q++;
			px=(p=acc)+n;
			while (p<px) *p++=(*q++);
			p=acc; 
			i=n;
			if (i==0 || *p!=' ')
			{
				ndo |= doarg(actx,i,p,aeof);
				continue;
			};
			while (!eof)
			{
				while (p<px && *p==' ') p++;
				if (p>=px) break;
				q=acc; 
				k=0;
				while (p<px)
				{
					i=(*p++);
					if (i=='\'' || i=='"')
					{
						if (i==k) {
							k=0; 
							continue;
						};
						if (k==0) {
							k=i; 
							continue;
						};
					};
					if (i==' ' && k==0) break;
					*q++=i;
				};
				ndo |= doarg(actx,q-acc,acc,aeof);
			};
			continue;
		};
		if (ctx.sfd)
		{
			px=(p=acc)+ns;
			for (i=0;p<px;) *p++=ss[i++];
			ndo |= doarg(actx,0,0);
			px=(p=acc)+ns;
			for (i=0;p<px;) ss[i++]=(*p++);
			goto REDO;
		};
OUT: 
		ndo |= 1;
		i=fmt.ffs;
		if (ctx.flg[F'g'] && ctx.flg[F'q'])
		{
			if (nt>i) err |= EDT;
			k=(LNL-1)-fmt.ffm;
			if (nt>k) {
				nt=k; 
				err |= ELL;
			};
		}
		else {
			k=maxcol-fmt.ffm;
			if (i>k) i=k;
			if (nu>i) {
				nu=i; 
				err |= ELL;
			};
		};
		if (!ctx.flg[F'q'])
		{
			n=nu; 
			q=uu;
			if ((k=fmt.ffm)>0)
			{
				n += k; 
				q=acc;
				for (p=q;k>0;k--) *p++=trt[' '];
				for (i=0;i<nu;i++) *p++=uu[i];
			};
			if (qout(n,q)<0) err |= EEX;
		};
		if (!err && !ctx.flg[F'l']) continue;
		if (ctx.flg[F'g']) {
			n=nt; 
			q=tt;
		}
		else {
			n=nu; 
			q=uu;
			if (code!=ASC)
			{
				px=(p=uu)+nu;
				if(nu > 0)
				do
					*p = rtr[0377 & *p];
				while(++p < px);
			};
		};
		if ((k=fmt.ffm)>0)
		{
			p=q; 
			i=0;
			n += k; 
			q=ss;
			while (i<k) ss[i++]=' ';
			while (i<n) ss[i++]=(*p++);
		};
		if (ctx.flg[F'h']) n=tab(n,q);
		m=0;
		if (err)
		{
			if (err&ELL) err &= ~EDT;
			for (i=1;i<32;i <<= 1)
			{
				if (!(i&err)) continue;
				p="";
				if (i==ENG) {
					p="non-graphic deleted"; 
					k=584;
				};
				if (i==ETB) {
					p="undefined tab deleted"; 
					k=585;
				};
				if (i==EDT) {
					p="long line detected"; 
					k=593;
				};
				if (i==ELL) {
					p="long line truncated"; 
					k=594;
				};
				if (i==EEX) {
					p="illegal card excised"; 
					k=595;
				};
				if (*p) m |= diag(actx,p,0,k);
			};
			if (m) putl(&dbf,n,q);
		};
		if (!ctx.flg[F'l']) continue;
		if (!m || !oed) putl(&obf,n,q);
	};
	if (ctx.sfd) ndo |= doarg(actx,0,0);
	rstrp(atrp);
	if (descr) close(descr);
	else {
		bsp0(aibf); 
		ibf0=0;
	};
	if (abort) return (ndo);
	if (oldctx->flg[F'd']) oldctx->kw1=ctx.kw1;
	pk=oldctx->kw0; 
	qk=oldctx->kw1;
	if (ctx.kw0!=pk || ctx.kw1!=qk) index(pk,qk);
	i=oldctx->flg[F'f'];
	if (ctx.flg[F'f']!=i) fold(i);
	if (oldctx->typ==TPTY && oldctx->flg[F'm'])
	{
		p=oldctx->nam+2;
		for (i=0;*p;i++) ss[i]=(*p++);
		ss[i++]='\n'; 
		write(getty(),ss,i);
	};
	if (ctx.dfl!=0 && rsdir(ctx.dsv)<0)
		diag(actx,"cannot reverse chdir",0,586);
	return (ndo);
};

/*end*/
