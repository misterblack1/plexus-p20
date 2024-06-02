#include "sys/plexus.h"
#include "saio.h"
#include "sys/imsc.h"

/* IMSC standalone tape driver */

#define DISKCAB 0xfe00
#define TAPECAB 0xfe18
#define IMSCWU	0x1f
#define IMSCOWU	0xff

#define reterr ((WUB->cab_etype)<<8 | (WUB->cab_ecode))
#undef MAXTOCNT
#define MAXTOCNT 18500000

int imsctapecount;
extern imscthere();
short ptinitdone;
short firstopen;
int numtapes;
int written;

/* figger out if there is am imsc tape and if so, set imsctapecount
 * to 1, bump numtapes by 1, and return 1.  If none, return 0.
 */
ptinit() {		/* ptthere() from stand/pimsc.c */
	struct cab_def *JUB;
	short stat;

	if(ptinitdone++)
		return imsctapecount;

	if (!imscthere())		/* imsc board not there */
		return(0);
	JUB = (struct cab_def *)TAPECAB;
	JUB->cab_flags=0;		/* idle this CAB */
	JUB = (struct cab_def *)DISKCAB;
	if((stat = (waitimsc(CAB_UTIL,CAB_TPTEST,(long)10000))) == 0)
		imsctapecount = 1;	/* new imsc, has tape */
	if (((JUB->cab_etype<<8)|(JUB->cab_ecode)) == E_CMD)
		imsctapecount = 1;	/* old imsc, assume it has tape */

	numtapes += imsctapecount;
	return(imsctapecount);
}

ptopen(io)
register struct iob *io;
{
	register int status;
	register struct cab_move *WUB;

	if(imsctapecount == 0) {
		errno = ENXIO;
		return -1;
	}

	/* Initialize the tape drive by doing a C_IREW: this enables
	 * controller to read status.  Do this only on the first open.
	 */

	/* imsc tape must be rewound on first open: hdwe restriction */
	if(!firstopen++) {
		WUB = (struct cab_move *)DISKCAB;
		if ((status=ptcommand(C_IREW)) != NORMAL) {
			errno = EIO;
			printf("%s: %s: (pt) %s failed stat=%x err=%x\n",
				myname,io->i_dp->dt_name,
				"rewind on open failed",WUB->cab_estat,status);
			return(-1);
		}
	}
	io->i_atime = 0;	/* for EOF detection */
	return 0;
}

ptclose(io)
register struct iob *io;
{
	register int status;
	register struct cab_move *WUB;

	WUB = (struct cab_move *) DISKCAB;
	if(written) {
		/* write just one EOF: no such thing as EOT on cartridge.
		 * Writing multiple files on a cartridge tape should
		 * be done in one crack without rewinding tape.
		 * Searching to end of a file and then writing it can be
		 * dangerous: sometimes it does what you want,
		 * sometimes it doesn't
		 */
		if ((status=ptcommand(C_IWEOF)) != NORMAL) {
			errno = EIO;
			printf("%s: %s: (pt) %s failed stat=%x, err=%x\n",
				myname,io->i_dp->dt_name,"write eof at close",
				WUB->cab_estat, status);
			return(-1);
		}
	}
	if ((io->i_dp->dt_unit & 04) == 0)
		if ((status=ptcommand(C_IREW)) != NORMAL) {
			errno = EIO;
			printf("%s: %s: (pt) %s failed stat=%x, err=%x\n",
				myname,io->i_dp->dt_name,"rewind at close",
				WUB->cab_estat, status);
			return(-1);
		}
	else 
		/* Position to start of next file: controller cannot do
		 * this; this code should keep track of which file it is
		 * at and position to it. Maybe later.
		 */
		;
	written = 0;
}

ptstrategy(io,func)
register struct iob *io;
int func;
{
	register struct cab_rw *WUB;

	if(io->i_atime == E_TEOS && func == READ) return 0;

	if (io->i_cc % 512 && func != SRCHEOF) {
		errno = EINVAL;
		printf("%s: %s: (pt) io not multiple of 512 bytes\n",
			myname,io->i_dp->dt_name);
		return(-1);
	}

	WUB=(struct cab_rw *)DISKCAB;
	WUB->cab_nblocks=io->i_cc >> S_BLOCKSIZE; /* blks */
	WUB->cab_bufaddr=((long)MBSDSEG << 16) | (long)io->i_ma;
	if(func == SRCHEOF) { 
		/* imsc tape cannot position to a file relative to current
		   position.  you must give it an absolute file number */
		if(io->i_dp->dt_boff != 0) {
			WUB=(struct cab_rw *)DISKCAB;
			((struct cab_move *)WUB)->cab_segmentnum=
				io->i_dp->dt_boff;
			if (tapego(CAB_UTIL,CAB_MOVE)) {
				printf("%s: %s: (pt) %s stat=%x, err=%x\n",
					myname,io->i_dp->dt_name,
					"tape positioning error",
					WUB->cab_estat, reterr);
				errno = EIO;
				return(-1);
			}
		}
		return 0;
	}
	if (tapego(CAB_IO,(func==READ?CAB_RDTP:CAB_WRTP))) {
		errno = EIO;
		printf("%s: %s: (pt): io error, stat=%x, err=%x\n",
			myname,io->i_dp->dt_name,WUB->cab_estat,reterr);
		return(-1);
	}
		/* Have we reached an end of file? */
	if(((WUB->cab_etype << 8) | WUB->cab_ecode) == E_TEOS)
		io->i_atime = E_TEOS;
	if (func == WRITE) written = 1;
		/* rtn actual # xferred.
		 * cab_nblocks is number of blocks not read.
		 */
	return(io->i_cc-(WUB->cab_nblocks << S_BLOCKSIZE)); 
}

tapego(class,op)
unsigned char class,op;
{
	register cab_t *WUB;
	long tocnt;
	long delay;

	WUB=(cab_t *)DISKCAB;
	WUB->cab_class=class;
	WUB->cab_op=op;
	WUB->cab_flag=B_CABGO;
	ledson(TAPELED);
	out_multibus(IMSCOWU,B_M_CMD);
	out_multibus(IMSCWU,B_M_CMD);
	for (tocnt=MAXTOCNT;tocnt && (WUB->cab_flag==B_CABGO);tocnt--) ;
	for (delay=100L;delay;delay--) ;
	out_multibus(IMSCWU,B_M_INTCLR);
	ledsoff(TAPELED);
		/* Have we reached an end of file? */
	if(((WUB->cab_etype << 8) | WUB->cab_ecode) == E_TEOS)
		return (!tocnt);
	return(!tocnt || (WUB->cab_flag==B_CABERR));
}

	/* Initialize the tape drive by doing a C_IREW: this enables
	 * controller to read status.
	 * To position to beginning of file X:
	 *	cab_class = CAB_UTIL
	 *	cab_op =CAB_MOVE
	 *	cab_segmentnum=X
	 * Note: imsc cartridge cannot update any part of a tape:
	 *	it must overwrite from the beginning.  Also, there
	 *	is no such thing as an end-of-tape marker/indication.
	 */
ptcommand(cmd)
int cmd;
{
	register struct cab_move *WUB;

	WUB=(struct cab_move *)DISKCAB;
	WUB->cab_segmentnum=0;
	switch(cmd) {
	case C_IREW:
		if (tapego(CAB_UTIL,CAB_MOVE)) 
			return(reterr);
		break;
	case C_IWEOF:
		if(tapego(CAB_MV,CAB_MARK))
			return(reterr);
		break;
	}
	return(NORMAL);
}
