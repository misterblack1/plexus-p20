static char	sccsid[] = "@(#)SYS.c	4.5";

#include "saio.h"
#include <sys/ino.h>
#include <sys/filsys.h>
#include <sys/dir.h>

#define INDIR	3	/* number of levels of indirection */

#ifdef STOCKIII
static char	_b[INDIR][BLKSIZ];
static daddr_t	_blknos[INDIR];
#else
char	_b[INDIR][BLKSIZ];
daddr_t	_blknos[INDIR];
#endif
/* Given: n - inode number
	  io->i_dp device of the inode number
   Returns: inode statistics of inode number in io->i_ino.
   	    Block number containing inode in io->bn (unless devread
	    changes it).
 */

_openi(n, io)
register ino_t n;
register struct iob *io; {
	register struct dinode *dp;

#ifdef STOCKIII
	io->i_bn = (daddr_t)((n+15)/INOPB) + io->i_dp->dt_boff;
	io->i_cc = BLKSIZ;
#else
 	io->i_bn = (daddr_t)((n+(2*INOPB-1))/INOPB) + io->i_dp->dt_boff;
 	io->i_cc = BSIZE;
#endif
	io->i_ma = io->i_buf;

	_devread(io);

	dp = (struct dinode *) io->i_buf;
	dp = &dp[(n-1)%INOPB];

	io->i_ino.i_number = n;
	io->i_ino.i_mode = dp->di_mode;
	io->i_ino.i_nlink = dp->di_nlink;
	io->i_ino.i_uid = dp->di_uid;
	io->i_ino.i_gid = dp->di_gid;
	io->i_ino.i_size = dp->di_size;
	io->i_atime = dp->di_atime;
	io->i_mtime = dp->di_mtime;
	io->i_ctime = dp->di_ctime;

	l3tol((char *)io->i_ino.i_addr, (char *)dp->di_addr, NADDR);
}
/* returns logical block # of a dev. given a logical block number, bn,
 * of a file, and
 * an inode, io->i_ino.  Modifies io->i_ma,i_cc,i_cc,i_buf and whatever
 * devread(io) modifies.
 */

daddr_t
_sbmap(io, bn)
register struct iob *io;
daddr_t bn; {
	register i;
	register struct inode *ip;
	int j, sh;
	daddr_t nb, *bap;

	ip = &io->i_ino;
	if (bn < 0) {
		errno = ENXIO;
		return ((daddr_t) 0);
	}

	/*
	 * blocks 0..NADDR-4 are direct blocks
	 */
	if (bn < NADDR-INDIR) {
		i = (int) bn;
		nb = ip->i_addr[i];
		return (nb);
	}

	/*
	 * addresses NADDR-3, NADDR-2, and NADDR-1
	 * have single, double, triple indirect blocks.
	 * the first step is to determine
	 * how many levels of indirection.
	 */
	sh = 0;
	nb = 1;
	bn -= NADDR-INDIR;
	for(j=INDIR; j>0; j--) {
		sh += NSHIFT;
		nb <<= NSHIFT;
		if (bn < nb)
			break;
		bn -= nb;
	}
	if (j == 0) {
		errno = EFBIG;
		return ((daddr_t) 0);
	}

	/*
	 * fetch the address from the inode
	 */
	nb = ip->i_addr[NADDR-j];
	if (nb == 0) {
		errno = ENXIO;
		return ((daddr_t) 0);
	}

	/*
	 * fetch through the indirect blocks
	 */
	for (j--; j<INDIR; j++) {
		if (_blknos[j-1] != nb) {
			io->i_bn = nb + io->i_dp->dt_boff;
			io->i_ma = _b[j-1];
			io->i_cc = BLKSIZ;
			_devread(io);
			_blknos[j-1] = nb;
		}
		bap = (daddr_t *) _b[j-1];
		sh -= NSHIFT;
		i = (bn>>sh) & NMASK;
		nb = bap[i];
		if (nb == 0) {
			errno = ENXIO;
			return ((daddr_t) 0);
		}
	}

	return (nb);
}

/* Given: a simple filename (no /), s, and an io buffer with io->i_ino
	  filled in with a directory inode.
   Returns: inode number of the file.
	    io->ibuf, incidentally, has block of directory with the
	    file entry. io->bn has that phys block number.
	    0 on error with errno set.
 */
static ino_t
_dlook(s, io)
char *s;
register struct iob *io; {
	register struct direct *dp;
	register struct inode *ip;
	daddr_t bn;
	int n, dc;

	if (s == NULL || *s == '\0') {
		errno = ENOENT;
		return ((ino_t) 0);
	}
	ip = &io->i_ino;
	if ((ip->i_mode&IFMT) != IFDIR) {
		errno = ENOTDIR;
		return ((ino_t) 0);
	}

	/* n <= # of entries in directory, incl null entries */
	n = ip->i_size / sizeof(struct direct);	

	if (n == 0) {
		errno = ENOENT;
		return ((ino_t) 0);
	}

	dc = BLKSIZ;
	bn = (daddr_t) 0;
	while (n--) {
		if (++dc >= BLKSIZ/sizeof(struct direct)) {
			if ((io->i_bn = _sbmap(io, bn++)) < 0)
				return ((ino_t) 0);
			io->i_bn += io->i_dp->dt_boff;
			io->i_ma = io->i_buf;
			io->i_cc = BLKSIZ;
			_devread(io);
			dp = (struct direct *) io->i_buf;
			dc = 0;
		}

		if (strncmp(s, dp->d_name, DIRSIZ) == 0)
			return (dp->d_ino);
		dp++;
	}
	errno = ENOENT;
	return (0);
}
/* Given: path, the full pathname of a file
	  io->i_dp, the device of the file
   Returns: the inode number of file
	    inode statistics in io->i_ino.
 */

ino_t
_find(path, io)
register char *path;
register struct iob *io; {
	register char *q;
	char c;
	ino_t n;

	_openi(ROOTINO, io);
	if (path == NULL || *path == '\0')
		return ((ino_t) ROOTINO);
	n = ROOTINO;
	q = path;
	for (;;) {
		while (*q != '/' && *q != '\0')
			q++;
		c = *q;
		*q = '\0';

		if ((n = _dlook(path, io)) == 0)
			return ((ino_t) 0);
		if (c == '\0')
			return (n);
		_openi(n, io);
		*q = c;
		path = ++q;
	}
}

char	*_cdir;

/* copies istr to ostr and:
	prpends slash (/) to beginning of ostr
	replace multiple occurences of / with one /
	removes terminating /
*/
_cond(istr, ostr)
char *istr, *ostr; {
	register char *ptr1, *ptr2;
	extern char *strchr();

	ptr1 = istr;

	if (*istr != '/' && *istr != '\0' && _cdir) {
		strcpy(ostr, _cdir);
		ptr2 = strchr(ostr, '\0');
	} else
		ptr2 = ostr;

	*ptr2 = '/';

	while (*ptr1) {
		if (*ptr1 == '/' && *ptr2 == '/') {
			ptr1++;
			continue;
		}
		*++ptr2 = *ptr1++;
#ifndef STOCKIII
		/* add '/' in is(,)sys3 case */
		if(*ptr2 == ')' && *ptr1) *++ptr2 = '/'; 
#endif
	}
#ifdef STOCKIII
	if (*ptr2 == '/')
#else
	if(*ptr2 == '/' && *(ptr2-1) != ')')	/* leave '/' in is(0,0)/ case */
#endif
		ptr2--;
	*++ptr2 = '\0';
	*++ptr2 = '\0';
}

#ifdef STOCKIII
#ifdef pdp11
static char *_tmesg[] = {
	"CPU error\n",
	"Illegal instruction\n",
	"BPT\n",
	"IOT\n",
	"Power fail\n",
	"EMT\n",
	"Syscall trap\n",
};

_trap(ps)
{

	_prs("TRAP: ");
	_prs(_tmesg[ps&07]);
}
#endif
#endif

#ifndef STOCKIII
_trap(ps)
int ps;
{
	printf("Trap %x\n", ps);
#ifdef V7
	getchar();
#endif
}

pfail() {
	_prs("POWER FAIL\n");
}
#endif

#ifndef STOCKIII

_devsrcheof(io)
register struct iob *io; {

	return ((*_devsw[io->i_ino.i_dev].dv_strategy)(io, SRCHEOF));
}

copydata(from,to,count) register char *from,*to; int count;
{
	while(count--) *to++ = *from++;
}

#endif
_devread(io)
register struct iob *io; {

	return ((*_devsw[io->i_ino.i_dev].dv_strategy)(io, READ));
}

_devwrite(io)
register struct iob *io; {

	return ((*_devsw[io->i_ino.i_dev].dv_strategy)(io, WRITE));
}

_devopen(io)
register struct iob *io; {

	(*_devsw[io->i_ino.i_dev].dv_open)(io);
}

_devclose(io)
register struct iob *io; {

	(*_devsw[io->i_ino.i_dev].dv_close)(io);
}

_nullsys()
{ }

#ifdef vax
_mfpr(reg)
{
	asm("	mfpr	4(ap),r0");
}

_mtpr(reg, val)
{
	asm("	mtpr	8(ap),4(ap)");
}
#endif
