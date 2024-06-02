#include <sys/plexus.h>
#include "saio.h"
#include <sys/imsc.h>
#ifndef V7
#include <sys/disk.h>
#include <sys/pd.h>
#endif

#define DISKCAB 0xfe00
#define TAPECAB 0xfe18
#define IMSCWU	0x1f
#define IMSCOWU	0xff
#define CAB_BADCMD 13

#define reterr ((WUB->cab_etype)<<8 | (WUB->cab_ecode))

#undef MAXTOCNT
#define MAXTOCNT 125000

#ifdef V7
#define NUMUNITS 4

struct s_iblk {
	unsigned cylinders;
	char remheads;
	char fixheads;
	char lsb_sectsize;
	char sectpertrack;
	char altcylinders;
	char msb_sectsize;
} 
inittab[NUMUNITS];

struct s_iblk pddefblk = { 
	589, 0, 7, 0, 34, 7, 2 };
#else
struct initinfo pddefblk = { 
	589, 0, 7, 0, 34, 20, 2 };
#define	ENDOFDISK 135422; /* based on 34 sect/trk, 7 heads, 589 cyl, 20 alt */
#ifndef V7
short	pdinitdone;
short	imscdiskcount;
short	numdisks;
short	ignoreblock0;
#endif

/* SYS3 */
/* initialize disk number "unit" with parameters defined in "tbl" */
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
	if (waitimsc(CAB_MAINT,CAB_DEF,(long) 10000))
		return(reterr);
	else
		return(NORMAL);
}
#endif

#ifdef V7
rw_disk(unit,func,blk,nbytes,buf)
int unit,func;
#else
/* read/write to disk.  Note that sys3 version uses cmd rather than func */
rw_disk(unit,cmd,blk,nbytes,buf)
int unit,cmd;
#endif
long blk,nbytes,buf;
{
	int status;
	register struct cab_rw *WUB;
	if ((status=seek_disk(unit,blk)) != NORMAL) return(status);

	WUB = (struct cab_rw *)DISKCAB;

	WUB->cab_blkno = blk;
	WUB->cab_nblocks=nbytes;
	WUB->cab_disknum=unit;
	WUB->cab_bufaddr=buf;

#ifdef V7
	if (diskgo())
#else
	if (waitimsc(CAB_IO,cmd,MAXTOCNT))
#endif
		return(reterr);
	else
		return(NORMAL);
}

pdstrategy(io,func)
register struct iob *io;
int func;
{
	daddr_t blk;
	int unit,status;

#ifdef V7
	blk=(long)(io->i_bn*(BSIZE/512));
	unit=io->i_unit;
	if ((status=rw_disk(unit,func,blk,(long)io->i_cc,MBSDSEG,io->i_ma)) 
		!= NORMAL) {
		printf("disk err: unit=%d blk=%ld stat=%x\n",unit,blk,status);
		return(-1);
	}
#else
	int	cmd;	/* command type */
	int	i;	/* temporary counter */
	int	logvol;	/* logical volume of disk unit, 0-15 */

	blk=(daddr_t)(io->i_bn*(BSIZE/512));
	unit = io->i_dp->dt_unit / PDLOGVOL;
	logvol = io->i_dp->dt_unit % PDLOGVOL;

	cmd = (func == READ) ? CAB_READ : CAB_WRITE;
	if(((blk + ((io->i_cc+512)>>9)) >= 
		pd_sizes[logvol].blkoff+pd_sizes[logvol].nblks) &&
		blk != 0) {
			errno = ENXIO;
			return -1;
	}
	if ((status=rw_disk(unit,cmd,blk,(long)io->i_cc,MBSDSEG,io->i_ma)) 
		!= NORMAL) {
		printf("%s: %s: (pd): unit=%d blk=%ld stat=%x\n",
			myname,io->i_dp->dt_name,unit,blk,status);
		return(-1);
	}
#endif
	return(io->i_cc);
}

#ifdef V7
pdinit(dev)
dev_t dev;
{
	register int status;

	if ((status=def_disk(dev,&inittab[dev])) != NORMAL) {
		printf("init err unit=%d stat=%x\n",dev,status);
		return(-1);
	}
	return(NORMAL);
}
#else

/*	Initializes imsc disk unit  with data from block 0 if there is
 *	any or default values otherwise.  
 *	This procedure returns the number of disks successfully
 *	initialized;  it sets imscdiskcount to the number of disks
 *	initialized and increments numdisks by the same value.
 */

pdinit()
{
	long		endofdisk; /* first invalid 512-byte sector of disk
					unit */
	register	unit;
	long		tocnt;	/* counter for controller time out loop */
	long		j;
	int		logvol;	/* logical disk  volume number */

  struct block0 block0;
  register short i;
  register cab_t *WUB;

	if (pdinitdone++)
		return 0;

	WUB = (cab_t *)DISKCAB;
	if(!imscthere())
		return 0;
  
	for(unit = 0; unit < PDUNITS; unit++) {
		if(def_pddisk(unit, &pddefblk) != NORMAL)
			continue;
	
		block0.id = 0;
		block0.initsize = 0;
		/* be quiet about nonexistent disks format on disk */
		if(rw_disk(unit,CAB_READ,0L,(long)sizeof(struct block0),
			MBSDSEG,&block0) != NORMAL) continue;
		deftdisk = 'pd';
		if (block0.id == 'pd' && ignoreblock0 == 0) {
			def_pddisk(unit, &block0);
			endofdisk= ((long) block0.init.sectpertrack) *
		    	    (block0.init.fixheads+ block0.init.remheads)*
		    	    (block0.init.cylinders-block0.init.altcylinders);
	
			pd_init[unit] = block0.init;
			if (block0.initsize)
				copydata(block0.confinfo,
					&pd_sizes[PDLOGVOL*unit],
					sizeof(struct diskconf) * PDLOGVOL);
	
			if (block0.nodenm[0] && !numdisks)
				for (i=0; i<9; i++)
					utsname.nodename[i] = block0.nodenm[i];
			if (block0.nswap && !numdisks) {
				rootdev = block0.rootdev;
				pipedev = block0.pipedev;
				dumpdev = block0.dumpdev;
				swapdev = block0.swapdev;
				nswap = block0.nswap;
				swplo = block0.swplo;
			}
		}
		else {
			pd_init[unit] = pddefblk;
			endofdisk= ENDOFDISK;
		}
		if (block0.initsize == 0) {
			/* use defaults for pd_sizes if none in block 0 */
			pd_sizes[unit*PDLOGVOL].nblks=endofdisk;
			pd_sizes[unit*PDLOGVOL+1].nblks=40000;
			for(logvol=unit*PDLOGVOL+2,j = 40000;
				logvol<(unit+1)*PDLOGVOL;logvol++) {
				pd_sizes[logvol].blkoff=j;
				pd_sizes[logvol].nblks=endofdisk-j;
				j += 20000;
			}
		}
		numdisks++;
		imscdiskcount++;
	}
	return(imscdiskcount);
}
#endif

pdopen(io)
register struct iob *io;
{

	return (0);
}


imscthere() {	/* returns 1 if imsc is there */

	register struct cab_def *WUB;

	WUB=(struct cab_def *)TAPECAB;
	WUB->cab_flag = 0;	/* zero out "do tape command" bit */

	return(waitimsc(CAB_UTIL,CAB_DKRDY,(long)10000) != -1);
}
/* returns -1 on timeout, nonzero on other errors, 0 if everything ok */
waitimsc(class,op,timeout)
long timeout;
{
	register cab_t *JUB;
	long tocnt;
  	long delay;

	JUB=(cab_t *)DISKCAB;
	JUB->cab_flag=B_CABGO;
	JUB->cab_class=class;
	JUB->cab_op=op;
	ledson(DISKLED);
	out_multibus(IMSCOWU,B_M_CMD);
  	out_multibus(IMSCWU,B_M_CMD);
	for (tocnt=timeout;tocnt && (JUB->cab_flag==B_CABGO);tocnt--) ;
  	for (delay=100L;delay;delay--) ;
  	out_multibus(IMSCWU,B_M_INTCLR);
	ledsoff(DISKLED);
	return(tocnt ? (JUB->cab_flags&B_CABERR) : -1);
}

/* this is here to make code compatible with old imsc boards */
seek_disk(unit,blk)
int unit;
long blk;
{
	register struct cab_seek *WUB;

	WUB = (struct cab_seek *)DISKCAB;

/***
	WUB->cab_class=CAB_UTIL;
	WUB->cab_op=CAB_SEEK;
***/

	WUB->cab_blocknum=blk;
	WUB->cab_disknum=unit;

	if (waitimsc(CAB_UTIL,CAB_SEEK,MAXTOCNT))
		return(reterr);
	else
		return(NORMAL);
}
