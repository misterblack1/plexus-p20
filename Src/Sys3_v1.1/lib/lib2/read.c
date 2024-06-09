static char	sccsid[] = "@(#)read.c	4.2";
/* STOCKIII ifdefs were removed */

# include "saio.h"

daddr_t	_sbmap ();

static
_readc (io)
register struct iob *io; {
	register char *p;
	register c;
	int i;
	int off;

	p = io->i_ma;
	if (io->i_cc <= 0) {
		io->i_bn = io->i_offset >> BSHIFT;
		if((io->i_flgs & F_FILE) != 0)
			if ((io->i_bn = _sbmap (io, io->i_bn)) <= 0)
				return (-1);
		io->i_bn += io->i_dp->dt_boff;
		io->i_ma = io->i_buf;
		io->i_cc = BLKSIZ;
		if ((i = _devread (io)) <= 0) {
			return (i == 0 ? (-2) : (-1));
		}
		if((io->i_flgs & F_FILE) != 0) {
			off = io->i_offset & BMASK;
			if (io->i_offset + (BLKSIZ-off) >= io->i_ino.i_size)
				io->i_cc = io->i_ino.i_size - io->i_offset+off;
			io->i_cc -= off;
			if (io->i_cc <= 0)
				return (-2);
		}
		else {
			/* guard against tapes w/o 1024 records */
			if(i != BLKSIZ) {
				errno = ENOTBLK;
				return (-1);
			}
			off = 0;
		}
		p = &io->i_buf[off];
	}
	io->i_cc--;
	io->i_offset++;
	c = (int) *p++;
	io->i_ma = p;
	return (c & 0377);
}

read (fdesc, buf, count)
int fdesc;
char *buf;
int count; {
	register i;
	register struct iob *io;

	if (fdesc >= 0 && fdesc <= 2)
		return (_ttread (buf, count));

	fdesc -= 3;
	if (fdesc < 0
	    || fdesc >= NFILES
	    || ((io = &_iobuf[fdesc])->i_flgs&F_ALLOC) == 0
	    || (io->i_flgs&F_READ) == 0) {
		errno = EBADF;
		return (-1);
	}
	/* do not buffer reads of >= BSIZE unless you have already done
	   some buffered reads. 
	 */
	if ((io->i_flgs&F_FILE) == 0 && count >= BSIZE &&
		io->i_offset%BSIZE == 0) {
		io->i_cc = count;
		io->i_ma = buf;
		io->i_bn = (io->i_offset >> BSHIFT) + io->i_dp->dt_boff;
		if ((i = _devread (io)) > 0)
			io->i_offset += i;
		return (i);
	}
	if ((io->i_flgs&F_FILE) != 0) {
		if (io->i_offset+count > io->i_ino.i_size)
			count = io->i_ino.i_size - io->i_offset;
	}
	if ((i = count) <= 0)
		return (0);
	do {
		switch (*buf++ = _readc (io)) {
		case -1:
			return (-1);
		case -2:
			count -= i;
			i = 1;
			break;
		}
	} while (--i);
	return (count);
}
