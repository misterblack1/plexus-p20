#include "tdef.hd"
#include "strs.hd"
#ifdef NROFF
#include "tw.hd"
#endif

/*
troff6.c

width functions, sizes and fonts
*/

#ifndef INCORE
extern struct envblock eblock;
#else
extern struct envblock eblock[NEV];
extern int ev;
#endif
extern struct ttable t;
extern struct datablock dblock;
extern struct tmpfaddr ip;

extern int Hipb;
extern int tch[];
extern int *cstk[], cstkl;
extern int ch_CMASK;
extern long atoi0();
int sbold = 0;
int bdtab[4] = {0, 0, 3, 0};
extern int eschar;
extern int widthp;
extern int xfont;
extern int smnt;
extern int setwdf;
extern int nonumb;
extern int noscale;
extern int nlflg;
extern int nform;
extern int dfact;
extern int dfactd;
extern int vflag;
extern int level;
extern int ch;
extern int res;
int fontlab[] = {'R','I','B','S',0};
width(c)
int c;
{
	register i;

	if(c & MOT){
		if (c & VMOT) return (0);
		i = c & ~MOTV;
		return ((c & NMOT) ? -i : i);
	}
	if((i = (c & CMASK)) == 010) return (-widthp);
	if(i == PRESC)i = eschar;
	if((i == ohc) || (i >= 0370) || (c & ZBIT)) return (0);
	i = trtab[i] & BMASK;
	if(i < 040) return (0);
	return (widthp = (*(t.codetab[i-32]) & 0177) * t.Char);
}
setch(){
	register i,*j;
	extern int chtab[];

	if((i = getrq()) == 0)return(0);
	for(j=chtab;*j != i;j++)if(*(j++) == 0)return(0);
	return (*(++j) | chbits);
}
find(i,j)
int i,j[];
{
	register k;

	if(((k = i-'0') >= 1) && (k <= 4) && (k != smnt))return(--k);
	for(k=0; j[k] != i; k++)if(j[k] == 0)return(-1);
	return(k);
}
mchbits(){
	chbits = (((pts)<<2) | font) << (BYTE + 1);
	sps = width(' ' | chbits);
}
setps(){
	register i,j;

	if((((i=getch() & CMASK) == '+')  || (i == '-')) &&
	  (((j=(ch = getch() & CMASK) - '0') >= 0) && (j <= 9))){
		ch = 0;
		return;
	}
	if((i -= '0') == 0){
		return;
	}
	if((i > 0) && (i <= 9)){
		if((i <= 3) &&
		  ((j=(ch = getch() & CMASK) - '0') >= 0) && (j <= 9)){
			i = 10*i +j;
			ch = 0;
		}
	}
}
caseft(){
	skip();
	setfont(1);
}
setfont(a)
int a;
{
	register i,j;

	if(a)i = getrq();
		else i = getsn();
	if(!i || (i == 'P'))
		j = font1;
	    else if ((i == 'S') || ((j = find(i,fontlab)) == -1))
			return;
	font1 = font;
	font = j;
	if (font == 1) Hipb = -2;	/* mark italic (not cu) to gettch */
	    else if (Hipb == -2)	/* clear Hipb */
			Hipb = -1;
	mchbits();
}
setwd(){
	register i, base, wid;
	int delim, em, k;
	int savlevel, savhp, savfont, savfont1;

	base = v_st = v_sb = wid = v_ct = 0;
	if (getch() && ((delim = ch_CMASK) & MOT)) return;
	savhp = v_hp;
	savlevel = level;
	v_hp = level = 0;
	savfont = font;
	savfont1 = font1;
	setwdf++;
	while ((i = getch()) && (ch_CMASK != delim) && !nlflg)	{
		wid += width(i);
		if(!(i & MOT)){
			em = 2*t.Halfline;
		}else if(i & VMOT){
			k = i & ~MOTV;
			if(i & NMOT)k = -k;
			base -= k;
			em = 0;
		}else continue;
		if(base < v_sb)v_sb = base;
		if((k=base + em) > v_st)v_st = k;
	}
	nform = 0;
	setn1(wid);
	v_hp = savhp;
	level = savlevel;
	font = savfont;
	font1 = savfont1;
	mchbits();
	setwdf = 0;
}
mot(){
	register i, j;

	j = HOR;
	getch(); /*eat delim*/
	if(i = atoi()){
		if(vflag)j = VERT;
		i = makem(quant(i,j));
	}
	getch();
	vflag = 0;
	dfact = 1;
	return(i);
}
sethl(k)
int k;
{
	register i;

	i = t.Halfline;
	if(k == 'u')i = -i;
	else if(k == 'r')i = -2*i;
	vflag++;
	i = makem(i);
	vflag = 0;
	return(i);
}
makem(i)
int i;
{
	register j;

	if((j = i) < 0)j = -j;
	j = (j & ~MOTV) | MOT;
	if(i < 0)j |= NMOT;
	if(vflag)j |= VMOT;
	return(j);
}
casefp(){
	register i, j;

	skip();
	if(((i = (getch() & CMASK) - '0' -1) < 0) || (i >3))return;
	if(skip() || !(j = getrq()))return;
	fontlab[i] = j;
}
casevs(){
	register i;

	skip();
	vflag++;
	dfact = INCH; /*default scaling is points!*/
	dfactd = 72;
	res = VERT;
	i = inumb(&lss);
	if(nonumb)i = lss1;
	if(i < VERT)i = VERT;
	lss1 = lss;
	lss = i;
}
xlss(){
	register i, j;

	getch();
	dfact = lss;
	i = quant(atoi(),VERT);
	dfact = 1;
	getch();
	if((j = i) < 0)j = -j;
	setch0(((j & 03700)<<3) | HX);
	if(i < 0) setch0(tch[0] | 040000);
	return(((j & 077)<<9) | LX);
}
casebd()
		/* this code for bold overstrike courtesy of cb btl */
{
	register i, j, k;

	k = 0;
bd0:
	if (skip() || !(i = getrq()) ||
	    ((j = find(i,fontlab)) == -1)) {
		if (k) goto bd1;
		  else return;	}
	if (j == (smnt - 1)) {
	    k = smnt;
	    goto bd0;	}
	if (k) {
	    sbold = j + 1;
	    j = k - 1;	}
bd1:
	skip();
	noscale++;
	k = atoi();
	bdtab[j] = ((k < 0) || (k > 50)) ? 0 : k;
	noscale = 0;
}
caseps(){}
caselg(){}
casecs(){}
casess(){}
