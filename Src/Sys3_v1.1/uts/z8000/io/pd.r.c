#include "sys/param.h"
#include "sys/plexus.h"
#include "sys/buf.h"
#include "sys/conf.h"
#include "sys/dir.h"
#include "sys/user.h"
#include "sys/disk.h"
#include "sys/pd.h"
#include "sys/imsc.h"
#include "sys/systm.h"
#include "sys/utsname.h"

/*
 * special redeclarations for
 * the head of the queue per
 * device driver.
 */
#define	b_actf	av_forw
#define	b_actl	av_back
#define	b_active b_bcount
#define	b_errcnt b_resid

struct buf pdtab;
struct buf rpdbuf;

int imscsem;
int pdintr();
caddr_t imscintr;

char pdiniterr[4];

pdinit()
{
  register int savepri;
  register int unit;
  register int i;
  struct block0 block0;
  static pdinitdone = 0;

  if (pdinitdone++)
    goto done;
  imscsem = 0;
  imscintr = (caddr_t)pdintr;

  savepri=spl7();
	if (pd_sizes[PDLOGVOL].nblks == 0)
		copyio(SDSEG, &pd_sizes[PDLOGVOL], &pd_sizes[0],
			sizeof(struct diskconf)*PDLOGVOL*(PDUNITS-1), 2);
	for (unit = 0; unit < PDUNITS; unit++) {
		def_pddisk(unit, &pd_init[unit]);
		seek_disk(unit,0L);
		pdiniterr[unit] = pdwaitdisk();
		rw_disk(unit,CAB_READ,(long)sizeof(struct block0),
			segtomb(SDSEG), &block0);
		pdiniterr[unit] |= pdwaitdisk();
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

  out_multibus((~WUA)&0xff,B_M_INTCLR);
  out_local(P_SPIC1,in_local(P_SPIC1)&(~IDINT));
  out_local(P_MBTOINT,0);
  splx(savepri);
done:
  for (unit=0; unit<PDUNITS; unit++)
    if (pdiniterr[unit] == 0) break;
  return((unit==PDUNITS) ? -1 : 0);
}

def_pddisk(unit,tbl)
int unit;
register struct block0 *tbl;
{
  register struct cab_def *WUB;

  WUB=(struct cab_def *)WUA;
  WUB->cab_disknum = unit;
  WUB->cab_sectnum = tbl->init.sectpertrack;
  WUB->cab_headnum = tbl->init.fixheads;
  WUB->cab_cylnum = tbl->init.cylinders;
  WUB->cab_altcylnum = (int)tbl->init.altcylinders;
  pdgo(CAB_MAINT,CAB_DEF);
  return(pdwaitdisk());
}

pdopen(dev,wflag)
dev_t dev;
int wflag;
{
  if ((minor(dev) >= (PDLOGVOL*PDUNITS)) ||
      pdiniterr[minor(dev)/PDLOGVOL] )
    u.u_error = ENXIO;
}

pdstrategy(bp)
register struct buf *bp;
{
  register int logvol;
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
  register int logvol;
  register int unit;
  register int cmd;
  register struct buf *bp;
  register int savepri;


  while (1) {
    if ((bp=pdtab.b_actf) == NULL) return;
    savepri=spl7();
    if ((imscsem & B_BUSY) == 0) break;
    imscsem |= B_WANTED;
    sleep((caddr_t)&imscsem,PRIBIO);
    }
  imscsem |= B_BUSY;
  imscintr = (caddr_t)pdintr;
  splx(savepri);

  logvol = minor(bp->b_dev);
  unit = logvol / PDLOGVOL;

  if (pdtab.b_active == NULL) {		/* issue seek */
    pdtab.b_active = SSEEK;
    seek_disk(unit,pd_sizes[logvol].blkoff+bp->b_blkno);
    }
  else if (pdtab.b_active == SRECAL) {
    pdgo(CAB_UTIL,CAB_DKRECAL);
    }
  else {
    cmd = (bp->b_flags&B_READ) ? CAB_READ : CAB_WRITE;
    rw_disk(unit,cmd,(long)bp->b_bcount,bp->b_paddr);
    }
  out_local(P_PIOBDATA,in_local(P_PIOBDATA) & (~DISKLED));
}

pdintr()
{
  
  register cab_t *WUB;
  register int status;
  register struct buf *bp;
  register unsigned s;

  out_multibus((~WUA)&0xff,B_M_INTCLR);

  if ((bp=pdtab.b_actf)==NULL) {
    return;
    }

  out_local(P_PIOBDATA,in_local(P_PIOBDATA) | DISKLED);
  if (imscsem & B_WANTED)
    wakeup((caddr_t)&imscsem);
  imscsem &= ~(B_BUSY|B_WANTED);

  WUB = (cab_t *)WUA;
  status = (WUB->cab_flag == B_CABERR) ?
    ((WUB->cab_etype)<<8 | (WUB->cab_ecode)) : 0;
  if (!status) {
    switch (pdtab.b_active) {
      case SSEEK:
        pdtab.b_active = SIO; /* seek done, start up i/o */
        pdstart();
        return;
      case SIO:
      case SRECAL:
        pdtab.b_active = NULL;
        break;
      default:
        printf("(pd): illegal internal state\n");
        return;
      }
    }
  else {
    pdtab.b_active = SRECAL;
    bp->b_flags |= B_ERROR;
    printf("(pd): err unit=%d status=%x\n",WUB->cab_p5,status);
    pdstart();
    return;
    }
  pdtab.b_actf = bp->av_forw;
  bp->b_resid = 0;
  iodone(bp);
  pdstart();
}

seek_disk(unit,sect)
int unit;
long sect;
{
  register struct cab_seek *WUB;

  WUB = (struct cab_seek *)WUA;
  WUB->cab_disknum = unit;
  WUB->cab_blocknum = sect;
  pdgo(CAB_UTIL,CAB_SEEK);
}

rw_disk(unit,cmd,nbytes,seg,mem)
int unit;
int cmd;
long nbytes;
register unsigned seg,mem;
{
  register struct cab_rw *WUB;

  WUB = (struct cab_rw *)WUA;

  WUB->cab_disknum = unit;
  WUB->cab_nblocks = nbytes;
  WUB->cab_bufaddr = (((long)segtomb(seg))<<16) | mem;
  pdgo(CAB_IO,cmd);
}

pdgo(class,op)
unsigned char class;
unsigned char op;
{
  register cab_t *WUB;
  WUB=(cab_t *)WUA;
  WUB->cab_class=class;
  WUB->cab_op=op;
  WUB->cab_flag=B_CABGO;
  out_multibus((~WUA)&0xff,B_M_CMD);
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
  long tocnt;
  register cab_t *WUB;

  WUB=(cab_t *)WUA;
  for (tocnt=10000;tocnt && (WUB->cab_flag == B_CABGO);tocnt--) ;
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
