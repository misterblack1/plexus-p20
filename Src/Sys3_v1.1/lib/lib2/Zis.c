#define ERRFLAGS	0xc0

/*
 * This is a driver for the INTEL iSBC 220 SMD disk controller.
 * 
 * The driver handles up to 4 physical units with one fixed and removable
 * volume on each unit. Each of these 8 physical volumes can be split into
 * up to 8 different logical volumes thus providing for a maximum of 64 logical
 * volumes on one controller. 
 *
 * The minor device number chooses one of the 64 logical volumes. The low order
 * 3 bits gives the logical volume on a particular physical volume. The next
 * most sig. bit is 1 for the removable volume on a unit and 0 for the fixed
 * volume. The 2 most sig. bits give the disk unit number.
 *
 * Configuration:
 *
 * The following information in the driver source depends on the types of
 * disks attached to the controller and switch settings on the controller
 * board.
 *
 * WUA:	This is a define which is the value in the wake up address switches
 *	on the iSBC 220 controller card.
 *
 * init:	This array contains initialization data for each of the 4
 *		hardware units connected to the controller. Information used
 *		to fill in this array can be found in the controller and disk
 *		hardware manuals. The array is indexed by harware unit number.
 *
 * size_off	This array contains an element per logical volume
 *		(minor device). The element contains the number of 512 byte
 *		blocks and the starting cylinder number which defines the
 *		portion of the physical volume which comprises the logical
 *		volume. Note that the alternate tracks at the end of the
 *		physical volume are not included in any logical volumes.
 * 
 * DK_N	This number is the index of this controller in the disk statistics
 *	tables.
 */

#include "saio.h"
#include <sys/plexus.h>
#include <sys/is.h>
#undef MAXTOCNT
	/* 60000 copied from Sys3 boots stuff */
#define MAXTOCNT 60000


int	isiniterr;	/* minus one if the controller did not initialize */
int	sectpercyl[ISUNITS];	/* used to save extra calculation */
int	isbcdiskcount;
int	numdisks;
short	isinitdone; /* nonzero when controller initialized */
short	ignoreblock0;

#define	ENDOFDISK 140525; /* based on 35 sect/trk, 5 heads, 823 cyl, 20 alt */
struct initinfo defblk = { 823, 0, 5, 0, 35, 20, 2};

/*
 * This procedure sets up the iop structure and starts the requested
 * function to the controller. Note that some of the parameters will
 * not matter for various functions and that the requested transfer
 * count parameter is actually two integer parameters so that the
 * lsw and msw can be flipped for the iSBC 220.
 */

isgo(func, unit, modifier, cyl, sect, head ,xmem, buf, rmsw, rlsw)
char	func;
char	unit;
unsigned	modifier;
unsigned	cyl;
char	sect;
char	head;
unsigned	xmem;
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
	iop.i_dbseg = xmemto8086seg(xmem);
			/* Disk controller will not transfer a single byte */
	if (rlsw == 1)
		rlsw++;
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
 * This procedure returns the number of disks initialized.
 * It sets isbcdiskcount to the number of isbc disks initialized
 * and bumps numdisks by the same value.
 */


isinit()
{
	struct block0 block0; /* disk configuration information */
	long		endofdisk; /* first invalid 512-byte sector of disk
					unit */
	long		tocnt;	/* counter for controller time out loop */
	register	unit;
	register	stat;	/* temp for command status */
	register	i;	/* local tmp variable */
	long		j;
	int		logvol;	/* logical disk  volume number */

	if(isinitdone++) 
		return isbcdiskcount;

	WUB->w_wubr1 = 0;
	WUB->w_soc = 1;
	WUB->w_ccboff = (unsigned)&ccb;
	WUB->w_ccbseg = xmemto8086seg(MBSDSEG);

	ccb.c_bsy1 = 0xff;
	ccb.c_ccw1 = 1;
	ccb.c_ciboff = (unsigned) &cib.c_csaoff;
	ccb.c_cibseg = xmemto8086seg(MBSDSEG);
	ccb.c_ccbr1 = 0;
	ccb.c_bsy2 = 0;
	ccb.c_ccw2 = 1;
	ccb.c_cpoff = (unsigned) &ccb.c_cp;
	ccb.c_cpseg = xmemto8086seg(MBSDSEG);
	ccb.c_cp = 4;

	cib.c_opstat = 0;
	cib.c_cibr1 = 0;
	cib.c_statsem = 0;
	cib.c_cmdsem = 0;
	cib.c_csaoff = 0;
	cib.c_csaseg = 0;
	cib.c_iopoff = (unsigned) &iop;
	cib.c_iopseg = xmemto8086seg(MBSDSEG);
	cib.c_cibr2 = 0;
	cib.c_cibr3 = 0;

	out_multibus(WUA, OP_RESET);
	out_multibus(WUA, OP_CLEAR);
	out_multibus(WUA, OP_START);

	ledson(DISKLED);
	for (tocnt = MAXTOCNT; tocnt && ccb.c_bsy1; tocnt--);
	ledsoff(DISKLED);

		/* no isbc controller: assume imsc is checked first. */
	if (!tocnt && !numdisks) {
		return(-1);
	}
	if (!tocnt)
		return 0;
		
	for(unit = 0; unit < ISUNITS; unit++) {
		if(def_isdisk(unit, &defblk) != 0)
			continue;
	
		/* Sys3: be quiet about nonexistant disks */
		block0.id = 0;
		block0.initsize = 0;
		isgo(DF_READ, unit, MOD_NOINT, 0, 0, 0, MBSDSEG,
			&block0, (long) sizeof( struct block0));
		iswaitdisk();
		if(deftdisk == 0) deftdisk='is';
		if (block0.id == 'is' && ignoreblock0 == 0) {
			def_isdisk(unit, &block0);
			is_init[unit] = block0.init;
			endofdisk= ((long) block0.init.sectpertrack) *
			    (block0.init.fixheads+block0.init.remheads) *
			    (block0.init.cylinders-block0.init.altcylinders);
			if (block0.initsize)
				copydata(block0.confinfo,
					&is_sizes[ISLOGVOL*unit],
					sizeof(struct diskconf) * ISLOGVOL);
			/* assume imsc disk checked for first and use
			 * data from that disk's block 0.
			 */
			if (block0.nodenm[0] && !isbcdiskcount && !numdisks)
				for (i=0; i<9; i++)
					utsname.nodename[i] = block0.nodenm[i];
			if (block0.nswap && !isbcdiskcount && !numdisks) {
				rootdev = block0.rootdev;
				pipedev = block0.pipedev;
				dumpdev = block0.dumpdev;
				swapdev = block0.swapdev;
				nswap = block0.nswap;
				swplo = block0.swplo;
			}
		}
		else {
			is_init[unit] = defblk;
			endofdisk= ENDOFDISK;
		}
		if (block0.initsize == 0) {
			/* use defaults for is_sizes if none in block 0 */
			is_sizes[unit*ISLOGVOL].nblks=endofdisk;
			is_sizes[unit*ISLOGVOL+1].nblks=40000;
			for(logvol=unit*ISLOGVOL+2,j = 40000;
				logvol<(unit+1)*ISLOGVOL;logvol++) {
				is_sizes[logvol].blkoff=j;
				is_sizes[logvol].nblks=endofdisk-j;
				j += 20000;
			}
		}
		isbcdiskcount++;
		numdisks++;
	}
	return isbcdiskcount;
}

/*
 * This procedure is called to poke the disk with the appropriate
 * initialization information.
 */

def_isdisk(unit,tbl)
int unit;
struct initinfo *tbl;
{
	isgo(DF_INIT, unit, MOD_NOINT, 0, 0, 0, MBSDSEG, tbl, 0, 0);
	return (iswaitdisk());
}

/*
 * Open the disk
 */

isopen(io)
register struct iob *io;
{

	return(0);
}


/*
 * This procedure is the strategy routine for the iSBC 220 controller.
 * This procedure is called to start the next operation on the disk
 * controller. There are two cases, the first is a normal I/O, and the second
 * is a request that failed for which an error status is to be obtained. 
 */

isstrategy(io, func)
register struct iob *io;
{
	struct	buf	*bp;	/* pointer to current request */
	daddr_t	blkno;	/* block number of request */
	int	cyl;	/* cylinder number for request */
	int	sect_trk;	/* number of sectors per track */
	int	sect_cyl;	/* number of sectors per cyl (all heads) */
	int	offset_in_cyl;	/* number of sectors into cylinder */
	int	head;	/* head number */
	int	sect;	/* sector number */
	int	cmd;	/* command type */
	int	unit;	/* unit number alone */
	int	pvol;	/* fixed or removable volume flag */
	int	status;	/* completion status of request */
	int	i;	/* temporary counter */
	int	logvol;	/* logical volume of disk unit, 0-15 */

	unit = io->i_dp->dt_unit / ISLOGVOL;
	logvol = io->i_dp->dt_unit % ISLOGVOL;

	cmd = (func == READ) ? DF_READ : DF_WRITE;
	blkno = io->i_bn*(BSIZE/512);
	if((blkno + ((io->i_cc+512)>>9)) >= 
		is_sizes[logvol].blkoff+is_sizes[logvol].nblks &&
		blkno != 0) {
			errno = ENXIO;
			return -1;
	}
	sect_trk = is_init[unit].sectpertrack;
	sect_cyl = sect_trk * is_init[unit].fixheads;

	cyl = blkno / sect_cyl;
	offset_in_cyl = blkno % sect_cyl;
	head = offset_in_cyl / sect_trk;
	sect = offset_in_cyl % sect_trk;

	isgo(cmd, unit, MOD_NOINT, cyl, sect, head, MBSDSEG,
			io->i_ma, (long)io->i_cc);

	iswaitdisk();

	status = cib.c_opstat & 0xff;
	cib.c_opstat = 0;
	if (S_SUMMARY(status) || S_HARD(status)) {
		printf("%s: %s: (is) unit=%d, cyl=%d, head=%d, %s=%d,%s=0x%x\n",
			myname,io->i_dp->dt_name,unit, cyl,head,"sector",sect,
			" status", status);
		if (S_SUMMARY(status)) {
			isgo(DF_STATUS, unit, MOD_NOINT, cyl, sect, head, MBSDSEG,
				io->i_ma, (long) io->i_cc);
			iswaitdisk();
			cib.c_statsem = 0;
			for (i=0; i<12; i++)
				printf("%x, ",(*(char *)&io->i_ma[i^1])&0xff);
			printf("\n");
			cib.c_opstat = 0;
		}
		errno = EIO;
		return(-1);
	}
	return(io->i_cc);
}

iswaitdisk() {
	long tocnt;

	ledson(DISKLED);
	for ( tocnt = MAXTOCNT; tocnt && !cib.c_statsem; tocnt--) ;
	ledsoff(DISKLED);

#ifdef V7
	if (!tocnt)
		printf("Disk error: timeout\n");
	
	if (S_HARD(cib.c_opstat))
		printf("Disk error: hard error\n");
#else
	if (!tocnt) return -2;
	if (S_HARD(cib.c_opstat)) {
		cib.c_statsem = 0;
		return -1;
	}
#endif
	cib.c_statsem = 0;
#ifndef V7
	return 0;
#endif
}
