static char	sccsid[] = "@(#)hp.c	4.10";

/*
 * RP04/RP06 disk driver
 */

#include <stand.h>

struct	device {
	int	hpcs1;		/* Control and Status register 1 */
	int	hpwc;		/* Word count register */
	caddr_t	hpba;		/* UNIBUS address register */
	int	hpda;		/* Desired address register */
	int	hpcs2;		/* Control and Status register 2*/
	int	hpds;		/* Drive Status */
	int	hper1;		/* Error register 1 */
	int	hpas;		/* Attention Summary */
	int	hpla;		/* Look ahead */
	int	hpdb;		/* Data buffer */
	int	hpmr;		/* Maintenance register */
	int	hpdt;		/* Drive type */
	int	hpsn;		/* Serial number */
	int	hpof;		/* Offset register */
	int	hpdc;		/* Desired Cylinder address register */
	int	hpcc;		/* Current Cylinder */
	int	hper2;		/* Error register 2 */
	int	hper3;		/* Error register 3 */
	int	hpec1;		/* Burst error bit position */
	int	hpec2;		/* Burst error bit pattern */
	int	hpbae;		/* 11/70 bus extension */
	int	hpcs3;
};

#define HPADDR	((struct device *) 0176700)
#define NSECT	22
#define NTRAC	19

#define GO	01
#define DCLR	010
#define PRESET	020
#define WCOM	060
#define RCOM	070
#define RDY	0200

#define VV	0100
#define MOL	010000
#define TRE	040000

#define FMT22	010000

#define DCK	0100000

static struct {
	int	cn;
	int	tn;
	int	sn;
} _rp04;

_hpstrategy (io, func)
register struct iob *io; {
	register i, unit;
	int errcnt = 0;
	daddr_t bn;

	if (((unit = io->i_dp->dt_unit) & 04) == 0)
		bn = io->i_bn;
	else {
		unit &= 03;
		bn = io->i_bn;
		bn -= io->i_dp->dt_boff;
		i = unit + 1;
		unit = bn % i;
		bn /= i;
		bn += io->i_dp->dt_boff;
	}

	HPADDR->hpcs2 = unit;

	if ((HPADDR->hpds & VV) == 0) {
		HPADDR->hpcs1 = PRESET | GO;
		HPADDR->hpof = FMT22;
	}

	_rp04.cn = bn / (NSECT * NTRAC);
	_rp04.sn = bn % (NSECT * NTRAC);
	_rp04.tn = _rp04.sn / NSECT;
	_rp04.sn %= NSECT;

	unit = (segflag << 8) | GO;
	if (func == READ)
		unit |= RCOM;
	else
		unit |= WCOM;

	if ((HPADDR->hpds & MOL) == 0) {
		_prs ("RP04/05/06 unit not ready\n");
		while ((HPADDR->hpds & MOL) == 0);
	}

	i = 1;

	do {
		if (i) {
			HPADDR->hpdc = _rp04.cn;
			HPADDR->hpda = (_rp04.tn << 8) | _rp04.sn;
			HPADDR->hpba = io->i_ma;
			HPADDR->hpwc = -(io->i_cc >> 1);
		}
		HPADDR->hpcs1 = unit;
		while ((HPADDR->hpcs1 & RDY) == 0);
		if ((HPADDR->hpcs1 & TRE) == 0
		    || ((HPADDR->hper1 == DCK) && (i = _hpecc ()) < 0))
			return (io->i_cc);
		HPADDR->hpcs1 = TRE | DCLR | GO;
	} while (++errcnt < 10);

	errno = EIO;
	return (-1);
}

static
_hpecc (io)
register struct iob *io; {
	register i, n;
	int b;

	if (HPADDR->hpec2 == 0)
		return (1);
	i = HPADDR->hpec1 - 1;
	n = i & 017;
	i = (i & ~017) >> 3;
	b = ((HPADDR->hpwc + (io->i_cc >> 1) - 1) >> 8) & 0377;
	i += b << BSHIFT;
	if (i < io->i_cc)
		*((int *) (&io->i_ma[i])) ^= HPADDR->hpec2 << n;
	i += 2;
	if (i < io->i_cc)
		*((int *) (&io->i_ma[i])) ^= HPADDR->hpec2 >> (16-n);
	if (HPADDR->hpwc == 0)
		return (-1);
	i = _rp04.tn + _rp04.sn + b + 1;
	if (i >= NSECT * NTRAC) {
		i -= NSECT * NTRAC;
		++_rp04.cn;
	}
	_rp04.tn = i / NSECT;
	_rp04.sn = i % NSECT;
	HPADDR->hpdc = _rp04.cn;
	HPADDR->hpda = (_rp04.tn << 8) | _rp04.sn;
	return (0);
}
