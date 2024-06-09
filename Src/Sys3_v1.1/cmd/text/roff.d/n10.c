#include "tdef.hd"
#ifdef unix
#include "sys/ioctl.h"
#include "termio.h"
#endif
#include "strs.hd"
#include "tw.hd"
extern struct ttable t;
/*
nroff10.c

Device interfaces
*/

extern struct d d[NDI], *dip;
#ifndef INCORE
extern struct envblock eblock;
#else
extern struct envblock eblock[NEV];
extern int ev;
#endif

#ifdef ebcdic
extern char *fname();
#endif
extern int stop;
extern int bdtab[];
extern char obuf[];
extern char *obufp;
extern int xfont;
extern int esc;
extern int lead;
extern int oline[];
extern int *olinep;
extern int ulfont;
extern int esct;
extern int ttysave;
extern struct termio ttys;
extern char termtab[];
extern filedes ptid;
extern int waitf;
#ifndef SMALL
extern int pipeflg;
#endif
extern int eqflg;
extern int hflg;
extern int ascii;
int dtab;
int bdmode;
int itmode;
int plotmode;
ptinit(){
	register i;
	register char **p;
	char *q;
	int strsize;
#ifndef INCORE
	extern char *setbrk();
#else
static char termstbuf[TSTBUF];	/* terminal strings buffer */
#endif
#ifdef tso
	filedes j;
#endif

#ifdef unix
	if((i = open(termtab,0)) < 0){
#endif
#ifdef tso
	if ((j=fopen(fname(termtab),"r,BINARY")) == NULL) {
#endif
		prstr("Non-existent terminal type\n");
		exit(-1);
	}
#ifdef unix
	read(i, &strsize, sizeof(strsize));
	read(i,(char *)&t.bset, sizeof(struct ttable));
#endif
#ifdef tso
	fread((char *)&strsize,sizeof(strsize),1,j);
	fread((char *)&t.bset,sizeof(struct ttable),1,j);
#endif
#ifndef INCORE
	q = setbrk(strsize);
#else
	if (TSTBUF < strsize)	{
		prstr("terminal table too big");
		exit(-1);	}
	q = termstbuf;		/* terminal table */
#endif
#ifdef unix
	read(i,q,strsize);
#endif
#ifdef tso
	fread(q,strsize,1,j);
#endif
	for(p = &t.twinit; p < &t.zzz; p++)
		*p = q + ((int) *p);	/* relocate pointers */

	sps = EM;
	ics = EM*2;
	dtab = 8 * t.Em;
	for(i=0; i<16; i++)tabtab[i] = dtab * (i+1);
	if(eqflg)t.Adj = t.Hor;
}
twdone(){
	obufp = obuf;
	oputs(t.twrest);
	flusho();
#ifndef SMALL
	if(pipeflg){
		close(ptid);
		wait(&waitf);
	}
#endif
#ifdef tso
	fclose(ptid);
#endif
#ifdef unix
	if(ttysave != -1) {
		ttys.c_oflag = ttysave;
		ioctl(1, TCSETAW, &ttys);
	}
#endif
}
ptout(i)
int i;
{
	*olinep++ = i;
	if(olinep >= &oline[LNSIZE])olinep--;
	if((i&CMASK) != '\n')return;
	olinep--;
	lead += dip->blss + lss - t.Newline;
	dip->blss = 0;
	esct = esc = 0;
	if(olinep>oline){
		move();
		ptout1();
		oputs(t.twnl);
	}else{
		lead += t.Newline;
		move();
	}
	lead += dip->alss;
	dip->alss = 0;
	olinep = oline;
}
ptout1()
{
	register i, k;
	register char *codep;
	extern char *plot();
	int *q, w, j, phyw;

      for(q=oline; q<olinep; q++){
	if((i = *q) & MOT){
		j = i & ~MOTV;
		if(i & NMOT)j = -j;
		if(i & VMOT)lead += j;
		else esc += j;
		continue;
	}
	if((k = (i & CMASK)) <= 040){
		if (k == ' ') {	/* space */
			esc += t.Char;
			continue;	}
		switch (k)	{
			case 033:  codep = "\000\033";	/* ascii esc */
				   break;
			case 007:  codep = "\000\007";	/* ascii bel */
				   break;
			case COLON: codep = "\000\013";	/* lem's character */
				    break;
			default:  codep = (char *)0;
				  break;	}}
	    else codep = t.codetab[k-32];
	if (codep == (char *)0) continue;
	w = t.Char * (*codep++ & 0177);
	phyw = w;
	if(i&ZBIT)w = 0;
	if(*codep && (esc || lead))move();
	esct += w;
	if(i&074000)xfont = (i>>9) & 03;
	if((*t.bdon) & 0377){
		if(!bdmode && (xfont == 2)){
			oputs(t.bdon);
			itmode = 0;	/* now in roman not italic */
			bdmode++;
		}
		if(bdmode && (xfont != 2)){
			oputs(t.bdoff);
			bdmode = 0;
		}
	}
	if ((*t.iton) & 0377)	{
		if (!itmode && (xfont == 1))	{	/* enter italic */
			oputs(t.iton);
			itmode++;	}
		else if (itmode && (xfont != 1))	{	/* leave italic */
			oputs(t.itoff);
			itmode = 0;	}}

	if((xfont == ulfont) && !(*t.iton & 0377))	{
		for(k=w/t.Char;k>0;k--)oput('_');
		for(k=w/t.Char;k>0;k--)oput('\b');
	}
	if (!(*t.bdon & 0377) && ((k = bdtab[xfont]) || xfont == 2))
		k++;
	  else k = 1;
	while(*codep != 0){
		i = k;
		if(*codep & 0200){
			codep = plot(codep);
			oputs(t.plotoff);
			oput(' ');
		} else {
			if(plotmode)oputs(t.plotoff);
			if (*codep != '\b') {
			    while (i--) {
				oput(*codep);
				if (i) oput('\b');	}}
			    else oput(*codep);
			codep++;
		}
	}
	if(!w)for(k=phyw/t.Char;k>0;k--)oput('\b');
	}
}
char *plot(x)
char *x;
{
	register int i;
	register char *j, *k;

	if(!plotmode)oputs(t.ploton);
	k = x;
	if((*k & 0377) == 0200)k++;
	for(; *k; k++){
		if(*k & 0200){
			if(*k & 0100){
				if(*k & 040)j = t.up; else j = t.down;
			}else{
				if(*k & 040)j = t.left; else j = t.right;
			}
			if(!(i = *k & 037))return(++k);
			while(i--)oputs(j);
		}else oput(*k);
	}
	return(k);
}
move(){
	register k;
	register char *i, *j;
	char *p, *q;
	int iesct, dt;

	iesct = esct;
	if(esct += esc)i = "\0"; else i = "\012\0" /*\n*/;
	j = t.hlf;
	p = t.right;
	q = t.down;
	if(lead){
		if(lead < 0){
			lead = -lead;
			i = t.flr;
		/*	if(!esct)i = t.flr; else i = "\0";*/
			j = t.hlr;
			q = t.up;
		}
		if(*i & 0377){
			k = lead/t.Newline;
			lead = lead%t.Newline;
			while((k--)>0)oputs(i);
		}
		if(*j & 0377){
			k = lead/t.Halfline;
			lead = lead%t.Halfline;
			while((k--)>0)oputs(j);
		}
		else { /* no half-line forward, not at line begining */
			k = lead/t.Newline;
			lead = lead%t.Newline;
			if (k>0) esc=esct;
			i = "\012" /*\n*/;
			while ((k--)>0) oputs(i);
		}
	}
	if(esc){
		if(esc < 0){
			esc = -esc;
			j = "\010" /*\b*/;
			p = t.left;
		}else{
			j = "\040"/*space*/;
			if(hflg) while (((dt = dtab - (iesct%dtab)) <= esc) &&
				(!(dt%t.Em)))	{
				oput(TAB);
				esc -= dt;
				iesct += dt;
			}
		}
		k = esc/t.Em;
		esc = esc%t.Em;
		while((k--)>0)oputs(j);
	}
	if((*t.ploton & 0377) && (esc || lead)){
		if(!plotmode)oputs(t.ploton);
		esc /= t.Adj;
		lead /= t.Vert;
		while((esc--)>0)oputs(p);
		while((lead--)>0)oputs(q);
		oputs(t.plotoff);
	}
	esc = lead = 0;
}
ptlead(){move();}
dostop(){
	char junk;

	flusho();
	aprstr("\007");
#ifdef unix
	while ((read(2,&junk,1) != -1) && (junk != '\n')) ;
#endif
#ifdef tso
	while ((fread((char *)&junk,1,1,stderr)!=EOF) && (junk!='\n'));
#endif
}
