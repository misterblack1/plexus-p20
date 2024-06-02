#ifdef	PNETDFS
#include "sys/param.h"
#include "sys/systm.h"
/*
#include "sys/ino.h"
*/
#include "sys/buf.h"
#include "sys/dir.h"
#include "sys/user.h"
#include "sys/inode.h"
#include "sys/file.h"
#include "sys/conf.h"
#include "sys/stat.h"
#include "sys/ioctl.h"
#include "sys/var.h"
#include "sys/utsname.h"
#include "sys/pnetdfs.h"
#include "sys/mount.h"

extern	ddbug;

/*
 * the remote mount system call.
 */
rmount()
{
	register struct inode *ip;
	struct	dfsmnt	*mmsg;
	struct	dfswe	*dwe;
	struct	dfswe	*gdfswe();
	int	i;
	register struct a {
		char	*rmtdir;
		char	*rmtnode;
		char	*lcldir;
		int	ronly;
	} *uap;

	uap = (struct a *)u.u_ap;
/*
 * Only the super user can executed remote mounts.
 */
	if(!suser())
		return;
/*
 * See if the local directory exists, has no others using it, 
 * is not a remote inode, and is not the root of any file system.
 */
	u.u_dirp = (caddr_t)uap->lcldir;
	ip = namei(uchar, 0, NULL);
	if(ip == NULL)
		return;
	if ((ip->i_mode&IFMT) != IFDIR) {
		u.u_error = ENOTDIR;
		goto out;
	}
	if (ip->i_count != 1)
		goto out;
	if ( ip->i_flag & IRMT )
		goto out;
	if (ip->i_number == ROOTINO)
		goto out;
/*
 * Set the inode as remote.
 */
	ip->i_flag |= IRMT;

/*
 * Set up message to mount process.
 */
	mmsg = (struct dfsmnt *)smalloc( sizeof(struct dfsmnt));
	mmsg->dm_type = DFSMNTRQ;
	gustrng( uap->rmtdir, mmsg->dm_rdir, sizeof(mmsg->dm_rdir) );
	gustrng( uap->lcldir, mmsg->dm_ldir, sizeof(mmsg->dm_ldir) );
	gustrng( uap->rmtnode, mmsg->dm_node, sizeof(mmsg->dm_node) );
/*
 * Get a dfs work entry for the mount process and point to the mount 
 * message.
 */

	dwe = gdfswe();
	dwe->wm_msg = mmsg;
	dwe->wm_ip = ip;
	dwe->wm_type = DWEMNT;

/*
 * Call dmount to start mount process and give it request.
 */

	adfsq( dwe );

/*
 * See if mount succeeded.
 */

	if ( dwe->wm_stat != DFSOK ) {
		goto out1;
	}

/* 
 * The request succeeded.  Release the inode and return the dfs
 * work entry and message block.
 */
	ip->i_flag |= IMOUNT;
	prele(ip);
	rdfswe( dwe );
	smfree( sizeof(struct dfsmnt), mmsg );
	return;

out1:
	ip->i_flag &= ~IRMT;
	rdfswe( dwe );
	smfree( sizeof(struct dfsmnt), mmsg );

out:
	if (u.u_error == 0)
		u.u_error = EBUSY;
	iput(ip);
}

/*
 * the umount system call.
 */
rumount()
{
	register struct inode *ip;
	register struct mount *mp, *smp;
	register struct dfswe *dwe;
	struct	dfswe	*gdfswe();
	char	rdir[50];
	char	rnode[9];
	register struct a {
		char	*rmtdir;
		char	*rmtnode;
	} *uap;

	if(!suser())
		return;
	uap = (struct a *)u.u_ap;
	gustrng( uap->rmtdir, rdir, sizeof( rdir ) );
	gustrng( uap->rmtnode, rnode, sizeof( rnode ) );
	if(u.u_error)
		return;
if ( ddbug ) {
	printf( "rumount called %s, %s\n", rdir, rnode );
	debug();
}
	smp = NULL;
	for ( mp = &mount[0]; mp < (struct mount *)v.ve_mount; mp++ ) {
		if ( mp->m_flags != (MINUSE|MREMOTE) )
			continue;
		if ( !(nstrncmp( mp->m_node, rnode )) &&
		     !(nstrncmp( mp->m_dir, rdir )) ) {
			smp = mp;
			break;
		}
	}
	if ( smp == NULL ) {
		u.u_error = EINVAL;
		return;
	}
	plock( smp->m_inodp );
	if ( smp->m_inodp->i_count != 1 )
		goto	outum;
	for(ip = &inode[0]; ip < (struct inode *)v.ve_inode; ip++) {
		if ( ip == smp->m_inodp )
			continue;
		if ( (ip->i_flag & IRMT) && 
				(ip->i_dfsid == (int *)smp->m_dpro) )
			goto outum;
	}
	iput(smp->m_inodp);
	mp->m_flags = MFREE;

	dwe = gdfswe();
	dwe->we_type = DWEUMNT;
	adddwe( smp->m_dpro, dwe );
	return;

	outum:
	prele( smp->m_inodp );
	u.u_error = EBUSY;
	return;
}

/*
 * Get string from user space.
 */

gustrng( from, to, cnt )
register char	*to, *from;
{
	register	c;

	if ( cnt < 0 )
		return;
	while ( cnt-- ) {
		if ( (c = fubyte( from++ )) == -1 ) {
			u.u_error = EFAULT;
			return;
		}
		*to++ = c;
		if ( !c )
			return;
	}
}
#endif
