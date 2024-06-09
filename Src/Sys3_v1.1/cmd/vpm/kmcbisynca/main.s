/*
 * VPM interpreter
 *
 * Allocation of scratch-pad registers:
 *
 *	r15 - unused
 *	r14 - interpreter flags
 *	r13 - current state of the dialog with the PDP-11
 *	r12 - current state of the virtual process
 *	r11 - frame pointer (not used in phase 1)
 *	r10 - stack pointer
 *	 r9 - high-order bits of program counter (PC)
 *	 r8 - low-order bits of program counter (PC)
 *	 r7 - unused
 *	 r6 - accumulator (AC)
 *	 r5 - scratch
 *	 r4 - scratch
 *	 r3 - scratch
 *	 r2 - scratch
 *	 r1 - scratch
 *	 r0 - scratch
/*
 * Define the "BUSWAIT" macro
 */
#define BUSWAIT 0:mov npr,brg;br0 0b;mov nprx,brg;br0 buserr;
#include "sys/vpm.h"
/*
 * Define error codes for process-termination signal
 */
#define HALTRCVD	0
#define ILLEGAL		1
#define FETCHERR	2
#define STACKERR	3
#define JMPERR		4
#define BUSERR		5
#define XBUFERR		6
#define RBUFERR		7
#define EXITINST	8
#define CRCERR		9
#define HANGUP		10
/*
 * Define flags in r14
 */
#define	TOUT	(1<<0)
#define	REPORT	(1<<1)
#define	XEOM	(1<<2)
#define	XLAST	(1<<3)
/*
 * Define the states for a transmit or receive buffer
 * (Values in x_state and r_state)
 */
#define	NOBUF	0
#define	HAVEBUF	1
#define	OPEN	2
#define	CLOSED	3
/*
 * Define error flags for GETBYTE and PUTBYTE
 */
#define	NOMORE	(1<<0)
#define	NOROOM	(1<<0)
/*
 * Define the states of a virtual process (values in r12)
 */
#define	IDLE	0
#define	READY	1
#define	INWAIT	2
#define	OUTWAIT	3
#define	WAITDSR	4
/*
 * Define bits in npr
 */
#define NRQ (1<<0)
#define OUT (1<<4)
#define BYTE (1<<7)
/*
 * Define bits in nprx
 */
#define NEM (1<<0)
#define ACLO (1<<1)
#define PCLK (1<<4)
#define VEC4 (1<<6)
#define BRQ (1<<7)
/*
 * Define bits in csr0
 */
#define IEI (1<<0)
#define IEO (1<<4)
#define RQI (1<<7)
/*
 * Define bits in csr2
 */
#define RDYO (1<<7)
#define RDYI (1<<4)
/*
 * Define bits in out control register (lur1)
 */
#define OCLRP (1<<7)
#define OUTRDY (1<<4)
#define TEOM (1<<1)
#define TSOM (1<<0)
/*
 * Define bits in In control register (lur2)
 */
#define ICLRP (1<<7)
#define INRDY (1<<4)
/*
 * Define bits in modem control register (lur3)
 */
#define RING (1<<7)
#define DTR (1<<6)
#define HDX (1<<4)
#define MDMRDY (1<<3)
/*
 * Data definitions
 */
	.data
/*
 * Last command received from the pdp-11
 */
cmdsave: .byte	0,0,0,0,0,0,0,0
/*
 * Status bits for script interpreter
 *
 * The bits in the status byte are used as follows:
 *
 *	Bit	Meaning
 *
 *	0	Report 0 needed
 *	1	Report 1 needed
 *	2	Report 2 needed
 *	3	Report 3 needed
 */
reports:	.byte	0
/*
 * Temp save for report parameter
 */
errcode: .byte	0
/*
 * Reason for script termination
 */
termcode: .byte	0
/*
 * Temp save for TRACE parameter
 */
traceval: .byte	0
/*
 * Address of source/destination for i/o instruction
 */
ioadr:	.byte	0
/*
 * interval timer (one tick = 50 microseconds)
 */
clock:	.byte	0,0
/*
 * ticks until next timeout occurs
 */
clock1:	.byte	0
/*
 * Saved info from last time-out request
 */
toutsave: .byte	0,0,0,0
/*
 * address of array to receive crc accumulation
 */
crcsav:	.byte	0
/*
 * Transmit-buffer info
 */
x_desc:	.byte	0,0,0	/* descriptor address */
x_count: .byte	0,0	/* buffer size */
x_bufadr: .byte	0,0,0	/* buffer address */
x_type:	.byte	0	/* flags */
x_sta:	.byte	0	/* station */
x_dev:	.byte	0	/* device */
x_npc:	.byte	0,0	/* number of characters previously
			   fetched from this buffer */
x_cp:	.byte	0,0,0	/* pointer to next word in input buffer */
x_save:	.byte	0	/* saved byte from last two-byte fetch */
x_state: .byte	0	/* buffer state */
/*
 * Receive-buffer info
 */
r_desc:	.byte	0,0,0	/* descriptor address */
r_count: .byte	0,0	/* buffer size */
r_bufadr: .byte	0,0,0	/* buffer address */
r_type:	.byte	0	/* flags */
r_sta:	.byte	0	/* station */
r_dev:	.byte	0	/* device */
r_npc:	.byte	0,0	/* number of characters previously
			   fetched from this buffer */
r_cp:	.byte	0,0,0	/* pointer to next word in input buffer */
r_save:	.byte	0	/* saved byte from last two-byte fetch */
r_state: .byte	0	/* buffer state */
/*
 * Virtual-process instruction space starts here
 */
start:	/* Base address for instruction fetches */
#include	"opdef.h"
#include	"./sas_tempc"
end:
/*
 * Virtual process d-space starts here
 */
	.org	3*256
regs:
/*
 * Instruction text starts here
 */
	.text
/*
 * KMC initialization sequence--keep this at location zero
 */
	.org	0
	br	init	/* keep this instruction at location 0 */
/*
 * Vectors of jump instructions
 *
 * The reason for putting the vectors here is to avoid
 * splitting a vector across a page boundary.
 */
dispsw:
	br	disp0
	br	disp1
	br	disp2
	br	disp3
	br	disp4
	br	e1
/*
 * Switch on process state
 */
statesw:
	br	disp		/* 0 - idle */
	br	fetch		/* 1 - ready */
	br	inwait		/* 2 - input wait */
	br	outwait		/* 3 - output wait */
	br	waitdsr		/* 4 - dsr wait */
	br	e1
#include	"opsw.s"
/*
 * Initialize the kmc
 */
init:
	mov	0,brg
	mov	brg,csr0
	mov	brg,csr2
	mov	brg,r14
	mov	brg,r13
	mov	brg,r12
	mov	brg,lur3
	br	disp
/*
 * Dispatcher loop--keep looking for something to do
 */
disp:
/*
 * If the 50-microsecond timer has expired then go to tick
 */
	mov	nprx,brg
	br4	tick
/*
 * Go to disp0, disp1, disp2, disp3, or disp4 depending on the
 * current state of the dialog with the PDP-11
 */
	mov	dispsw,brg
	br	(add,brg,r13),%dispsw
/*
 * Case 0 (interface inactive):
 *
 * If the driver has set RQI then go to rqiset
 */
disp0:
	mov	csr0,brg
	br7	rqiset
/*
 * If there is something to report to the driver then go to report
 */
	mov	r14,brg
	br1	report
/*
 * Go to dispb
 */
	br	dispb
/*
 * The driver has requested the interface (by setting RQI)
 * in order to pass a command to the KMC.
 */
rqiset:
/*
 * Set RDYI
 */
	mov	csr2,r0
	mov	RDYI,brg
	or	brg,r0,csr2
/*
 * Set dialog state = 1
 */
	mov	1,brg
	mov	brg,r13
/*
 * Go to disp
 */
	br	disp
/*
 * Case 1 (RQI  received, RDYI sent):
 */
disp1:
/*
 * If the driver has cleared RDYI then go to command
 */
	mov	csr2,brg
	br4	1f
	br	command
1:
/*
 * If the driver has set IEI then go to ieiset
 */
	mov	csr0,brg
	br0	ieiset
/*
 * Go to dispb
 */
	br	dispb
/*
 * The driver has requested an interrupt (by setting IEI).
 * The driver has already set RQI and the KMC has responded
 * by setting RDYI.
 */
ieiset:
/*
 * Send interrupt 1 to the PDP-11
 */
	mov	BRQ,brg
	mov	brg,nprx
/*
 * Set dialog state = 2
 */
	mov	2,brg
	mov	brg,r13
/*
 * Go to disp
 */
	br	disp
/*
 * Case 2 (RQI and IEI received, RDYI and BRQ sent):
 */
disp2:
/*
 * If the driver has cleared RDYI and BRQ then go to command,
 * otherwise go to dispb
 */
	mov	csr2,brg
	br4	dispb
	mov	nprx,brg
	br7	dispb
	br	command
/*
 * Case 3 (RDYO has been set)
 */
disp3:
/*
 * If the driver has set IEO then go to ieoset, else if the
 * driver has cleared RDYO then set dialog state = 0 and
 * go to disp;
 * otherwise go to dispb
 */
	mov	csr0,brg
	br4	ieoset
	mov	csr2,brg
	br7	dispb
	mov	0,brg
	mov	brg,r13
	br	disp
/*
 * Case 4 (RDYO, BRQ, and VEC4 have been set):
 */
disp4:
/*
 * If the driver has cleared RDYO and BRQ then set dialog state = 0
 * and go to disp;
 * otherwise go to dispb
 */
	mov	csr2,brg
	br7	dispb
	mov	nprx,brg
	br7	dispb
	mov	0,brg
	mov	brg,r13
	br	disp
/*
 * Branch to disp, ready, inwait, outwait, or waitdsr
 * depending on the current state of the virtual process
 */
dispb:
	mov	statesw,brg
	br	(add,brg,r12),%statesw
/*
 * The KMC has something to report to the driver
 *
 * (We get here from state 0 when the report-needed bit in r14
 * is set.)
 *
 * Reports are passed to the driver via the shared-memory interface
 * as follows:
 *
 *	Byte	Bits		Contents
 *
 *	csr2	2-0	Command type (0-7)
 *	csr3	5-0	Line number (0-63)
 *	csr4-csr7	Content depends on report type.
 */
report:
/*
 * Get report-scheduling bits
 */
	mov	reports,mar
	mov	%reports,%mar
	mov	mem,r0|brg
/*
 * If bit 0 is set then go to xbufout;
 * else if bit 1 is set then go to rbufout;
 * else if bit 2 is set then go to traceout;
 * else if bit 3 is set then go to errout;
 * else if bit 4 is set then go to startout;
 */
	br0	xbufout
	br1	rbufout
	br4	startout
	mov	0,brg>>
	mov	0,brg>>
	br0	traceout
	br1	errout
 /*
 * Clear the report-needed bit in r14
 */
	mov	~REPORT,brg
	and	brg,r14
/*
 * Go to disp
 */
	br	disp
/*
 * Report 0:  Send an RTNXBUF signal to the driver
 *
 * The format of an RTNXBUF report is as follows:
 *
 *	Byte	Contents
 *
 *	csr2 - report number
 *	csr3 - line number
 *	csr4 - bits 7-0 of the descriptor address
 *	csr5 - bits 15-8 of the descriptor address
 *	csr6 - bits 17-16 of the descriptor address
 *	csr7 - unused
 */
xbufout:
/*
 * Clear the bit that says to send this report
 */
	mov	~(1<<0),brg
	and	brg,r0,mem
/*
 * Put the address of the buffer descriptor into csr4-csr6
 */
	mov	x_desc,mar
	mov	mem,csr4|mar++
	mov	mem,csr5|mar++
	mov	mem,csr6
/*
 * Set up a unibus request to update the XTYPE parameter
 * in the pdp-11 copy of the buffer descriptor
 */
#ifdef	XBP
	mov	x_desc,mar
	mov	%x_desc,%mar
	mov	mem,r0|mar++
	mov	mem,r1|mar++
	mov	mem,r2
	mov	5,brg
	add	brg,r0,brg
	mov	brg,oal
	adc	r1,brg
	mov	brg,oah
	adc	r2,brg
	mov	3,r3
	and	brg,r3
	asl	r3
	asl	r3
	mov	nprx,r0
	mov	~(BRQ|ACLO),brg
	and	brg,r0
	mov	r3,brg
	or	brg,r0,nprx
	mov	BYTE|OUT|NRQ,brg
	mov	brg,npr
/*
 * Wait for the unibus transfer to complete
 */
	BUSWAIT
#endif
/*
 * Set buffer state = NOBUF
 */
	mov	x_state,mar
	mov	NOBUF,mem
/*
 * Put the report number into brg
 */
	mov	RRTNXBUF,brg
/*
 * Go to reporte
 */
	br	reporte
/*
 * Report 1:  Send a RTNRBUF signal to the driver
 *
 * The format of a RTNRBUF report is as follows:
 *
 *	Byte	Contents
 *
 *	csr2 - report number
 *	csr3 - line number
 *	csr4 - bits 7-0 of the descriptor address
 *	csr5 - bits 15-8 of the descriptor address
 *	csr6 - bits 17-16 of the descriptor address
 *	csr7 - unused
 */
rbufout:
/*
 * Clear the bit that says to send this report
 */
	mov	~(1<<1),brg
	and	brg,r0,mem
/*
 * Put the descriptor address in csr4-csr6
 */
	mov	r_desc,mar
	mov	mem,csr4|mar++
	mov	mem,csr5|mar++
	mov	mem,csr6
/*
 * Set up a unibus request to update the RTYPE parameter
 * in the pdp-11 copy of the buffer descriptor
 */
#ifdef	RBP
	mov	r_type,mar
	mov	mem,odh
	mov	r_desc,mar
	mov	mem,r0|mar++
	mov	mem,r1|mar++
	mov	mem,r2
	mov	5,brg
	add	brg,r0,oal
	adc	r1,brg
	mov	brg,oah
	adc	r2,brg
	mov	3,r3
	and	brg,r3
	asl	r3
	asl	r3
	mov	nprx,r0
	mov	~(BRQ|ACLO),brg
	and	brg,r0
	mov	r3,brg
	or	brg,r0,nprx
	mov	BYTE|OUT|NRQ,brg
	mov	brg,npr
/*
 * Wait for the unibus transfer to complete
 */
	BUSWAIT
#endif
/*
 * Set up a unibus request to update the COUNT parameter
 * in the pdp-11 copy of the buffer descriptor
 */
	mov	r_npc,mar
	mov	mem,odl|mar++
	mov	mem,odh
	mov	r_desc,mar
	mov	mem,oal|mar++
	mov	mem,oah|mar++
	mov	3,r3
	and	mem,r3
	asl	r3
	asl	r3
	mov	nprx,r0
	mov	~(BRQ|ACLO),brg
	and	brg,r0
	mov	r3,brg
	or	brg,r0,nprx
	mov	OUT|NRQ,brg
	mov	brg,npr
/*
 * Wait for the unibus transfer to complete
 */
	BUSWAIT
/*
 * Set buffer state = NOBUF
 */
	mov	r_state,mar
	mov	NOBUF,mem
/*
 * Put the report type in brg
 */
	mov	RRTNRBUF,brg
/*
 * Go to reporte
 */
	br	reporte
/*
 * Report 2:  send a TRACE signal to the driver
 */
traceout:
/*
 * Clear the bit that says to send this report
 */
	mov	~(1<<2),brg
	and	brg,r0,mem
/*
 * Put the trace parameter in csr6
 */
	mov	traceval,mar
	mov	%traceval,%mar
	mov	mem,csr6
/*
 * Put reentry address in csr4-csr5
 */
	mov	r8,brg
	mov	brg,csr4
	mov	r9,brg
	mov	brg,csr5
/*
 * Put the report type in brg
 */
	mov	RTRACE,brg
/*
 * Go to reporte
 */
	br	reporte
/*
 * Report 3:  send an ERRTERM signal to the driver
 */
errout:
/*
 * Clear the bit that says to send this report
 */
	mov	~(1<<3),brg
	and	brg,r0,mem
/*
 * Put the termination code in csr6
 */
	mov	termcode,mar
	mov	%termcode,%mar
	mov	mem,csr6
/*
 * Put reentry address in csr4-csr5
 */
	mov	r8,brg
	mov	brg,csr4
	mov	r9,brg
	mov	brg,csr5
/*
 * Put the report type in brg
 */
	mov	ERRTERM,brg
/*
 * Go to reporte
 */
	br	reporte
/*
 * Report 4:  send a STARTUP signal to the driver
 */
startout:
/*
 * Clear the bit that says to send this report
 */
	mov	~(1<<4),brg
	and	brg,r0,mem
	mov	1,brg
	mov	brg,csr4
	mov	brg,csr5
	mov	0,brg
	mov	brg,csr6
	mov	brg,csr7
/*
 * Put the report type in brg
 */
	mov	STARTUP,brg
/*
 * Go to reporte
 */
	br	reporte
/*
 * Wrapup of report generation
 */
reporte:
/*
 * Put report type in csr2
 */
	mov	brg,csr2
/*
 * Set RDYO
 */
	mov	csr2,r0
	mov	RDYO,brg
	or	brg,r0,csr2
/*
 * If IEO is set then go to ieoset
 */
	mov	csr0,brg
	br4	ieoset
/*
 * Set dialog state = 3
 */
	mov	3,brg
	mov	brg,r13
/*
 * Go to disp
 */
	br	disp
/*
 * Continuation of driver-report processing
 *
 * We come here when the driver sets IEO (in response to
 * our setting RDYO)
 */
ieoset:
/*
 * Send interrupt 2 to the PDP-11
 */
	mov	BRQ|VEC4,brg
	mov	brg,nprx
/*
 * Set dialog state = 4
 */
	mov	4,brg
	mov	brg,r13
/*
 * Go to disp
 */
	br	disp
/*
 * Process a command from the driver
 */
command:
/*
 * Set dialog state = 0
 */
	mov	0,brg
	mov	brg,r13
/*
 * Save command info for debugging
 */
	mov	cmdsave,mar
	mov	%cmdsave,%mar
	mov	csr0,mem|mar++
	mov	csr1,mem|mar++
	mov	csr2,mem|mar++
	mov	csr3,mem|mar++
	mov	csr4,mem|mar++
	mov	csr5,mem|mar++
	mov	csr6,mem|mar++
	mov	csr7,mem|mar++
/*
 * Get the command type from csr2
 */
	mov	csr2,r0
	mov	7,brg
	and	brg,r0
/*
 * If command type == 0 then go to xbufin
 * else if command type == 1 then go to rbufin
 * else if command type == 2 then go to run
 * else if command type == 3 then go to halt
 * else if command type == 4 then go to halt
 * else go to disp
 */
	dec	r0
	brz	xbufin
	dec	r0
	brz	rbufin
	dec	r0
	brz	run
	dec	r0
	brz	halt
	dec	r0
	brz	halt
	br	disp
/*
 * Process a RUN command
 *
 * The format of a RUN command is as follows:
 *
 *	Byte		Contents
 *
 *	csr2	Command type
 *	csr3	Line number
 *	csr4-csr7 unused
 */
run:
/*
 * Clear x_state and r_state
 */
	mov	x_state,mar
	mov	%x_state,%mar
	mov	0,mem
	mov	r_state,mar
	mov	0,mem
/*
 * Clear the user data area
 */
	mov	SSTACK,brg
	mov	brg,r0|mar
	mov	%regs,%mar
run2:
	mov	0,mem
	inc	r0,r0|mar
	brc	0f
	br	run2
0:
/*
 * Initialize the PC
 */
	mov	0,brg
	mov	brg,r8
	mov	brg,r9
/*
 * Initialize the stack pointer
 */
	mov	SSTACK,brg
	mov	brg,r10
/*
 * Turn on data terminal ready and set receive mode = half duplex.
 */
	mov	DTR|HDX,brg
	mov	brg,lur3
/*
 * Set process state = READY
 */
	mov	READY,brg
	mov	brg,r12
/*
 * Clear the AC
 */
	mov	0,brg
	mov	brg,r6
/*
 * Schedule a STARTUP report
 */
	mov	1<<4,brg
	br	rptschd
/*
 * Schedule a report
 *
 * Entered with 1<<(report number) in brg
 */
rptschd:
	mov	brg,r0
	mov	reports,mar
	mov	%reports,%mar
	or	mem,r0,mem
/*
 * Set the report-needed flag in r14
 */
	mov	REPORT,brg
	or	brg,r14
/*
 * Go to disp
 */
	br	disp
/*
 * Process an EXIT command
 */
halt:
/*
 * Clear data-terminal-ready
 */
	mov	0,brg
	mov	brg,lur3
/*
 * Get reason for termination
 */
	mov	HALTRCVD,brg
/*
 * Go to errterm
 */
	br	errterm
/*
 * Process a RESET command
 *
 * The format of a RESET command is as follows:
 *
 *	Byte		Contents
 *
 *	csr2	Command type
 *	csr3	Line number
 *	csr4-csr7 unused
 */
reset:
/* *** reset code moved to run command *** */
	br	disp
/*
 * Process an XBUFIN command
 *
 * The format of an XBUFIN command is:
 *
 *	Byte		Contents
 *
 *	csr2	Command type
 *	csr3	Line number
 *	csr4	Bits 7-0 of the descriptor address
 *	csr5	Bits 15-8 of the descriptor address
 *	csr6	Bits 17-16 of the descriptor address
 *	csr7	Unused
 */
xbufin:
/*
 * If we already have a transmit buffer then error
 */
	mov	x_state,mar
	mov	mem,r0
	dec	r0,-
	brc	xbuferr
/*
 * If the buffer address is odd then go to xbuferr
 */
	mov	csr4,brg
	br0	xbuferr
/*
 * Set the buffer state = READY to be opened.
 */
	mov	HAVEBUF,mem
/*
 * Record the address of the buffer descriptor
 */
	mov	x_desc,mar
/*
 * Merge with code for rbufin
 */
	br	rbufin0
/*
 * Process a receive-buffer-in command
 *
 * The format of an RBUFIN command is:
 *
 *	Byte		Contents
 *
 *	csr2	Command type
 *	csr3	Line number
 *	csr4	Bits 7-0 of the descriptor address
 *	csr5	Bits 15-8 of the descriptor address
 *	csr6	Bits 17-16 of the descriptor address
 *	csr7	Unused
 */
rbufin:
/*
 * If we already have a receive buffer then error
 */
	mov	r_state,mar
	mov	mem,r0
	dec	r0,-
	brc	rbuferr
/*
 * If the buffer address is odd then go to rbuferr
 */
	mov	csr4,brg
	br0	rbuferr
/*
 * Set the buffer state = READY
 */
	mov	HAVEBUF,mem
/*
 * Record the address of the buffer descriptor
 */
	mov	r_desc,mar
/*
 * Entered here from xbufin
 */
rbufin0:
	mov	csr4,mem
	mov	mem,r0|mar++
	mov	csr5,mem
	mov	mem,r1|mar++
	mov	csr6,mem
	mov	mem,r2|mar++
/*
 * Copy the buffer descriptor into the kmc data space
 *
 * The format of the buffer descriptor is as follows:
 *
 *	Byte		Contents
 *
 *	0	Bits 7-0 of the count
 *	1	Bits 15-8 of the count
 *	2	bits 7-0 of the buffer address
 *	3	Bits 15-8 of the buffer address
 *	4	Bits 17-16 of the buffer address (right-adjusted)
 *	5	Type
 *	6	Station
 *	7	Device
 */
/*
 * The following loop copies eight bytes from the pdp-11
 * to the kmc (two bytes on each iteration).
 *
 * Set loop counter for four iterations.
 */
	mov	3,brg
	mov	brg,r4
/*
 * Loop starts here
 */
rbufin1:
/*
 * Set up unibus request for two bytes
 */
	mov	r0,ial
	mov	r1,brg
	mov	brg,iah
	mov	r2,brg
	mov	3,r3
	and	brg,r3
	asl	r3
	asl	r3
	mov	NRQ,brg
	or	brg,r3,brg
	mov	brg,npr
/*
 * Wait for the completion of the unibus transfer
 */
	BUSWAIT
/*
 * Store both bytes in the kmc
 */
	mov	idl,mem|mar++
	mov	idh,mem|mar++
/*
 * Decrement the loop counter.  If the result indicates
 * that the counter value was zero, go to disp
 */
	dec	r4
	brz	disp
/*
 * Increment the unibus address by two
 */
	mov	2,brg
	add	brg,r0
	adc	r1
	adc	r2
/*
 * Go to rbufin1
 */
	br	rbufin1
/*
 * The 50-microsecond hardware timer has expired
 */
tick:
/*
 * Reset the 50-microsecond timer
 */
	mov	nprx,r0
	mov	~(BRQ|ACLO),brg
	and	brg,r0
	mov	PCLK,brg
	or	brg,r0,nprx
/*
 * If no timeout in progress then go to disp
 */
	mov	r14,brg
	br0	1f
	br	disp
1:
/*
 * Decrement the low-order byte of the clock register.
 * If the result is non-negative then go to disp.
 */
	mov	%clock,%mar
	mov	clock,mar
	mov	mem,r0
	dec	r0,mem|mar++
	brc	disp
/*
 * Decrement the high-order byte of the clock register.
 * If the result is non-negative then go to disp.
 */
	mov	mem,r0
	dec	r0,mem|mar++
	brc	disp
	mov	clock,mar
/*
 * Reload the 100-millisecond timer
 */
	mov	208,mem|mar++
	mov	7,mem
/*
 * Decrement the twait count.
 * If the result is non-negative then go to disp.
 */
	mov	clock1,mar
	mov	mem,r0
	dec	r0,mem
	brc	disp
/*
 * Clear the timeout-in-progress flag
 */
	mov	~TOUT,brg
	and	brg,r14
/*
 * Set process state = READY.  (It may have been waiting on i/o.)
 */
	mov	READY,brg
	mov	brg,r12
/*
 * Set AC = 1
 */
	mov	1,brg
	mov	brg,r6
/*
 * Point to save area
 */
	mov	toutsave,mar
/*
 * Restore the stack pointer and frame pointer
 */
	mov	mem,r10|mar++
	mov	mem,r11|mar++
/*
 * Get the saved jump address
 */
	mov	mem,r0|mar++
	mov	mem,r1|mar++
/*
 * Go to jump
 */
	br	jump
/*
 * Process state == DSR WAIT:  Check for dataset ready
 */
waitdsr:
/*
 * If modem ready is not on then go to disp
 */
	mov	lur3,r0
	asl	r0,brg
	br4	1f
	br	disp
1:
/*
 * Set process state = READY
 */
	mov	READY,brg
	mov	brg,r12
/*
 * Go to disp
 */
	br	disp
/*
 * Process state == INPUT WAIT:  Check for input ready
 */
inwait:
/*
 * If modem ready is not on then go to hangup
 */
	mov	lur3,r0
	asl	r0,brg
	br4	1f
	br	hangup
1:
/*
 * If the input silo is empty then go to disp
 */
	mov	lur2,brg|r0
	br4	1f
	br	disp
1:
/*
 * Get the saved address of the vpm memory location
 * to receive this byte
 */
	mov	ioadr,mar
	mov	%ioadr,%mar
	mov	mem,mar
	mov	%regs,%mar
/*
 * Move the received data byte to the specified location
 */
	mov	lur0,brg
	mov	brg,mem
/*
 * Set process state = READY
 */
	mov	READY,brg
	mov	brg,r12
/*
 * Go to disp
 */
	br	disp
/*
 * Process state == OUTPUT WAIT:  Check for output ready
 */
outwait:
/*
 * If modem ready is not on then go to hangup
 */
	mov	lur3,r0
	asl	r0,brg
	br4	1f
	br	hangup
1:
/*
 * If the output silo is full then go to disp
 */
	mov	lur1,brg
	br4	1f
	br	disp
1:
/*
 * If XEOM is set then go to xeomset
 */
	mov	XEOM,brg
	orn	brg,r14,-
	brz	xeomset
/*
 * Clear TSOM
 */
	mov	0,brg
	mov	brg,lur1
/*
 * Load the character to be transmitted into the output
 * data silo
 */
	mov	ioadr,mar
	mov	%ioadr,%mar
	mov	mem,lur0
/*
 * If XLAST is set then go to xlastset
 */
	mov	XLAST,brg
	orn	brg,r14,-
	brz	xlastset
/*
 * Set process state = READY.
 */
	mov	READY,brg
	mov	brg,r12
/*
 * Go to disp
 */
	br	disp
/*
 * Prepare to shut down the transmitter after this character.
 */
xlastset:
/*
 * Set XEOM
 */
	mov	XEOM,brg
	or	brg,r14
/*
 * Go to disp
 */
	br	disp
/*
 * Shut down the transmitter.
 */
xeomset:
/*
 * Set the TEOM bit in the out control register
 */
	mov	TEOM,brg
	mov	brg,lur0
/*
 * Clear XEOM and XLAST
 */
	mov	~(XEOM|XLAST),brg
	and	brg,r14
/*
 * Go to disp
 */
	br	disp
/*
 * A disconnect has occurred--terminate the script
 */
hangup:
	mov	HANGUP,brg
	br	errterm
#include	"fetch.s"
