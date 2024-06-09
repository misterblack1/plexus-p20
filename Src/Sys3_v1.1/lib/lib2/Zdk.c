
#include "sys/param.h"
#include "sys/plexus.h"
#include "sys/buf.h"

int imscpd;
int numdisks;


dkinit()
{
	imscpd = pdinit();
	isinit();
}

dkopen(io)
register struct iob *io;
{
	if (imscpd)
		return(pdopen(io));
	else
		return(isopen(io));
}


dkstrategy(io,func)
register struct iob *io;
{
	if (imscpd)
		return(pdstrategy(io,func));
	else
		return(isstrategy(io,func));
}

