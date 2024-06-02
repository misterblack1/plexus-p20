#include "sys/param.h"
#include "sys/systm.h"
#include "sys/stat.h"
#include "sys/mount.h"
#include "sys/ino.h"
#include "sys/buf.h"
#include "sys/filsys.h"
#include "sys/dir.h"
#include "sys/user.h"
#include "sys/inode.h"
#include "sys/file.h"
#include "sys/conf.h"
#include "sys/ioctl.h"
#include "sys/var.h"
#ifdef	PNETDFS
extern	ddbug;
#endif

/*
 * the fstat system call.
 */
fstat()
{
	register struct file *fp;
	register struct a {
		int	fdes;
		struct stat *sb;
	} *uap;

	uap = (struct a *)u.u_ap;
	fp = getf(uap->fdes);
	if(fp == NULL)
		return;
	stat1(fp->f_inode, uap->sb);
}

/*
 * the stat system call.
 */
stat()
{
	register struct inode *ip;
	register struct a {
		char	*fname;
		struct stat *sb;
	} *uap;

	uap = (struct a *)u.u_ap;
#ifndef	PNETDFS
	ip = namei(uchar, 0);
#else
	ip = namei(uchar, 0, NULL);
#endif
	if(ip == NULL)
		return;
	stat1(ip, uap->sb);
	iput(ip);
}

/*
 * The basic routine for fstat and stat:
 * get the inode and pass appropriate parts back.
 */
stat1(ip, ub)
register struct inode *ip;
struct stat *ub;
{
	register struct buf *bp;
	struct stat ds;

#ifdef	PNETDFS
	if ( ip->i_flag & IRMT ) {
		rstat1( ip, ub );
		return;
	}
#endif

	if(ip->i_flag&(IACC|IUPD|ICHG))
		iupdat(ip, &time, &time);
	/*
	 * first copy from inode table
	 */
	ds.st_dev = ip->i_dev;
	ds.st_ino = ip->i_number;
	ds.st_mode = ip->i_mode;
	ds.st_nlink = ip->i_nlink;
	ds.st_uid = ip->i_uid;
	ds.st_gid = ip->i_gid;
	ds.st_rdev = (dev_t)ip->i_rdev;
	ds.st_size = ip->i_size;
	/*
	 * next the dates in the disk
	 */
	bp = bread(ip->i_dev, itod(ip->i_number));
	copyio(paddr(bp)+itoo(ip->i_number)*sizeof(struct dinode)
		+(long)(&((struct dinode *)0)->di_atime),
	    &ds.st_atime, 3*sizeof(time_t), U_RKD);
	brelse(bp);
	if (copyout((caddr_t)&ds, (caddr_t)ub, sizeof(ds)) < 0)
		u.u_error = EFAULT;
}

/*
 * the dup system call.
 *
 * dup2 has been added to the stock SYSTEM III code for compatibility
 * with v7.
 */
dup()
{
	register struct file *fp;
	register struct a {
		int	fdes;
		int	fdes2;
	} *uap;
	register i,m;

	uap = (struct a *)u.u_ap;
	m = uap->fdes & ~077;
	uap->fdes &= 077;
	fp = getf(uap->fdes);
	if(fp == NULL)
		return;
	if ((m&0100) == 0) {
		if ((i = ufalloc(0)) < 0)
			return;
	} else {
		i = uap->fdes2;
		if (i<0 || i>=NOFILE) {
			u.u_error = EBADF;
			return;
		}
		u.u_rval1 = i;
	}
	if (i!=uap->fdes) {
		if (u.u_ofile[i]!=NULL)
			closef(u.u_ofile[i]);
		u.u_ofile[i] = fp;
		fp->f_count++;
	}
}

/*
 * the file control system call.
 */
fcntl()
{
	register struct file *fp;
	register struct a {
		int	fdes;
		int	cmd;
		int	arg;
	} *uap;
	register i;

	uap = (struct a *)u.u_ap;
	fp = getf(uap->fdes);
	if (fp == NULL)
		return;
	switch(uap->cmd) {
	case 0:
		i = uap->arg;
		if (i < 0 || i > NOFILE) {
			u.u_error = EINVAL;
			return;
		}
		if ((i = ufalloc(i)) < 0)
			return;
#ifdef	PNETDFS
		if ( fp->f_inode->i_flag & IRMT ) {
if ( ddbug ) {
			printf( "fcntl dup on remote file %x\n", fp );
			debug();
}
			rfcntl( uap->fdes, i, 0, uap->arg );
		}
#endif
		u.u_ofile[i] = fp;
		fp->f_count++;
		break;

	case 1:
		u.u_rval1 = u.u_pofile[uap->fdes];
		break;

	case 2:
		u.u_pofile[uap->fdes] = uap->arg;
		break;

	case 3:
		u.u_rval1 = fp->f_flag+FOPEN;
		break;

	case 4:
		fp->f_flag &= (FREAD|FWRITE);
#ifdef	PNETDFS
		if ( fp->f_inode->i_flag & IRMT ) {
if ( ddbug ) {
			printf( "fcntl setfl on remote file %x\n", fp );
			debug();
}
			rfcntl( uap->fdes, i, 4, uap->arg );
		}
#endif
		fp->f_flag |= (uap->arg-FOPEN) & ~(FREAD|FWRITE);
		break;

	default:
		u.u_error = EINVAL;
	}
}

/*
 * character special i/o control
 */
ioctl()
{
	register struct file *fp;
	register struct inode *ip;
	register struct a {
		int	fdes;
		int	cmd;
		int	arg;
	} *uap;
	register dev_t dev;

	uap = (struct a *)u.u_ap;
	if ((fp = getf(uap->fdes)) == NULL)
		return;
	ip = fp->f_inode;
	if ((ip->i_mode&IFMT) != IFCHR) {
		u.u_error = ENOTTY;
		return;
	}
	u.u_rval1 = 0;
#ifdef	PNETDFS
	if ( ip->i_flag & IRMT ) {
		rioctl( uap->fdes, uap->cmd, uap->arg );
		return;
	}
#endif
	dev = (dev_t)ip->i_rdev;
	(*cdevsw[major(dev)].d_ioctl)(minor(dev),uap->cmd,uap->arg,fp->f_flag);
}

/*
 * old stty and gtty
 */
stty()
{
	register struct a {
		int	fdes;
		int	arg;
		int	narg;
	} *uap;

	uap = (struct a *)u.u_ap;
	uap->narg = uap->arg;
	uap->arg = TIOCSETP;
	ioctl();
}

gtty()
{
	register struct a {
		int	fdes;
		int	arg;
		int	narg;
	} *uap;

	uap = (struct a *)u.u_ap;
	uap->narg = uap->arg;
	uap->arg = TIOCGETP;
	ioctl();
}

/*
 * the mount system call.
 */
smount()
{
	dev_t dev;
	register struct inode *ip;
	register struct mount *mp;
	struct mount *smp;
	register struct filsys *fp;
	struct buf *bp;
	register struct a {
		char	*fspec;
		char	*freg;
		int	ronly;
	} *uap;

	uap = (struct a *)u.u_ap;
	if(!suser())
		return;
	dev = getmdev();
	if(u.u_error)
		return;
	u.u_dirp = (caddr_t)uap->freg;
#ifndef	PNETDFS
	ip = namei(uchar, 0);
#else
	ip = namei(uchar, 0, NULL);
#endif
	if(ip == NULL)
		return;
	if ((ip->i_mode&IFMT) != IFDIR) {
		u.u_error = ENOTDIR;
		goto out;
	}
	if (ip->i_count != 1)
		goto out;
	if (ip->i_number == ROOTINO)
		goto out;
	smp = NULL;
	for(mp = &mount[0]; mp < (struct mount *)v.ve_mount; mp++) {
		if(mp->m_flags != MFREE) {
			if(dev == mp->m_dev)
				goto out;
		} else
		if(smp == NULL)
			smp = mp;
	}
	mp = smp;
	if(mp == NULL)
		goto out;
	mp->m_flags = MINTER;
	mp->m_dev = dev;
	(*bdevsw[major(dev)].d_open)(minor(dev), !uap->ronly);
	if(u.u_error)
		goto out1;
	bp = bread(dev, SUPERB);
	if(u.u_error) {
		brelse(bp);
		goto out1;
	}
	mp->m_inodp = ip;
	mp->m_bufp = getablk(1);
	fp = (struct filsys *)paddr(mp->m_bufp);
	copyio(paddr(bp), fp, sizeof(struct filsys), U_RKD);
	mp->m_flags = MINUSE;
	fp->s_ilock = 0;
	fp->s_flock = 0;
	fp->s_ninode = 0;
	fp->s_inode[0] = 0;
	fp->s_ronly = uap->ronly & 1;
	brelse(bp);
	ip->i_flag |= IMOUNT;
	prele(ip);
	return;

out1:
	mp->m_flags = MFREE;
out:
	if (u.u_error == 0)
		u.u_error = EBUSY;
	iput(ip);
}

/*
 * the umount system call.
 */
sumount()
{
	dev_t dev;
	register struct inode *ip;
	register struct mount *mp;
	register struct a {
		char	*fspec;
	};

	if(!suser())
		return;
	dev = getmdev();
	if(u.u_error)
		return;
	xumount(dev);	/* remove unused sticky files from text table */
	update();
	for(mp = &mount[0]; mp < (struct mount *)v.ve_mount; mp++)
		if(mp->m_flags == MINUSE && dev == mp->m_dev)
			goto found;
	u.u_error = EINVAL;
	return;

found:
	for(ip = &inode[0]; ip < (struct inode *)v.ve_inode; ip++)
		if(ip->i_number != 0 && dev == ip->i_dev) {
			u.u_error = EBUSY;
			return;
		}
	(*bdevsw[major(dev)].d_close)(minor(dev), 0);
	binval(dev);
	ip = mp->m_inodp;
	ip->i_flag &= ~IMOUNT;
	plock(ip);
	iput(ip);
	brelse(mp->m_bufp);
	mp->m_bufp = NULL;
	mp->m_flags = MFREE;
}

/*
 * Common code for mount and umount.
 * Check that the user's argument is a reasonable
 * thing on which to mount, and return the device number if so.
 */
dev_t
getmdev()
{
	dev_t dev;
	register struct inode *ip;

#ifndef	PNETDFS
	ip = namei(uchar, 0);
#else
	ip = namei(uchar, 0, NULL);
#endif
	if(ip == NULL)
		return(NODEV);
	if((ip->i_mode&IFMT) != IFBLK)
		u.u_error = ENOTBLK;
	dev = (dev_t)ip->i_rdev;
	if(major(dev) >= bdevcnt)
		u.u_error = ENXIO;
	iput(ip);
	return(dev);
}
