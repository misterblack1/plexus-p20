#include "tdef.hd"
#include "strs.hd"
#ifdef NROFF
#include "tw.hd"
extern struct ttable t;
#endif
/*
troff4.c

number registers, conversion, arithmetic
*/

extern struct s *frame;
#ifndef INCORE
extern struct envblock eblock;
#else
extern struct envblock eblock[NEV];
extern int ev;
#endif
extern struct d d[NDI], *dip;
extern struct datablock dblock;

extern int tch[];
extern int cd;
extern int freenslot;
extern int *cstk[], cstkl;
extern int ch_CMASK;
extern int dilev;
extern int ascii;
extern int cbuf[NC];
extern int *cp;
extern int ch;
extern int lgf;
extern int pl;
extern int ralss;
extern int totout;
extern int nrbits;
extern int nonumb;
extern int vflag;
extern int noscale;
extern int dfact;
extern int dfactd;
extern int po;
extern int nform;
extern int bdtab[];
extern int res;
extern int cwidth;
extern int dotT;
extern int print;
int regcnt = NNAMES;
setn()
{
	register i,j;
	int f;

	f = nform = 0;
	if (getch() && (ch_CMASK == '+')) f = 1;
		else if(ch_CMASK== '-')f = -1;
			else ch = ch_CMASK;
	if((i=getsn()) == 0)return;
	if((i & 0177) == '.')switch(i>>BYTE){
		case 's': i = pts & 077;	break;
		case 'v': i = lss;		break;
		case 'f': i = font + 1;	break;
		case 'p': i = pl;		break;
		case 't':  i = findt1();	break;
		case 'o': i = po;		break;
		case 'l': i = ll;		break;
		case 'i': i = in;		break;
		case '$': i = frame->nargs;		break;
		case 'A': i = ascii;		break;
		case 'c': i = cd;		break;
		case 'n': i = lastl;		break;
		case 'a': i = ralss;		break;
		case 'h': i = dip->hnl;	break;
		case 'd':
			if(dip != d)i = dip->dnl; else i = v_nl;
			break;
		case 'u': i = fi;		break;
		case 'j': i = ad + 2*admod;	break;
		case 'w': i = cwidth;		break;
		case 'x': i = nel;	break;
		case 'y': i = un;		break;
		case 'T': i = dotT;		break; /*-Tterm used in nroff*/
		case 'V': i = VERT;		break;
		case 'H': i = HOR;		break;
		case 'k': i = ne;		break;
		case 'P': i = print;		break;
		case 'L': i = ls;		break;
		case 'R': i = NN - regcnt;	break;
		case 'z': i = dip->curd;
			cbuf[0] = i & BMASK;
			cbuf[1] = (i >> BYTE) & BMASK;
			cbuf[2] = 0;
			setcp(cbuf);
			return;
		case 'b': i = bdtab[font];		break;

		default:
			goto s0;
	}
	else{
s0:
		if((j=findr(i)) == -1)i = 0;
		else{
			i = (vlist[j] = (vlist[j] + inc[j]*f));
			nform = fmt[j];
		}
	}
	setn1(i);
	setcp(cbuf);
}
setn1(i)
int i;
{
	extern int wrc();

	cp = cbuf;
	nrbits = 0;
	fnumb(i,wrc);
	*cp = 0;
	setcp(cbuf);
}
findr(i)
int i;
{
	register j;
	static int numerr = 0;

	if (!i) return -1;
	if ((j = fnrname(i)) != -1) return j;

	if (freenslot != -1)	{
		r[freenslot] = i;
		regcnt++;
		return freenslot;	}

	if(!numerr)prstrfl("Too many number registers.\n");
	if(++numerr > 1)done2(04); else edone(04);
	return -1;
}
fnumb(i,f)
int i, (*f)();
{
	register j;

	j = 0;
	if(i < 0){
		j = (*f)('-' | nrbits);
		i = -i;
	}
	switch(nform){
		default:
		case '1':
		case 0: return(decml(i,f) + j);
		case 'i':
		case 'I': return(roman(i,f) + j);
		case 'a':
		case 'A': return(abc(i,f) + j);
	}
}
decml(i,f)
int i, (*f)();
{
	register j,k;

	k = 0;
	nform--;
	if((j=i/10) || (nform > 0))k = decml(j,f);
	return(k + (*f)((i%10 + '0') | nrbits));
}
roman(i,f)
int i, (*f)();
{

	if(!i)return((*f)('0' | nrbits));
	if(nform == 'i')return(roman0(i,f,"\151\170\143\155\172"/*ixcmz*/,"\166\154\144\167"/*vldw*/));
	else return(roman0(i,f,"\111\130\103\115\132"/*IXCMZ*/,"\126\114\104\127"/*VLDW*/));
}
roman0(i,f,onesp,fivesp)
int i, (*f)();
char *onesp, *fivesp;
{
	register q, rem, k;

	k = 0;
	if(!i)return(0);
	k = roman0(i/10,f,onesp+1,fivesp+1);
	q = (i=i%10)/5;
	rem = i%5;
	if(rem == 4){
		k += (*f)(*onesp | nrbits);
		if(q)i = *(onesp+1);
			else i = *fivesp;
		return(k += (*f)(i | nrbits));
	}
	if(q)k += (*f)(*fivesp | nrbits);
	while(--rem >= 0)
		k += (*f)(*onesp | nrbits);
	return(k);
}
abc(i,f)
int i, (*f)();
{
	if(!i)return((*f)('0' | nrbits));
	else return(abc0(i-1,f));
}
abc0(i,f)
int i, (*f)();
{
	register j, k;

	k = 0;
	if(j=i/26)k = abc0(j-1,f);
	return(k + (*f)((i%26 + nform) | nrbits));
}
wrc(i)
int i;
{
	if(cp >= &cbuf[NC])return(0);
	*cp++ = i;
	return(1);
}
long atoi0()
{
	register ii, k, cnt;
	long i, acc;
	extern long ckph();

	i = 0; acc = 0;
	nonumb = 0;
	cnt = -1;
a0:
	cnt++;
	ii = getch();
	switch (ch_CMASK)	{
		default:
			ch = ii;
			if(cnt)break;
		case '+':
			i = ckph();
			if(nonumb)break;
			acc += i;
			goto a0;
		case '-':
			i = ckph();
			if(nonumb)break;
			acc -= i;
			goto a0;
		case '*':
			i = ckph();
			if(nonumb)break;
			acc *= i;
			goto a0;
		case '/':
			i = ckph();
			if(nonumb)break;
			if(i == 0){
				prstrfl("Divide by zero.\n");
				acc = 0;
			}else acc /= i;
			goto a0;
		case '%':
			i = ckph();
			if(nonumb)break;
			acc %= i;
			goto a0;
		case '&':	/*and*/
			i = ckph();
			if(nonumb)break;
			if((acc > 0) && (i > 0))acc = 1; else acc = 0;
			goto a0;
		case ':':	/*or*/
			i = ckph();
			if(nonumb)break;
			if((acc > 0) || (i > 0))acc = 1; else acc = 0;
			goto a0;
		case '=':
			if ((ii = getch()) && (ch_CMASK != '=')) ch = ii;
			i = ckph();
			if(nonumb){acc = 0; break;}
			if(i == acc)acc = 1;
			else acc = 0;
			goto a0;
		case '>':
			k = 0;
			if ((ii = getch()) && (ch_CMASK == '=')) k++;
				else ch = ii;
			i = ckph();
			if(nonumb){acc = 0; break;}
			if(acc > (i - k))acc = 1; else acc = 0;
			goto a0;
		case '<':
			k = 0;
			if ((ii = getch()) && (ch_CMASK == '=')) k++;
				else ch = ii;
			i = ckph();
			if(nonumb){acc = 0; break;}
			if(acc < (i + k))acc = 1; else acc = 0;
			goto a0;
		case ')': break;
		case '(':
			acc = atoi0();
			goto a0;
	}
	return(acc);
}
long ckph(){
	extern long atoi0();
	extern long atoi1();

	if ((ch = getch()) && (ch_CMASK != '(')) return ((long) atoi1());
	else{
		ch = 0;
		return ((long) atoi0());
	}
}
long atoi1()
{
	register i, j, digits;
	long acc;
	int neg, aabs, field;

	neg = aabs = field = digits = 0;
	acc = 0;
a0:
	i = getch();
	switch (ch_CMASK) {
		default:
			ch = i;
			break;
		case '+':
			goto a0;
		case '-':
			neg = 1;
			goto a0;
		case '|':
			aabs = 1 + neg;
			neg = 0;
			goto a0;
	}
a1:
	while ((i = getch()) && ((j = ch_CMASK - '0') >= 0) && (j <= 9)) {
		field++;
		digits++;
		acc = 10*acc + j;
	}
	if(ch_CMASK == '.'){
		field++;
		digits = 0;
		goto a1;
	}
	ch = i;
	if(!field)goto a2;
	i = getch();
	switch (ch_CMASK) {
		case 'u':
			i = j = 1;
			break;
		case 'v':	/*VSs - vert spacing*/
			j = lss;
			i = 1;
			break;
		case 'm':	/*Ems*/
			j = EM;
			i = 1;
			break;
		case 'n':	/*Ens*/
			j = EM;
#ifndef NROFF
			i = 2;
#endif
#ifdef NROFF
			i = 1;	/*Same as Ems in NROFF*/
#endif
			break;
		case 'p':	/*Points*/
			j = INCH;
			i = 72;
			break;
		case 'i':	/*Inches*/
			j = INCH;
			i = 1;
			break;
		case 'c':	/*Centimeters*/
			j = INCH*50;
			i = 127;
			break;
		case 'P':	/*Picas*/
			j = INCH;
			i = 6;
			break;
		default:
			j = dfact;
			ch = i;
			i = dfactd;
	}
	if(neg) acc = -acc;
	if(!noscale){
		acc = (acc*j)/i;
	}
	if((field != digits) && (digits > 0))while(digits--)acc /= 10;
	if(aabs){
		if(dip != d)j = dip->dnl; else j = v_nl;
		if(!vflag)j = v_hp;
		if(aabs == 2)j = -j;
		acc -= j;
	}
a2:
	nonumb = !field;
	return(acc);
}
caserr(){
	register i;

	lgf++;
	while (!skip() && (i = getrq()))
	    if ((i = fnrname(i)) != -1)	{	/* do this number reg */
		r[i] = -1;
		vlist[i] = inc[i] = fmt[i] = 0;	}
}
casenr(){
	register i, j;

	lgf++;
	skip();
	if((i = findr(getrq())) == -1)goto rtn;
	skip();
	j = inumb(&vlist[i]);
	if(nonumb)goto rtn;
	vlist[i] = j;
	skip();
	j = atoi();
	if(nonumb)goto rtn;
	inc[i] = j;
rtn:
	return;
}
caseaf(){
	register i, j, k;

	lgf++;
	if(skip() || !(i = getrq()) || skip())return;
	k = 0;
	if(!alph(j=getch())){
		ch = j;
		while (getch() && ((j = ch_CMASK) >= '0') && (j <= '9'))
			k++;
		if (k > 20) k = 20;
	}
			/* force unusual formats into '1' form */
	else switch (j & BMASK) {
		case 'A':
		case 'a':
		case 'I':
		case 'i':	break;
		default:	k++;	}
	if(!k)k=j;
	fmt[findr(i)] = k & BMASK;
}
setaf()		/* return the format of a number reg - in the .af form */
{
	register int i, j;
	register int *p;

	if ((i=fnrname(getsn())) == -1) return;	/* no register */
	if (fmt[i] > 20)	/* A, a, I, or i. NOTE: 20 is arbitrary */
		setch0(fmt[i])
	    else	{	/* 0001 format */
		p = cbuf;
		for (j=(fmt[i])?fmt[i]:1; j; j--)
			*p++ = '0';
		*p = 0;
		setcp(cbuf);	}
}
vnumb(i)
int *i;
{
	vflag++;
	dfact = lss;
	res = VERT;
	return(inumb(i));
}
hnumb(i)
int *i;
{
	dfact = EM;
	res = HOR;
	return(inumb(i));
}
inumb(n)
int *n;
{
	register i, f;

	f = 0;
	if(n){
		if ((i = getch()) && (ch_CMASK == '+')) f = 1;
			else if (ch_CMASK == '-') f = -1;
				else ch = i;	}
	i = atoi();
	if(n && f)i = *n + f*i;
	i = quant(i,res);
	vflag = 0;
	res = dfactd = dfact = 1;
	if(nonumb)i = 0;
	return(i);
}
quant(n,m)
int n, m;
{
	register i, neg;

	neg = 0;
	if(n<0){
		neg++;
		n = -n;
	}
	i = n/m;
	if((n - m*i) > (m/2))i += 1;
	i *= m;
	if(neg && (i != 0))i = -i;
	return(i);
}
