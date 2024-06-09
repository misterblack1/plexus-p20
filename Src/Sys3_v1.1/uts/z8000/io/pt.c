#include "sys/param.h"
#include "sys/plexus.h"
#include "sys/buf.h"
#include "sys/conf.h"
#include "sys/dir.h"
#include "sys/user.h"
#include "sys/iobuf.h"
#include "sys/imsc.h"
#include "sys/elog.h"
#include "sys/systm.h"
#include "sys/utsname.h"

#define IMSCWU 0x1f
#define TAPECAB 0xfe18

/* this def must match the one in low.s */
#define TAPEBIT 2

#define ITINT (0x80 << 1)
#define TPBSIZE 512


#define unit(x) (x&0x3)
#define special(x) (x&0x4)

long pt_nxrec;
short pt_openf;
short pt_written;
short ptiniterr = 0;

/*short imscsem;	/* locking between imsc disk and tape */
short imscint;
long pdblkoff();

struct iostat ptstat[4];
struct iobuf pttab = tabinit(PT0,ptstat);
struct buf rptbuf;
struct buf cptbuf;
struct ptcmd ptcmd;

ptopen(dev,flag)
dev_t dev;
short flag;
{
  if (ptiniterr || unit(minor(dev)) || pt_openf ) {
    u.u_error=ENXIO;
    return;
    }
  pt_written=FALSE;
  pt_nxrec=0L;
  pttab.b_flags |= B_TAPE;
  pt_openf++;
}

ptclose(dev,flag)
register dev_t dev;
short flag;
{
  if (unit(minor(dev)) || !pt_openf ) {
    u.u_error=ENXIO;
    return;
    }
  if (pt_written)
    ptcommand(dev,C_IWEOF);
  if (!special(minor(dev)))
    ptcommand(dev,C_IREW);
  pt_openf=0;
}

ptstrategy(bp)
register struct buf *bp;
{
  if (unit(minor(bp->b_dev)) || !pt_openf || (bp->b_bcount % TPBSIZE)) {
    bp->b_bcount = bp->b_resid = 0;
    bp->b_flags |= B_ERROR;
    iodone(bp);
    return;
    }

  if (bp->b_flags & B_PHYS)
    mapalloc(bp);

  bp->av_forw = NULL;
  spl5();
  if (pttab.b_actf == NULL)
    pttab.b_actf=bp;
  else
    pttab.b_actl->av_forw=bp;
  pttab.b_actl=bp;
  if (pttab.b_active == NULL)
    ptstart();
  spl0();
}

ptstart()
{
  register struct cab_rw *WUB;
  register struct buf *bp;
  register short savepri;

loop:
  if ((bp=pttab.b_actf) == NULL)
    return;
  
  if ((!pt_openf) || ((bp != &cptbuf) && (bp->b_blkno != pt_nxrec))) {
    bp->b_flags |= B_ERROR;
    bp->b_resid = bp->b_bcount = 0;
    goto next;
    }

  if (bp != &cptbuf) {
    pttab.b_active=C_IIO;
    WUB=(struct cab_rw *)TAPECAB;
    WUB->cab_nblocks=bp->b_bcount/TPBSIZE;
    WUB->cab_bufaddr = ((long)segtomb(hiword(bp->b_paddr))<<16) |
	loword(bp->b_paddr);
    ptstat[minor(bp->b_dev)].io_ops++;
    blkacty |= (1<<PT0);
    ptgo(CAB_IO, (bp->b_flags & B_READ)==0?
	pt_written=TRUE,CAB_WRTP : CAB_RDTP);
    goto out;
    }
  else
    {
    pttab.b_active=bp->b_resid;
    switch (bp->b_resid) {
    case C_IERASE:
      ptgo(CAB_MAINT,CAB_ERASE);
      goto out;
    case C_IRETENSION:
      ptgo(CAB_MAINT,CAB_RETENSION);
      goto out;
    case C_IREW:
    case C_IMOVE:
      {
      register struct cab_move *WUBM;
      WUBM=(struct cab_move *)TAPECAB;
      WUBM->cab_segmentnum=(bp->b_resid == C_IREW)?
	0 : ptcmd.blkcnt;
      ptgo(CAB_UTIL,CAB_MOVE);
      goto out;
      }
    case C_IWEOF:
      ptgo(CAB_MV,CAB_MARK);
      goto out;
    case C_ISAVE:
    case C_IRECALL:
/*      spl7();
      while (imscsem & B_BUSY)
      {
	imscsem |= B_WANTED;
	sleep((caddr_t)&imscsem,PRIBIO);
	spl7();
      }
      imscsem |= B_BUSY;
      imscint |= TAPEBIT;
      seek_disk(minor(ptcmd.dknum)/16,
		pdblkoff(ptcmd.dknum)+ptcmd.blkno);
      goto out;
    case C_I2SAVE:
    case C_I2RECALL: */
      {
      register struct cab_rw *WUBS;
/*      if ((bp->b_flags & B_ERROR)==0) { /* make sure seek worked!! */
        WUBS=(struct cab_rw *)TAPECAB;
        WUBS->cab_nblocks = ptcmd.blkcnt;
        WUBS->cab_disknum = minor(ptcmd.dknum)/16;
	WUBS->cab_blkno = pdblkoff(ptcmd.dknum)+ptcmd.blkno;
        ptgo(CAB_MV,(bp->b_resid==C_ISAVE)? CAB_SAVE : CAB_RECALL);
        goto out;
	}
/*      else 
      if (imscsem & B_WANTED) 
         wakeup((caddr_t)&imscsem);
      imscsem &= ~(B_BUSY|B_WANTED); 
	goto next;
      } */
    default:
      prdev("PT illegal internal state",pttab.b_dev);
      goto out;
    }
    }

/* may want to give the disk a chance to get in here somewhere */
next:
  pttab.b_actf = bp->av_forw;
  iodone(bp);
  goto loop;

out:
  out_local(P_PIOBDATA, in_local(P_PIOBDATA) & (~TAPELED));
  return;

}

ptintr()
{
  register short nbread,err;
  register struct cab_rw *WUB;
  register struct buf *bp;
  register short state;
  int i;

/*  spl7(); */
  if ((bp=pttab.b_actf) == NULL) {
    return;
    }
  
  WUB=(struct cab_rw *)TAPECAB;
  if (WUB->cab_flag == B_CABGO)
  {
	return; /* tape cmd not completed yet */
  }
/*  spl0(); */

  for (i=0;i<100;i++) ;
  out_multibus(IMSCWU,B_M_INTCLR);
  imscint &= ~TAPEBIT;
  blkacty &= ~(1<<PT0);

  out_local(P_PIOBDATA, in_local(P_PIOBDATA) | TAPELED);

  state = pttab.b_active;
  pttab.b_active = NULL;

  switch (state) {

    case C_IREW:
    case C_IERASE:
    case C_IRETENSION:
      nbread = 0;
      break;
/*    case C_I2SAVE:
    case C_I2RECALL: */
      case C_ISAVE:
      case C_IRECALL:
      {
      register struct cab_rw* WUB2;
/*      if (imscsem & B_WANTED)
         wakeup((caddr_t)&imscsem);
      imscsem &= ~(B_BUSY|B_WANTED); */
      WUB2=(struct cab_rw *)TAPECAB;
      ptcmd.blkcnt -= WUB2->cab_nblocks;
      ptcmd.blkno = WUB2->cab_blkno;
      } /* fall thru */
    case C_IIO:
      {
      register struct cab_rw *WUBIO;
      WUBIO=(struct cab_rw *)TAPECAB;
      nbread=(bp->b_bcount/TPBSIZE) - (short)WUBIO->cab_nblocks;
      break;
      }
/*    case C_ISAVE:
      bp->b_resid = C_I2SAVE;
      ptstart();
      return;
    case C_IRECALL:
      bp->b_resid = C_I2RECALL;
      ptstart();
      return; */
    }

  pt_nxrec += nbread;

  if (WUB->cab_flag == B_CABERR) {
    err=(WUB->cab_etype << 8) | WUB->cab_ecode;
/*    bp->b_bcount = 0; */
    bp->b_resid = bp->b_bcount - (nbread * TPBSIZE);
    if (err != E_TEOS) {
      deverr("PT", &pttab, WUB->cab_estat, err);
      bp->b_flags |= B_ERROR;
      }
      pttab.io_stp = &ptstat[minor(bp->b_dev)];
      fmtberr(&pttab,0);
    }
  else {
/*    bp->b_bcount = nbread*TPBSIZE; */
    bp->b_resid=0;
    }

  if (pttab.io_erec)
     logberr(&pttab,bp->b_flags&B_ERROR);
  pttab.b_actf = bp->av_forw;
  iodone(bp);
  ptstart();
}

ptcommand(dev,cmd)
register dev_t dev;
register short cmd;
{
  register struct buf *bp;
  int s;
  
  bp = &cptbuf;
  s=spl5();
  while (bp->b_flags & B_BUSY) {
    bp->b_flags |= B_WANTED;
    sleep ( (caddr_t)bp,PRIBIO);
    }
  bp->b_flags = B_READ | B_BUSY;
  splx(s);
  bp->b_dev =dev;
  bp->b_resid= cmd;
  bp->b_bcount=0;
  bp->b_blkno=0;
  ptstrategy(bp);
  iowait(bp);
  if (bp->b_flags & B_WANTED)
    wakeup( (caddr_t)bp);
  bp->b_flags = 0;
  return(NORMAL);
}  

ptgo(class,op)
unsigned char class;
unsigned char op;
{
   register cab_t *WUB;
   register short s;

   WUB=(cab_t *)TAPECAB;
   WUB->cab_class=class;
   WUB->cab_op=op;
   WUB->cab_flag=B_CABGO;
   s=spl7();
   out_multibus(IMSCWU,B_M_CMD);
   imscint |= TAPEBIT;
   splx(s);
}

ptinit()
{
  register short savepri;
  register struct cab_move *WUB;

/*  imscsem = 0;		/* init locking semaphore */

  savepri = spl7();
  WUB = (struct cab_move *)TAPECAB;
  ptiniterr=ptzap(CAB_UTIL,CAB_TPTEST);
  if (!ptiniterr) /* issue rewind to force a read status on drive */
  {
  	WUB->cab_segmentnum = 0;
  	ptzap(CAB_UTIL,CAB_MOVE);
  }
  out_local(P_SPIC1,in_local(P_SPIC1)&(~ITINT));
  out_local(P_MBTOINT,0);
  splx(savepri);
  return(!ptiniterr ? 0 : -1);
}

ptzap(class,op)
unsigned char class;
unsigned char op;
{
  register cab_t *WUB;
  long tocnt,delay;
  WUB = (cab_t *)TAPECAB;
  WUB->cab_class=class;
  WUB->cab_op=op;
  WUB->cab_flag=B_CABGO;
  out_multibus(IMSCWU,B_M_CMD);
  for (tocnt=10000;tocnt && (WUB->cab_flag==B_CABGO);tocnt--) ;
  for (delay=100L;delay;delay--) ; /* hardware bug */
  out_multibus(IMSCWU,B_M_INTCLR);
  return(!tocnt || (WUB->cab_flag==B_CABERR) );
}

ptread(dev)
dev_t dev;
{
  physio(ptstrategy,&rptbuf,dev,B_READ);
}

ptwrite(dev)
dev_t dev;
{
  physio(ptstrategy,&rptbuf,dev,B_WRITE);
}

ptioctl(dev,cmd,addr,flag)
register dev_t dev;
register unsigned cmd;
register caddr_t addr;
register unsigned flag;
{
  switch (cmd) {
    case(C_IREW):
    case(C_IERASE):
    case(C_IRETENSION):
    case(C_IWEOF):
      ptcommand(dev,cmd);
      return;
    case(C_IMOVE):
    case(C_ISAVE):
    case(C_IRECALL):
      if (copyin(addr,(caddr_t)&ptcmd,sizeof(struct ptcmd))) {
	u.u_error=EFAULT;
	return;
	}
      if (cmd != C_IMOVE)
         if (pddevbad(ptcmd.dknum) || pdbadoff(ptcmd.dknum,ptcmd.blkno)
		|| pdnblkbad(ptcmd.dknum,ptcmd.blkcnt)) {
			u.u_error = ENXIO;
			return;
			}
      ptcommand(dev,cmd);
      if (copyout((caddr_t)&ptcmd,addr,sizeof(struct ptcmd))) {
	u.u_error=EFAULT;
	}
      return;
    default:
      u.u_error=ENXIO;
      return;
    }
}
