/*
 * This source contains defines and structures used by the CPC Tapemaster
 * tape controller.
 */

/* ioctl requests */

#define	RMPOSN		(('r'<<8)|0)

/* commands */

#define C_CONFIG	0x00
#define	C_OLREW		0x04
#define	C_RFOREIGN	0x1c
#define	C_NOP		0x20
#define	C_STATUS	0x28
#define C_READ		0x2c
#define C_WRITE		0x30
#define	C_REW		0x34
#define	C_UNLOAD	0x38
#define	C_WEOF		0x40
#define	C_SRCHEOF	0x44
#define	C_SPACE		0x48
#define	C_ERASE		0x4c
#define	C_ERASEALL	0x50
#define	C_SPACEEOF	0x70
#define	C_SRCHMEOF	0x94
#define	C_REV		0x4000

/* values for xcb fields */

#define	CCWNORM		0x11
#define	CCWCLR		0x09
#define	GATECLOSED	0xff
#define	GATEOPEN	0x00

/* control fields */

#define	BUS16	0x8000
#define	CONT	0x1000
#define	HISPEED	0x0800
#define	REVERSE	0x0400
#define	BANK1	0x0100
#define	BUSLOK	0x0080
#define	LINK	0x0040
#define	INTR	0x0020
#define	MAILBOX	0x0010
#define	UNIT0	0x0000
#define	UNIT1	0x0004
#define	UNIT2	0x0008
#define	UNIT3	0x000c

/* status fields */

#define	ENTERED		0x8000
#define	COMPLETED	0x4000
#define	RETRY		0x2000
#define	ERRMASK		0x1f00
#define	ERRSHIFT	8
#define	FILEMARK	0x0080
#define	ONLINE		0x0040
#define	LOADPOINT	0x0020
#define	EOT		0x0010
#define	READY		0x0008
#define	FORMATBSY	0x0004
#define	WRITEPROTECT	0x0002

/* error codes */

#define NOERR		(0x00 << ERRSHIFT)
#define HARDERR		(0x0a << ERRSHIFT)
#define	OVERRUN		(0x0b << ERRSHIFT)
#define TIMEOUT		(0x0f << ERRSHIFT)
#define EOFDETECT	(0x15 << ERRSHIFT)

/*
 * structure used in ioctl calls to rm tape driver
 */

struct rmcmd_struct {
	unsigned rm_cmd;
	unsigned rm_cnt;
	unsigned rm_status;
};

/* 
 * structures used by the controller
 */

struct	scp_struct {
	char	s_scpr1;
	char	s_sysbus;
	unsigned	s_scboff;
	unsigned	s_scbseg;
};

struct	scb_struct {
	char	s_scbr1;
	char	s_soc;
	unsigned	s_xcboff;
	unsigned	s_xcbseg;
};

struct	xcb_struct {
	char	c_gate;
	char	c_ccw;
	unsigned	c_pboff;
	unsigned	c_pbseg;
	unsigned	c_xcbr1;
};

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
	unsigned	p_li1;
	unsigned	p_li2;
};
