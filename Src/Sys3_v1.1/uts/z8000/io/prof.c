/*
 *	PLEXUS SYS3/UNIX Operating System Profiler
 *
 *	Sorted Kernel text addresses are written into driver.  At each
 *	clock interrupt a binary search locates the counter for the
 *	interval containing the captured PC and increments it.
 *	The last counter is used to hold the User mode counts.
 */

#include	"sys/param.h"
#include	"sys/plexus.h"
#include	"sys/psl.h"
#include	"sys/dir.h"
#include	"sys/user.h"
#include	"sys/buf.h"

# define PRFMAX  512		/* maximum number of text addresses */
# define PRF_ON    1		/* profiler collecting samples */
# define PRF_VAL   2		/* profiler contains valid text symbols */
# define NBPL	   4		/* number of bytes per long */
# define L2BPW	   1		/* log2(NBPW) */

unsigned  prfstat;		/* state of profiler */
unsigned  prfmax;		/* number of loaded text symbols */
unsigned  long prfctr[PRFMAX+1]; /* counters for symbols; last used for User */
unsigned  prfsym[PRFMAX];	/* text symbols */

prfread()
{
	unsigned  min();

	if((prfstat & PRF_VAL) == 0) {
		u.u_error = ENXIO;
		return;
	}
	pimove((paddr_t) prfsym + segtoaddr((long)SDSEG),
		min(u.u_count, prfmax * NBPW), B_READ);
	pimove((paddr_t) prfctr + segtoaddr((long)SDSEG),
		min(u.u_count, (prfmax + 1) * NBPL), B_READ);
}

prfwrite()
{
	register  unsigned  *ip;
	register  unsigned long *lp;

	if(u.u_count > sizeof prfsym)
		u.u_error = ENOSPC;
	else if(u.u_count & (NBPW - 1) || u.u_count < 3 * NBPW)
		u.u_error = E2BIG;
	else if(prfstat & PRF_ON)
		u.u_error = EBUSY;
	if(u.u_error)
		return;
	for(lp = prfctr; lp != &prfctr[PRFMAX + 1]; )
		*lp++ = 0;
	prfmax = u.u_count >> L2BPW;
	pimove((paddr_t) prfsym + segtoaddr((long)SDSEG),
		u.u_count, B_WRITE);
	for(ip = &prfsym[1]; ip != &prfsym[prfmax]; ip++)
		if(*ip < ip[-1]) {
			u.u_error = EINVAL;
			break;
		}
	if(u.u_error)
		prfstat = 0;
	else
		prfstat = PRF_VAL;
}

prfioctl(dev, cmd, arg, mode)
{
	switch(cmd) {
	case 1:
		u.u_r.r_reg.r_val1 = prfstat;
		break;
	case 2:
		u.u_r.r_reg.r_val1 = prfmax;
		break;
	case 3:
		if(prfstat & PRF_VAL) {
			prfstat = PRF_VAL | arg & PRF_ON;
			break;
		}
	default:
		u.u_error = EINVAL;
	}
}

prfintr(pc, fcw)
	register  unsigned  pc;
{
	register  int  h, l, m;

	if((fcw & B_SYS) == 0)
		prfctr[prfmax]++;
	else {
		l = 0;
		h = prfmax;
		while((m = (l + h) / 2) != l)
			if(pc >= prfsym[m])
				l = m;
			else
				h = m;
		prfctr[m]++;
	}
}
