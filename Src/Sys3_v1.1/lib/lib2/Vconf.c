static char	sccsid[] = "@(#)Aconf.c	4.3";

#include "saio.h"

int	_nullsys();
int	_hpstrategy();
int	_htstrategy(), _htclose();

struct devsw _devsw[] = {
/* 0: hp */	_hpstrategy,	_nullsys,	_nullsys,
/* 1: ht */	_htstrategy,	_nullsys,	_htclose,
};

int	_devcnt = 2;
