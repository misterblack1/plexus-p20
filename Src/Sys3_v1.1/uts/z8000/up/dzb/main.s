/*
 * This is the KMC-11 portion of the UTS driver for a DZ-11
 * with KMC assist.
 *
 * The following functions are performed in the KMC:
 *
 *	1. On command from the driver, transmit multi-character
 *	   buffers to output lines.
 *
 *	2. Notify the driver of output buffer completions.
 *
 *	3. Optionally insert delays for carriage-control characters
 *	   (CR, LF, BS, HT, VT, and FF).
 *
 *	4. Optionally replace HT by an appropriate number of blanks.
 *
 *	5. Optionally replace LF by CR-LF.
 *
 *	6. Optionally generate escape sequences for upper-case-only
 *	   terminals.
 *
 *	7. Strip out EOT.
 *
 * Permanently assigned registers:
 *
 *	r15 - 100-microsecond timer/DZ-11 commutator
 *	r14 - dispatcher flag bits
 *	r13 - current state of the dialog with the PDP-11
 *	r12 - pointer to list of available queue entries
 *	r11 - pointer to list of available 16-byte input buffers
 *	r10 - address of current line-table entry
 *	r9 - unused
 *	r8 - DZ number currently being serviced
 *	r7 - entry number of current line-table entry
 *
 * The structure of a line-table entry is as follows:
 *
 *  Byte 	Contents
 *
 *   0 - current state of this line
 *
 *   1 - number of characters remaining in the current output buffer
 *
 *   2 - high-order byte of the current output word
 *
 *   3 - saved character to be sent after the current character
 *
 *   4 - low-order byte of the address of the current output word
 *
 *   5 - high-order byte of the address of the current output word
 *
 *   6 - bits 17-16 of the address of the current output word.
 *
 *   7 - flags
 *
 *   8 - More flags (overflow from flags)
 *
 *   9 - count for time delay, etc.
 *
 *  10 - column pointer for tab and delay calculations
 *
 *  11 - pointer to 4-byte queue entry which contains the
 *	 address of the next output buffer, if any
 *
 *  12 - pointer to head of input buffer queue
 *
 *  13 - pointer to end of input buffer queue
 *
 *  14 - number of characters in kmc input queue
 *
 *  15 - initial value for ITIME
 *
 *  16 - timer from first character until input is queued
 *
 *  17 - low byte of c_iflag
 *
 *  18 - high byte of c_iflag
 *
 *  19 - low byte of c_oflag
 *
 *  20 - high byte of c_oflag (delay bits)
 *
 *  21 - index of first character in input buffer to be copied into c-list
 *
 *  22 - next (express) character to be output (used with SNDEXP)
 *
 *  23 - unused
 *
 * The flag bits in byte 7 are used as follows:
 *
 *  Bit		Meaning
 *
 * 1<<1 - The high-order data byte has not been sent
 * 1<<4 - There is a byte in SAVECHAR to be sent
 * 1<<0 - The current newline character has already been expanded
 *
 * Define useful constants
 */
#define	NIL	0377
#define	EOT	004
#define	NUL	0
#define	CEOT	004
#define	NL	012
#define	CR	015
#define	CSTART	021
#define	CSTOP	023
#define	SP	040
#define BS	010
#define HT	011
#define	ESCAPE	0134
#define	DEL	0177
#define	CQUIT	034
#define	CINTR	0177
/*
 * Define report types
 */
#define	XBUFOUT	0
#define	RBUFOUT	1
#define	RBUF1OT	2
#define	RBRK	3
#define	COCHG	4
#define	ERROUT	5
/*
 * Define the byte offset for the fields of a line-table entry
 *
 * Note:  Must have STATE addr = 0
 *
 */
#define	STATE	0
#define	NCH	1
#define	HIBYTE	2
#define	SAVECHAR  3
#define	BUFAD	4
#define	FLAGS	7
#define	FLAGS1  8
#define	COUNT	9
#define	ICOL	10
#define	OUTBUF	11
#define	HEADINQ	12
#define	TAILINQ	13
#define	NICH	14
#define	INITIM	15
#define	ITIME  16
#define IFLAG1	17
#define IFLAG2	18
#define OFLAG	19
#define DELAYS	20
#define RBCOPO	21
#define EXPCHR 22
#define UNUSED 23

/*
 * STATE == 0	no output in progress
 * STATE == 1	sending normal data
 * STATE == 2	sending character that needs time fill
 * STATE == 3	expanding horizontal tabs
 * STATE == 4	sending fill characters
 * STATE == 5	twait state (Timer running)
 */
/*
 * Define bits in FLAGS 
 */
#define	NLEXP	(1<<0)
#define	SENDHIGH	(1<<1)
#define	EMITESC	(1<<2)
#define	SENDSAVE	(1<<4)
#define	IPREVESC	(1<<5)
#define	CARR_ON	(1<<6)
/*
 * Define bits in FLAGS1 (overflow from FLAGS)
 */
#define	TTSTOP	(1<<0)
#define DELIM (1<<1)
#define TIMACT	(1<<2)
#define LTEINQ (1<<4)
#define PREVNB	(1<<5)
#define SNDEXP	(1<<6)
#define OUTBRK	(1<<7)
/*
 * Define the bits in IFLAG1
 */
#define	IGNBRK	(1<<0)
#define	BRKINT	(1<<1)
#define	IGNPAR	(1<<2)
#define	PARMRK	(1<<3)
#define	INPCK	(1<<4)
#define	ISTRIP	(1<<5)
#define	INLCR	(1<<6)
#define	IGNCR	(1<<7)
/*
 * Define bits in IFLAG2
 */
#define	ICRNL	(1<<0)
#define	IUCLC	(1<<1)
#define	IXON	(1<<2)
#define	IXANY	(1<<3)
/*
 * Define the bits in OFLAGS
 */
#define	OPOST	(1<<0)
#define	OLCUC	(1<<1)
#define	ONLCR	(1<<2)
#define	OCRNL	(1<<3)
#define	ONOCR	(1<<4)
#define	ONLRET	(1<<5)
#define	OFILL	(1<<6)
#define	OFDEL	(1<<7)
/*
 * Define the bits in DELAYS
 */
#define	NLDELAY	(1<<0)
#define	CRDELAY	(3<<1)
#define	HTDELAY	(3<<3)
#define	BSDELAY	(1<<5)
#define	VTDELAY	(1<<6)
#define	FFDELAY	(1<<7)
/*
/*
 * Define the relevant items of a clist block
 */
#ifdef	vax
#define	C_FIRST	4
#define	C_DATA	6
#else
#define	C_FIRST	2
#define	C_DATA	4
#endif
/*
 * Define the length of the data portion of a clist block
 */
#define	CLSIZE	24
/*
 * Define the "BUSWAIT" macro
 */
#define BUSWAIT(X) 1:mov npr,brg;br0 1b;mov nprx,brg;br0 X;
/*
 * Buffer-management macros
 *
 * r11 points to the head of the free buffer list
 */
#define	RELEASE(X)\
	mov	017,brg;\
	and	brg,X,%mar;\
	mov	~017,brg;\
	and	brg,X,mar;\
	mov	r11,mem;\
	mov	X,brg;\
	mov	brg,r11

#define	ALLOCATE(X,Y)\
	mov	r11,brg;\
	brz	Y;\
	mov	brg,X;\
	mov	017,brg;\
	and	brg,X,%mar;\
	mov	~017,brg;\
	and	brg,X,mar;\
	mov	mem,r11
/*
 * Macros to get and release queue entries
 *
 * r12 points to the list of available queue entries
 */
#define	FREEQ(X)\
	mov	03,brg;\
	and	brg,X,%mar;\
	mov	~03,brg;\
	and	brg,X,mar;\
	mov	r12,mem;\
	mov	X,brg;\
	mov	brg,r12

#define	GETQ(X,Y)\
	mov	r12,brg;\
	brz	Y;\
	mov	brg,X;\
	mov	03,brg;\
	and	brg,X,%mar;\
	mov	~03,brg;\
	and	brg,X,mar;\
	mov	mem,r12
/*
 * Subroutine CALL and RETURN macros
 *
 * Nested calls are not supported
 */
#define	CALL(X)\
	mov	%return,%mar;\
	mov	return,mar;\
	mov	%0f,mem|mar++;\
	mov	0f,mem;\
	mov	%X,brg;\
	mov	brg,pcr;\
	jmp	X;\
0:

#define	RETURN	\
	mov	%return,%mar;\
	mov	return,mar;\
	mov	mem,pcr|mar++;\
	jmp	(mem)

#define	ERROR(X) \
	mov	X,brg; \
	mov	brg,r1; \
	CALL(erqueue);

#define HALT	br	.;

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
#define RQI (1<<7)
#define IEI (1<<0)
#define IEO (1<<4)
/*
 * Define bits in csr2
 */
#define RDYO (1<<7)
#define RDYI (1<<4)
/*
 * Define bits in r14
 */
#define	CHKOUTQ	(1<<0)
#define	CHKBRKQ	(1<<2)
#define	CHKERRQ	(1<<3)
#define	CHKCSRQ	(1<<4)
#define	CHKCOCQ	(1<<5)
#define	CHKINRQ	(1<<6)
/*
 * Define error codes
 */
#define	BUSERR0	1
#define	BASEINERR  2
#define	ERRA	3
#define	ERRB	4	/* Invalid line/DZ number in output completion queue */
#define	ERRC	5	/* Invalid state value in r13 */
#define	BUSERR2	6
#define	OUTSTART  7
#define	FLUSH	8
#define	ERRE	9
#define	ERRF	10
#define	ERRG	11
#define	ERRH	12
#define	QUEUERR0	13
#define BUSERR1	14
#define BUSERR3	15
#define INVALCD	16
/*
 * Data definitions
 */
	.data
/*
 * The following data structures are replicated on a different
 * page for each DZ-11
 */
	.org	0
/*
 * Save room for the line-table entry address table
 * and the enable bits
 */
	.org	.+8
/*
 * Define the macro for generating line-table entries
 */
#define	LTENT	.org	.+24
/*
 * Now generate the actual line-table entries
 */
lte0:	LTENT
lte1:	LTENT
lte2:	LTENT
lte3:	LTENT
lte4:	LTENT
lte5:	LTENT
lte6:	LTENT
lte7:	LTENT
/*
 * Unibus base address for this DZ-11
 *
 * Bits 7-0 are in the first byte, bits 15-8 are in the second byte,
 * and bits 17-16 are in the third byte (right-adjusted)
 */
csrget:	.byte	0,0,0
/*
 * Cell used to save status info for each active DZ-11
 * (There is a different cell for each active DZ-11)
 *
 * The bits in the status cell are used as follows:
 *
 *	1<<0 - CSR check requested
 *	1<<1 - driver notification requested
 *	1<<2 - carrier-on change report requested
 *	1<<5 - error has occurred
 *	1<<7 - base-in command received
 */
status:	.byte	0
/*
 * Carrier-on status bits from last carrier-on status check
 */
costatus:	.byte	0
/*
 * Error counter for DZ
 */
errcnt:	.byte	0
/*
 * kmc copy of the transmit-enable bits in the DZ-11
 */
actline:	.byte	0
/*
 * kmc copy of the break bits in the dz
 */
brkbits:	.byte	0
/*
 * 16.7 msec software timer
 */
clock1:	.byte	0
/*
 * 12.8 msec software timer--used to schedule modem status checks
 */
clock2:	.byte	0
/*
 * Link to next DZ in report-needed queue
 */
rptlink:	.byte	0
/*
 * Link to next DZ in csr-check-needed queue
 */
csrlink:	.byte	0
/*
 * Link to next DZ in carrier-on-change-report-needed queue
 */
coclink:	.byte	0
/*
 * Number used by the driver to identify this DZ
 * (The low-order three bits select one of the four possible
 * DZ's for this KMC)
 */
dznr:	.byte	0
/*
 * Set up the table of addresses of the line-table entries
 *
 * Each byte holds the address of one line-table entry
 *
 * This table must be at location zero of the page for each DZ
 */
	.org	12*256
dzst:
	.byte	lte0,lte1,lte2,lte3,lte4,lte5,lte6,lte7
/*
 * Ditto for DZ 1
 */
	.org	13*256
	.byte	lte0,lte1,lte2,lte3,lte4,lte5,lte6,lte7
/*
 * Ditto for DZ 2
 */
	.org	14*256
	.byte	lte0,lte1,lte2,lte3,lte4,lte5,lte6,lte7
/*
 * Ditto for DZ 3
 */
	.org	15*256
	.byte	lte0,lte1,lte2,lte3,lte4,lte5,lte6,lte7
/*
 * Global data structures
 */
	.org	0
/*
 * Last command received from the host for this DZ
 */
cmdsave:  .byte	0,0,0,0,0,0,0,0
rptsave:  .byte	0,0,0,0,0,0,0,0
/*
 * Masks to set or clear the transmit-enable bit for each line
 */
enblbits:	.byte	1<<0,1<<1,1<<2,1<<3,1<<4,1<<5,1<<6,1<<7
return:	.byte	0,0
putret:	.byte	0,0
charsave:  .byte  0
/*
 * Input-ready queue
 */
headinrq:	.byte	NIL
tailinrq:	.byte	NIL
/*
 * Output-buffer-completion-report queue
 */
headoutq:	.byte	NIL
tailoutq:	.byte	NIL
/*
 * Interrupt-request queue
 */
headbrkq:	.byte	NIL
tailbrkq:	.byte	NIL
/*
 * CSR-check-needed queue
 */
headcsrq:	.byte	NIL
tailcsrq:	.byte	NIL
/*
 * Error-report-needed queue
 */
headerrq:	.byte	NIL
tailerrq:	.byte	NIL
/*
 * Carrier-on-change-report-needed queue
 */
headcocq:	.byte	NIL
tailcocq:	.byte	NIL
/*
 * Empty-clist-block queue
 */
headecbq:	.byte	NIL
tailecbq:	.byte	NIL
/*
 * Queue entries
 */
inqueue:
/*
 * Instruction text starts here
 */
	.text
/*
 * Segment 0
 *
 * This is the main segment
 */
	.org	0
seg0:
/*
 * KMC initialization sequence--keep this at location zero
 */
	CALL(init)
	mov	%dzst,brg
	add	brg,r8,%mar
	br	disp
/*
 * Tables of jump instructions
 *
 * The reason for putting the tables here is to avoid
 * splitting a table across a page boundary.
 */
dispsw:
	br	disp0
	br	disp1
	br	disp2
	br	disp3
	br	disp4
	ERROR(ERRC)
	br	disp
/*
 * Dispatcher loop--keep looking for something to do
 */
disp:
/*
 * If the 50-microsecond timer has expired then go to tick
 */
	mov	nprx,brg
	br4	1f
	br	2f
1:
	mov	%tick,brg
	mov	brg,pcr
	jmp	tick
2:
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
 * If the input-ready queue is nonempty
 * then go to rbufout
 */
	mov	CHKINRQ,brg
	orn	brg,r14,-
	brz	rbufout
/*
 * If the carrier-on-change-report queue is nonempty
 * then go to cochange
 */
	mov	CHKCOCQ,brg
	orn	brg,r14,-
	brz	cochange
/*
 * If the break-request queue is nonempty
 * then go to rbrkrpt
 */
	mov	CHKBRKQ,brg
	orn	brg,r14,-
	brz	rbrkrpt
/*
 * If the output-buffer-completion-report queue is nonempty
 * then go to xbufout
 */
	mov	CHKOUTQ,brg
	orn	brg,r14,-
	brz	xbufout
/*
 * If the error queue is nonempty then go to errout
 */
	mov	CHKERRQ,brg
	orn	brg,r14,-
	brz	errout

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
 * If the driver has set IEO then go to report2, else if the
 * driver has cleared RDYO then set dialog state = 0 and
 * go to disp, otherwise go to dispb
 */
	mov	csr0,brg
	br4	report2
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
 * and go to disp, otherwise go to dispb
 */
	mov	csr2,brg
	br7	dispb
	mov	nprx,brg
	br7	dispb
	mov	0,brg
	mov	brg,r13
	br	disp
dispb:
/*
 * If a CSR check is needed then go to csrcheck
 */
	mov	CHKCSRQ,brg
	orn	brg,r14,-
	brz	1f
	br	disp
1:
	mov	%csrcheck,brg
	mov	brg,pcr
	jmp	csrcheck
/*
 * Report an output buffer completion
 *
 * The format is:
 *
 * csr2 - report type
 * csr3 - line number and DZ identification number
 * csr4 - bits 7-0 of the output buffer address
 * csr5 - bits 15-8 of the output buffer address
 * csr6 - bits 17-16 of the output buffer address (right-adjusted)
 * csr7 - unused
 *
 */
xbufout:
/*
 * Get the address of the first queue entry on the 
 * output-buffer-completion-report-needed queue
 */
	mov	headoutq,mar
	mov	%headoutq,%mar
	mov	mem,r4
	brz	queuerr0
/*
 * Save the link to the next queue entry
 */
	mov	3,brg
	and	brg,r4,%mar
	mov	~3,brg
	and	brg,r4,mar
	mov	mem,r5|mar++
	mov	mem,csr4|mar++
	mov	mem,csr5|mar++
	mov	3,r3
	and	mem,r3,brg
	mov	brg,csr6
	mov	mem,brg
	mov	0,brg>>
	mov	0,brg>>
	mov	7,r7
	and	brg,r7
	mov	0,brg>>
	mov	0,brg>>
	mov	0,brg>>
	mov	3,r3
	and	brg,r3
	mov	%dzst,brg
	add	brg,r3,%mar
	mov	dznr,mar
	mov	mem,r0
	asl	r0
	asl	r0
	asl	r0,brg
	or	brg,r7,brg
	mov	brg,csr3
	mov	0,brg
	mov	brg,csr7
/*
 * Put this queue entry onto the front of the list of 
 * available queue entries
 */
	FREEQ(r4);
/*
 * Remove this queue entry from the output-buffer-completion-
 * report-needed queue
 */
	mov	headoutq,mar
	mov	%headoutq,%mar
	mov	r5,mem|mar++
/*
 * If the queue is empty then adjust the pointer to the end of
 * the queue and clear the CHKOUTQ bit in r14
 */
	brz	1f
	br	2f
1:
	mov	r5,mem
	mov	~CHKOUTQ,brg
	and	brg,r14
2:
/*
 * Put the report type in csr2
 */
	mov	XBUFOUT,brg
	mov	brg,csr2
/*
 * Go to reporte
 */
	br	reporte
/*
 * Report an error
 *
 * The format is:
 *
 * csr2 - report type
 * csr3 - dznr<<3|line nr
 * csr4 - error number
 * csr5-7 zero
 *
 */
errout:
/*
 * Get the address of the first queue entry on the  error queue
 */
	mov	headerrq,mar
	mov	%headerrq,%mar
	mov	mem,r4
	brz	errnon
/*
 * Save the link to the next queue entry
 */
	mov	3,brg
	and	brg,r4,%mar
	mov	~3,brg
	and	brg,r4,mar
/*
 * Get link to next entry
 */
	mov	mem,r5|mar++
/*
 * Set up csrs
 */
	mov	mem,csr3|mar++
	mov	mem,csr4|mar++
	mov	0,brg
	mov	brg,csr5
	mov	brg,csr6
	mov	brg,csr7
/*
 * Reset error bit in status for this dz
 */
	mov	mem,r8
	mov	%dzst,brg
	add	brg,r8,%mar
	mov	status,mar
	mov	mem,r0
	mov	~(1<<5),brg
	and	brg,r0,mem
/*
 * Put this queue entry onto the front of the list of 
 * available queue entries
 */
	FREEQ(r4)
/*
 * Remove this queue entry from error queue
 */
	mov	headerrq,mar
	mov	%headerrq,%mar
	mov	r5,mem|mar++
/*
 * If the queue is empty then adjust the pointer to the end of
 * the queue and clear the CHKERRQ bit in r14
 */
	brz	1f
	br	2f
1:
	mov	r5,mem
	mov	~CHKERRQ,brg
	and	brg,r14
2:
/*
 * Put the report type in csr2
 */
	mov	ERROUT,brg
	mov	brg,csr2
/*
 * Go to reporte
 */
	br	reporte
errnon:
	mov	~CHKERRQ,brg
	and	brg,r14
	br	dispb
/*
 * Report an input buffer completion
 *
 * The RBUF1OT format is:
 * csr2 - report type
 * csr3 - line number and DZ number
 * csr4 - first character
 * csr5 - second character
 * csr6 - third character
 * csr7 - number of characters
 *
 * The RBUFOUT format is:
 *
 * csr2 - report type
 * csr3 - line number and DZ identification number
 * csr4 - bits 7-0 of the clist buffer address
 * csr5 - bits 15-8 of the clist buffer address
 * csr6 - bits 17-16 of the clist buffer address (right-adjusted)
 * csr7 - unused
 *
 * This code is entered from the dispatcher loop when the
 * report window is open and the CHKINRQ
 * bit is set in r14.  At most one line is serviced on each pass
 * through the dispatcher loop.  If one or two characters are in the
 * input queue they are passed in through a csrs whithout using
 * a c-list via RBUF1OT.  Otherwise if an empty clist block is
 * available the clist block is loaded with data for
 * that line and the clist buffer address is passed
 * to the host via RBUFOUT
 */
rbufout:
/*
 * Get the pointer to the queue entry at the head of the input-
 * ready queue.  If the queue is empty then go to rbufoutf
 */
	mov	headinrq,mar
	mov	%headinrq,%mar
	mov	mem,r2
	brz	rbnomo
/*
 * Get the line number and the DZ number
 * from the queue entry at the head of the input-ready queue
 */
	mov	~03,brg
	and	brg,r2,brg
	mov	brg,r0
	inc	r0,mar
	mov	03,brg
	and	brg,r2,%mar
	mov	mem,r7|mar++
	mov	mem,r8
	mov	%dzst,brg
	add	brg,r8,%mar
	mov	r7,mar
	mov	mem,r10
/*
 * See if there is input for this line since it could
 * have been flushed after line was queued
 */
	mov	HEADINQ,brg
	add	brg,r10,mar
	mov	mem,brg
	brz	1f
	br	2f
1:
/*
 * No input for this line, remove from input ready queue
 */
	mov	FLAGS1,brg
	add	brg,r10,mar
	mov	mem,r0
	mov	~(TIMACT|LTEINQ),brg
	and	brg,r0,mem
	mov	03,brg
	and	brg,r2,%mar
	mov	~03,brg
	and	brg,r2,mar
	mov	mem,r1
	FREEQ(r2)
	mov	headinrq,mar
	mov	%headinrq,%mar
	mov	r1,mem|mar++
	brz	1f
/*
 * More input exists, go get next request
 */
	br	rbufout
/*
 * No more input, go to rbnomo
 */
1:
	mov	r1,mem
	br	rbnomo
2:
/*
 * See if there is one to three input characters
 */
	mov	3,brg
	mov	brg,r0
	mov	NICH,brg
	add	brg,r10,mar
	mov	mem,brg
	sub	brg,r0,-
	brc	rb1out
	br	rbcout
/*
 * One to three characters remain in the input queue.  They
 * are passed in through csrs without using a empty c-list.
 */
rb1out:
/*
 * Bookkeep the LTE
 */
	mov	0,mem
/*
 * Put command into csr2
 */
	mov	RBUF1OT,brg
	mov	brg,csr2
/*
 * Get input queue pointer
 */
	mov	HEADINQ,brg
	add	brg,r10,mar
	mov	mem,r6
	mov	NIL,mem|mar++
	mov	NIL,mem
/*
 * Get offset into input buffer
 */
	mov	RBCOPO,brg
	add	brg,r10,mar
	mov	mem,r4
	mov	0,mem
/*
 * Clear line in queue flag
 */
	mov	FLAGS1,brg
	add	brg,r10,mar
	mov	mem,r0
	mov	~(TIMACT|LTEINQ),brg
	and	brg,r0,mem
/*
 * Get dznr, line number into csr3
 */
	mov	dznr,mar
	mov	mem,r0
	asl	r0
	asl	r0
	asl	r0
	mov	07,brg
	and	brg,r7,brg
	or	brg,r0
	mov	r0,csr3
/*
 * Remove this line from the inrq
 */
	mov	%headinrq,%mar
	mov	headinrq,mar
	mov	mem,r0
	mov	03,brg
	and	brg,r0,%mar
	mov	~03,brg
	and	brg,r0,mar
	mov	mem,r1
	FREEQ(r0)
	mov	%headinrq,%mar
	mov	headinrq,mar
	mov	r1,mem|mar++
	brz	1f
	br	2f
/*
 * No more lines queued, turn off CHKINRQ
 */
1:
	mov	r1,mem
	mov	~CHKINRQ,brg
	and	brg,r14
2:
/*
 * Get pointer to data
 */
	mov	03,brg
	and	brg,r6,%mar
	mov	~03,brg
	and	brg,r6,mar
	mov	brg,brg|mar++
	mov	mem,r5|mar++
/*
 * Get character count
 */
	mov	mem,csr7|mar++
/*
 * Get characters into csrs
 */
	mov	017,brg
	and	brg,r5,%mar
	mov	~017,brg
	and	brg,r5,brg
	add	brg,r4,mar
	mov	mem,csr4|mar++
	mov	mem,csr5|mar++
	mov	mem,csr6
/*
 * Free up buffer/queue
 */
	FREEQ(r6)
	RELEASE(r5)
/*
 * Pass character(s) in
 */
	br	reporte

/*
 * Get the pointer to the queue entry for the clist block at the head
 * of the empty-clist-block queue.  If the queue is empty then
 * go to rbufoutg.
 */
rbcout:
	mov	headecbq,mar
	mov	%headecbq,%mar
	mov	mem,r6
	brz	rbnomo
/*
 * Save addr of c-list
 */
	mov	03,brg
	and	brg,r6,%mar
	mov	~03,brg
	and	brg,r6,mar
	mov	mem,r1
	mov	0,mem|mar++
	mov	mem,csr4|mar++
	mov	mem,csr5|mar++
	mov	mem,csr6
/*
 * Remove queue entry from list
 */
	mov	headecbq,mar
	mov	%headecbq,%mar
	mov	r1,mem|mar++
	brz	1f
	br	2f
1:
	mov	r1,mem
2:
/*
 * R6 now points to a queue entry with location 0 initially
 * containing a zero.  This location is a counter for the number
 * of characters in the c-list.  The next 3 locations contain the
 * physical address of the c-list.  All arithmetic will be done
 * in this queue entry.
 */

/*
 * Initialize the remainder of the csrs
 */
	mov	RBUFOUT,brg
	mov	brg,csr2
	mov	0,brg
	mov	brg,csr7
/*
 * Get the driver's idea of the DZ number and combine it with the line
 * number to get the minor device number
 */
	mov	%dzst,brg
	add	brg,r8,%mar
	mov	dznr,mar
	mov	mem,r0
	asl	r0
	asl	r0
	asl	r0
	mov	07,brg
	and	brg,r7,brg
	or	brg,r0
/*
 * Put the minor device number into csr3
 */
	mov	r0,csr3
/*
 * Csrs are initialized
 */

/*
 * Initialize the empty c-list to point to the first word of the data section
 */
	mov	03,brg
	and	brg,r6,%mar
	mov	~03,brg
	and	brg,r6,brg
	mov	brg,r0
	inc	r0,mar
	mov	C_DATA,brg
	mov	mem,r0
	add	brg,r0,mem|mar++
	mov	mem,r0
	adc	r0,mem|mar++
	mov	mem,r0
	adc	r0,mem


/*
 * Copy datea in unitl the c-list is full or the input data
 * is exhausted
 */

rbloop1:
/*
 * Get offset into first buffer, increment by 2
 */
	mov	%dzst,brg
	add	brg,r8,%mar
	mov	RBCOPO,brg
	add	brg,r10,mar
	mov	mem,r1
	mov	2,brg
	add	brg,r1,mem
/*
 * Increment the number of input characters
 */
	mov	NICH,brg
	add	brg,r10,mar
	mov	mem,r0
	mov	2,brg
	sub	brg,r0,mem
/*
 * Get address of input buffer
 */
	mov	HEADINQ,brg
	add	brg,r10,mar
	mov	mem,r5
	mov	03,brg
	and	brg,r5,%mar
	mov	~03,brg
	and	brg,r5,brg
	mov	brg,r0
	inc	r0,mar
/*
 * Get pointer to input buffer
 */
	mov	mem,r4|mar++
/*
 * Decrement number of characters in this buffer by 2
 */
	mov	mem,r0
	mov	2,brg
/*
 * Save number of characters input for later calculatoins
 */
	mov	brg,r2
/*
 * Perform subtraction, if number goes negative there was only
 * one remaining character in the input buffer
 */
	sub	brg,r0,mem
	brz	1f
	br	2f
1:
	mov	0,mem
	mov	1,brg
	mov	brg,r2
2:
/*
 * Calculate address of characters in buffer by adding offset
 */
	mov	017,brg
	and	brg,r4,%mar
	mov	~017,brg
	and	brg,r4,brg
	add	brg,r1,mar
/*
 * Get next 2 characters
 */
	mov	mem,odl|mar++
	mov	mem,odh|mar++
/*
 * Set up unibus address
 */
	mov	03,brg
	and	brg,r6,%mar
	mov	~03,brg
	and	brg,r6,brg
	mov	brg,r0
	inc	r0,mar
	mov	mem,oal|mar++
	mov	mem,oah|mar++
	mov	mem,r0
	mov	03,brg
	and	brg,r0
	asl	r0
	asl	r0
	mov	~(BRQ|ACLO|(3<<2)),brg
	mov	nprx,r1
	and	brg,r1,brg
	or	brg,r0,nprx
/*
 * Send in the clowns
 */
	mov	NRQ|OUT,brg
	mov	brg,npr
	BUSWAIT(buserr0)
/*
 * Decrement number of characters in c-list, r2 has
 * number from input buffer
 */
	mov	03,brg
	and	brg,r6,%mar
	mov	~03,brg
	and	brg,r6,mar
	add	mem,r2,mem|mar++
/*
 * Increment c-list address by 2
 */
	mov	2,brg
	mov	mem,r0
	add	brg,r0,mem|mar++
	mov	mem,r0
	adc	r0,mem|mar++
	mov	mem,r0
	adc	r0,mem
/*
 * See if the number of characters in the input buffer is zero
 */
	mov	03,brg
	and	brg,r5,%mar
	mov	~03,brg
	and	brg,r5,brg
	mov	brg,r0
	mov	2,brg
	add	brg,r0,mar
	mov	mem,r0|mar++
	dec	r0,-
	brc	rbtstcl
/*
 * Free the input buffer
 */
	RELEASE(r4)
/*
 * Release the queue entry
 */
	mov	03,brg
	and	brg,r5,%mar
	mov	~03,brg
	and	brg,r5,mar
	mov	mem,r1
	FREEQ(r5)
/*
 * Set next offset to 0
 */
	mov	%dzst,brg
	add	brg,r8,%mar
	mov	RBCOPO,brg
	add	brg,r10,mar
	mov	0,mem
/*
 * Remove from HEADINQ
 */

	mov	HEADINQ,brg
	add	brg,r10,mar
	mov	r1,mem|mar++
	brz	1f
/*
 * See if c-list is full
 */
	br	rbtstcl

/*
 * There are no more characters in this queue, bookkeep LTE
 */
1:
	mov	r1,mem
	mov	NICH,brg
	add	brg,r10,mar
	mov	0,mem
	mov	FLAGS1,brg
	add	brg,r10,mar
	mov	mem,r0
	mov	~(TIMACT|LTEINQ),brg
	and	brg,r0,mem
/*
 * Remove this line from the inrq
 */
	mov	%headinrq,%mar
	mov	headinrq,mar
	mov	mem,r0
	mov	03,brg
	and	brg,r0,%mar
	mov	~03,brg
	and	brg,r0,mar
	mov	mem,r1
	FREEQ(r0)
	mov	%headinrq,%mar
	mov	headinrq,mar
	mov	r1,mem|mar++
	brz	1f
	br	2f
/*
 * No more lines queued, turn off CHKINRQ
 */
1:
	mov	r1,mem
	mov	~CHKINRQ,brg
	and	brg,r14
2:
/*
 * Send it out
 */
	br	rbsendit


/*
 * See if the c-list is full
 */
rbtstcl:
	mov	03,brg
	and	brg,r6,%mar
	mov	~03,brg
	and	brg,r6,mar
	mov	mem,r0
	mov	CLSIZE,brg
	addn	brg,r0,-
	brz	rbsendit
/*
 * The c-list has more room for data
 */
	br	rbloop1

/*
 * Input data and/or c-list is full - send in c-list
 */

rbsendit:
/*
 * Get the number of characters in the c-list, put it
 * into C_NEXT, 0 in C_FIRST
 */
	mov	03,brg
	and	brg,r6,%mar
	mov	~03,brg
	and	brg,r6,mar
	mov	mem,odh
	FREEQ(r6)
	mov	0,brg
	mov	brg,odl
/*
 * Get the addr of the c-list, point to C_FIRST
 */
	mov	csr4,r1
	mov	csr5,r2
	mov	csr6,r0
	mov	C_FIRST,brg
	add	brg,r1,brg
	mov	brg,oal
	adc	r2,brg
	mov	brg,oah
	mov	03,brg
	and	brg,r0
	asl	r0
	asl	r0
	mov	~(BRQ|ACLO|(3<<2)),brg
	mov	nprx,r1
	and	brg,r1,brg
	or	brg,r0,nprx
/*
 * Send it in
 */
	mov	NRQ|OUT,brg
	mov	brg,npr
	BUSWAIT(buserr0)
	br	reporte
/*
 * No more data and/or no more empty c-lists, turn off CHKINRQ
 * (note; CHKINRQ will get set the next time a c-list is returned from host)
 */
rbnomo:
	mov	~CHKINRQ,brg
	and	brg,r14
	br	disp

/*
 * Fini
 */
/*
 * Report a break request (interrupt) (FS or DEL/break)
 *
 * The format is:
 *
 * csr2 - report type
 * csr3 - line number and DZ identification number
 */
rbrkrpt:
/*
 * Get the address of the first queue entry on the 
 * break-request-report-needed queue
 */
	mov	headbrkq,mar
	mov	%headbrkq,%mar
	mov	mem,r4
	brz	queuerr0
/*
 * Save the link to the next queue entry
 */
	mov	3,brg
	and	brg,r4,%mar
	mov	~3,brg
	and	brg,r4,mar
	mov	mem,r5|mar++
	mov	mem,brg|mar++
	mov	mem,csr4
	mov	7,r7
	and	brg,r7
	mov	0,brg>>
	mov	0,brg>>
	mov	0,brg>>
	mov	3,r3
	and	brg,r3
	mov	%dzst,brg
	add	brg,r3,%mar
	mov	dznr,mar
	mov	mem,r0
	asl	r0
	asl	r0
	asl	r0,brg
	or	brg,r7,brg
	mov	brg,csr3
	mov	0,brg
	mov	brg,csr5
	mov	brg,csr6
	mov	brg,csr7
/*
 * Put this queue entry onto the front of the list of 
 * available queue entries
 */
	FREEQ(r4);
/*
 * Remove this queue entry from the break-report-
 * report-needed queue
 */
	mov	headbrkq,mar
	mov	%headbrkq,%mar
	mov	r5,mem|mar++
/*
 * If the queue is empty then adjust the pointer to the end of
 * the queue and clear the CHKBRKQ bit in r14
 */
	brz	1f
	br	2f
1:
	mov	r5,mem
	mov	~CHKBRKQ,brg
	and	brg,r14
2:
/*
 * Put the report type in csr2
 */
	mov	RBRK,brg
	mov	brg,csr2
/*
 * Go to reporte
 */
	br	reporte
/*
 * Report a carrier-on status change
 */
cochange:
/*
 * Get the address of the first queue entry on the 
 * carrier-on-change-report-needed queue
 */
	mov	headcocq,mar
	mov	%headcocq,%mar
	mov	mem,r8
	brz	1f
	br	2f
1:
	mov	~CHKCOCQ,brg
	and	brg,r14
	br	disp
/*
 * Remove the page for this DZ from the coc-report-needed queue
 */
2:
	mov	coclink,mar
	mov	mem,brg
	mov	%headcocq,%mar
	mov	headcocq,mar
	mov	brg,mem|mar++
	brz	1f
	br	2f
1:
	mov	brg,mem
	mov	~CHKCOCQ,brg
	and	brg,r14
2:
/*
 * Clear the bit which indicates that a carrier-on
 * status-change report has been scheduled for this DZ
 */
	mov	%dzst,brg
	add	brg,r8,%mar
	mov	status,mar
	mov	~(1<<0),brg
	mov	brg,r0
	and	mem,r0,mem
/*
 * Put the carrier-on status flags in csr4
 */
	mov	costatus,mar
	mov	mem,csr4
/*
 * Put the DZ identification in csr3
 */
	mov	dznr,mar
	mov	mem,csr3
/*
 * Put the report type in csr2
 */
	mov	COCHG,brg
	mov	brg,csr2
/*
 * Go to reporte
 */
	br	reporte
/*
 * Wrap-up of report processing
 */
reporte:
/*
 * Save command sent to host
 */
	mov	rptsave,mar
	mov	%rptsave,%mar
	mov	csr0,mem|mar++
	mov	csr1,mem|mar++
	mov	csr2,mem|mar++
	mov	csr3,mem|mar++
	mov	csr4,mem|mar++
	mov	csr5,mem|mar++
	mov	csr6,mem|mar++
	mov	csr7,mem
/*
 * Set RDYO
 */
	mov	csr2,r0
	mov	RDYO,brg
	or	brg,r0,csr2
/*
 * If IEO is set then go to report2
 */
	mov	csr0,brg
	br4	report2
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
report2:
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
 * Extract DZ number and set page register = DZ number.
 */
	mov	csr3,brg
	mov	0,brg>>
	mov	0,brg>>
	mov	0,brg>>
	mov	3,r3
	and	brg,r3,brg
	mov	brg,r8
	mov	%dzst,brg
	add	brg,r8,%mar
/*
 * Get the command type from csr2
 */
	mov	csr2,r0
	mov	7,brg
	and	brg,r0
/*
 * If command type == 0 then go to xbufin
 * else if command type == 1 then go to ioctl
 * else if command type == 2 then go to basein
 * else if command type == 3 then go to rbufin
 * else if command type == 4 then go to cmdin
 * else go to disp
 */
	dec	r0
	brz	xbufin
	dec	r0
	brz	ioctl
	dec	r0
	brz	basein
	dec	r0
	brz	rbufin
	dec	r0
	brz	cmdin
	br	disp
/*
 * Process a BASEIN command
 *
 * The format of a BASEIN command is:
 *
 *	csr3 - DZ number<<3
 *	csr4 - low byte of DZ-11 unibus address
 *	csr5 - high byte of DZ-11 unibus address
 *	csr6 - bits 17-16 of DZ-11 unibus address
 */
basein:
/*
 * If this DZ is already active then go to basinerr
 * otherwise mark this DZ as active.
 */
	mov	status,mar
	mov	mem,brg
	br7	basinerr
	mov	1<<7,mem
/*
 * Record the DZ number
 */
	mov	csr3,brg
	mov	0,brg>>
	mov	0,brg>>
	mov	0,brg>>
	mov	7,r7
	mov	dznr,mar
	and	brg,r7,mem
/*
 * Record the unibus address for this DZ-11
 */
	mov	csrget,mar
	mov	csr4,mem|mar++
	mov	csr5,mem|mar++
	mov	csr6,mem
	mov	3,r3
	and	brg,r3,mem
	mov	3,mem	/* *** temp for testing *** */
/*
 * Go to disp
 */
	br	disp
/*
 * Process a buffer-send command
 *
 * The format of a buffer-send command is:
 *
 *	csr3 - ((DZ number)<<3)|(line number)
 *	csr4 - low byte of clist buffer address
 *	csr5 - high byte of clist buffer address
 *	csr6 - bits 17-16 of the clist buffer address (right-adjusted)
 *	csr7 - unused
 */
xbufin:
/*
 * If a base-in command has not been received yet for this DZ
 * then report error
 */
	mov	status,mar
	mov	mem,brg
	br7	1f
	ERROR(ERRE)
	br	disp
1:
/*
 * Extract the line number and look up the address of the
 * line-table entry
 */
	mov	7,r7
	mov	csr3,brg
	and	brg,r7,r7|mar
	mov	mem,r10|mar
/*
 * Get an unused queue entry
 */
	GETQ(r4,queuerr0)
/*
 * Copy the buffer address into the queue entry
 */
	mov	csr3,mem|mar++
	mov	csr4,mem|mar++
	mov	csr5,mem|mar++
	mov	csr6,mem
/*
 * Pack last byte to be:
 *			bits 6-5 = dz relative to this kmc
 *			bits 4-2 = line number (bits 2-0 in csr3)
 *			bits 1-0 = bits 17-16 of address
 */
	mov	3,r3
	and	mem,r3,mem
	mov	r8,brg
	mov	brg,r0
	asl	r0
	asl	r0
	asl	r0
	mov	r7,brg
	or	brg,r0
	asl	r0
	asl	r0
	or	mem,r0,mem
/*
 * Restore the page register
 */
	mov	%dzst,brg
	add	brg,r8,%mar
/*
 * Put a pointer to the queue entry into the LTE
 */
	mov	OUTBUF,brg
	add	brg,r10,mar
	mov	mem,-
	brz	1f
	ERROR(ERRF)
	br	disp
1:
	mov	r4,mem
/*
 * If the line is already active (state != 0) then go to disp
 */
	mov	STATE,brg
	add	brg,r10,mar
	mov	mem,r0
	dec	r0,-
	brc	disp
/*
 * Set state = 1
 */
	mov	1,mem
/*
 * Turn on the enable bit for this line
 */
	CALL(enable)
/*
 * Go to disp
 */
	br	disp
/*
 * Process a receive-buffer-in command
 *
 * The format of a receive-buffer-in command is:
 *
 *	csr3 - unused
 *	csr4 - low byte of clist buffer address
 *	csr5 - high byte of clist buffer address
 *	csr6 - bits 17-16 of the clist buffer address (right-adjusted)
 *	csr7 - unused
 */
rbufin:
/*
 * Get an unused queue entry
 */
	GETQ(r4,queuerr0)
/*
 * Indicate no successor
 */
	mov	NIL,mem|mar++
/*
 * Copy the buffer address into the queue entry
 */
	mov	csr4,mem|mar++
	mov	csr5,mem|mar++
	mov	csr6,mem
/*
 * Attach the queue entry to the end of the queue of empty
 * clist blocks
 */
	mov	tailecbq,mar
	mov	%tailecbq,%mar
	mov	mem,r3
	brz	1f
	mov	r4,mem
	mov	3,brg
	and	brg,r3,%mar
	mov	~03,brg
	and	brg,r3,mar
	br	2f
1:
	mov	headecbq,mar
	mov	r4,mem|mar++
2:
	mov	r4,mem
/*
 * If the input-ready queue is non-empty then set the bit which causes
 * that queue to be checked
 */
	mov	headinrq,mar
	mov	%headinrq,%mar
	mov	mem,-
	brz	1f
	mov	CHKINRQ,brg
	or	brg,r14
1:
/*
 * Go to disp
 */
	br	disp
/*
 * Process an ioctl command
 *
 * The format of an ioctl command is:
 *
 *	csr3 - ((DZ number)<<3)|(line number)
 *	csr4 - new value for IFLAG1
 *	csr5 - new values for IFLAG2
 *	csr6 - new values for OFLAG
 *	csr7 - new values of DELAYS
 *
 */
ioctl:
/*
 * If a base-in command has not been received yet for this DZ
 * then report error
 */
	mov	status,mar
	mov	mem,brg
	br7	1f
	ERROR(ERRA)
	br	disp
1:
/*
 * Extract the line number and look up the address of the
 * line-table entry
 */
	mov	7,r7
	mov	csr3,brg
	and	brg,r7,r7|mar
	mov	mem,r10|mar
/*
 * Copy the stty flags to the lte (four bytes)
 */
	mov	IFLAG1,brg
	add	brg,r10,mar
	mov	csr4,mem|mar++
	mov	csr5,mem|mar++
	mov	csr6,mem|mar++
	mov	csr7,mem
/*
 * Go to disp
 */
	br	disp
/*
 * Process an cmdin command
 *
 * The format of an cmdin command is:
 *
 *	csr3 - ((DZ number)<<3)|(line number)
 *	csr4 - command byte as described below
 *	csr5 - unused
 *	csr6 - character to be transmitted
 *	csr7 - unused
 *
 */

/*
 * The following bits describe the commands
 */
#define IFLUSH	(1<<0)	/* Flush input */
#define OFLUSH	(1<<1)	/* Flush output */
#define OSPND	(1<<2)	/* Suspend output */
#define ORSME	(1<<3)	/* Resume output */
#define SCHAR	(1<<4)	/* Send character in csr6 */
#define SETTMR	(1<<5)	/* Set ITIME to value in csr7 */
#define SBRK	(1<<6)	/* Send break */

cmdin:
/*
 * If a base-in command has not been received yet for this DZ
 * then report error
 */
	mov	status,mar
	mov	mem,brg
	br7	1f
	ERROR(ERRA)
	br	disp
1:
/*
 * Extract the line number and look up the address of the
 * line-table entry
 */
	mov	7,r7
	mov	csr3,brg
	and	brg,r7,r7|mar
	mov	mem,r10|mar
	mov	csr4,r4
/*
 * Process commands
 */
	mov	IFLUSH,brg
	orn	brg,r4,-
	brz	1f
	br	2f
1:
	CALL(iflush)
	mov	%dzst,brg
	add	brg,r8,%mar
	mov	csr4,r4
2:
	mov	OFLUSH,brg
	orn	brg,r4,-
	brz	1f
	br	2f
1:
	CALL(oflush)
	CALL(ubreak)
	mov	%dzst,brg
	add	brg,r8,%mar
	mov	csr4,r4
2:
/*
 * Setup for next two commands
 */
	mov	FLAGS1,brg
	add	brg,r10,mar
	mov	mem,r0
/*
 * See if suspend
 */
	mov	OSPND,brg
	orn	brg,r4,-
	brz	1f
	br	2f
1:
	mov	TTSTOP,brg
	or	brg,r0,mem
2:
	mov	ORSME,brg
	orn	brg,r4,-
	brz	1f
	br	2f
1:
/*
 * See if line was stopped
 */
	mov	~TTSTOP,brg
	or	brg,r0,-
	brz	1f
	br	2f
/*
 * Restart line
 */
1:
	and	brg,r0,mem
	CALL(enable)
	mov	%dzst,brg
	add	brg,r8,%mar
	mov	csr4,r4
2:
/*
 * See if character is to be sent
 */
	mov	SCHAR,brg
	orn	brg,r4,-
	brz	1f
	br	2f
1:
/*
 * Put character into LTE and set SNDEXP
 */
	mov	EXPCHR,brg
	add	brg,r10,mar
	mov	csr6,mem
	mov	FLAGS1,brg
	add	brg,r10,mar
	mov	mem,r0
	mov	SNDEXP,brg
	or	brg,r0,mem
	CALL(enable)
	mov	%dzst,brg
	add	brg,r8,%mar
	mov	csr4,r4
2:
/*
 * See if ITIME is to be loaded
 */
	mov	SETTMR,brg
	orn	brg,r4,-
	brz	1f
	br	2f
1:
	mov	INITIM,brg
	add	brg,r10,mar
	mov	csr7,mem
2:
/*
 * See if break is to be sent
 */
	mov	SBRK,brg
	orn	brg,r4,-
	brz	1f
	br	2f
1:
/*
 * Send a break
 */
	CALL(break)
2:
/*
 * Return to dispatcher
 */
	br	disp
/*
 * The unibus transfer failed to complete within a reasonable time
 */
buserr0:
/*
 * Clear the non-existent memory bit
 */
	mov	nprx,r0
	mov	~(BRQ|ACLO|NEM),brg
	and	brg,r0,nprx
	ERROR(BUSERR0)
	br	disp
basinerr:
	ERROR(BASEINERR)
	br	disp
/*
 * No queue entries available
 */
queuerr0:
	ERROR(QUEUERR0)
	HALT
/*
 * End of segment zero
 */
endseg0:
/*
 * Pick up code for other segments
 */
#include	"output.s"
#include	"input.s"
#include	"subs.s"
