#
/*
 *
 *	UNIX debugger
 *
 */

#include "defs.h"


MSG		NOCFN;

INT		callpc;
BOOL		localok;
SYMTAB		symbol;

STRING		errflg;


findroutine(cframe,space)
	L_INT		cframe;
	INT		space;
{
	REG INT		narg, inst;
	INT		lastpc, back2;
	BOOL		v;

	v=FALSE; localok=FALSE; lastpc=callpc;
	callpc=get(cframe+2, DSP);
	IF( (inst=get(leng(callpc-4), ISP)) != 0x5f00
	    && inst&0xff0f != 0x1f00 )	/* call addr */
	THEN	errflg=NOCFN;
		return(0);
	FI
	narg = 0;
	IF findsym(lastpc, space) == -1
	THEN	symbol.symc[0] = '?';
		symbol.symc[1] = 0;
		symbol.symv = 0;
	ELSE	localok=TRUE;
	FI
	inst = get(leng(callpc), ISP);
	IF (inst & 0xfff0) == 0xa9f0		/* inc r15,$n */
	THEN	return(narg + ((inst&0x0f)+1) / 2);
	ELIF inst == 0x010f
	THEN	return(narg + get(leng(callpc+2))/2);
	FI
	return(narg);
}

