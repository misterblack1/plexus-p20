static char	sccsid[] = "%W%";

/*
 * TE16 tape driver
 */

#include "saio.h"

struct	device
{
	int	htcs1;
	int	htds;
	int	hter;
	int	htmr;
	int	htas;
	int	htfc;
	int	htdt;
	int	htck;
	int	htsn;
	int	httc;
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

#define MBA_MAP 0x800
#define MBA_ERB 0x400

#define MBA1 	((struct mba_regs *)0x20012000)
#define	HTADDR	((struct device *)0x20012400)
#define NUNIT	4

static daddr_t	_eof[NUNIT];

#define GO	01
#define REW	06
#define DCLR	010
#define ERASE	024
#define WEOF	026
#define SFORW	030
#define SREV	032
#define WCOM	060
#define RCOM	070
#define TRE	040000

#define P800	01300		/*  800 + pdp11 mode */
#define P1600	02300		/* 1600 + pdp11 mode */

#define TM	04
#define DRY	0200
#define MOL	010000

#define INIT	01

#define DTABT	0x1000

static
_tcommand (io, com, htaddr)
register struct iob *io;
register com;
register struct device *htaddr; {
	register unit;

	unit = io->i_dp->dt_unit & 03;
	htaddr->htcs1 = com | GO;
	while ((htaddr->htds & DRY) == 0);
	if (htaddr->htds & TM) {
		_eof[unit]++;
		return (0);
	}
	if (MBA1->mba_sr & DTABT)
		return (-1);
	return ((htaddr->htfc & 0xffff) + (com == RCOM ? 0 : io->i_cc));
}

_htclose (io)
register struct iob *io; {
	register flag, unit;
	register struct device *htaddr;

	htaddr = (struct device *) (((int *) HTADDR) + 32*(unit & 03));

	flag = io->i_flgs;
	unit = io->i_dp->dt_unit & 03;
	htaddr->htfc = 0;
	if (flag & F_WRITE) {
		_tcommand (io, WEOF, htaddr);
		_tcommand (io, WEOF, htaddr);
	}
	if ((io->i_dp->dt_unit & 04) == 0)
		_tcommand (io, REW, htaddr);
	else if (flag & F_WRITE)
		_tcommand (io, SREV, htaddr);
	else if (!_eof[unit])
		_tcommand (io, SFORW, htaddr);
	_eof[unit] = 0;
}

_htstrategy (io, func)
register struct iob *io; {
	register com, unit;
	int errcnt = 0;
	int den, dev;
	int ret;
	register struct device *htaddr;
	register i, *mbap, frame;

	dev = io->i_dp->dt_unit;
	unit = dev & 03;
	htaddr = (struct device *) (((int *) HTADDR) + 32*unit);

	MBA1->mba_cr = INIT;
	htaddr->htcs1 = DCLR | GO;

	if (func == READ)
		com = RCOM;
	else if (func == WRITE)
		com = WCOM;
	else
		com = func;
	den = ((dev & 010) ? P1600 : P800) | unit;
	if ((htaddr->httc & 03777) != den)
		htaddr->httc = den;
	if ((htaddr->htds & MOL) == 0) {
		_prs ("TE16 unit not ready\n");
		while ((htaddr->htds & MOL) == 0);
	}

	mbap = (int *) MBA1;
	mbap += MBA_MAP / 4;
	frame = ((int) io->i_ma) >> 9;
	for (i = ((io->i_cc + 511) >> 9) + 1; i > 0; i--)
		*mbap++ = 0x80000000 | frame++;
	*mbap = 0;	/* invalidate mba entry */

	do {
		MBA1->mba_var = ((int) io->i_ma) & 0x1ff;
		MBA1->mba_bcr = htaddr->htfc = -io->i_cc;
		_eof[unit] = 0;
		while ((htaddr->htds & DRY) == 0);
		if ((ret = _tcommand (io, com, htaddr)) > 0)
			return (ret);
		MBA1->mba_cr = INIT;
		htaddr->htcs1 = DCLR | GO;
		if (ret == 0)
			return (0);
		htaddr->htfc = -1;
		while ((htaddr->htds & DRY) == 0);
		_tcommand (io, SREV, htaddr);
		if (com == WCOM) {
			while ((htaddr->htds & DRY) == 0);
			_tcommand (io, ERASE, htaddr);
		}
	} while (++errcnt < 10);
	errno = EIO;
	return (-1);
}
