/*
 * trap handlers for various ugly conditions
 */

#include "/p3/usr/include/icp/sioc.h"	/* icp specific */

/*
 * extended instruction, priveleged instruction, or syscall trap
 */

illinst(r0, r1, r2, r3, r4, r5, r6, r7, id, fcw, pc)
{

	printf("SIOC: illegal instruction. ");
	printf("PC: 0x%x, FCW: 0x%x, ID: 0x%x\n", pc, fcw, id);
}


/*
 * parity error or power failure.
 */

pwrfail(r0, r1, r2, r3, r4, r5, r6, r7, id, fcw, pc)
{

	if (in_local(PSTAT) & PERR) {
		out_local(PCMD, CMDINIT | RESPERR);	/* reset parity bit */
		out_local(PCMD, CMDINIT);
		printf("SIOC: parity error. ");
	} else {
		printf("SIOC: power failure. ");
	}
	printf("PC: 0x%x, FCW: 0x%x, ID: 0x%x\n", pc, fcw, id);
}


/*
 * illegal vectored interrupt.
 */

illvi(r0, r1, r2, r3, r4, r5, r6, r7, id, fcw, pc)
{

	printf("SIOC: illegal vectored interrupt. ");
	printf("PC: 0x%x, FCW: 0x%x, ID: 0x%x\n", pc, fcw, id);
}


/*
 * illegal non vectored interrupt.
 */

illnvi(r0, r1, r2, r3, r4, r5, r6, r7, id, fcw, pc)
{

	printf("SIOC: illegal non vectored interrupt. ");
	printf("PC: 0x%x, FCW: 0x%x, ID: 0x%x\n", pc, fcw, id);
}
