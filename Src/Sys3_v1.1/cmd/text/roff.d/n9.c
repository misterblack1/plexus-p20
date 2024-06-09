#include "tdef.hd"
#include "strs.hd"
#ifdef NROFF
#include "tw.hd"
extern struct ttable t;
#endif
/*
troff9.c

misc functions
*/

#ifndef INCORE
extern struct envblock eblock;
#else
extern struct envblock eblock[NEV];
extern int ev;
#endif
extern struct datablock dblock;

extern int rptbuf[];
extern int *cstk[], cstkl;
extern int ch_CMASK;
extern long atoi0();
extern int cbuf[];
extern int ch;
extern int dfact;
extern int vflag;
extern int fc;
extern int padc;
extern int nlflg;
extern int tabch, ldrch;
setline(){
	register *i, length, c;
	int w, cnt, delim, rem, temp;

	if((delim = getch()) & MOT)return;
		else delim &= CMASK;
	vflag = 0;
	dfact = EM;
	length = quant(atoi(),HOR);
	dfact = 1;
	if(!length){
		eat(delim);
		return;
	}
s0:
	if ((c = getch()) && (ch_CMASK == delim)) {
		ch = c;
		c = 0204 | chbits;	}
	  else if (ch_CMASK == FILLER) goto s0;
	w = width(c);
	i = cbuf;
	if(length < 0){
		*i++ = makem(length);
		length = -length;
	}
	if(!(cnt = length/w)){
		*i++ = makem(-(temp = ((w-length)/2)));
		*i++ = c;
		*i++ = makem(-(w - length - temp));
		goto s1;
	}
	if(rem = length%w){
		switch(c & CMASK){
			case 0204: /*rule*/
			case 0224: /*underrule*/
			case 0276: /*root en*/
				*i++ = c | ZBIT;
			default:
				*i++ = makem(rem);
		}
	}
	while (cnt--) *i++ = c;
s1:
	*i++ = 0;
	eat(delim);
	setcp(cbuf);
}
eat(c)
int c;
{
	do getch();
	    while ((ch_CMASK != c) && (ch_CMASK != '\n'));
}
setov(){
	register i, j, k;
	int *p, delim, o[NOV], w[NOV];

	if((delim = getch()) & MOT)return;
		else delim &= CMASK;
	for (k=0; ((k<NOV) && (i = getch()) && (ch_CMASK != delim) &&
		(ch_CMASK != '\n')); k++){
			o[k] = i;
			w[k] = width(i);
	}
	o[k] = w[k] = 0;
	if(o[0])
	    do {
		j = 0;
		for(k=1; o[k] ; k++)
			if(w[k-1] < w[k]){
				j++;
				i = w[k];
				w[k] = w[k-1];
				w[k-1] = i;
				i = o[k];
				o[k] = o[k-1];
				o[k-1] = i;	}}
	    while (j);
	else return;
	p = cbuf;
	for(k=0; o[k]; k++){
		*p++ = o[k];
		*p++ = makem(-((w[k]+w[k+1])/2));
	}
	*p++ = makem(w[0]/2);
	*p = 0;
	setcp(cbuf);
}
setbra(){
	register i, *j;
	int cnt, delim, dwn;

	if((delim = getch()) & MOT)return;
		else delim &= CMASK;
	j = cbuf + 1;
	cnt = 0;
#ifdef NROFF
	dwn = (2*t.Halfline) | MOT | VMOT;
#endif
#ifndef NROFF
	dwn = EM | MOT | VMOT;
#endif
	while ((i = getch()) && (ch_CMASK != delim) && (ch_CMASK != '\n') &&
		(j <= (cbuf+NC-4))){
		*j++ = i | ZBIT;
		*j++ = dwn;
		cnt++;
	}
	if(--cnt < 0)return;
		else if (!cnt){
			ch = *(j-2);
			return;
	}
	*j = 0;
#ifdef NROFF
	*--j = *cbuf = (cnt*t.Halfline) | MOT | NMOT | VMOT;
#endif
#ifndef NROFF
	*--j = *cbuf = (cnt*EM)/2 | MOT | NMOT | VMOT;
#endif
	*--j &= ~ZBIT;
	setcp(cbuf);
}
setvline(){
	register i, c, *k;
	int cnt, neg, rem, ver, delim;

	if((delim = getch()) & MOT)return;
		else delim &= CMASK;
	dfact = lss;
	vflag++;
	i = quant(atoi(),VERT);
	dfact = 1;
	if(!i){
		eat(delim);
		vflag = 0;
		return;
	}
	if ((c = getch()) && (ch_CMASK == delim)) {
		c = 0337 | chbits;	/*default box rule*/
	}else getch();
	c |= ZBIT;
	neg = 0;
	if(i < 0){
		i = -i;
		neg = NMOT;
	}
#ifdef NROFF
	ver = 2*t.Halfline;
#endif
#ifndef NROFF
	ver = EM;
#endif
	cnt = i/ver;
	rem = makem(i%ver) | neg;
	ver = makem(ver) | neg;
	k = cbuf;
	if(!neg)*k++ = ver;
	if(rem & ~MOTV){
		*k++ = c;
		*k++ = rem;
	}
	while((k < (cbuf+NC-3)) && cnt--){
		*k++ = c;
		*k++ = ver;
	}
	*(k-2) &= ~ZBIT;
	if(!neg)k--;
	*k = 0;
	setcp(cbuf);
	vflag = 0;
}
casefc(){

	fc = IMP;
	padc = ' ';
	if (skip() || (getch() & MOT) || (ch_CMASK == '\n')) return;
	fc = ch_CMASK;
	if(skip() || (ch & MOT) || ((ch &= CMASK) == fc))return;
	padc = ch;
}
setfield(x)
int x;
{
	register i, j, *fp;
	int length, ws, npad, temp, type, rchar;
	int **pp, *padptr[NPP];
	static int fbuf[FBUFSZ];
	int savfc, savtc, savlc;

	if(x == tabch) rchar = tabc | chbits;
	else if(x ==  ldrch) rchar = dotc | chbits;
	temp = npad = ws = 0;
	savfc = fc; savtc = tabch; savlc = ldrch;
	tabch = ldrch = fc = IMP;
	for(j=0;1;j++){
		if((tabtab[j] & TMASK)== 0){
			if(x==savfc)prstr("Zero field width.\n");
			j = 0;
			goto rtn;
		}
		if((length = ((tabtab[j] & TMASK) - v_hp)) > 0 )break;
	}
	type = tabtab[j] & (~TMASK);
	fp = fbuf;
	pp = padptr;
      if(x == savfc){while(1){
		if ((i = getch()) && (ch_CMASK == padc)) {
			npad++;
			*pp++ = fp;
			if(pp > (padptr + NPP - 1))break;
			goto s1;
		}else if(ch_CMASK == savfc) break;
			else if(ch_CMASK == '\n'){
				temp = ch_CMASK;
				nlflg = 0;
				break;
			}
		ws += width(i);
	s1:
		*fp++ = i;
		if(fp > (fbuf + FBUFSZ -3))break;
	}
	if(!npad){
		npad++;
		*pp++ = fp;
		*fp++ = 0;
	}
	*fp++ = temp;
	*fp++ = 0;
	temp = i = (j = length-ws)/npad;
	i = (i/HOR)*HOR;
	if((j -= i*npad) <0)j = -j;
	i = makem(i);
	if(temp <0)i |= NMOT;
	for(;npad > 0; npad--){
		*(*--pp) = i;
		if(j){
			j -= HOR;
			(*(*pp)) += HOR;
		}
	}
	setcp(fbuf);
	j = 0;
      }else if(type == 0){
	/*plain tab or leader*/
		if((j = width(rchar)) != 0)	{
			fp = rptbuf;
			i = ((length/j) > (RPTLEN-1)) ?
				RPTLEN - 1 : length / j;
			for (;i;i--)
				*fp++ = rchar;
			*fp = 0;
			setrp(rptbuf);	/* setup repeat string */
			length %= j;	}
		if(length)j = length | MOT;
		else j = getch0();
      }else{
	/*center tab*/
	/*right tab*/
		while ((i = getch()) && (ch_CMASK != savtc) &&
		    (ch_CMASK != '\n') && (ch_CMASK != savlc)) {
			ws += width(i);
			*fp++ = i;
			if(fp > (fbuf +FBUFSZ - 3)) break;
		}
		*fp++ = i;
		*fp++ = 0;
		if(type == RTAB)length -= ws;
		else length -= ws/2; /*CTAB*/
		if(((j = width(rchar)) != 0) && (length > 0))	{
			fp = rptbuf;
			i = ((length/j) > (RPTLEN-1)) ?
				RPTLEN - 1 : length / j;
			for (;i;i--)
				*fp++ = rchar;
			*fp = 0;
			setrp(rptbuf);	/* setup repeat string */
			length %= j;	}
		length = (length/HOR)*HOR;
		j = makem(length);
		setcp(fbuf);
		nlflg = 0;
	}
rtn:
	fc = savfc; tabch = savtc; ldrch = savlc;
	return(j);
}
