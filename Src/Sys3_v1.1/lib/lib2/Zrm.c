/*
 * The driver can control up to 4 physical units.
 * 
 * The minor device number selects not only the physical unit
 * to be used but also the type of drive selected by the unit
 * and other drive and controller parameters such as writing
 * speed and bus locking mode. The msb of the minor device number
 * is used to control rewind on close. If the bit is on then
 * close will not cause a rewind on the tape unit.
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
 * CHANATTN:	The constant in this macro is the controller channel attention
 *		port I/O address.
 *
 * ctl:		This array is indexed by minor device number (after dropping
 *		the msb). The contents of the array element are the parameters
 *		used by the controller for all operations on the unit.
 *
 * LUNIT:	This is the number of logical units supported by the driver.
 *		There is one entry in ctl for each logical unit.
 *
 */

#include "sys/plexus.h"
#include "saio.h"

/* misc defines */

#define	SCP	((struct scp_struct *) 0xfff6)
#define	CHANATTN	out_multibus(0xaaaa, 0x1)
#define	CHANRESET	out_multibus(0xaaab, 0x1)
#define	PUNIT	4
#define	LUNIT	16
#define	LUNITMASK	0x7f
#define	NOREW		0x80
#define	xmemto8086seg(s)	((s) << 12)	/* convert to 8086 seg num */
#define	MAXTOCNT	50000		/* wait counter */
#define	GATECLOSED	0xff
#define	GATEOPEN	0
#define	CCWNORM		0x11
#define	CCWCLR		0x09
#define	T_WRITTEN	1
#define	SSEEK		1
#define	SIO		2
#define	SCOM		3

/* parameter buffer control word defines */

#define	PUNITSHIFT	2
#define	PUNITMASK	0x000c
#define	BUS16	0x8000
#define	DMALOK	0x4000
#define	CONT	0x1000
#define	HISPEED	0x0800
#define	REVERSE	0x0400
#define	BUSLOK	0x0080
#define	INTR	0x0020
#define	UNIT0	0x0000
#define	UNIT1	0x0004
#define	UNIT2	0x0008
#define	UNIT3	0x000c

/* drive types */

#define	STREAMER	0
#define	CIPHER		1
#define	PERTEC		2

/* command types */

#define	C_REV		0x4000
#define	INT		0x8000
#define	C_CONFIG	0x00
#define C_OLREW		0x04
#define C_NOP		0x20
#define	C_STATUS	0x28
#define	C_READ		0x2c
#define	C_WRITE		0x30
#define	C_REW		0x34
#define	C_WEOF		0x40
#define	C_SRCHEOF	0x44
#define	C_SPACE		0x48
#define	C_ASSIGN	0x74

/* parameter buffer status word defines */

#define	ENTERED		0x8000
#define	COMPLETED	0x4000
#define	ERRMASK		0x1f00
#define	ERRSHIFT	8
#define	ONLINE		0x0040
#define	LOADPOINT	0x0020
#define	EOT		0x0010
#define	READY		0x0008
#define	FORMATBSY	0x0004
#define	WRITEPROTECT	0x0002

/* error codes */

#define	NOERR		0x00
#define TIMEOUT		0x0f<<ERRSHIFT
#define	EOFDETECT	0x15<<ERRSHIFT


struct	scp_struct {
	char	s_scpr1;
	char	s_sysbus;
	unsigned	s_scboff;
	unsigned	s_scbseg;
};

struct	scb_struct {
	char	s_scbr1;
	char	s_soc;
	unsigned	s_ccboff;
	unsigned	s_ccbseg;
} scb;

struct	ccbt_struct {
	char	c_gate;
	char	c_ccw;
	unsigned	c_pboff;
	unsigned	c_pbseg;
	unsigned	c_ccbr1;
} ccbt;

struct	pb_struct {
	char	p_pbr1;
	char	p_cmd;
	unsigned	p_pbr2;
	unsigned	p_control;
	unsigned	p_count;
	unsigned	p_size;
	unsigned	p_records;
	unsigned	p_dboff;
	unsigned	p_dbseg;
	unsigned	p_status;
	unsigned	p_l1;
	unsigned	p_l2;
} pb;


int written;
int rmtapecount;
int rminitdone;
int numtapes;

/*
 * This routine initiates a request to the controller. Note that the
 * reverse and interrupt control bits are passed in as part of the cmd
 * parameter. Note also that the rest of the control word is gotten
 * from the one set up by the open on the device.
 */

rmgo(cmd, unit, size, recs, xmem, buf)
unsigned	cmd;
unsigned	unit;
unsigned	size;
unsigned	recs;
unsigned	xmem;
char		*buf;
{
	
	pb.p_pbr1 = 0;
#ifdef V7
	pb.p_cmd = cmd;
	pb.p_pbr2 = 0;
	pb.p_control = (BUS16 | UNIT0);
#else
	pb.p_cmd = cmd & (~C_REV);
	pb.p_pbr2 = 0;
	pb.p_control = (BUS16 | UNIT0) | (cmd & C_REV ? REVERSE : 0);
#endif
	if (cmd == C_SRCHEOF)
		pb.p_control |= HISPEED;
	pb.p_count = 0;
	pb.p_size = size;
	pb.p_records = recs;
	pb.p_dboff = (unsigned) buf;
	pb.p_dbseg = xmemto8086seg(xmem);

	ccbt.c_gate = GATECLOSED;

	CHANATTN;
}


/*
 * The following routine is called early in unix startup to initialize
 * the controller.
 */

rminit()
{
	register	tocnt;


	if(rminitdone++)
		return rmtapecount;

	CHANRESET;

	SCP->s_scpr1 = 0;
	SCP->s_sysbus = 1;
	SCP->s_scboff = (unsigned) &scb;
	SCP->s_scbseg = xmemto8086seg(MBSDSEG);

	pb.p_cmd = C_NOP;

	scb.s_scbr1 = 0;
	scb.s_soc = 3;
	scb.s_ccboff = (unsigned) &ccbt;
	scb.s_ccbseg = xmemto8086seg(MBSDSEG);

	ccbt.c_gate = GATECLOSED;
	ccbt.c_ccw = CCWNORM;
	ccbt.c_pboff = (unsigned) &pb;
	ccbt.c_pbseg = xmemto8086seg(MBSDSEG);
	ccbt.c_ccbr1 = 0;

	CHANATTN;	/* let controller do initialization */

	ledson(TAPELED);
	for (tocnt = MAXTOCNT; tocnt && (ccbt.c_gate != GATEOPEN); tocnt--) {
	}
	ledsoff(TAPELED);

	if (tocnt == 0 && !numtapes) {
		return(0);
	}
	if (tocnt == 0)
		return 0;

	rmgo(C_CONFIG, 0, 0, 0, 0, 0);	/* issue configure command */

	ledson(TAPELED);
	while (ccbt.c_gate != GATEOPEN) {	/* wait for it to finish */
	}
	ledsoff(TAPELED);
	if ((pb.p_status & ERRMASK) == NOERR)
		rmtapecount = 1;

	numtapes += rmtapecount;
	return (rmtapecount);
}


/* 
 * Open a unit.
 */

rmopen(io)
register struct iob *io;
{
	if(rmtapecount == 0) {
		errno = ENXIO;
		return -1;
	}
	io->i_atime = 0;
	return 0;
}


/*
 * Close a unit
 */
rmclose(io)
register struct iob *io;
{
	if (written) {
		rmstrategy(io, C_WEOF);
		rmstrategy(io, C_WEOF);
	}
#ifdef V7
	rmstrategy(io, C_REW);
#else
	if ((io->i_dp->dt_unit & 04) == 0)
		rmstrategy(io,C_REW);
	else if (written)
		rmstrategy(io, C_SRCHEOF | C_REV);

	written = 0;
#endif
}

rmstrategy(io, func)
register struct iob *io;
{
	register unit, den, errcnt;
	register int status;
	int cmd;

#ifdef V7
	unit = io->i_unit;
#else
	unit = io->i_dp->dt_unit;
#endif
	if(io->i_atime == EOFDETECT && func == READ) return 0;

	errcnt = 0;

	ledson(TAPELED);
	while (ccbt.c_gate != GATEOPEN)
		;
	ledsoff(TAPELED);

	if (func == READ)
		cmd = C_READ;
	else if (func == WRITE) {
#ifdef V7
		written = cmd = C_WRITE;
#else
		written = 1;
		cmd = C_WRITE;
#endif
	}
	else if (func == SRCHEOF) {
		if(io->i_dp->dt_boff == 0) return 0;
		cmd = C_SRCHEOF;
	}
	else
		cmd = func;

	do {
		rmgo(cmd, unit, io->i_cc, 0, MBSDSEG, io->i_ma);

		ledson(TAPELED);
		while (ccbt.c_gate != GATEOPEN)
			;
		ledsoff(TAPELED);
	
		status = pb.p_status & ERRMASK;
	
		if ((status != NOERR)  && (status != EOFDETECT)
			&& ((func != READ) || (status != TIMEOUT))) {
			errno = EIO;
			printf("%s: %s: (rm) error=0x%x\n", myname,
				io->i_dp->dt_name,status>>ERRSHIFT);
			return(-1);
		}
	} while((cmd == C_SRCHEOF) && --(io->i_dp->dt_boff));

#ifndef V7
	if(status == EOFDETECT) io->i_atime = EOFDETECT;

	if (status == EOFDETECT || cmd == C_SRCHEOF) return 0;
#endif
	
	if (pb.p_records > pb.p_size) {
		errno = EIO;
		printf("%s: %s: (rm) requested=0x%x, block size=0x%x, %s0x%x\n",
			myname,io->i_dp->dt_name, pb.p_size,pb.p_records,
			"returned=",pb.p_count);
	}
	return(pb.p_count);
}

tapedump(io)
register struct iob *io;
{
	register unsigned i;

#ifdef V7
	rmopen(io->i_unit);
#else
	rmopen(io->i_dp->dt_unit);
#endif
	io->i_ma = 0;
	for (i=0; i<32; i++) {
		io->i_cc = 2048;
		rmstrategy(io, C_WRITE);
		io->i_ma += 2048;
	}
	rmstrategy(io, C_WEOF);
	rmstrategy(io, C_WEOF);
#ifdef V7
	rmclose(io->i_unit);
#else
	rmclose(io->i_dp->dt_unit);
#endif
}
