#include "sys/param.h"
#include "sys/systm.h"
#include "sys/acct.h"
#include "sys/dir.h"
#include "sys/user.h"
#include "sys/inode.h"

/*
 * Perform process accounting functions.
 */

sysacct()
{
	register struct inode *ip;
	register struct a {
		char	*fname;
	} *uap;

	uap = (struct a *)u.u_ap;
	if (!suser())
		return;
	if (uap->fname==NULL) {
		if (acctp) {
			plock(acctp);
			iput(acctp);
			acctp = NULL;
		}
		return;
	}
	if (acctp) {
		u.u_error = EBUSY;
		return;
	}
#ifndef	PNETDFS
	ip = namei(uchar, 0);
#else
	ip = namei(uchar, 0, NULL);
#endif
	if(ip == NULL)
		return;
	if((ip->i_mode & IFMT) != IFREG) {
		u.u_error = EACCES;
		iput(ip);
		return;
	}
	acctp = ip;
	prele(ip);
}

/*
 * On exit, write a record on the accounting file.
 */
acct(st)
{
	register struct inode *ip;
	off_t siz;

	if ((ip=acctp)==NULL)
		return;
	plock(ip);
	bcopy(u.u_comm,acctbuf.ac_comm,sizeof(acctbuf.ac_comm));
	acctbuf.ac_btime = u.u_start;
	acctbuf.ac_utime = compress(u.u_utime);
	acctbuf.ac_stime = compress(u.u_stime);
	acctbuf.ac_etime = compress(lbolt - u.u_ticks);
	acctbuf.ac_mem = compress(u.u_mem);
	acctbuf.ac_io = compress(u.u_ioch);
	acctbuf.ac_rw = compress(u.u_ior+u.u_iow);
	acctbuf.ac_uid = u.u_ruid;
	acctbuf.ac_gid = u.u_rgid;
	acctbuf.ac_tty = u.u_ttyp ? u.u_ttyd : NODEV;
	acctbuf.ac_stat = st;
	acctbuf.ac_flag = u.u_acflag;
	siz = ip->i_size;
	u.u_offset = siz;
	u.u_base = (caddr_t)&acctbuf;
	u.u_count = sizeof(acctbuf);
	u.u_segflg = 1;
	u.u_error = 0;
	u.u_limit = (daddr_t)5000;
	writei(ip);
	if(u.u_error)
		ip->i_size = siz;
	prele(ip);
}

/*
 * Produce a pseudo-floating point representation
 * with 3 bits base-8 exponent, 13 bits fraction.
 */
compress(t)
register time_t t;
{
	register exp = 0, round = 0;

	while (t >= 8192) {
		exp++;
		round = t&04;
		t >>= 3;
	}
	if (round) {
		t++;
		if (t >= 8192) {
			t >>= 3;
			exp++;
		}
	}
	return((exp<<13) + t);
}
