static char	sccsid[] = "@(#)Zconf.c	4.3";

#include "saio.h"

int	dkstrategy(), dkopen();
int	pdstrategy(), pdopen();
int	isstrategy(), isopen();
int	mtstrategy(), mtclose(), mtopen();
int	ptstrategy(), ptopen(), ptclose();
int	rmstrategy(), rmclose(), rmopen();
int	_nullsys();
int	tapedump();
/*
int	usstrategy(), usclose(), usopen();
*/
int	kmemstrategy();
int	kmemopen();
int	physmem();

struct devsw _devsw[] = {
	dkstrategy,	dkopen,		_nullsys,
	mtstrategy,	mtopen,		mtclose,
	pdstrategy,	pdopen,		_nullsys,
	ptstrategy,	ptopen,		ptclose,
	isstrategy,	isopen,		_nullsys,
	rmstrategy,	rmopen,		rmclose,
	_nullsys,	tapedump,	_nullsys,
	_nullsys,	_nullsys,	_nullsys,
/*
	usstrategy,	usopen,		usclose,
*/
	kmemstrategy,	kmemopen,	_nullsys,
	_nullsys,	physmem,	_nullsys,
};

int	_devcnt = sizeof(_devsw)/sizeof(struct devsw);
