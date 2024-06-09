#ifdef	PNETDFS
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

extern	ushort	ddbug;
/*
 * riget - establish a local inode for a romote file.
 */

struct inode *
riget( id, sip, lock )
register struct inode *sip;
register int	*id;
{
	register i;
	register struct inode *ip;
	register struct inode *oip;

loop:
	oip = NULL;
	for ( ip = &inode[0]; ip < &inode[v.v_inode]; ip++ ) {
		if ( id == ip->i_dfsid && sip == ip->i_dfsi ) {
			goto found;
		}
		if(oip==NULL && ip->i_count==0) {
			oip = ip;
		}
	}
	ip = oip;
	if(ip == NULL) {
		printf("Inode table overflow\n");
		u.u_error = ENFILE;
		return(NULL);
	}
	if ( !lock ) {
		printf( "riget new w/o lock\n" );
		debug();
	}
	ip->i_flag = IRMT | lock;
	if ( lock )
		ip->i_count = 1;
	ip->i_dev = DFSMAJ << 8;
	ip->i_number = 0;
	ip->i_dfsid = id;
	ip->i_dfsi = sip;
	ip->i_dddev = ((struct dqp *)id)->qp_dddev;
	ip->i_ddino = ((struct dqp *)id)->qp_ddino;
	return( ip );
found:
	if( lock && (ip->i_flag&ILOCK) != 0) {
		ip->i_flag |= IWANT;
		sleep((caddr_t)ip, PINOD);
		goto loop;
	}
	ip->i_flag |= lock;
	if ( lock )
		ip->i_count++;
	return(ip);
}
/*
 * riput - issue iput to remote system inode.
 */
riput(ip)
register struct inode *ip;
{
	register struct dfsdfs *msg;
	register struct dfswe *dwe;
	struct	dfswe	*gdfswe();
	int	spri;

if ( ddbug ) {
	printf( "riput called %x\n", ip );
	debug();
}
	dwe = gdfswe();
	dwe->wi_ip = ip->i_dfsi;
	dwe->we_type = DWEIP;
	spri = spl0();
	adddwe( ip->i_dfsid, dwe );
	sleep( (caddr_t)dwe, DFSPRI );
	splx( spri );
	rdfswe( dwe );
	if( ip->i_count == 1 ) {
		ip->i_flag = 0;
		ip->i_number = 0;
		ip->i_count = 0;
		ip->i_dfsid = 0;
		ip->i_dfsi = 0;
		return;
	}
	ip->i_count--;
#ifdef	PNETDFS
	if ( ip->i_count < 0 ) {
		printf( "riput i_count < 0\n" );
		debug();
		ip->i_count = 0;
	}
#endif
	lprele( ip );
}

/* 
 * liput - perform local iput on remote node.
 */

liput(ip)
register struct inode *ip;
{

	if(ip->i_count == 1) {
		ip->i_flag = 0;
		ip->i_number = 0;
		ip->i_count = 0;
		ip->i_dfsi = 0;
		ip->i_dfsid = 0;
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
	lprele(ip);
}
/*
 * rprele - remote version of prele.
 */

rprele( ip )
register struct inode *ip;
{
	register struct dfswe *dwe;
	struct	dfswe	*gdfswe();
	int	spri;

if ( ddbug ) {
	printf( "rprele called %x\n", ip );
	debug();
}
	dwe = gdfswe();
	dwe->wp_ip = ip->i_dfsi;
	dwe->we_type = DWEPRE;
	spri = spl0();
	adddwe( ip->i_dfsid, dwe );
	sleep( (caddr_t)dwe, DFSPRI );
	splx( spri );
	rdfswe( dwe );

}

/*
 * lprele - local prele on remote file.
 */

lprele(ip)
register struct	inode	*ip;
{
	ip->i_flag &= ~ILOCK;
	if ( ip->i_flag & IWANT ) {
		ip->i_flag &= ~IWANT;
		wakeup( (caddr_t)ip );
	}
}

/*
 * rplock - issue remote plock.
 */

rplock( ip )
register struct inode *ip;
{
	register struct dfswe *dwe;
	struct	dfswe	*gdfswe();
	int	spri;

if ( ddbug ) {
	printf( "plock called %x\n", ip );
	debug();
}
	dwe = gdfswe();
	dwe->wpl_ip = ip->i_dfsi;
	dwe->we_type = DWEPLCK;
	spri = spl0();
	adddwe( ip->i_dfsid, dwe );
	sleep( (caddr_t)dwe, DFSPRI );
	splx( spri );
	rdfswe( dwe );

}

/*
 * Ritrunc - issue remote itrunc.
 */

ritrunc( ip )
register struct inode *ip;
{

	register struct dfswe *dwe;
	struct	dfswe	*gdfswe();
	int	spri;

if ( ddbug ) {
	printf( "ritrunc called %x\n", ip );
	debug();
}
	dwe = gdfswe();
	dwe->we_type = DWEIT;
	dwe->wit_ip = ip->i_dfsi;
	spri = spl0();
	adddwe( ip->i_dfsid, dwe );
	sleep( (caddr_t)dwe, DFSPRI );
	splx( spri );
	rdfswe( dwe );

}

/*
 * Make a new file on remote system..
 */
struct inode *
rmaknode(mode, dev )
{
	register struct inode *ip;
	register struct dfsdfs *msg;
	register struct dfswe *dwe;
	struct	dfswe	*gdfswe();
	int	spri, i;

if ( ddbug ) {
	printf( "rmaknode called %x\n", mode );
	debug();
}
	dwe = gdfswe();
	msg = (struct dfsdfs *)smalloc( sizeof( struct dfsdfs) );
	dwe->we_type = DWEMNODE;
	dwe->wmn_msg = msg;
	for ( i = 0; i < DIRSIZ; i++ )
		msg->mkn_dname[i] = u.u_dent.d_name[i];
	msg->mkn_dino = u.u_dent.d_ino;
	msg->mkn_offset = u.u_offset;
	msg->mkn_ip = u.u_pdir->i_dfsi;
	msg->mkn_mode = mode;
	msg->mkn_cmask = u.u_cmask;
	msg->mkn_uid = u.u_uid;
	msg->mkn_gid = u.u_gid;
	msg->mkn_rdev = dev;
	spri = spl0();
	adddwe( u.u_pdir->i_dfsid, dwe );
	sleep( (caddr_t)dwe, DFSPRI );
	splx( spri );
	smfree( sizeof( struct dfsdfs), msg );
	ip = dwe->wmn_ip;
	liput( u.u_pdir );
	u.u_error = dwe->we_error;
	rdfswe( dwe );
	return( ip );
}
/*
 * Rrmaknode - the remote version of maknode.
 */

rrmaknode( vc, m )
struct dfsdfs	*m;
{
	register ushort	i;
	register struct inode *ip;

	u.u_pdir = m->mkn_ip;
	u.u_offset = m->mkn_offset;
	u.u_dent.d_ino = m->mkn_dino;
	for ( i = 0; i < DIRSIZ; i++ )
		u.u_dent.d_name[i] = m->mkn_dname[i];
	u.u_cmask = m->mkn_cmask;
	u.u_uid = m->mkn_uid;
	u.u_gid = m->mkn_gid;
	ip = maknode( m->mkn_mode );
	m->mkn_rip = ip;
	m->mkn_error = u.u_error;
	if ( ip != NULL ) {
		m->mkn_uid = ip->i_uid;
		m->mkn_gid = ip->i_gid;
		m->mkn_iflag = ip->i_flag & ITEXT;
		m->mkn_imode = ip->i_mode;
		m->mkn_fflag = 0;
		switch ( ip->i_mode & IFMT ) {
		case IFCHR:
		case IFBLK:
			ip->i_rdev = m->mkn_rdev;
			ip->i_flag |= ICHG;
		}
	}
	nwrite( vc, m, sizeof(struct dfsdfs));

}

/*
 * Write a directory entry with parameters left as side effects
 * to a call to namei.
 */
rwdir(ip)
struct inode *ip;
{

/*
	u.u_dent.d_ino = ip->i_number;
	u.u_count = sizeof(struct direct);
	u.u_segflg = 1;
	u.u_base = (caddr_t)&u.u_dent;
	writei(u.u_pdir);
	iput(u.u_pdir);
*/
}
#endif
