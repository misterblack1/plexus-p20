#
/* send3.c */


/* This information is proprietary and is the property of Bell
   Telephone Laboratories, Incorporated.  Its reproduction or
   disclosure to others, either orally or in writing, is pro-
   hibited without written permission of Bell Laboratories. */


#define IDMOD send3

#define IDSTR "~|^`send3.c 1.10 4/22/76

#include "send.h"
#include "ctype.h"
#include <sys/types.h>
#include <sys/stat.h>

doarg(ctx,n,s,eof)
struct context *ctx;
char *s;
int *eof;
{
	static int nsh;
	char *px,*avv,vv[LNX];
	int ndo,k,nv,vt,vx,pd[2];
	struct keywd *pk;
	register int i;
	register char *p;
	register struct context *x;
	char	*argv[5];
	char	**envir();
	avv=vv;
	x=ctx;
	if (abort)
	{
		if (x->sfd>0) unlink(x->stm);
		x->sfd=0; 
		return (1);
	};
	px=(p=s)+n;
	for (i=0;p<px;)
	{
		avv[i] = *p;
		if ((k=(*p++))=='\\' && p<px) {
			avv[i] = *p;
			k=0200|(*p++);
		};
		ss[i]=k;
		if (i<LNL) i++;
	};
	if(i >= LNL)
		i = LNL - 1;
	avv[i] = '\0';
	nv = i;
	px = avv+i;
	vt=type(nv,ss,&vx);
	if (vt==TDOL)
	{
		flush(&obf);
		flush(&dbf);
		if (x->sfd==0)
		{
			prf("%q/tmp/sh",x->stm);
			x->sfd=i=mktmp(x->stm,nsh++);
			if (i>0) inibf(&tbf,i);
		};
		if (x->sfd>0) putl(&tbf,nv-1,avv+1);
		return (0);
	};
	ndo=0;
	if (x->sfd)
	{
		if (x->sfd<0) {
			p="(create)"; 
			goto NS;
		};
		flush(&tbf);
		close(x->sfd);
		if (xpipe(pd)<0) {
			p="(pipe)"; 
			goto NS;
		};
		if ((i=tfork())==(-1))
		{
			p="(fork)";
NS: 
			k=diag(x,"cannot run shell",p,587);
			goto XS;
		};
		if (i==0)
		{
			close(0); 
			open("/dev/null",0);
			close(1); 
			dup(pd[1]);
			for(i=3;i<20;i++)
				close(i);
			argv[0]="sh";
			argv[1]=x->stm;
			argv[2]=0;
			execve("/bin/sh",argv,envir());
			exit(040);
		};
		close(pd[1]);
		ndo |= dofile(pd[0],x->stm,x,TDOL);
		k=shex(x);
XS: 
		if (k && (i=open(x->stm,0))>=0)
		{
			inibf(&tbf,i);
			while ((i=getl(&tbf,ss))>=0) putl(&dbf,i,ss);
			close(tbf.fd);
		};
		if (x->sfd>0) unlink(x->stm);
		x->sfd=0;
	};
	switch (vt)
	{
	case TCOM: 
		break;
	case TEOF: 
		*eof=1; 
		break;
	case TINP: 
		ndo |= dofile(0,"-",x,vt); 
		break;
	case TTTY: 
		ndo |= dofile(dup(getty()),"+",x,vt); 
		break;
	case TMSG: 
		*px='\n'; 
		write(getty(),avv+1,nv); 
		break;
	case TPIN:
		*px='\n';
		if (tin) {
			write(getty(),avv+2,nv-1); 
			vt=TPTY;
		};
		*px=0;
		ndo |= dofile(0,avv,x,vt); 
		break;
	case TPTY:
		*px='\n';
		write(getty(),avv+2,nv-1);
		*px=0;
		ndo |= dofile(dup(getty()),avv,x,vt); 
		break;
	case TFMT:
		i=0; 
		ss[i++]='<';
		for (p=avv;p<px;) ss[i++]=(*p++);
		ss[i++]='>';
		if (exfm(i,ss,x->dsp)<0)
			diag(x,"default spec rejected",avv,588);
		ndo |= 1; 
		break;
	case TSFL:
		for (p=avv+1;p<px;p++)
		{
			x->flg[i=F(*p)]=1;
			if (i==(F'f')) fold(1);
			if (i==(F'k'))
			{
				x->kw0=x->kw1=(x->ocx)->kw1;
				for (i=0;i<256;i++) kwx[i]=0;
			};
			if(i==(F'x'))
				seqflg++;
		};
		ndo |= 1; 
		break;
	case TRFL:
		for (p=avv+1;p<px;p++)
		{
			x->flg[i=F(*p)]=0;
			if (i==(F'f')) fold(0);
			if (i==(F'k'))
				index(x->kw0=(x->ocx)->kw1,x->kw1);
		};
		ndo |= 1; 
		break;
	case TVFL:
		for (p=avv+1;p<px;p++)
		{
			i=F(*p);
			x->flg[i]=k=(x->ocx)->flg[i];
			if (i==(F'f')) fold(k);
			if (i==(F'k'))
				index(x->kw0=(x->ocx)->kw0,x->kw1);
		};
		ndo |= 1; 
		break;
	case TPKY:
		avv += 2;
		nv -= 2;
		goto prompt;
	case TQPK:
		avv += 2; 
		nv -= 2;
		if (pk= (struct keywd *)gtky(x,nv,avv))
		{
			dfky(x,nv,avv,pk->nr,pk->rp); 
			break;
		};
	prompt:
		*px='=';
		write(getty(),avv,nv+1);
		px=(p=avv)+LNL;
		p += nv;
		while (read(getty(),p,1)==1 && *p!='\n')
		{
			if (p<px) p++;
		};
		px=p; 
		p=avv+nv;
		for (i=0;p<px;)
		{
			if ((k=(*p++))=='\\' && p<px)
			{
				k=0200|(*p++);
			};
			ss[i++]=k;
		};
		if ((k=hex(i,ss))>=0)
		{
			dfky(x,nv,avv,0,k); 
			break;
		};
		p = avv+nv;
		for(i = 0;p < px;) {
			if((k=(*p++))=='\\' && p<px)
				k=(*p++);
			ss[i++]=k;
		}
		dfky(x,nv,avv,i,ss); 
		break;
	case TQHX:
		avv++;
		nv--;
		if (pk= (struct keywd *)gtky(x,nv-4,avv)) {
			if(pk->nr>0 || pk->rp<=(char *)0377) {
				dfky(x,nv-4,avv,pk->nr,pk->rp);
				break;
			}
		}
	case TDHX:
		dfky(x,nv-4,avv,0,vx); 
		break;
	case TQKY:
		avv++;
		nv--;
		if (pk=(struct keywd *)gtky(x,--vx,avv)) {
			if(pk->nr>0 || pk->rp<=(char *)0377) {
				dfky(x,vx,avv,pk->nr,pk->rp);
				break;
			}
		}
	case TDKY:
		i=vx+1;
		dfky(x,vx,avv,nv-i,avv+i); 
		break;
	case TCHD:
		if (x->dfl==0)
		{
			svdir(x->dsv,NDR); 
			x->dfl++;
		};
		if (chdir(avv+1)<0)
			diag(x,"cannot change directory",avv+1,589);
		break;
	case TEXC:
		flush(&obf);
		flush(&dbf);
		avv++; 
		nv--;
		if (xpipe(pd)<0) {
			p="(pipe)"; 
			goto NC;
		};
		if ((i=tfork())==(-1))
		{
			p="(fork)";
NC: 
			k=diag(x,"cannot run command",p,590);
			goto XC;
		};
		if (i==0)
		{
			close(0); 
			open("/dev/null",0);
			close(1); 
			dup(pd[1]);
			for(i=3;i<20;i++)
				close(i);
			argv[0]="sh";
			argv[1]="-c";
			argv[2]=avv;
			argv[3]=0;
			execve("/bin/sh",argv,envir());
			exit(040);
		};
		close(pd[1]);
		ndo |= dofile(pd[0],avv,x,vt);
		k=shex(x);
XC: 
		if (k) putl(&dbf,nv,avv); 
		break;
	case TFIL:
		ndo |= dofile(open(avv,0),avv,x,vt);
	};
	return (ndo);
};

struct linsf
{
	char *hst,*sys,*dir,*pfx,*dev,*peri,*phne;
};

type(n,s,xp)
char *s;
int *xp;
{
	char *px, tbuf[128];
	register int i,t;
	register char *p;
	struct linsf *q, *linz, line[NLIN];
	char linx[NLIX];
	i=s[0];
	if (n<=0) return (TCOM);
	if (n==1)
	{
		if (i=='.') return (TEOF);
		if (i=='-') return (TINP);
		if (i=='+') return (TTTY);
	};
	if (n>=2 && s[1]==':')
	{
		if (i=='-') return (TPIN);
		if (i=='+') return (TPTY);
		if (i=='=') return (TPKY);
		if (i=='?') return (TQPK);
	};
	if (i==':')
	{
		if (n>=2 && s[n-1]==':') return (TFMT);
		return (TMSG);
	};
	if (i=='!')
	{
		return (TEXC);
	};
	if (i=='$') return (TDOL);
	if (i=='@' && n > 1) return(TCHD);
	if (i=='~') return (TCOM);
	if (i=='-' || i=='+' || i=='=')
	{
		px=(p=s)+n;
		while (++p<px)
		{
			if ((t=(*p))>=0140) t &= 0137;
			if (t<'A' || t>'Z') break;
		};
		if (p==px)
		{
			if (i=='-') return (TSFL);
			if (i=='+') return (TRFL);
			if (i=='=') return (TVFL);
		};
	};
	p=s+n;
	while (--p>=s && *p!='=');
	i=p-s;
	if (i>=0)
	{
		t=i+1;
		t=hex(n-t,s+t);
		if (t>=0) {
			*xp=t; 
			return(s[0]=='?'?TQHX:TDHX);
		};
		*xp=i; 
		return(s[0]=='?'?TQKY:TDKY);
	};
	strncpy(tbuf,s,n);
	tbuf[n] = '\0';
	q = line;
	i = rjetab("lines",q,NLIN*7,linx,NLIX);
	linz = q + (i/7);
	for(; q < linz; q++) {
		if(strncmp(tbuf,q->hst,strlen(tbuf)) == 0) {
			strcpy(hstname,tbuf);
			nhst = 1;
			return(TCOM);
		}
	}
	return(TFIL);
};

hex(n,s)
char *s;
{
	register int i,h;
	register char *p;
	p=s;
	if (n!=3 || *p++!='^') return (-1);
	h=0;
	i=(*p++);
	if (i>=0140) i &= 0137;
	if (i>='0' && i<='9') h += (i-'0');
	else if (i>='A' && i<='F') h += (i+10-'A');
	else return (-1);
	h <<= 4;
	i=(*p++);
	if (i>=0140) i &= 0137;
	if (i>='0' && i<='9') h += (i-'0');
	else if (i>='A' && i<='F') h += (i+10-'A');
	else return (-1);
	return (h);
};

char	**
envir()
{
	register char	*p,*q;
	register struct keywd	*key;
	char	**env;
	static char	*ans[100];
	env = ans;
	for(key=kwx['$'];key!=0;key=key->lk)
	if((key->nr > 0 || key->rp <= (char *)0377) && key->nk > 1) {
		if(badenv(key->kp,key->nk) || dupenv(ans,env,key))
			continue;
		p = key->kp;
		q = p++;
		*env++ = q;
		while(--key->nk > 0)
			*q++ = *p++;
		*q++ = '=';
		if(key->nr > 0) {
			q += key->nr;
			*q = 0;
			continue;
		}
		*q++ = (int)key->rp;
		*q = 0;
	}
	*env++ = 0;
	return(ans);
}

dupenv(e1,e2,kwd)
register char	**e1,**e2;
register struct keywd	*kwd;
{
	while(e1 < e2) {
		if(mtch(kwd->nk-1,*e1,kwd->kp+1))
			return(1);
		e1++;
	}
	return(0);
}

badenv(p,lp)
register char	*p;
register int	lp;
{
	if(--lp < 1)
		return(1);
	if(*p++ != '$')
		return(1);
	if(!isalpha(*p) && *p != '_')
		return(1);
	p++;
	while(--lp > 0) {
		if(!isalpha(*p) && !isdigit(*p) && *p != '_')
			return(1);
		p++;
	}
	return(0);
}

/*end*/
