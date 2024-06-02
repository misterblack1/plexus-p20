#include "sys/param.h"
#include "sys/systm.h"
#include "sys/buf.h"
#include "sys/conf.h"
#include "sys/map.h"
#include "sys/utsname.h"
#include "sys/elog.h"
#include "sys/erec.h"
#include "sys/err.h"
#include "sys/iobuf.h"
#include "sys/var.h"

errinit()
{
	if(err.e_nslot)
		mfree(err.e_map,err.e_nslot,1);
	err.e_org = err.e_ptrs;
	err.e_nxt = err.e_ptrs;
}
struct errhdr *
geteslot(size)
{
	register ns, *p;
	register struct errhdr *ep;
	int n, sps;

	ns = (size+sizeof(struct errhdr)+sizeof(struct errslot)-1)
		/sizeof(struct errslot);
	sps = spl7();
	n = malloc(err.e_map,ns);
	splx(sps);
	if(n == 0)
		return(NULL);
	ep = (struct errhdr *)(&err.e_slot[--n]);
	ns *= sizeof(struct errslot)/sizeof(int);
	p = (int *)ep;
	do {
		*p++ = 0;
	} while(--ns);
	ep->e_len = size + sizeof(struct errhdr);
	return(++ep);
}

freeslot(ep)
register struct errhdr *ep;
{
	register ns, sps;

	ns = (ep->e_len+sizeof(struct errslot)-1)/sizeof(struct errslot);
	sps = spl7();
	mfree(err.e_map,ns,(((struct errslot *)ep)-err.e_slot)+1);
	splx(sps);
}

struct errhdr *
geterec()
{
	register sps;
	register struct errhdr *ep;

	sps = spl7();
	while(*err.e_org == NULL)
		sleep(&err.e_org,PZERO+1);
	ep = *err.e_org;
	*err.e_org++ = NULL;
	if(err.e_org >= &err.e_ptrs[err.e_nslot])
		err.e_org = err.e_ptrs;
	splx(sps);
	return(ep);
}

puterec(ep,type)
register struct errhdr *ep;
{
	register sps;

	(--ep)->e_type = type;
	ep->e_time = time;
	sps = spl7();
	*err.e_nxt++ = ep;
	if(err.e_nxt >= &err.e_ptrs[err.e_nslot])
		err.e_nxt = err.e_ptrs;
	splx(sps);
	wakeup(&err.e_org);
}

logstart()
{
	register sps;
	register struct estart *ep;
	register struct bdevsw *bdp;

	sps = spl7();
	for(err.e_org = &err.e_ptrs[err.e_nslot-1]; err.e_org >= err.e_ptrs; err.e_org--)
		if(*err.e_org != NULL) {
			freeslot(*err.e_org);
			*err.e_org = NULL;
		}
	err.e_org = err.e_ptrs;
	err.e_nxt = err.e_ptrs;
	ep = (struct estart *)geteslot(sizeof(struct estart));
	splx(sps);
	if(ep == NULL)
		return;
	ep->e_cpu = ctc0tim;
	ep->e_syssize = sysmem;
	ep->e_name = utsname;
	for(bdp = &bdevsw[bdevcnt-1]; bdp >= bdevsw; bdp--)
		if(bdp->d_tab != NULL)
			ep->e_bconf |= 1<<major((bdp->d_tab->b_dev));
	puterec(ep,E_GOTS);
}

logtchg(nt)
time_t nt;
{
	register struct etimchg *ep;

	if((ep = (struct etimchg *)geteslot(sizeof(struct etimchg))) != NULL) {
		ep->e_ntime = nt;
		puterec(ep,E_TCHG);
	}
}

logstray(addr)
physadr addr;
{
	register struct estray *ep;

	if((ep = (struct estray *)geteslot(sizeof(struct estray))) != NULL) {
		ep->e_saddr = addr;
		ep->e_sbacty = blkacty;
		puterec(ep,E_STRAY);
	}
}

logecc(syndrome,bank)
char syndrome, bank;
{
	register struct eccerr *ep;
	register n;

	if((ep = (struct eccerr *)geteslot(sizeof(struct eccerr))) != NULL) {
		ep->e_syndrome = syndrome;
		ep->e_bank = bank;
		puterec(ep,E_ECC);
	}
}

fmtberr(dp,cyl)
register struct iobuf *dp;
{
	register struct eblock *ep;
	register struct buf *bp;
	register n;
	register short *p;
	struct br {
		struct eblock	eb;
		short		cregs[1];
	};
	struct	iostat	*iosp;
	physadr	addr;

	if(dp->io_erec != NULL) {
		dp->io_erec->e_rtry++;
		return;
	}
	iosp = dp->io_stp;
	if(dp->io_addr == NULL || (ep = (struct eblock *)geteslot(sizeof(struct eblock)
		+(dp->io_nreg*sizeof(short)))) == NULL) {
		iosp->io_unlog++;
		return;
	}
	n = major(dp->b_dev);
	bp = dp->b_actf;
	ep->e_dev = makedev(n,(bp==NULL)?minor(dp->b_dev):minor(bp->b_dev));
	ep->e_regloc = addr = dp->io_addr;
	ep->e_bacty = blkacty;
	ep->e_stats.io_ops = iosp->io_ops;
	ep->e_stats.io_misc = iosp->io_misc;
	ep->e_stats.io_unlog = iosp->io_unlog;
	if(bp != NULL) {
		ep->e_bflags = (bp->b_flags&B_READ) ? E_READ : E_WRITE;
		if(bp->b_flags & B_PHYS)
			ep->e_bflags |= E_PHYS;
		if(bp->b_flags & B_MAP)
			ep->e_bflags |= E_MAP;
		ep->e_bnum = bp->b_blkno;
		ep->e_bytes = bp->b_bcount;
		ep->e_memadd = paddr(bp);
	}
	else
		ep->e_bflags = E_NOIO;
	ep->e_cyloff = cyl;
	ep->e_nreg = dp->io_nreg;
	p = (short *)(&((struct br *)ep)->cregs[0]);
	n = dp->io_nreg;
	while(--n >= 0) {
		*p++ = addr->r[0];
		addr++;
	}
	dp->io_erec = ep;
}

logberr(dp,err)
register struct iobuf *dp;
{
	register struct eblock *ep;

	if((ep = dp->io_erec) == NULL)
		return;
	if(err)
		ep->e_bflags |= E_ERROR;
	puterec(ep,E_BLK);
	dp->io_erec = NULL;
}
