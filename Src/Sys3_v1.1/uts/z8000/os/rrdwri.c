#ifdef	PNETDFS
#include "sys/param.h"
#include "sys/inode.h"
#include "sys/dir.h"
#include "sys/user.h"
#include "sys/buf.h"
#include "sys/conf.h"
#include "sys/file.h"
#include "sys/systm.h"
#include "sys/stat.h"
#include "sys/pnetdfs.h"

extern	ushort	ddbug;
/*
 * rrdwr - issue remote read or write request.
 */

rrdwr( ip, fdes, mode )
register struct inode *ip;
{

	if ( mode == FREAD ) {
		rreadi( ip, fdes );
	} else {
		rwritei( ip, fdes );
	}
}

/*
 * Read the remote file corresponding to
 * the inode pointed at by the argument.
 */
rreadi(ip, fdes)
register struct inode *ip;
{
	register struct	dfsrdwr	*msg;
	register unsigned n;

	if (u.u_count == 0)
		return;
	if (u.u_offset < 0) {
		u.u_error = EINVAL;
		return;
	}
	msg = (struct dfsrdwr *)smalloc( sizeof(struct dfsrdwr) );
	msg->rw_type = DFSRDRQ;
	msg->rw_count = u.u_count;
	msg->rw_fp = u.u_rofile[fdes];
if ( ddbug ) {
	printf( "rreadi sending read request %x, %x\n", u.u_count,
					msg->rw_fp);
	debug();
}
	nwrite( u.u_dfsvc, msg, sizeof( struct dfsrdwr ) );
	do {
		n = nread( u.u_dfsvc, msg, sizeof( struct dfsrdwr ) );
if ( ddbug ) {
		printf( "rreadi revcv data %x\n", msg );
		debug();
}
		if ( msg->rw_type != DFSRDRQ ) {
			printf( "rreadi got odd read %x\n", msg );
			debug();
			u.u_error = EIO;
			return;
		}
		n = msg->rw_count;
		u.u_error = msg->rw_error;
if ( ddbug ) {
		printf( "rreadi copyout %x, %x, %x\n", msg->rw_data,
					u.u_base, n );
		debug();
}
		if ( n > 0 ) {
			if ( copyout( msg->rw_data, u.u_base, n ) == -1 )
				u.u_error = EFAULT;
			u.u_base += n;
			u.u_offset += n;
			u.u_count -= n;
		}
	} while ( u.u_error==0 && u.u_count && !(msg->rw_flag&DFSMLST) );
	smfree( sizeof(struct dfsrdwr), msg );
}

/*
 * System read of the remote file corresponding to
 * the inode pointed at by the argument.
 */
srreadi( ip )
register struct inode *ip;
{
	register struct	dfsrdwr	*msg;
	register unsigned n;

	printf( "srreadi called, not there %x\n", ip );
	debug();
	return;
/*
	msg = (struct dfsrdwr *)smalloc( sizeof(struct dfsrdwr) );
	msg->rw_type = DFSRDRQ;
	msg->rw_count = u.u_count;
	msg->rw_fp = u.u_rofile[fdes];
if ( ddbug ) {
	printf( "srreadi sending read request %x, %x\n", u.u_count,
					msg->rw_fp);
	debug();
}
	nwrite( u.u_dfsvc, msg, sizeof( struct dfsrdwr ) );
	do {
		n = nread( u.u_dfsvc, msg, sizeof( struct dfsrdwr ) );
if ( ddbug ) {
		printf( "rreadi revcv data %x\n", msg );
		debug();
}
		if ( msg->rw_type != DFSRDRQ ) {
			printf( "rreadi got odd read %x\n", msg );
			debug();
			u.u_error = EIO;
			return;
		}
		n = msg->rw_count;
		u.u_error = msg->rw_error;
if ( ddbug ) {
		printf( "rreadi copyout %x, %x, %x\n", msg->rw_data,
					u.u_base, n );
		debug();
}
		if ( n > 0 ) {
			if ( copyout( msg->rw_data, u.u_base, n ) == -1 )
				u.u_error = EFAULT;
			u.u_base += n;
			u.u_offset += n;
			u.u_count -= n;
		}
	} while ( u.u_error==0 && u.u_count && !(msg->rw_flag&DFSMLST) );
	smfree( sizeof(struct dfsrdwr), msg );
*/
}

/*
 * Write the remote file corresponding to
 * the inode pointed at by the argument.
 */
rwritei( ip, fdes)
register struct inode *ip;
{
	register struct dfsrdwr	*msg;
	register unsigned n;
	register	fflag;

	if (u.u_offset < 0) {
		u.u_error = EINVAL;
		return;
	}
	if ( u.u_count == 0 )
		return;
	msg = (struct dfsrdwr *)smalloc( sizeof(struct dfsrdwr) );
	msg->rw_type = DFSWRRQ;
	msg->rw_fp = u.u_rofile[fdes];
	fflag = DFSMFST;
	do {
		n = min( u.u_count, MAXDFSREC );
		u.u_count -= n;
if ( ddbug ) {
		printf( "rwritei copyin %x, %x, %x\n", msg->rw_data,
				u.u_base, n );
		debug();
}
		if ( copyin( u.u_base, msg->rw_data, n ) < 0 ) {
			u.u_error = EFAULT;
			n = 0;
			if ( fflag & DFSMFST )
				goto out1;
			fflag = DFSMLST;
		}
		u.u_base += n;
		msg->rw_count = n;
		msg->rw_flag = ( !(u.u_count) ? (DFSMLST | fflag) : fflag );
		fflag = DFSMMID;

if ( ddbug ) {
		printf( "rwritei %x, %x, %x\n", msg, u.u_base, u.u_count );
		debug();
}
		nwrite( u.u_dfsvc, msg, sizeof(struct dfsrdwr) );
	} while ( u.u_error==0 && u.u_count && n!=0 );
	nread( u.u_dfsvc, msg, sizeof(struct dfsrdwr) );
	if ( msg->rw_type != DFSWRRQ ) {
		printf( "rwritei bad net msg %x\n", msg );
		u.u_error = EIO;
	} else u.u_error = msg->rw_error;
	out1:
	smfree( sizeof(struct dfsrdwr), msg );
}

/*
 * System write of the remote file corresponding to
 * the inode pointed at by the argument.
 */
srwritei( ip )
register struct inode *ip;
{
	register struct dfsrdwr	*msg;
	register unsigned n;
	register	fflag;

	printf( "srwritei called, not there %x\n", ip );
	debug();
	return;
/*
	msg = (struct dfsrdwr *)smalloc( sizeof(struct dfsrdwr) );
	msg->rw_type = DFSWRRQ;
	msg->rw_fp = u.u_rofile[fdes];
	fflag = DFSMFST;
	do {
		n = min( u.u_count, MAXDFSREC );
		u.u_count -= n;
if ( ddbug ) {
		printf( "rwritei copyin %x, %x, %x\n", msg->rw_data,
				u.u_base, n );
		debug();
}
		if ( copyin( u.u_base, msg->rw_data, n ) < 0 ) {
			u.u_error = EFAULT;
			n = 0;
			if ( fflag & DFSMFST )
				goto out1;
			fflag = DFSMLST;
		}
		u.u_base += n;
		msg->rw_count = n;
		msg->rw_flag = ( !(u.u_count) ? (DFSMLST | fflag) : fflag );
		fflag = DFSMMID;

if ( ddbug ) {
		printf( "rwritei %x, %x, %x\n", msg, u.u_base, u.u_count );
		debug();
}
		nwrite( u.u_dfsvc, msg, sizeof(struct dfsrdwr) );
	} while ( u.u_error==0 && u.u_count && n!=0 );
	nread( u.u_dfsvc, msg, sizeof(struct dfsrdwr) );
	if ( msg->rw_type != DFSWRRQ ) {
		printf( "rwritei bad net msg %x\n", msg );
		u.u_error = EIO;
	} else u.u_error = msg->rw_error;
	out1:
	smfree( sizeof(struct dfsrdwr), msg );
*/
}

/*
 *  Perform file read for remote request.
 */

rmtread( msg, uvc )
register struct dfsrdwr *msg;
{
	register struct	inode	*ip;
	register struct file	*fp;
	register	n, scnt;
	int	mflag;
	struct	inode	*rmtsetup();

	fp = msg->rw_fp;
	ip = rmtsetup( msg, FREAD );
	if ( !u.u_error ) {
		n = msg->rw_count;
		mflag = DFSMFST;
		while ( n && !u.u_error ) {
			scnt = u.u_count = min( n, MAXDFSREC );
			u.u_base = (caddr_t)msg->rw_data;
			u.u_offset = fp->f_offset;
if ( ddbug ) {
			printf("rmtread readi %x, %x, %x, %x, %x, %x\n",
				ip, u.u_base, u.u_offset, u.u_count, 
				u.u_segflg );
			debug();
}
			readi( ip );
			if ( u.u_count ) {
if ( ddbug ) {
				printf("non-zero u.u_count %x\n", u.u_count);
				debug();
}
				scnt -= u.u_count;
				n = scnt;
			}
			n -= scnt;
			msg->rw_count = scnt;
			msg->rw_error = u.u_error;
			fp->f_offset += scnt;
			msg->rw_flag = ( !n || u.u_error ?
					(DFSMLST|mflag) : mflag );
			mflag = DFSMMID;
			nwrite(uvc, msg, sizeof(struct dfsrdwr) );
		}
		prele( ip );
	} else {
		msg->rw_error = u.u_error;
		msg->rw_count = 0;
		nwrite(uvc, msg, sizeof(struct dfsrdwr) );
	}
}

/*
 *  Perform file write for remote request.
 */

rmtwrite( msg, uvc )
register struct dfsrdwr *msg;
{
	register struct	inode	*ip;
	register struct file *fp;
	struct	inode	*rmtsetup();

	fp = msg->rw_fp;
	ip = rmtsetup( msg, FWRITE );
	if ( !u.u_error ) {
		while ( !u.u_error ) {
			u.u_base = (caddr_t)msg->rw_data;
			u.u_count = msg->rw_count;
			u.u_offset = fp->f_offset;
if ( ddbug ) {
			printf("rmtwrite readi %x, %x, %x, %x, %x, %x\n",
				ip, u.u_base, u.u_offset, u.u_count, 
				u.u_segflg );
			debug();
}
			fp->f_offset += u.u_count;
			writei( ip );
			if ( msg->rw_flag & DFSMLST )
				break;
			nread( uvc, msg, sizeof(struct dfsrdwr) );
		}
	}
	prele( ip );
	msg->rw_error = u.u_error;
	nwrite( uvc, msg, sizeof(struct dfsrdwr) );
}

struct	inode	*
rmtsetup( msg, mode )
register struct dfsrdwr	*msg;
{
	register struct	inode	*ip;
	register struct file	*fp;
	register	type;

	fp = msg->rw_fp;
	if ( (fp == NULL) || (fp->f_flag&mode) == 0 ) {
		msg->rw_error = EBADF;
		msg->rw_count = 0;
		return(NULL);
	}
	u.u_segflg = 1;
	u.u_fmode = fp->f_flag;
	ip = fp->f_inode;
	type = ip->i_mode&IFMT;
	if( type == IFREG || type == IFDIR ) {
		if ( ip->i_locklist &&
			locked(1,ip,u.u_offset,u.u_offset+u.u_count) ){
				msg->rw_error = u.u_error;
		} else {
			if ( (u.u_fmode&FAPPEND) && (mode == FWRITE) )
				fp->f_offset = ip->i_size;
			plock( ip );
		}
	} else if ( type == IFIFO ) {
		if ( ip->i_locklist &&
			locked(1,ip,u.u_offset,u.u_offset+u.u_count)) {
				msg->rw_error = u.u_error;
		} else {
			plock( ip );
			fp->f_offset = 0;
		}
	}
	u.u_offset = fp->f_offset;
	return( ip );
}
#endif
