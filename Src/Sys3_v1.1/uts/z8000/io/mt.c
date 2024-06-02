#include "sys/param.h"
#include "sys/plexus.h"
#include "sys/buf.h"
#include "sys/elog.h"
#include "sys/iobuf.h"

struct iostat mtstat[8];
struct iobuf mttab = tabinit(MT0,mtstat);
int	imscpt;

mtinit(dev)
dev_t	dev;
{
#ifdef VPMSYS
#ifdef VPMSYSIS
		rminit(dev);
#else
		ptinit(dev);
		imscpt++;
#endif
#endif
#ifndef VPMSYS
	if (ptinit(dev) == -1)
		rminit(dev);
	else
		imscpt++;
#endif
}


mtopen(dev, wflag)
dev_t	dev;
int	wflag;
{
#ifdef VPMSYS
#ifdef VPMSYSIS
		rmopen(dev, wflag);
#else
		ptopen(dev, wflag);
#endif
#endif
#ifndef VPMSYS
	if (imscpt)
		ptopen(dev, wflag);
	else
		rmopen(dev, wflag);
#endif
}



mtclose(dev, flag)
dev_t	dev;
int	flag;
{
#ifdef VPMSYS
#ifdef VPMSYSIS
		rmclose(dev, flag);
#else
		ptclose(dev, flag);
#endif
#endif
#ifndef VPMSYS
	if (imscpt)
		ptclose(dev, flag);
	else
		rmclose(dev, flag);
#endif
}


mtstrategy(bp)
struct	buf	*bp;
{
#ifdef VPMSYS
#ifdef VPMSYSIS
		rmstrategy(bp);
#else
		ptstrategy(bp);
#endif
#endif
#ifndef VPMSYS
	if (imscpt)
		ptstrategy(bp);
	else
		rmstrategy(bp);
#endif
}


mtread(dev)
dev_t	dev;
{
#ifdef VPMSYS
#ifdef VPMSYSIS
		rmread(dev);
#else
		ptread(dev);
#endif
#endif
#ifndef VPMSYS
	if (imscpt)
		ptread(dev);
	else
		rmread(dev);
#endif
}


mtwrite(dev)
dev_t	dev;
{
#ifdef VPMSYS
#ifdef VPMSYSIS
		rmwrite(dev);
#else
		ptwrite(dev);
#endif
#endif
#ifndef VPMSYS
	if (imscpt)
		ptwrite(dev);
	else
		rmwrite(dev);
#endif
}


mtioctl(dev, com, addr, flag)
dev_t	dev;
int	com;
caddr_t	addr;
int	flag;
{
#ifdef VPMSYS
#ifdef VPMSYSIS
		rmioctl(dev, com, addr, flag);
#else
		ptioctl(dev, com, addr, flag);
#endif
#endif
#ifndef VPMSYS
	if (imscpt)
		ptioctl(dev, com, addr, flag);
	else
		rmioctl(dev, com, addr, flag);
#endif
}

