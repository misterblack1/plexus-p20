#include "saio.h"
/* srcheof(fdesc,count)
 *	Position to file number "count" on tape. Count starts at 0.
 *	Fdesc is a file descriptor gotten from an open of /dev/rmt0 
 *	or /dev/nrmt0.	Closing /dev/rmt0 will rewind;
 *	closing /dev/nrmt0 will leave tape at current spot.
 *	This is all Plexus stuff and not in STOCKIII.
 */
int	mtstrategy();
int	rmstrategy();
int	ptstrategy();
srcheof(fdesc,count) 
int fdesc, count;
{
	register struct iob *io; 
	register savboff;
	register status;

	fdesc -= 3;
	if (fdesc < 0
	    || fdesc >= NFILES
	    || ((io = &_iobuf[fdesc])->i_flgs&F_ALLOC) == 0) {
		errno = EBADF;
		return (-1);
	}
	if((*io->i_dp->dt_devp->dv_strategy != mtstrategy) &&
	(*io->i_dp->dt_devp->dv_strategy != rmstrategy) &&
	(*io->i_dp->dt_devp->dv_strategy != ptstrategy)) {
		errno = EBADF;
		return (-1);
	}
	savboff=io->i_dp->dt_boff;
	io->i_dp->dt_boff = count;
	status = _devsrcheof(io);
	io->i_dp->dt_boff = savboff;
	return status;
}

