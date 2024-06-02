/*
 *	UNIX Operating System Profiler
 */

#include	"sys/param.h"
#include	"sys/dir.h"
#include	"sys/user.h"
#include	"sys/seg.h"
#include	"sys/map.h"

int	prfbuf;		/* address of profile buffer */
int	prfctr;		/* offset for profile counters */
int	prfstat;	/* profiling state */

extern	struct	map	coremap[];

/* states */
#define	PRF_ON	01
#define	PRF_VAL	02

#define	P_READ	1
#define	P_WRITE	0

prfread()
{
	if((prfstat & PRF_VAL) == 0 || u.u_count > prfctr * 3 + 4) {
		u.u_error = ENXIO;
		return;
	}
	pimove(((long)(unsigned) prfbuf) << 6, u.u_count, P_READ);
}

prfwrite()
{
	register  int  a;

	if(u.u_count * 3 + 4 >= 8192 || u.u_count & 01 ||
		u.u_count == 0 || prfstat & PRF_ON) {
		u.u_error = ENXIO;
		return;
	}
	if(prfstat & PRF_VAL)
		mfree(coremap, btoc(prfctr * 3 + 4), prfbuf);
	if((prfbuf = malloc(coremap, btoc(u.u_count * 3 + 4))) == 0) {
		u.u_error = ENOSPC;
		prfstat = 0;
		prfctr = 0;
		return;
	}
	for(a = 0; a < btoc(u.u_count * 3 + 4); a++)
		clearseg(prfbuf + a);
	prfctr = u.u_count;
	pimove(((long)(unsigned) prfbuf) << 6, u.u_count, P_WRITE);
	if(u.u_error == 0)
		prfstat |= PRF_VAL;
	else {
		mfree(coremap, btoc(prfctr * 3 + 4), prfbuf);
		prfctr = 0;
		prfbuf = 0;
		prfstat = 0;
	}
}

prfioctl(dev, cmd, arg, mode)
{
	switch(cmd) {
	case 1:
		u.u_r.r_reg.r_val1 = prfstat;
		break;
	case 2:
		u.u_r.r_reg.r_val1 = prfctr / NBPW;
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
