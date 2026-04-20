/*  */
/* @(#)imsc.h	5.1 4/22/86 */

#define	E_NOERROR	0		/* code for non-error return	*/

#define	E_BUSY		0x0201		/* reserved for controller busy */
#define	E_CMD		0x0301		/* command undefined		*/
#define	E_CANT		0x0401		/* command cannot be done	*/
#define	E_CAB		0x0501		/* bad CAB parameters		*/
#define	E_CRASH		0x0f01		/* firmware bug encountered	*/
#define E_INTERRUPT	0x0601		/* internal cmd interrupts	*/
#define E_PARITY	0x0701		/* parity error occurred	*/
#define E_CHKSUM	0x0801		/* PROM checksum error		*/

#define	E_DPROT		0x1103		/* disk protected from writing	*/
#define	E_DREADY	0x1203		/* disk not ready		*/
#define	E_DFAULT	0x1303		/* disk drive fault indicated	*/
#define	E_DSELECT	0x1403		/* disk failed to select	*/
#define	E_DTIMEOUT	0x1503		/* disk operation time out error*/
#define	E_DFORMAT	0x1603		/* disk failed in formatting	*/
#define	E_DSEEK		0x1703		/* disk seek error */
#define	E_DECCA		0x1803		/* disk ECC error in id field	*/
#define	E_DECCD		0x1903		/* disk ECC error in data field	*/
#define	E_DECCW		0x1a03		/* reserved			*/
#define	E_DDEF		0x1b03		/* disk limits not defined	*/
#define E_DNOTRK	0x1c03		/* disk unable to locate track	*/

#define	E_TEOS		0x1004		/* end of segment reached	*/
#define	E_TEXC		0x1304		/* non end-of-segment EXCeption */
#define	E_TTIMEOUT	0x1504		/* tape time out error		*/
#define E_TRECALL	0x1604		/* err during recall		*/
#define E_TSAVE		0x1704		/* err during save		*/
#define E_TSTAT		0x1804		/* err during stat tape		*/
#define E_TCMD		0x1904		/* not rstat cmd with exception */
#define E_TPGONE	0x2004		/* no tape drive present	*/
#define E_WRECALL	0x2104		/* timeout during wait recall	*/
#define E_WSAVE		0x2204		/* timeout during wait save	*/
#define E_STAT1		0x2304		/* timeout during stat tape(top)*/
#define E_STAT2		0x2404		/* timeout during stat tape(bot)*/
#define E_CMD1		0x2504		/* timeout during cmd tape(top) */
#define E_CMD2		0x2604		/* timeout during cmd tape(bot)	*/
#define E_RDYTP		0x2704		/* timeout during ready tape	*/
#define E_FOB		0x2804		/* tape drive inconsistent at
						start of tape command	*/

#define	E_HTIMEOUT	0x1505		/* timeout on Host bus request	*/


#define	RESEEKS		15		/* number of seek attempts - 1	*/
#define	REREADS		4		/* number of read attempts - 1	*/

#define DINFOPTR	0		/* location in RAMSEG that
					   contains address of dcb_list
					   so downloaded code can find
					   disk info 	*/
#define MAXBUF		64		/* total 1024 byte buffers	*/
#define	BLOCKSIZE	512		/* bytes per block		*/
#define	S_BLOCKSIZE	9		/*	shift factor for 512	*/
#define MAXSECT		33		/* max sector / track		*/
#define	NONCYL		0xffff		/* code for invalid cylinder number */
#define	NONSEG		0xffff		/* code for invalide tape segment   */
#define	MAXCLASS	6		/* max command class code	*/
#define	MAXOP		6		/* max command op code		*/
#define	MAXDISK		3		/* device number of last disk	*/
#define	DEVTAPE		4		/* device/DCB number for Tape	*/
#define	DEVHOST		5		/* device/DCB number for Host	*/
#define	DEV2HOST	6		/* addition host dcb		*/
#define	DEVBAD		7		/* dummy device for bad command	*/
#define	MAXDEV		7		/* number of devices/DCBs - 1	*/

#define	RAMSEG		3		/* z8001 segment, normal data	*/
#define	BUFSEG		5		/* z8001 segment for buffers	*/
#define	HOSTSEG		9		/* Z8001 segment for host memeory */
#define RPROMSEG	17		/* seg to read out prom */


#define	TRUE	1
#define	FALSE	0
#define	HI	0xffff
#define LO	0
#define NORMAL	0
#define	NIL	(0xffff)
#define NULL	0

#define	LSBYTE	0x00ff
#define	MSBYTE	0xff00


#ifndef ASSEM
typedef	unsigned short	flag;
typedef	unsigned char	byte;
typedef	unsigned long	lword;		/* 32-bit variable */
typedef	unsigned short	dn_t;
typedef	unsigned short	cn_t;		/* NONCYL is code for invalide cyl */
typedef	unsigned short	hn_t;
typedef	unsigned short	sn_t;
typedef	long		bn_t;
typedef	unsigned short	gn_t;			/* tape segment number	*/
typedef	unsigned short	kn_t;			/* tape block number	*/
typedef	unsigned short	na_t;			/* non-segmented address*/
typedef	unsigned char	ad_t;			/* segmented address	*/
typedef	unsigned long	hostaddr_t;		/* host bus address	*/
#endif ASSEM


#define	pin(p)		in_multibus(p)
#define	min(x,y)	(((x)<(y)) ? (x) : (y))
#define	max(x,y)	(((x)>(y)) ? (x) : (y))
#define lowbyte(x)	((byte) ((unsigned short) (x) & LSBYTE))
#define highbyte(x)	((byte) ((unsigned short) (x) >> 8  &  LSBYTE))



#ifndef ASSEM
typedef short	(*pfunc_t)();


typedef	struct
	{
		unsigned short	id_cylnum;
		byte		id_sectnum;
		byte		id_headnum;
		unsigned short	id_altcyl;
		byte		id_altsect;
		byte		id_althead;
		unsigned short	id_rest;
	}
	id_t;


typedef union
	{
		byte	block_buffer[BLOCKSIZE];
		id_t	id_buffer[MAXSECT];
	}
	buf_t;


typedef	struct buffer_header
	{
		flag	bh_status;		/* status of associated block */
		struct dcb_struct *bh_dcb;	/* pointer to associated dcb */
		bn_t	bh_blknum;		/* block number this device */
		bn_t	bh_nsects;		/* num valid sects this group */
		bn_t	bh_reqcnt;		/* total sects this group */
		buf_t	*bh_bufptr;		/* buffer address	    */
	}
	bh_t;


typedef struct dcb_struct
	{
		dn_t	devnum;			/* device number of dcb	*/
		cn_t	cyl_limit;		/* cylinders per drive 	*/
		hn_t	head_limit;		/* heads per cylinder	*/
		sn_t	sect_limit;		/* sectors per track	*/
		cn_t	serv_cylnum;		/* service cylinder num	*/
		flag	active;			/* completion pending	*/
		flag	eos;			/* tape end of segment	*/
		byte	estat[6];		/* error status bytes	*/

/* not used 	bn_t	blocknum;		/* block number operand	*/
		short	lock;			/* for locking dcb	*/
		short	intflag;		/* interrupt flag	*/
		bn_t	nblocks;		/* block count operand	*/
		cn_t	cylnum;			/* physical cylinder	*/
		hn_t	headnum;		/* physical head	*/
		sn_t	sectnum;		/* physical sector	*/
		buf_t	*maddr;			/* buffer addr operand	*/
		int	bseg;			/* RAMSEG or BUFSEG     */

		unsigned	at_cylnum;	/* disk cylinder arg	*/
		unsigned	at_headnum;	/* disk head argument	*/
		unsigned	at_sectnum;	/* disk sector argument	*/
		unsigned	at_nsects;	/* disk count argument	*/
		unsigned	at_segnum;	/* tape seg argument	*/
		unsigned	at_blocknum;	/* tape block argument	*/
		unsigned	at_nblocks;	/* tape count argument	*/

		gn_t	cur_segnum;		/* real tape position	*/
		kn_t	cur_blocknum;		/* real tape position	*/
		flag	in_recall;		/* tape in read command	*/

		bn_t	want_blocknum;		/* untransfered portion	*/
		bn_t	want_nblocks;		/* and count		*/

		bn_t	need_blocknum;		/* param for write	*/
		bn_t	need_nblocks;		/* and count		*/

		unsigned cur_cyl;		/* current cylinder num */
		unsigned cur_hd;		/* current head number  */
	}
	dcb_t;
#endif ASSEM


#define WUA 0xfe00

#define MAXTOCNT 50000

#define LOCKED 1
#define WANTED 2

#define	CAB_HEADER	\
	unsigned char	cab_flag;\
	unsigned char	cab_rel2;\
	short	cab_rel0;\
	unsigned char	cab_etype;\
	unsigned char	cab_ecode;\
	short	cab_estat;\
	unsigned char	cab_class;\
	unsigned char	cab_op;

#ifndef ASSEM


typedef	struct
	{
		CAB_HEADER
		unsigned char	cab_p0;
		unsigned char	cab_p1;
		unsigned char	cab_p2;
		unsigned char	cab_p3;
		unsigned char	cab_p4;
		unsigned char	cab_p5;
		unsigned char	cab_p6;
		unsigned char	cab_p7;
		unsigned char	cab_p8;
		unsigned char	cab_p9;
		long	cab_spares;
	}
	cab_t;
#endif ASSEM

#define CAB_IO 0
#define CAB_READ 1
#define CAB_WRITE 2
#define CAB_RDTP 3
#define CAB_WRTP 4

#define CAB_MV 1
#define CAB_COPY 1
#define CAB_SAVE 2
#define CAB_RECALL 3
#define CAB_MARK 4

#define CAB_UTIL 2
#define CAB_SEEK 1
#define CAB_MOVE 2
#define CAB_DKRDY 3
#define CAB_DKRECAL 4
#define CAB_TPTEST 5
#define CAB_STAT 6

#define CAB_MAINT 3
#define CAB_FMT 1
#define CAB_ALTFMT 2
#define CAB_DEF 3
#define CAB_ERASE 4
#define CAB_RETENSION 5

#define CAB_COMM 4
#define CAB_MRD 1
#define CAB_MWR 2
#define CAB_JMP 3

#define CAB_CACHE 5
#define CAB_CSET 1
#define CAB_CBRD 2
#define CAB_CBWR 3
#define CAB_CRRD 4
#define CAB_CRWR 5

#define S_CABGO 0
#define S_CABERR 1
#define S_CABLXFER 2
#define S_CREAD 3
#define B_CABGO (1<<S_CABGO)
#define B_CABERR (1<<S_CABERR)
#define B_CABLXFER (1<<S_CABLXFER)
#define B_CREAD (1<<S_CREAD)

#define C_IREW 1
#define C_IWEOF 2
#define C_IERASE 3
#define C_IRETENSION 4
#define C_ISAVE 5
#define C_I2SAVE 6
#define C_IRECALL 7
#define C_I2RECALL 8
#define C_IIO 9
#define C_IMOVE 10
#define C_ISTATUS 11
#define C_IRNOP 12	/* this is a robin command only and must not be
			   used in any other cartridge driver.  This is 
			   used in the tape program to differentiate
			   between robin and other cartridge tapes.  This
			   define MUST be the same as in tp.h */

#define IM_JMPADDR	2
#define IM_RESET	3
#define IM_SETCACHE	4

#ifndef ASSEM
struct	cab_rw			/* read_blocks, write_blocks commands */
{				/* save_blocks, recall_blocks */
	CAB_HEADER
	long	cab_nblocks;
	unsigned char	cab_dk_unsed;
	unsigned char	cab_disknum;
	long	cab_bufaddr; 
	long	cab_blkno;
};


struct	cab_copy		/* copy_blocks commands */
{
	CAB_HEADER
	long	cab_nblocks;
	unsigned char	cab_todisk;
	unsigned char	cab_fromdisk;
	long	cab_toblock;
};


struct	cab_seek		/* seek_block, disk_ready, recal_disk */
{
	CAB_HEADER
	long	cab_blocknum;
	unsigned char	cab_dk_unused;
	unsigned char	cab_disknum;
};


struct	cab_move		/* mark_tape, move_tape */
{
	CAB_HEADER
	unsigned char	cab_t0_unused;
	unsigned char	cab_segmentnum;
	short	cab_t02_unused;
	short	cab_t04_unused;
};


struct cab_mem
{
	CAB_HEADER
	long	cab_hostaddr;
	long	cab_imscaddr;
	short	cab_bcount;
};

struct cab_cache
{
	CAB_HEADER
	short	cab_csize;
	short	cab_numbuf;
	short	cab_maxreq;
	short	cab_strsize;
	long	cab_cspare;
};

struct cab_cbstat
{
	CAB_HEADER
	long	cab_breads;
	long	cab_bwrites;
	long	cab_chits;
};
	
struct cab_crstat
{
	CAB_HEADER
	long	cab_rreads;
	long	cab_rwrites;
	long	cab_totblks;
};
	
struct	cab_def			/* format_alt_track, def_disk */
{				/* format_track (upper part) */
	CAB_HEADER
	unsigned char	cab_sectnum;
	unsigned char	cab_headnum;
	short	cab_cylnum;
	unsigned char	cab_dk_unused;
	unsigned char	cab_disknum;
	unsigned char	cab_fm_unused;
	unsigned char	cab_altheadnum;
	short	cab_altcylnum;
};

struct cab_stat			/* read 6 bytes of status from cart */
{
	CAB_HEADER
	byte	cab_ptstat[6];
};

struct ptcmd {
	short dknum;
	long blkno;
	long blkcnt;
	};

#endif ASSEM
#


/*	IMSC BOARD I/O PORT DEFINITIONS		*/



/*	GENERAL I/O PORTS SECTION	*/


#define P_MEMADDR	0x00	/* SQR Memory Address Counter (DKCNT) */
#define S_MEMADDR	1
#define B__MEMADDR	(0x7fff >> S_MEMADDR)	/* RAM address bits <1:15> */
#define B_MEMSEG	0x8000			/* RAM segment select */

#define P_MOFFSET	0x02	/* Host Memory Offset Register, addr <16:19> */
#define B_MOFFSET	0x001f
#define B_BHEN		0x10	/* must always set BHEN for MB word access */
#define S_TAPESEG	6
#define B__TAPESEG	(3<<S_TAPESEG)
#define B_T3SEG		(1<<S_TAPESEG)
#define B_T5SEG		(2<<S_TAPESEG)

#define P_LEDINT	0x04	/* LED, interrupt, parity control */
#define B_L_LED		0x007f	/*	LED output bits <0:6> */
#define B_L_ENPAR	0x0080	/*	enable memory parity detect */
#define	B_L_INT		0x0100	/*	generate interrupt to Host */
#define B_L_0CLPAR	0X0200	/*	clear parity error */

#define P_SYNBYT	0x06	/* SQR Sync-byte Register */
#define B_SYNBYT	0x00ff	/*	0x09 code assumed by hardware */

#define P_UTIL		0x10	/* general Utility Register */
#define S_U_BEPC		2
#define	B_U_SERSEL	0x0001	/*	MP/SQR0 SERDES bus select */
#define B_U_ENMHR	0x0002	/*	enable Memory Holding Reg */
#define B_U__BEPC		(0x0007 << S_U_BEPC)	/* BEP bits <C0:C2> */
#define B_U_DKRW		0x0020	/*	Disk Read/Write0 bit */
#define B_U_CLK		0x00c0	/*	Disk-SQR clock select <0:1> */
#define B_U_SELTAG	0x0100	/*	Disk Unit Select Tag */
#define B_U_TAG1		0x0200	/*	Disk Tag1 */
#define B_U_TAG2		0x0400	/*	Disk Tag2 */
#define B_U_TAG3		0x0800	/*	Disk Tag3 */
#define	B_U_TPRD		0x1000	/*	Tape read transfer mode (DMA) */
#define B_U_TPWR		0x2000	/*	Tape write transfer mode (DMA)*/
#define B_U_0TPONL	0x4000	/*	Tape ONLine signal */
#define B_U_DPMUX	0x8000	/*	SQR/MP0 Data Path MUX control */

#define P_SQSSCLK	0x20	/* SQR single step clock */

#define P_SQRST		0x22	/* SQR reset */

#define P_SQCMD		0x24	/* SQR command port */
#define B_C_CMD		0x0007	/*	command code */
#define B_C_SECTSEL	0x0008	/*	1-K byte sector select */
#define B_C_RESERVED	0x0010
#define B_C_ACKCLR	0x0020	/*	clear command acknowledge flag */

#define P_IDCNT		0x26	/* SQR sector ID and iteration counters */
#define	S_I_ID		8
#define B_I_CNT		0x00ff	/*		iteration count */
#define B_I__ID		(0x00ff<<S_I_ID)     /* initial sector ID counter */

#define P_DKSEL		0x30	/* Disk Data and Unit Selection latch	*/
#define S_D_USEL	11
#define B_D_BUS		0x07ff	/*		Disk Data Bus <0:10>	*/
#define B_D__USEL	(0x000f	<< S_D_USEL)  /* Unit Number Data	*/
#define B_D_0TPRST	0x8000	/* Tape RESET Line			*/

#define P_BEPCLK	0x40	/* mP to BEP clock */

#define P_BEP		0x50	/* BEP read port */
#define S_B_LP		4 
#define S_B_OUT		8 
#define B_B_ER		0x0001	/*		Error bit */ 
#define B_B_EP		0x0002	/*		EP bit */
#define B_B_AE		0x0004	/*		AE bit */
#define B_B_UNUSED	0x0008
#define B_B__LP		(0x000f << S_B_LP) /* 	LP<0:3> */
#define B_B__OUT	(0x00ff << S_B_OUT) /* OUT<0:3> */

#define P_TPOUTPUT	0x60	/* Tape command data output port	*/
#define B_TPOUTPUT	0x00ff	/*		data bits <0:7>		*/

#define P_TPRSTREQ	0x70	/* reset Tape-REQquest */

#define P_RETI		0x80	/* generate Z80 Return-from-Interrupt signal */
#define B_RETI		0x00ff	/* 	store with bytes 0xed and 0x4d */

#define P_BOARD		0x90	/* input Board Address Setting */
#define B_B_SW		0x00ff	/* 	DIP switches <0:7> */
#define B_B_JP		0xff00	/* 	jumpers */

#define P_SWSQCNT	0xa0	/* Diagnostic Switches and SQR Counter */
#define S_W_SQCNT	8
#define B_W_SW		0x00ff	/* 		Switches <0:7> */
#define B_W_0PYES	0x1	/* enable parity */
#define B_W__SQCNT	(0x003f << S_W_SQCNT) /*SQR iteration counter */
#define B_W_0PARERR	0x4000	/* 		parity error status */
				/* 		implies not power-fail */
#define B_W_0TPEXC	0x8000	/* 		Tape EXCeption signal */

#define P_STAT		0xa2	/* Disk Drive & SQR status */
#define B_S_SEL1	0x0001	/*	Drive 1 selected */
#define B_S_SEL2	0x0002	/*	Drive 2 selected */
#define B_S_SEL3	0x0004	/*	Drive 3 selected */
#define B_S_SEL4	0x0008	/*	Drive 4 selected */
#define B_S_SELD	0x000f	/*	Drive selected field */
#define B_S_0TPRDY	0x0010	/*	Tape READY Bit			*/
#define B_S_SQACK	0x0020	/*	SQR command acknowledge */
#define B_S_SQDONE	0x0040	/*	SQR command done */
#define B_S_0SQDFLD	0x0080	/*	SQR in-data-field flag */
#define B_S_SEEKERR	0x0100	/*	seek error */

#define B_S_WRITEPROT	0x0200	/*	write protected */

#define B_S_SECTOR	0x0400	/*	sector pulse */
#define B_S_INDEX	0x0800	/*	index pulse */
#define B_S_BUSY	0x1000	/*	busy */
#define B_S_FAULT	0x2000	/*	fault */
#define B_S_READY	0x4000	/*	unit ready */
#define B_S_CYLINDER	0x8000	/*	on cylinder */

#define P_TPINPUT	0xa6	/* Tape status data input port		*/
#define B_TPINPUT	0x00ff

#define P_MULTIBUS	0xb0	/* read Multibus data<0:7> for interrupt stat */
#define B_M_RST		0x0001	/*	Host resets controller */
#define B_M_CMD		0x0002	/*	Host issues controller command */
#define B_M_INTCLR	0x0004	/*	Host clears interrupt by controller */


/*	LSI CHIP PORTS		*/


#define	B_CTC		0x00ff
#define	P_CTC10		0xc0	/* CTC 1 channel 0 */
#define P_CTC11		0xc1	/* CTC 1 channel 1 */
#define P_CTC12		0xc2	/* CTC 1 channel 2 */
#define P_CTC13		0xc3	/* CTC 1 channel 3 */

#define P_CTC00		0xd0	/* CTC 0 channel 0 */
#define P_CTC01		0xd1	/* CTC 0 channel 1 */
#define P_CTC02		0xd2	/* CTC 0 channel 2 */
#define P_CTC03		0xd3	/* CTC 0 channel 3 */

#define P_DMA		0xe0	/* Tape DMA chip port		*/
#define B_DMA		0x00ff	/*	an 8-bit device		*/

#define P_SIOAD		0xf0	/* SIO channel A data port */
#define P_SIOBD		0xf1	/* SIO channel B data port */
#define P_SIOAC		0xf2	/* SIO channel A control port */
#define P_SIOBC		0xf3	/* SIO channel B control port */



/*	use the following include files for LSI chip bit-definitions:
 *
 *		/usr/sys/dev/sioc/h/ctc.h
 *		/usr/sys/dev/sioc/h/sio.h
*/

/*
 * Defines for the z80 ctc.
 */

/* channel control word */

#define	CINTEN		(1<<7)		/* interrupt enable */
#define	CCTR		(1<<6)		/* counter mode */
#define	CCLK256X	(1<<5)		/* clk times 256 */
#define	CUPCLK		(1<<4)		/* clk on rising edge */
#define	TRGSTART	(1<<3)		/* clk/trg pulse starts timer */
#define	CTIME		(1<<2)		/* time const to follow */
#define	CRESET		(1<<1)		/* reset timer */
#define	CCTRL		(1<<0)		/* control word */
#define	VECCTC0		0		/* interrupt vector number, CTC 0 */
#define	VECCTC1		8		/* interrupt vector number, CTC 1 */

#define	VEC_BUS		(VECCTC0+4)	/* vector number for bus grant	*/

#define VEC_CMD		(VECCTC1+6)	/* vector number for cmd activation */

#define VEC_DMA		16		/* vector number for dma chip */


#define IMSTKSIZE 256
#define IMPROCS 5
#define IMSQSIZE 16

#ifndef ASSEM
/*
 * One structure allocated per active
 * process. It contains all data needed
 * about the process.
 */
struct	proc {
	char	p_stat;
	char	p_pri;		/* priority, negative is high */
	long	p_tocnt;	/* sleep timeout count */
	caddr_t	p_wchan;	/* event process is awaiting */
	struct proc *p_link;	/* linked list of running processes */
	dcb_t	*p_dcb;		/* save global dcb ptr */
	dcb_t	*p_dcbfrom;
	dcb_t	*p_dcbto;
	label_t	p_rsav;		/* save area for restart info */
	short	p_stk[IMSTKSIZE];	/* stack for process */
};

extern struct proc proc[];	/* the proc table itself */
extern struct proc *curproc;	/* pointer to the executing process */
#endif ASSEM

/* stat codes */
#define	SSLEEP	1		/* awaiting an event */
#define	SWAIT	2		/* (abandoned state) */
#define	SRUN	3		/* running */
#define	SIDL	4		/* intermediate state in process creation */
#define	SZOMB	5		/* intermediate state in process termination */
#define	SSTOP	6		/* process being traced */

