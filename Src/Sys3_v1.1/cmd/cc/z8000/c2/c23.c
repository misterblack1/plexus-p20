/*	Plexus - Sys3 - August 1982	*/

static char c23_c[] = "@(#)c23.c	1.1";

/*
 *	subroutines for z8000/pcc optimizer
 */

#include "c2.h"
#include <ctype.h>

clrop2(s1)
char *s1;
{
	register char *p = s1;

	do {
		if( *p == ',' ) {
			*p = '\0';
			break;
		}
	} while( *p++ );
}

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

#ifndef QUICK1
/*
 *	procedure level register optimization
 *	if a procedure has no call instructions
 *	a lot can possibly be done
 */

/*
 *	the following flag bits specify the status of a register
 *	variable that is being moved to a scratch register
 */
#define USEDREG	0x01
#define USEDLONG	0x02
#define MAPREG	0x04

static char usedregs[NREG+2];
static char mapreg[NREG];
static char usedauto, usedr14, usedr15;

procopt() {
	register struct node *p;
	register i;
	register char *s;
	register int *regvar;
	int freetreg, freevreg;
	int usetreg, usevreg;
	int j;
	char rt1[20],rt2[20];

	for( i=0;i<NREG;i++)
		usedregs[i] = 0;
	usedauto = usedr14 = usedr15 = 0;

	for(p=first.forw;p!=0;p=p->forw) {
	switch( p->op ) {

	case CALL:
		if( equsubstr("csv", p->code) ) {
			if( ! equstr( p->back->code, "r0,$12" ) )
				usedauto = 1;
			continue;
		}
	case LDM:
	case LDIR:
	case LDDR:
	case ADDF:
	case SUBF:
	case DIVF:
	case MULF:
	case CPF:
	case NEGF:
		return(0);

	case LD:
	case SDA:
	case SDL:
	case CP:
	case ADD:
	case SUB:
	case AND:
	case OR:
	case MUL:
	case DIV:
	case XOR:
	case BIT:
	case SET:
	case RES:
	case LDA:
	case PUSH:
	case POP:
	case LDCTL:
		dualop(p);
		if( checkreg(regs[RT1]) || checkreg(regs[RT2]) )
			return(0);
		continue;

	case INC:
	case DEC:
	case LDK:
	case SLA:
	case SLL:
	case SRA:
	case SRL:
		dualop(p); /* the second is a constant */
		if( checkreg(regs[RT1]) )
			return(0);
		continue;

	case SOB:
	case CLR:
	case COM:
	case NEG:
	case TST:
	case EXTS:
	case TCC:
		singop(p);
		if( checkreg(regs[RT1]) )
			return(0);
		continue;
	}
	}
	/* still here, maybee we can optimize */
	/* how many free regs of each type */
/*
 * the following isn't needed anymore since we always attempt
 * the assignment
	freetreg = freevreg = usetreg = usevreg = 0;
	for(i=1;i<8;i++)
		if(usedregs[i]) usetreg++;
		else freetreg++;
	for(i=8;i<NREG;i++)
		if(usedregs[i]) usevreg++;
		else freevreg++;
 */

	if( usedauto || usedr15 )
		return( 0 );
		
	for(i=0;i<NREG;i++)
		mapreg[i] = 0;
	/* first do the longs */
	for(i=8;i<NREG;i+=2) {
		if( usedregs[i] & USEDLONG ) {
			for(j=2;j<8;j+=2) {
				if( usedregs[j] == 0 && usedregs[j+1] == 0 ) {
					usedregs[i] |= MAPREG;
					usedregs[i+1] |= MAPREG;
					usedregs[j] = USEDREG;
					usedregs[j+1] = USEDREG;
					mapreg[i] = j;
					mapreg[i+1] = j+1;
					break;
				}
			}
		}
	}
	for(i=8;i<NREG;i++) {
		if( usedregs[i] && (usedregs[i] & MAPREG) == 0 ) {
			for(j=1;j<8;j++) {
				if( usedregs[j] == 0 ) {
					usedregs[i] |= MAPREG;
					usedregs[j] = USEDREG;
					mapreg[i] = j;
					break;
				}
			}
		}
	}
	for(i=8;i<NREG;i++) {
		if( usedregs[i] && (usedregs[i] & MAPREG) == 0 )
			return( 0 );
	}
	nproc++;
	for(p=first.forw;p!=0;p=p->forw) {
	switch( p->op ) {

	case CALL:
		p->back->back->forw = p->forw;
		p->forw->back = p->back->back;
		continue;

	case LD:
	case SDA:
	case SDL:
	case CP:
	case ADD:
	case SUB:
	case AND:
	case OR:
	case MUL:
	case DIV:
	case XOR:
	case BIT:
	case SET:
	case RES:
	case LDA:
	case PUSH:
	case POP:
	case INC:
	case DEC:
	case LDK:
	case SLA:
	case SLL:
	case SRA:
	case SRL:
	case LDCTL:
		dualop(p);
		i = newreg(rt1,regs[RT1]);
		if( newreg(rt2,regs[RT2]) || i ) {
			for( s=rt1;*s;s++ );
			*s++ = ',';
			*s = '\0';
			p->code = copy(2, rt1, rt2);
		}
		continue;

	case SOB:
	case CLR:
	case COM:
	case NEG:
	case TST:
	case EXTS:
	case TCC:
		singop(p);
		if( newreg(rt1,regs[RT1]) )
			p->code = copy(1, rt1);
		continue;
	
	case JP:
	case CJP:
	case JR:
	case CJR:
#ifdef OPTCRET
		if( p->code )
#endif
		{
			if( !equsubstr("cret", p->code ) )
				continue;
			p->code = 0;
#ifdef OPTCRET
		} else {
			if( p->labno < CRETLAB )
				continue;
#endif
		}
		if( p->op == CJP || p->op == CJR )
			p->op = CRET;
		else
			p->op = RET;
		continue;
	}
	}
}

static regoffset;
static offstr;
static char stroffset[20];

newreg(d,s)
char *s,*d;
{
	register r;
	register base,type;

	r = isreg(s);
	base = rbase(r);
	type = rtype(r);
	if( base > 7 && base < NREG ) {
		makereg( d, regnum(mapreg[base],type) );
		return( 1 );
	} else if( (r=isstarreg(s)) >= 0 && r>7 && r<NREG ) {
		sprintf( d, "*r%d", mapreg[r] );
		return(1);
	} else if( (r=isoffreg(s)) >= 0 ) {
		if( r>7 && r<NREG ) {
			if( stroffset[0] )
				sprintf( d, "%s(r%d)", stroffset, mapreg[r] );
			else
				sprintf( d, "%d(r%d)", regoffset, mapreg[r] );
			return(1);
		} else if( r == 14 ) {
			sprintf(d, "%d(r15)", regoffset-2 );
			return(1);
		}
	}
	while( *d++ = *s++ );
	return( 0 );
}

checkreg(s)
char *s;
{
	register r,base,type;
	register i;
	register indir, maxfix;

	if( (r = isreg(s)) >= 0 || (r = isanyreg(s)) >= 0 ) {
		indir = 0;
	reg:
		base = rbase(r);
		type = rtype(r);
		if( base == 15 )
			usedr15 = 1;
		else if ( base == 14 && !indir )
			return(1);
		usedregs[base] |= USEDREG;
		maxfix = 3; /* in case its quad */
		switch(type) {

		case TREG:
		case TBREG:
		case THBREG:
			break;
		
		case TDREG:
			usedregs[base] |= USEDLONG;
			maxfix = 1;

		case TQREG:
			for(i=0;i<maxfix;i++) {
				usedregs[++base] = 1;
			}
		}
	} else if( (r = isoffreg(s)) >= 0 ) {
		indir = 1;
		goto reg;
	} else if( (r = isstarreg(s)) >= 0 ) {
		indir = 1;
		goto reg;
	}
	return(0);
}

isstarreg(s)
char *s;
{
	int r;

	if( *s != '*' )
		return(-1);
	r = isanyreg(s+1);
	return( r );
}

isoffreg(s)
char *s;
{
	register r;

	r = isindex(s);
	return( r );
}


isindex(s1)
register char *s1;
{
	register char *s;
	int reg;

	s = s1;
	regoffset = 0;
	stroffset[0] = '\0';
	while( *s && *s++ != '(' );
	if( *s == 0 || *s++ != 'r' )
		return( -1 );
	if( sscanf( s, "%d)", &reg ) != 1 )
		return( -1 );
	if( *s1 == '-' || *s1 == '+' || isdigit( *s1 ) )
		sscanf( s1, "%d(", &regoffset );
	else {
		for(s=stroffset;(*s++ = *s1++) != '(';);
		*--s = '\0';
	}
	if( reg == 14 ) {
		usedr14 = 1;
	}
	if( reg != 14 )
		return( reg );
	return( reg );
}
#endif
