/*
 * Synchronous terminal interface for the PDP 11.
 */

#include	"sys/param.h"
#include	"sys/st.h"
#include	"sys/dir.h"
#include	"sys/map.h"
#include	"sys/user.h"
#include	"sys/proc.h"
#include	"sys/buf.h"

#define NULL	0
#define	STMPRI	(PZERO + 1)		/* allow interrupted reads */
#define stid(X)	(unsigned)((X->sm_sta << 8) | X->sm_dev)

extern	struct	st	st_st[];	/* sync channels */
extern	int	st_cnt;			/* number of channels */

struct	map	stomap[STMAP];		/* map for alloc of stbuf */
struct	map	stimap[STMAP];		/* map for alloc of stbuf */
struct  stbhdr	sthdrb[STHBUF];		/* header buffer pool */
struct  buf	sthdr;			/* mask for bigetc use */

unsigned	wrccnt;
int	stoflg, sthflg;

unsigned  stobuf;
unsigned  stibuf;
long stibase, stobase;
struct stbhdr *ffree;
unsigned sthdcnt = 0;
unsigned stopflg;
unsigned ttyid[NKMC];

stopen(dev)
int dev;
{
	register  struct  st  *dp;
	register  struct  stmsghdr  *hp;
	register  int	*ip;

	dp = &st_st[dev];
	if(dev >= st_cnt) {
		u.u_error = ENXIO;
		return;
	}
	spl5();
	if ((dp->s_flags & CH_OPEN) == NULL) {
		ip = (int *)dp;
		while (ip!=(int *)(((caddr_t)dp)+sizeof(struct st)))
			*ip++ = 0;
		hp = &dp->s_hdr;
		hp->s_act = 0;
		hp->s_max = NSTCHQ;
		hp->s_first = hp->s_last = 0;
		if (dev < STNDSCH) {
			dp->s_flags = CH_OPEN;
			dp->s_ttyid = (unsigned) ( -(dev + 1));
			dp->s_mode = STRAW;
		} else {
			dp->s_flags |= (CH_OPEN|CH_AVAIL);
			while(dp->s_flags & CH_AVAIL)
				sleep((caddr_t)dp,STMPRI);
			dp->s_mode = STRAW;
		}
	}
	spl0();
	if ((dp->s_mode == 0) && (dev >= STNDSCH)) {
		u.u_error = ENXIO;
		return;
	}
}

stclose(dev)
int dev;
{
	register  struct  st	*dp;
	register  struct  stbhdr	*mp;
	extern    struct  stbhdr	*getmsg();

	dp = &st_st[dev];
	spl5();
	if (dp->s_rbuf)
		stfree((unsigned) stimap,dp->s_rbuf);
	while(mp = getmsg(&dp->s_hdr))
		stfree((unsigned)stimap, mp);
	dp->s_flags = NULL;
	spl0();
}

stwrite(dev)
int dev;
{
	register struct st	*dp;
	register unsigned	loc;

	dp = &st_st[dev];
	while((loc=malloc(stomap,btoc(u.u_count))) == NULL) {
		stoflg++;
		sleep((caddr_t)stomap,STMPRI);
	}
	pimove(ctob((paddr_t) loc),wrccnt=u.u_count,B_WRITE);
	while(putmsg(loc,wrccnt,wrccnt,dp) == NULL) {
		sthflg++;
		sleep((caddr_t)&sthflg,STMPRI);
	}
}

stread(dev)
int dev;
{
	register  struct  st   *dp;
	register struct  stbhdr  *mp;
	extern    struct  stbhdr  *getmsg();
	unsigned n, cnt;

	dp = &st_st[dev];
	spl5();
	while(((dp->s_hdr).s_act==0)&&(dp->s_rbuf==NULL))
	     sleep((caddr_t)dp, STMPRI);
	spl0();
	while((cnt=u.u_count) && u.u_error == 0) {
		if (dp->s_rbuf == NULL)
			if ((dp->s_rbuf = getmsg(&dp->s_hdr)) == NULL)
				return;
			else
				dp->s_roffset = 0;
		mp = dp->s_rbuf;
		pimove(mp->sm_loc + dp->s_roffset,
		    n=min(cnt,mp->sm_count - dp->s_roffset),B_READ);
		if (n == (mp->sm_count - dp->s_roffset)) {
			stfree((unsigned)stimap,mp);
			dp->s_rbuf = NULL;
		}
		else
			dp->s_roffset += n;
		continue;
	}
}

strxint(sel0,sel2,sel4)
unsigned sel0, sel2;
register struct stbhdr *sel4;
{

	switch(sel2) {

	case RRTNXBUF:
		if (sel4->sm_type & FAIL)
			printf("stxmit fail\n");
		stfree((unsigned)stomap,sel4);
		break;

	case RRTNEBUF:
		stfree((unsigned)stimap,sel4);
		break;

	case RRTNRBUF:
		strcvb(sel0);
		stinput(sel0,sel4);
		break;
	}
}

stioctl(dev,cmd,arg,mode)
{
	register struct st *dp;
	struct set {
		char s_spa;
		char s_ssa;
		char typet;
		char s_dev;
		unsigned mode;
	} setbuff;
	register struct set *sp = &setbuff;
	extern struct stbhdr *stghdr();
	register unsigned i;
	unsigned base;

	dp = &st_st[dev];
	if (dev < STNDSCH) {
		if (copyin(arg,sp,sizeof setbuff)) {
			u.u_error = EFAULT;
			return;
		}
		dp->s_port = sp->s_dev;
		if (sp->mode) {
			vpmstop(sp->s_dev);
			stopflg |= (1 << (sp->s_dev & 07));
			return;
		}
		for (i=STOBSZ; i; i>>=1)
			while (malloc(stomap,i));
		for (i=STIBSZ; i; i>>=1)
			while (malloc(stimap,i));
		sthdcnt = 0;
		stinithd();
		if((stobuf=malloc(coremap,btoc(STOBSZ)+btoc(STIBSZ)))==NULL) {
			u.u_error = ENOMEM;
			return;
		}
		stibuf = stobuf + btoc(STOBSZ);
		sthdr.b_flags |= B_PHYS;
		sthdr.b_bcount = STOBSZ;
		sthdr.b_paddr = ctob((paddr_t)stobuf);
		mfree((unsigned)stomap, btoc(STOBSZ), stobuf);
		base = ubmalloc(STOBSZ);
		stobase = ubmaddr(&sthdr,base);
		mfree((unsigned)stimap,btoc(STIBSZ),stibuf);
		sthdr.b_paddr = ctob((paddr_t)stibuf);
		sthdr.b_bcount = STIBSZ;
		base = ubmalloc(STIBSZ);
		stibase = ubmaddr(&sthdr,base);
		if (u.u_error = vpmstart(sp->s_dev,STRM,strxint))
			return;
		stopflg &= ~(1 << (sp->s_dev & 07));
		strcvb(sp->s_dev);
	}
}
stinput(sel0,mp)
register struct stbhdr *mp;
unsigned sel0;
{
	register  struct  st	*cp, *ap;
	struct	st	*ep;
	int	sps;

	ap = cp = NULL;
	ep = &st_st[st_cnt];
	sps = spl5();
	if (mp->sm_type & FBLOCK)
		ttyid[sel0 & 07] = stid(mp);
	sthdr.b_paddr = mp->sm_loc;
	for(cp = &st_st[1]; cp != ep; cp++) {
		if(cp->s_ttyid == ttyid[sel0 & 07] &&
		    cp->s_port == sel0 &&
		    (cp->s_flags & (CH_OPEN | CH_AVAIL)) == CH_OPEN)
			break;
		if(!ap && cp->s_flags & CH_AVAIL)
			ap = cp;
	}
	if(cp != ep) {
		if(putmesgi(&cp->s_hdr, mp)) {
			if ((mp->sm_type & LBLOCK) ||
			    ((cp->s_hdr).s_max <= (cp->s_hdr).s_act))
				wakeup((caddr_t)cp);
			splx(sps);
			return(NULL);
		}
	} else	if(cp == ep && ap) {
		if(putmesgi(&ap->s_hdr, mp)) {
			ap->s_flags &= ~CH_AVAIL;
			ap->s_ttyid = ttyid[sel0 & 07];
			ap->s_port = sel0;
			if ((mp->sm_type & LBLOCK) ||
			    ((ap->s_hdr).s_max <= (ap->s_hdr).s_act))
				wakeup((caddr_t)ap);
			splx(sps);
			return(NULL);
		}
	}
	stfree((unsigned)stimap,mp);
	splx(sps);
	return(ENOSPC);
}

static
putmsg(loc, size, count, cp)
register struct  st    *cp;
unsigned  loc, size, count;
{
	register struct stbhdr *dp;
	extern struct stbhdr *stghdr();
	paddr_t sttemp;

	if((dp = stghdr()) == 0)
		return(NULL);
	sttemp = stobase + ctob((paddr_t) (loc - stobuf));
	dp->sm_locu = (sttemp>>16) & 03;
	dp->sm_locl = sttemp & 0177777;
	dp->sm_loc = ctob((paddr_t) loc);
	dp->sm_size = size;
	dp->sm_count = count;
	dp->sm_sta = (cp->s_ttyid >> 8) & 0377;
	dp->sm_dev = cp->s_ttyid & 0377;
	dp->sm_type = 0;
	vpmxmtq((unsigned) cp->s_port,dp);
	return(1);
}

static
putmesgi(hdr,mp)
register struct stmsghdr *hdr;
register struct stbhdr   *mp;
{

	register struct stbhdr *mp2;

	if (hdr->s_act == hdr->s_max) {
		return(NULL);
	}
	hdr->s_act++;
	if (hdr->s_first == NULL)
		hdr->s_first = mp;
	if (mp2 = hdr->s_last)
		mp2->sm_nxt = mp;
	hdr->s_last = mp;
	return(1);
}

static
struct  stbhdr  *
getmsg(hdr)
register  struct  stmsghdr  *hdr;
{
	register  struct  stbhdr  *dp;

	if(hdr->s_act) {
		if (--hdr->s_act == 0)
			hdr->s_last = 0;
		dp = hdr->s_first;
		hdr->s_first = dp->sm_nxt;
		return(dp);
	}
	else
		return(NULL);
}

static
stfree(map,mp)
unsigned map;
register struct stbhdr *mp;
{
	register int sps;

	sps = spl5();
	mfree(map,btoc(mp->sm_size),ptoc(mp->sm_loc));
	stphdr(mp);
	if (stoflg) {
		stoflg = 0;
		wakeup((caddr_t)stomap);
	}
	splx(sps);
}

stinithd()
{
	register struct stbhdr *bp;

	ffree = NULL;
	for (bp=sthdrb;bp < &sthdrb[STHBUF];bp++)
		stphdr(bp);
}

stphdr(bp)
register struct stbhdr *bp;
{
	register int sps;

	sps = spl5();
	bp->sm_nxt = ffree;
	ffree = bp;
	sthdcnt++;
	if (sthflg) {
		sthflg = 0;
		wakeup((caddr_t) &sthflg);
	}
	splx(sps);
	return(0);
}

struct stbhdr *
stghdr()
{
	register struct stbhdr *bp;
	register int sps;

	sps = spl5();
	if (ffree == NULL) {
		splx(sps);
		return(0);
	}
	bp = ffree;
	ffree = bp->sm_nxt;
	bp->sm_nxt = 0;
	sthdcnt--;
	splx(sps);
	return(bp);
}

strcvb(sel0)
unsigned sel0;
{
	unsigned mem;
	struct stbhdr *bp;
	paddr_t sttemp;

	if (stopflg & (1 << (sel0 & 07)))
		return;
	while(vpmemptq(sel0,(struct stbhdr *)0) < RCVLEN) {
		bp = NULL;
		if ((bp=stghdr()) && (mem=malloc(stimap,STMBSZ>>6))) {
			bp->sm_count = bp->sm_size = STMBSZ;
			bp->sm_type = 0;
			sttemp = stibase + ctob((paddr_t)mem - stibuf);
			bp->sm_locu = (sttemp>>16) & 03;
			bp->sm_locl = sttemp & 0177777;
			bp->sm_loc = ctob((paddr_t) mem);
			vpmemptq(sel0,bp);
		} else {
			if (bp)
				stphdr(bp);
			timeout(strcvb,sel0,60);
			break;
		}
	}
}
