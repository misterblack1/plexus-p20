static char	sccsid[] = "%W%";

/*
 * RP04/RP06 disk driver
 */

#include "saio.h"

struct	device {
	int	hpcs1;		/* Control and Status register 1 */
	int	hpds;		/* Drive Status */
	int	hper1;		/* Error register 1 */
	int	hpmr;		/* Maintenance */ 
	int	hpas;		/* Attention Summary */
	int	hpda;		/* Desired address register */
	int	hpdt;		/* Drive type */
	int	hpla;		/* Look ahead */
	int	hpsn;		/* Serial number */
	int	hpof;		/* Offset register */
	int	hpdc;		/* Desired Cylinder address register */
	int	hpcc;		/* Current Cylinder */
	int	hper2;		/* Error register 2 */
	int	hper3;		/* Error register 3 */
	int	hpec1;		/* Burst error bit position */
	int	hpec2;		/* Burst error bit pattern */
};

/*
 * VAX Massbus adapter registers
 */

struct mba_regs {
	int mba_csr,
	    mba_cr,
	    mba_sr,
	    mba_var,
	    mba_bcr;
};

#define MBA_ERB 0x400
#define MBA_MAP 0x800

#define MBA0	((struct mba_regs *) 0x20010000)
#define HPADDR	((struct device *) 0x20010400)
#define NSECT	22
#define NTRAC	19

#define GO	01
#define DCLR	010
#define PRESET	020
#define WCOM	060
#define RCOM	070

#define VV	0100
#define MOL	010000

#define FMT22	010000

#define DCK	0100000

#define INIT	01

#define DTABT	0x1000
#define DTCMP	0x2000

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
	register struct device *hpaddr;
	register *mbap, frame;

	unit = io->i_dp->dt_unit;

	if ((unit & 04) == 0)
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

	hpaddr = (struct device *) (((int *) HPADDR) + 32*unit);

	MBA0->mba_cr = INIT;
	hpaddr->hpcs1 = DCLR | GO;

	if ((hpaddr->hpds & VV) == 0) {
		hpaddr->hpcs1 = PRESET | GO;
		hpaddr->hpof = FMT22;
	}

	_rp04.cn = bn / (NSECT * NTRAC);
	_rp04.sn = bn % (NSECT * NTRAC);
	_rp04.tn = _rp04.sn / NSECT;
	_rp04.sn %= NSECT;

	unit = GO;
	if (func == READ)
		unit |= RCOM;
	else
		unit |= WCOM;

	if ((hpaddr->hpds & MOL) == 0) {
		_prs ("RP04/05/06 unit not ready\n");
		while ((hpaddr->hpds & MOL) == 0);
	}

	mbap = (int *) MBA0;
	mbap += MBA_MAP / 4;
	frame = ((int) io->i_ma) >> 9;
	for (i = ((io->i_cc + 511) >> 9) + 1; i > 0; i--)
		*mbap++ = 0x80000000 | frame++;
	*mbap = 0;	/* invalidate mba entry */

	i = 1;

	do {
		if (i) {
			hpaddr->hpdc = _rp04.cn;
			hpaddr->hpda = (_rp04.tn << 8) | _rp04.sn;
			MBA0->mba_var = ((int) io->i_ma) & 0x1ff;
			MBA0->mba_bcr = -io->i_cc;
		}
		hpaddr->hpcs1 = unit;
		while ((MBA0->mba_sr & DTCMP) == 0);
		if ((MBA0->mba_sr & DTABT) == 0
		    || ((hpaddr->hper1 == DCK) && _hpecc () < 0))
			return (io->i_cc);
		MBA0->mba_cr = INIT;
		hpaddr->hpcs1 = DCLR | GO;
	} while (++errcnt < 10);

	errno = EIO;
	return (-1);
}

static
_hpecc (io, hpaddr)
register struct iob *io;
register struct device *hpaddr; {
	register i, n, b;

	if (hpaddr->hpec2 == 0)
		return (1);
	i = (hpaddr->hpec1 & 0xffff) - 1;
	n = i & 017;
	i = (i & ~017) >> 3;
	b = (((MBA0->mba_bcr & 0xffff) + io->i_cc - 1) >> 9) & 0177;
	i += b << BSHIFT;
	if (i < io->i_cc)
		*((int *) (&io->i_ma[i])) ^= hpaddr->hpec2 << n;
	i += 2;
	if (i < io->i_cc)
		*((int *) (&io->i_ma[i])) ^= hpaddr->hpec2 >> (16-n);
	if (MBA0->mba_bcr == 0)
		return (-1);
	i = _rp04.tn + _rp04.sn + b + 1;
	if (i >= NSECT * NTRAC) {
		i -= NSECT * NTRAC;
		++_rp04.cn;
	}
	_rp04.tn = i / NSECT;
	_rp04.sn = i % NSECT;
	hpaddr->hpdc = _rp04.cn;
	hpaddr->hpda = (_rp04.tn << 8) | _rp04.sn;
	return (0);
}
