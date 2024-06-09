#include "sys/param.h"
#include "sys/systm.h"
#include "sys/dir.h"
#include "sys/user.h"
#include "sys/file.h"
#include "sys/inode.h"
#include "sys/sysinfo.h"
#ifdef	PNETDFS
extern	ddbug;
#endif

/*
 * read system call
 */
read()
{
	rdwr(FREAD);
}

/*
 * write system call
 */
write()
{
	rdwr(FWRITE);
}

/*
 * common code for read and write calls:
 * check permissions, set base, count, and offset,
 * and switch out to readi or writei code.
 */
rdwr(mode)
register mode;
{
	register struct file *fp;
	register struct inode *ip;
	register struct a {
		int	fdes;
		char	*cbuf;
		unsigned count;
	} *uap;
	int	type;

	uap = (struct a *)u.u_ap;
	fp = getf(uap->fdes);
	if (fp == NULL)
		return;
	if ((fp->f_flag&mode) == 0) {
		u.u_error = EBADF;
		return;
	}
	u.u_base = (caddr_t)uap->cbuf;
	u.u_count = uap->count;
	u.u_segflg = 0;
	u.u_fmode = fp->f_flag;
	ip = fp->f_inode;
#ifdef	PNETDFS
	if ( ip->i_flag & IRMT ) {
		rrdwr( ip, uap->fdes, mode );
		goto dfsout;
	}
#endif
	type = ip->i_mode&IFMT;
	if (type==IFREG || type==IFDIR) {
		if ((u.u_fmode&FAPPEND) && (mode == FWRITE))
			fp->f_offset = ip->i_size;
		if (ip->i_locklist &&
			locked(1, ip, fp->f_offset, fp->f_offset + u.u_count))
				return;
		plock(ip);
	} else if (type == IFIFO) {
		fp->f_offset = 0;
		if (ip->i_locklist &&
			locked(1, ip, fp->f_offset, fp->f_offset + u.u_count))
				return;
		plock(ip);
	}
	u.u_offset = fp->f_offset;
	if (mode == FREAD)
		readi(ip);
	else
		writei(ip);
	if (type==IFREG || type==IFDIR || type==IFIFO)
		prele(ip);
#ifdef	PNETDFS
	dfsout:
#endif
	fp->f_offset += uap->count-u.u_count;
	u.u_rval1 = uap->count-u.u_count;
	u.u_ioch += u.u_rval1;
	if (mode == FREAD)
		sysinfo.readch += u.u_rval1;
	else
		sysinfo.writech += u.u_rval1;
}

/*
 * open system call
 */
open()
{
	register struct a {
		char	*fname;
		int	mode;
		int	crtmode;
	} *uap;

	uap = (struct a *)u.u_ap;
	copen(uap->mode-FOPEN, uap->crtmode);
}

/*
 * creat system call
 */
creat()
{
	register struct a {
		char	*fname;
		int	fmode;
	} *uap;

	uap = (struct a *)u.u_ap;
	copen(FWRITE|FCREAT|FTRUNC, uap->fmode);
}

/*
 * common code for open and creat.
 * Check permissions, allocate an open file structure,
 * and call the device open routine if any.
 */
copen(mode, arg)
register mode;
{
	register struct inode *ip;
	register struct file *fp;
	int i;

	if ((mode&(FREAD|FWRITE)) == 0) {
		u.u_error = EINVAL;
		return;
	}
	if (mode&FCREAT) {
#ifndef	PNETDFS
		ip = namei(uchar, 1);
#else
		ip = namei(uchar, 1, NULL);
#endif
		if (ip == NULL) {
			if (u.u_error)
				return;
			ip = maknode(arg&07777&(~ISVTX));
			if (ip == NULL)
				return;
			mode &= ~FTRUNC;
		} else {
			if (ip->i_locklist != NULL &&
				(ip->i_flag & IFMT) == IFREG &&
				locked(2, ip, (long)(0L), (long)(1L<<30))) {
					iput(ip);
					return;
			}
			if (mode&FEXCL) {
				u.u_error = EEXIST;
				iput(ip);
				return;
			}
			mode &= ~FCREAT;
		}
	} else {
#ifndef	PNETDFS
		ip = namei(uchar, 0);
#else
		ip = namei(uchar, 0, NULL);
#endif
		if (ip == NULL)
			return;
	}
	if (!(mode&FCREAT)) {
		if (mode&FREAD)
			access(ip, IREAD);
		if (mode&(FWRITE|FTRUNC)) {
			access(ip, IWRITE);
			if ((ip->i_mode&IFMT) == IFDIR)
				u.u_error = EISDIR;
		}
	}
	if (u.u_error || (fp = falloc()) == NULL) {
		iput(ip);
		return;
	}
	if (mode&FTRUNC)
		itrunc(ip);
	prele(ip);
	fp->f_flag = mode&FMASK;
	fp->f_inode = ip;
	i = u.u_rval1;
	if (setjmp(u.u_qsav)) {	/* catch half-opens */
		if (u.u_abreq)
			(*u.u_abreq)();
		if (u.u_error == 0)
			u.u_error = EINTR;
		closef(fp);
		u.u_ofile[i] = NULL;
	} else {
		openi(ip, mode);
		if (u.u_error == 0)
			return;
		u.u_ofile[i] = NULL;
		fp->f_count--;
		iput(ip);
	}
}

/*
 * close system call
 */
close()
{
	register struct file *fp;
	register struct a {
		int	fdes;
	} *uap;

	uap = (struct a *)u.u_ap;
	fp = getf(uap->fdes);
	if (fp == NULL)
		return;
	closef(fp);
	u.u_ofile[uap->fdes] = NULL;
}

/*
 * seek system call
 */
seek()
{
	register struct file *fp;
	register struct inode *ip;
	register struct a {
		int	fdes;
		off_t	off;
		int	sbase;
	} *uap;

	uap = (struct a *)u.u_ap;
	fp = getf(uap->fdes);
	if (fp == NULL)
		return;
	ip = fp->f_inode;
	if ((ip->i_mode&IFMT)==IFIFO) {
		u.u_error = ESPIPE;
		return;
	}
#ifdef	PNETDFS
	if ( ip->i_flag & IRMT ) {
if ( ddbug ) {
		printf( "remote seek %x\n", ip );
		debug();
}
		rseek( uap->fdes, uap->off, uap->sbase );
		return;
	}
#endif
	if (uap->sbase == 1)
		uap->off += fp->f_offset;
	else if (uap->sbase == 2)
		uap->off += fp->f_inode->i_size;
	else if (uap->sbase != 0) {
		u.u_error = EINVAL;
		psignal(u.u_procp, SIGSYS);
		return;
	}
	if (uap->off < 0) {
		u.u_error = EINVAL;
		return;
	}
	fp->f_offset = uap->off;
	u.u_roff = uap->off;
}

/*
 * link system call
 */
link()
{
	register struct inode *ip, *xp;
	register struct a {
		char	*target;
		char	*linkname;
	} *uap;

	uap = (struct a *)u.u_ap;
#ifndef	PNETDFS
	ip = namei(uchar, 0);
#else
	ip = namei(uchar, 0, NULL);
#endif
	if (ip == NULL)
		return;
	if (ip->i_nlink >= MAXLINK) {
		u.u_error = EMLINK;
		goto out;
	}
	if ((ip->i_mode&IFMT)==IFDIR && !suser())
		goto out;
	/*
	 * Unlock to avoid possibly hanging the namei.
	 * Sadly, this means races. (Suppose someone
	 * deletes the file in the meantime?)
	 * Nor can it be locked again later
	 * because then there will be deadly
	 * embraces.
	 */
	prele(ip);
	u.u_dirp = (caddr_t)uap->linkname;
#ifndef	PNETDFS
	xp = namei(uchar, 1);
#else
	xp = namei(uchar, 1, NULL);
#endif
	if (xp != NULL) {
		u.u_error = EEXIST;
		iput(xp);
		goto out;
	}
	if (u.u_error)
		goto out;
#ifdef PNETDFS
	if ( u.u_pdir && u.u_pdir->i_flag & IRMT ) {
		printf( "link on remote dfs, not there yet\n" );
		debug();
		u.u_error = EXDEV;
		iput( u.u_pdir );
		goto	out;
	}
#endif
	if (u.u_pdir->i_dev != ip->i_dev) {
		iput(u.u_pdir);
		u.u_error = EXDEV;
		goto out;
	}
	wdir(ip);
	if (u.u_error==0) {
		ip->i_nlink++;
		ip->i_flag |= ICHG;
	}

out:
	iput(ip);
}

/*
 * mknod system call
 */
mknod()
{
	register struct inode *ip;
	register struct a {
		char	*fname;
		int	fmode;
		dev_t	dev;
	} *uap;
#ifdef	PNETDFS
	struct	inode	*rmaknode();
#endif

	uap = (struct a *)u.u_ap;
	if ((uap->fmode&IFMT) != IFIFO && !suser())
		return;
#ifndef	PNETDFS
	ip = namei(uchar, 1);
#else
	ip = namei(uchar, 1, NULL);
#endif
	if (ip != NULL) {
		u.u_error = EEXIST;
		goto out;
	}
	if (u.u_error)
		return;
#ifdef	PNETDFS
	if ( u.u_pdir->i_flag & IRMT ) {
		ip = rmaknode( uap->fmode );
		return;
	}
#endif
	ip = maknode(uap->fmode);
	if (ip == NULL)
		return;
	switch(ip->i_mode&IFMT) {
	case IFCHR:
	case IFBLK:
		ip->i_rdev = (dev_t)uap->dev;
		ip->i_flag |= ICHG;
	}

out:
	iput(ip);
}

/*
 * access system call
 */
saccess()
{
	register svuid, svgid;
	register struct inode *ip;
	register struct a {
		char	*fname;
		int	fmode;
	} *uap;

	uap = (struct a *)u.u_ap;
	svuid = u.u_uid;
	svgid = u.u_gid;
	u.u_uid = u.u_ruid;
	u.u_gid = u.u_rgid;
#ifndef	PNETDFS
	ip = namei(uchar, 0);
#else
	ip = namei(uchar, 0, NULL);
#endif
	if (ip != NULL) {
		if (uap->fmode&(IREAD>>6))
			access(ip, IREAD);
		if (uap->fmode&(IWRITE>>6))
			access(ip, IWRITE);
		if (uap->fmode&(IEXEC>>6))
			access(ip, IEXEC);
		iput(ip);
	}
	u.u_uid = svuid;
	u.u_gid = svgid;
}
