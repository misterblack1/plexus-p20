#ifndef vax
static char	sccsid[] = "@(#)rk.c	4.5";

/*
 * RK disk driver
 */

#include <stand.h>

#define	RKADDR	((struct device *) 0177400)

#define	RDY	0200
#define DRESET	014
#define	RCOM	04
#define	WCOM	02
#define CRESET	0
#define	GO	01

#define DRY	0200

struct	device {
	int	rkds;
	int	rker;
	int	rkcs;
	int	rkwc;
	caddr_t	rkba;
	int	rkda;
};

_rkstrategy(io, func)
register struct iob *io; {
	register com;
	register errcnt = 0;
	daddr_t bn;
	int cn, sn;

	bn = io->i_bn;
	cn = bn / 12;
	sn = bn % 12;

	com = (segflag << 4) | GO;
	if (func == READ)
		com |= RCOM;
	else
		com |= WCOM;

	RKADDR->rkda = (io->i_dp->dt_unit << 13) | (cn << 4) | sn;

	if ((RKADDR->rkds & DRY) == 0) {
		_prs ("RK05 unit not ready\n");
		while ((RKADDR->rkds & DRY) == 0);
	}

	do {
		RKADDR->rkba = io->i_ma;
		RKADDR->rkwc = -(io->i_cc >> 1);
		while ((RKADDR->rkcs & RDY) == 0);
		RKADDR->rkcs = com;
		while ((RKADDR->rkcs & RDY) == 0);
		if (RKADDR->rkcs >= 0)
			return (io->i_cc);
		RKADDR->rkcs = CRESET | GO;
		while ((RKADDR->rkcs & RDY) == 0);
		RKADDR->rkcs = DRESET | GO;
	} while (++errcnt < 10);

	errno = EIO;
	return (-1);
}
#endif
