#include "sys/param.h"
#include "sys/systm.h"
#include "sys/dir.h"
#include "sys/user.h"
#include "sys/inode.h"
#include "sys/file.h"

/*
 * The sys-pipe entry.
 * Allocate an inode on the root device.
 * Allocate 2 file structures.
 * Put it all together with flags.
 */
pipe()
{
	register struct inode *ip;
	register struct file *rf, *wf;
	int r;

	ip = ialloc(pipedev);
	if(ip == NULL)
		return;
	rf = falloc();
	if(rf == NULL) {
		iput(ip);
		return;
	}
	r = u.u_rval1;
	wf = falloc();
	if(wf == NULL) {
		rf->f_count = 0;
		u.u_ofile[r] = NULL;
		iput(ip);
		return;
	}
	u.u_rval2 = u.u_rval1;
	u.u_rval1 = r;
	wf->f_flag = FWRITE;
	wf->f_inode = ip;
	rf->f_flag = FREAD;
	rf->f_inode = ip;
	ip->i_count = 2;
	ip->i_mode = IFIFO;
	ip->i_flag = IACC|IUPD|ICHG;
	ip->i_frcnt = 1;
	ip->i_fwcnt = 1;
	iupdat(ip, &time, &time);
}

/*
 * Open a pipe
 * Check read and write counts, delay as necessary
 */

openp(ip, mode)
register struct inode *ip;
register mode;
{
	if (mode&FREAD) {
		if (ip->i_frcnt++ == 0)
			wakeup((caddr_t)&ip->i_frcnt);
	}
	if (mode&FWRITE) {
		if (mode&FNDELAY && ip->i_frcnt == 0) {
			u.u_error = ENXIO;
			return;
		}
		if (ip->i_fwcnt++ == 0)
			wakeup((caddr_t)&ip->i_fwcnt);
	}
	if (mode&FREAD) {
		while (ip->i_fwcnt == 0) {
			if (mode&FNDELAY || ip->i_size)
				return;
			sleep(&ip->i_fwcnt, PPIPE);
		}
	}
	if (mode&FWRITE) {
		while (ip->i_frcnt == 0)
			sleep(&ip->i_frcnt, PPIPE);
	}
}

/*
 * Close a pipe
 * Update counts and cleanup
 */

closep(ip, mode)
register struct inode *ip;
register mode;
{
	register i;
	daddr_t bn;

	if (mode&FREAD) {
		if ((--ip->i_frcnt == 0) && (ip->i_fflag&IFIW)) {
			ip->i_fflag &= ~IFIW;
			wakeup((caddr_t)&ip->i_fwcnt);
		}
	}
	if (mode&FWRITE) {
		if ((--ip->i_fwcnt == 0) && (ip->i_fflag&IFIR)) {
			ip->i_fflag &= ~IFIR;
			wakeup((caddr_t)&ip->i_frcnt);
		}
	}
	if ((ip->i_frcnt == 0) && (ip->i_fwcnt == 0)) {
		for (i=NFADDR-1; i>=0; i--) {
			bn = ip->i_faddr[i];
			if (bn == (daddr_t)0)
				continue;
			ip->i_faddr[i] = (daddr_t)0;
			free(ip->i_dev, bn);
		}
		ip->i_size = 0;
		ip->i_frptr = 0;
		ip->i_fwptr = 0;
		ip->i_flag |= IUPD|ICHG;
	}
}

/*
 * Lock a pipe.
 * If its already locked,
 * set the WANT bit and sleep.
 */
plock(ip)
register struct inode *ip;
{

	while(ip->i_flag&ILOCK) {
		ip->i_flag |= IWANT;
		sleep((caddr_t)ip, PINOD);
	}
	ip->i_flag |= ILOCK;
#ifdef	PNETDFS
	if ( ip->i_flag & IRMT ) {
		rplock( ip );
	}
#endif
}

/*
 * Unlock a pipe.
 * If WANT bit is on,
 * wakeup.
 * This routine is also used
 * to unlock inodes in general.
 */
prele(ip)
register struct inode *ip;
{

#ifdef	PNETDFS
	extern	ddbug;
	if ( ip->i_flag & IRMT ) {
		if ( ip->i_flag & IMOUNT )
			if ( ddbug ) {
				printf( "prele on mounted file %x\n", ip );
				debug();
			}
		rprele( ip );
	}
#endif
	ip->i_flag &= ~ILOCK;
	if(ip->i_flag&IWANT) {
		ip->i_flag &= ~IWANT;
		wakeup((caddr_t)ip);
	}
}
