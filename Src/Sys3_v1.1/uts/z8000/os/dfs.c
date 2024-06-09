/*
 * The Distributed File System Process.
 */
#ifdef	PNETDFS
#include	"sys/plexus.h"
#include	"sys/param.h"
#include 	"sys/systm.h"
#include	"sys/dir.h"
#include	"sys/user.h"
#include	"sys/inode.h"
#include	"sys/buf.h"
#include	"sys/proc.h"
#include	"sys/file.h"
#include	"sys/map.h"
#include	"sys/var.h"
#include 	"sys/utsname.h"
#include	"sys/stat.h"
#include	"sys/pnetdfs.h"
#include	"sys/mount.h"
#include	"sys/mdpncf.h"

ushort	ddbug = 1;

#define	MAXNCFMSG	512	/* maximum size of message from NCF */

dfsl()
{
	register struct dfswe	*dwe;
	register struct	dqp	*dpro;
	struct	dqp	*gdprocess();
	register	i;
	struct	inode	*scdir, *srdir;
	int	spri;
	struct	dfswe	*gqent();

	scdir = u.u_cdir;
	srdir = u.u_rdir;
	for ( i = 0; i < NDFSPRO; i++ ) 
		if ( dqp[i].qp_name[0] == '\0' )
			break;
	if ( i == NDFSPRO ) {
		printf( "No dfs processes left dsfl\n" );
		debug();
	}
	printf("Distributed File System (Local) Activated\n");
	dqp[i].qp_name[0] = u.u_comm[0] = 'D';
	dqp[i].qp_name[1] = u.u_comm[1] = 'F';
	dqp[i].qp_name[2] = u.u_comm[2] = 'S';
	dqp[i].qp_name[3] = u.u_comm[3] = 'L';
	dqp[i].qp_name[4] = u.u_comm[4] = '\0';
	while (1) {
		dwe = gqent( &dqp[i] );
		/*
		printf( "dfsl got dwe = %x\n", dwe );
		*/
		switch ( dwe->we_type ) {

		default:
			printf( "Illegal dfs work entry\n" );
			break;

		case DWEMNT:
			/*
			printf( "dfsl mount request\n" );
			*/
			dpro = gdprocess('M');
			if ( dpro == (struct dqp *)-1 ) { 
				dwe->we_stat = DNOPRO;
				wakeup( (caddr_t)dwe );
				break;
			}
			dpro->qp_flink = dpro->qp_llink = dwe;
			if ( sysproc() ) {
				for ( i = 0; i < 9; i++ )
					u.u_comm[i] = dpro->qp_name[i];
				dmntl( dpro );
				badfrk();
			}
			break;
		}
	}
}


dfsr()
{
	register	struct	dqp	*dpro;
	int	i, n, nvc, spri;
	struct	ncfmsg	rbuf;
	struct	dfsmnt	*mbuf;
	struct	inode	*scdir, *srdir;

	scdir = u.u_cdir;
	srdir = u.u_rdir;
	for ( i = 0; i < NDFSPRO; i++ ) 
		if ( dqp[i].qp_name[0] == '\0' )
			break;
	if ( i == NDFSPRO ) {
		printf( "No dfs processes left dsfr\n" );
		debug();
	}
	printf("Distributed File System (Remote) Activated\n");
	dqp[i].qp_name[0] = u.u_comm[0] = 'D';
	dqp[i].qp_name[1] = u.u_comm[1] = 'F';
	dqp[i].qp_name[2] = u.u_comm[2] = 'S';
	dqp[i].qp_name[3] = u.u_comm[3] = 'R';
	dqp[i].qp_name[4] = u.u_comm[4] = '\0';
	nvc = nattach( "DFSR" );
	/*
	printf( "dfsr nattached = %x\n", nvc );
	*/
	while (1) {
		n = nread( nvc, &rbuf, sizeof(rbuf) );
		u.u_error = NULL;
		if ( n != 0 ) {
			/*
			printf( "dfsr msg from ncf\n" );
			*/
			switch ( rbuf.ncf_type ) {

			case	NCFORQ:
				/*
				printf( "dfsr open req\n" );
				*/
				rbuf.ncf_stat = DFSOK;
				nwrite( nvc, &rbuf, sizeof( rbuf ) );
				break;

			case	NCFCRQ:
				/*
				printf( "dfsr close req\n" );
				*/
				rbuf.ncf_stat = DFSOK;
				nwrite( nvc, &rbuf, sizeof( rbuf ) );
				break;
			default:
				printf( "Illegal read from NCF\n" );
				break;


			}
		} else {
			/*
			printf( "dfsr data some conn\n" );
			*/
			while ( (i = nioctl( nvc, NCFDATA )) != -1 ) {
				/*
				printf( "dfsr data on %x\n", i );
				*/
				mbuf = ( struct dfsmnt *)
						smalloc(sizeof(struct dfsmnt));
				nread( i, mbuf, sizeof(struct dfsmnt) );
				u.u_error = NULL;
				switch (  mbuf->dm_type ) {

				case DFSMNTRQ:
					/*
					printf( "dfsr got rmount request\n" );
					*/
					dpro = gdprocess('M');
					if ( sysproc() ) {
						for (i=0; i<9; i++)
						  	u.u_comm[i] =
							   dpro->qp_name[i];
						rdmntr( dpro, mbuf );
						badfrk();
					}
					break;

				default:
					printf("Illegal Net msg\n" );
					break;
				}
			}
		}
	}
}


/*
 *  Get a dfs process table entry.
 */
struct	dqp *
gdprocess( fchar )
char	fchar;
{
	register i;
	for ( i = 0; i < NDFSPRO; i++ ) 
		if ( dqp[i].qp_name[0] == '\0' )
			break;
	if ( i == NDFSPRO )
		return( (struct dqp *)-1 );
	else {
		dqp[i].qp_name[0] = fchar;
		dqp[i].qp_name[1] = 'D';
		dqp[i].qp_name[2] = 'F';
		dqp[i].qp_name[3] = 'S';
		dqp[i].qp_name[4] = '0' + i;
		dqp[i].qp_name[5] = '\0';
		return ( (struct dqp *)&dqp[i] );
	}
}

/*
 *  Return a dfs process table entry.
 */
rdprocess( dpro )
register struct	dqp	*dpro;
{
	register struct dfswe	*dwe;

	dpro->qp_name[0] = '\0';
	for ( dwe = dpro->qp_flink; dwe; dwe = dwe->we_link ) {
		rdfswe( dwe );
	}
	dpro->qp_flink = dpro->qp_llink = NULL;

}


/*
 *  Get a dfs process table entry.
 */
struct	rmntab *
grmntab()
{
	register i;
	for ( i = 0; i < NRMOUNT; i++ ) 
		if ( rmntab[i].mt_rvc == 0 )
			break;
	if ( i == NRMOUNT )
		return( (struct rmntab *)0 );
	else {
		rmntab[i].mt_rvc = 0xff;
		return( (struct rmntab *)&rmntab[i] );
	}
}

/*
 *  Get an entry from the dfs work entry pool.
 */

struct	dfswe *
gdfswe()
{
	int	spri;
	struct	dfswe	*gwe;

	spri = spl7();
	while ( !dwefree ) 
		sleep( (caddr_t)&dwefree, DFSPRI );
	gwe = dwefree;
	dwefree = dwefree->we_link;
	splx( spri );
	gwe->we_link = 0;
	return( gwe );
}

/*
 *  Release a work entry.
 */

rdfswe( dwe )
struct dfswe	*dwe;
{
	int	spri;

	spri = spl7();
	dwe->we_link = dwefree;
	dwefree = dwe;
	splx( spri );
	wakeup( (caddr_t)&dwefree );
}

/*
 * Initialize the distributed file system.
 */

dfsinit()
{

	struct	dfswe	*dwe, *cdwe;

	dwefree = &dfswe[0];
	for ( cdwe = &dfswe[0], dwe = &dfswe[1]; cdwe < &dfswe[NDFSWE-1];
				cdwe++, dwe++ ) {
		cdwe->we_link = dwe;
	}
	dfswe[NDFSWE-1].we_link = (struct dfswe *)0;
}

/*
 *  Dsf mount request local worker process.
 */

dmntl( dpro )
struct	dqp	*dpro;
{
	register struct dfswe	*dwe;
	register struct dqp	*mdpro;
	register struct inode	*dp;
	register char	*cp;
	struct	buf	*bp;
	struct dfswe	*gqent();
	struct ncfmsg	nmsg;
	register	wvc;
	int	nvc, tvc, i, spri;
	struct	mount	*smp, *mp;
	struct	dfsdfs	*m;
	char	mflag;
	dev_t	d;
	struct	inode	*scdir, *srdir;

	/*
	printf( "Dmount called and running\n" );
	*/
	scdir = u.u_cdir;
	srdir = u.u_rdir;
	dwe = gqent( dpro );
/*
 * We have to find the i-number and dev for '..' to use after branch
 * is mounted on.
 */
	u.u_dent.d_name[0] = '.';
	u.u_dent.d_name[1] = '.';
	u.u_dent.d_name[2] = '\0';
	dp = dwe->wm_ip;
	mflag = NULL;
	/*
	 * set up to search a directory
	 */
seloop:
	u.u_offset = 0;
	bp = NULL;
eloop:

	/*
	 * If at the end of the directory,
	 * the search failed. Can not mount here.
	 */

	if(u.u_offset >= dp->i_size) {
		if(bp != NULL)
			brelse(bp);
		u.u_error = ENOENT;
		goto out1;
	}

	/*
	 * If offset is on a block boundary,
	 * read the next directory block.
	 * Release previous if it exists.
	 */

	if((u.u_offset&BMASK) == 0) {
		daddr_t bn;

		if(bp != NULL)
			brelse(bp);
		bn = bmap(dp, (daddr_t)(u.u_offset>>BSHIFT), B_READ);
		if (u.u_error)
			goto out1;
		if (bn < 0) {
			u.u_error = EIO;
			goto out1;
		}
		bp = bread(dp->i_dev, bn);
		if (u.u_error) {
			brelse(bp);
			goto out1;
		}
	}
	cp = (char *)&u.u_t[0];
	copyio(paddr(bp)+((unsigned)u.u_offset&BMASK),
		cp, sizeof(struct direct), U_RKD);
	u.u_offset += sizeof(struct direct);
	u.u_dent.d_ino = ((struct direct *)cp)->d_ino;
	if(u.u_dent.d_ino == 0) {
		goto eloop;
	}
	cp = &((struct direct *)cp)->d_name[0];
	for(i=0; i<DIRSIZ; i++)
		if(*cp++ != u.u_dent.d_name[i])
			goto eloop;
/*
 * We found '..', now see if local mount effects it.
 */

	if(bp != NULL)
		brelse(bp);
	d = dp->i_dev;
	if(u.u_dent.d_ino == ROOTINO)
	if(dp->i_number == ROOTINO)
		for(i=1; i<v.v_mount; i++)
			if(mount[i].m_flags == MINUSE)
			if(mount[i].m_dev == d) {
				dp = mount[i].m_inodp;
				dp->i_count++;
				plock(dp);
				mflag++;
				goto seloop;
			}
	if ( mflag )
		iput(dp);
/*
 * Now we have the '..' i-number in u.u_dent.d_ino and the dev in d.
 */
	smp = NULL;
	for ( mp = &mount[0]; mp < (struct mount*)v.ve_mount; mp++ ) {
		if( mp->m_flags == MFREE )
			if( smp == NULL )
				smp = mp;
		if ( !(nstrncmp( mp->m_node, 
			((struct dfsmnt *)(dwe->wm_msg))->dm_node, 9 )) )
			if ( !(nstrncmp( mp->m_dir,
				((struct dfsmnt *)
				(dwe->wm_msg))->dm_node, DFSMAXP )) ) {
				printf( "Try to mount already mounted" );
				printf( " remote directory\n" );
				goto out1;
			}

	}
	mp = smp;
	if ( mp == NULL )  {
		printf( "dmntl no mount entries\n" );
		goto out1;
	}
	mp->m_flags = MINTER;
	for ( i = 0; i < 9; i++ )
		if ( !(mp->m_node[i] =
			((struct dfsmnt *)(dwe->wm_msg))->dm_node[i]) )
			break;
	for ( i = 0; i < DFSMAXP; i++ )
		if ( !(mp->m_dir[i] = 
			((struct dfsmnt *)(dwe->wm_msg))->dm_rdir[i]) )
			break;
	nvc = nattach( dpro->qp_name );
	tvc = nopen( nvc, ((struct dfsmnt *)(dwe->wm_msg))->dm_node, 
					"DFSR" );
	/*
	printf( "dmntl nopen = %x\n", tvc );
	*/
	for ( i = 0; i < 9; i++ )
		if ( !(((struct dfsmnt *)(dwe->wm_msg))->dm_rnode[i] = 
				utsname.nodename[i]) )
			break;
	for ( i = 0; i < 9; i++ )
		if ( !(((struct dfsmnt *)(dwe->wm_msg))->dm_pro[i] = 
				dpro->qp_name[i] ) )
			break;
	nwrite( tvc, dwe->wm_msg, sizeof(struct dfsmnt) );
	nread( nvc, &nmsg, sizeof(nmsg) );
	if ( nmsg.ncf_type != NCFORQ ) {
		printf( "Dmount ncf msg not open\n" );
		debug();
	}
	/*
	printf( "%s NCFORQ recv %x\n" , dpro->qp_name, nmsg.ncf_lvc );
	*/
	nmsg.ncf_stat = DFSOK;
	wvc = nmsg.ncf_lvc;
	nwrite( nvc, &nmsg, sizeof(nmsg) );
	nclose( tvc );
	nread( wvc, dwe->wm_msg, sizeof(struct dfsmnt) );
	if ( ((struct dfsmnt *)(dwe->wm_msg))->dm_stat != DFSOK ) {
		printf( "dmntl bad remote response\n" );
		debug();
	}
	/*
	printf( "dmntl got good response\n" );
	debug();
	*/
	mdpro = gdprocess( 'M' );
	if ( mdpro == (struct dqp *)-1 ) {
		printf( "dmntl could not get dprocess\n" );
		debug();
	}
	adddwe( mdpro, dwe );
	/*
	printf( "dmntl sent dwe to dmntr\n" );
	*/
	dwe->wm_dpro = dpro;
	if ( sysproc() ) {
		for ( i = 0; i < 9; i++ )
			u.u_comm[i] = mdpro->qp_name[i];
		dmntr( mdpro, dpro );
		badfrk();
	}
	/*
	debug();
	*/
	dwe = gqent( dpro );
	if ( ((struct dfsmnt *)(dwe->wm_msg))->dm_stat != DFSOK ) {
		printf( "dmntl bad dmntr response\n" );
		debug();
		goto out1;
	}
	/*
	printf( "dmntl successful rmount\n" );
	debug();
	*/
	mp->m_dpro = dpro;
	mp->m_flags = MREMOTE | MINUSE;
	mp->m_dev = dwe->wm_ip->i_dev;
	mp->m_inodp = dwe->wm_ip;
	dwe->wm_ip->i_dfsid = (int *)dpro;
	dwe->wm_ip->i_dfsi = dwe->wm_msg->dm_ip;
	dpro->qp_ddino = dwe->wm_ip->i_ddino = u.u_dent.d_ino;
	dpro->qp_dddev = dwe->wm_ip->i_dddev = d;
	wakeup( (caddr_t)dwe );
	while ( 1 ) {
		dwe = gqent( dpro );
		/*
		printf( "dmntl got dwe = %x\n", dwe );
		*/
		switch ( dwe->we_type ) {

		default:
			printf( "Illegal dfs work entry\n" );
			break;

		case DWEUMNT:
if ( ddbug ) {
			printf( "dmntl got umount\n" );
			debug();
}
			nclose( wvc );
			ndetach( nvc );
			rdprocess( dpro );
			rdfswe( dwe );
			u.u_cdir = scdir;
			u.u_rdir = srdir;
			sysexit();
			break;

		case DWENMI:
if ( ddbug ) {
			printf( "dmntl namei request\n" );
}
			m = (struct dfsdfs *)dwe->wn_msg;
			m->nmi_type = DFSNMIRQ;
			m->nmi_dwe = dwe;
			nwrite( wvc, m, sizeof(struct dfsdfs) );
			break;

		case DWEIP:
if ( ddbug ) {
			printf( "dmntl iput req\n" );
}
			m = (struct dfsdfs *)smalloc(sizeof(struct dfsdfs));
			m->ip_type = DFSIPRQ;
			m->ip_inode = dwe->wi_ip;
			m->ip_dwe = dwe;
			nwrite( wvc, m, sizeof(struct dfsdfs) );
			smfree( sizeof(struct dfsdfs), m );
			break;

		case DWEIC:
if ( ddbug ) {
			printf( "dmntl icount req\n" );
}
			m = (struct dfsdfs *)smalloc(sizeof(struct dfsdfs));
			m->cip_type = DFSICRQ;
			m->cip_inode = dwe->wic_ip;
			m->cip_dwe = dwe;
			nwrite( wvc, m, sizeof(struct dfsdfs) );
			smfree( sizeof(struct dfsdfs), m );
			break;

		case DWEPRE:
if ( ddbug ) {
			printf( "dmntl prele req\n" );
}
			m = (struct dfsdfs *)smalloc(sizeof(struct dfsdfs));
			m->ip_type = DFSPRERQ;
			m->ip_inode = dwe->wp_ip;
			m->ip_dwe = dwe;
			nwrite( wvc, m, sizeof(struct dfsdfs) );
			smfree( sizeof(struct dfsdfs), m );
			break;

		case DWEPLCK:
if ( ddbug ) {
			printf( "dmntl plock req\n" );
}
			m = (struct dfsdfs *)smalloc(sizeof(struct dfsdfs));
			m->ip_type = DFSPLCKRQ;
			m->ip_inode = dwe->wpl_ip;
			m->ip_dwe = dwe;
			nwrite( wvc, m, sizeof(struct dfsdfs) );
			smfree( sizeof(struct dfsdfs), m );
			break;

		case DWESIS:
if ( ddbug ) {
			printf( "dmntl sister req %s\n", dwe->ws_rname );
			debug();
}
			m = (struct dfsdfs *)smalloc(sizeof(struct dfsdfs));
			m->sis_type = DFSSISRQ;
			for ( i = 0; i < 9; i++ )
				if( !(m->sis_name[i] = dwe->ws_rname[i]) )
					break;
			for ( i = 0; i < 9; i++ )
				if( !(m->sis_node[i] = utsname.nodename[i]) )
					break;
			m->sis_dwe = dwe;
			nwrite( wvc, m, sizeof(struct dfsdfs) );
			smfree( sizeof(struct dfsdfs), m );
			break;
			
		case DWECHMOD:
if ( ddbug ) {
			printf( "dmntl chmod req\n" );
			debug();
}
			m = (struct dfsdfs *)smalloc(sizeof(struct dfsdfs));
			m->cm_type = DFSCMRQ;
			m->cm_ip = dwe->wc_ip;
			m->cm_mode = dwe->wc_mode;
			m->cm_dwe = dwe;
			nwrite( wvc, m, sizeof(struct dfsdfs) );
			smfree( sizeof(struct dfsdfs), m );
			break;
			
		case DWECHOWN:
if ( ddbug ) {
			printf( "dmntl chown req\n" );
			debug();
}
			m = (struct dfsdfs *)smalloc(sizeof(struct dfsdfs));
			m->co_type = DFSCORQ;
			m->co_ip = dwe->wco_ip;
			m->co_uid = dwe->wco_uid;
			m->co_gid = dwe->wco_gid;
			m->co_uuid = dwe->wco_uuid;
			m->co_dwe = dwe;
			nwrite( wvc, m, sizeof(struct dfsdfs) );
			smfree( sizeof(struct dfsdfs), m );
			break;
			
		case DWESTAT1:
if ( ddbug ) {
			printf( "dmntl stat1 req\n" );
			debug();
}
			m = (struct dfsdfs *)smalloc(sizeof(struct dfsdfs));
			m->s1_type = DFSS1RQ;
			m->s1_ip = dwe->ws1_ip;
			m->s1_dwe = dwe;
			nwrite( wvc, m, sizeof(struct dfsdfs) );
			smfree( sizeof(struct dfsdfs), m );
			break;

		case DWEMNODE:
if ( ddbug ) {
			printf( "dmntl maknode request\n" );
}
			m = (struct dfsdfs *)dwe->wmn_msg;
			m->mkn_type = DFSMKNRQ;
			m->mkn_dwe = dwe;
			nwrite( wvc, m, sizeof(struct dfsdfs) );
			break;

		case DWEIT:
if ( ddbug ) {
			printf( "dmntl itrunc req\n" );
}
			m = (struct dfsdfs *)smalloc(sizeof(struct dfsdfs));
			m->it_type = DFSITRQ;
			m->it_inode = dwe->wit_ip;
			m->it_dwe = dwe;
			nwrite( wvc, m, sizeof(struct dfsdfs) );
			smfree( sizeof(struct dfsdfs), m );
			break;

		case DWEUT:
if ( ddbug ) {
			printf( "dmntl utime req\n" );
			debug();
}
			m = (struct dfsdfs *)smalloc(sizeof(struct dfsdfs));
			m->ut_type = DFSUTRQ;
			m->ut_ip = dwe->wu_ip;
			m->ut_tacc = dwe->wu_tacc;
			m->ut_tmod = dwe->wu_tmod;
			m->ut_dwe = dwe;
			nwrite( wvc, m, sizeof(struct dfsdfs) );
			smfree( sizeof(struct dfsdfs), m );
			break;
			
		}
	}
	out1:
	printf( "Dmntl out1 hit\n" );
	debug();
}

/*
 *  Dsf mount request remote reader process.
 */

dmntr( dpro, mdpro )
struct	dqp	*dpro;
struct	dqp	*mdpro;
{
	register struct dfswe	*dwe;
	register struct dfsdfs	*mbuf;
	struct	inode	*ip, *scdir, *srdir, *riget();
	struct	ncfmsg	rbuf;
	register	nvc, tvc;
	register 	i;
	int	n;

	/*
	printf( "Dmntr called and running\n" );
	debug();
	*/
	scdir = u.u_cdir;
	srdir = u.u_rdir;
	dwe = gqent( dpro );
	nvc = nattach( dpro->qp_name );
	tvc = nopen( nvc, ((struct dfsmnt *)(dwe->wm_msg))->dm_node, 
			((struct dfsmnt *)(dwe->wm_msg))->dm_rpro );
	/*
	printf( "dmntr nopen = %x\n", tvc );
	*/
	adddwe( dwe->wm_dpro, dwe );
	while ( 1 ) {
		n = nread( nvc, &rbuf, sizeof(rbuf) );
		if ( n != 0 ) {
			/*
			printf( "dfsr msg from ncf\n" );
			*/
			u.u_error = NULL;
			switch ( rbuf.ncf_type ) {

			case	NCFORQ:
				printf( "dmntr open req, illegal\n" );
				debug();
				break;

			case	NCFCRQ:
if ( ddbug ) {
				printf( "dmntr close req\n" );
				debug();
}
				if ( rbuf.ncf_lvc != tvc ) {
if ( ddbug ) {
					printf( "dmntr NCFCRQ %x\n",
						rbuf.ncf_lvc );
					debug();
}
					return;
				}
				rbuf.ncf_stat = DFSOK;
				nwrite( nvc, &rbuf, sizeof(rbuf) );
				ndetach( nvc );
				rdprocess( dpro );
				u.u_cdir = scdir;
				u.u_rdir = srdir;
				sysexit();
				break;

			default:
				printf( "Illegal read from NCF\n" );
				break;


			}
		} else {
			/*
			printf( "dmntr data,  some conn\n" );
			*/
			while ( (i = nioctl( nvc, NCFDATA )) != -1 ) {
				/*
				printf( "dmntr data on %x\n", i );
				*/
				mbuf = ( struct dfsdfs *)
					smalloc(sizeof(struct dfsdfs));
				nread( i, mbuf, sizeof(struct dfsdfs) );
				u.u_error = NULL;
				switch (  mbuf->op_type ) {

				case DFSNMIRQ:
if ( ddbug ) {
					printf( "dmntr got namei req\n" );
					debug();
}
					mbuf->nmi_dpro = mdpro;
					mbuf->nmi_dwe->wn_rmsg = mbuf;
					wakeup((caddr_t)mbuf->nmi_dwe);
					break;

				case DFSIPRQ:
if ( ddbug ) {
					printf("dmntr got iput resp\n");
					debug();
}
					wakeup( (caddr_t)mbuf->ip_dwe );
					goto	rtnbuf;
					break;

				case DFSICRQ:
if ( ddbug ) {
					printf("dmntr got icount resp\n");
					debug();
}
					wakeup( (caddr_t)mbuf->cip_dwe );
					goto	rtnbuf;
					break;

				case DFSPRERQ:
					wakeup( (caddr_t)mbuf->ip_dwe );
					goto	rtnbuf;
					break;

				case DFSPLCKRQ:
					wakeup( (caddr_t)mbuf->ip_dwe );
					goto	rtnbuf;
					break;

				case DFSSISRQ:
					wakeup( (caddr_t)mbuf->sis_dwe );
					goto	rtnbuf;
					break;

				case DFSCMRQ:
if ( ddbug ) {
					printf("dmntr got chmod resp\n");
					debug();
}
					wakeup( (caddr_t)mbuf->cm_dwe );
					goto	rtnbuf;
					break;

				case DFSCORQ:
if ( ddbug ) {
					printf("dmntr got chown resp\n");
					debug();

}
					mbuf->co_dwe->wco_mode =
						mbuf->co_imode;
					mbuf->co_dwe->wco_uid = 
						mbuf->co_uid;
					mbuf->co_dwe->wco_gid = 
						mbuf->co_gid;
					wakeup( (caddr_t)mbuf->co_dwe );
					goto	rtnbuf;
					break;

				case DFSS1RQ:
if ( ddbug ) {
					printf("dmntr got stat1 resp\n");
					debug();
}
					mbuf->s1_dwe->ws1_rmsg = mbuf;
					wakeup( (caddr_t)mbuf->s1_dwe );
					break;

				case DFSMKNRQ:
if ( ddbug ) {
					printf( "dmntr got maknode req\n" );
					debug();
}
					if ( mbuf->mkn_rip != NULL ) {
						ip = riget(mdpro,
							    mbuf->mkn_rip,
								ILOCK);
						if ( ip != NULL ) {
							ip->i_uid = 
							  mbuf->mkn_uid;
							ip->i_gid = 
							  mbuf->mkn_gid;
							ip->i_flag |=
							  mbuf->mkn_iflag;
							ip->i_dev |=
							  mbuf->mkn_fflag;
							ip->i_mode =
							  mbuf->mkn_imode;
						}
					} else ip = NULL;
					mbuf->mkn_dwe->wmn_ip = ip;
if ( ddbug ) {
					printf( "rtn from riget %x\n",
							ip );
					debug();
}
					mbuf->mkn_dwe->wmn_error =
						mbuf->mkn_error;
					wakeup((caddr_t)mbuf->mkn_dwe);
					goto	rtnbuf;
					break;

				case DFSITRQ:
if ( ddbug ) {
					printf("dmntr got itrunc resp\n");
					debug();
}
					wakeup( (caddr_t)mbuf->it_dwe );
					goto	rtnbuf;
					break;

				case DFSUTRQ:
if ( ddbug ) {
					printf("dmntr got utime resp\n");
					debug();
}
					wakeup( (caddr_t)mbuf->ut_dwe );
					goto	rtnbuf;
					break;

				default:
					printf("dmntr illegal msg\n" );
					debug();
					goto	rtnbuf;
					break;
				}
				continue;
				rtnbuf:
					smfree(sizeof(struct dfsdfs),mbuf);
			}
		}
	}
}

/*
 * Remote dfs mount request.
 */

rdmntr( dpro, msg )
register struct	dfsmnt	*msg;
register struct	dqp	*dpro;
{
	register	i, nvc, rvc, wvc;
	register struct	dfsdfs	*mbuf;
	struct	inode	*ip, *scdir, *srdir;
	struct ncfmsg	rbuf;
	register struct rmntab	*mntab;
	struct	rmntab	*grmntab();
	struct	dqp	*udpro;
	int	schar(), n;

	/*
	printf( "Rdmntr called %x, %x\n", dpro, msg );
	debug();
	*/
	nvc = nattach( dpro->qp_name );
	/*
	printf( "Rdmntr attach %x\n", nvc );
	*/
	rvc = nopen( nvc, msg->dm_rnode, msg->dm_pro );
	/*
	printf( "Rdmntr nopen %x\n", rvc );
	*/
	for ( i = 0; i < 9; i++ )
		if ( !(msg->dm_rpro[i] = dpro->qp_name[i]) )
			break;
	u.u_dirp = msg->dm_rdir;

	ip = namei( schar, 0, NULL );
	if ( ip == NULL ) {
		printf( "Namei failed %x\n", u.u_error );
		msg->dm_error = u.u_error;
		goto out1;
	}
	/*
	printf( "Namei passed %x\n", ip );
	*/
	if ( (ip->i_mode&IFMT) != IFDIR ) {
		printf( "%s not directory\n", msg->dm_rdir );
		msg->dm_error = ENOTDIR;
		goto out1;
	}
	mntab = grmntab();
	if ( mntab == 0 ) {
		printf( "No rmttab table entries\n" );
		goto out1;
	}
	mntab->mt_inode = ip;
	mntab->mt_rvc = rvc;
	for ( i = 0; i < 9; i++ ) 
		if ( !(mntab->mt_dir[i] = msg->dm_rdir[i]) )
			break;
	msg->dm_stat = DFSOK;
	msg->dm_ip = ip;
	scdir = u.u_cdir;
	srdir = u.u_rdir;
	u.u_cdir = u.u_rdir = ip;
	nwrite( rvc, msg, sizeof(struct dfsmnt) );
	smfree( sizeof(struct dfsmnt), msg );
	while ( 1 ) {
		n = nread( nvc, &rbuf, sizeof(rbuf) );
		if ( n != 0 ) {
			/*
			printf( "dfsr msg from ncf\n" );
			*/
			u.u_error = NULL;
			switch ( rbuf.ncf_type ) {

			case	NCFORQ:
				/*
				printf("rdmntr open req %x\n", rbuf.ncf_lvc);
				debug();
				*/
				rbuf.ncf_stat = DFSOK;
				nwrite( nvc, &rbuf, sizeof(rbuf) );
				wvc = mntab->mt_wvc = rbuf.ncf_lvc;
				break;

			case	NCFCRQ:
if ( ddbug ) {
				printf( "rdmntr close req\n" );
				debug();
}
				if ( rbuf.ncf_lvc != rvc ) {
if ( ddbug ) {
					printf( "rdmntr NCFCRQ %x\n",
						rbuf.ncf_lvc );
					debug();
}
					return;
				}
				rbuf.ncf_stat = DFSOK;
				nwrite( nvc, &rbuf, sizeof(rbuf) );
				mntab->mt_wvc = NULL;
				mntab->mt_rvc = NULL;
				nclose( wvc );
				ndetach( nvc );
				rdprocess( dpro );
				u.u_cdir = scdir;
				u.u_rdir = srdir;
				sysexit();
				break;

			default:
				printf( "Illegal read from NCF\n" );
				break;


			}
		} else {
			/*
			printf( "rdmntr data,  some conn\n" );
			*/
			while ( (i = nioctl( nvc, NCFDATA )) != -1 ) {
				/*
				printf( "rdmntr data on %x\n", i );
				*/
				mbuf = ( struct dfsdfs *)
					smalloc(sizeof(struct dfsdfs));
				nread( i, mbuf, sizeof(struct dfsdfs) );
				u.u_error = NULL;
				switch (  mbuf->op_type ) {

				

				case DFSNMIRQ:
if ( ddbug ) {
					printf( "rdmntr got namei req\n" );
					debug();
}
					rrnamei( wvc, mbuf );
					goto	rtnbuf;
					break;

				case DFSIPRQ:
if ( ddbug ) {
					printf( "rdmntr got iput\n" );
}
					iput( mbuf->ip_inode );
					mbuf->ip_stat = DFSOK;
					nwrite(wvc,mbuf,sizeof(struct dfsdfs));
					goto	rtnbuf;
					break;

				case DFSICRQ:
if ( ddbug ) {
					printf( "rdmntr got icount\n" );
}
					mbuf->cip_inode->i_count++;
					mbuf->cip_stat = DFSOK;
					nwrite(wvc,mbuf,sizeof(struct dfsdfs));
					goto	rtnbuf;
					break;

				case DFSPRERQ:
if ( ddbug ) {
					printf( "rdmntr got prele\n" );
}
					prele( mbuf->ip_inode );
					mbuf->ip_stat = DFSOK;
					nwrite(wvc,mbuf,sizeof(struct dfsdfs));
					goto	rtnbuf;
					break;

				case DFSPLCKRQ:
if ( ddbug ) {
					printf( "rdmntr got plock\n" );
}
					plock( mbuf->ip_inode );
					mbuf->ip_stat = DFSOK;
					nwrite(wvc,mbuf,sizeof(struct dfsdfs));
					goto	rtnbuf;
					break;

				case DFSSISRQ:
if ( ddbug ) {
					printf( "rdmntr sister req " );
					printf( "%s, %s\n", mbuf->sis_name,
							mbuf->sis_node );
}
					udpro = gdprocess( 'U' );
					if ( udpro == (struct dqp *)-1 ) {
						printf("rdmntr no process\n");
						debug();
					}
					if( sysproc() ) {
						for(i=0; i<9; i++)
							u.u_comm[i] =
							  udpro->qp_name[i];
						ruserp( udpro, mbuf );
						rdprocess( udpro );
						sysexit();
					}
					mbuf->sis_stat = DFSOK;
					nwrite(wvc,mbuf,sizeof(struct dfsdfs));
					break;

				case DFSCMRQ:
if ( ddbug ) {
					printf( "rdmntr got chmod req\n" );
					debug();
}
					rrchmod( wvc, mbuf );
					goto	rtnbuf;
					break;

				case DFSCORQ:
if ( ddbug ) {
					printf( "rdmntr got chown req\n" );
					debug();
}
					rrchown( wvc, mbuf );
					goto	rtnbuf;
					break;

				case DFSS1RQ:
if ( ddbug ) {
					printf( "rdmntr got stat1 req\n" );
					debug();
}
					rrstat1( wvc, mbuf );
					goto	rtnbuf;
					break;

				case DFSMKNRQ:
if ( ddbug ) {
					printf( "rdmntr got maknode req\n" );
					debug();
}
					rrmaknode( wvc, mbuf );
					goto	rtnbuf;
					break;

				case DFSITRQ:
if ( ddbug ) {
					printf( "rdmntr got iput\n" );
}
					itrunc( mbuf->it_inode );
					mbuf->it_stat = DFSOK;
					nwrite(wvc,mbuf,sizeof(struct dfsdfs));
					goto	rtnbuf;
					break;

				case DFSUTRQ:
if ( ddbug ) {
					printf( "rdmntr got utime req\n" );
					debug();
}
					rrutime( mbuf );
					nwrite(wvc,mbuf,sizeof(struct dfsdfs));
					goto	rtnbuf;
					break;

				default:
					printf("rdmntr illegal msg\n" );
					debug();
					goto	rtnbuf;
					break;
				}
				continue;
				rtnbuf:
					smfree( sizeof(struct dfsdfs), mbuf );
			}
		}
	}
	out1:
	msg->dm_stat = DFAIL;
	nwrite( rvc, msg, sizeof(struct dfsmnt) );
	smfree( sizeof(struct dfsmnt), msg );
	debug();
	sysexit();
}


/*
 * Remote User sister process.
 */

ruserp( dpro, msg )
register struct	dfsdfs	*msg;
register struct	dqp	*dpro;
{
	register	i, nvc, uvc;
	register struct	dfsdfs	*mbuf;
	struct ncfmsg	rbuf;
	struct inode *scdir, *srdir;
	int	n;

if ( ddbug ) {
	printf( "ruserp called %x, %x\n", dpro, msg );
	debug();
}
	scdir = u.u_cdir;
	srdir = u.u_rdir;
	nvc = nattach( dpro->qp_name );
if ( ddbug ) {
	printf( "ruserp attach %x\n", nvc );
}
	uvc = nopen( nvc, msg->sis_node, msg->sis_name );
if ( ddbug ) {
	printf( "ruserp nopen %x\n", uvc );
}
	smfree( sizeof(struct dfsdfs), msg );
	while ( 1 ) {
		n = nread( nvc, &rbuf, sizeof(rbuf) );
		if ( n != 0 ) {
if ( ddbug ) {
			printf( "ruserp msg from ncf\n" );
}
			u.u_error = NULL;
			switch ( rbuf.ncf_type ) {

			case	NCFORQ:
if ( ddbug ) {
				printf("ruserp open req %x\n", rbuf.ncf_lvc);
				debug();
}
				break;

			case	NCFCRQ:
if ( ddbug ) {
				printf( "ruserp close req %x\n", &rbuf );
}
				if ( rbuf.ncf_lvc != uvc ) {
if ( ddbug ) {
					printf( "ruserp NCFCRQ %x\n",
						rbuf.ncf_lvc );
					debug();
}
					return;
				}
				rbuf.ncf_stat = DFSOK;
				nwrite( nvc, &rbuf, sizeof(rbuf) );
				ndetach( nvc );
if ( ddbug ) {
				printf( "ruserp exiting\n" );
				debug();
}
				u.u_cdir = scdir;
				u.u_rdir = srdir;
				return;
				break;

			default:
				printf( "Illegal read from NCF\n" );
				break;


			}
		} else {
if ( ddbug ) {
			printf( "ruserp data,  some conn\n" );
}
			while ( (i = nioctl( nvc, NCFDATA )) != -1 ) {
if ( ddbug ) {
				printf( "ruserp data on %x\n", i );
}
				mbuf = ( struct dfsdfs *)
					smalloc(sizeof(struct dfsrdwr));
				nread( i, mbuf, sizeof(struct dfsrdwr) );
				u.u_error = NULL;
				switch (  mbuf->op_type ) {

				case DFSRDRQ:
if ( ddbug ) {
					printf( "ruserp got read request\n" );
					debug();
}
					rmtread( mbuf, uvc );
					goto	rtnbuf;
					break;
				
				case DFSWRRQ:
if ( ddbug ) {
					printf( "ruserp got write request\n" );
					debug();
}
					rmtwrite( mbuf, uvc );
					goto	rtnbuf;
					break;

				case DFSOPNRQ:
if ( ddbug ) {
					printf( "ruserp got open request\n" );
					debug();
}
					ropen( mbuf );
					nwrite(uvc,mbuf,sizeof(struct dfsdfs));
					goto	rtnbuf;
					break;

				case DFSCLSRQ:
if ( ddbug ) {
					printf( "ruserp got close\n" );
					debug();
}
					rclose( mbuf );
					nwrite(uvc,mbuf,sizeof(struct dfsdfs));
					goto	rtnbuf;
					break;

				case DFSFCRQ:

if ( ddbug ) {
					printf( "ruserp got fcntl\n" );
					debug();
}
					rrfcntl( mbuf );
					nwrite(uvc,mbuf,sizeof(struct dfsdfs));
					goto	rtnbuf;
					break;

				case DFSSKRQ:

if ( ddbug ) {
					printf( "ruserp got seek\n" );
					debug();
}
					rrseek( mbuf );
					nwrite(uvc,mbuf,sizeof(struct dfsdfs));
					goto	rtnbuf;
					break;

				default:
					printf("ruserp illegal msg\n" );
					debug();
					goto	rtnbuf;
					break;
				}
				continue;
				rtnbuf:
					smfree( sizeof(struct dfsrdwr), mbuf );
			}
		}
	}
}

/*
 * ropen - remote version of open.
 */

ropen( msg )
register struct dfsdfs *msg;
{

	register struct	file	*fp;

	if ( (fp = falloc()) == NULL ) {
		msg->op_stat = DFAIL;
		msg->op_error = u.u_error;
		printf( "ropen falloc error %x\n", u.u_error );
		debug();
		return;
	}
	fp->f_flag = msg->op_flag;
	fp->f_inode = msg->op_inode;
	openi( msg->op_inode, msg->op_flag );
if ( ddbug ) {
	printf( "ruserp openi returned %x\n", u.u_error );
	debug();
}
	if ( u.u_error )
		iput( msg->op_inode );
	msg->op_error = u.u_error;
	msg->op_fp = fp;

}

/*
 *  Remote file close.
 */

rclose( msg )
register struct dfsdfs *msg;
{

	register	i;

	for ( i = 0; i < NOFILE; i++ ) {
		if ( msg->cl_fp ==  u.u_ofile[i] )
			break;
		}
	if ( i < NOFILE ) {
		closef( msg->cl_fp );
		u.u_ofile[i] = 0;
	}
	msg->cl_error = u.u_error;
}

/*
 *  Get and entry from the work queue of an dfs process.
 */

struct	dfswe *
gqent( dpro )
struct	dqp	*dpro;
{
	register struct dfswe *dwe;
	int	spri;

	spri = spl7();
	while ( !dpro->qp_flink )
		sleep( (caddr_t)dpro, DFSPRI );
	dwe = dpro->qp_flink;
	if ( !(dpro->qp_flink = dwe->we_link) )
		dpro->qp_llink = (struct dfswe *)0;
	splx( spri );
	return( dwe );
}

/*
 *  Add to the dfs work queue.
 */

adfsq( dwe )
struct dfswe	*dwe;
{
	register	spri;

	spri = spl7();
	if ( !dqp[0].qp_flink ) {
		dqp[0].qp_flink = dqp[0].qp_llink = dwe;
	} else {
		dqp[0].qp_llink->we_link = dwe;
		dqp[0].qp_llink = dwe;
	}
	dwe->we_link = (struct dfswe *)0;
	splx( spri );
	wakeup( (caddr_t)&dqp[0] );
	sleep( (caddr_t)dwe, DFSPRI );
}

/*
 *  Add to work queue.
 */

adddwe( dpro, dwe )
register struct dfswe	*dwe;
register struct dqp	*dpro;
{
	register	spri;

	spri = spl7();
	if ( !dpro->qp_flink ) {
		dpro->qp_flink = dpro->qp_llink = dwe;
	} else {
		dpro->qp_llink->we_link = dwe;
		dpro->qp_llink = dwe;
	}
	dwe->we_link = (struct dfswe *)0;
	splx( spri );
	wakeup( (caddr_t)dpro );
}

badfrk()
{
	printf( "Returned to fork spot\n" );
	debug();
}

/*
 *  System process exit routine.
 */

sysexit()
{
	register struct proc	*p;

	p = u.u_procp;
	if ( u.u_cdir ) 
		u.u_cdir->i_count--;
	if ( u.u_rdir )
		u.u_rdir->i_count--;
	mfree( coremap, p->p_size, p->p_addr );
	freeproc( p, 0 );
	resume( proc[0].p_addr, u.u_qsav );
}

/* 
 * Dummy NCF routines.
 */

nattach( name )
char	*name;
{
	register	i, j;

if ( ddbug ) {
	printf( "nattach %s\n", name );
}
	for ( i = 0; i < NNCFPRO; i++ )
		if ( np[i].np_name[0] == '\0' )
			break;
	if ( i == NNCFPRO ) {
		printf( "No more NCF processes allowed\n" );
		sleep( (caddr_t)&np[0], DFSPRI );
	}
	for ( j = 0; j < 9; j++ )
		if ( !(np[i].np_name[j] = *name++) )
			break;
	np[i].nq[0].nq_stat = QACTIVE;
	np[i].nq[0].nq_pvc = (i << 8);
	return( ( i << 8 )  );
}

ndetach( pvc )
{
	register struct ncfq	*cp;
	register	i;

if ( ddbug ) {
	printf( "ndetach called %x\n", pvc );
	debug();
}
	for ( i = 0; i < MCONS; i++ ) {
		cp = &np[pvc>>8].nq[i];
		if ( cp->nq_stat == QACTIVE )
			nclose( pvc | i );
	}
	for ( i=0; i<9; i++ )
		np[pvc>>8].np_name[i] = '\0';
}

nread( vc, buf, cnt )
char	*buf;
{
	register char	*bp;
	register struct ncfq	*npp;
	register	i, j, k;
	int	*saddr;
	int	scnt;

if ( ddbug ) {
	printf( "nread %x, %x, %x\n", vc, buf, cnt );
}
	j = vc>>8;
	k = vc&0xff;
	npp = &np[j].nq[k];
	while ( !(npp->nq_list[0]) ) {
		if ( !k &&
			(i = nioctl( vc, NCFDATA)) != -1 )
				return(0);
		sleep( (caddr_t)npp, DFSPRI );
	}
	saddr = (int *)npp->nq_list[0];
	scnt = npp->nq_lsize[0];
	for ( bp = buf; cnt-- && npp->nq_lsize[0]--; bp++ ) {
		*bp = *npp->nq_list[0]++;
	}
	smfree( scnt, saddr );
	for ( i = 0; i < (NNCFLIST-1); i++ ) {
		npp->nq_list[i] = 
			npp->nq_list[i+1];
		npp->nq_lsize[i] = 
			npp->nq_lsize[i+1];
	}
	return( bp - buf );
}

nwrite( vc, buf, cnt )
register char	*buf;
{
	register	i, j, k;
	register struct ncfmsg *bp;

if ( ddbug ) {
	printf( "nwrite %x, %x, %x\n", vc, buf, cnt );
}
	i = vc>>8;
	j = vc&0xff;
	if ( !j ) {
		bp = (struct ncfmsg *)buf;
		switch ( bp->ncf_type ) {

		default:
			printf( "Write to NCF unknown type\n" );
			debug();
			break;

		case	NCFORQ:
			j = bp->ncf_lvc & 0xff;
			np[i].nq[j].nq_stat = QACTIVE;
			np[i].nq[j].nq_pvc = bp->ncf_rvc;
			i = bp->ncf_rvc >> 8;
			j = bp->ncf_rvc & 0xff;
			np[i].nq[j].nq_stat = QACTIVE;
			np[i].nq[j].nq_pvc = bp->ncf_lvc;
			wakeup( (caddr_t)&np[i].nq[j] );
			break;

		case	NCFCRQ:
			j = bp->ncf_lvc & 0xff;
			i = bp->ncf_lvc >> 8;
			for ( k = 0; k < NNCFLIST; k++ ) {
				if ( !(np[i].nq[j].nq_list[k]) )
					break;
				smfree( np[i].nq[j].nq_lsize[k], 
					np[i].nq[j].nq_list[k] );
				np[i].nq[j].nq_list[k] = 0;
				np[i].nq[j].nq_lsize[k] = 0;
			}
			np[i].nq[j].nq_stat = QFREE;
			break;

		}
	} else {
		if ( np[i].nq[j].nq_stat != QACTIVE )
			return( -1 );
		i = nqueue( np[i].nq[j].nq_pvc, buf, cnt );
		return( !i ? cnt: -1 );
	}
}

nopen( pvc, node, who )
register char	*node, *who;
{
	register i, j, k;
	struct	ncfmsg	omsg;
	int	spri;

if ( ddbug ) {
	printf( "nopen %x, %s, %s\n", pvc, node, who );
}
	for ( i = 0; i < NNCFPRO; i++ ) {
		if ( !nstrncmp( who, np[i].np_name, 9 ) )
			break;
	}
	if ( i == NNCFPRO ) {
		printf( "Try to nopen non-existing process\n" );
		return( -1 );
	}
	for ( j = 0; j < MCONS; j++ ) {
		if ( np[pvc>>8].nq[j].nq_stat == QFREE )
			break;
	}
	if ( j == MCONS ) {
		printf( "No connections available\n" );
		return( -1 );
	}
	np[pvc >> 8].nq[j].nq_stat = QWAIT;
	for ( k = 0; k < MCONS; k++ ) {
		if ( np[i].nq[k].nq_stat == QFREE )
			break;
	}
	if ( k == MCONS ) {
		printf( "Nothing to connect to at destination\n" );
		np[pvc >> 8].nq[j].nq_stat = QFREE;
		return( -1 );
	}
	np[i].nq[k].nq_stat = QWAIT;
	omsg.ncf_lvc = (i << 8 ) | k;
	omsg.ncf_rvc = pvc | j;
	omsg.ncf_type = NCFORQ;
	spri = spl7();
	nqueue( (i << 8), &omsg, sizeof(omsg) );
	sleep( (caddr_t)&np[pvc >>8].nq[j], DFSPRI );
	splx(spri);
	return( omsg.ncf_rvc );
}

nclose( vc )
{
	register	struct	ncfq *lpp, *rpp;
	register	i, k;
	struct	ncfmsg	cmsg;

	lpp = &np[vc>>8].nq[vc&0xff];
	rpp = &np[lpp->nq_pvc>>8].nq[lpp->nq_pvc&0xff];
	cmsg.ncf_type = NCFCRQ;
	cmsg.ncf_stat = CNORM;
	cmsg.ncf_lvc = lpp->nq_pvc;
	for ( k = 0; k < NNCFLIST; k++ ) {
		if ( !lpp->nq_list[k] )
			break;
		smfree( lpp->nq_lsize[k], lpp->nq_list[k] );
		lpp->nq_list[k] = 0;
		lpp->nq_lsize[k] = 0;
	}
	lpp->nq_stat = QFREE;
	i = lpp->nq_pvc >> 8;
	lpp->nq_pvc = 0;
	for ( k = 0; k < NNCFLIST; k++ ) {
		if ( !rpp->nq_list[k] )
			break;
		smfree( rpp->nq_lsize[k], rpp->nq_list[k] );
		rpp->nq_list[k] = 0;
		rpp->nq_lsize[k] = 0;
	}
	rpp->nq_stat = QFREE;
	rpp->nq_pvc = 0;
	if ( (i << 8) != vc )
		nqueue( (i << 8), &cmsg, sizeof(cmsg) );
	return( 0 );
}

nioctl( pvc, cmd )
{
	register	i, j;

if ( ddbug ) {
	printf( "nioclt %x, %x\n", pvc, cmd );
}
	switch ( cmd ) {

	default:
		printf( "Illegal nioctl call\n" );
		break;

	case	NCFDATA:

		j = pvc>>8;
		for ( i = 0; i < MCONS; i++ ) {
			if ( np[j].nq[i].nq_list[0] )
				return( (j << 8) | i );
		}
		return( -1 );
		break;
	}
}

/*
 *  Queue a message to the specified queue.
 */

nqueue( pvc, buf, cnt )
register char	*buf;
{
	register	i, j, k;
	register char	*bp;

	i = pvc >> 8;
	j = pvc & 0xff;
	for ( k = 0; k < NNCFLIST; k++ ) {
		if ( np[i].nq[j].nq_list[k] )
			continue;
		bp = np[i].nq[j].nq_list[k] = (char *)smalloc( cnt );
		np[i].nq[j].nq_lsize[k] = cnt;
		while ( cnt-- )
			*bp++ = *buf++;
		wakeup( (caddr_t)&np[i].nq[j] );
		wakeup( (caddr_t)&np[i].nq[0] );
		return( 0 );
	}
	return(-1);
}
/*
 * Compare strings (at most n bytes):  s1>s2: >0  s1==s2: 0  s1<s2: <0
 */

nstrncmp(s1, s2, n)
register char *s1, *s2;
register n;
{

	while (--n >= 0 && *s1 == *s2++)
		if (*s1++ == '\0')
			return(0);
	return(n<0 ? 0 : *s1 - *--s2);
}

#endif
