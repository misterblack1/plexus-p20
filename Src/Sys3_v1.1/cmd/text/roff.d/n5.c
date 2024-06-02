#include "tdef.hd"
#ifdef unix
#include "sys/ioctl.h"
#include "termio.h"
#endif
#include "strs.hd"

/*
troff5.c

misc processing requests
*/

extern struct s *frame;
extern struct tmpfaddr offset, ip;
extern struct d d[NDI], *dip;
#ifndef INCORE
extern struct envblock eblock;
#else
extern struct envblock eblock[NEV];
extern int ev;
extern int maclev;
#endif
extern struct datablock dblock;

extern int ch_CMASK;
extern long atoi0();
extern int dilev;
extern int ascii;
extern int nonumb;
extern int po;
extern int po1;
extern int lgf;
extern int pl;
extern int npn;
extern int npnflg;
extern int copyf;
extern char newf[];
extern int trap;
extern int em;
extern int evlist[EVLSZ];
extern int evi;
extern int ibf;
extern int ev;
extern int ch;
extern int nflush;
extern int tty;
extern struct termio ttys;
extern int quiet;
extern int iflg;
extern int eschar;
extern int sfont;
extern int fontlab[];
extern int noscale;
extern int sv;
extern int esc;
extern int nlflg;
extern int ulfont;
extern int ulbit;
extern int error;
int Hipb = -1;		/* Hold ip.b for cu test in gettch - set by caseul */
int iflist[NIF];
int ifx;
casead(){

	ad = 1;
	/*leave admod alone*/
	if(skip())return;
	getch();
	switch (ch_CMASK)	{
		case 'r':	/*right adj, left ragged*/
			admod = 2;
			break;
		case 'l':	/*left adj, right ragged*/
			admod = ad = 0;	/*same as casena*/
			break;
		case 'c':	/*centered adj*/
			admod = 1;
			break;
		case 'b': case 'n':
			admod = 0;
			break;
		case '0': case '2': case '4':
			ad = 0;
		case '1': case '3': case '5':
			admod = (ch_CMASK - '0')/2;
	}
}
casena(){
	ad = 0;
}
casefi(){
	tbreak();
	fi++;
	pendnf = 0;
	lnsize = LNSIZE;
}
casenf(){
	tbreak();
	fi = 0;
}
casers(){
	dip->nls = 0;
}
casens(){
	dip->nls++;
}
chget(c)
int c;
{
	register i;

	if(skip()) return (c);
	if (((i = getch()) & MOT) ||
	  (ch_CMASK == ' ') ||
	  (ch_CMASK == '\n')){
		ch = i;
		return(c);
	}else return(i & BMASK);
}
casecc(){
	cc = chget('.');
}
casec2(){
	c2 = chget('\'');
}
casehc(){
	ohc = chget(OHC);
}
casetc(){
	tabc = chget(0);
}
caselc(){
	dotc = chget(0);
}
casehy(){
	register i;

	hyf = 1;
	if(skip())return;
	noscale++;
	i = atoi();
	noscale = 0;
	if(nonumb)return;
	hyf = max(i,0);
}
casenh(){
	hyf = 0;
}
max(aa,bb)
int aa,bb;
{
	if(aa>bb)return(aa);
	else return(bb);
}
casece(){
	register i;

	noscale++;
	skip();
	i = max(atoi(),0);
	if(nonumb)i = 1;
	tbreak();
	ce = i;
	noscale = 0;
}
casein(){
	register i;

	if(skip())i = in1;
	else i = max(hnumb(&in),0);
	tbreak();
	in1 = in;
	in = i;
	if(!nc){
		un = in;
		setnel();
	}
}
casell(){
	register i;

	if(skip())i = ll1;
	else i = max(hnumb(&ll),INCH/10);
	ll1 = ll;
	ll = i;
	setnel();
}
caselt(){
	register i;

	if(skip())i = lt1;
	else i = max(hnumb(&lt),0);
	lt1 = lt;
	lt = i;
}
caseti(){
	register i;

	if(skip())return;
	i = max(hnumb(&in),0);
	tbreak();
	un1 = i;
	setnel();
}
casels(){
	register i;

	noscale++;
	if(skip())i = ls1;
	else i = max(inumb(&ls),1);
	ls1 = ls;
	ls = i;
	noscale = 0;
}
casepo(){
	register i;

	if(skip())i = po1;
	else i = max(hnumb(&po),0);
	po1 = po;
	po = i;
#ifndef NROFF
	if(!ascii)esc += po - po1;
#endif
}
casepl(){
	register i;

	skip();
	if((i = vnumb(&pl)) == 0)pl = 11 * INCH; /*11in*/
		else pl = i;
	if(v_nl > pl)v_nl = pl;
}
casewh(){
	register i, j, k;

	lgf++;
	skip();
	i = vnumb((int *)0);
	if(nonumb)return;
	skip();
	j = getrq();
	if((k=findn(i)) != NTRAP){
		mlist[k] = j;
		return;
	}
	for(k=0; k<NTRAP; k++)if(mlist[k] == 0)break;
	if(k == NTRAP){
		prstrfl("Cannot plant trap.\n");
		return;
	}
	mlist[k] = j;
	nlist[k] = i;
}
casech(){
	register i, j, k;

	lgf++;
	skip();
	if(!(j=getrq()))return;
		else for(k=0; k<NTRAP; k++)if(mlist[k] == j)break;
	if(k == NTRAP)return;
	skip();
	i = vnumb((int *)0);
	if(nonumb)mlist[k] = 0;
	nlist[k] = i;
}
findn(i)
int i;
{
	register k;

	for(k=0; k<NTRAP; k++)
		if((nlist[k] == i) && (mlist[k] != 0))break;
	return(k);
}
casepn(){
	register i;

	skip();
	noscale++;
	i = max(inumb(&v_pn),0);
	noscale = 0;
	if(!nonumb){
		npn = i;
		npnflg++;
	}
}
casebp(){
	register i;
	register struct s *savframe;

	if (dilev > 0) return;
#ifndef INCORE
	savframe = frame;
#else
	savframe = (struct s *)maclev;
#endif
	skip();
	if((i = inumb(&v_pn)) < 0)i = 0;
	tbreak();
	if(!nonumb){
		npn = i;
		npnflg++;
	}else if(dip->nls)return;
	eject(savframe);
}
casetm(x) int x;{
	register i;
	char tmbuf[NTM];

	lgf++;
	copyf++;
	if(skip() && x)prstrfl("User Abort.");
	for(i=0; i<NTM-2;)if((tmbuf[i++]=getch()) == '\n')break;
	if(i == NTM-2)tmbuf[i++] = '\n';
	tmbuf[i] = 0;
	aprstrfl(tmbuf);
	copyf--;
}
casesp(a)
int a;
{
	register i, j, savlss;

	tbreak();
	if(dip->nls || trap)return;
	i = findt1();
	if(!a){
		skip();
		j = vnumb((int *)0);
		if(nonumb)j = lss;
	}else j = a;
	if(j == 0)return;
	if(i < j)j = i;
	savlss = lss;
	if (dilev > 0) i = dip->dnl; else i = v_nl;
	if((i + j) < 0)j = -i;
	lss = j;
	newline(0);
	lss = savlss;
}
casert(){
	register a, *p;

	skip();
	if (dilev > 0) p = &dip->dnl; else p = &v_nl;
	a = vnumb(p);
	if(nonumb)a = dip->mkline;
	if((a < 0) || (a >= *p))return;
	nb++;
	casesp(a - *p);
}
caseem(){
	lgf++;
	skip();
	em = getrq();
}
casefl(){
	tbreak();
	flusho();
}
caseev(){
	register nxev;

	if(skip()){
e0:
		if(evi == 0)return;
		nxev =  evlist[--evi];
	} else {
		noscale++;
		nxev = atoi();
		noscale = 0;
		if(nonumb)goto e0;
		flushi();
		if((nxev >= NEV) || (nxev < 0) || (evi >= EVLSZ)){
			prstrfl("Cannot do ev_\n");
			if(error)done2(040);else edone(040);
			return;	}
		evlist[evi++] = ev;	}
	if(ev == nxev)return;
#ifndef INCORE
	lseek(ibf, (long)(ev*sizeof(struct envblock)), 0);
	write(ibf,(char *)&eblock, sizeof(struct envblock));
	lseek(ibf, (long)(nxev*sizeof(struct envblock)), 0);
	read(ibf,(char *)&eblock, sizeof(struct envblock));
#endif
	ev = nxev;
}
caseel(){
	if(--ifx < 0){
		ifx = 0;
		iflist[0] = 0;
	}
	caseif(2);
}
caseie(){
	if(ifx >= NIF){
		prstr("if-else overflow.\n");
		ifx = 0;
		edone(040);
	}
	caseif(1);
	ifx++;
}
caseif(x)
int x;
{
	register i, notflag, true;

	if(x == 2){
		notflag = 0;
		true = iflist[ifx];
		goto i1;
	}
	true = 0;
	skip();
	if ((ch = getch()) && (notflag = (ch_CMASK == '!')))
		ch = 0;
	i = atoi();
	if(!nonumb){
		if(i > 0)true++;
		goto i1;
	}
	i = getch();
	switch (ch_CMASK)	{
		case 'e':
			if(!(v_pn & 01))true++;
			break;
		case 'o':
			if(v_pn & 01)true++;
			break;
#ifdef NROFF
		case 'n':
			true++;
		case 't':
#endif
#ifndef NROFF
		case 't':
			true++;
		case 'n':
#endif
		case ' ':
			break;
		default:
			true = cmpstr(i);
	}
i1:
	true ^= notflag;
	if(x == 1)iflist[ifx] = !true;
	if(true){
		do
		    do { v_hp = 0;  i = getch(); }
		    while (ch_CMASK == ' ');
		while (ch_CMASK == LEFT);
		ch = i;
		nflush++;	}
	else {
		copyf++;
		eatblk(0);
		copyf--;	}
}
eatblk(inblk)
int inblk;
{	register int cnt, i;

	cnt = 0;
	do    {
		if (ch)	{
			i = ch & CMASK;
			ch = 0;	}
		    else
			i = getch0() & CMASK;
		if (i == ESC) cnt++;
		    else if (cnt == 1)
			     if (i == '{') i = LEFT;
				 else if (i == '}') i = RIGHT;
					  else cnt = 0;
			     else cnt = 0;
		if (i == LEFT) eatblk(1);	}
	    while ((!inblk && (i != '\n')) || (inblk && (i != RIGHT)));

	if (i = '\n') nlflg++;
}
cmpstr(delim)
int delim;
{
	register i;
	struct tmpfaddr p;
	extern filep alloc();
	struct tmpfaddr begin;
	int cnt, k;
	int savapts, savapts1, savfont, savfont1,
		savpts, savpts1;
	int smacbcnt;
	extern int macbcnt;

	if(delim & MOT)return(0);
	delim &= CMASK;
	if (dilev > 0) wbfl();
	if((offset.b = begin.b = alloc()) == (filep)0)return(0);
	offset.o = begin.o = 0;
	cnt = 0;
	v_hp = 0;
	savapts = apts;
	savapts1 = apts1;
	savfont = font;
	savfont1 = font1;
	savpts = pts;
	savpts1 = pts1;
	while ((i = getch()) && (ch_CMASK != delim) && (ch_CMASK != '\n')) {
		wbf(i);
		cnt++;	}
	wbt(0);
	k = !cnt;
	if(nlflg)goto rtn;
	cptmpfaddr(p,begin);
	ch = getch();		/* force first read */
	smacbcnt = macbcnt;
	macbcnt = 0;
	apts = savapts;
	apts1 = savapts1;
	font = savfont;
	font1 = savfont1;
	pts = savpts;
	pts1 = savpts1;
	mchbits();
	v_hp = 0;
	while ((i = getch()) && (ch_CMASK != delim) && (ch_CMASK != '\n')) {
		if(rbf0(&p) != i){
			eat(delim);
			k = 0;
			break;
		}
		incoff(p);
		k = !(--cnt);
	}
	macbcnt = smacbcnt;
rtn:
	apts = savapts;
	apts1 = savapts1;
	font = savfont;
	font1 = savfont1;
	pts = savpts;
	pts1 = savpts1;
	mchbits();
	cptmpfaddr(offset,dip->op);
	ffree(begin.b);
	return(k);
}
caserd(){

	lgf++;
	skip();
	getname();
	if(!iflg){
		if(quiet){
#ifdef unix
			ttys.c_lflag &= ~ECHO;
			ioctl(0, TCSETAW, &ttys);
#endif
			aprstrfl("\007"); /*bell*/
		}else{
			if(newf[0]){
				prstr(newf);
				prstr(":");
			}else{
				aprstr("\007"); /*bell*/
			}
		}
	}
	collect();
	tty++;
	pushi((filep)-1);
}
rdtty(){
	char onechar;

	onechar = 0;
#ifdef unix
	if(read(0, &onechar, 1) == 1){
#endif
#ifdef tso
	if (fread((char *)&onechar,1,1,stdin) == 1)	{
#endif
		if(onechar == '\n')tty++;
			else tty = 1;
		if(tty != 3)return(onechar);
	}
	popi();
	tty = 0;
	if(quiet){
#ifdef unix
		ttys.c_lflag |= ECHO;
		ioctl(0, TCSETAW, &ttys);
#endif
	}
	return(0);
}
caseec(){
	eschar = chget('\\');
}
caseeo(){
	eschar = 0;
}
caseta(){
	register i;

	tabtab[0] = nonumb = 0;
	for(i=0; ((i < (NTAB-1)) && !nonumb); i++){
		if(skip())break;
		tabtab[i] = max(hnumb(&tabtab[max(i-1,0)]),0) & TMASK;
		if(!nonumb) switch(ch & CMASK){
			case 'C':
				tabtab[i] |= CTAB;
				break;
			case 'R':
				tabtab[i] |= RTAB;
				break;
			default: /*includes L*/
				break;
			}
		nonumb = ch = 0;
	}
	tabtab[i] = 0;
}
casene(){
	register i, j;

	skip();
	i = vnumb((int *)0);
	if(nonumb)i = lss;
	if(i > (j = findt1())){
		i = lss;
		lss = j;
		dip->nls = 0;
		newline(0);
		lss = i;
	}
}
casetr(){
	register i;

	lgf++;
	skip();
	while (getch() && ((i = ch_CMASK) != '\n')) {
		if ((ch_CMASK & MOT) || (getch() & MOT)) return;
		if (ch_CMASK == '\n') ch_CMASK = ' ';
		trtab[i] = ch_CMASK;
	}
}
casecu(){
	cu++;
	caseul();
}
caseul(){
	register i;

	noscale++;
	if(skip())i = 1;
	else i = atoi();
	if (cu) Hipb = ip.b;	/* save ip.b for test in gettch */
	    else Hipb = -2;	/* ul - for gettch */
	if(ul && (i == 0)){
		font = sfont;
		ul = cu = 0;
	}
	if(i){
		if(!ul){
			sfont = font;
			font = ulfont;
		}
		ul = i;	}
	    else cu = 0;
	noscale = 0;
	mchbits();
}
caseuf(){
	register i, j;

	if(skip() || !(i = getrq()) || (i == 'S') ||
		((j = find(i,fontlab))  == -1))
			ulfont = 1; /*default position 2*/
	else ulfont = j;
#ifdef NROFF
	if(ulfont == 0)ulfont = 1;
#endif
	ulbit = ulfont<<9;
}
caseit(){
	register i;

	lgf++;
	it = itmac = 0;
	noscale++;
	skip();
	i = atoi();
	skip();
	if(!nonumb && (itmac = getrq()))it = i;
	noscale = 0;
}
casemc(){
	register i;

	if(icf > 1)ic = 0;
	icf = 0;
	if(skip())return;
	ic = getch();
	icf = 1;
	skip();
	i = max(hnumb((int *)0),0);
	if(!nonumb)ics = i;
}
casemk(){
	register i, j;

	if (dilev > 0) j = dip->dnl; else j = v_nl;
	if(skip()){
		dip->mkline = j;
		return;
	}
	if((i = getrq()) == 0)return;
	vlist[findr(i)] = j;
}
casesv(){
	register i;

	skip();
	if((i = vnumb((int *)0)) < 0)return;
	if(nonumb)i = 1;
	sv += i;
	caseos();
}
caseos(){
	register savlss;

	if(sv <= findt1()){
		savlss = lss;
		lss = sv;
		newline(0);
		lss = savlss;
		sv = 0;
	}
}
casenm(){
	register i;

	lnmod = nn = 0;
	if(skip())return;
	lnmod++;
	noscale++;
	i = inumb(&v_ln);
	if(!nonumb)v_ln = max(i,0);
	getnm(&ndf,1);
	getnm(&nms,0);
	getnm(&ni,0);
	noscale = 0;
	nmbits = chbits;
}
getnm(p,min)
int *p, min;
{
	register i;

	eat(' ');
	if(skip())return;
	i = atoi();
	if(nonumb)return;
	*p = max(i,min);
}
casenn(){
	noscale++;
	skip();
	nn = max(atoi(),1);
	noscale = 0;
}
caseab(){
	casetm(1);
	done2(0);
}
