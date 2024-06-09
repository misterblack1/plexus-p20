#include "tdef.hd"
#include "strs.hd"
#ifdef NROFF
#include "tw.hd"
extern struct ttable t;
#endif

/*
troff3.c

macro and string routines, storage allocation
*/

extern struct s *frame, *stk, *nxf;
extern struct tmpfaddr ip;
extern struct tmpfaddr offset;
extern struct tmpfaddr nextb;
extern struct d d[NDI], *dip;
#ifndef INCORE
extern struct envblock eblock;
#else
extern struct envblock eblock[NEV];
extern int ev;
extern char *malloc();
#include "stdio.h"
#endif
extern struct datablock dblock;
typedef unsigned short tint;	/* type of characters stored in tmp file */

extern char *enda;

extern int vflag;
extern int tch[];
extern int *cstk[], cstkl;
extern int ch_CMASK;
extern int ch;
extern int ibf;
extern int lgf;
extern int copyf;
extern int app;
extern int ds;
extern int nlflg;
extern int *argtop;
extern int dilev;
extern int nonumb;
extern int nrbits;
extern int nform;
extern int oldmn;
extern int newmn;
extern int macerr;
extern struct tmpfaddr apptr;
extern int diflg;
extern struct tmpfaddr woff;
extern int wbfi;
extern int po;
int **argpp;
int umaccp=0;
int rmaccp=0;		/* use and real macro cache buffer pointer */
int macbcnt=0;		/* macro block read count */
int pagech = '%';
int strflg;
union	{
	tint	wnt[BLK];
	char	wp[BLK*(sizeof(tint)/sizeof(char))];
	} wbuf;			/* output buffer */
union	{
	tint	rnt[NB][BLK];
	char	rp[NB][BLK*(sizeof(tint)/sizeof(char))];
	} rbuf;			/* macro input buffer cache */
int roff[NB];		/* block names for buffers resident in the cache */
int maclev=0;		/* depth of macro (or string or diversion) calls */
extern int regcnt;
int freenslot = -1;
#ifdef INCORE
tint *memblist[NBLIST];	/* incore tmp file mem pointers */
int *argstrs[NMACS];	/* macro arg strings */
int argstrp = 0;	/* index into argstrs */
int *argsp;		/* pointer to latest collected args */
int *sstrp;
#endif
caseig(){
	register i;

	offset.b = 0;
	if((i = copyb()) != '.')control(i,1);
}
casern(){
	register i,j;

	lgf++;
	skip();
	if ((oldmn = frmname(getrq())) < 0) return;
	skip();
	drmname(i = frmname(j = getrq()));
	if (j)	{
		if (freenslot == -1)	{
		    if (i == -1)	{
			prstr("too many names for rn");
			ferrex();	}
		    freenslot = i;	}

		nametab[freenslot].ename = (nametab[oldmn].ename & TAGSMASK) | j;
		nametab[freenslot].vv.val = nametab[oldmn].vv.val;
		nametab[oldmn].ename = -1;	}
}
caserm(){
	lgf++;
	while(!skip()){
		drmname(frmname(getrq()));
	}
}
caseas(){
	app++;
	caseds();
}
caseds(){
	ds++;
	casede();
}
caseam(){
	app++;
	casede();
}
casede(){
	register i, req;
	register struct tmpfaddr savoff;
	extern filep finds();

	if (dilev > 0) wbfl();
	req = '.';
	lgf++;
	skip();
	if ((i = getrq()) && finds(i)) {
		if(ds)copys();
			else req = copyb();
		wbfl();
		drmname(oldmn);
		if (newmn >= 0) nametab[newmn].ename = i | MMASK;
		if(apptr.b){
			cptmpfaddr(savoff,offset);
			cptmpfaddr(offset,apptr);
			wbt(IMP);
			cptmpfaddr(offset,savoff);
		}
		cptmpfaddr(offset,dip->op);
		if(req != '.')control(req,1);	}
	ds = app = 0;
	return;
}
frmname(name)
		/* find name in hashed request/macro/string table */
int name;
{
	register int hname, toff, htoff;

	if (!name) return -1;
	freenslot = -1;		/* pos of first free name slot */
	hname = name & NAMEMASK;
	htoff = toff = abs(HASH(name,NM));	/* save orig value */

	while (nametab[toff].ename)	{	/* check next entry */
		if ((nametab[toff].ename & NAMEMASK) == hname) return toff;
		if ((nametab[toff].ename == -1) && (freenslot == -1))
				freenslot = toff;
		toff = (toff + 1) % NM;
		if (toff == htoff) return -1;	}
	if (freenslot == -1) freenslot = toff;
	return -1;		/* not found */
}
fnrname(name)		/* find a name in the hashed number register table */
int name;
{
	register int hname, toff, htoff;

	if (!name) return -1;
	freenslot = -1;		/* mark first free slot pos */
	hname = name & NAMEMASK;
	htoff = toff = abs(HASH(name,NN));

	while (r[toff])	{	/* check next slot */
		if ((r[toff] & NAMEMASK) == hname) return toff;
		if ((r[toff] == -1) && (freenslot == -1)) freenslot = toff;
		toff = (toff + 1) % NN;
		if (toff == htoff) return -1;	}
	if (freenslot == -1) freenslot = toff;
	return -1;		/* not found */
}
drmname(toff)	/* remove name from request, macro table */
register int toff;
{
	if (toff == -1) return;
	if (nametab[toff].ename & MMASK)	/* free macro storage */
		ffree((filep)nametab[toff].vv.val);
	nametab[toff].ename = -1;	/* free hashed entry */
}
filep finds(mn)
int mn;
{
	register i, j;
	register struct tmpfaddr tip;
	int savmacbcnt;
	extern filep alloc();

	oldmn = frmname(mn);		/* get macro name */
	newmn = -1;
	apptr.b = (filep)0;
	if(app && (oldmn >= 0) && (nametab[oldmn].ename & MMASK)){
			maclev++;
			savmacbcnt = macbcnt;
			tip.b = (filep)nametab[oldmn].vv.val;
			tip.o = 0;
			macbcnt = 0;
			oldmn = -1;
			while (rbf0(&tip)) incoff(tip);
			cptmpfaddr(apptr,tip);
			if (!diflg) incoff(tip);
			cptmpfaddr(nextb,tip);
			macbcnt = savmacbcnt;
			maclev--;
	}else{
		if (((i = freenslot) == -1) && (oldmn >= 0))
		    for (j=(oldmn+1)%NM; j!=oldmn; j=((j+1)%NM))
			if ((nametab[j].ename == -1) || (!nametab[j].ename))
				{  i = j;  break;  }
		if ((i == -1) || (i == oldmn) || !alloc())	{
			app = 0;
			if(macerr++ > 1)done2(02);
			prstr("Too many string/macro names.\n");
			edone(04);
			offset.o = 0;
			return(offset.b = 0);
		}
		nametab[i].vv.val = (filep)nextb.b;
		if(!diflg){
			newmn = i;
			if(oldmn == -1)nametab[i].ename = -1;	}
		    else nametab[i].ename = mn | MMASK;
		offset.o = 0;
	}

	app = 0;
	cptmpfaddr(offset,nextb);
	return (offset.b);
}
skip(){
	register i;

	do i = getch();
	    while (ch_CMASK == ' ');
	ch=i;
	return(nlflg);
}
copyb()
{
	register ii, j, k;
	int i, req;
	int tbuf[3];

	if(skip() || !(j=getrq()))j = '.';
	req = j;
	k = j>>BYTE;
	j &= BMASK;
	copyf++;
	flushi();

	while (1)	{	/* read body of macro */
	    nlflg = 0;
	    ii = 0;
	    if ((tbuf[ii++] = getch()) &&
			((ch_CMASK == cc) || (ch_CMASK == c2)))	{

		do tbuf[ii] = getch();	/* skip leading blanks */
		    while ((ch_CMASK == ' ') || (ch_CMASK == '\t'));
		ii++;

		if (ch_CMASK == j)	{	/* first terminating char? */
		    if ((tbuf[ii++] = getch()) && (ch_CMASK == k))
			    goto found;

		    if (!k)	{	/* single letter call? */
			ch = tbuf[ii-1];
			i = getach();
			ch = tbuf[ii-1];
			if (!i) goto found;	}}}

	    for (i=0; i<ii; i++)	/* copy early part of line */
		wbf(tbuf[i]);

	    if (tbuf[ii-1] != '\n')
		do wbf(i = getch());
		    while (i != '\n');	}	/* copy to end of line */

found:
	wbt(0);
	copyf--;
	return(req);
}
copys()
{
	register i;

	copyf++;
	if(skip())goto c0;
	if(((i=getch()) & CMASK) != '"')wbf(i);
	while(((i=getch()) & CMASK) != '\n')wbf(i);
c0:
	wbt(0);
	copyf--;
}
filep alloc()
{
	register filep i;

	for(i=1; i<=NBLIST; i++)
		if(blist[i] == 0)break;
	if (i == NBLIST+1) i = 0;
		else  {  blist[i] = -1;
#ifdef INCORE
			 if ((memblist[i] =
			     (tint *)malloc(BLK*sizeof(tint))) == NULL)	{
				prstr("out of memory for store");
				exit(1);	}
#endif
			}
	nextb.o = 0;
	return (nextb.b = i);
}
ffree(i)
register filep i;
{
	register j;

	while((blist[j = i]) != (filep)-1){
#ifdef INCORE
	free(memblist[i]);
#endif
		i = blist[i];
		blist[j] = 0;
	}
	blist[j] = 0;
}
wbt(i)
int i;
{
	wbf(i);
	wbfl();
}
wbf(i)
int i;
{
	if(!offset.b)return;
	if(!woff.b){
		cptmpfaddr(woff,offset);
		wbfi = 0;
	}
	wbuf.wnt[wbfi++] = (tint)i;
	if (!(offset.o = ++offset.o & (BLK - 1)))	{
		wbfl();
		if(blist[offset.b] == (filep)-1){
			if(alloc() == 0){
				prstr("Out of temp file space.\n");
				done2(01);	}
			blist[offset.b] = (filep) nextb.b;	}
		offset.b = (filep) blist[offset.b];	}
	if(wbfi >= BLK)wbfl();
}
wbfl(){
	register int i, j;
#ifdef INCORE
	register tint *bp;
#endif

	if (!woff.b) return;
#ifndef INCORE
	lseek(ibf, (long)(boff(woff.b) + woff.o) * sizeof(tint), 0);
	write(ibf, wbuf.wp, (int)(wbfi * sizeof(tint)));
#else
	bp = memblist[woff.b] + woff.o;
	for (i=0; i<wbfi; i++)
		*bp++ = wbuf.wnt[i];	/* save block */
#endif
	for (i=0; i<NB; i++)
	    if (woff.b == roff[i])	{
		for (j=0; j<wbfi; j++)
			rbuf.rnt[i][woff.o+j] = wbuf.wnt[j];	/* fix up read buffer */
		break;	}
	woff.b = 0;
}
rbf(){
	register i;

	if (!(i = rbf0(&ip)))	{
		i = popi();
		if (maclev) rmaccp = (!rmaccp) ? NB-1 : rmaccp-1;	}
	    else incoff(ip);
	return(i);
}
rbf0(fp)
struct tmpfaddr *fp;
{
	register filep i;

	if ((fp->b == (filep)-1) || (fp->b == (filep)-2)) return -1;

	if (fp->b != roff[umaccp])	{	/* test if buffer in cache */
	    for (i=0; i<NB; i++)
		if (fp->b == roff[i]) break;	/* found */
	    umaccp = (i == NB) ? getblk(fp->b) : i;	}

#ifndef tso
	return (int)rbuf.rnt[umaccp][fp->o];
#else
	return (((int)rbuf.rnt[umaccp][fp->o]) & 0177777);
#endif
}
getblk(blk)
filep blk;		/* find a place in the cache and get a block */
{
	register int i;
#ifdef INCORE
	register int j;
	register tint *bp;
#endif

	i = (!rmaccp) ? NB-1 : rmaccp-1;
	if (!macbcnt || (!(umaccp == i)))	{
		i = rmaccp;
		rmaccp = (rmaccp + 1) %  NB;	}
	macbcnt++;
#ifndef INCORE
	lseek(ibf, (long)boff(blk) * sizeof(tint), 0);
	if (read(ibf, rbuf.rp[i], BLK * sizeof(tint)) == 0)	{
		prstr("Macro read failed\n");
		ferrex();	}
#else
	bp = memblist[blk];
	for (j=0; j<BLK; j++)
		rbuf.rnt[i][j] = *bp++;	/* get buffer */
#endif
	roff[i] = blk;
	return i;
}
errbsa()
{
	prstr("Bad storage allocation\n");
	done2(-5);
}
popi(){
	register struct s *p;

	if(frame == stk)return(0);
	if(strflg)strflg--;
#ifdef INCORE
	free(nxf);
#endif
	p = nxf = frame;
	p->nargs = 0;
	frame = p->pframe;
	cptmpfaddr(ip,p->pip);
	macbcnt = p->macbcnt;
	pendt = p->ppendt;
	if (tch[0] = p->pCH0) cstk[CH0] = tch;
	cstkl = p->pcstkl;
	cstk[RP] = p->pRP;
	cstk[AP] = p->pAP;
	cstk[CP] = p->pCP;
#ifdef INCORE
	if ((argstrp > 0) && (argsp)) free(argsp);	/* args */
	argsp = argstrs[--argstrp];
#endif
	maclev--;
	return(p->pch);
}
pushi(newip)
filep newip;
{
	register struct s *p;
#ifndef INCORE
	extern char *setbrk();
#endif

	p = nxf;
#ifndef INCORE
	if((enda - sizeof(struct s)) < (char *)nxf)setbrk(DELTA);
#else
	if (argstrp < NMACS)
		argstrs[argstrp++] = argsp;	/* save latest */
	    else	{
		prstr("too many macro calls");
		exit(1);	}
	argsp = sstrp;		/* new args pointer */
	sstrp = (int *)0;
#endif
	p->pframe = frame;
	cptmpfaddr(p->pip,ip);
	p->macbcnt = macbcnt;
	p->ppendt = pendt;
	p->pCH0 = tch[0];
	p->pch = ch;
	p->pcstkl = cstkl;
	p->pRP = cstk[RP];
	p->pAP = cstk[AP];
	p->pCP = cstk[CP];
	cstkl  = pendt = ch = 0;
	cstk[RP] = cstk[AP] = cstk[CP] = (int *) 0;
	frame = nxf;
#ifndef INCORE
	if(nxf->nargs == 0) nxf += 1;
		else nxf = (struct s *)argtop;
#else
	if ((nxf = (struct s *)malloc(sizeof(struct s))) == NULL) {
		prstr("out of memory for macro call");
		exit(1);	}
#endif
	maclev++;
	macbcnt = 0;
	ip.o = 0;
	return(ip.b = newip);
}

#ifndef INCORE
char *setbrk(x)
int x;
{
#define ALIGN(addr,grain)  ((addr+sizeof(grain)-1) & ~(sizeof(grain)-1))
	register char *i;
	char *sbrk();

	x = ALIGN(x, long);

	if((i = sbrk(x)) == (char *)-1){
		prstrfl("Core limit reached.\n");
		edone(0100);	}
	    else enda = i;

	return((char *)ALIGN((int)i, long));
}
#endif
getsn(){
	register i;

	if((i=getach()) == 0)return(0);
	if(i == '(')return(getrq());
		else return(i);
}
setstr(){
	register i;

	lgf++;
	if(((i=getsn()) == 0) ||
	   ((i=frmname(i)) == -1) ||
	   !(nametab[i].ename & MMASK))
		lgf--;
	else	{
#ifndef INCORE
		if((enda-sizeof(int)) <= (char *)nxf)setbrk(DELTA);
#endif
		nxf->nargs = 0;
		strflg++;
		lgf--;
		pushi((filep)nametab[i].vv.val);
	}
}
collect()
{
	register i;
	register int *strp;
	int *lim;
	int quote;
	struct s *savnxf;
	union	{
		int	**dbl;
		int	*pt;
		} argppend;
#ifdef INCORE
	int *hsstrp;
#endif

	copyf++;
	nxf->nargs = 0;
	if(skip())goto rtn;
	strflg = 0;
	savnxf = nxf;
#ifndef INCORE
	lim = (int *)(nxf = savnxf + sizeof(struct s));
	if((argppend.dbl =
		(argpp=(int **)savnxf+(sizeof(struct s)/sizeof(int **)))
		    +(sizeof(struct s)-1)) > (int **)enda)setbrk(DELTA);
#else
	if ((argpp = (int **)malloc(MARGL*sizeof(int))) == NULL) {
		prstr("out of memory for macro call");
		exit(1);	}
	hsstrp = (int *)argpp;
	argppend.pt = hsstrp + sizeof(struct s) - 1;
	lim = hsstrp + (MARGL*sizeof(int));	/* end of buf */
#endif
	strp = argppend.pt;
	for(i=8; i>=0; i--)argpp[i] = 0;
	while((argpp != argppend.dbl) && (!skip())){
		*argpp++ = strp;
		quote = 0;
		ch = getch();
		if (ch_CMASK == '"')	{
			ch = 0;
			quote++;	}
		while(1){
			i = getch();
			if( nlflg ||
			  ((!quote) && (ch_CMASK == ' ')))break;
			if(quote && (ch_CMASK == '"') &&
			    ((i = getch()) && (ch_CMASK != '"'))) {
				ch = i;
				break;
			}
			*strp++ = i;
			if(strflg && (strp >= lim)){
				prstrfl("Macro argument too long.\n");
				copyf--;
				edone(004);
			}
#ifndef INCORE
			if((enda-(2*sizeof(int))) <= (char *)strp)setbrk(DELTA);
#endif
		}
		*strp++ = 0;
	}
	nxf = savnxf;
#ifndef INCORE
	nxf->nargs = argpp -(int **)(nxf + 1);
#else
	sstrp = hsstrp;
	nxf->nargs = (int *)argpp - sstrp;
#endif
	argtop = strp;
rtn:
	copyf--;
}
caseda(){
	app++;
	casedi();
}
casedi(){
	register i, j;
	register *k;

	lgf++;
	if(skip() || ((i=getrq()) == 0)){
		if(dilev > 0){
			wbt(0);
			v_dn = dip->dnl;
			v_dl = dip->maxl;
			dip = &d[--dilev];
			cptmpfaddr(offset,dip->op);
		}
		goto rtn;
	}
	if(++dilev == NDI){
		--dilev;
		prstr("Cannot divert.\n");
		edone(02);
	}
	if (dilev > 1) wbt(0);
	diflg++;
	dip = &d[dilev];
	dip->op.b = finds(i);
	dip->op.o = offset.o;
	dip->curd = i;
	drmname(oldmn);
	k = (int *)&dip->dnl;
	for(j=0; j<10; j++)k[j] = 0;	/*not op and curd*/
rtn:
	app = 0;
	diflg = 0;
}
casedt(){
	lgf++;
	dip->dimac = dip->ditrap = dip->ditf = 0;
	skip();
	dip->ditrap = vnumb((int *)0);
	if(nonumb)return;
	skip();
	dip->dimac = getrq();
}
casetl(){
	register i, j;
	int w1, w2, w3, delim;
	struct tmpfaddr begin;
	static struct tmpfaddr null = {0,0};
	extern width(), pchar();

	dip->nls = 0;
	skip();
	if (dilev > 0) wbfl();
	if((offset.b = begin.b = alloc()) == 0)return;
	offset.o = begin.o = 0;
	if((delim = getch()) & MOT){
		ch = delim;
		delim = '\'';
	}else delim = ch_CMASK;
	if(!nlflg)
		while(((i = getch()) & CMASK) != '\n'){
			if(ch_CMASK == delim)i = IMP;
			wbf(i);
		}
	wbf(IMP);wbf(IMP);wbt(0);

	w1 = hseg(width,&begin);
	w2 = hseg(width,&null);
	w3 = hseg(width,&null);
	cptmpfaddr(offset,dip->op);
#ifdef NROFF
	if(!offset.b)horiz(po);
#endif
	hseg(pchar,&begin);
	if(w2 || w3)horiz(j=quant((lt - w2)/2-w1,HOR));
	hseg(pchar,&null);
	if(w3){
		horiz(lt-w1-w2-w3-j);
		hseg(pchar,&null);
	}
	newline(0);
	if(dilev > 0) {if(dip->dnl > dip->hnl)dip->hnl = dip->dnl;}
	else{if(v_nl > dip->hnl)dip->hnl = v_nl;}
	ffree(begin.b);
}
casepc(){
	pagech = chget(IMP);
}
hseg(f,p)
int (*f)();
struct tmpfaddr *p;
{
	register acc, i;
	static struct tmpfaddr q;
	int smacbcnt;

	acc = 0;
	if(p->b) { q.o = p->o; q.b = p->b; }
	smacbcnt = macbcnt;
	macbcnt = 0;
	while(1){
		i = rbf0(&q);
		incoff(q);
		if(!i || (i == IMP))	{
				macbcnt = smacbcnt;
				return (acc);	};
		if((i & CMASK) == pagech){
			nrbits = i & ~CMASK;
			nform = fmt[findr('%')];
			acc += fnumb(v_pn,f);
		}else acc += (*f)(i);
	}
}
casepm(){
#ifndef NDIAGS
	register i, k;
	register char *p;
	int xx, cnt, kk, tot;
	filep j;
	char *kvt();
	char pmline[10];

	kk = cnt = 0;
	tot = !skip();
	for(i = 0; i<NM; i++){
		if (((xx = nametab[i].ename) == -1) || (!(xx & MMASK))) continue;
		p = pmline;
		j = (filep)nametab[i].vv.val;
		k = 1;
		while((j = blist[j]) != (filep)-1) k++;
		cnt++;
		kk += k;
		if(!tot){
			*p++ = xx & 0177;
			if(!(*p++ = (xx >> BYTE) & 0177))*(p-1) = ' ';
			*p++ = ' ';
			kvt(k,p);
			aprstr(pmline);
		}
	}
	if(tot || (cnt > 1)){
		kvt(kk,pmline);
		aprstr(pmline);
	}
}
char *kvt(k,p)
int k;
char *p;
{
	if(k>=100)*p++ = k/100 + '0';
	if(k>=10)*p++ = (k%100)/10 + '0';
	*p++ = k%10 + '0';
	*p++ = '\n';
	*p = 0;
#endif
}
