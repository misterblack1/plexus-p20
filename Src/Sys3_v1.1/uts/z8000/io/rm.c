#include "sys/plexus.h"
#include "sys/param.h"
#include "sys/buf.h"
#include "sys/dir.h"
#include "sys/conf.h"
#include "sys/file.h"
#include "sys/iobuf.h"
#include "sys/user.h"
#include "sys/elog.h"
#include "sys/systm.h"
#include "sys/rm.h"

/*
 * This is a driver for the CPC TAPEMASTER tape controller.
 *
 * The driver can control up to 4 physical units.
 * 
 * The minor device number selects not only the physical unit
 * to be used but also the type of drive selected by the unit
 * and other drive and controller parameters such as writing
 * speed and bus locking mode.
 *
 * Configuration:
 *
 * The following information in the driver source depends on the types of 
 * devices connected to the controller, the jumper settings on the controller
 * and the correspondence between minor device number and physical
 * controller parameter settings.
 *
 * SCP:		The constant in this macro is the system memory address
 *		of the start up pointer for the controller.
 *
 * ATTN:	This constant is the multibus i/o wakeup address for the
 *		controller.
 *
 * SOFTRESET:	This is the multibus i/o software reset address for the
 *		controller.
 *
 * RMINT:	This constant has a bit on in the position which identifies
 *		the multibus interrupt line used by the controller. Note
 *		that this field should be shifted left one bit.
 *
 * TAPELED:	This constant has a bit on that defines which led on
 *		the cpu board will light when a request is in progress.
 *
 * ctl:		This array is indexed by minor device number.
 *		The contents of the array elements are the parameters
 *		used by the controller for all operations on the unit.
 *
 * LUNIT:	This is the number of logical units supported by the driver.
 *		There is one entry in ctl for each logical unit.
 *
 */

/* misc defines */

#define	SCP	((struct scp_struct *) 0xfff6)
#define	ATTN		0xaaaa
#define	RESET		0xaaab
#define	RMINT		(0x02 << 1)
#define	PUNIT	4
#define	LUNIT	32
#define	REW		0
#define	NOREW		1
#define	xmemto8086seg(s)	((s) << 12)	/* convert to 8086 seg num */
#define	MAXTOCNT	50000			/* wait counter */
#define	T_WRITTEN	1
#define	SSEEK		1
#define	SIO		2
#define	SCOM		3
#define	SRETRY		4
#define	SOVER		5

/* parameter buffer defines */

#define	PUNITSHIFT	2
#define	PUNITMASK	0x000c

/* command fields only used in the driver */

#define	SILENT		0x2000
#define	INT		0x8000

/* global variables */

struct iostat	rmstat[4];
struct iobuf	rmtab = tabinit(RM0,rmstat);;
struct buf	crmbuf;
struct buf	rrmbuf;
char	r_flags[PUNIT];
char	r_openf[PUNIT];
daddr_t	r_blkno[PUNIT];
daddr_t	r_nxrec[PUNIT];
unsigned	r_ctl[PUNIT];
int	rminiterr;
struct	scb_struct  scb;
struct	xcb_struct xcb;
struct	pb_struct pb;
struct	{
	char	rewflg;
	unsigned	unitcontrol;
} ctl[LUNIT] = {
	REW, (BUS16 | UNIT0),
	REW, (BUS16 | UNIT1),
	REW, (BUS16 | UNIT2),
	REW, (BUS16 | UNIT3),
	NOREW, (BUS16 | UNIT0),
	NOREW, (BUS16 | UNIT1),
	NOREW, (BUS16 | UNIT2),
	NOREW, (BUS16 | UNIT3),
	REW, (BUS16 | HISPEED | UNIT0),
	REW, (BUS16 | HISPEED | UNIT1),
	REW, (BUS16 | HISPEED | UNIT2),
	REW, (BUS16 | HISPEED | UNIT3),
	NOREW, (BUS16 | HISPEED | UNIT0),
	NOREW, (BUS16 | HISPEED | UNIT1),
	NOREW, (BUS16 | HISPEED | UNIT2),
	NOREW, (BUS16 | HISPEED | UNIT3),
};

struct rmcmd_struct rmcmd;
#define	NUMLEGAL	13
int rmlegal[NUMLEGAL] = {
	C_OLREW,
	C_NOP,
	C_RFOREIGN,
	C_STATUS,
	C_REW,
	C_UNLOAD,
	C_WEOF,
	C_SRCHEOF,
	C_SPACE,
	C_ERASE,
	C_ERASEALL,
	C_SPACEEOF,
	C_SRCHMEOF};

/*
 * This routine initiates a request to the controller. Note that the
 * reverse and interrupt control bits are passed in as part of the cmd
 * parameter. Note also that the rest of the control word is gotten
 * from the one set up by the open on the device.
 */

rmgo(cmd, unit, size, recs, seg, buf)
unsigned	cmd;
unsigned	unit;
unsigned	size;
unsigned	recs;
unsigned	seg;
char		*buf;
{
	register tocnt;
	
	pb.p_pbr1 = 0;
	pb.p_cmd = cmd & (~C_REV) & (~INT);
	pb.p_pbr2 = 0;
	pb.p_control = r_ctl[unit] | (cmd & C_REV ? REVERSE : 0) |
				     (cmd & INT ? INTR : 0);
	pb.p_count = 0;
	pb.p_size = size;
	pb.p_records = recs;
	pb.p_dboff = (unsigned) buf;
	pb.p_dbseg = xmemto8086seg(segtomb(seg));
	pb.p_status = 0;
	pb.p_li1 = 0;
	pb.p_li2 = 0;
	xcb.c_gate = GATECLOSED;
	out_multibus(ATTN, 0);  /* start operation */
	if (cmd & INT) {
		for(tocnt = 0; (in_local(P_SPIC0) & RMINT) == 1; tocnt++);
		out_local(P_SPIC1, in_local(P_SPIC1) & (~RMINT));  /* ints on */
	}
}


/*
 * The following routine is called early in unix startup to initialize
 * the controller.
 */

rminit()
{
	register	tocnt;
	register	savepri;

	savepri = spl7();

	out_multibus(RESET, 0);  /* reset the controller */

	SCP->s_scpr1 = 0;
	SCP->s_sysbus = 1;
	SCP->s_scboff = (unsigned) &scb;
	SCP->s_scbseg = xmemto8086seg(segtomb(SDSEG));

	scb.s_scbr1 = 0;
	scb.s_soc = 3;
	scb.s_xcboff = (unsigned) &xcb;
	scb.s_xcbseg = xmemto8086seg(segtomb(SDSEG));

	xcb.c_gate = GATECLOSED;
	xcb.c_ccw = CCWNORM;
	xcb.c_pboff = (unsigned) &pb;
	xcb.c_pbseg = xmemto8086seg(segtomb(SDSEG));
	xcb.c_xcbr1 = 0;


	out_multibus(ATTN, 0);	/* let controller do initialization */

	for (tocnt = MAXTOCNT; tocnt && (xcb.c_gate != GATEOPEN); tocnt--) {
	}

	if (tocnt == 0) {
		rminiterr = -1;
		goto out;
	}

	r_ctl[0] = (BUS16 | UNIT0);	/* set this up, initiate uses it */
	rmgo(C_CONFIG, 0, 0, 0, 0, 0);	/* issue configure command */

	for (tocnt = MAXTOCNT; tocnt && (xcb.c_gate != GATEOPEN); tocnt--);

	if (!tocnt) {
		rminiterr = -1;
		goto out;
	}

	if ((pb.p_status & ERRMASK) != NOERR) {
		rminiterr = pb.p_status;
		goto out;
	}

	out:
	xcb.c_ccw = CCWCLR;
	out_local(P_MBTOINT, 0);  /* clear any mbtos caused here */
	splx(savepri);
}


/* 
 * Open a unit.
 */

rmopen(dev, flag)
dev_t	dev;
int	flag;
{
	register	punit;
	register	lunit;
	register	status;

	lunit = minor(dev);
	if (lunit >= LUNIT) {
		u.u_error = ENXIO;
		return;
	}
	punit = ((ctl[lunit].unitcontrol) & PUNITMASK) >> PUNITSHIFT;
	if (r_openf[punit] || rminiterr) {
		u.u_error = ENXIO;
		return;
	}
	r_blkno[punit] = 0;
	r_nxrec[punit] = 65535;
	r_flags[punit] = 0;
	r_ctl[punit] = ctl[lunit].unitcontrol;
	rmtab.b_flags |= B_TAPE;
	r_openf[punit]++;
}


/*
 * Close a unit.
 */

rmclose(dev, flag)
dev_t	dev;
int	flag;
{
	register	punit;
	register	lunit;
	int		status;

	lunit = minor(dev);
	punit = ((ctl[lunit].unitcontrol) & PUNITMASK) >> PUNITSHIFT;
	if (flag == FWRITE ||
	    ((flag & FWRITE) && (r_flags[punit] & T_WRITTEN))) {
		rcommand(dev, C_WEOF | SILENT, 0, 0);
		rcommand(dev, C_WEOF | SILENT, 0, &status);
		if ((status & ERRMASK) == NOERR) {
			rcommand(dev, C_SRCHEOF | C_REV | SILENT, 0, 0);
		}
	}
	if (ctl[lunit].rewflg == REW) {
		rcommand(dev, C_REW | SILENT, 0, 0);
	}
	r_openf[punit] = 0;
}


/*
 * Start a command to a tape unit from within the driver.
 */

rcommand(dev, com, recs, status)
dev_t	dev;
int	com;
int	*recs;
int	*status;
{
	register struct buf	*bp;

	bp = &crmbuf;
	spl5();
	while (bp->b_flags & B_BUSY) {
		bp->b_flags |= B_WANTED;
		sleep((caddr_t) bp, PRIBIO);
	}
	bp->b_flags = B_BUSY | B_READ;
	spl0();
	bp->b_dev = dev;
	bp->b_resid = com;
	bp->b_bcount = (recs != 0) ? *recs : 0;
	bp->b_blkno = 0;
	rmstrategy(bp);
	iowait(bp);
	if (bp->b_flags & B_WANTED) {
		wakeup((caddr_t) bp);
	}
	bp->b_flags = 0;
	if (status != 0) {
		*status = bp->b_resid;
	}
	if (recs != 0) {
		*recs = bp->b_bcount;
	}
}


/*
 * Add a request to the request queue for the controller
 */

rmstrategy(bp)
register struct buf	*bp;
{
	register daddr_t	*p;
	register	lunit;
	register	punit;

	lunit = minor(bp->b_dev);
	punit = ((ctl[lunit].unitcontrol) & PUNITMASK) >> PUNITSHIFT;
	if(bp->b_flags & B_PHYS)
		mapalloc(bp);
	if (bp != &crmbuf) {
		p = &r_nxrec[punit];
		if (*p <= dbtofsb(bp->b_blkno)) {
			if (*p < dbtofsb(bp->b_blkno)) {
				bp->b_flags |= B_ERROR;
				iodone(bp);
				return;
			}
			if (bp->b_flags & B_READ) {
				clear(paddr(bp), BSIZE);
				bp->b_resid = 0;
				iodone(bp);
				return;
			}
		}
		if ((bp->b_flags & B_READ) == 0) {
			r_flags[punit] |= T_WRITTEN;
			*p = dbtofsb(bp->b_blkno)+1;
		}
	}
	bp->av_forw = 0;
	spl5();
	if (rmtab.b_actf == NULL)
		rmtab.b_actf = bp;
	else
		rmtab.b_actl->av_forw = bp;
	rmtab.b_actl = bp;
	if (rmtab.b_active == NULL)
		rmstart();
	spl0();
}

rmstart()
{
	register struct buf	*bp;
	register	lunit;
	register	punit;
	register daddr_t	*blkno;

    loop:
	if ((bp = rmtab.b_actf) == 0)
		return;
	lunit = minor(bp->b_dev);
	punit = ((ctl[lunit].unitcontrol) & PUNITMASK) >> PUNITSHIFT;
	blkno = &r_blkno[punit];
	if (bp == &crmbuf) {
		rmtab.b_active = SCOM;
		rmgo(crmbuf.b_resid | INT, punit, 0, crmbuf.b_bcount, 0, 0);
		goto out;
	}
	if (r_openf[punit] < 0) {
		bp->b_flags |= B_ERROR;
		goto next;
	}
	if (*blkno != dbtofsb(bp->b_blkno)) {
		rmtab.b_active = SSEEK;
		if (*blkno < dbtofsb(bp->b_blkno)) {
			rmgo(C_SPACE | INT, punit, 0,
			     (unsigned) (dbtofsb(bp->b_blkno) - *blkno), 0, 0);
		} else if (dbtofsb(bp->b_blkno) == 0) {
			rmgo(C_REW | INT, punit, 0, 0, 0, 0);
		} else {
			rmgo(C_SPACE | C_REV | INT, punit, 0,
			     (unsigned) (*blkno - dbtofsb(bp->b_blkno)), 0, 0);
		}
		goto out;
	}
	rmtab.b_active = SIO;
	rmstat[lunit].io_ops++;
	blkacty |= (1<<RM0);
	if (bp->b_flags & B_READ) {
		rmgo(C_READ | INT, punit, bp->b_bcount, 0,
			 hiword(bp->b_paddr), loword(bp->b_paddr));
	} else {
		rmgo(C_WRITE | INT, punit, bp->b_bcount, 0,
			 hiword(bp->b_paddr), loword(bp->b_paddr));
	}
	goto out;

next:
	rmtab.b_actf = bp->av_forw;
	iodone(bp);
	goto loop;

	out:
	out_local(P_PIOBDATA, in_local(P_PIOBDATA) & (~TAPELED));
	return;
}

rmintr()
{
	register struct buf	*bp;
	register	lunit;
	register	punit;
	register	state;
	register unsigned	*pbptr;
	register	err;

	out_local(P_SPIC1, in_local(P_SPIC1) | RMINT);  /* ints off */

	if ((bp = rmtab.b_actf) == NULL) {
		return;
	}
	blkacty &= ~(1<<RM0);
	out_local(P_PIOBDATA, in_local(P_PIOBDATA) | TAPELED);
	lunit = minor(bp->b_dev);
	punit = ((ctl[lunit].unitcontrol) & PUNITMASK) >> PUNITSHIFT;
	state = rmtab.b_active;
	rmtab.b_active = 0;
	if((bp == &crmbuf) && (bp->b_resid & SILENT)) {
		goto out;
	}
	err = pb.p_status & ERRMASK;
	if (((err != NOERR) && (err != TIMEOUT)) ||
	    ((pb.p_cmd == C_READ) && (pb.p_count < pb.p_records))) {
		if (err == EOFDETECT) {
			pb.p_count = 0;	/* say that zero bytes were read */
			r_nxrec[punit] = dbtofsb(bp->b_blkno);
			state = SCOM;
			goto out;
		}
		if ((err == OVERRUN) &&
		    (state == SIO)) {
			rmtab.b_active = SOVER;
			rmgo(C_SPACE | C_REV | INT, punit, 0, 1, 0, 0);
			return;
		}
		if ((err == HARDERR) &&
		    (state == SIO) &&
		    ((bp->b_flags & B_READ) == 0) &&
		    (++rmtab.b_errcnt < 10)) {
			rmtab.b_active = SRETRY;
			rmgo(C_SPACE | C_REV | INT, punit, 0, 1, 0, 0);
			return;
		}
		if (r_openf[punit] > 0 && bp != &rrmbuf) {
			r_openf[punit] = -1;
		}
		if (err == 0) {
			err = 0xffff;
		}
		deverr("RM", &rmtab, 0, err>>ERRSHIFT);
		bp->b_flags |= B_ERROR;
		state = SIO;
		fmtberr(&rmtab,0);
		if (rmtab.io_erec)
			logberr(&rmtab,bp->b_flags&B_ERROR);
	}

out:
	switch ( state ) {
	case SIO:
		r_blkno[punit] += (pb.p_count >> BSHIFT);
	case SCOM:
		rmtab.b_errcnt = 0;
		rmtab.b_actf = bp->av_forw;
		if (bp != &crmbuf) {
			bp->b_resid = pb.p_size - pb.p_count;
		} else {
			bp->b_resid = pb.p_status;
			bp->b_bcount = pb.p_count;
		}
		iodone(bp);
		break;
	case SSEEK:
		r_blkno[punit] = dbtofsb(bp->b_blkno);
		break;
	case SRETRY:
		rmtab.b_active = SSEEK;
		rmgo(C_ERASE | INT, punit, 0, 1, 0, 0);
		return;
		break;
	case SOVER:
		break;
	default:
		return;
		break;
	}
	rmstart();
}

rmread(dev)
{
	rmphys(dev);
	physio(rmstrategy, &rrmbuf, dev, B_READ);
}

rmwrite(dev)
{
	rmphys(dev);
	physio(rmstrategy, &rrmbuf, dev, B_WRITE);
}

rmphys(dev)
{
	register lunit;
	register punit;
	daddr_t a;

	lunit = minor(dev);
	punit = ((ctl[lunit].unitcontrol) & PUNITMASK) >> PUNITSHIFT;
	a = u.u_offset >>BSHIFT;
	r_blkno[punit] = a;
	r_nxrec[punit] = a+1;
}

/*
 * control the tape drive
 */

rmioctl(dev, com, addr, flag)
dev_t	dev;
int	com;
caddr_t	addr;
int	flag;
{
	register	punit;
	register	lunit;
	register	i;

	lunit = minor(dev);
	punit = ((ctl[lunit].unitcontrol) & PUNITMASK) >> PUNITSHIFT;
	switch (com) {

	case RMPOSN:
		if(copyin(addr, (caddr_t) &rmcmd, sizeof(rmcmd))) {
			u.u_error = EFAULT;
			goto out;
		}
		for (i = 0; i < NUMLEGAL; i++) {
			if (rmlegal[i] == (rmcmd.rm_cmd & 0xff)) {
				break;
			}
		}
		if (i >= NUMLEGAL) {
			u.u_error = ENXIO;
			goto out;
		}
		rcommand(dev, rmcmd.rm_cmd | SILENT,
			 &rmcmd.rm_cnt, &rmcmd.rm_status);
		if (copyout((caddr_t) &rmcmd, addr, sizeof(rmcmd))) {
			u.u_error = EFAULT;
			goto out;
		}
		break;

	default:
		u.u_error = ENXIO;
		goto out;
		break;
	}

	out:
	return;
}
