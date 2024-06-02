#ifdef	PNETDFS
#include "sys/param.h"
#include "sys/systm.h"
#include "sys/dir.h"
#include "sys/user.h"
#include "sys/file.h"
#include "sys/inode.h"
#include "sys/stat.h"
#include "sys/proc.h"
#include "sys/pnetdfs.h"
#include "sys/mdpncf.h"
#include "sys/acct.h"
#include "sys/buf.h"
#include "sys/ino.h"

extern	ushort	ddbug;
/*
 * Internal form of close for remote files.
 * Send message to remote node for close.
 */
rclosef(fp)
register struct file *fp;
{

	register struct	dfsdfs *msg;
	register struct inode	*ip;
	register	i;

	if(fp == NULL)
		return;
	for ( i = 0; i < NOFILE; i++ )
		if ( fp == u.u_ofile[i] )
			break;
	if ( i == NOFILE ) {
		printf("rclosef bad fp %x\n", fp );
		debug();
		return;
	}
	msg = (struct dfsdfs *)smalloc(sizeof(struct dfsdfs));
	msg->cl_type = DFSCLSRQ;
	msg->cl_fp = u.u_rofile[i];
	nwrite( u.u_dfsvc, msg, sizeof(struct dfsdfs) );
	nread( u.u_dfsvc, msg, sizeof(struct dfsdfs) );
	if ( msg->cl_type != DFSCLSRQ ) {
		printf( "rclosef bad net message %x\n", msg );
		debug();
	}
	smfree( sizeof(struct dfsdfs), msg );
	if ( (unsigned) fp->f_count > 1 ) {
		fp->f_count--;
		return;
	}
if ( ddbug ) {
	printf( "Last close of remote file %x\n", fp );
	debug();
}
	ip = fp->f_inode;
	while(ip->i_flag&ILOCK) {
		ip->i_flag |= IWANT;
		sleep((caddr_t)ip, PINOD);
	}
	ip->i_flag |= ILOCK;
	if( ip->i_count == 1 ) {
		ip->i_flag = 0;
		ip->i_dev = 0;
		ip->i_number = 0;
		ip->i_count = 0;
		ip->i_dfsid = 0;
		ip->i_dfsi = 0;
	} else ip->i_count--;
	if ( ip->i_count < 0 ) {
		printf( "rclosef i_count < 0\n" );
		debug();
		ip->i_count = 0;
	}
	ip->i_flag &= ~ILOCK;
	fp->f_count = 0;
}

/*
 * ropeni - issue openi to remote system.
 */
ropeni(ip, flag)
register struct inode *ip;
{
	register struct dfswe *dwe;
	register struct dfsdfs *msg;
	struct ncfmsg	nmsg;
	struct	dfswe	*gdfswe();
	char	rname[9];
	int	spri, i, nvc;

if ( ddbug ) {
	printf( "ropeni called %x\n", ip );
	debug();
}
	if ( !u.u_dfsvc ) {
if ( ddbug ) {
		printf( "ropeni getting sister\n" );
		debug();
}
		gdfsname( rname );
		nvc = nattach( rname );
		dwe = gdfswe();
		dwe->ws_type = DWESIS;
		for ( i = 0; i < 9; i++ )
			if ( !(dwe->ws_rname[i] = rname[i]) )
				break;
		spri = spl0();
		adddwe( ip->i_dfsid, dwe );
		sleep( (caddr_t)dwe, DFSPRI );
		splx(spri);
		i = nread( nvc, &nmsg, sizeof(struct ncfmsg) );
		if ( i == 0 ) {
			printf( "ropeni got 0 nvc read\n" );
			debug();
		}
		if ( nmsg.ncf_type != NCFORQ ) {
			printf( "ropeni got not NCFORQ\n" );
			debug();
		}
		nmsg.ncf_stat = DFSOK;
		u.u_dfsvc = nmsg.ncf_lvc;
if ( ddbug ) {
		printf( "ropeni got connect %x\n", u.u_dfsvc );
		debug();
}
		nwrite( nvc, &nmsg, sizeof(struct ncfmsg) );
		rdfswe( dwe );
	}
	msg = (struct dfsdfs *)smalloc(sizeof( struct dfsdfs ));
	msg->op_inode = ip->i_dfsi;
	msg->op_flag = flag;
	msg->op_type = DFSOPNRQ;
	nwrite( u.u_dfsvc, msg, sizeof(struct dfsdfs) );
	nread( u.u_dfsvc, msg, sizeof(struct dfsdfs) );
	u.u_error = msg->op_error;
	u.u_rofile[u.u_rval1] = msg->op_fp;
	smfree( sizeof(struct dfsdfs), msg );

}

/*
 * Rchmod - issue remote chmod.
 */

rchmod( ip, mode )
register struct inode *ip;
{
	register struct dfswe *dwe;
	struct	dfswe	*gdfswe();
	int	spri;

if ( ddbug ) {
	printf( "rchmod called %x\n", ip );
	debug();
}
	dwe = gdfswe();
	dwe->wc_type = DWECHMOD;
	dwe->wc_ip = ip->i_dfsi;
	dwe->wc_mode = mode;
	spri = spl0();
	adddwe( ip->i_dfsid, dwe );
	sleep( (caddr_t)dwe, DFSPRI );
	splx( spri );
	ip->i_mode |= mode&07777;
	rdfswe( dwe );
	iput(ip);
}

/*
 * Rrchmod - The remote version of chmod.
 */

rrchmod( vc, m )
register struct dfsdfs	*m;
{
	register struct inode	*ip;

	ip = m->cm_ip;
if ( ddbug ) {
	printf( "rrchmod called %x, %x\n", m, ip );
	debug();
}
	ip->i_mode &= ~07777;
	ip->i_mode |= m->cm_mode&07777;
	ip->i_flag |= ICHG;
	if ( ip->i_flag&ITEXT && (ip->i_mode&ISVTX)==0 )
		xrele( ip );
	m->cm_error = u.u_error;
	nwrite( vc, m, sizeof(struct dfsdfs) );
}

/*
 * Rchown - issue remote chown.
 */

rchown( ip, uid, gid )
register struct inode *ip;
{
	register struct dfswe *dwe;
	struct	dfswe	*gdfswe();
	int	spri;

if ( ddbug ) {
	printf( "rchown called %x\n", ip );
	debug();
}
	dwe = gdfswe();
	dwe->wco_type = DWECHOWN;
	dwe->wco_ip = ip->i_dfsi;
	dwe->wco_uid = uid;
	dwe->wco_uuid = u.u_uid;
	dwe->wco_gid = gid;
	spri = spl0();
	adddwe( ip->i_dfsid, dwe );
	sleep( (caddr_t)dwe, DFSPRI );
	splx( spri );
	ip->i_uid = dwe->wco_uid;
	ip->i_gid = dwe->wco_gid;
	ip->i_mode = dwe->wco_mode;
	rdfswe( dwe );
	iput(ip);
}

/*
 * Rrchown - The remote version of chown.
 */

rrchown( vc, m )
register struct dfsdfs	*m;
{
	register struct inode	*ip;

	ip = m->co_ip;
if ( ddbug ) {
	printf( "rrchown called %x, %x\n", m, ip );
	debug();
}
	ip->i_uid = m->co_uid;
	ip->i_gid = m->co_gid;
	if ( m->co_uuid != 0 )
		ip->i_mode &= ~(ISUID | ISGID);
	m->co_imode = ip->i_mode;
	ip->i_flag |= ICHG;
	m->co_error = u.u_error;
	nwrite( vc, m, sizeof(struct dfsdfs) );
}
/* 
 * Rstat1 - issue remote file stat1 request.
 */

rstat1( ip, ub )
register struct inode *ip;
struct stat	*ub;
{
	register struct dfswe *dwe;
	register struct dfsdfs *m;
	struct	dfswe	*gdfswe();
	int	spri;

if ( ddbug ) {
	printf( "rstat1 called %x\n", ip );
	debug();
}
	dwe = gdfswe();
	dwe->ws1_type = DWESTAT1;
	dwe->ws1_ip = ip->i_dfsi;
	dwe->ws1_ub = ub;
	spri = spl0();
	adddwe( ip->i_dfsid, dwe );
	sleep( (caddr_t)dwe, DFSPRI );
	splx( spri );
	m = dwe->ws1_rmsg;
	if ( (ip->i_flag & IRMT) && (ip->i_flag & IMOUNT) ) {
if ( ddbug ) {
		printf( "stat1 on mounted inode %x\n", ip );
		debug();
}
		m->s1_statb.st_dev = ip->i_dev;
		m->s1_statb.st_ino = ip->i_number;
	}
	if (copyout((caddr_t)&m->s1_statb,(caddr_t)ub,sizeof(struct stat))
		< 0 )
		u.u_error = EFAULT;
	smfree( sizeof(struct dfsdfs), m );
	rdfswe( dwe );
}

/*
 * Rrstat1 - remote file version of stat1.
 */

rrstat1( vc, m )
register struct dfsdfs *m;
{
	register struct inode	*ip;
	register struct buf	*bp;

	ip = m->cm_ip;
if ( ddbug ) {
	printf( "rrstat1 called %x, %x\n", m, ip );
	debug();
}
	m->s1_dev = ip->i_dev;
	m->s1_ino = ip->i_number;
	m->s1_mode = ip->i_mode;
	m->s1_nlink = ip->i_nlink;
	m->s1_uid = ip->i_uid;
	m->s1_gid = ip->i_gid;
	m->s1_rdev = (dev_t)ip->i_rdev;
	m->s1_size = ip->i_size;
	/*
	 * next the dates in the disk
	 */
	bp = bread(ip->i_dev, itod(ip->i_number));
	copyio(paddr(bp)+itoo(ip->i_number)*sizeof(struct dinode)
		+(long)(&((struct dinode *)0)->di_atime),
	    &m->s1_atime, 3*sizeof(time_t), U_RKD);
	brelse(bp);
	m->s1_error = u.u_error;
	nwrite( vc, m, sizeof(struct dfsdfs) );
}

/*
 * Ricount - keep i_count consistent for remote files.
 */

ricount( ip )
register struct inode	*ip;
{
	register struct dfswe *dwe;
	struct	dfswe	*gdfswe();
	int	spri;

if ( ddbug ) {
	printf( "ricount called %x\n", ip );
	debug();
}
	dwe = gdfswe();
	dwe->we_type = DWEIC;
	dwe->wic_ip = ip->i_dfsi;
	spri = spl0();
	adddwe( ip->i_dfsid, dwe );
	sleep( (caddr_t)dwe, DFSPRI );
	splx( spri );
	rdfswe( dwe );
	ip->i_count++;
}

/*
 * Rfcntl - issue remote fcntl system call.
 */

rfcntl( fdo, fdn, cmd, arg )
{
	register struct dfsdfs *m;

if ( ddbug ) {
	printf( "rfcntl called %x, %x, %x\n", fdo, fdn, cmd );
	debug();
}
	m  = (struct dfsdfs *)smalloc(sizeof(struct dfsdfs));
	m->fc_type = DFSFCRQ;
	m->fc_fp = u.u_rofile[fdo];
	m->fc_cmd = cmd;
	m->fc_arg = arg;
	nwrite( u.u_dfsvc, m, sizeof(struct dfsdfs) );
	nread( u.u_dfsvc, m, sizeof(struct dfsdfs) );
	u.u_error = m->fc_error;
	u.u_rofile[fdn] = u.u_rofile[fdo];
}

/* 
 * Rrfcntl - remote version of fcntl for dup and setfl.
 */

rrfcntl( m )
register struct dfsdfs *m;
{
	switch ( m->fc_cmd ) {

	case	0:
		m->fc_fp->f_count++;
		m->fc_error = NULL;
		break;

	case	4:
		m->fc_fp->f_flag &= (FREAD|FWRITE);
		m->fc_fp->f_flag |= (m->fc_arg-FOPEN) & 
				    ~(FREAD|FWRITE);
		m->fc_error = NULL;
		break;
	
	default:
		m->fc_error = EINVAL;

	}

}

/*
 * Rioctl - issue remote file ioctl.
 */

rioctl( fd, cmd, arg )
{
	register struct dfsdfs *m;

	printf( "remote ioctl, not there\n" );
	debug();
	/*
	m = (struct dfsdfs *)smalloc( sizeof(struct dfsdfs) );
	m->ioc_type = DFSIOCRQ;
	m->ioc_erro = NULL;
	m->ioc_fp = u.u_rofile[fd];
	m->ioc_cmd = cmd;
	m->ioc_arg = arg;
	nwrite( u.u_dfsvc, m, sizeof(struct dfsdfs) );
	nread( u.u_dfsvc, m, sizeof(struct dfsdfs) );
	u.u_error = m->fc_error;
	*/


}

/*
 * Rrioctl - remote version of ioctl.
 */

rrioctl( m )
register struct dfsdfs	*m;
{
	register dev_t dev;

	/*
	ip = m->ioc_fp->f_inode;
	dev = (dev_t)m->ioc_fp->f_inode->i_rdev;
	(*cdevsw[major(dev)].d_ioctl)(minor(dev),m->ioc_cmd,
				      m->ioc_arg, m->ioc_fp->f_flag );
	m->ioc_error = u.u_error;
	switch ( m->ioc_cmd ) {


	}
	*/
}

/*
 * Rseek - issue remote seek.
 */

rseek( fd, off, base )
off_t	off;
{
	register struct dfsdfs *m;

	m = (struct dfsdfs *)smalloc( sizeof(struct dfsdfs) );
	m->sk_type = DFSSKRQ;
	m->sk_error = NULL;
	m->sk_fp = u.u_rofile[fd];
	m->sk_off = off;
	m->sk_base = base;
	m->sk_sig = 0;
	nwrite( u.u_dfsvc, m, sizeof(struct dfsdfs) );
	nread( u.u_dfsvc, m, sizeof(struct dfsdfs) );
	u.u_error = m->fc_error;
	if ( m->sk_sig )
		psignal( u.u_procp, SIGSYS );
	if ( u.u_error )
		return;
	u.u_roff = m->sk_off;
}

/*
 * Rrseek - perform remote file seek.
 */

rrseek( m )
register struct dfsdfs *m;
{
	switch ( m->sk_base ) {

	case	1:

		m->sk_off += m->sk_fp->f_offset;
		break;

	case	2:

		m->sk_off += m->sk_fp->f_inode->i_size;
		break;

	case	0:
		break;

	default:
		m->sk_error = EINVAL;
		m->sk_sig++;
		return;
	}
	if ( m->sk_off < 0 ) {
		m->sk_error = EINVAL;
		return;
	}
	m->sk_fp->f_offset = m->sk_off;

}

/*
 * Rutime - issue remote utime.
 */

rutime( ip, ta, tm )
register struct inode *ip;
register time_t	*ta, *tm;
{
	register struct dfswe *dwe;
	struct	dfswe	*gdfswe();
	int	spri;

if ( ddbug ) {
	printf( "rutime called %x\n", ip );
	debug();
}
	dwe = gdfswe();
	dwe->wu_type = DWEUT;
	dwe->wu_ip = ip->i_dfsi;
	dwe->wu_tacc = *ta;
	dwe->wu_tmod = *tm;
	spri = spl0();
	adddwe( ip->i_dfsid, dwe );
	sleep( (caddr_t)dwe, DFSPRI );
	splx( spri );
	rdfswe( dwe );
	iput(ip);

}

/*
 * Rrutime - perform remote utime.
 */

rrutime( m )
register struct dfsdfs *m;
{
	m->ut_ip->i_flag |= IACC|IUPD|ICHG;
	iupdat( m->ut_ip, &m->ut_tacc, &m->ut_tmod );
	m->ut_error = u.u_error;
}
gdfsname( cs )
register char	*cs;
{
	cs[0] = 'U';
	cs[1] = (char)(((int)(u.u_procp) >> 12) & 0xf) + '0';
	cs[2] = (char)(((int)(u.u_procp) >> 8) & 0xf) + '0';
	cs[3] = (char)(((int)(u.u_procp) >> 4) & 0xf) + '0';
	cs[4] = (char)((u.u_procp->p_pid >> 12) & 0xf) + '0';
	cs[5] = (char)((u.u_procp->p_pid >> 8) & 0xf) + '0';
	cs[6] = (char)((u.u_procp->p_pid >> 4) & 0xf) + '0';
	cs[7] = (char)((u.u_procp->p_pid >> 0) & 0xf) + '0';
	cs[8] = '\0';
if ( ddbug ) {
	printf( "dfsname %s\n", cs );
}
}
#endif
