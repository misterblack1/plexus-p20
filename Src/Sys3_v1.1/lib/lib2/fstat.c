static char	sccsid[] = "@(#)fstat.c	4.3";

# include "saio.h"
# include <sys/ino.h>
# include <sys/stat.h>

fstat (fdesc, buf)
register struct stat *buf; {
	register struct iob *io;
	extern long time ();

	fdesc -= 3;

	if (fdesc < -3
	    || fdesc >= NFILES
	    || (fdesc >= 0 && ((io = &_iobuf[fdesc])->i_flgs & F_ALLOC) == 0)) {
		errno = EBADF;
		return (-1);
	}

	if (fdesc < 0 || (io->i_flgs & F_FILE) == 0) {
		if (fdesc < 0)
			buf->st_dev = 0;
		else
			buf->st_dev = io->i_dp - &_dtab[0];
		buf->st_ino = 0;
		buf->st_mode = 020600;
		buf->st_nlink = 1;
		buf->st_uid = 0;
		buf->st_gid = 1;
		buf->st_rdev = buf->st_dev;
		buf->st_size = 0;
		buf->st_atime = time ((long *) 0);
		buf->st_mtime = buf->st_ctime = buf->st_atime;

		return (0);
	}

	buf->st_dev = io->i_dp - &_dtab[0];
	buf->st_ino = io->i_ino.i_number;
	buf->st_mode = io->i_ino.i_mode;
	buf->st_nlink = io->i_ino.i_nlink;
	buf->st_uid = io->i_ino.i_uid;
	buf->st_gid = io->i_ino.i_gid;
	buf->st_rdev = io->i_ino.i_rdev;
	buf->st_size = io->i_ino.i_size;
	buf->st_atime = io->i_atime;
	buf->st_mtime = io->i_mtime;
	buf->st_ctime = io->i_ctime;

	return (0);
}
