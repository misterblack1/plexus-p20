static char	sccsid[] = "@(#)write.c	4.2";

# include "saio.h"

write(fdesc, buf, count)
int fdesc;
char *buf;
int count; {
	register i;
	register struct iob *io;

	if (fdesc >= 0 && fdesc <= 2)
		return (_ttwrite(buf, count));

	fdesc -= 3;
	if (fdesc < 0
	    || fdesc >= NFILES
	    || ((io = &_iobuf[fdesc])->i_flgs&F_ALLOC) == 0
	    || (io->i_flgs&F_WRITE) == 0) {
		errno = EBADF;
		return (-1);
	}
	io->i_cc = count;
	io->i_ma = buf;
#ifdef STOCKIII
	io->i_bn = (io->i_offset >> 9) + io->i_dp->dt_boff;
#else
	io->i_bn = (io->i_offset >> BSHIFT) + io->i_dp->dt_boff;
#endif
	if ((i = _devwrite(io)) > 0)
		io->i_offset += i;
	return (i);
}
