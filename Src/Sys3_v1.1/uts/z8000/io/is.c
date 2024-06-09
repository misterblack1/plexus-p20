#include "sys/param.h"
#include "sys/plexus.h"
#include "sys/systm.h"
#include "sys/buf.h"
#include "sys/iobuf.h"
#include "sys/conf.h"
#include "sys/dir.h"
#include "sys/user.h"
#include "sys/is.h"
#include "sys/io.h"
#include "sys/elog.h"
#include "sys/utsname.h"

/*
 * This is a driver for the INTEL iSBC 220 SMD disk controller.
 * 
 * The minor device number is used to index into a table which
 * gives the physical unit, starting cylinder, number of cylinders, and
 * addressing parameters for the logical volume.
 *
 * Configuration:
 *
 * The following information in the driver source depends on the types of
 * disks attached to the controller and switch settings on the controller
 * board.
 *
 * WUA:		This is a define which is the value in the wake up address 
 *		switches on the iSBC 220 controller card.
 *
 * init:	This array contains initialization data for each of the 4
 *		hardware units connected to the controller. Information used
 *		to fill in this array can be found in the controller and disk
 *		hardware manuals. The array is indexed by harware unit number.
 *
 * parms:	This array contains an element per logical volume (minor dev).
 *		The fields in an array element have the following meanings:
 *
 *		blkoff - The starting blk number for the logical volume.
 *
 *		nblk - The number of blks on the logical volume.
 *
 *		punit - The physical unit used for the logical volume.
 *
 *		blkcyl - Number of blocks per cylinder on punit.
 *
 *		blktrk - Number of blocks per track on punit.
 * 
 * DK_N:	This number is the index of this controller in the disk
 *		statistics tables.
 *
 * ISINT:	This constant has a bit on in the position of the int
 *		req line for the disk controller. The constant must be
 *		shifted left 1.
 *
 * DISKLED:	This constant has a bit on for the led that will light
 *		up while the disk is doing a request.
 */

int	isiniterr;	/* minus one if the controller did not initialize */
char	ebuf[12];	/* error status buffer */
			/* head of the buffer request chain */
struct	iostat	isstat[8];
struct	iobuf	istab = tabinit(IS0,isstat); 
struct	buf	risbuf;	/* used for physio */

int	sectpercyl[ISUNITS];	/* used to save extra calculation */
long	tocnt;


/*
 * This procedure sets up the iop structure and starts the requested
 * function to the controller. Note that some of the parameters will
 * not matter for various functions and that the requested transfer
 * count parameter is actually two integer parameters so that the
 * lsw and msw can be flipped for the iSBC 220.
 */

isgo(func, unit, modifier, cyl, sect, head, seg, buf, rmsw, rlsw)
char	func;
char	unit;
unsigned	modifier;
unsigned	cyl;
char	sect;
char	head;
unsigned	seg;
unsigned	*buf;
unsigned	rmsw;
unsigned	rlsw;
{

	iop.i_iopr1 = 0;
	iop.i_iopr2 = 0;
	iop.i_iopr3 = 0;
	iop.i_function = func;
	iop.i_unit = unit;
	iop.i_modifier = modifier;
	iop.i_cylinder = cyl;
	iop.i_sector = sect;
	iop.i_head = head;
	iop.i_dboff = (unsigned) buf;
	iop.i_dbseg = xmemto8086seg(segtomb(seg));
	iop.i_rc1 = rlsw;
	iop.i_rc2 = rmsw;
	iop.i_iopr4 = 0;
	iop.i_iopr5 = 0;
	out_multibus(WUA, OP_START); /* start the controller */
}

/*
 * This procedure is called to initialize the iSBC220 controller.
 * Completion status wait loops are used to test for completed commands
 * in this code for simplicity.
 */

isinit()
{
	register	tocnt;	/* counter for controller time out loop */
	register 	stat;	/* temp for command status */
	register	unit;	/* unit number being initialized */
	register	savepri;	/* priority at entrance to routine */
	register	i;	/* local tmp variable */
	struct block0 block0;	/* disk configuration information */
	static	isinitdone = 0; /* flag to allow only one is initialization */

	if (isinitdone++)
		return(isiniterr);
	savepri = spl7();
	WUB->w_wubr1 = 0;
	WUB->w_soc = 1;
	WUB->w_ccboff = (unsigned) &ccb;
	WUB->w_ccbseg = xmemto8086seg(segtomb(SDSEG));

	ccb.c_bsy1 = 0xff;
	ccb.c_ccw1 = 1;
	ccb.c_ciboff = (unsigned) &cib.c_csaoff;
	ccb.c_cibseg = xmemto8086seg(segtomb(SDSEG));
	ccb.c_ccbr1 = 0;
	ccb.c_bsy2 = 0;
	ccb.c_ccw2 = 1;
	ccb.c_cpoff = (unsigned) &ccb.c_cp;
	ccb.c_cpseg = xmemto8086seg(segtomb(SDSEG));
	ccb.c_cp = 4;

	cib.c_opstat = 0;
	cib.c_cibr1 = 0;
	cib.c_statsem = 0;
	cib.c_cmdsem = 0;
	cib.c_csaoff = 0;
	cib.c_csaseg = 0;
	cib.c_iopoff = (unsigned) &iop;
	cib.c_iopseg = xmemto8086seg(segtomb(SDSEG));
	cib.c_cibr2 = 0;
	cib.c_cibr3 = 0;

	out_multibus(WUA, OP_RESET);
	out_multibus(WUA, OP_CLEAR);
	out_multibus(WUA, OP_START);

	for (tocnt = MAXTOCNT; tocnt && ccb.c_bsy1; tocnt--) {
	}
	if (!tocnt) {
		isiniterr = -1;
		goto out;
	}

	if (is_sizes[ISLOGVOL].nblks == 0)
		copyio(SDSEG, &is_sizes[ISLOGVOL], &is_sizes[0],
			sizeof(struct diskconf)*ISLOGVOL*(ISUNITS-1), 2);
	for (unit = 0; unit < ISUNITS; unit++) {
		def_isdisk(unit, &is_init[unit]);
		sectpercyl[unit] = is_init[unit].sectpertrack 
			* is_init[unit].fixheads;
		block0.id = 0;
		isgo(DF_READ, unit, MOD_NOINT, 0, 0, 0, SDSEG,
			&block0, (long) sizeof(block0));
		iswaitdisk();
		if (block0.id == 'is') {
			def_isdisk(unit, &block0);
			sectpercyl[unit] = block0.init.sectpertrack
				* block0.init.fixheads;
			if (block0.initsize) 
				copyio(SDSEG, &is_sizes[ISLOGVOL*unit],
					block0.confinfo,
					sizeof(struct diskconf) * ISLOGVOL, 2);
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

	out:
	out_local(P_SPIC1, in_local(P_SPIC1) & (~ISINT)); /* ints on */
	out_local(P_MBTOINT, 0);  /* clear any mbtos caused here */
	splx(savepri);
	return(isiniterr);
}

/*
 * This procedure is called to poke the disk with the appropriate
 * initialization information.
 */

def_isdisk(unit,tbl)
int unit;
struct isiinit *tbl;
{
	isgo(DF_INIT, unit, MOD_NOINT, 0, 0, 0, SDSEG, tbl, 0, 0);
	iswaitdisk();
}

/*
 * This procedure is called to open a logical disk volume.
 */

isopen(dev, wflag)
dev_t	dev;
int	wflag;
{

	if ((minor(dev)>=ISLOGVOL*ISUNITS) || isiniterr) {
		u.u_error = ENXIO;
	}
}


/*
 * This procedure is the strategy routine for the iSBC 220 controller.
 * The main purpose of this routine is to check the request buffer 'buf'
 * for a legal request and then to hook the request into the request
 * queue. The controller start routine is called if the controller is
 * idle.
 */

isstrategy(bp)
struct	buf	*bp;
{
	register int	logvol;	/* logical volume */
	register long	sz;	/* num of disk blks affected by the request*/

	if (bp->b_flags & B_PHYS) 
		mapalloc(bp);	/* set up multibus mapping */
	logvol = minor(bp->b_dev);
	sz = bp->b_bcount;
	sz = (sz + BMASK) >> BSHIFT;	/* number of whole blocks */
	if (logvol >= ISLOGVOL*ISUNITS ||
	    bp->b_blkno + sz >= is_sizes[logvol].nblks) {
		bp->b_flags |= B_ERROR;
		iodone(bp);
		return;
	}
	bp->av_forw = NULL;
	spl5();
	if (istab.b_actf == NULL) {
		istab.b_actf = bp;
	} else {
		istab.b_actl->av_forw = bp;
	}
	istab.b_actl = bp;
	if (istab.b_active == NULL) {
		isstart();
	}
	spl0();
}

/*
 * This procedure is called to start the next operation on the disk
 * controller. There are two cases, the first is a normal I/O, and the second
 * is a request for which further status is to be obtained. 
 * Notice that b_active is used to tell the interrupt routine what kind 
 * of request is being worked on.
 */

isstart()
{
	struct	buf	*bp;	/* pointer to current request */
	daddr_t	blkno;	/* block number */
	int	cyl;	/* cylinder number */
	int	blk_in_cyl;	/* number of blocks into cylinder */
	int	head;	/* head number */
	int	sect;	/* sector number */
	int	cmd;	/* command type */
	int	logvol;	/* logical volume */
	int	blkcyl;	/* blocks per cylinder */
	int	blktrk;	/* blocks per track */
	int	unit;	/* disk physical unit */

	if ((bp = istab.b_actf) == NULL) {
		return;
	}
	logvol = minor(bp->b_dev);
	unit = logvol / ISLOGVOL;
	if (istab.b_errcnt) {
		istab.b_active = SSTAT;
		isgo(DF_STATUS, unit, MOD_STD,
			0, 0, 0, SDSEG, ebuf, (long) sizeof(ebuf));
	} else {
		istab.b_active = SIO;
		cmd = (bp->b_flags & B_READ) ? DF_READ : DF_WRITE;
		bp->b_resid = cmd;
		blkno = is_sizes[logvol].blkoff + bp->b_blkno;
		blktrk = is_init[unit].sectpertrack;
		blkcyl = sectpercyl[unit];
		cyl = blkno / blkcyl;
		blk_in_cyl = blkno % blkcyl;
		head = blk_in_cyl / blktrk;
		sect = blk_in_cyl % blktrk;
		isstat[unit].io_ops++;
		blkacty |= (1<<IS0);
		isgo(cmd, unit, MOD_STD, cyl, sect,
			head, hiword(bp->b_paddr), loword(bp->b_paddr), 
		     	(long) bp->b_bcount);
		dk_numb[DK_N] += 1;
		dk_wds[DK_N] += bp->b_bcount >> 6;
	}
	dk_busy |= 1 << DK_N;
	out_local(P_PIOBDATA, in_local(P_PIOBDATA) & (~DISKLED));
}

/*
 * This procedure is called when an interrupt is received from the
 * disk controller. The request may either be completed here or, in the case
 * that further status is needed the request may be altered and started again.
 */

isintr()
{
	int	state;	/* state of request which caused the interrupt */
	int	status;	/* completion status of request */
	int	unit;	/* unit number for current request */
	struct	buf	*bp;	/* pointer to current request */
	int	i;	/* index into error buffer */

	out_multibus(WUA, OP_CLEAR);
	status = cib.c_opstat & 0xff;
	cib.c_statsem = 0;
	if (S_MEDIACHG(status) ||
	    S_SEEK(status) ||
	    istab.b_active == NULL) {
		logstray(status);
		return;
	}
	blkacty &= ~(1<<IS0);
	bp = istab.b_actf;
	unit = minor(bp->b_dev) / ISLOGVOL;
	if (unit != S_UNIT(status)) {
		logstray(status);
		return;
	}
	dk_busy &= ~(1 << DK_N);
	out_local(P_PIOBDATA, in_local(P_PIOBDATA) | DISKLED);
	state = istab.b_active;
	istab.b_active = NULL;
	if (S_SUMMARY(status)) {
		istab.b_errcnt = status & 0xff;
		isstart();
		return;	
	}
	if (state == SSTAT) {
		status = istab.b_errcnt;	/* saved status from request */
	}
	if (S_HARD(status)) {
		istab.io_stp = &isstat[unit];
		fmtberr(&istab,0);
		deverr("IS", &istab, 0, status);
		bp->b_flags |= B_ERROR;
	}
	if (state == SSTAT) {
		printf("%s: IS physical status: ",utsname.sysname);
		for (i = 0; i < sizeof(ebuf); i++) {
			printf(" %x", 0xff & ebuf[i ^ 1]);
		}
		printf("\n");
	}
	if (istab.io_erec)
		logberr(&istab,bp->b_flags&B_ERROR);
	istab.b_errcnt = 0;
	istab.b_actf = bp->av_forw;
	bp->b_resid = 0;
	iodone(bp);
	isstart();
}

/*
 * This procedure is called for a raw type read on 'dev'.
 */

isread(dev)
dev_t	dev;
{
	
	physio(isstrategy, &risbuf, dev, B_READ);
}

/*
 * This procedure is called for a raw type write on "dev".
 */

iswrite(dev)
dev_t	dev;
{

	physio(isstrategy, &risbuf, dev, B_WRITE);
}

/*
 * This procedure waits for the disk in a skip-if-flags-set manner,
 * used during initialization.
 */
iswaitdisk() {
	for ( tocnt = MAXTOCNT; tocnt && !cib.c_statsem; tocnt--) ;
	cib.c_statsem = 0;
	if (!tocnt)
		return(2);
	return(0);
}
