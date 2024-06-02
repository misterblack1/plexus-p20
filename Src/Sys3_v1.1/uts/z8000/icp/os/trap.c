/*
 * trap handlers for various ugly conditions
 */

#include "icp/sio.h"	/* icp specific */
#include "icp/sioc.h"	/* icp specific */

/*
 * extended instruction, priveleged instruction, or syscall trap
 */

illinst(r0, r1, r2, r3, r4, r5, r6, r7, id, fcw, pc)
{

	printf("ICP: ill. inst. ");
	printf("PC:%x, FCW:%x, ID:%x\n", pc, fcw, id);
}


/*
 * parity error or power failure. (or profiling)
 */
#ifdef PROFILE
int profile[650];
#endif

pwrfail(r0, r1, r2, r3, r4, r5, r6, r7, id, fcw, pc)
register unsigned int pc;
{

#ifdef PROFILE
	if((pc>=0x4000) && (pc<0x9000)) {
		profile[((pc-0x4000)>>5)&0x3ff]++;
	}
#else
	if (in_local(PSTAT) & PERR) {
		out_local(PCMD, cmdsav | RESPERR);	/* reset parity bit */
		out_local(PCMD, cmdsav);
		printf("ICP: parity. ");
	} else {
		printf("ICP: pwr fail. ");
	}
	printf("PC:%x, FCW:%x, ID:%x\n", pc, fcw, id);
#endif
}


/*
 * illegal vectored interrupt.
 */

illvi(r0, r1, r2, r3, r4, r5, r6, r7, id, fcw, pc)
{

	printf("ICP: ill. vect. int. ");
	printf("PC:%x, FCW:%x, ID:%x\n", pc, fcw, id);
}


/*
 * illegal non vectored interrupt.
 */

illnvi(r0, r1, r2, r3, r4, r5, r6, r7, id, fcw, pc)
{

	printf("ICP: ill. non-vect. int. ");
	printf("PC:%x, FCW:%x, ID:%x\n", pc, fcw, id);
}

/*
 * sio transmit buffer empty interrupt
 */

#ifdef	MDP
extern long	rfint;
#endif
sioxmit(r0, r1, r2, r3, r4, r5, r6, r7, id, fcw, pc)
{

	printf( "sioxmit\n" );
	/* insert code to turn off sio transmitter buffer empty interrupt
	   enable here */
#ifdef MDP
	rfint++;
#endif
	out_local(psioc[((id>>3)^1)&0xfe],RTNFROMINT);
/*	putreti(); */
}
