/*
 *	subroutines for z8000/pcc optimizer
 */

#include "c2.h"

char regbit[] = {
	0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };

#ifndef SMALL

#define isdst(p,r) (p->dst.dir & regbit[r])
#define usedst(p,r) (p->dst.ind & regbit[r])
#define issrc(p,r) (p->src.dir & regbit[r])
#define usesrc(p,r) (p->src.ind & regbit[r])

#endif

islbreg(as)
char *as;
{
	register char *s;
	register v;

	s = as;
	if (*s++ =='r' && *s++ == 'l' && (v = shortval(s)) <= 7)
		return( v );
	return(-1);
}

isrrreg(as)
char *as;
{
	register char *s;
	register v;

	s = as;
	if(*s++ == 'r' && *s++ == 'r' && (v = shortval(s)) < 7)
		return( v );
	return( -1 );
}
isrqreg(as)
char *as;
{
	register char *s;
	register v;

	s = as;
	if(*s++ == 'r' && *s++ == 'q' && (v = shortval(s)) < 7)
		return( v );
	return( -1 );
}

#ifndef SMALL
whregs(p)
register struct node *p;
{
	if( p->op == LDM || p->op == LDIR || p->op == 0 ) {
		p->dst.dir = 0xff;
		p->dst.ind = 0xff;
		p->src.dir = 0xff;
		p->src.ind = 0xff;
	} else {
		dualop(p);
		setreg(&p->dst, regs[RT1]);
		setreg(&p->src, regs[RT2]);
	}
}

setreg(rp, as)
register struct usage *rp;
char *as;
{
	register char *s;
	register rflag = 0;
	register r;
	register r1;

	s = as;
	rp->dir = 0;
	rp->ind = 0;
	if(s == 0 || *s == 0)
		return;
	if( *s == '*' )
		s++;
	else {
		if(*s == 'r')
			rflag = 1;
		else {
			while( *++s );
			if( *--s != ')' || *--s >= '8' || *(s-1) != 'r' )
				return;
			rp->ind = regbit[*s - '0'];
			return;
		}
	}
	if(*s != 'r')
		return;
	if( (r=isreg(s)) >= 0 || (r=islbreg(s)) >= 0 ) {
		if( r >= 8 )
			return;
		rp->ind = regbit[r];
	} else if( (r=isrrreg(s)) >= 0 )
		rp->ind = regbit[r] | regbit[r+1];
	else if( (r=isrqreg(s)) >= 0 )
		for(r1=r; r1<r+4; rp->ind |= regbit[r1++]);
	if(rflag)
		rp->dir = rp->ind;
}

#define HOWFAR 15

needreg(p, r, count)
register struct node *p;
register int r, count;
{
	if( r == 0 )
		return( 1 );
	for(; p; p=p->forw) {
		if( ++count > HOWFAR )
			break;
		if( ((p->op == CLR || p->op == LDK)
		      && isdst(p, r))
		   || (p->op == LD || p->op == LDA)
		      && isdst(p, r)
		      && !usesrc(p, r) )
			return( 0 );
		if( p->op == CALL ) {
			if( usedst(p, r) )
				break;
			else
				return( 0 );
		}
		if( p->op == JP ) {
			if( equstr(p->code,"cret") ) {
				if( r >= 4 )
					break;
				else
					return( 0 );
			}
			if( usedst(p, r) )
				break;
			if( usedst(p, 0xff) )
				return( 0 );
			if( p->ref && (p=p->ref->back) )
				continue;
			else
				break;
		}
		if(p->op == CJP) {
			if( equstr(p->code,"cret") ) {
				if( r >= 4 )
					break;
				else
					return( 0 );
			}
			if( usedst(p, r) )
				break;
			if( usedst(p, 0xff) )
				return( 0 );
			return( needreg(p->forw,r,count)
				|| needreg(p->ref,r,count) );
		}
		if( usesrc(p, r) || usedst(p, r) )
			break;
	}
	return( 1 );
}
#endif

int chgcc(),usecc();

needcc(p)
register struct node *p;
{
	for(; p != 0; p = p->forw) {
		if(lostcc(p))
			return( 0 );
		if(usecc(p))
			return( 1 );
	}
	return( 1 );
}

lostcc(p) /* allow replacement of and with clr */
struct node *p;
/*
 * condition codes are assumed lost when a label is encountered.
 */
{
	switch(p->op){
	case CLR:
	case LD:
	case LDA:
	case LDK:
	case LDM:
	case RES:
	case SET:
	case CJP:
	case TCC:
		return( 0 );

	default:
		ccloc[0]=0;
		return( 1 );
	}
}

usecc(p)
struct node *p;
{
	switch(p->op){
	case CJP:
	case TCC:
		return( 1 );
	default:
		return( 0 );
	}
}
