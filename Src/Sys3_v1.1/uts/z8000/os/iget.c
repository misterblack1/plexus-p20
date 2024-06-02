#include "sys/param.h"
#include "sys/systm.h"
#include "sys/sysinfo.h"
#ifdef	PNETDFS
#include "sys/stat.h"
#include "sys/pnetdfs.h"
#endif
#include "sys/mount.h"
#include "sys/dir.h"
#include "sys/user.h"
#include "sys/inode.h"
#include "sys/ino.h"
#include "sys/filsys.h"
#include "sys/buf.h"
#include "sys/var.h"

/*
 * Look up an inode by device,inumber.
 * If it is in core (in the inode structure), honor the locking protocol.
 * If it is not in core, read it in from the specified device.
 * If the inode is mounted on, perform the indicated indirection.
 * In all cases, a pointer to a locked inode structure is returned.
 *
 * printf warning: no inodes -- if the inode structure is full
 * panic: no imt -- if the mounted filesystem is not in the mount table.
 *	"cannot happen"
 */
struct inode *
iget(dev, ino)
dev_t dev;
ino_t ino;
{
	register i;
	register struct inode *ip;
	register struct inode *oip;
	register struct mount *mp;
	struct inode *iread();

	sysinfo.iget++;
loop:
	ip = &inode[0];
	oip = NULL;
	i = v.v_inode;
	do {
		if(ino == ip->i_number && dev == ip->i_dev) {
			goto found;
		}
		if(oip==NULL && ip->i_count==0)
			oip = ip;
		ip++;
	} while(--i);
	ip = oip;
	if(ip == NULL) {
		printf("Inode table overflow\n");
		u.u_error = ENFILE;
		return(NULL);
	}
	ip->i_dev = dev;
	ip->i_number = ino;
	ip->i_flag = ILOCK;
	ip->i_count++;
	ip->i_lastr = 0;
	return(iread(ip));
found:
	if((ip->i_flag&ILOCK) != 0) {
		ip->i_flag |= IWANT;
		sleep((caddr_t)ip, PINOD);
		goto loop;
	}
#ifndef	PNETDFS
	if((ip->i_flag&IMOUNT) != 0) {
#else
	if((ip->i_flag&IMOUNT) != 0 && !(ip->i_flag&IRMT)) {
#endif
		for(mp = &mount[0]; mp < (struct mount *)v.ve_mount; mp++)
		if(mp->m_inodp == ip) {
			dev = mp->m_dev;
			ino = ROOTINO;
			goto loop;
		}
		panic("no imt");
	}
	ip->i_count++;
	ip->i_flag |= ILOCK;
	return(ip);
}

struct inode *
iread(ip)
register struct inode *ip;
{
	register char *p1, *p2;
	struct buf *bp;
	unsigned i;

	bp = bread(ip->i_dev, itod(ip->i_number));
	if (u.u_error) {
		brelse(bp);
		iput(ip);
		return(NULL);
	}
	i = itoo(ip->i_number)*sizeof(struct dinode);
	copyio(paddr(bp)+i, &ip->i_mode,
	    sizeof(struct dinode)-3*sizeof(time_t), U_RKD);
	p1 = (char *)&ip->i_addr[NADDR];
	p2 = (char *)(&ip->i_addr[0]) + 3*NADDR;
	for(i=NADDR; i; i--) {
		*--p1 = *--p2;
		*--p1 = *--p2;
		*--p1 = *--p2;
		*--p1 = 0;
	}
	brelse(bp);
	return(ip);
}

/*
 * Decrement reference count of an inode structure.
 * On the last reference, write the inode out and if necessary,
 * truncate and deallocate the file.
 */
#ifdef	PNETDFS
extern	ddbug;
#endif
iput(ip)
register struct inode *ip;
{

#ifdef	PNETDFS
	if ( ip->i_flag & IRMT ) {
		if ( (ip->i_flag & IMOUNT) && ddbug ) {
			printf( "iput on mounted inode %x\n", ip );
			debug();
		}
		riput( ip );
		return;
	}
#endif
	if(ip->i_count == 1) {
		ip->i_flag |= ILOCK;
		if(ip->i_nlink <= 0) {
			itrunc(ip);
			ip->i_mode = 0;
			ip->i_flag |= IUPD|ICHG;
			ifree(ip->i_dev, ip->i_number);
		}
		if(ip->i_flag&(IACC|IUPD|ICHG))
			iupdat(ip, &time, &time);
		prele(ip);
		ip->i_flag = 0;
		ip->i_number = 0;
		ip->i_count = 0;
		return;
	}
	ip->i_count--;
#ifdef	PNETDFS
	if ( ip->i_count < 0 ) {
		printf( "iput i_count < 0\n" );
		debug();
		ip->i_count = 0;
	}
#endif
	prele(ip);
}

/*
 * Update the inode with the current time.
 */
iupdat(ip, ta, tm)
register struct inode *ip;
time_t *ta, *tm;
{
	register struct buf *bp;
	struct dinode *dp;
	register char *p1;
	char *p2;
	unsigned i;

	if(getfs(ip->i_dev)->s_ronly) {
		if(ip->i_flag&(IUPD|ICHG))
			u.u_error = EROFS;
		ip->i_flag &= ~(IACC|IUPD|ICHG);
		return;
	}
	bp = bread(ip->i_dev, itod(ip->i_number));
	if (bp->b_flags & B_ERROR) {
		brelse(bp);
		return;
	}
	dp = (struct dinode *)&u.u_t[0];
	dp->di_mode = ip->i_mode;
	dp->di_nlink = ip->i_nlink;
	dp->di_uid = ip->i_uid;
	dp->di_gid = ip->i_gid;
	dp->di_size = ip->i_size;
	p1 = (char *)dp->di_addr;
	p2 = (char *)ip->i_addr;
	if ((ip->i_mode&IFMT)==IFIFO) {
		for (i=0; i<NFADDR; i++) {
			if (*p2++ != 0)
				printf("iaddress > 2^24\n");
			*p1++ = *p2++;
			*p1++ = *p2++;
			*p1++ = *p2++;
		}
		for (; i<NADDR; i++) {
			*p1++ = 0;
			*p1++ = 0;
			*p1++ = 0;
		}
	} else
	for(i=0; i<NADDR; i++) {
		if(*p2++ != 0)
			printf("iaddress > 2^24\n");
		*p1++ = *p2++;
		*p1++ = *p2++;
		*p1++ = *p2++;
	}
	dp = &((struct dinode *)0)[itoo(ip->i_number)];
	copyio(paddr(bp)+(unsigned)dp, &u.u_t[0],
	    sizeof(struct dinode)-3*sizeof(time_t), U_WKD);
	if(ip->i_flag&IACC)
		biputl(paddr(bp), &dp->di_atime, *ta);
	if(ip->i_flag&IUPD)
		biputl(paddr(bp), &dp->di_mtime, *tm);
	if(ip->i_flag&ICHG)
		biputl(paddr(bp), &dp->di_ctime, time);
	ip->i_flag &= ~(IACC|IUPD|ICHG);
	bdwrite(bp);
}

/*
 * Free all the disk blocks associated with the specified inode structure.
 * The blocks of the file are removed in reverse order. This FILO
 * algorithm will tend to maintain
 * a contiguous free list much longer than FIFO.
 */
itrunc(ip)
register struct inode *ip;
{
	register i;
	dev_t dev;
	daddr_t bn;

#ifdef	PNETDFS
	if ( ip->i_flag & IRMT ) {
		if ( ip->i_flag & IMOUNT ) {
			printf( "itrunc on mounted inode %x\n", ip );
			printf( "in advisable to continue!!\n" );
			debug();
		} else {
			ritrunc( ip );
			return;
		}
	}
#endif
	i = ip->i_mode & IFMT;
	if (i!=IFREG && i!=IFDIR)
		return;
	dev = ip->i_dev;
	for(i=NADDR-1; i>=0; i--) {
		bn = ip->i_addr[i];
		if(bn == (daddr_t)0)
			continue;
		ip->i_addr[i] = (daddr_t)0;
		switch(i) {

		default:
			free(dev, bn);
			break;

		case NADDR-3:
			tloop(dev, bn, 0, 0);
			break;

		case NADDR-2:
			tloop(dev, bn, 1, 0);
			break;

		case NADDR-1:
			tloop(dev, bn, 1, 1);
		}
	}
	ip->i_size = 0;
	ip->i_flag |= IUPD|ICHG;
}

tloop(dev, bn, f1, f2)
dev_t dev;
daddr_t bn;
{
	register i;
	register struct buf *bp;
	daddr_t nb;

	bp = NULL;
	for(i=NINDIR-1; i>=0; i--) {
		if(bp == NULL) {
			bp = bread(dev, bn);
			if (bp->b_flags & B_ERROR) {
				brelse(bp);
				return;
			}
		}
		nb = bigetl(paddr(bp), &((daddr_t *)0)[i]);
		if(nb == (daddr_t)0)
			continue;
		if(f1) {
			brelse(bp);
			bp = NULL;
			tloop(dev, nb, f2, 0);
		} else
			free(dev, nb);
	}
	if(bp != NULL)
		brelse(bp);
	free(dev, bn);
}

/*
 * Make a new file.
 */
struct inode *
maknode(mode)
register mode;
{
	register struct inode *ip;

#ifdef	PNETDFS
	struct	inode	*rmaknode();

	if ( u.u_pdir->i_flag & IRMT ) {
		ip = rmaknode( mode, 0 );
		return( ip );
	}
#endif
	ip = ialloc(u.u_pdir->i_dev);
	if(ip == NULL) {
		iput(u.u_pdir);
		return(NULL);
	}
	ip->i_flag |= IACC|IUPD|ICHG;
	if((mode&IFMT) == 0)
		mode |= IFREG;
	ip->i_mode = mode & ~u.u_cmask;
	ip->i_nlink = 1;
	ip->i_uid = u.u_uid;
	ip->i_gid = u.u_gid;
	iupdat(ip, &time, &time);
	wdir(ip);
	return(ip);
}

/*
 * Write a directory entry with parameters left as side effects
 * to a call to namei.
 */
wdir(ip)
struct inode *ip;
{

	u.u_dent.d_ino = ip->i_number;
	u.u_count = sizeof(struct direct);
	u.u_segflg = 1;
	u.u_base = (caddr_t)&u.u_dent;
	writei(u.u_pdir);
	iput(u.u_pdir);
}
