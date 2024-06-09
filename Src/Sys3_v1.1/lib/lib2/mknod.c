static char	sccsid[] = "@(#)mknod.c	4.1";

# include "saio.h"

MKNOD(dn, tabno, unit, boff)
char *dn;
daddr_t boff; {
	register struct dtab *dp;
	char dname[NAMSIZ];
	extern char *malloc();
	extern _devcnt;

	if (tabno >= _devcnt
	    || tabno < 0
	    || unit < 0
	    || boff < 0) {
		errno = EINVAL;
		return (-1);
	}

	_cond(dn, dname);

	for (dp = &_dtab[0]; dp < &_dtab[NDEV]; dp++)
		if (dp->dt_name == 0 || strcmp(dname, dp->dt_name) == 0)
			break;

	if (dp == &_dtab[NDEV]) {
		_prs("MKNOD: no space in device table\n");
		return (-1);
	}
	if (dp->dt_name) {
		errno = EEXIST;
		return (-1);
	}

	dp->dt_name = malloc((unsigned) (strlen(dname) + 1));
	strcpy(dp->dt_name, dname);
	dp->dt_devp = &_devsw[tabno];
	dp->dt_unit = unit;
	dp->dt_boff = boff;
	return (0);
}
