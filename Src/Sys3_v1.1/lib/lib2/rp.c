#ifndef vax
static char	sccsid[] = "@(#)rp.c	4.4";

#
/*
 * rp03 disk driver
 */

#include <stand.h>

struct device {
	int	rpds;
	int	rper;
	int	rpcs;
	int	rpwc;
	caddr_t	rpba;
	int	rpca;
	int	rpda;
};

#define RPADDR	((struct device *) 0176710)
#define NSECT	10
#define NTRAC	20

#define RDY	0200
#define	RCOM	04
#define	WCOM	02
#define	GO	01

#define MOL	040000

_rpstrategy(io, func)
register struct iob *io; {
	register com;
	register errcnt = 0;
	int cn, tn, sn;

	cn = io->i_bn / (NSECT * NTRAC);
	sn = io->i_bn % (NSECT * NTRAC);
	tn = sn / NSECT;
	sn %= NSECT;

	com = io->i_dp->dt_unit << 8;
	RPADDR->rpcs = com;

	com |= (segflag << 4) | GO;
	if (func == READ)
		com |= RCOM;
	else
		com |= WCOM;

	if ((RPADDR->rpds & MOL) == 0) {
		_prs ("RP03 unit not ready\n");
		while ((RPADDR->rpds & MOL) == 0);
	}

	RPADDR->rpca = cn;
	RPADDR->rpda = (tn << 8) | sn;

	do {
		RPADDR->rpba = io->i_ma;
		RPADDR->rpwc = -(io->i_cc >> 1);
		while ((RPADDR->rpcs & RDY) == 0);
		RPADDR->rpcs = com;
		while ((RPADDR->rpcs & RDY) == 0);
		if (RPADDR->rpcs >= 0)
			return (io->i_cc);
	} while (++errcnt < 10);
	
	errno = EIO;
	return (-1);
}
#endif
