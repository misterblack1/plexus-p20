#define WUA 0xfe00

#define MAXTOCNT 10000

#define	CAB_HEADER	\
	u_char	cab_flag;\
	u_char	cab_rel2;\
	int	cab_rel0;\
	u_char	cab_etype;\
	u_char	cab_ecode;\
	int	cab_estat;\
	u_char	cab_class;\
	u_char	cab_op;

#ifndef ASSEM
typedef	unsigned char	u_char;


typedef	struct
	{
		CAB_HEADER
		u_char	cab_p0;
		u_char	cab_p1;
		u_char	cab_p2;
		u_char	cab_p3;
		u_char	cab_p4;
		u_char	cab_p5;
		u_char	cab_p6;
		u_char	cab_p7;
		u_char	cab_p8;
		u_char	cab_p9;
	}
	cab_t;
#endif

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

#define CAB_MAINT 3
#define CAB_FMT 1
#define CAB_ALTFMT 2
#define CAB_DEF 3
#define CAB_ERASE 4
#define CAB_RETENSION 5

#define S_CABGO 0
#define S_CABERR 1
#define B_CABGO (1<<S_CABGO)
#define B_CABERR (1<<S_CABERR)

#define C_REW 1
#define C_WEOF 2
#define C_ERASE 3
#define C_RETENSION 4
#define C_SAVE 5
#define C_2SAVE 6
#define C_RECALL 7
#define C_2RECALL 8
#define C_IO 9

#ifndef ASSEM
struct	cab_rw			/* read_blocks, write_blocks commands */
{				/* save_blocks, recall_blocks (upper part) */
	CAB_HEADER
	long	cab_nblocks;
	u_char	cab_dk_unsed;
	u_char	cab_disknum;
	long	cab_bufaddr;
};


struct	cab_copy		/* copy_blocks commands */
{
	CAB_HEADER
	long	cab_nblocks;
	u_char	cab_todisk;
	u_char	cab_fromdisk;
	long	cab_toblock;
};


struct	cab_seek		/* seek_block, disk_ready, recal_disk */
{
	CAB_HEADER
	long	cab_blocknum;
	u_char	cab_dk_unused;
	u_char	cab_disknum;
};


struct	cab_move		/* mark_tape, move_tape */
{
	CAB_HEADER
	u_char	cab_t0_unused;
	u_char	cab_segmentnum;
	int	cab_t02_unused;
	int	cab_t04_unused;
};


struct	cab_def			/* format_alt_track, def_disk */
{				/* format_track (upper part) */
	CAB_HEADER
	u_char	cab_sectnum;
	u_char	cab_headnum;
	int	cab_cylnum;
	u_char	cab_dk_unused;
	u_char	cab_disknum;
	u_char	cab_fm_unused;
	u_char	cab_altheadnum;
	int	cab_altcylnum;
};

struct ptcmd {
	int dknum;
	long blkno;
	long blkcnt;
	};
#endif


#define	E_NOERROR	0		/* code for non-error return	*/


#define	E_BUSY		0x0201		/* reserved for controller busy */
#define	E_CMD		0x0301		/* command undefined		*/
#define	E_CANT		0x0401		/* command cannot be done	*/
#define	E_CAB		0x0501		/* bad CAB parameters		*/
#define	E_CRASH		0x0f01		/* firmware bug encountered	*/



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


#define	E_HTIMEOUT	0x1505		/* timeout on Host bus request	*/


#define	RESEEKS		15		/* number of seek attempts - 1	*/
#define	REREADS		15		/* number of read attempts - 1	*/

#define	MAXBUF		1		/* total track buffers - 1	*/
#define	BLOCKSIZE	512		/* bytes per block		*/
#define	S_BLOCKSIZE	9		/*	shift factor for 512	*/
#define MAXSECT		33		/* max sector / track		*/
#define	BUFFERSIZE	(MAXSECT+1)	/* enough blocks for 1 track	*/
#define	NONCYL		0xffff		/* code for invalid cylinder number */
#define	NONSEG		0xffff		/* code for invalide tape segment   */
#define	MAXCLASS	3		/* max command class code	*/
#define	MAXOP		5		/* max command op code		*/
#define	MAXDISK		3		/* device number of last disk	*/
#define	DEVTAPE		4		/* device/DCB number for Tape	*/
#define	DEVHOST		5		/* device/DCB number for Host	*/
#define	DEVBAD		6		/* dummy device for bad command	*/
#define	MAXDEV		6		/* number of devices/DCBs - 1	*/

#define	RAMSEG		3		/* z8001 segment, normal data	*/
#define	BUFSEG		5		/* z8001 segment for buffers	*/
#define	HOSTSEG		9		/* Z8001 segment for host memeory */


#define	TRUE	1
#define	FALSE	0
#define	HI	0xffff
#define LO	0
#define NORMAL	0
#define	NIL	(0xffff)


#define	LSBYTE	0x00ff
#define	MSBYTE	0xff00


#ifndef ASSEM
typedef	unsigned int	flag;
typedef	unsigned char	byte;
typedef	unsigned long	lword;		/* 32-bit variable */
typedef	unsigned int	dn_t;
typedef	unsigned int	cn_t;		/* NONCYL is code for invalide cyl */
typedef	unsigned int	hn_t;
typedef	unsigned int	sn_t;
typedef	unsigned long	bn_t;
typedef	unsigned int	gn_t;			/* tape segment number	*/
typedef	unsigned int	kn_t;			/* tape block number	*/
typedef	unsigned int	na_t;			/* non-segmented address*/
typedef	unsigned char	ad_t;			/* segmented address	*/
typedef	unsigned long	hostaddr_t;		/* host bus address	*/
#endif



#define	pin(p)		in_multibus(p)
#define	min(x,y)	(((x)<(y)) ? (x) : (y))
#define lowbyte(x)	((byte) ((unsigned int) (x) & LSBYTE))
#define highbyte(x)	((byte) ((unsigned int) (x) >> 8  &  LSBYTE))




#ifndef ASSEM
typedef int	(*pfunc_t)();


typedef	struct
	{
		unsigned	id_cylnum;
		byte		id_sectnum;
		byte		id_headnum;
		unsigned	id_altcyl;
		byte		id_altsect;
		byte		id_althead;
		unsigned	id_rest;
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
		struct buffer_header	*next;
		struct buffer_header	*last;
		buf_t	*bh_buffer;		/* buffer address	    */
						/*	NIL for end of list */
		int	bh_bseg;		/* RAMSEG or BUFSEG	  */
		cn_t	bh_cylnum;		/* cyl number of contents */
		hn_t	bh_headnum;		/* head number of contents */
		flag	bh_inuse;		/* disable re-allocation */
		buf_t	*bh_bufp;		/* current sect in buffer */
		sn_t	bh_nsects;		/* number sects unprocessed */
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

/* not used */	bn_t	blocknum;		/* block number operand	*/
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

		struct dcb_struct *condcb;	/* ptr to consumer dev	*/
		bh_t	bhcache;		/* cache buffers chain	*/
		bn_t	want_blocknum;		/* untransfered portion	*/
		bn_t	want_nblocks;		/* and count		*/

		struct dcb_struct *prodcb;	/* ptr to producer dev	*/
		bn_t	need_blocknum;		/* param for write	*/
		bn_t	need_nblocks;		/* and count		*/
	}
	dcb_t;
#endif


/*	IMSC BOARD I/O PORT DEFINITIONS		*/



/*	GENERAL I/O PORTS SECTION	*/


#define P_MEMADDR	0x00	/* SQR Memory Address Counter (DKCNT) */
#define S_MEMADDR	1
#define B__MEMADDR	(0x7fff >> S_MEMADDR)	/* RAM address bits <1:15> */
#define B_MEMSEG	0x8000			/* RAM segment select */

#define P_MOFFSET	0x02	/* Host Memory Offset Register, addr <16:19> */
#define B_MOFFSET	0x000f

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
#define B_D_BUS		0x03ff	/*		Disk Data Bus <0:9>	*/
#define B_D_RESERVED	0x0400
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

#define B_S_WRITEPROT	0x0000	/*	write protected */

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
