static char	sccsid[] = "@(#)mount.c	4.1";

#include "saio.h"

mount(dn, mn)
char *dn, *mn; {
	register struct mtab *mp, *fmp;
	register struct dtab *dp;
	char dname[NAMSIZ], mname[NAMSIZ];
	extern char *malloc();

	_cond(dn, dname);
	_cond(mn, mname);

	for (dp = &_dtab[0]; dp < &_dtab[NDEV]; dp++)
		if (dp->dt_name == 0 || strcmp(dname, dp->dt_name) == 0)
			break;
	if (dp->dt_name == 0 || dp == &_dtab[NDEV]) {
		errno = ENODEV;
		return (-1);
	}
	fmp = 0;
	for (mp = &_mtab[0]; mp < &_mtab[NMOUNT]; mp++) {
		if (mp->mt_name == 0) {
			fmp = mp;
			continue;
		}
		if (strcmp(mname, mp->mt_name) == 0) {
			errno = EINVAL;
			return (-1);
		}
	}
	if (fmp == 0) {
		errno = EBUSY;
		return (-1);
	}
	fmp->mt_name = malloc((unsigned) (strlen(mname) + 1));
	strcpy(fmp->mt_name, mname);
	fmp->mt_dp = dp;
	return (0);
}
