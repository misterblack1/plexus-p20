/*	Plexus - Sys3 - August 1982	*/

static char c22_c[] = "@(#)c22.c	1.2";

/*
 *	subroutines for z8000/pcc optimizer
 */

#include "c2.h"

equop(ap1, p2)
struct node *ap1, *p2;
{
	register char *cp1, *cp2;
	register struct node *p1;

	p1 = ap1;
	if (p1->op!=p2->op || p1->subop!=p2->subop)
		return(0);
	if (p1->op>0 && p1->op<LD)
		return(0);
	cp1 = p1->code;
	cp2 = p2->code;
	if (cp1==0 && cp2==0)
		return(1);
	if (cp1==0 || cp2==0)
		return(0);
	while (*cp1 == *cp2++)
		if (*cp1++ == 0)
			return(1);
	return(0);
}

decref(p)
register struct node *p;
{
	if (--p->refc <= 0) {
		nrlab++;
		p->back->forw = p->forw;
		p->forw->back = p->back;
	}
}

struct node *
nonlab(p)
struct node *p;
{
	while (p && p->op==LABEL)
		p = p->forw;
	return(p);
}

char *
alloc(n)
register n;
{
	register char *p;

	n++;
	n &= ~01;
	if (lasta+n >= lastr) {
		if (sbrk(2000) == (char *)-1) {
			fprintf(stderr, "C Optimizer: out of space\n");
			exit(1);
		}
		lastr += 2000;
	}
	p = lasta;
	lasta += n;
	return(p);
}

clearreg()
{
	register int i;

	for (i=0; i<NREG; i++) {       /* ERR - was 2*NREG */
		regs[i][0] = '\0';
	}
	conloc[0] = 0;
	ccloc[0] = 0;
}

savereg(ai, as)
char *as;
{
	register char *p, *s, *sp;
	register r,i;
	char buf[6];

	r = rbase(ai);
	regstype[r] = rtype(ai);
	regsdepend[r] = -1;
	sp = p = regs[r];
	s = as;
	if (source(s))
		return;
	while (*p++ = *s) {
		if( (s[0] == '*' || s[0] == '(')
		    && s[1] =='r' && (s[2] != '1' || s[3] != '4') ) {
#ifndef BUGSAVE
			register char *v,*w;
			int r1;
			/*
			 * source is indirect or indexed through a register.
			 * if the source and destination registers are
			 * different then do the save and note the
			 * dependency of the register on the source oprand reg
			 */
			v = buf;
			w = s+1;
			*v = 0;
			if( *w == 'r' ) {
				while( *v++ = *w )
					if( *w == ')' || *w++ == ',' )
						break;
				*--v = 0;
			}
			if( (r1 = isreg(buf)) < 0 || r1 == r ) {
				*sp = 0;
				return;
			}
			regsdepend[r] = r1;
#else
			*sp = 0;
			return;
#endif
		}
		if (*s++ == ',')
			break;
	}
	*--p = '\0';
}

dest(as, type)
char *as;
{
	register r;
	register char *p;
	register base;
	char buf[4];
	register maxfix,i;

	p = buf;
	if( (r=isreg(as)) >= 0 ) {
		/* check for overlap with reg pair or quad */
		base = rbase(r);
		base &= ~1; /* register pair */
		if( regstype[base] == TDREG ) {
			makereg( p, base );
			dest1(p, TREG);
		}
		base &= ~2;
		if( regstype[base] == TQREG ) {
			makereg ( p, base );
			dest1(p, TREG);
		}
		base = rbase(r);
		makereg( p, base );
		maxfix = 3;	/* in case its quad */
		dest1(p, TREG);
		switch(rtype(r)) {

		case TREG:
			break;
		case TBREG:
			maxfix = 0;
			goto fixit;
		case TDREG:
			maxfix = 1;
			goto fixit;
		case TQREG:
		fixit:
			for( i=0; i<maxfix; i++ ) {
				makereg( p, ++base );
				dest1(p, TREG);
			}
			while ((i = findrand(as, type)) >= 0)
				regs[i][0] = 0;
		}
	} else dest1(as, type);
}

dest1(as, type)
char *as;
{
	register char *s;
	register int i;
	register r;

	s = as;
	source(s);
	if ((r = isreg(s)) >= 0)
		regs[r][0] = 0;
	for (i=0; i<NREG; i++) /* ERR - was 2*NREG */
		if (*regs[i]=='*' && equstr(s, regs[i]+1))
			regs[i][0] = 0;
	while ((i = findrand(s, type)) >= 0)
		regs[i][0] = 0;
#ifndef BUGSAVE
	if(r >= 0) for(i=0; i<NREG; i++)
		if( *regs[i] && regsdepend[i] == r )
			regs[i][0] = 0;
#endif
	if ( equstr(s, conloc) )
		conloc[0] = 0;
	while (*s) {
		if ((*s=='(' && (*(s+1)!='r' || *(s+2)!='1' || *(s+3) != '4'))
		   || *s++=='*') {
			for (i=0; i<NREG; i++) { /* ERR - was 2*NREG */
				if (regs[i][0] != '$')
					regs[i][0] = 0;
			}
			conloc[0] = 0;
			return;
		}
	}
}

singop(ap)
struct node *ap;
{
	register char *p1, *p2;

	p1 = ap->code;
	p2 = regs[RT1];
	while (*p2++ = *p1++);
	regs[RT2][0] = 0;
}


dualop(ap)
struct node *ap;
{
	register char *p1, *p2;
	register struct node *p;

	p = ap;
	p1 = p->code;
	p2 = regs[RT1];
	while (*p1 && *p1!=',')
		*p2++ = *p1++;
	*p2++ = 0;
	p2 = regs[RT2];
	*p2 = 0;
	if (*p1++ !=',')
		return;
	while (*p2++ = *p1++);
}

findrand(as, type)
char *as;
char type;
{
	register int i;

	for (i = 0; i<NREG; i++) {
		if( equstr(regs[i], as) && regstype[i] == type )
			return(i);
	}
	return(-1);
}

isreg(as)
char *as;
{
	register char *s;
	register v;

	s = as;
	if(*s++ != 'r')
		return( -1 );
	if ( (v = shortval(s)) <= 13 )
		return( regnum( v, TREG ) );
	v = shortval(&s[1]);
	if (*s == 'l' && v <= 7 )
		return( regnum( v, TBREG ) );
	if (*s == 'h' && v <= 7 )
		return( regnum( v, THBREG ) );
	if(*s == 'r' && v < 13 )
		return( regnum( v, TDREG ) );
	if(*s == 'q' && v < 7 )
		return( regnum( v, TQREG ) );
	return(-1);
}

/*
 *	isreg14 can only be used as true or false.
 *	the register value returned is invalid in most computaions
 */
isreg14(as)
char *as;
{
	register char *s;
	register v;

	s = as;
	if (s[0]=='r' && (v = shortval(&s[1])) <= 14)
		return( v );
	return(-1);
}

isanyreg(as)
char *as;
{
	register char *s;
	register v;

	s = as;
	if (s[0]=='r' && (v = shortval(&s[1])) <= 14)
		return( v );
	return(-1);
}

check()
{
	register struct node *p, *lp;

	lp = &first;
	for (p=first.forw; p!=0; p = p->forw) {
		if (p->back != lp)
			abort();
		lp = p;
	}
}

source(ap)
char *ap;
{
	register char *p1, *p2;

/* used to identify '-(...', '*-(...' and '...+' not used on z8000
                       returns 'true' if one of the above
	p1 = ap;
	p2 = p1;
	if (*p1==0)
		return(0);
	while (*p2++);
	if (*p1=='-' && *(p1+1)=='('
	 || *p1=='*' && *(p1+1)=='-' && *(p1+2)=='('
	 || *(p2-2)=='+') {
		while (*p1 && *p1++!='r');
		if (*p1>='0' && *p1<='4')
			regs[*p1 - '0'][0] = 0;
		return(1);
	}
*/
	return(0);
}

typeit(subop) {

	switch(subop){
	case 0:
		return(TREG);
	case BYTE:
		return(TBREG);
	case LONG:
		return(TDREG);
	case QUAD:
		return(TQREG);
	}
}

repladdr(p, f)
struct node *p;
{
	register r;
	register char *p1, *p2;
	register r1;
	static char rt1[50], rt2[50];
	register type;

#ifndef NOREPAD
	type = typeit(p->subop);
	r = r1 = -1;
	if( f & REPSRC )
		r1 = findrand(regs[RT2], type);
	if( f & REPDST )
		r = findrand(regs[RT1], type);
	if ( (r>=0 || r1>=0)
		/* avoid: cp mem,reg */
	    && !( p->op == CP && r < 0 && r1 >= 0 && isreg(regs[RT1]) < 0 ) ) {
		p2 = regs[RT1];
		for (p1 = rt1; *p1++ = *p2++;);
		if (regs[RT2][0]) {
			p1 = rt2;
			*p1++ = ',';
			for (p2 = regs[RT2]; *p1++ = *p2++;);
		} else
			rt2[0] = 0;
		if (r>=0) {
			makereg( &rt1[0], regnum(r, type) );
			nsaddr++;
		}
		if (r1>=0) {
			makereg( &rt2[1], regnum(r1, type) );
			nsaddr++;
		}
		p->code = copy(2, rt1, rt2);
	}
#endif
}

makereg(p,r)
register char *p;
register int r;
{
	register base;

	*p++ = 'r';
	switch(rtype(r)) {

	case TREG:
		break;
	case TBREG:
		*p++ = 'l';
		break;
	case THBREG:
		*p++ = 'h';
		break;
	case TDREG:
		*p++ = 'r';
		break;
	case TQREG:
		*p++ = 'q';
		break;
	}
	base = rbase(r);
	if( base>9 ) {
		*p++ = '1';
		base -= 10;
	}
	*p++ = base + '0';
	*p = 0;
}

movedat()
{
	register struct node *p1, *p2;
	struct node *p3;
	register seg;
	struct node data;
	struct node *datp;

	if (first.forw == 0)
		return;
	if (lastseg != TEXT && lastseg != -1) {
		p1 = (struct node *)alloc(sizeof(first));
		p1->op = lastseg;
		p1->subop = 0;
		p1->code = NULL;
		p1->forw = first.forw;
		p1->back = &first;
		first.forw->back = p1;
		first.forw = p1;
	}
	datp = &data;
	for (p1 = first.forw; p1!=0; p1 = p1->forw) {
		if (p1->op == DATA) {
			p2 = p1->forw;
			while (p2 && p2->op!=TEXT)
				p2 = p2->forw;
			if (p2==0)
				break;
			p3 = p1->back;
			p1->back->forw = p2->forw;
			p2->forw->back = p3;
			p2->forw = 0;
			datp->forw = p1;
			p1->back = datp;
			p1 = p3;
			datp = p2;
		}
	}
	if (data.forw) {
		datp->forw = first.forw;
		first.forw->back = datp;
		data.forw->back = &first;
		first.forw = data.forw;
	}
	seg = lastseg;
	for (p1 = first.forw; p1!=0; p1 = p1->forw) {
		if (p1->op==TEXT||p1->op==DATA||p1->op==BSS) {
			if (p2 = p1->forw) {
				if (p2->op==TEXT||p2->op==DATA||p2->op==BSS)
					p1->op  = p2->op;
			}
			if (p1->op == seg || p1->forw&&p1->forw->op==seg) {
				p1->back->forw = p1->forw;
				p1->forw->back = p1->back;
				p1 = p1->back;
				continue;
			}
			seg = p1->op;
		}
	}
}

redunbr(p)
register struct node *p;
{
	register struct node *p1;
	register char *ap1;
	char *ap2;

	if ((p1 = p->ref) == 0)
		return;
	p1 = nonlab(p1);
	if (p1->op==TST) {
		singop(p1);
		savereg(RT2, "$0");
	} else if (p1->op==CP)
		dualop(p1);
	else
		return;
	if (p1->forw->op!=CJP)
		return;
	ap1 = findcon(RT1,typeit(p1->subop));
	ap2 = findcon(RT2,typeit(p1->subop));
	p1 = p1->forw;
	if (compare(p1->subop, ap1, ap2)>0) {
		nredunj++;
		nchange++;
		decref(p->ref);
		if( p1->ref ) {
			p->ref = p1->ref;
			p->labno = p1->labno;
			p->ref->refc++;
		} else { /* must be jpr cc,cret */
			p->ref = 0;
			p->labno = 0;
			p->code = copy(1,p1->code);
		}
	}
}

char *
findcon(i,type)
{
	register char *p;
	register r;

	p = regs[i];
	if (*p=='$')
		return(p);
	if ((r = isreg(p)) >= 0 && rtype(r) == regstype[rbase(r)])
		return(regs[rbase(r)]);
	if (equstr(p, conloc) && contype == type)
		return(conval);
	return(p);
}

compare(oper, cp1, cp2)
char *cp1, *cp2;
{
	register unsigned n1, n2;

	if (*cp1++ != '$' || *cp2++ != '$')
		return(-1);
	n1 = streval(&cp2);
	n2 = n1;
	n1 = streval(&cp1);
	if (*cp1=='+')
		cp1++;
	if (*cp2=='+')
		cp2++;
	do {
		if (*cp1++ != *cp2)
			return(-1);
	} while (*cp2++);
	switch(oper) {

	case JEQ:
		return(n1 == n2);
	case JNE:
		return(n1 != n2);
	case JLE:
		return((int)n1 <= (int)n2);
	case JGE:
		return((int)n1 >= (int)n2);
	case JLT:
		return((int)n1 < (int)n2);
	case JGT:
		return((int)n1 > (int)n2);
	case JLO:
		return(n1 < n2);
	case JHI:
		return(n1 > n2);
	case JLOS:
		return(n1 <= n2);
	case JHIS:
		return(n1 >= n2);
	}
	return(-1);
}

unsigned
streval(pp)
char **pp;
{
	register char *p;
	register unsigned v = 0;

	p = *pp;
	while( *p >= '0' && *p <= '9' ) {
		v = 10*v + (*p++ - '0');
	}
	*pp = p;
	return( v );
}

setcon(ar1, ar2, type)
char *ar1, *ar2;
{
	register char *cl, *cv, *p;

	cl = ar2;
	cv = ar1;
	if (*cv != '$')
		return;
	if (!natural(cl))
		return;
	p = conloc;
	while (*p++ = *cl++);
	p = conval;
	while (*p++ = *cv++);
	contype = type;
}

equstr(ap1, ap2)
char *ap1, *ap2;
{
	char *p1, *p2;

	p1 = ap1;
	p2 = ap2;
	do {
		if (*p1++ != *p2)
			return(0);
	} while (*p2++);
	return(1);
}

equsubstr(ap1,ap2)
char *ap1,*ap2;
{
register char *p1 = ap1;
register char *p2 = ap2;

	while( *p1 != '\0' ) {
		if( *p1++ != *p2++ )
			return( 0 );
	}
	return( 1 );
}
	
setcc(ap)
char *ap;
{
	register char *p, *p1;

	p = ap;
	if (!natural(p)) {
		ccloc[0] = 0;
		return;
	}
	p1 = ccloc;
	while (*p1++ = *p++);
}

natural(ap)
char *ap;
{
	register char *p;

	p = ap;
	if (*p=='*')
		return(0);
	while (*p++);
	p--;
	if (*--p ==')' && *--p != '4' && *--p != '1')
		return(0);
	return(1);
}

/* look for small constants */
shortval(ap)
register char *ap;
{
register int v = 0;

	while( *ap>='0' && *ap<='9' ) {
		v = 10*v + *ap - '0';
		if( v > 257 ) return(257);
		ap++;
	}
	if( *ap != '\0' )
		return(257);
	else
		return(v);
}
