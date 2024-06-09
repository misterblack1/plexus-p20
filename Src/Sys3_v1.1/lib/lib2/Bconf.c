static char	sccsid[] = "@(#)Bconf.c	4.3";

#include <stand.h>

int	_nullsys();
int	_rpstrategy();
int	_rkstrategy();
int	_tmstrategy(), _tmclose();

struct devsw _devsw[] = {
/* 0: rp */	_rpstrategy,	_nullsys,	_nullsys,
/* 1: rk */	_rkstrategy,	_nullsys,	_nullsys,
/* 2: tm */	_tmstrategy,	_nullsys,	_tmclose,
};

int	_devcnt = 3;
