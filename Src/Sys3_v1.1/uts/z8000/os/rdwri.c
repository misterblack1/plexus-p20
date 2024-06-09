#include "sys/param.h"
#include "sys/inode.h"
#include "sys/dir.h"
#include "sys/user.h"
#include "sys/buf.h"
#include "sys/conf.h"
#include "sys/file.h"
#include "sys/systm.h"

/*
 * Read the file corresponding to
 * the inode pointed at by the argument.
 * The actual read arguments are found
 * in the variables:
 *	u_base		core address for destination
 *	u_offset	byte offset in file
 *	u_count		number of bytes to read
 *	u_segflg	read to kernel/user/user I
 */
readi(ip)
register struct inode *ip;
{
	struct buf *bp;
	dev_t dev;
	daddr_t lbn, bn;
	off_t diff;
	register unsigned on, n;
	register type;

	if (u.u_count == 0)
		return;
	if (u.u_offset < 0) {
		u.u_error = EINVAL;
		return;
	}
#ifdef	PNETDFS
	if ( ip->i_flag & IRMT ) {
		srreadi( ip );
		return;
	}
#endif
	dev = (dev_t)ip->i_rdev;
	type = ip->i_mode&IFMT;
	switch(type) {
	case IFCHR:
		ip->i_flag |= IACC;
		(*cdevsw[major(dev)].d_read)(minor(dev));
		break;

	case IFIFO:
		while (ip->i_size == 0) {
			if (ip->i_fwcnt == 0)
				return;
			if (u.u_fmode&FNDELAY)
				return;
			ip->i_fflag |= IFIR;
			prele(ip);
			sleep((caddr_t)&ip->i_frcnt, PPIPE);
			plock(ip);
		}
		u.u_offset = ip->i_frptr;
	case IFBLK:
	case IFREG:
	case IFDIR:
	do {
		lbn = bn = u.u_offset >> BSHIFT;
		on = u.u_offset & BMASK;
		n = min((unsigned)(BSIZE-on), u.u_count);
		if (type != IFBLK) {
			if (type == IFIFO)
				diff = ip->i_size;
			else
				diff = ip->i_size - u.u_offset;
			if (diff <= 0)
				break;
			if (diff < n)
				n = diff;
			bn = bmap(ip, bn, B_READ);
			if (u.u_error)
				break;
			dev = ip->i_dev;
		} else
			rablock = bn+1;
		if ((long)bn<0) {
			if (type != IFREG)
				break;
			bp = getablk(0);
			clear(paddr(bp), BSIZE);
			bp->b_resid = 0;
		} else if (ip->i_lastr+1 == lbn &&
		    (type != IFIFO) &&
		    (on+n) == BSIZE)
			bp = breada(dev, bn, rablock);
		else
			bp = bread(dev, bn);
		if ((on+n) == BSIZE)
			ip->i_lastr = lbn;
		if (bp->b_resid) {
			n = 0;
		}
		if (n!=0)
			pimove(paddr(bp)+on, n, B_READ);
		if (type == IFIFO) {
			ip->i_size -= n;
			if (u.u_offset >= PIPSIZ)
				u.u_offset = 0;
			if ((on+n) == BSIZE && ip->i_size < (PIPSIZ-BSIZE))
				bp->b_flags &= ~B_DELWRI;
		}
		brelse(bp);
		ip->i_flag |= IACC;
	} while (u.u_error==0 && u.u_count!=0 && n!=0);
		if (type == IFIFO) {
			if (ip->i_size)
				ip->i_frptr = u.u_offset;
			else
				ip->i_frptr = ip->i_fwptr = 0;
			if (ip->i_fflag&IFIW) {
				ip->i_fflag &= ~IFIW;
				curpri = PPIPE;
				wakeup((caddr_t)&ip->i_fwcnt);
			}
		}
		break;

	default:
		u.u_error = ENODEV;
	}
}

/*
 * Write the file corresponding to
 * the inode pointed at by the argument.
 * The actual write arguments are found
 * in the variables:
 *	u_base		core address for source
 *	u_offset	byte offset in file
 *	u_count		number of bytes to write
 *	u_segflg	write to kernel/user/user I
 */
writei(ip)
register struct inode *ip;
{
	struct buf *bp;
	dev_t dev;
	daddr_t bn;
	register unsigned n, on;
	register type;
	unsigned int usave;

	if (u.u_offset < 0) {
		u.u_error = EINVAL;
		return;
	}
#ifdef	PNETDFS
	if ( ip->i_flag & IRMT ) {
		srwritei( ip );
		return;
	}
#endif
	dev = (dev_t)ip->i_rdev;
	type = ip->i_mode&IFMT;
	switch (type) {
	case IFCHR:
		ip->i_flag |= IUPD|ICHG;
		(*cdevsw[major(dev)].d_write)(minor(dev));
		break;

	case IFIFO:
	floop:
		usave = 0;
		while ((u.u_count+ip->i_size) > PIPSIZ) {
			if (ip->i_frcnt == 0)
				break;
			if ((u.u_count > PIPSIZ) && (ip->i_size < PIPSIZ)) {
				usave = u.u_count;
				u.u_count = PIPSIZ - ip->i_size;
				usave -= u.u_count;
				break;
			}
			if (u.u_fmode&FNDELAY)
				return;
			ip->i_fflag |= IFIW;
			prele(ip);
			sleep((caddr_t)&ip->i_fwcnt, PPIPE);
			plock(ip);
		}
		if (ip->i_frcnt == 0) {
			u.u_error = EPIPE;
			psignal(u.u_procp, SIGPIPE);
			break;
		}
		u.u_offset = ip->i_fwptr;
	case IFBLK:
	case IFREG:
	case IFDIR:
	while (u.u_error==0 && u.u_count!=0) {
		bn = u.u_offset >> BSHIFT;
		on = u.u_offset & BMASK;
		n = min((unsigned)(BSIZE-on), u.u_count);
		if (type != IFBLK) {
			bn = bmap(ip, bn, B_WRITE);
			if (u.u_error)
				break;
			dev = ip->i_dev;
		}
		if (n == BSIZE) 
			bp = getblk(dev, bn);
		else if (type==IFIFO && on==0 && ip->i_size < (PIPSIZ-BSIZE))
			bp = getblk(dev, bn);
		else
			bp = bread(dev, bn);
		pimove(paddr(bp)+on, n, B_WRITE);
		if (u.u_error)
			brelse(bp);
		else
			bdwrite(bp);
		if (type == IFREG || type == IFDIR) {
			if (u.u_offset > ip->i_size)
				ip->i_size = u.u_offset;
		} else if (type == IFIFO) {
			ip->i_size += n;
			if (u.u_offset == PIPSIZ)
				u.u_offset = 0;
		}
		ip->i_flag |= IUPD|ICHG;
	}
		if (type == IFIFO) {
			ip->i_fwptr = u.u_offset;
			if (ip->i_fflag&IFIR) {
				ip->i_fflag &= ~IFIR;
				curpri = PPIPE;
				wakeup((caddr_t)&ip->i_frcnt);
			}
			if (u.u_error==0 && usave!=0) {
				u.u_count = usave;
				goto floop;
			}
		}
		break;

	default:
		u.u_error = ENODEV;
	}
}

/*
 * Move n bytes at byte location
 * cp to/from (flag) the
 * user/kernel (u.segflg) area starting at u.base.
 * Update all the arguments by the number
 * of bytes moved.
 */
pimove(cp, n, flag)
long cp;
register unsigned n;
{
	if (copyio(cp, u.u_base, n, (u.u_segflg<<1)|flag))
		u.u_error = EFAULT;
	else {
		u.u_base += n;
		u.u_offset += n;
		u.u_count -= n;
	}
}
