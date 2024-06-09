#include "sys/param.h"
#include "sys/plexus.h"
#include "sys/buf.h"
#include "sys/elog.h"
#include "sys/iobuf.h"

struct iostat dkstat[8];
struct iobuf dktab = tabinit(DK0,dkstat);
int imscpd;

dkinit()
{
#ifdef VPMSYS
#ifdef VPMSYSIS
		isinit();
#else
		pdinit();
		imscpd++;
#endif
#endif
#ifndef VPMSYS
	if (pdinit() == -1)
		isinit();
	else
		imscpd++;
#endif
}


dkopen(dev, wflag)
dev_t	dev;
int	wflag;
{
#ifdef VPMSYS
#ifdef VPMSYSIS
		isopen(dev, wflag);
#else
		pdopen(dev, wflag);
#endif
#endif
#ifndef VPMSYS
	if (imscpd)
		pdopen(dev, wflag);
	else
		isopen(dev, wflag);
#endif
}


dkstrategy(bp)
struct	buf	*bp;
{
#ifdef VPMSYS
#ifdef VPMSYSIS
		isstrategy(bp);
#else
		pdstrategy(bp);
#endif
#endif
#ifndef VPMSYS
	if (imscpd)
		pdstrategy(bp);
	else
		isstrategy(bp);
#endif
}


dkread(dev)
dev_t	dev;
{
#ifdef VPMSYS
#ifdef VPMSYSIS
		isread(dev);
#else
		pdread(dev);
#endif
#endif
#ifndef VPMSYS
	if (imscpd)
		pdread(dev);
	else
		isread(dev);
#endif
}


dkwrite(dev)
dev_t	dev;
{
#ifdef VPMSYS
#ifdef VPMSYSIS
		iswrite(dev);
#else
		pdwrite(dev);
#endif
#endif
#ifndef VPMSYS
	if (imscpd)
		pdwrite(dev);
	else
		iswrite(dev);
#endif
}
