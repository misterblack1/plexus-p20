/* SID @(#)rm.h	5.1 */

/*
 * This source contains defines and structures used by the CPC Tapemaster
 * tape controller.
 */

/* ioctl requests */

#define	RMPOSN		(('r'<<8)|0)

/*
 * structure used in ioctl calls to rm tape driver
 */

struct rmcmd_struct {
	unsigned rm_cmd;    /* command to controller. May be ored with C_REV */
	unsigned rm_cnt;    /* pass records value. returns count field */
	unsigned rm_status; /* returns the status field */
};

/*
	The following values are allowed in the r_cmd field of the RMPOSN
	ioctl call to the rrm driver. If necessary, a more complete description
	is avaialble in the manual for the controller. 
*/


#define	C_OLREW		0x04	/* overlapped rewind. Initiates a rewind command
				Periodic C_STATUS commands may be issued to 
				check for load point. */

#define	C_RFOREIGN	0x1c	/* read foreign tape. Counts bytes in the next
				record. Data is ignored. The lower 16 bits of 
				the count are retruned in rm_count. The upper
				16 bits of the count are lost. Records over
				64K can't be read anyway. */

#define	C_NOP		0x20	/* no operation. You do not want to do this. */

#define	C_STATUS	0x28	/* status. Just returns the status of the drive
				in rm_status. All commands except C_NOP do this
				as well as their other function. */

#define	C_REW		0x34	/* rewind. rewinds tape to load point. Does not
				return until rewind is complete. */

#define	C_UNLOAD	0x38	/* offline/unload. rewinds and unloads the tape.
				Drive goes offline. returns when complete */

#define	C_WEOF		0x40	/* writes one file mark on the tape */

#define	C_SRCHEOF	0x44	/* search filemark. searches forward or reverse
				until a filemark is found. If an EOT (forward)
				or Load Point (reverse) is encountered, it will
				terminate early. To get reverse operation,
				use (C_SRCHEOF|C_REV). */

#define	C_SPACE		0x48	/* space. Spaces, forward or reverse, a 
				specified number of records. A filemark is 
				counted as a record. The desired number of
				records is passed in rm_count. Note that 
				rm_count is destroyed on return. To get reverse
				operation use (C_SPACE|C_REV). */

#define	C_ERASE		0x4c	/* erase fixed length. erases approximately
				3.5 * rm_count inches of tape from the current
				position. */

#define	C_ERASEALL	0x50	/* erase tape. erases the entire tape from
				current position to several feet beyond the
				EOT. */

#define	C_SPACEEOF	0x70	/* space filemark. similar to C_SPACE except
				that it terminates early if a filemark is
				encountered before all the records are spaced
				over. */

#define	C_SRCHMEOF	0x94	/* search multiple filemarks. Similar to 
				C_SRCHEOF except that it proceeds until a 
				specified number of consecutively written
				filemarks are located. This command is very
				useful when using double or triple filemarks
				as file separators. The number of filemarks,
				up to 255, is specified in rm_count. */

#define C_OPTION	0x78

#define	C_REV		0x4000	/* This is not a command, but is a flag that
				can be ored with the search and space commands
				to cause them to go in reverse. It sets bit
				10 in the control field (Reverse). */

#define	C_SET		0xff	/* sets (or resets) some of the bits in the
				command field for this tape drive. This allows
				read-reverses, busslocks, and high-density 
				select. Note that these settings stay in effect
				until the drive is closed.  JF */

/*
	the following commands are used by the driver but are not allowed in the
	ioctl call.
*/
#define C_CONFIG	0x00
#define C_READ		0x2c
#define C_WRITE		0x30
#define C_CLRINTR	0x9c


/* 
	The following defines describe the fields in rm_status after the
	ioctl call returns.
*/

#define	ENTERED		0x8000	/* always set */
#define	COMPLETED	0x4000	/* set if command completed successfully */
#define	RETRY		0x2000	/* controller did at least one retry */
#define	ERRMASK		0x1f00	/* mask to extract error number (see below) */
#define	ERRSHIFT	8	/* shift count to right justify error number */
#define	FILEMARK	0x0080	/* a filemark was detected on this operation */
#define	ONLINE		0x0040	/* the drive is on line */
#define	LOADPOINT	0x0020	/* the tape is at load point */
#define	EOT		0x0010	/* the EOT marker was detected */
#define	READY		0x0008	/* The selected drive is ready */
#define	FORMATBSY	0x0004	/* the formatter is busy */
#define	WRITEPROTECT	0x0002	/* the tape does not have a write enable ring */

/* modified records/overrun field for SELECT OPTION command (returned in rm_count)*/
/* status bits returned */
#define OPT_S	0x0200
#define OPT_ABS	0x0100

/* option select field */
#define OPT_BS_E	0x0004
#define	OPT_BS_D	0x0008
#define OPT_A_A		0x0001
#define OPT_A_O		0x0002


/* Tapemaster Errors. returned in ((rm_status>>ERRSHIFT)&ERRMASK) */

#define RM_NOERR	0	/* no unrecoverable error */
#define RM_TOUT1	1	/* timeout waiting for expected data 
					busy false */
#define RM_TOUT2	2	/* timeout waiting for expected data
					busy false, formatter busy,
					and ready true */
#define RM_TOUT3	3	/* timeout waiting for expected ready
					false */
#define RM_TOUT4	4	/* timeout waiting for expected ready
					true */
#define RM_TOUT5	5	/* timeout waiting for expected data
					busy true */
#define RM_TOUT6	6	/* memory time occurred */
#define RM_BLANK	7	/* blank tape encountered */
#define RM_MERR		8	/* error in micro diagnostic */
#define RM_EOT		9	/* unexpected EOT during forward operation,
					or load point during reverse 
					operation */
#define RM_ERROR	0xa	/* hard or soft error occurred which could
					not be corrected by retry */
#define RM_OFLO		0xb	/* read overflow or write underflow */
#define RM_PARITY	0xd	/* read parity error on byte interface 
					between drive and Tapemaster */
#define RM_PCKSUM	0xe	/* prom checksum error */
#define RM_TTIM		0xf	/* tape timeout - usually due to attempt
					to read a smaller record than was
					written */
#define RM_NORDY	0x10	/* tape not ready */
#define RM_NORING	0x11	/* write attempted without write enable ring*/
#define RM_DIAG		0x13	/* need diagnostic jumper for diagnostic mode*/
#define RM_LINK		0x14	/* attempt to link on non linkable command */
#define RM_FM		0x15	/* unexpected filemark during tape read */
#define RM_PARAM	0x16	/* bad parameter to controller - usually byte
					count field entry too large or zero */
#define RM_FONEHOME	0x18	/* unidentifiable hardware error -
					call factory */
#define RM_ABORT	0x19	/* streaming r/w terminated by system or disk*/




/*
	The remainder of this include file is of interest to the driver, and
	is not of much use for the ioctl call to the driver.
*/

/* values for xcb fields */

#define	CCWNORM		0x11
#define	CCWCLR		0x09
#ifdef z8000
#define	GATECLOSED	0xff
#define	GATEOPEN	0x00
#endif
#ifdef m68
#define	RM_GATECLOSED	0xff
#define	RM_GATEOPEN	0x00
#endif

/* control fields */

#define	BUS16	0x8000
#define	CONT	0x1000
#define	HISPEED	0x0800
#define	REVERSE	0x0400
#define SKPEOT	0x0200
#define	BANK1	0x0100
#define	BUSLOK	0x0080
#define	LINK	0x0040
#define	INTR	0x0020
#define	MAILBOX	0x0010
#ifdef z8000
#define	UNIT0	0x0000
#define	UNIT1	0x0004
#define	UNIT2	0x0008
#define	UNIT3	0x000c
#endif
#ifdef m68
#define	RM_UNIT0	0x0000
#define	RM_UNIT1	0x0008
#define	RM_UNIT2	0x0004
#define	RM_UNIT3	0x000c
#endif
/* error codes */

#define NOERR		(0x00 << ERRSHIFT)
#define HARDERR		(0x0a << ERRSHIFT)
#define	OVERRUN		(0x0b << ERRSHIFT)
#define TIMEOUT		(0x0f << ERRSHIFT)
#define EOFDETECT	(0x15 << ERRSHIFT)

/* 
 * structures used by the controller
 */

struct	scp_struct {
	unsigned char	s_scpr1;
	unsigned char	s_sysbus;
	unsigned short	s_scboff;
	unsigned short	s_scbseg;
};

struct	scb_struct {
	unsigned char	s_scbr1;
	unsigned char	s_soc;
	unsigned short	s_xcboff;
	unsigned short	s_xcbseg;
};

struct	xcb_struct {
	unsigned char	c_gate;
	unsigned char	c_ccw;
	unsigned short	c_pboff;
	unsigned short	c_pbseg;
	unsigned short	c_xcbr1;
};

struct	pb_struct {
	unsigned char	p_pbr1;
	unsigned char	p_cmd;
	unsigned short	p_pbr2;
	unsigned short	p_control;
	unsigned short	p_count;
	unsigned short	p_size;
	unsigned short	p_records;
	unsigned short	p_dboff;
	unsigned short	p_dbseg;
	unsigned short	p_status;
	unsigned short	p_li1;
	unsigned short	p_li2;
};
#define RM_MAXCOUNT	(64*1024 - 1) /* maximum supported record size */
