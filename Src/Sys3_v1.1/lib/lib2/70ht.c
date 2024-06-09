static char	sccsid[] = "@(#)ht.c	4.8";

/*
 * TJU16 tape driver
 */

# include <stand.h>

struct	device {
	int	htcs1;
	int	htwc;
	caddr_t	htba;
	int	htfc;
	int	htcs2;
	int	htds;
	int	hter;
	int	htas;
	int	htck;
	int	htdb;
	int	htmr;
	int	htdt;
	int	htsn;
	int	httc;
	int	htbae;		/* 11/70 bus extension */
	int	htcs3;
};

# define HTADDR	((struct device *) 0172440)
# define NUNIT	4

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

static
_tcommand (io, com)
register struct iob *io;
register com; {
	register unit;

	unit = io->i_dp->dt_unit & 03;
	HTADDR->htcs1 = (segflag << 8) | com | GO;
	while ((HTADDR->htds & DRY) == 0);
	if (HTADDR->htds & TM) {
		_eof[unit]++;
		return (0);
	}
	if (HTADDR->htcs1 & TRE)
		return (-1);
	return (HTADDR->htfc + (com == RCOM ? 0 : io->i_cc));
}

_htclose (io)
register struct iob *io; {
	register flag, unit;

	flag = io->i_flgs;
	unit = io->i_dp->dt_unit & 03;
	HTADDR->htfc = 0;
	if (flag & F_WRITE) {
		_tcommand (io, WEOF);
		_tcommand (io, WEOF);
	}
	if ((io->i_dp->dt_unit & 04) == 0)
		_tcommand (io, REW);
	else if (flag & F_WRITE)
		_tcommand (io, SREV);
	else if (!_eof[unit])
		_tcommand (io, SFORW);
	_eof[unit] = 0;
}

_htstrategy (io, func)
register struct iob *io; {
	register com, unit;
	int errcnt = 0;
	int den, dev;
	int ret;

	dev = io->i_dp->dt_unit;
	HTADDR->htcs2 = unit = dev & 03;
	if (func == READ)
		com = RCOM;
	else if (func == WRITE)
		com = WCOM;
	else
		com = func;
	den = ((dev & 010) ? P1600 : P800) | unit;
	if ((HTADDR->httc & 03777) != den)
		HTADDR->httc = den;
	if ((HTADDR->htds & MOL) == 0) {
		_prs ("TU16 unit not ready\n");
		while ((HTADDR->htds & MOL) == 0);
	}
	do {
		HTADDR->htba = io->i_ma;
		HTADDR->htfc = -io->i_cc;
		HTADDR->htwc = -(io->i_cc >> 1);
		_eof[unit] = 0;
		while ((HTADDR->htds & DRY) == 0);
		if ((ret = _tcommand (io, com)) > 0)
			return (ret);
		HTADDR->htcs1 = TRE | DCLR | GO;
		if (ret == 0)
			return (0);
		HTADDR->htfc = -1;
		while ((HTADDR->htds & DRY) == 0);
		_tcommand (io, SREV);
		if (com == WCOM) {
			while ((HTADDR->htds & DRY) == 0);
			_tcommand (io, ERASE);
		}
	} while (++errcnt < 10);
	errno = EIO;
	return (-1);
}
