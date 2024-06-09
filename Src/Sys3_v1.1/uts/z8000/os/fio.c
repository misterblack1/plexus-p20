#include "sys/param.h"
#include "sys/systm.h"
#include "sys/dir.h"
#include "sys/user.h"
#include "sys/filsys.h"
#include "sys/file.h"
#include "sys/conf.h"
#include "sys/inode.h"
#ifdef PNETDFS
#include "sys/stat.h"
#include "sys/pnetdfs.h"
#endif
#include "sys/mount.h"
#include "sys/var.h"
#include "sys/acct.h"

/*
 * Convert a user supplied file descriptor into a pointer
 * to a file structure.
 * Only task is to check range of the descriptor.
 */
struct file *
getf(f)
register int f;
{
	register struct file *fp;

	if(0 <= f && f < NOFILE) {
		fp = u.u_ofile[f];
		if(fp != NULL)
			return(fp);
	}
	u.u_error = EBADF;
	return(NULL);
}

/*
 * Internal form of close.
 * Decrement reference count on file structure.
 * Also make sure the pipe protocol does not constipate.
 *
 * Decrement reference count on the inode following
 * removal to the referencing file structure.
 * On the last close switch out to the device handler for
 * special files.  Note that the handler is called
 * on every open but only the last close.
 */
closef(fp)
register struct file *fp;
{
	register struct inode *ip;
	int flag, fmt;
	dev_t dev;
	register int (*cfunc)();

	if(fp == NULL)
		return;
#ifdef	PNETDFS
	if ( fp->f_inode->i_flag&IRMT ) {
		rclosef( fp );
		return;
	}
#endif
	unlock(fp->f_inode);
	if ((unsigned)fp->f_count > 1) {
		fp->f_count--;
		return;
	}
	ip = fp->f_inode;
	plock(ip);
	flag = fp->f_flag;
	dev = (dev_t)ip->i_rdev;
	fmt = ip->i_mode&IFMT;
	fp->f_count = 0;
	switch(fmt) {

	case IFCHR:
		cfunc = cdevsw[major(dev)].d_close;
		break;

	case IFBLK:
		cfunc = bdevsw[major(dev)].d_close;
		break;

	case IFIFO:
		closep(ip, flag);

	default:
		iput(ip);
		return;
	}
	for (fp = file; fp < (struct file *)v.ve_file; fp++) {
		register struct inode *tip;

		if (fp->f_count) {
			tip = fp->f_inode;
			if (tip->i_rdev == dev &&
			  (tip->i_mode&IFMT) == fmt)
				goto out;
		}
	}
	if (fmt == IFBLK) {
		register struct mount *mp;

		for (mp = mount; mp < (struct mount *)v.ve_mount; mp++)
			if (mp->m_flags == MINUSE && mp->m_dev == dev)
				goto out;
		bflush(dev);
		(*cfunc)(minor(dev), flag);
		binval(dev);
	} else {
		prele(ip);
		(*cfunc)(minor(dev), flag);
	}
out:
	iput(ip);
}

/*
 * openi called to allow handler of special files to initialize and
 * validate before actual IO.
 */
openi(ip, flag)
register struct inode *ip;
{
	dev_t dev;
	register unsigned int maj;

#ifdef	PNETDFS
	if ( ip->i_flag & IRMT ) {
		ropeni( ip, flag );
		return;
	}
#endif
	dev = (dev_t)ip->i_rdev;
	maj = major(dev);
	switch(ip->i_mode&IFMT) {

	case IFCHR:
		if(maj >= cdevcnt)
			goto bad;
		if (u.u_ttyp == NULL)
			u.u_ttyd = dev;
		(*cdevsw[maj].d_open)(minor(dev), flag);
		break;

	case IFBLK:
		if(maj >= bdevcnt)
			goto bad;
		(*bdevsw[maj].d_open)(minor(dev), flag);
		break;

	case IFIFO:
		openp(ip, flag);
		break;
	}
	return;

bad:
	u.u_error = ENXIO;
}

/*
 * Check mode permission on inode pointer.
 * Mode is READ, WRITE or EXEC.
 * In the case of WRITE, the read-only status of the file
 * system is checked. Also in WRITE, prototype text
 * segments cannot be written.
 * The mode is shifted to select the owner/group/other fields.
 * The super user is granted all permissions.
 */
access(ip, mode)
register struct inode *ip;
{
	register m;

	m = mode;
	if(m == IWRITE) {
#ifdef	PNETDFS
		if ( major(ip->i_dev) == DFSMAJ ) {
			if ( minor(ip->i_dev) & DFSRO ) {
				u.u_error = EROFS;
				return(1);
			}
		} else if (getfs(ip->i_dev)->s_ronly != 0) {
			u.u_error = EROFS;
			return(1);
		       }
#else
		if(getfs(ip->i_dev)->s_ronly != 0) {
			u.u_error = EROFS;
			return(1);
		}
#endif
		if (ip->i_flag&ITEXT)
			xrele(ip);
		if(ip->i_flag & ITEXT) {
			u.u_error = ETXTBSY;
			return(1);
		}
	}
	if(u.u_uid == 0)
		return(0);
	if(u.u_uid != ip->i_uid) {
		m >>= 3;
		if(u.u_gid != ip->i_gid)
			m >>= 3;
	}
	if((ip->i_mode&m) != 0)
		return(0);

	u.u_error = EACCES;
	return(1);
}

/*
 * Look up a pathname and test if the resultant inode is owned by the
 * current user. If not, try for super-user.
 * If permission is granted, return inode pointer.
 */
struct inode *
owner()
{
	register struct inode *ip;

#ifndef	PNETDFS
	ip = namei(uchar, 0);
#else
	ip = namei(uchar, 0, NULL);
#endif
	if(ip == NULL)
		return(NULL);
	if(u.u_uid == ip->i_uid)
		return(ip);
	if(suser())
		return(ip);
	iput(ip);
	return(NULL);
}

/*
 * Test if the current user is the super user.
 */
suser()
{

	if(u.u_uid == 0) {
		u.u_acflag |= ASU;
		return(1);
	}
	u.u_error = EPERM;
	return(0);
}

/*
 * Allocate a user file descriptor.
 */
ufalloc(i)
register i;
{

	for(; i<NOFILE; i++)
		if(u.u_ofile[i] == NULL) {
			u.u_rval1 = i;
			u.u_pofile[i] = 0;
			return(i);
		}
	u.u_error = EMFILE;
	return(-1);
}

/*
 * Allocate a user file descriptor and a file structure.
 * Initialize the descriptor to point at the file structure.
 *
 * no file -- if there are no available file structures.
 */
struct file *
falloc()
{
	register struct file *fp;
	register i;

	i = ufalloc(0);
	if(i < 0)
		return(NULL);
	for(fp = &file[0]; fp < (struct file *)v.ve_file; fp++)
		if(fp->f_count == 0) {
			u.u_ofile[i] = fp;
			fp->f_count++;
			fp->f_offset = 0;
			return(fp);
		}
	printf("no file\n");
	u.u_error = ENFILE;
	return(NULL);
}
