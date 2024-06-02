static char	sccsid[] = "@(#)open.c	4.2";

# include "saio.h"

ino_t	_find();
char	*strchr(), *strrchr();

/* open is comparable to V7 open(2). it can open device (special) as
 * well as normal, disk, files.  each open of a file opens the device
 * file on which it resides.
 * Comments added by JSE as deduced from code.
 */
open(strx, how)
char *strx; {
	register char *cp1, *cp2;
	register struct iob *io;
	struct dtab *dp;
	struct mtab *mp;
	int fdesc;
	char c, str[NAMSIZ];
#ifndef STOCKIII
	short	v7kludge;
	char	savchar;
#endif

	ino_t n;

	if (strx == NULL || how < 0 || how > 2) {
		errno = EINVAL;
		return (-1);
	}

	for (fdesc = 0; fdesc < NFILES; fdesc++)
		if (_iobuf[fdesc].i_flgs == 0)
			goto gotfile;
	errno = EMFILE;
	return (-1);
gotfile:
	(io = &_iobuf[fdesc])->i_flgs |= F_ALLOC;

	/* returns 1st member of pathname, strx, in str, prepended
	 * with a '/' and with all redundant '/'s removed 
	 */
	_cond(strx, str);

#ifndef STOCKIII
		/* kludge for v7 type files: device(offset,unit) */
		/* look in Zinit for this */
	v7kludge = 0;
	if((v7files(str,&v7kludge)) == -1) {
		errno = ENODEV;
		io->i_flgs = 0;
		return (-1);
	}
	if(v7kludge == 1) {
		/* str is of type: is(0,0)/pathname */
		cp1 = strchr(str,')');
		savchar = *++cp1;
		*cp1 = '\0';
		mount(str,str);
		*cp1 = savchar;
	}
#endif

		/* is strx a device file? */
	for (dp = &_dtab[0]; dp < &_dtab[NDEV]; dp++)
		if (dp->dt_name == 0 || strcmp(str, dp->dt_name) == 0)
			break;

	if (dp->dt_name && dp < &_dtab[NDEV]) {
		io->i_ino.i_dev = dp->dt_devp - &_devsw[0];
		io->i_dp = dp;
		/* strx is device file: open the device */
#ifdef STOCKIII
		_devopen(io);
#else
		if(_devopen(io) == -1) {
			io->i_flgs = 0;
			return -1;
		}
#endif
		io->i_flgs |= how+1;
		io->i_offset = 0;
#ifndef STOCKIII
		if(v7kludge == 2) {
			_devsrcheof(io);
			io->i_dp->dt_unit = 0; /* for rewind on close */
		}
#endif
		return (fdesc+3);
	}

	c = '\0';
	cp2 = strchr(str, '\0');

	for (;;) {
		/* is strx a mounted file system? */
		for (mp = &_mtab[0]; mp < &_mtab[NMOUNT]; mp++) {
			if (mp->mt_name == 0)
				continue;
			if (strcmp(str, mp->mt_name) == 0)
				break;
		}
		if (mp != &_mtab[NMOUNT]) {
			/* strx is mounted file system: set dp to it */
			dp = mp->mt_dp;
			goto gotname;
		}
		cp1 = strrchr(str, '/');
		*cp2 = c;
		if (cp1 == NULL)
			break;
		c = '/';
		*(cp2 = cp1) = '\0';
	}
	io->i_flgs = 0;
	errno = ENOENT;
	return (-1);
gotname:
	io->i_ino.i_dev = dp->dt_devp - &_devsw[0];
	io->i_dp = dp;
#ifdef STOCKIII
	_devopen(io);
#else
	if(_devopen(io) == -1){ /*open device on which strx, a file, resides */
		io->i_flgs = 0;
		return -1;
	}
#endif

	if ((n = _find(++cp2, io)) == 0) {
		io->i_flgs = 0;
		return (-1);
	}

	if (how != 0) {
		io->i_flgs = 0;
		errno = EACCES;
		return (-1);
	}

	_openi(n, io);
	io->i_offset = 0;
	io->i_cc = 0;
	io->i_flgs |= F_FILE | (how+1);
	return (fdesc+3);
}
