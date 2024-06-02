
/*
 * This file contains code for the fast tape back-up of disk.
 */


#include "saio.h"
#include "sys/is.h"
#include "sys/plexus.h"

#define	ERRFLAGS	0xc0	/* error flags in cib.c_opstat */
#define	DTBLKBUF	0x20	/* number of disk blocks per buffer */
#define	DTBUFNUM	4	/* number of buffers */
#define DTBUFSIZE	0x4000
/*
#define	DTBUFSIZE	DTBLKBUF*512 */	/* number of chars. per buffer */


/*
#define	dbunit	((struct db_i *) ((WUA << 4) - 0x4b0))
#define	dbzero	((struct db_i *) ((WUA << 4) - 0x4a8))
 */
#define	D_CYLS	823			/* default number of cylinders */
#define	D_RHEADS	0		/* default number of removable heads */
#define	D_FHEADS	4		/* default number of fixed heads */
#define D_BPS	512			/* default bytes per sector */
#define	D_SPT	35			/* default num. of sectors per track */
#define	D_ALTCYLS	7		/* default number of alternate cyls. */
#define D_INTERL	5		/* default interleave factor */
#define	TT_MSTREAM	0		/* Microstreamer tape type */
#define TT_CSS		1		/* Cipher Start/Stop tape type */
#define TT_PERTEC	2		/* Pertec tape type */
char	p_unit, pt_unit, p_ttype;
int	p_bus16, p_dmalok, p_cont, p_speed, p_buslok;
char	restore;
struct	db_i {
	unsigned	db_cyls;
	char	db_rheads;
	char	db_fheads;
	char	db_bpsl;
	char	db_spt;
	char	db_altcyls;
	char	db_bpsh;
};

struct s_iblk {
	unsigned cylinders;
	char remheads;
	char fixheads;
	char lsb_sectsize;
	char sectpertrack;
	char altcylinders;
	char msb_sectsize;
};

struct	db_i	dbunit;
struct	db_i dbzero = { 0, 0, 0, 0, 0, 0, 0 };
char	wbuf[10];
char	debug;
char	memdis;
struct	dtbufs {
	char	dt_char[2];
};
/*
} dtbufs[DTBUFNUM];;
 */
struct	{  /* initialized below for restartability */
	char	*dtbufsp;
} dtbufs[4] = { (char *)0x0000, (char *)0x4000, (char *)0x8000, (char *)0xc000 };
/*
} dtbufs[2] = { (char *)0x0000, (char *)0x8000 };
*/

char	buf[512];			/* buf for input */
char	dtbuf_assigned;		/* number of buffers assigned */
char	pb_assigned;		/* number of paramter blocks assigned */
int	dtbuf_next;		/* next buffer to assign ordinal */
char	pb_current;		/* current parameter block being used */
char	pb_lstlnk;		/* last linked parameter block */
char	tape_done;		/* tape is done flag */
char	disk_done;		/* disk is done flag */
char	tape_running;		/* tape running flag */
unsigned long	max_blk;	/* last block to transfer */
long	tocnt;			/* time-out counter */
unsigned	c_next;		/* next cylinder to access on disk */
unsigned	dbc_next;	/* next count for disk access */
unsigned	tbc_next;	/* next count for tape access */
unsigned	sect_trk;	/* sectors per track */
unsigned	head_cyl;	/* heads per cylinder */
unsigned	h_next;		/* next head to access on disk */
unsigned	s_next;		/* next sector to access on disk */
unsigned long	dnext_blk;	/* next block on disk to access */
unsigned long	tnext_blk;	/* next block ontape to read/write */
/* misc defines */

#define	SCP	((struct scp_struct *) 0xfff6)
#define	CHANATTN	out_multibus(0xaaaa, 0x1)
#define	SOFTRESET	out_multibus(0xaaab, 0x0)
#define	PUNIT	4
#define	LUNIT	16
#define	LUNITMASK	0x7f
#define	NOREW		0x80
#define	GATECLOSED	0xff
#define	GATEOPEN	0
#define	CCWNORM		0x11
#define	CCWCLR		0x09
#define	T_WRITTEN	1
#define	SSEEK		1
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
#define	LINKIT	0x0040	
#define	INTR	0x0020
#define	TUNIT0	0x0000
#define	TUNIT1	0x0004
#define	TUNIT2	0x0008
#define	TUNIT3	0x000c

/* drive types */

#define	STREAMER	0
#define	CIPHER		1
#define	PERTEC		2

/* command types */

#define	REV		0x4000
#define	INT		0x8000
#define	C_CONFIG	0x00
#define	C_STATUS	0x28
#define	C_READ		0x2c
#define	C_WRITE		0x30
#define	C_REW		0x34
#define	C_WEOF		0x40
#define	C_SRCHEOF	0x44
#define	C_SPACE		0x48
#define C_ERASE		0x4c
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
#define E_TODBF	0x01
#define E_TODFR	0x02
#define E_TORF	0x03
#define E_TORT	0x04
#define E_TODBT	0x05
#define E_TOMEM	0x06
#define	E_BLANK		0x07
#define E_DIAGERR	0x08
#define E_UNEXEOT	0x09
#define E_NOELIM	0x0a
#define E_OVERUNDER	0x0b
#define E_NOUSE1	0x0c
#define E_RDPARITY	0x0d
#define E_PROMCHKSUM	0x0e
#define E_TOTAPE	0x0f
#define E_NOTRDY	0x10
#define E_NORING	0x11
#define E_NOUSE2	0x12
#define E_NOJUMPER	0x13
#define E_BADLINK	0x14
#define E_FILEMARK	0x15
#define E_PARAMERR	0x16
#define E_NOUSE3	0x17
#define E_HARDERR	0x18
#define	E_STREAMEND	0x19
#define	EOFDETECT	0x15

struct {
	char	*errmsg;
} ems[] = {
	"No unrecoverable error!",
	"Time-out - Data Busy False!",
	"Time-out - Data Busy False, Fmtr Busy False, Rdy True!",
	"Time-out - Ready False!",
	"Time-out - Ready True!",
	"Time-out - Data Busy True!",
	"Blank tape encountered while reading!",
	"Micro-Diagnostic Error!",
	"Unexpected End-of-Tape!",
	"Hard/Soft Error could not be eliminated!",
	"Read overflow/Write underflow occurred!",
	"Unused error code received!",
	"Read Parity error between TAPEMASTER and drive!",
	"PROM checksum error!",
	"Tape Time-out!",
	"Tape not ready!",
	"No write ring!",
	"Unused error code received!",
	"No diagnostic mode jumper!",
	"Link attempted in illegal situation!",
	"Unexpected filemark received!",
	"Parameter error!",
	"Unused error code received!",
	"Unidentified hardware error!",
	"Streaming stopped by System!"
};

char	r_flags[PUNIT];
char	r_openf[PUNIT];
daddr_t	r_blkno[PUNIT];
daddr_t	r_nxrec[PUNIT];
unsigned	r_ctl[PUNIT];
int	ctrlinitstat;
int	assignparm;

struct	scp_struct {
	char	s_scpr1;
	char	s_sysbus;
	unsigned	s_scboff;
	unsigned	s_scbseg;
};

struct	scb_struct {
	char	s_scbr1;
	char	s_soc;
	unsigned	s_ccbtoff;
	unsigned	s_ccbtseg;
} scb;

struct	ccbt_struct {
	char	c_gate;
	char	c_ccw;
	unsigned	c_pboff;
	unsigned	c_pbseg;
	unsigned	c_ccbtr1;
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
	unsigned	p_lnkoff;
	unsigned	p_lnkseg;
} pb[DTBUFNUM];

struct tlbl_struct {
	char	plxmsg[32];
	char	tdate[26];
	char	usrmsg[16];
	char	brmsg[257];
} lbl;

char	plxmsgd[] = { "Plexus Fast Disk to Tape Backup\n" };
char	usrmsgd[] = { "Backup Message: " };

tinit()
{
	int	i;

	SOFTRESET;

	pb_lstlnk = ( DTBUFNUM - 1 );
	tape_running = 0;
	tape_done = 0;

	SCP->s_scpr1 = 0;
	SCP->s_sysbus = 1;
	SCP->s_scboff = (unsigned) &scb;
	SCP->s_scbseg = xmemto8086seg(MBSDSEG);

	scb.s_scbr1 = 0;
	scb.s_soc = 3;
	scb.s_ccbtoff = (unsigned) &ccbt;
	scb.s_ccbtseg = xmemto8086seg(MBSDSEG);

	ccbt.c_gate = GATECLOSED;
	ccbt.c_ccw = CCWNORM;
	ccbt.c_pboff = (unsigned) &pb[0];
	ccbt.c_pbseg = xmemto8086seg(MBSDSEG);
	ccbt.c_ccbtr1 = 0;

	if ( debug ) {
	{	int	i;
		int	*iptr;
		iptr = (int *)0xfff6;
		printf( "SCP = " );
		for ( i = 0; i < 3; i++ )
			printf( "%x, ", *iptr++ );
		printf( "\n" );
		iptr = (int *)&scb;
		printf( "SCB = " );
		for ( i=0; i<3; i++ )
			printf( "%x, ", *iptr++ );
		printf ( "\nCCB = " );
		iptr = (int *)&ccbt;
		for ( i=0; i<4; i++ )
			printf( "%x, ", *iptr++ );
		printf( "\n" );
		mywaitgo();
	}
	}
	CHANATTN;

	for (tocnt = MAXTOCNT; tocnt && (ccbt.c_gate == GATECLOSED); tocnt--) {
	}

	if (tocnt == 0) {
		printf( "Can't wake-up tape controller\n" );
		endit();
	}
	if ( debug )
		printf( "Tape controller wake-up ok!\n" );

	pb[0].p_pbr1 = 0;
	pb[0].p_cmd = C_CONFIG;
	pb[0].p_pbr2 = 0;
	pb[0].p_control = 0;
	pb[0].p_count = 0;
	pb[0].p_size = 0;
	pb[0].p_records = 0;
	pb[0].p_dboff = 0;
	pb[0].p_dbseg = 0;
	pb[0].p_status = 0;
	pb[0].p_lnkoff = 0;
	pb[0].p_lnkseg = 0;
	ccbt.c_gate = GATECLOSED;
	if ( debug ) {
	{	int	i;
		int	*iptr;
		iptr = (int *)0xfff6;
		printf( "SCP = " );
		for ( i = 0; i < 3; i++ )
			printf( "%x, ", *iptr++ );
		printf( "\n" );
		iptr = (int *)&scb;
		printf( "SCB = " );
		for ( i=0; i<3; i++ )
			printf( "%x, ", *iptr++ );
		printf ( "\nCCB = " );
		iptr = (int *)&ccbt;
		for ( i=0; i<4; i++ )
			printf( "%x, ", *iptr++ );
		printf( "\nPB[0] = " );
		iptr = (int *)&pb[0];
		for ( i = 0; i < 11; i++ )
			printf( "%x, ", *iptr++ );
		printf( "\n" );
		mywaitgo();
	}
	}

	CHANATTN;

	for ( tocnt = MAXTOCNT; tocnt && (ccbt.c_gate == GATECLOSED); tocnt-- ) {
	}
	if ( !tocnt || (( pb[0].p_status & ERRMASK ) != NOERR ) ) {
		printf( "Can't configure tape drive\n" );
		endit();
	}
	pb[0].p_records = ( 0xff & ~( 3 << ( pt_unit * 2))) | ( p_ttype << ( pt_unit * 2 ));
	pb[0].p_status = 0;
	pb[0].p_cmd = C_ASSIGN;
	ccbt.c_gate = GATECLOSED;
	if ( debug ) {
	{	int	i, j;
		int	*iptr;
		iptr = (int *)0xfff6;
		printf( "SCP = " );
		for ( i = 0; i < 3; i++ )
			printf( "%x, ", *iptr++ );
		printf( "\n" );
		iptr = (int *)&scb;
		printf( "SCB = " );
		for ( i=0; i<3; i++ )
			printf( "%x, ", *iptr++ );
		printf ( "\nCCB = " );
		iptr = (int *)&ccbt;
		for ( i=0; i<4; i++ )
			printf( "%x, ", *iptr++ );
		printf( "\nPB[0] = " );
		iptr = (int *)&pb[0];
		for ( j=0; j<11; j++ )
			printf( "%x, ", *iptr++ );
		printf( "\n" );
		mywaitgo();
	}
	}
	CHANATTN;

	for ( tocnt = MAXTOCNT; tocnt && !(pb[0].p_status & COMPLETED); tocnt-- ) {
	}
	if ( debug ) {
		printf( "tocnt = %ld\n", tocnt );
		printf( "pb[0].p_status = %x\n", pb[0].p_status );
		mywaitgo();
	}
	if ( !tocnt || (( pb[0].p_status & ERRMASK ) != NOERR ) ) {
		printf( "Can't assign tape drive\n" );
		printf( "status = %x\n", ((pb[0].p_status & ERRMASK) >> 8) );
		endit();
	}
	pb[0].p_records = 0;
	if ( debug ) {
		printf( "Tape initialization complete!\n" );
		mywaitgo();
	}
}

initpb()
{
	int	i;

	for ( i = 0; i < DTBUFNUM; i++ ) {
		if ( restore )
			pb[i].p_cmd = C_READ;
		else pb[i].p_cmd = C_WRITE;
		pb[i].p_control = ( p_bus16 | p_dmalok | p_cont | p_speed
				    | p_buslok | (pt_unit << 2) );
/*
		pb[i].p_size = ( DTBLKBUF * 512 );
 */
		pb[i].p_size = 0x8000;
		pb[i].p_pbr1 = 0;
		pb[i].p_pbr2 = pb[i].p_count = pb[i].p_records = 0;
		pb[i].p_dboff = pb[i].p_dbseg = pb[i].p_status = 0;
		pb[i].p_lnkoff = pb[i].p_lnkseg = 0;
		if ( debug ) {
			int	j;
			int	*iptr;
			iptr = (int *)&pb[i];
			for ( j = 0; j < 11; j++ ) {
				printf( "%x, ", *iptr++ );
			}
			printf( "\n" );
			mywaitgo();
		}
	}
}

rwtape()
{
	pb[0].p_cmd = C_REW;
	pb[0].p_control &= ~LINKIT;
	pb[0].p_status = 0;
	ccbt.c_pboff = (unsigned)&pb[0];
	ccbt.c_gate = GATECLOSED;
	CHANATTN;

	for ( tocnt = MAXTOCNT*90; tocnt && !( pb[0].p_status & COMPLETED); tocnt-- ) {
	}

	if ( !tocnt || (( pb[0].p_status & ERRMASK) != NOERR) ) {
		printf( "Can't rewind tape.\n" );
		printf( "status = %x\n", ((pb[0].p_status & ERRMASK) >> 8) );
	}
}


sttape()
{
	if ( (pb_current == pb_lstlnk) && ( disk_done || tape_done ) )
		tape_done++;
	else {
		ccbt.c_pboff = (unsigned)&pb[pb_current];
		ccbt.c_gate = GATECLOSED;
		tape_running++;
		if ( debug ) {
			printf( "pb's before start of tape!\n" );
			disbuf( &pb[0], (int)(&pb[DTBUFNUM] - &pb[0]) );
			mywaitgo();
		}
		CHANATTN;
		if ( debug ) {
			printf( "Tape started\n" );
		}
	}
}

doitdsk( func, cyl, head, sect, dboff, dbseg, dbcnt )
unsigned	cyl, dboff, dbseg, dbcnt;
char	func, head, sect;
{
	int x;

	iop.i_function = func;
	iop.i_cylinder = cyl;
	iop.i_sector = sect;
	iop.i_head = head;
	iop.i_dboff = dboff;
	iop.i_dbseg = dbseg;
	iop.i_rc1 = dbcnt;
	cib.c_opstat = 0;
	cib.c_statsem = 0;
	if ( debug ) {
		{	int i,j;
			int *iptr;
			iptr = (int *)&ccb;
			printf( "CCB = " );
			for (i=0; i<8; i++) {
				printf("%x, ",*iptr++);
			}
			printf("\n");
			iptr = (int *)&cib;
			printf( "CIB = " );
			for (i=0; i<8; i++) {
				printf("%x, ",*iptr++);
			}
			printf("\n");
			iptr = (int *)&iop;
			printf( "IOP = " );
			for (i=0; i<15; i++) {
				printf("%x, ",*iptr++);
			}
			printf( "\n" );
		}
	mywaitgo();
	}

	for ( ;; ) {
		out_multibus( WUA, OP_START );
		x = in_local( 0x7f90 );
		if ( !( x & 4 ) )
			break;
		out_local( 0x7fb3, 0 );
		printf( "*********************received I/O time-out!\n" );
	}


	for ( tocnt = 250000; tocnt && !cib.c_statsem; tocnt-- ) {
	}

	if ( !tocnt || ( cib.c_opstat & ERRFLAGS ) ) {
		if ( !tocnt ) {
			printf( "Time-out on disk request!\n" );
			printf( "For debug we'll try again\n" );
			out_multibus( WUA, OP_START );
			for ( tocnt = MAXTOCNT; tocnt && !cib.c_statsem; tocnt-- ) { } 
			if ( cib.c_statsem && !(cib.c_opstat & ERRFLAGS) ) {
				goto good2;
			}
			printf( "One more try!!!!!\n" );
			out_multibus( WUA, OP_START );
			for ( tocnt=MAXTOCNT; tocnt && !cib.c_statsem; tocnt-- ){ }
			if ( cib.c_statsem && !(cib.c_opstat & ERRFLAGS) ) {
				goto good3;
			}
			printf( "Second try no good either!\n" );
			mywaitgo();
		} else {
			printf( "Disk error = %x\n", (cib.c_opstat & 0xff) );
			derror();
		}
		endit();
		good2:
		printf( "Second try worked!\n" );
		goto goodout;
		good3:
		printf( "Third time worked!\n" );
		mywaitgo();
	}
	goodout:
	if ( !(cib.c_opstat & 1) ) {
		printf( "All looks complete but not!\n" );
		{	int i,j;
			int *iptr;
			iptr = (int *)&ccb;
			printf( "CCB = " );
			for (i=0; i<8; i++) {
				printf("%x, ",*iptr++);
			}
			printf("\n");
			iptr = (int *)&cib;
			printf( "CIB = " );
			for (i=0; i<8; i++) {
				printf("%x, ",*iptr++);
			}
			printf("\n");
			iptr = (int *)&iop;
			printf( "IOP = " );
			for (i=0; i<15; i++) {
				printf("%x, ",*iptr++);
			}
			printf( "\n" );
		}
		mywaitgo();
	}
	if ( debug ) {
		printf( "Disk read/write buffer!\n" );
		disbuf( iop.i_dboff, iop.i_rc1 );
		chkbuf( iop.i_dboff, iop.i_rc1 );
		mywaitgo();
	}
}

disbuf( adr, siz )
int	*adr, siz;
{
	int i, j, k;

	for ( i = 0; i < 15; i++ ) {
		for ( j = 0; j < 13; j++ ) {
			printf( " %x,", *adr++ );
		}
		printf( "\n" );
		for ( k=5000; k; k-- ) { }
	}
}

rddisk( bufadr, bufseg )
unsigned	bufseg;
struct	dtbufs	*bufadr;
{
	int	reltrk;


	if ( debug )
		printf( "rddisk bufadr = %x\n", bufadr );
	doitdsk( DF_READ, c_next, h_next, s_next, bufadr, bufseg, dbc_next );

	if ( ( dnext_blk += DTBLKBUF ) < max_blk ) {
		reltrk = dnext_blk / sect_trk;
		s_next = dnext_blk % sect_trk;
		c_next = reltrk / head_cyl;
		h_next = reltrk % head_cyl;
		if ( ( dnext_blk + DTBLKBUF ) > max_blk )
			dbc_next = ( max_blk - dnext_blk ) * 512;
	} else {
		disk_done++;
		printf( "Disk reads complete!\n" );
	}
	if ( debug )
		printf( "dnext_blk = %ld, max_blk = %ld\n", dnext_blk, max_blk );
}


lnkbuf( bufadr, bufseg )
unsigned	bufseg;
struct	dtbufs	*bufadr;
{
	int	pbnext;

	if ( ( pbnext = (pb_lstlnk + 1) ) >= DTBUFNUM )
		pbnext = 0;
	if ( debug ) {
		printf( "IN - tnext_blk= %ld, tbc_next = %x\n", tnext_blk , tbc_next );
	}
	if ( tnext_blk >= max_blk )
		return(0);
	if ( ( tnext_blk + DTBLKBUF ) > max_blk )
		tbc_next = ( max_blk - tnext_blk ) * 512;
	pb[pbnext].p_size = tbc_next;
	/*
	if ( pb[pbnext].p_size != (unsigned)0x8000 ) {
		printf( "Short tape access = %x\n", pb[pbnext].p_size );
		printf( "tnext_blk = %d, DTBLKBUF = %d\n", tnext_blk, 
				DTBLKBUF );
		printf( "dnext_blk = %d, dbc_next = %d\n", dnext_blk,
				dbc_next );
	}
	*/
	tnext_blk += DTBLKBUF;
	if ( debug ) {
		printf( "OUT - tnext_blk = %ld, tbc_next = %x\n", tnext_blk, tbc_next );
		mywaitgo();
	}
	if ( debug ) {
		printf( "bufseg = %x\n", bufseg );
	}
	pb[pbnext].p_dboff = (unsigned)bufadr;
	pb[pbnext].p_dbseg = bufseg;
	pb[pbnext].p_control &= ~LINKIT;
	pb[pb_lstlnk].p_lnkoff = (unsigned)&pb[pbnext];
	pb[pb_lstlnk].p_lnkseg = xmemto8086seg( MBSDSEG );
	pb[pb_lstlnk].p_control |= LINKIT;
	pb_lstlnk = pbnext;
}


endit()
{
	exit(1);
}

/*
taperror( p )
struct	pb_struct	*p;
{

	int	stat;

	stat = (p->p_status & ERRMASK) >> ERRSHIFT;
	printf( "Tape error = %x, %s\n", stat, ems[stat].errmsg );
	switch ( stat ) {

	case E_UNEXEOT:

		unload();
		printf( "Load next tape!\n" );
		tapstat( READY | ONLINE | LOADPOINT );
		break;
	}

}
 */
chktsent()
{
	if ( (pb[pb_current].p_status & COMPLETED) || (pb[pb_current].p_status & ERRMASK) ) {
		if ( debug )
			printf( "Tape complete!\n" );
		if ( pb[pb_current].p_status & ERRMASK ) {
			printf( "Tape error = %x\n",
				(( pb[pb_current].p_status & ERRMASK )
				   >> ERRSHIFT ) );
			if ( ((pb[pb_current].p_status & ERRMASK) >> ERRSHIFT)
				!= E_NOELIM )
				endit();
			else tprecover();
		} else {
			if ( debug ) {
				chkbuf( pb[pb_current].p_dboff, pb[pb_current].p_count );
			}
			pb[pb_current].p_control &= ~LINKIT;
			pb[pb_current].p_status = 0;
			dtbuf_assigned--;
			pb_assigned--;
			if ( ( pb_current == pb_lstlnk ) && disk_done ) {
				tape_done++;
			} else {
				if ( ++pb_current == DTBUFNUM )
					pb_current = 0;
			}
		}
	}
	if ( ccbt.c_gate == GATEOPEN )
		tape_running = 0;
}

struct pb_struct pbr;
tprecover()
{

	struct pb_struct pbr;

	printf( "Tape recovery - backup and erase!\n" );
	pbr.p_pbr1 = 0;
	pbr.p_cmd = C_SPACE;
	pbr.p_pbr2 = 0;
	pbr.p_control = ( p_bus16 | p_dmalok | p_cont | p_speed
			  | p_buslok | (pt_unit << 2) | REVERSE );
	pbr.p_count = 0;
	pbr.p_size = 0;
	pbr.p_records = 1;
	pbr.p_dboff = 0;
	pbr.p_dbseg = 0;
	pbr.p_status = 0;
	pbr.p_lnkoff = 0;
	pbr.p_lnkseg = 0;

	ccbt.c_pboff = (unsigned)&pbr;
	ccbt.c_gate = GATECLOSED;

	if ( debug ) {
	{	int	i;
		int	*iptr;
		printf( "Set up to space backwards!!\n" );
		iptr = (int *)0xfff6;
		printf( "SCP = " );
		for ( i = 0; i < 3; i++ )
			printf( "%x, ", *iptr++ );
		printf( "\n" );
		iptr = (int *)&scb;
		printf( "SCB = " );
		for ( i=0; i<3; i++ )
			printf( "%x, ", *iptr++ );
		printf ( "\nCCB = " );
		iptr = (int *)&ccbt;
		for ( i=0; i<4; i++ )
			printf( "%x, ", *iptr++ );
		printf( "\nPB[0] = " );
		iptr = (int *)&pbr;
		for ( i = 0; i < 11; i++ )
			printf( "%x, ", *iptr++ );
		printf( "\n" );
		mywaitgo();
	}
	}
	CHANATTN;

	for ( tocnt = (MAXTOCNT * 5); tocnt && !(pbr.p_status & COMPLETED); tocnt-- ) 	{ }

	for ( tocnt = MAXTOCNT/4 ; tocnt; tocnt-- ) { }

	pbr.p_cmd = C_ERASE;
	pbr.p_records = 1;
	pbr.p_control &= ~REVERSE;
	pbr.p_status = 0;

	ccbt.c_gate = GATECLOSED;

	if ( debug ) {
	{	int	i;
		int	*iptr;
		iptr = (int *)0xfff6;
		printf( "Set up to erase!\n" );
		printf( "SCP = " );
		for ( i = 0; i < 3; i++ )
			printf( "%x, ", *iptr++ );
		printf( "\n" );
		iptr = (int *)&scb;
		printf( "SCB = " );
		for ( i=0; i<3; i++ )
			printf( "%x, ", *iptr++ );
		printf ( "\nCCB = " );
		iptr = (int *)&ccbt;
		for ( i=0; i<4; i++ )
			printf( "%x, ", *iptr++ );
		printf( "\nPB[0] = " );
		iptr = (int *)&pbr;
		for ( i = 0; i < 11; i++ )
			printf( "%x, ", *iptr++ );
		printf( "\n" );
		mywaitgo();
	}
	}
	CHANATTN;

	for ( tocnt = (MAXTOCNT * 5); tocnt && !(pbr.p_status & COMPLETED); tocnt-- ) 	{ }

	for ( tocnt = MAXTOCNT/4 ; tocnt; tocnt-- ) { }
	pb[pb_current].p_status = 0;
	sttape();

}
chkbuf( buf, siz )
char	*buf;
int	siz;
{
	int	i;

	for ( i = 0; i < siz; i++ ) {
		if ( *buf++ != 0x69 ) {
			if ( debug ) {
				printf( "Buffer contents unknown data!\n" );
				mywaitgo();
			}
			break;
		}
	}
}

struct	dtbufs	*getdtbuf()
{
	struct	dtbufs	*p;

	if ( dtbuf_assigned < DTBUFNUM ) {
		/*
		p =(struct dtbufs *p)(&dtbufs[dtbuf_next]);
		*/
		p = (struct dtbufs *)(dtbufs[dtbuf_next].dtbufsp);
		if ( debug )
			printf( "getdtbuf bufadr = %x, dtbuf_next = %x\n", p, dtbuf_next );
		if ( ++dtbuf_next >= DTBUFNUM )
			dtbuf_next = 0;
		dtbuf_assigned++;
		return( p );
	} else return( (struct dtbufs *)(-1) );
}
rdblkd()
{
	struct	dtbufs	*bufadr;

	do {
		chktsent();
		if ( ((bufadr = getdtbuf()) != (struct dtbufs *)(-1)) && !disk_done ) {
			if ( debug )
				printf( "rdblkd bufadr = %x\n", bufadr );
/*
			rddisk( bufadr, xmemto8086seg( MBSDSEG ) );
			lnkbuf( bufadr, xmemto8086seg( MBSDSEG ) );
 */
			rddisk( bufadr, xmemto8086seg( 0xe ) );
			lnkbuf( bufadr, xmemto8086seg( 0xe ) );
			if (debug) {
				printf( "Another disk read set up!\n" );
			}
		}
	} while ( bufadr != (struct dtbufs *)(-1) );
}

frestor()
{
	initpb();
	dtbuf_assigned = 0;
	pb_current = 0;
	pb[pb_current].p_status = 0;

	setrdt();
	do {
		sttape();
		do {
			wrblkd();
		} while ( tape_running );
	} while ( !tape_done );
	rwtape();
	printf( "Restore completed successfully!\n" );
}
			
setrdt()
{
	struct	dtbufs	*bufadr;

	do {
		if ( (( bufadr = getdtbuf()) != (struct dtbufs *)(-1)) && !tape_done )
/*
			lnkbuf( bufadr, xmemto8086seg( MBSDSEG ) );
 */
			lnkbuf( bufadr, xmemto8086seq( 0xe ) );
	} while ( bufadr != (struct dtbufs *)(-1) );
}

wrblkd()
{

	for ( ; !disk_done; ) {
		if ( (pb[pb_current].p_status & COMPLETED) || (pb[pb_current].p_status & ERRMASK) ) {
			if ( pb[pb_current].p_status & ERRMASK ) {
				printf( "Tape error = %x\n",
					((pb[pb_current].p_status & ERRMASK)
					  >> ERRSHIFT) );
				endit();
			}
			wrdisk( pb[pb_current].p_dboff, pb[pb_current].p_dbseg );
			pb[pb_current].p_status &= ~(COMPLETED | ERRMASK | ENTERED);
			if ( pb[pb_current].p_status & EOT ) {
				tape_done++;
				tape_running = 0;
				printf( "EOT encountered on tape!\n" );
				break;
			}
			if ( ccbt.c_gate == GATEOPEN )
				tape_running = 0;

			if ( ( pb_current == pb_lstlnk ) && disk_done ) {
				tape_done++;
			} else {
				lnkbuf( pb[pb_current].p_dboff, pb[pb_current].p_dbseg );
				if ( ++pb_current == DTBUFNUM )
					pb_current = 0;
			}
		} else break;
	}
}

wrdisk( bufadr, bufseg )
unsigned	bufseg;
struct	dtbufs	*bufadr;
{
	int	reltrk;

	doitdsk( DF_WRITE, c_next, h_next, s_next, bufadr, bufseg, dbc_next );

	if ( ( dnext_blk += DTBLKBUF ) < max_blk ) {
		reltrk = dnext_blk / sect_trk;
		s_next = dnext_blk % sect_trk;
		c_next = reltrk / head_cyl;
		h_next = reltrk % head_cyl;
		if ( ( dnext_blk + DTBLKBUF ) > max_blk )
			dbc_next = ( max_blk - dnext_blk ) * 512;
		if ( debug ) {
			printf( "dnext_blk = %ld, max_blk = %ld\n", dnext_blk, max_blk );
		}
	} else {
		disk_done++;
		tape_done++;
	}
}

mywaitgo()
{

	printf( "Type <cr> to go. " );
	gets( wbuf );
	if ( wbuf[0] == 'd' ) {
		debug = 0;
	}
	if ( wbuf[0] == 'b' ) {
		memdis = 1;
	}
	printf( "\n" );
}

dinit()
{

	int	i, reltrk;
	dtbuf_next = 0;
	disk_done = 0;
	sect_trk = dbunit.db_spt;
	head_cyl = dbunit.db_fheads;
	reltrk = dnext_blk / sect_trk;
	s_next = dnext_blk % sect_trk;
	c_next = reltrk / head_cyl;
	h_next = reltrk % head_cyl;
	if ( ( dnext_blk + DTBLKBUF ) > max_blk )
		tbc_next = dbc_next = ( max_blk - dnext_blk ) * 512;
	else tbc_next = dbc_next = ( DTBLKBUF * 512 );

	iop.i_unit = p_unit;
	cib.c_statsem = 0;
	if ( debug ) {
		printf( "Disk initialization complete!\n" );
		mywaitgo();
	}
}

hexchk( hex )
char	*hex;
{

	int	i, j, gotonext, digit;

	j = gotonext = 0;
	hex[j] = 0;
	for ( i = 0; (i < 8) && ( buf[i] != '\0' ); i++ ) {
		if ( (( buf[i] - '0' ) >= 0) && (( buf[i] - '0' ) <= 9) )
			digit = ( buf[i] - '0' );
		else if ( ( buf[i] - 'a' ) <= 6 && ( buf[i] - 'a' ) >= 0 )
			digit = (buf[i] - 'a') + 10;
		else return(0);
		if ( gotonext ) {
			hex[j] |= digit;
			gotonext--;
			j++;
			hex[j] = 0;
		} else {
			hex[j] = (( hex[j] |= digit ) << 4);
			gotonext++;
		}
	}
	if ( i != 8 )
		return(0);
	else return( 1 );
}

numchk( buf, l, h )
char	*buf;
int l, h;
{

	int	num;

	num = atoi( buf );
	if ( ( num >= l ) && ( num <= h ) )
		return( num );
	else return( -1 );
}

long numchkl( buf, l, h )
char	*buf;
long	l, h;
{
	long	num, atol();

	num = atol( buf );
	if ( ( num >= l ) && ( num <= h ) )
		return( num );
	else return ( -1 );
}


setdef(mode)
{

	int	i, j, k, temp;
	char	tchar[32];
	long	maxmax_blk, templ;
	char savebuf[2];

	pt_unit = 0;
	p_unit = 0;
	p_ttype = TT_MSTREAM;
	p_bus16 = BUS16;
	p_dmalok = 0;
	p_cont = 0;
	p_speed = HISPEED;
	p_buslok = 0;
	if ( debug ) {
		do {
			printf( "Defaults? [yn]: " );
			gets( buf );
		} while ((( buf[0] != 'y') && ( buf[0] != 'n' )) 
				|| ( buf[1] != '\0' ));
		savebuf[0] = buf[0];
	} else savebuf[0] = 'y';
	for (;;) {
		printf( "Disk unit? [0-3]: " );
		gets( buf );
		if ( buf[0] == '\0' )
			continue;
		if ((buf[0] >= '0') && (buf[0] <= '3')) {
			char tch[2];
			tch[0] = buf[0];
			tch[1] = 0;
			strcpy(buf,"dk(");
			strcat(buf,tch);
			strcat(buf,",0)");
		}
		for ( i=0; i<50; i++ )
			if ( buf[i] == '(' )
				break;
		if (open(buf,2) == -1) {
			printf( "Invalid device!\n");
			continue;
		}
		temp = numchk( &buf[i+1], 0, 3 );
		if (((deftdisk == 'pd') && !(buf[0]=='i' && buf[1]=='s')) ||
				((buf[0] == 'p') || (buf[1] == 'd'))) {
			imain( atoi(&buf[i+1]), mode );
			exit(0);
		}
		if ( temp == (-1) ) {
			printf( "Invalid unit number!\n" );
			continue;
		} else {
			p_unit = temp;
			break;
		}
	}
	for (;;) {
		printf( "Tape unit? [0-3]: ", pt_unit );
		gets( buf );
		if ( buf[0] == '\0' )
			continue;
		temp = numchk( buf, 0, 3 );
		if ( temp == (-1) ) {
			printf( "Invalid unit number!\n" );
			continue;
		} else {
			pt_unit = temp;
			break;
		}
	}
	buf[0] = savebuf[0];
	if ( buf[0] == 'n' ) {
		do {
			printf( "Tape type - Microstreamer, Cipher S/S, Pertec?" );
			printf( " (m) [mcp]: " );
			gets( buf );
			if ( buf[0] == '\0' )
				break;
		} while ( ( buf[0] != 'm' ) && ( buf[0] != 'c' ) && ( buf[0] != 'p' ) );
		if ( buf[0] == 'm' )
			p_ttype = TT_MSTREAM;
		else if ( buf[0] == 'c' )
			p_ttype = TT_CSS;
		else if ( buf[0] == 'p' )
			p_ttype = TT_PERTEC;
		do {
			printf( "System Bus Width 16? (%s) [yn]: ",
				p_bus16 ? "y" : "n" );
			gets( buf );
			if ( buf[0] == '\0' ) 
				break;
		} while (((buf[0] != 'y') && (buf[0] != 'n' )) || (buf[1] != '\0') );
		if ( buf[0] == 'y' )
			p_bus16 = BUS16;
		else if ( buf[0] == 'n' )
			p_bus16 = 0;
		do {
			printf( "Continuous tape? (%s) [yn]: ",
				p_cont ? "y" : "n" );
			gets( buf );
			if ( buf[0] == '\0' )
				break;
		} while (((buf[0] != 'y') && (buf[0] != 'n')) || (buf[1] != '\0'));
		if ( buf[0] == 'y' )
			p_cont = CONT;
		else if ( buf[0] == 'n' )
			p_cont = 0;
		do {
			printf( "High speed tape? (%s) [yn]: ",
				p_speed ? "y" : "n" );
			gets( buf );
			if ( buf[0] == '\0' )
				break;
		} while (((buf[0] != 'y') && (buf[0] != 'n')) || (buf[1] != '\0'));
		if ( buf[0] == 'y' )
			p_speed = HISPEED;
		else if ( buf[0] == 'n' )
			p_speed = 0;
		do {
			printf( "Bus Lock Option? (%s) [yn]: ",
				p_buslok ? "y" : "n" );
			gets( buf );
			if ( buf[0] == '\0' )
				break;
		} while (((buf[0] != 'y') && (buf[0] != 'n')) || (buf[1] != '\0'));
		if ( buf[0] == 'y' )
			p_buslok = BUSLOK;
		else if ( buf[0] == 'n' )
			p_buslok = 0;
		do {
			printf( "DMA Lock Option? (%s) [yn]: ",
				p_dmalok ? "y" : "n" );
			gets( buf );
			if ( buf[0] == '\0' )
				break;
		} while (((buf[0] != 'y') && (buf[0] != 'n')) || (buf[1] != '\0'));
		if ( buf[0] == 'y' )
			p_dmalok = DMALOK;
		else if ( buf[0] == 'n' )
			p_dmalok = 0;
	}

	dbunit.db_cyls = is_init[p_unit].cylinders;
	dbunit.db_altcyls = is_init[p_unit].altcylinders;
	dbunit.db_spt = is_init[p_unit].sectpertrack;
	dbunit.db_fheads = is_init[p_unit].fixheads;
	maxmax_blk = (((long)(dbunit.db_cyls - dbunit.db_altcyls) *
			dbunit.db_spt) * dbunit.db_fheads);
	for ( ;; ) {
		printf( "Starting disk block number for backup/restore? [0,%ld]: ",
			(maxmax_blk - 1) );
		gets( buf );
		if ( buf[0] == '\0' ) {
			dnext_blk = 0;
			tnext_blk = 0;
			break;
		} else {
			templ = numchkl( buf, (long)0, (maxmax_blk -1) );
			if ( templ == (-1) ) {
				printf( "Invalid starting block number!\n" );
				continue;
			} else {
				tnext_blk = dnext_blk = templ;
				break;
			}
		}
	}
	for ( ;; ) {
		printf( "Number of blocks in backup/restore? [1,%ld]: ",
			(maxmax_blk - dnext_blk) );
		gets( buf );
		if ( buf[0] == '\0' ) {
			max_blk = maxmax_blk;
			break;
		} else {
			templ = numchkl( buf, (long)1, (maxmax_blk - 1) );
			if ( templ == (-1) ) {
				printf( "Invalid number of blocks!\n" );
				continue;
			} else {
				max_blk = dnext_blk + templ;
				break;
			}
		}
	}
	if (debug) {
		printf( "dnext_blk = %ld, max_blk = %ld\n", dnext_blk, max_blk );
		mywaitgo();
	}
}

derror()
{

	debug = 1;
	iop.i_function = DF_STATUS;
	cib.c_statsem = 0;
	iop.i_dboff = (unsigned) buf;
	printf( "Additional status avail., current blocks are: \n" );
	if ( debug ) {
		{	int i,j;
			int *iptr;
			iptr = (int *)&ccb;
			printf( "CCB = " );
			for (i=0; i<8; i++) {
				printf("%x, ",*iptr++);
			}
			printf("\n");
			iptr = (int *)&cib;
			printf( "CIB = " );
			for (i=0; i<8; i++) {
				printf("%x, ",*iptr++);
			}
			printf("\n");
			iptr = (int *)&iop;
			printf( "IOP = " );
			for (i=0; i<15; i++) {
				printf("%x, ",*iptr++);
			}
			printf( "\n" );
		}
	mywaitgo();
	}
	out_multibus( WUA, OP_START );
	for ( tocnt = MAXTOCNT; tocnt && (cib.c_statsem == 0); tocnt-- ) {
	}
	if ( !tocnt ) {
		printf( "Can't read status from controller\n" );
		endit();
	}
	printf( "HES = %o, SES = %o, D/C1 = %o, D/C2 = %o, D/H = %o, D/S = %o\n",
		    buf[1], buf[0], buf[3], buf[2], buf[5], buf[4] );
}

/*
wrtlbl()
{
	int	i;
	long	retclock, time();
	char	cmd, *ctime(), *readtime; 

	rwtape();
	for ( ;; ) {
		retclock = time(0);
		readtime = ctime( &retclock );
		for( i = 0; i < 26; i++ )
			lbl.tdate[i] = *readtime++;
		for ( i = 0; i < 32; i++ )
			lbl.plxmsg[i] = plxmsgd[i];
		for ( i = 0; i < 16; i++ ) 
			lbl.usrmsg[i] = usrmsgd[i];
		printf( "%s", lbl.plxmsg );
		printf( "Backup date/time is %s", lbl.tdate );
		printf( "Enter message for tape header (up to 256 characters).\n" );
		gets( buf );
		for ( i = 0; i < 256 && buf[i] != '\0'; i++ ) {
			lbl.brmsg[i] = buf[i];
		}
		lbl.brmsg[i] = '\0';
		cmd = pb[0].p_cmd;
		pb[0].p_cmd = C_WRITE;
		pb[0].p_status = 0;
		pb[0].p_size = sizeof( lbl );
		pb[0].p_dboff = (unsigned)&lbl;
		pb[0].p_dbseg = xmemto8086seg( MBSDSEG );
		ccbt.c_gate = GATECLOSED;

		if ( debug ) {
			{	int i, *iptr;
				iptr = (int *)&pb[0];
				printf( "pb before label write = " );
				for ( i=0; i<11; i++ ) {
					printf( "%x, ", *iptr++ );
				}
				mywaitgo();
			}
		}

		CHANATTN;
		for ( tocnt=MAXTOCNT; tocnt && (ccbt.c_gate == GATECLOSED); tocnt-- ) { }
		if ( !tocnt || (pb[0].p_status & ERRFLAGS) ) {
			printf( "Can't write tape backup label\n" );
			printf( "Tape status = %x\n", ((pb[0].p_status & ERRFLAGS) >> 8 ) );
			endit();
		}
		pb[0].p_cmd = cmd;
		break;
	}
}
rdlbl()
{
	int	i;
	char	cmd; 

	rwtape();
	for ( ;; ) {
		cmd = pb[0].p_cmd;
		pb[0].p_cmd = C_READ;
		pb[0].p_status = 0;
		pb[0].p_size = sizeof( lbl );
		pb[0].p_dboff = (unsigned)&lbl;
		pb[0].p_dbseg = xmemto8086seg( MBSDSEG );
		ccbt.c_gate = GATECLOSED;

		if ( debug ) {
			{	int i, *iptr;
				iptr = (int *)&pb[0];
				printf( "pb before label write = " );
				for ( i=0; i<11; i++ ) {
					printf( "%x, ", *iptr++ );
				}
				mywaitgo();
			}
		}

		CHANATTN;
		for ( tocnt=MAXTOCNT; tocnt && (ccbt.c_gate == GATECLOSED); tocnt-- ) { }
		if ( !tocnt || (pb[0].p_status & ERRFLAGS) ) {
			printf( "Can't read tape backup label\n" );
			printf( "Tape status = %x\n", ((pb[0].p_status & ERRFLAGS) >> 8 ) );
			endit();
		}
		pb[0].p_cmd = cmd;
		for ( i = 0; i < 32; i++ ) {
			if ( lbl.plxmsg[i] != plxmsgd[i] ) {
				printf( "Tape not written by fbackup!\n" );
				endit();
			}
		}
		printf( "%s%s%s%s", lbl.plxmsg, lbl.tdate, lbl.usrmsg, lbl.brmsg );
		break;
	}
}

 */
fbackup()
{

	int	a;

	initpb();
/*
	wrtlbl();
 */
	pb_current = ( DTBUFNUM - 1 );
	pb[pb_current].p_status = COMPLETED;
	pb_assigned = 1;
	dtbuf_assigned = 1;

	do {
		rdblkd();
		sttape();
		do {
			rdblkd();
		} while ( tape_running );
	} while ( !tape_done );
	rwtape();
	printf( "Backup to tape completed successfully\n" );
}
main()
{
	char	x;
	int	i;

	printf( "$$ fbackup\n" );
	dtbufs[0].dtbufsp = (char *)0x0000;
	dtbufs[1].dtbufsp = (char *)0x4000;
	dtbufs[2].dtbufsp = (char *)0x8000;
	dtbufs[3].dtbufsp = (char *)0xc000;

/*
	out_local( 0x80bf, 0x10 );
	out_local( 0x80ff, 0x10 );
	out_local( 0xff98, 0 );
	out_local( 0xffa0, 0 );
 */
	out_local( 0xff8e, 0xfb );
	for ( i = 0; i < 32; i++ ) {
		out_local( (0x8100 + (i * 2)), (0x240 + i) );
	}
	out_local( 0x7f90, 0x14 );
	out_local( 0x7fb3, 0 );
	debug = 0;
	/*
	do {
		printf( "Debug? [yn]: " );
		gets( buf );
	} while ((( buf[0] != 'y' ) && ( buf[0] != 'n' )) 
			|| ( buf[1] != '\0' ));
	if ( buf[0] == 'y' )
		debug = 1;
	*/
	do {
		printf( "Backup to tape or Restore from tape? [br]: " );
		gets( buf );
		if ( buf[0] == '?' )
			debug = 1;
	} while ((( buf[0] != 'b' ) && ( buf[0] != 'r' )) 
			|| ( buf[1] != '\0' ));
	x = buf[0];
	memdis = 0;
	setdef(x);
	tinit();
	dinit();
	if ( x == 'r' ) {
			restore++;
			frestor();
		} else fbackup();
		exit(0);
}
