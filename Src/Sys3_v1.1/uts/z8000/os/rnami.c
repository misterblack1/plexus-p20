#ifdef	PNETDFS
/*
 * rnamei - remote file namei.  Same as namei except conciders remote
 *	    system files.
 */

#include "sys/param.h"
#include "sys/systm.h"
#include "sys/sysinfo.h"
#include "sys/inode.h"
#include "sys/stat.h"
#include "sys/pnetdfs.h"
#include "sys/mount.h"
#include "sys/dir.h"
#include "sys/user.h"
#include "sys/buf.h"
#include "sys/var.h"

extern ushort ddbug;
struct inode *
rnamei(dp, c, flag, func)
int (*func)();
register struct inode *dp;
{
	register struct dfsdfs	*msg, *rmsg;
	register struct dfswe	*dwe;
	register struct inode	*ip;
	register *id;
	caddr_t	sdirp;
	struct	inode	*riget();
	struct	dfswe	*gdfswe();
	struct buf *bp;
	int i, spri;

if ( ddbug ) {
	printf( "rnamei called %x, %x, %x, %x\n", dp, c, flag, func );
	debug();
}
	msg = (struct dfsdfs *) smalloc( sizeof(struct dfsdfs));
	msg->nmi_path[0] = c;
	if ( c == '\0' ) {
if ( ddbug ) {
		printf( "rnamei null path name\n" );
		debug();
}
		msg->nmi_inode = dp->i_dfsi;
	} else {
		sdirp = u.u_dirp;
		msg->nmi_flag = flag;
		for ( i = 1; i < DFSMAXP; i++ )
			if ( !(msg->nmi_path[i] = (*func)()) )
				break;
if ( ddbug ) {
		printf( "rnamei got %s\n", msg->nmi_path );
		debug();
}
		msg->nmi_uid = u.u_uid;
		msg->nmi_gid = u.u_gid;
		if ( u.u_cdir && u.u_cdir->i_flag & IRMT && 
				(u.u_cdir->i_dfsi == dp->i_dfsi ) &&
				(u.u_cdir->i_dfsid == dp->i_dfsid ) )
			msg->nmi_cdir = u.u_cdir->i_dfsi;
		else msg->nmi_cdir = NULL;
	}
	dwe = gdfswe();
	dwe->wn_type = DWENMI;
	dwe->wn_msg = msg;
	spri = spl0();
	adddwe( dp->i_dfsid, dwe );
	sleep( (caddr_t)dwe, DFSPRI );
	splx( spri );
	rmsg = dwe->wn_rmsg;
if ( ddbug ) {
	printf( "rnamei return from work for dmntl %x\n", rmsg->nmi_rinode );
	debug();
}
	u.u_error = rmsg->nmi_error;
	if ( dwe->wn_stat != DFSOK ) {
		ip = NULL;
		goto	out;
	}
	if ( c != '\0' ) {
		if ( !(rmsg->nmi_done) ) {
if ( ddbug ) {
			printf( "rnamei not done %x, %x\n", dp,
					rmsg->nmi_pcnt );
			debug();
}
			liput( dp );
			sdirp--;
			sdirp += rmsg->nmi_pcnt;
			u.u_dirp = sdirp;
			ip = (struct inode *)-1;
			goto out;
		}
		u.u_offset = rmsg->nmi_offset;
		for ( i = 0; i < DIRSIZ; i++ )
			u.u_dent.d_name[i] = rmsg->nmi_dname[i];
		u.u_dent.d_ino = rmsg->nmi_dino;
		id = dp->i_dfsid;
		if ( u.u_error || (flag == 0) || (flag == 1 ) ||
		     ((flag == 2) && (rmsg->nmi_rinode != NULL) &&
				(dp->i_dfsi != rmsg->nmi_rinode)) )  {
			liput( dp );
		}
		if ( rmsg->nmi_rinode != NULL ) {
			ip = riget(rmsg->nmi_dpro, rmsg->nmi_rinode, ILOCK );
			if ( ip != NULL ) {
				ip->i_uid = rmsg->nmi_uid;
				ip->i_gid = rmsg->nmi_gid;
				ip->i_flag |= rmsg->nmi_iflag;
				ip->i_dev |= rmsg->nmi_fflag;
				ip->i_mode = rmsg->nmi_mode;
			}
		} else ip = NULL;
if ( ddbug ) {
		printf( "rtn from riget %x\n", ip );
		debug();
}
		if ( rmsg->nmi_pdir )
			u.u_pdir = riget( id, rmsg->nmi_pdir, ILOCK);
		else u.u_pdir = NULL;
	} else {
		ip = dp;
	}
	out:
	smfree( sizeof(struct dfsdfs), msg );
	smfree( sizeof(struct dfsdfs), rmsg );
	rdfswe( dwe );
	return( ip );
}

/*
 * The remote version of namei.
 */

rrnamei( vc, m )
struct	dfsdfs	*m;
{
	struct	inode *ip;
	int	schar(), i;

	u.u_dirp = m->nmi_path;
if ( ddbug ) {
	if ( m->nmi_path[0] != '\0' )
		printf( "rrnamei ready to call namei %s\n", u.u_dirp );
	else printf( "rrnamei null path name, iget only\n" );
	debug();
}
	if ( m->nmi_path[0] == '\0' ) {
		ip = m->nmi_inode;
		iget( ip->i_dev, ip->i_number );
		m->nmi_rinode = NULL;
	} else {
		u.u_pdir = NULL;
		if ( m->nmi_cdir != NULL ) {
			u.u_cdir = m->nmi_cdir;
		} else {
			u.u_cdir = u.u_rdir;
		}
		ip = namei( schar, m->nmi_flag, 1 );
if ( ddbug ) {
		printf( "namei returned, %x, %x\n", ip, u.u_error );
}
		if ( u.u_error == -1 ) {
if (ddbug ) {
			printf( "rrnamei not done %x, %x\n", ip, u.u_dirp);
			debug();
}
			m->nmi_done = NULL;
			m->nmi_pcnt = u.u_dirp - m->nmi_path - 1;
			u.u_error = 0;
			iput( ip );
		} else m->nmi_done = 1;
		m->nmi_rinode = ip;
		if ( ip != NULL ) {
			m->nmi_uid = ip->i_uid;
			m->nmi_gid = ip->i_gid;
			m->nmi_iflag = ip->i_flag & ITEXT;
			m->nmi_mode = ip->i_mode;
			m->nmi_fflag = 0;
		}
		m->nmi_pdir = u.u_pdir;
		m->nmi_offset = u.u_offset;
		m->nmi_dino = u.u_dent.d_ino;
		for ( i = 0; i < DIRSIZ; i++ )
			m->nmi_dname[i] = u.u_dent.d_name[i];
	}
	m->nmi_error = u.u_error;
	nwrite( vc, m, sizeof( struct dfsdfs ) );
}

#endif
