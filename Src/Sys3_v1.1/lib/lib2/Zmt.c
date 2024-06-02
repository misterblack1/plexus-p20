
#include "sys/param.h"
#include "sys/plexus.h"
#include "sys/buf.h"

int imscpt;
int numtapes;

mtinit(io)
register struct iob *io;
{
	imscpt = ptinit();

	rminit();
}

mtopen(io)
register struct iob *io;
{
	if (imscpt)
		return(ptopen(io));
	else
		return(rmopen(io));
}


mtclose(io)
register struct iob *io;
{
	if (imscpt)
		return(ptclose(io));
	else
		return(rmclose(io));
}


mtstrategy(io,func)
register struct iob *io;
{
	if (imscpt)
		return(ptstrategy(io,func));
	else
		return(rmstrategy(io,func));
}

