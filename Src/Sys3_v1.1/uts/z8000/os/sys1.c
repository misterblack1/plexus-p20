#include "sys/plexus.h"
#include "sys/param.h"
#include "sys/systm.h"
#include "sys/map.h"
#include "sys/dir.h"
#include "sys/user.h"
#include "sys/proc.h"
#include "sys/buf.h"
#include "sys/reg.h"
#include "sys/inode.h"
#include "sys/seg.h"
#include "sys/acct.h"
#include "sys/var.h"
#include "sys/tty.h"
#include "sys/ioctl.h"	/* added 8/25/82 PAF */
#include "sys/conf.h"	/* added 8/25/82 PAF */
#ifdef	PNETDFS
extern	ddbug;
#endif

/*
 * exec system call, with and without environments.
 */

struct execa {
	char	*fname;
	char	**argp;
	char	**envp;
};

exec()
{
	((struct execa *)u.u_ap)->envp = NULL;
	exece();
}

#define	NCABLK	((NCARGS+BSIZE-1)/BSIZE)
exece()
{
	register unsigned nc;
	register char *cp;
	register struct buf *bp;
	register struct execa *uap;
	int na, ne, ucp, ap, c;
	unsigned bno;
	struct inode *ip;
	extern struct inode *gethead();

	if ((ip = gethead()) == NULL)
		return;
	bp = 0;
	na = nc = ne = 0;
	uap = (struct execa *)u.u_ap;
	/* collect arglist */
#ifndef BIGBLK
	if ((bno = malloc(swapmap,NCABLK)) == 0)
#else
	if ((bno = malloc(swapmap,ctod((int) btoc(NCARGS+BSIZE)))) == 0)
#endif
		panic("Out of swap");
	if (uap->argp) for (;;) {
		ap = NULL;
		if (uap->argp) {
			ap = fuword((caddr_t)uap->argp);
			uap->argp++;
		}
		if (ap==NULL && uap->envp) {
			uap->argp = NULL;
			if ((ap = fuword((caddr_t)uap->envp)) == NULL)
				break;
			uap->envp++;
			ne++;
		}
		if (ap==NULL)
			break;
		na++;
		if (ap == -1)
			u.u_error = EFAULT;
		do {
			if (nc >= NCARGS-1)
				u.u_error = E2BIG;
			if ((c = fubyte((caddr_t)ap++)) < 0)
				u.u_error = EFAULT;
			if (u.u_error)
				goto bad;
			if ((nc&BMASK) == 0) {
				if (bp)
					bdwrite(bp);
#ifndef BIGBLK
				bp = getblk(swapdev, swplo+bno+(nc>>BSHIFT));
#else
				bp = getblk(swapdev, dbtofsb(clrnd(swplo+bno))+
					(nc>>BSHIFT));
#endif
				cp = NULL;
			}
			nc++;
			biputc(paddr(bp), cp++, c);
		} while (c>0);
	}
	if (bp)
		bdwrite(bp);
	bp = 0;
	nc = (nc + NBPW-1) & ~(NBPW-1);
	getxfile(ip, nc + sizeof(char *)*na);
	if (u.u_error) {
		psignal(u.u_procp, SIGKILL);
		goto bad;
	}
	if (u.u_exdata.ux_mag == 0405)
		goto bad;

	/* copy back arglist */

	ucp = USRSTACK - nc - NBPW;
	ap = ucp - na*NBPW - 3*NBPW;
	u.u_ar0[R15] = ap;
	suword((caddr_t)ap, na-ne);
	nc = 0;
	for (;;) {
		ap += NBPW;
		if (na==ne) {
			suword((caddr_t)ap, 0);
			ap += NBPW;
		}
		if (--na < 0)
			break;
		suword((caddr_t)ap, ucp);
		do {
			if ((nc&BMASK) == 0) {
				if (bp)
					brelse(bp);
#ifndef BIGBLK
				bp = bread(swapdev, swplo+bno+(nc>>BSHIFT));
#else
				bp = bread(swapdev, dbtofsb(clrnd(swplo+bno))+
					(nc>>BSHIFT));
#endif
				bp->b_flags |= B_AGE;
				bp->b_flags &= ~B_DELWRI;
				cp = NULL;
			}
			subyte((caddr_t)ucp++, (c = bigetc(paddr(bp), cp++)));
			nc++;
		} while (c&0377);
	}
	suword((caddr_t)ap, 0);
	suword((caddr_t)ucp, 0);
	setregs();
	if (bp)
		brelse(bp);
	iput(ip);
#ifndef BIGBLK
	mfree(swapmap, NCABLK, bno);
#else
	mfree(swapmap, ctod((int) btoc(NCARGS+BSIZE)), bno);
#endif
	return;
bad:
	if (bp)
		brelse(bp);
	iput(ip);
#ifndef BIGBLK
	for (nc = 0; nc < NCABLK; nc++) {
		bp = getblk(swapdev, swplo+bno+nc);
		bp->b_flags |= B_AGE;
		bp->b_flags &= ~B_DELWRI;
		brelse(bp);
	}
	mfree(swapmap, NCABLK, bno);
#else
	for (nc = 0; nc < (ctod((int) btoc(NCARGS+BSIZE))); nc++) {
		bp = getblk(swapdev, dbtofsb(swplo+bno+nc));
		bp->b_flags |= B_AGE;
		bp->b_flags &= ~B_DELWRI;
		brelse(bp);
	}
	mfree(swapmap, ctod((int) btoc(NCARGS+BSIZE)), bno);
#endif
}

struct inode *
gethead()
{
	register struct inode *ip;
	register unsigned ds, ts;
	register sep;

#ifndef	PNETDFS
	if ((ip = namei(uchar, 0)) == NULL)
#else
	if ((ip = namei(uchar, 0, NULL)) == NULL)
#endif
		return(NULL);
	if (access(ip, IEXEC) ||
	   (ip->i_mode & IFMT) != IFREG ||
	   (ip->i_mode & (IEXEC|(IEXEC>>3)|(IEXEC>>6))) == 0) {
		u.u_error = EACCES;
		goto bad;
	}
	/*
	 * read in first few bytes of file for segment sizes
	 * ux_mag = 407/410/411/405
	 *  407 is plain executable
	 *  410 is RO text
	 *  411 is separated ID
	 *  405 is overlaid text
	 */
	u.u_base = (caddr_t)&u.u_exdata;
	u.u_count = sizeof(u.u_exdata);
	u.u_offset = 0;
	u.u_segflg = 1;
	readi(ip);
	u.u_segflg = 0;
	if (u.u_count!=0)
		u.u_exdata.ux_mag = 0;
	sep = 0;
	if (u.u_exdata.ux_mag == 0407) {
		ds = btoc((long)u.u_exdata.ux_tsize +
			(long)u.u_exdata.ux_dsize +
			(long)u.u_exdata.ux_bsize);
		ts = 0;
		u.u_exdata.ux_dsize += u.u_exdata.ux_tsize;
		u.u_exdata.ux_tsize = 0;
	} else {
		ts = btoc(u.u_exdata.ux_tsize);
		ds = btoc(u.u_exdata.ux_dsize+u.u_exdata.ux_bsize);
		if ((ip->i_flag&ITEXT)==0 && ip->i_count!=1)
			u.u_error = ETXTBSY;
		if (u.u_exdata.ux_mag == 0411)
			sep++;
		else if (u.u_exdata.ux_mag == 0405) {
			if (u.u_sep==0 && ctos(ts) != ctos(u.u_tsize))
				u.u_error = ENOMEM;
			goto bad;
		}
		else if (u.u_exdata.ux_mag != 0410) {
			u.u_error = ENOEXEC;
			goto bad;
		}
	}

	/* The following check is for programs that are too large for the */
	/*  address space available.  Originally, the maximum size for */
	/*  the stack included the maximum size for arguments */
	/*  (SSIZE+btoc(NCARGS-1)), but this limits the data space for */
	/*  programs to 0xe000 when NCARGS is 5120.  This means that */
	/*  some programs will be able to start execution, but will fail */
	/*  after execution starts (it will allow programs that don't use */
	/*  much in the way of arguments or use much stack space to run). */

	checkur(ts,ds,SSIZE,sep);
bad:
	if (u.u_error) {
		iput(ip);
		ip = NULL;
	}
	return(ip);
}

/*
 * Read in and set up memory for executed file.
 */
getxfile(ip, nargc)
register struct inode *ip;
{
	register unsigned ds;
	register i;

	xfree();
	u.u_tsize = btoc(u.u_exdata.ux_tsize);
	if (u.u_exdata.ux_mag == 0405) {
		xalloc(ip);
		u.u_ar0[PC] = u.u_exdata.ux_entloc & ~01;
	} else {
		u.u_prof.pr_scale = 0;
		u.u_ssize = SSIZE + btoct(nargc+SAFETYZONELEN);
		u.u_dsize = btoc(u.u_exdata.ux_dsize + u.u_exdata.ux_bsize);
		i = USIZE+u.u_dsize+u.u_ssize;
		expand(i);
		ds = USIZE+(btoct(u.u_exdata.ux_dsize));
		while (--i >= ds)
			clearseg(u.u_procp->p_addr+i);
		xalloc(ip);
		/* read in data segment */
		estabur((unsigned)0, u.u_dsize, (unsigned)0, 0, RO);
		u.u_base = 0;
		u.u_offset = sizeof(u.u_exdata)+u.u_exdata.ux_tsize;
		u.u_count = u.u_exdata.ux_dsize;
		readi(ip);
		if (u.u_count!=0)
			u.u_error = EFAULT;
		/*
		 * set SUID/SGID protections, if no tracing
		 */
		if ((u.u_procp->p_flag&STRC)==0) {
			if (ip->i_mode&ISUID)
				u.u_uid = ip->i_uid;
			if (ip->i_mode&ISGID)
				u.u_gid = ip->i_gid;
		} else
			psignal(u.u_procp, SIGTRAP);
		u.u_sep = (u.u_exdata.ux_mag == 0411)?1:0;
	}
	estabur(u.u_tsize, u.u_dsize, u.u_ssize,u.u_sep,RO);
}

/*
 * Clear registers on exec
 */
setregs()
{
	register int *rp;
	register char *cp;
	register i;

	for (rp = &u.u_signal[0]; rp < &u.u_signal[NSIG]; rp++)
		if ((*rp & 1) == 0)
			*rp = 0;
	for (cp = &regloc[0]; cp < &regloc[15];)
		u.u_ar0[*cp++] = 0;
	u.u_ar0[PC] = u.u_exdata.ux_entloc & ~01;
	u.u_ar0[PCSEG] = udseg;
	u.u_abreq = NULL;	
	for (i=0; i<NOFILE; i++) {
		if ((u.u_pofile[i]&EXCLOSE) && u.u_ofile[i] != NULL) {
			closef(u.u_ofile[i]);
			u.u_ofile[i] = NULL;
		}
	}
#ifdef	PNETDFS
	if ( u.u_dfsvc ) {
if ( ddbug ) {
		printf( "exit calling nclose/ndetach %x\n", u.u_dfsvc );
}
		nclose( u.u_dfsvc );
		ndetach( (u.u_dfsvc & 0xff00) );
		u.u_dfsvc = 0;
	}
#endif
	/*
	 * Remember file name for accounting.
	 */
	u.u_acflag &= ~AFORK;
	bcopy((caddr_t)u.u_dent.d_name, (caddr_t)u.u_comm, DIRSIZ);
}

/*
 * exit system call:
 * pass back caller's arg
 */
rexit()
{
	register struct a {
		int	rval;
	} *uap;

	uap = (struct a *)u.u_ap;
	exit((uap->rval & 0377) << 8);
}

/*
 * Release resources.
 * Enter zombie state.
 * Wake up parent and init processes,
 * and dispose of children.
 */
exit(rv)
{
	register int i;
	register struct proc *p, *q;

	p = u.u_procp;
	p->p_flag &= ~(STRC);
	p->p_clktim = 0;
	for (i=0; i<NSIG; i++)
		u.u_signal[i] = 1;
	expand(USIZE);
	if ((p->p_pid == p->p_pgrp)
	 && (u.u_ttyp != NULL)
	 && (u.u_ttyp->t_pgrp == p->p_pgrp)) {
		u.u_ttyp->t_pgrp = 0;
		/* added so that pgrp in icp is also set to 0 PAF 8/25/82 */
		(*cdevsw[major(u.u_ttyd)].d_ioctl)(minor(u.u_ttyd),CLRPGRP,0,0);
		signal(p->p_pgrp, SIGHUP);
	}
	p->p_pgrp = 0;
	for (i=0; i<NOFILE; i++) {
		if (u.u_ofile[i] != NULL)
			closef(u.u_ofile[i]);
	}
#ifdef	PNETDFS
	if ( u.u_dfsvc ) {
if ( ddbug ) {
		printf( "exit calling nclose/ndetach %x\n", u.u_dfsvc );
}
		nclose( u.u_dfsvc );
		ndetach( (u.u_dfsvc & 0xff00) );
		u.u_dfsvc = 0;
	}
#endif
	plock(u.u_cdir);
	iput(u.u_cdir);
	if (u.u_rdir) {
		plock(u.u_rdir);
		iput(u.u_rdir);
	}
	xfree();
	acct(rv);
	mfree(coremap, p->p_size, p->p_addr);
	p->p_stat = SZOMB;
	((struct xproc *)p)->xp_xstat = rv;
	((struct xproc *)p)->xp_utime = u.u_cutime + u.u_utime;
	((struct xproc *)p)->xp_stime = u.u_cstime + u.u_stime;
	for (q = &proc[1]; q < (struct proc *)v.ve_proc; q++) {
		if (p->p_pid == q->p_ppid) {
			q->p_ppid = 1;
			if (q->p_stat == SZOMB)
				psignal(&proc[1], SIGCLD);
			if (q->p_stat == SSTOP)
				setrun(q);
		} else
		if (p->p_ppid == q->p_pid)
			psignal(q, SIGCLD);
		if (p->p_pid == q->p_pgrp)
			q->p_pgrp = 0;
	}
	resume(proc[0].p_addr, u.u_qsav);
	/* no deposit, no return */
}

/*
 * Wait system call.
 * Search for a terminated (zombie) child,
 * finally lay it to rest, and collect its status.
 * Look also for stopped (traced) children,
 * and pass back status from them.
 */
wait()
{
	register f;
	register struct proc *p;

loop:
	f = 0;
	for (p = &proc[1]; p < (struct proc *)v.ve_proc; p++)
	if (p->p_ppid == u.u_procp->p_pid) {
		f++;
		if (p->p_stat == SZOMB) {
			freeproc(p, 1);
			return;
		}
		if (p->p_stat == SSTOP) {
			if ((p->p_flag&SWTED) == 0) {
				p->p_flag |= SWTED;
				u.u_rval1 = p->p_pid;
				u.u_rval2 = (fsig(p)<<8) | 0177;
				return;
			}
			continue;
		}
	}
	if (f) {
		sleep((caddr_t)u.u_procp, PWAIT);
		goto loop;
	}
	u.u_error = ECHILD;
}

/*
 * Remove zombie children from the process table.
 */
freeproc(p, flag)
register struct proc *p;
{
	if (flag) {
		u.u_rval1 = p->p_pid;
		u.u_rval2 = ((struct xproc *)p)->xp_xstat;
	}
	u.u_cutime += ((struct xproc *)p)->xp_utime;
	u.u_cstime += ((struct xproc *)p)->xp_stime;
	p->p_stat = NULL;
	p->p_pid = 0;
	p->p_ppid = 0;
	p->p_sig = 0;
	p->p_flag = 0;
	p->p_wchan = 0;
}

/*
 * fork system call.
 */
fork()
{
	register n;
	register struct proc *p1, *p2;
	register a;

	/*
	 * Make sure there's enough swap space for max
	 * core image, thus reducing chances of running out
	 */
	if ((a = malloc(swapmap, ctod(MAXMEM))) == 0) {
		u.u_error = ENOMEM;
		goto out;
	}
	mfree(swapmap, ctod(MAXMEM), a);
	a = 0;
	p1 = &proc[0];
	p2 = NULL;
	n = v.v_proc;
	do {
		if (p1->p_stat==NULL && p2==NULL)
			p2 = p1;
		else {
			if (p1->p_uid==u.u_uid && p1->p_stat!=NULL)
				a++;
		}
		p1++;
	} while (--n);
	/*
	 * Disallow if
	 *  No processes at all;
	 *  not su and too many procs owned; or
	 *  not su and would take last slot.
	 */
	if (p2==NULL || (u.u_uid!=0 && (p2==&proc[v.v_proc-1] || a>v.v_maxup))) {
		u.u_error = EAGAIN;
		goto out;
	}
	if (newproc()) {
		u.u_rval1 = p2->p_ppid;
		u.u_start = time;
		u.u_ticks = lbolt;
		u.u_mem = p2->p_size;
		u.u_ior = u.u_iow = u.u_ioch = 0;
		u.u_cstime = 0;
		u.u_stime = 0;
		u.u_cutime = 0;
		u.u_utime = 0;
		u.u_acflag = AFORK;
		return;
	}
	u.u_rval1 = p2->p_pid;

out:
	u.u_ar0[PC] += NBPW;
}

/*
 * break system call.
 *  -- bad planning: "break" is a dirty word in C.
 */
sbreak()
{
	struct a {
		unsigned nsiz;
	};
	register a, n, d;
	int i;

	/*
	 * set n to new data size
	 * set d to new-old
	 * set n to new total size
	 */

	n = btoc(((struct a *)u.u_ap)->nsiz);
	if (!u.u_sep)
		n -= ctos(u.u_tsize) * stoc(1);
	if (n < 0)
		n = 0;
	d = n - u.u_dsize;
	n += USIZE+u.u_ssize;
	if (estabur(u.u_tsize, u.u_dsize+d, u.u_ssize, u.u_sep, RO))
		return;
	u.u_dsize += d;
	if (d > 0)
		goto bigger;
	a = u.u_procp->p_addr + n - u.u_ssize;
	i = n;
	n = u.u_ssize;
	while (n--) {
		copyseg(a-d, a);
		a++;
	}
	expand(i);
	return;

bigger:
	expand(n);
	a = u.u_procp->p_addr + n;
	n = u.u_ssize;
	while (n--) {
		a--;
		copyseg(a-d, a);
	}
	while (d--)
		clearseg(--a);
}

/*
 * ugrow system call.
 * This call was added to allow a user program to expand its stack since
 * the system cannot automatically do this.
 */

ugrow()
{
	register struct a {
		caddr_t	stkend;
	} *uap;

	uap = (struct a *) u.u_ap;
	grow(uap->stkend);
}

