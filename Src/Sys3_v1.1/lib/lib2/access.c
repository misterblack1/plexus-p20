static char	sccsid[] = "@(#)access.c	4.1";

# include "saio.h"

access(name, mode)
register char *name; {
	register struct iob *io;
	register fdesc;
	int perm;

	if ((fdesc = open(name, 0)) < 0)
		return (-1);

	io = &_iobuf[fdesc - 3];

	if ((io->i_flgs&F_FILE) == 0
	    || io->i_ino.i_mode & ((mode&07) << 6))
		perm = 0;
	else {
		perm = -1;
		errno = EACCES;
	}

	close(fdesc);
	return (perm);
}
