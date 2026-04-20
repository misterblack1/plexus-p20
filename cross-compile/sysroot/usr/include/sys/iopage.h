/* SID @(#)iopage.h	5.3 */

/*
	this include file defines the memory usage of the iopage.
	At system boot time this page is mapped into the highest
	4k of the multibus memory space. It is used by various
	drivers for control structures needed by multibus boards.
	Most multibus devices communicate with their host via
	a buffer in multibus memory. The address of this buffer
	is either set with jumpers or loaded through an i/o port.
	This page is used for these buffers.

	To date this allocation has been somewhat sloppy. Each
	drivers area seems to begin at a 0x100 byte boundary.
	There are probably a few that do not use the entire
	0x100 bytes.
*/

#define addrto8086seg(s)	(((unsigned)(s) & 0xf0000)>>4)

/******** addresses 0x000 - 0x0ff  available ********/

/*
	addresses 0x100 - 0x1ff used for 9-track tape controller
*/
#define RM_SCBA	(LA_SYSIO + 0x100)
#define RM_SCB	((struct scb_struct *)RM_SCBA)
#define RM_XCBA	(RM_SCBA + sizeof(struct scb_struct))
#define RM_XCB	((struct xcb_struct *)RM_XCBA)
#define RM_PBA	(RM_XCBA + sizeof(struct xcb_struct))
#define RM_PB	((struct pb_struct *)RM_PBA)

/*
	addresses 0x200 - 0x3ff used for xylogic disk controllers
*/
/* 16 iopb's to allow one request per drive */
#define XY_WUA(x)	(LA_SYSIO + 0x200 + ((x) * 0x20))

/*
	addresses 0x400 - 0x5ff used for SCSI host adaptor info
*/
/* communication area used by host adaptor software */
#define SCSI_COMM	(LA_SYSIO + 0x400)
/* 7 buffers available to drivers for misc commands such as request sense */
#define SCSI_BUFSZ	0x20
#define SCSI_BUF(dev)	((unsigned char*)(LA_SYSIO+0x500 + ((dev)*SCSI_BUFSZ)))


/******** addresses 0x600 - 0x7ff  available ********/


/*
	addresses 0x800 - 0xcff used for SIOC control structure.
*/

#define	PBASE0	(LA_SYSIO + 0x800)
#define PBASE1	(LA_SYSIO + 0x900)
#define PBASE2 	(LA_SYSIO + 0xa00)
#define PBASE3	(LA_SYSIO + 0xb00)
#define PBASE4	(LA_SYSIO + 0xc00)

/******** addresses 0xd00 - 0xdff  available ********/

/*
	addresses 0xe00 - 0xeff used for imsc controller
*/
#define IMSC_WUA 0x001f	/* imsc wake up address (multibus i/o port) */
#define PD_WUB	 (LA_SYSIO + 0xe00)
#define PT_WUB	 (PD_WUB + sizeof(cab_t))


/******** addresses 0xf00 - 0xff5  available ********/


/*
	addresses 0xff6-0xfff tapemaster 9-track tape wakeup address
*/
#define	RM_SCP	((struct scp_struct *) (0xfff6|LA_SYSIO))

/*
 * Defines specific to KICKER systems only
 */
#ifdef KICKER
/*
 * Define some QIC2/R driver macros
 */
#define	TS_WUA(c)	(LA_VMESYSIO + 0x000 | ((c) << 5))
#endif
