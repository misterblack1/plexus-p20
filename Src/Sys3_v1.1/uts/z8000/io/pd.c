#include "sys/param.h"
#include "sys/plexus.h"
#include "sys/buf.h"
#include "sys/iobuf.h"
#include "sys/conf.h"
#include "sys/dir.h"
#include "sys/user.h"
#include "sys/disk.h"
#include "sys/pd.h"
#include "sys/imsc.h"
#include "sys/systm.h"
#include "sys/elog.h"
#include "sys/utsname.h"

#define DISKCAB 0xfe00
#define IMSCWU 0x1f

/* this def must match the one in low.s */
#define DISKBIT 1

struct iostat pdstat[4];
struct iobuf pdtab = tabinit(PD0,pdstat);
struct buf rpdbuf;

short imscint;

char pdiniterr[PDUNITS];

pdinit()
{
  register short savepri;
  register short unit;
  register short i;
  struct block0 block0;
  static pdinitdone = 0;
  register cab_t *WUB;

  if (pdinitdone++)
    goto done;

  savepri=spl7();
  WUB = (cab_t *)DISKCAB;
  pdgo(0,0);
  pdwaitdisk();
  if (WUB->cab_flags == B_CABGO)
  {
	/* no imsc */
	for (unit=0;unit<PDUNITS;unit++)
		pdiniterr[unit] = TRUE;
  }
  else
  {
	if (pd_sizes[PDLOGVOL].nblks == 0)
		copyio(SDSEG, &pd_sizes[PDLOGVOL], &pd_sizes[0],
			sizeof(struct diskconf)*PDLOGVOL*(PDUNITS-1), 2);
	for (unit = 0; unit < PDUNITS; unit++) {
		def_pddisk(unit, &pd_init[unit]);
/*		seek_disk(unit,0L);
		pdiniterr[unit] = pdwaitdisk(); */
		rw_disk(unit,CAB_READ,0L,(long)sizeof(struct block0),
			segtomb(SDSEG), &block0);
		pdiniterr[unit] = pdwaitdisk();
		if ( (block0.id == 'pd') && !pdiniterr[unit]) {
			def_pddisk(unit, &block0);
			if (block0.initsize) 
				copyio(SDSEG, &pd_sizes[PDLOGVOL*unit],
					block0.confinfo,
					sizeof(struct diskconf) * PDLOGVOL, 2);
			if (block0.nodenm[0] && !numdisks)
				for (i=0; i<9; i++)
					utsname.nodename[i] = block0.nodenm[i];
			if (block0.nswap && !numdisks++) {
				rootdev = block0.rootdev;
				pipedev = block0.pipedev;
				dumpdev = block0.dumpdev;
				swapdev = block0.swapdev;
				nswap = block0.nswap;
				swplo = block0.swplo;
			}
		}
	}
  }
  out_local(P_SPIC1,in_local(P_SPIC1)&(~IDINT));
  out_local(P_MBTOINT,0);
  splx(savepri);
done:
  for (unit=0; unit<PDUNITS; unit++)
    if (pdiniterr[unit] == 0) break;
  return((unit==PDUNITS) ? -1 : 0);
}

def_pddisk(unit,tbl)
short unit;
register struct block0 *tbl;
{
  register struct cab_def *WUB;

  WUB=(struct cab_def *)DISKCAB;
  WUB->cab_disknum = unit;
  WUB->cab_sectnum = tbl->init.sectpertrack;
  WUB->cab_headnum = tbl->init.fixheads;
  WUB->cab_cylnum = tbl->init.cylinders;
  WUB->cab_altcylnum = (short)tbl->init.altcylinders;
  pdgo(CAB_MAINT,CAB_DEF);
  return(pdwaitdisk());
}

pdopen(dev,wflag)
dev_t dev;
short wflag;
{
  if ((minor(dev) >= (PDLOGVOL*PDUNITS)) ||
      pdiniterr[minor(dev)/PDLOGVOL] )
    u.u_error = ENXIO;
}

pdstrategy(bp)
register struct buf *bp;
{
  register short logvol;
  long sz;
  
  if (bp->b_flags & B_PHYS)
    mapalloc(bp);
  logvol = minor(bp->b_dev);
  sz = (bp->b_bcount + BMASK)>>BSHIFT; /* sects */
  if (logvol >= (PDLOGVOL*PDUNITS) 
	|| bp->b_blkno + sz >= pd_sizes[logvol].nblks ) {
    bp->b_flags |= B_ERROR;
    iodone(bp);
    return;
    }
  bp->av_forw = NULL;
  spl5();
  if (pdtab.b_actf == NULL)
    pdtab.b_actf = bp;
  else
    pdtab.b_actl->av_forw = bp;
  pdtab.b_actl = bp;
  if (pdtab.b_active == NULL)
    pdstart();
  spl0();
}

pdstart()
{
  register short logvol;
  register short unit;
  register short cmd;
  register struct buf *bp;
  register short savepri;


    if ((bp=pdtab.b_actf) == NULL) return;

  logvol = minor(bp->b_dev);
  unit = logvol / PDLOGVOL;

  if (pdtab.b_active == SRECAL) {
    pdgo(CAB_UTIL,CAB_DKRECAL);
    }
  else {
    pdtab.b_active = SIO;
    cmd = (bp->b_flags&B_READ) ? CAB_READ : CAB_WRITE;
    pdstat[unit].io_ops++;
    blkacty |= (1<<PD0);
    rw_disk(unit,cmd,(long)pd_sizes[logvol].blkoff+bp->b_blkno,
	(long)bp->b_bcount,bp->b_paddr);
    }
  out_local(P_PIOBDATA,in_local(P_PIOBDATA) & (~DISKLED));
}

pdintr()
{
  
  register cab_t *WUB;
  register short status;
  register struct buf *bp;
  register unsigned s;
  int i;
#ifdef DEBUG
	static inpdintr = 0;
#endif

  if ((bp=pdtab.b_actf)==NULL) {
    return;
    }

  WUB = (cab_t *)DISKCAB;
  if (WUB->cab_flag == B_CABGO)
  {
	return; /* disk cmd not completed yet */
  }

#ifdef DEBUG
	if (inpdintr++) {
		printf("recursive pdintr\n");
		debug();
		}
#endif
  for (i=0;i<100;i++) ;
  out_multibus(IMSCWU,B_M_INTCLR);
  imscint &= ~DISKBIT;
  blkacty &= ~(1<<PD0);

  out_local(P_PIOBDATA,in_local(P_PIOBDATA) | DISKLED);

  status = (WUB->cab_flag == B_CABERR) ?
    ((WUB->cab_etype)<<8 | (WUB->cab_ecode)) : 0;
  if (!status) {
    switch (pdtab.b_active) {
      case SIO:
      case SRECAL:
        pdtab.b_active = NULL;
        break;
      default:
        prdev("PD illegal internal state",pdtab.b_dev);
#ifdef DEBUG
	inpdintr = 0;
#endif
        return;
      }
    }
  else {
    pdtab.b_active = SRECAL;
    deverr("PD", &pdtab, WUB->cab_p5, status);
    bp->b_flags |= B_ERROR;
    pdtab.io_stp = &pdstat[minor(bp->b_dev)/PDLOGVOL];
    fmtberr(&pdtab,0);
    pdstart();
#ifdef DEBUG
	inpdintr = 0;
#endif
    return;
    }
  if (pdtab.io_erec)
    logberr(&pdtab,bp->b_flags&B_ERROR);
#ifdef DEBUG
	{ unsigned s;
	if ((s=spl7()) & 0x1000) {
		printf("whoops--vie\n");
		debug();
		}
	splx(s);
	}
#endif
  pdtab.b_actf = bp->av_forw;
  bp->b_resid = 0;
  iodone(bp);
  pdstart();
#ifdef DEBUG
	inpdintr = 0;
#endif
}

rw_disk(unit,cmd,blk,nbytes,seg,mem)
short unit;
short cmd;
long blk,nbytes;
register unsigned seg,mem;
{
  register struct cab_rw *WUB;

  WUB = (struct cab_rw *)DISKCAB;

  WUB->cab_disknum = unit;
  WUB->cab_blkno = blk;
  WUB->cab_nblocks = nbytes;
  WUB->cab_bufaddr = (((long)segtomb(seg))<<16) | mem;
  pdgo(CAB_IO,cmd);
}

pdgo(class,op)
unsigned char class;
unsigned char op;
{
  register cab_t *WUB;
  register short s;

  WUB=(cab_t *)DISKCAB;
  WUB->cab_class=class;
  WUB->cab_op=op;
  WUB->cab_flag=B_CABGO;
  s=spl7();
  out_multibus(IMSCWU,B_M_CMD);
  imscint |= DISKBIT;
  splx(s);
}

pdread(dev)
dev_t dev;
{
  physio(pdstrategy,&rpdbuf,dev,B_READ);
}

pdwrite(dev)
dev_t dev;
{
  physio(pdstrategy,&rpdbuf,dev,B_WRITE);
}

pdwaitdisk()
{
  long tocnt,delay;
  register cab_t *WUB;

  WUB=(cab_t *)DISKCAB;
  for (tocnt=75000L;tocnt && (WUB->cab_flag == B_CABGO);tocnt--) ;
  for (delay=100L;delay;delay--) ; /* hardware bug */
  out_multibus(IMSCWU,B_M_INTCLR);
  imscint &= ~DISKBIT;
  return(!tocnt || (WUB->cab_flag == B_CABERR));
}

pddevbad(dev)
dev_t dev;
{
  if (minor(dev) < PDLOGVOL) return(FALSE);
  return(TRUE);
}

pdbadoff(dev,blk)
dev_t dev;
daddr_t blk;
{
  if (blk >= pd_sizes[minor(dev)].nblks) return(TRUE);
  return(FALSE);
}

long
pdblkoff(dev)
dev_t dev;
{
  return(pd_sizes[minor(dev)].blkoff);
}

pdnblkbad(dev,blk)
dev_t dev;
daddr_t blk;
{
  if (blk > pd_sizes[minor(dev)].nblks) return(TRUE);
  return(FALSE);
}
