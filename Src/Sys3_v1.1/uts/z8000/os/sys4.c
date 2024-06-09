#include "sys/param.h"
#include "sys/plexus.h"
#include "sys/systm.h"
#include "sys/dir.h"
#include "sys/user.h"
#include "sys/inode.h"
#include "sys/proc.h"
#include "sys/var.h"

#ifdef PNETDFS
extern int ddbug;
#endif
/*
 * Everything in this file is a routine implementing a system call.
 */

gtime()
{
	u.u_rtime = time;
}

stime()
{
	register struct a {
		time_t	time;
	} *uap;

	uap = (struct a *)u.u_ap;
	if (suser()) {
		logtchg(uap->time);
		time = uap->time;
		puttod(time);
	}
}

setuid()
{
	register uid;
	register struct a {
		int	uid;
	} *uap;

	uap = (struct a *)u.u_ap;
	uid = uap->uid;
	if (uid >= MAXUID) {
		u.u_error = EINVAL;
		return;
	}
	if (u.u_ruid == uid || suser()) {
		u.u_uid = uid;
		u.u_procp->p_uid = uid;
		u.u_ruid = uid;
	}
}

getuid()
{

	u.u_rval1 = u.u_ruid;
	u.u_rval2 = u.u_uid;
}

setgid()
{
	register gid;
	register struct a {
		int	gid;
	} *uap;

	uap = (struct a *)u.u_ap;
	gid = uap->gid;
	if (gid >= MAXUID) {
		u.u_error = EINVAL;
		return;
	}
	if (u.u_rgid == gid || suser()) {
		u.u_gid = gid;
		u.u_rgid = gid;
	}
}

getgid()
{

	u.u_rval1 = u.u_rgid;
	u.u_rval2 = u.u_gid;
}

getpid()
{
	u.u_rval1 = u.u_procp->p_pid;
	u.u_rval2 = u.u_procp->p_ppid;
}

setpgrp()
{
	register struct a {
		int	flag;
	} *uap;

	uap = (struct a *)u.u_ap;
	if (uap->flag) {
		u.u_procp->p_pgrp = u.u_procp->p_pid;
		u.u_ttyp = NULL;
	}
	u.u_rval1 = u.u_procp->p_pgrp;
}

sync()
{

	update();
}

nice()
{
	register n;
	register struct a {
		int	niceness;
	} *uap;

	uap = (struct a *)u.u_ap;
	n = uap->niceness;
	if ((n < 0 || n > 2*NZERO) && !suser())
		n = 0;
	n += u.u_procp->p_nice;
	if (n >= 2*NZERO)
		n = 2*NZERO -1;
	if (n < 0)
		n = 0;
	u.u_procp->p_nice = n;
	u.u_rval1 = n - NZERO;
}

/*
 * Unlink system call.
 * Hard to avoid races here, especially
 * in unlinking directories.
 */
unlink()
{
	register struct inode *ip, *pp;
	struct a {
		char	*fname;
	};

#ifndef	PNETDFS
	pp = namei(uchar, 2);
#else
	pp = namei(uchar, 2, NULL);
#endif
	if (pp == NULL)
		return;
#ifdef	PNETDFS
	if ( pp->i_flag & IRMT ) {
if ( ddbug ) {
		printf( "remote unlink, not there yet\n" );
		debug();
}
		goto out1;
	}
#endif
	/*
	 * Check for unlink(".")
	 * to avoid hanging on the iget
	 */
	if (pp->i_number == u.u_dent.d_ino) {
		ip = pp;
		ip->i_count++;
	} else
		ip = iget(pp->i_dev, u.u_dent.d_ino);
	if (ip == NULL)
		goto out1;
	if ((ip->i_mode&IFMT) == IFDIR && !suser())
		goto out;
	/*
	 * Don't unlink a mounted file.
	 */
	if (ip->i_dev != pp->i_dev) {
		u.u_error = EBUSY;
		goto out;
	}
	if (ip->i_flag&ITEXT)
		xrele(ip);	/* try once to free text */
	if (ip->i_flag&ITEXT && ip->i_nlink == 1) {
		u.u_error = ETXTBSY;
		goto out;
	}
	u.u_offset -= sizeof(struct direct);
	u.u_base = (caddr_t)&u.u_dent;
	u.u_count = sizeof(struct direct);
	u.u_dent.d_ino = 0;
	u.u_segflg = 1;
	writei(pp);
	if (u.u_error)
		goto out;
	ip->i_nlink--;
	ip->i_flag |= ICHG;

out:
	iput(ip);
out1:
	iput(pp);
}

chdir()
{
	chdirec(&u.u_cdir);
}

chroot()
{
	if (!suser())
		return;
	chdirec(&u.u_rdir);
}

chdirec(ipp)
register struct inode **ipp;
{
	register struct inode *ip;
	struct a {
		char	*fname;
	};

#ifndef	PNETDFS
	ip = namei(uchar, 0);
#else
	ip = namei(uchar, 0, NULL);
#endif
	if (ip == NULL)
		return;
	if ((ip->i_mode&IFMT) != IFDIR) {
		u.u_error = ENOTDIR;
		goto bad;
	}
	if (access(ip, IEXEC))
		goto bad;
	prele(ip);
	if (*ipp) {
		plock(*ipp);
		iput(*ipp);
	}
	*ipp = ip;
	return;

bad:
	iput(ip);
}

chmod()
{
	register struct inode *ip;
	register struct a {
		char	*fname;
		int	fmode;
	} *uap;

	uap = (struct a *)u.u_ap;
	if ((ip = owner()) == NULL)
		return;
	ip->i_mode &= ~07777;
	if (u.u_uid) {
		uap->fmode &= ~ISVTX;
		if (u.u_gid != ip->i_gid)
			uap->fmode &= ~ISGID;
	}
#ifdef	PNETDFS
	if ( ip->i_flag & IRMT ) {
		rchmod( ip, uap->fmode );
		return;
	}
#endif
	ip->i_mode |= uap->fmode&07777;
	ip->i_flag |= ICHG;
	if (ip->i_flag&ITEXT && (ip->i_mode&ISVTX)==0)
		xrele(ip);
	iput(ip);
}

chown()
{
	register struct inode *ip;
	register struct a {
		char	*fname;
		int	uid;
		int	gid;
	} *uap;

	uap = (struct a *)u.u_ap;
	if ((ip = owner()) == NULL)
		return;
#ifdef	PNETDFS
	if ( ip->i_flag & IRMT ) {
		rchown( ip, uap->uid, uap->gid );
		return;
	}
#endif
	ip->i_uid = uap->uid;
	ip->i_gid = uap->gid;
	if (u.u_uid != 0)
		ip->i_mode &= ~(ISUID|ISGID);
	ip->i_flag |= ICHG;
	iput(ip);
}

ssig()
{
	register a;
	register struct proc *p;
	struct a {
		int	signo;
		int	fun;
	} *uap;

	uap = (struct a *)u.u_ap;
	a = uap->signo;
	if (a <= 0 || a > NSIG || a == SIGKILL) {
		u.u_error = EINVAL;
		return;
	}
	u.u_rval1 = u.u_signal[a-1];
	u.u_signal[a-1] = uap->fun;
	u.u_procp->p_sig &= ~(1L<<(a-1));
	if (a == SIGCLD) {
		a = u.u_procp->p_pid;
		for (p = &proc[1]; p < (struct proc *)v.ve_proc; p++) {
			if (a == p->p_ppid && p->p_stat == SZOMB)
				psignal(u.u_procp, SIGCLD);
		}
	}
}

kill()
{
	register struct proc *p, *q;
	register arg;
	register struct a {
		int	pid;
		int	signo;
	} *uap;
	int f;

	uap = (struct a *)u.u_ap;
	if (uap->signo < 0 || uap->signo > NSIG) {
		u.u_error = EINVAL;
		return;
	}
	f = 0;
	arg = uap->pid;
	if (arg > 0)
		p = &proc[1];
	else
		p = &proc[2];
	q = u.u_procp;
	if (arg == 0 && q->p_pgrp == 0) {
		u.u_error = ESRCH;
		return;
	}
	for(; p < (struct proc *)v.ve_proc; p++) {
		if (p->p_stat == NULL)
			continue;
		if (arg > 0 && p->p_pid != arg)
			continue;
		if (arg == 0 && p->p_pgrp != q->p_pgrp)
			continue;
		if (arg < -1 && p->p_pgrp != -arg)
			continue;
		if (u.u_uid != 0 && u.u_uid != p->p_uid && u.u_procp != p)
			if (arg > 0) {
				u.u_error = EPERM;
				return;
			} else
				continue;
		f++;
		if (uap->signo)
			psignal(p, uap->signo);
		if (arg > 0)
			break;
	}
	if (f == 0)
		u.u_error = ESRCH;
}

times()
{
	register struct a {
		time_t	(*times)[4];
	} *uap;

	uap = (struct a *)u.u_ap;
	if (copyout((caddr_t)&u.u_utime, (caddr_t)uap->times, sizeof(*uap->times)))
		u.u_error = EFAULT;
	spl7();
	u.u_rtime = lbolt;
	spl0();
}

profil()
{
	register struct a {
		short	*bufbase;
		unsigned bufsize;
		unsigned pcoffset;
		unsigned pcscale;
	} *uap;

	uap = (struct a *)u.u_ap;
	u.u_prof.pr_base = uap->bufbase;
	u.u_prof.pr_size = uap->bufsize;
	u.u_prof.pr_off = uap->pcoffset;
	u.u_prof.pr_scale = uap->pcscale;
}

/*
 * alarm clock signal
 */
alarm()
{
	register struct proc *p;
	register c;
	register struct a {
		int	deltat;
	} *uap;

	uap = (struct a *)u.u_ap;
	p = u.u_procp;
	c = p->p_clktim;
	p->p_clktim = uap->deltat;
	u.u_rval1 = c;
}

/*
 * indefinite wait.
 * no one should wakeup(&u)
 */
pause()
{

	for(;;)
		sleep((caddr_t)&u, PSLEP);
}

/*
 * mode mask for creation of files
 */
umask()
{
	register struct a {
		int	mask;
	} *uap;
	register t;

	uap = (struct a *)u.u_ap;
	t = u.u_cmask;
	u.u_cmask = uap->mask & 0777;
	u.u_rval1 = t;
}

/*
 * Set IUPD and IACC times on file.
 */
utime()
{
	register struct a {
		char	*fname;
		time_t	*tptr;
	} *uap;
	register struct inode *ip;
	time_t tv[2];

	uap = (struct a *)u.u_ap;
	if (uap->tptr != NULL) {
		if (copyin((caddr_t)uap->tptr, (caddr_t)tv, sizeof(tv))) {
			u.u_error = EFAULT;
			return;
		}
	} else {
		tv[0] = time;
		tv[1] = time;
	}
#ifndef
	ip = namei(uchar, 0);
#else
	ip = namei(uchar, 0, NULL);
#endif
	if (ip == NULL)
		return;
	if (u.u_uid != ip->i_uid && u.u_uid != 0) {
		if (uap->tptr != NULL)
			u.u_error = EPERM;
		else
			access(ip, IWRITE);
	}
	if (!u.u_error) {
#ifdef	PNETDFS
		if ( ip->i_flag & IRMT ) {
			rutime( ip, &tv[0], &tv[1] );
			return;
		}
#endif
		ip->i_flag |= IACC|IUPD|ICHG;
		iupdat(ip, &tv[0], &tv[1]);
	}
	iput(ip);
}

ulimit()
{
	register struct a {
		int	cmd;
		long	arg;
	} *uap;

	register brk, stk;

	uap = (struct a *)u.u_ap;
	switch(uap->cmd) {
	case 2:
		if (uap->arg > u.u_limit && !suser())
			return;
		u.u_limit = uap->arg;
	case 1:
		u.u_roff = u.u_limit;
		break;

	case 3:
		u.u_roff = ctob((long)(NUMLOGPAGE - u.u_ssize));
		break;
	}
}
