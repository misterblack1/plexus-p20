
/* 
 * SID @(#)f3270.h	5.1
 */

#define CTLWR 3
#define DATWR 4
#define DATRD 5
#define RXRD  5
#define RXWR  6
#define TXRD  7
#define TXWR  8
#define VPMIO 9

#define DRDCTRL 0		/* Data Read Control state */
#define DRDLUIB 1		/*  "    "   LUIB struct state */
#define DRDMU   2		/*  "    "   MU     "     "   */


#define	TRSYNC	0x7aa8
#define	TCSYNC	0x7558
#define TRSYNM  0xfff8
#define TCSYNM  0xfff8
#define PNDSTS  0x10
#define PNDAID  0x20
#define PNDSHRT 0x40
#define PDNIO	0x70
#define PNDACK  0x80
#define GOTACK  0x90
#define GOTERR  0xA0
#define POLLED  0xB0
#define XMTERR  0xC0
#define XMTCXL  0xD0
#define XMTDAT  0xE0


#define SSBYTE 0x40
#define STAT_DE 0xC2
        
 

/*
 *
 *   Terminal Request/Response message structure
 *
 */


struct tr {

	int	trsync;		/* flag for synchronization */
	int	seqn;		/* sequence number			*/
	char	op;		/* control opcode */
	char	opx;		/* control sub code */
	char	lu;		/* logical unit # */
	char	b[3];		/* parameter array */

	};

/*
 *
 *   terminal response opcodes
 *
 */

#define	TROPEN	1		/* open control unit channel		*/
#define TRSTAT	2		/* device control 			*/
#define TRRESP	3 		/* ctrl response 	*/
#define TRTEXT	4		/* entering text mode			*/
#define TRCTRL  5		/* resuming ctrl mode			*/
#define TRCLOS	7		/* close control unit channel		*/
#define TRCNCL  8		/* cancel write request */

/*
 *
 *   TRSTAT  subcodes
 *
 *#define	PWROFF	0x00
 *#define	PWRON	0x01
 *#define	DEVBUSY	0x02
 *#define	NOTBUSY 0x03
 *#define	KDETECT 0x04
 *#define	ENTRKEY 0x05
 */

#define	SHRTKEY	0x06		/* short read enter-type key */


/*
 *
 *   TRRESP  subcodes
 *
 */

#define	TRPOS	POS
#define TRCAN	CANONLY
#define TRNEG	NEGRSP
#define TRERR	ERROR
#define TRXCP	EXR

 


/*
 *
 *   Terminal Control message structure
 *
 */

struct tc {

	int	tctype;		/* ctrl message type		*/
				/* 0 = successful return code	*/
				/* 1 = controller command	*/
				/* 2 = error return code	*/
	int	seqn;		/* sequence			*/
	char	op;		/* opcode			*/
	char	opx;		/* opcode modifier		*/
	char	lu;		/* logical unit			*/

	};

/*
 *
 *
 *   Terminal Control message type definitions
 *
 */


#define	TCOPOK	0	/* last operation successful		*/
#define TCOPCMD 1	/* new command				*/
#define TCOPERR 2	/* unsuccussful operation		*/

/* 
 *
 *  Terminal Control opcodes
 *
 */

#define TCACK	0	/* acknowledgment of write */
#define GPOLL	1	/* general poll				*/	
#define	SPOLL	2	/* specific poll			*/
#define SELECT	3	/* select, prepare for write data	*/
#define RCVTXT	4	/* receive host text			*/
			/* opx = { FIRST, MIDDLE, LAST}	*/
#define SELCNV	5	/* conversational reply			*/
#define	CTLSEQ	6	/* process control sequence		*/
#define TXENQ   7	/* are you there?			*/
#define COMMERR 8	/* communication line error		*/
#define TXRTRY  9	/* retry sending of poll to CU		*/

/*
 *
 *  LUOPEN return codes
 *
 */

#define TCBADLU	1
#define TCACTLU 2


/*
 *
 *  VPM Request/Response message structure
 *
 */

struct vr {

	char	vop;		/* opcode			*/
	char	vopx;		/* opcode modifier		*/
	char	vr_b[2];	/* optional bytes		*/

	};

/*
 *
 *  VPM Request/Response opcode defines
 *
 */

#define COMMOK	0		/* communication line active */
#define VGPOLL	1		/* general poll received     */
#define VSPOLL	2		/* specific poll received	*/
#define SELCT	3		/* select			*/
#define RDTXT	4		/* start of recv text block	*/
#define CRTXT	5		/* start of conversational text */
#define LNKCTL	6		/* link control			*/
#define VCOMERR	8		/* communication line error	*/
#define DSTAT	9		/* device  status follows	*/
#define CSTAT	10		/* controller status follows	*/


/*
 *
 *  VPM Device Control message structure 
 *
 */


struct vc {

	char	vop;		/* control opcode */
	char	vc_b[3];	/* parameter array */

	};

#define LBUFMAX 100
#define MAXLNS 42
struct lines {
	char *l_host;		/* IBM3270 host machine id */
	char *l_cardtype;	/* tty,console,ibm "tty" "con" "ibm" */
	char *l_ttyname;	/* file name upto 8 chars long	*/
	char *l_lunum;		/* logical unit IBM3270 device number (0-7) */
	char *l_termtype;	/* terminal type--vt100, vt52--upto 6 chars */
	char l_buf[LBUFMAX];
};

/*
 *
 *  VPM Link Control definitions
 *
 */

#define	DEVCTRL	0 << 5
#	define	LCOFLIN	0
#	define	LCONLIN 1
#	define	LCDVBSY 4
#	define  LCDVRDY 8
#	define  LCKACTV 0x10
#	define  LCPNAID 0x20
#	define  LCPSHRT 0x40
#	define  LCPSTAT 0x80
#	define  LCONFIG 0x81
 
#define REQSTAT	1 << 5

#define TXETB	4 << 5
#define TXETX	5 << 5

#define TXCTRL	6 << 5



/*
 *
 *  VPM Request/Response link control opcode modifiers		
 *
 */

#define	VEOT	EOT
#define VNAK	NAK
#define VENQ	ENQ
