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
 * Permanently assigned registers:
 *
 *	r15 - 100-microsecond timer/DZ-11 commutator
 *	r14 - pointer to head of report-needed queue
 *	r13 - pointer to end of report-needed queue
 *	r12 - pointer to head of csr-check-needed queue
 *	r11 - pointer to end of csr-check-needed queue
 *	r10 - address of current line-table entry
 *	r9 - current state of the dialog with the PDP-11
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
 *   8 - count for time delay, etc.
 *
 *   9 - icol
 *
 *   10 - character count for new output buffer
 *
 *   11 - sflags (single-bit stty options)
 *
 *   12 - NL delay algorithm
 *
 *   13 - HT delay algorithm
 *
 *   14 - CR delay algorithm
 *
 *   15 - (bsdelay<<1)|(vtdelay)
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
 * Note:  Must have STATE == 0
 *
 * Also, the fields beginning with OFLAG must remain in 
 * the order given
 */
#define	STATE	0
#define	NCH	1
#define	HIBYTE	2
#define	SAVECHAR	3
#define	BUFAD	4
#define	FLAGS	7
#define	FLAGS1	8
#define	COUNT	9
#define	ICOL	10
#define	NCHNEW	11
#define	OFLAG	12
#define DELAYS	13
#define EXPCHR	14
#define UNUSED	15
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
 * Define the "buswait" macro
 */
#define buswait 1:mov npr,brg;br0 1b;mov nprx,brg;br0 buserr;
/*
 * Subroutine CALL and RETURN macros
 *
 * Nested calls are not supported
 * R0 is destroyed by teh return instructions
 */
#define	CALL(X)\
	mov	%return,%mar;\
	mov	return,mar;\
	mov	%0f,mem|mar++;\
	mov	0f,mem;\
	br	X;\
0:

#define	RETURN		br	ret;

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
 * Data definitions
 */
	.data
/*
 * Save room for the line-table entry address table
 * and the enable bits
 */
	.org	.+16
/*
 * Define the macro for generating line-table entries
 */
#define LTENT  .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
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
 * temp for debugging
 *
 * last command received from the host computer for this DZ
 */
cmdsave:  .byte	0,0,0,0,0,0,0,0
rptsave:  .byte	0,0,0,0,0,0,0,0
/*
 * Byte strings for setting up unibus transfers
 *
 * Each string contains one unibus address
 *
 * Bits 7-0 are in the first byte, bits 15-8 are in the second byte,
 * and bits 17-16 are in the third byte
 *
 * String used to set up fetch for CSR
 */
csrget:	.byte	0,0,0
/*
 * String used to put one byte into TBUF
 */
tdrput:	.byte	0,0,0,0
/*
 * String used to set or clear the transmit-enable bits
 */
tcrput:	.byte	0,0,0,0
/*
 * Cell used to save status info for each active DZ-11
 * (There is a different cell for each active DZ-11)
 */
status:	.byte	0
/*
 * The bits in the status cell are used as follows:
 *
 *	1<<0 - CSR check requested
 *	1<<1 - driver notification requested
 *	1<<5 - unibus error has occurred
 *	1<<7 - base-in command received
 */
/*
 * Cell to record the DZ number for this page
 */
dznr:	.byte	0
/*
 * Cell to record output-buffer completions (one bit for each line)
 *
 * The bit is set when the last word of an output buffer is fetched
 * from the host computer.  The entire cell is cleared when the bits have
 * been passed to the driver
 */
buffcomp:	.byte	0
/*
 * KMC copy of the transmit-enable bits in the DZ-11
 */
actline:	.byte	0
/*
 * Current output character (for debugging)
 */
charsave:  .byte	0
/*
 * 16.7 msec software timer
 */
clock1:	.byte	0
/*
 * Link to next DZ in report-needed queue
 */
rptlink:	.byte	0
/*
 * Link to next DZ in csr-check-needed queue
 */
csrlink:	.byte	0
/*
 * The following variables are only used in page zero
 */

/*
 * Return address of the CALL/RETURN marcors
 */
return:	.byte	0,0
/*
 * Set up the table of addresses of the line-table entries
 *
 * Each byte holds the address of one line-table entry
 *
 * This table must be at location zero of each page
 */
	.org	0
	.byte	lte0
	.byte	lte1
	.byte	lte2
	.byte	lte3
	.byte	lte4
	.byte	lte5
	.byte	lte6
	.byte	lte7
/*
 * Masks to set or clear the transmit-enable bit for each line
 *
 * Note:  The enable bits must immediately follow the lte address list
 */
enblbits:	.byte	1<<0,1<<1,1<<2,1<<3,1<<4,1<<5,1<<6,1<<7
/*
 * Ditto for page 1
 */
	.org	256
	.byte	lte0
	.byte	lte1
	.byte	lte2
	.byte	lte3
	.byte	lte4
	.byte	lte5
	.byte	lte6
	.byte	lte7
	.byte	1<<0,1<<1,1<<2,1<<3,1<<4,1<<5,1<<6,1<<7
/*
 * Ditto for page 2
 */
	.org	512
	.byte	lte0
	.byte	lte1
	.byte	lte2
	.byte	lte3
	.byte	lte4
	.byte	lte5
	.byte	lte6
	.byte	lte7
	.byte	1<<0,1<<1,1<<2,1<<3,1<<4,1<<5,1<<6,1<<7
/*
 * Ditto for page 3
 */
	.org	768
	.byte	lte0
	.byte	lte1
	.byte	lte2
	.byte	lte3
	.byte	lte4
	.byte	lte5
	.byte	lte6
	.byte	lte7
	.byte	1<<0,1<<1,1<<2,1<<3,1<<4,1<<5,1<<6,1<<7
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
statesw:
	br	disable
	br	odrst1
	br	odrst2
	br	odrst3
	br	odrst4
	br	odrst5
	br	e1
/*
 * Code to do a subroutine return
 */
ret:
	mov	%return,%mar
	mov	return,mar
	mov	mem,r0|mar++
	mov	0f,brg
	br	(add, brg, r0), %0f
0:
	br	(mem),p0
	br	(mem),p1
	br	(mem),p2
	br	(mem),p3

/*
 * The following table of jump instructions is accessed by a
 * calculated branch instruction using the ASCII character
 * as index.  It provides a fast implementation of tests
 * for specific control characters and for characters which
 * require special handling for upper-case only terminals.
 */
charsw:
	br	sendit		/* 0 - NUL */
	br	sendit		/* 1 - SOH */
	br	sendit		/* 2 - STX */
	br	sendit		/* 3 - ETX */
	br	odrst1		/* 4 - EOT */
	br	sendit		/* 5 - ENQ */
	br	sendit		/* 6 - ACK */
	br	sendit		/* 7 - BEL */
	br	isbs		/* 010 - BS */
	br	isht		/* 011 - HT */
	br	isnl		/* 012 - NL */
	br	isvt		/* 013 - VT */
	br	isff		/* 014 - FF */
	br	iscr		/* 015 - CR */
	br	sendit		/* 016 - SO */
	br	sendit		/* 017 - SI */
	br	sendit		/* 020 - DLE */
	br	sendit		/* 021 - DC1 */
	br	sendit		/* 022 - DC2 */
	br	sendit		/* 023 - DC3 */
	br	sendit		/* 024 - DC4 */
	br	sendit		/* 025 - NAK */
	br	sendit		/* 026 - SYN */
	br	sendit		/* 027 - ETB */
	br	sendit		/* 030 - CAN */
	br	sendit		/* 031 - EM */
	br	sendit		/* 032 - SUB */
	br	sendit		/* 033 - ESC */
	br	sendit		/* 034 - FS */
	br	sendit		/* 035 - GS */
	br	sendit		/* 036 - RS */
	br	sendit		/* 037 - US */
/*
 * Initialize the KMC
 */
init:
	mov	0,brg
	mov	brg,r9
	mov	brg,csr2
	mov	IEO,brg
	mov	brg,csr0
	mov	NIL,brg
	mov	brg,r14
	mov	brg,r13
	mov	brg,r12
	mov	brg,r11
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
 * Get OFLAG into r6
 */
	mov	OFLAG,brg
	add	brg,r10,mar
	mov	mem,r6
/*
 * Go to disp0, disp1, disp2, disp3, or disp4 depending on the
 * current state of the dialog with the PDP-11
 */
	mov	dispsw,brg
	br	(add,brg,r9),%dispsw
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
	mov	r14,-
	brz	1f
	br	report
1:
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
	mov	brg,r9
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
	mov	brg,r9
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
	mov	brg,r9
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
	mov	brg,r9
	br	disp
/*
 * If a CSR check is needed then go to csrcheck
 * else go to disp
 */
dispb:
	mov	r12,-
	brz	disp
	br	csrcheck
/*
 * The KMC has something to report to the driver
 *
 * (We get here from state 0 when the report-needed queue
 * is non-empty.)
 */
report:
/*
 * Set page register = DZ number.
 */
	mov	r14,brg
	mov	brg,r8|%mar
/*
 * Remove this DZ page from the head of the report-needed queue.
 */
	mov	rptlink,mar
	mov	mem,r14
	brz	1f
	br	2f
1:
	mov	mem,r13
2:
/*
 * Copy the buffer-completion bits to the interface
 * and clear the bits in the KMC.
 */
	mov	buffcomp,mar
	mov	mem,csr4
	mov	0,mem
/*
 * Put the DZ number in csr3
 */
	mov	dznr,mar
	mov	mem,csr3
/*
 * Copy the status bits to csr7 and clear the report-needed
 * bit and the unibus-error bit in the KMC
 */
	mov	status,mar
	mov	mem,csr7
	mov	~((1<<1)|(1<<5)),brg
	mov	brg,r0
	and	mem,r0,mem
/*
 * Save the report
 */
	mov	rptsave,mar
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
	mov	brg,r9
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
	mov	brg,r9
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
	mov	brg,r9
/*
 * Set page register = low-order two bits of DZ number
 *
 *
 */
	mov	csr3,r0
	mov	3<<3,brg
	and	brg,r0,brg
	mov	brg,brg>>
	mov	brg,brg>>
	mov	brg,brg>>
	mov	brg,r8|%mar
/*
 * Save command info for debugging
 */
	mov	cmdsave,mar
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
 * If command type == 0 then go to buffsend
 * else if command type == 1 then go to ioctl
 * else if command type == 2 then go to basein
 * esle if command type == 3 then go to cmdin
 * else go to disp
 */
	dec	r0
	brz	buffsend
	dec	r0
	brz	ioctl
	dec	r0
	brz	basein
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
 *
 * Note: Eventually we may need to pass bits 17-16 of the
 * unibus address.  For now we assume they are always ones.
 */
basein:
/*
 * If this DZ is already active then ignore the base-in
 * command; otherwise mark this DZ as active.
 */
	mov	status,mar
	mov	mem,brg
	br7	disp
	mov	1<<7,mem
/*
 * Record the DZ number
 */
	mov	csr3,r0
	mov	7<<3,brg
	and	brg,r0,brg
	mov	brg,brg>>
	mov	brg,brg>>
	mov	brg,brg>>
	mov	dznr,mar
	mov	brg,mem
/*
 * Record the unibus address for this DZ-11
 */
	mov	csrget,mar
	mov	csr4,mem
	mov	mem,r0|mar++
	mov	csr5,mem
	mov	mem,r1|mar++
	mov	(3<<2)|NRQ,mem
/*
 * Calculate and save the TDR address
 */
	mov	tdrput,mar
	mov	6,brg
	add	brg,r0,mem|mar++
	adc	r1,mem|mar++
	mov	3<<2,mem|mar++
	mov	BYTE|OUT|NRQ,mem
/*
/*
 * Calculate and save the TCR address
 */
	mov	tcrput,mar
	mov	4,brg
	add	brg,r0,mem|mar++
	adc	r1,mem|mar++
	mov	3<<2,mem|mar++
	mov	BYTE|OUT|NRQ,mem
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
 *	csr4 - low byte of buffer address
 *	csr5 - high byte of buffer address
 *	csr6 - character count
 *	csr7 - bits 17-16 of buffer address (left-adjusted)
 */
buffsend:
/*
 * If a base-in command has not been received yet for this DZ
 * then go to disp
 */
	mov	status,mar
	mov	mem,brg
	br7	1f
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
 * Move the character count to the line-table entry
 */
	mov	NCHNEW,brg
	add	brg,r10,mar
	mov	csr6,mem|mar++
/*
 * Copy the buffer address to the line-table entry
 */
	mov	BUFAD,brg
	add	brg,r10,mar
	mov	csr4,mem|mar++
	mov	csr5,mem|mar++
	mov	csr7,brg
	mov	0,brg>>
	mov	0,brg>>
	mov	0,brg>>
	mov	0,brg>>
	mov	0,brg>>
	mov	0,brg>>
	mov	brg,mem
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
 * Enable the line
 */
	CALL(enable)
	mov	r8,%mar
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
 *	csr4 - new value for OFLAG
 *	csr5 - new values for DELAYS
 *
 */
ioctl:
/*
 * If a base-in command has not been received yet for this DZ
 * then go to disp
 */
	mov	status,mar
	mov	mem,brg
	br7	1f
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
 * Copy the stty flags to the lte (two bytes)
 */
	mov	OFLAG,brg
	add	brg,r10,mar
	mov	csr4,mem|mar++
	mov	csr5,mem
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
#define OFLUSH	(1<<1)	/* Flush output */
#define OSPND	(1<<2)	/* Suspend output */
#define ORSME	(1<<3)	/* Resume output */
#define SCHAR	(1<<4)	/* Send character in csr6 */

cmdin:
/*
 * If a base-in command has not been received yet for this DZ
 * then go to disp
 */
	mov	status,mar
	mov	mem,brg
	br7	1f
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
	mov	OFLUSH,brg
	orn	brg,r4,-
	brz	1f
	br	2f
1:
	CALL(oflush)
	CALL(oqueue)
	mov	r8,%mar
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
	mov	r8,%mar
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
	mov	r8,%mar
	mov	csr4,r4
2:
/*
 * Return to dispatcher
 */
	br	disp
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
 * The effect of the following code is to schedule a
 * csr check for a different DZ every 100 microseconds.
 * Since there are four possible DZ's, each DZ gets scheduled
 * for a csr check every 400 microseconds.  This is approximately
 * a factor of two better than we need for 9600 baud (one character
 * every 1042 microseconds).
 */
/*
 * Increment r15.  If the result is odd then go to disp.
 */
	inc	r15,r15|brg
	br0	disp
/*
 * Set the page register from bits 2-1 of r15.
 */
	mov	3<<1,brg
	and	brg,r15,brg
	mov	brg,brg>>
	mov	brg,r8|%mar
/*
 * If a base-in command has not been received yet for this DZ
 * then go to disp
 */
	mov	status,mar
	mov	mem,brg
	br7	1f
	br	disp
1:
/*
 * If a csr check has already been requested for this DZ
 * then go to delaycheck
 */
	br0	delaycheck
/*
 * Set a flag to indicate that a csr check has been requested
 */
	mov	1<<0,r1
	or	mem,r1,mem
/*
 * Insert the page for the current DZ at the end of the
 * csr-check-needed queue
 */
	mov	csrlink,mar
	mov	NIL,mem
	mov	r11,brg
	brz	1f
	mov	brg,%mar
	mov	r8,mem
	mov	r8,brg|%mar
	mov	brg,r11
	br	2f
1:
	mov	r8,brg
	mov	brg,r11
	mov	brg,r12
2:
delaycheck:
/*
 * Decrement the 16.7 msec timer.  If the result is 
 * non-negative then go to disp
 */
	mov	r8,%mar
	mov	clock1,mar
	mov	mem,r0
	dec	r0,mem
	brc	disp
/*
 * Reset the 16.7 msec timer
 */
	mov	35,mem
/*
 * The 16.7 msec timer has expired.  Therefore we need to decrement
 * the count field for each lte which is currently in the twait
 * state (delaying)
 */
/*
 * Initialize registers for loop which follows
 */
	mov	5,r5
	mov	7,r7|mar
/*
 * The following loop is executed once for each lte
 * in the page for the selected DZ.
 *
 * If we find a line which is in the twait state
 * then we decrement the count field of the lte.
 * If the result is negative then we put the line
 * back into state 1 and enable the line.
 */
delaych1:
/*
 * Get the address of the lte
 *
 * If state == 5 (twait) then go to delaych3
 *
 * Note that the following code assumes that STATE is the
 * first cell in the lte and that the memory-address
 * register (mar) has already been loaded from r7
 */
	mov	mem,r10|mar
	addn	mem,r5,-
	brz	delaych3
/*
 * Test for end of loop
 */
delaych2:
/*
 * Decrement the current entry number.  If the result is
 * non-negative then go to delaych1
 */
	dec	r7,r7|mar
	brc	delaych1
/*
 * Go to disp
 */
	br	disp
/*
 * We get here when we find a line with STATE == 5 (twait)
 */
delaych3:
/*
 * Decrement the count field of the lte.  If the result is
 * non-negative then go to delaych2
 */
	mov	COUNT,brg
	add	brg,r10,mar
	mov	mem,r0
	dec	r0,mem
	brc	delaych2
/*
 * The lte pointed to by r7 and r10 has completed the specified
 * delay.  Therefore we must remove it from the twait state and
 * put it back into state 1 (sending normal data).
 */
/*
 * Set state = 1 (sending normal data)
 */
	mov	STATE,brg
	add	brg,r10,mar
	mov	1,mem
/*
 * Enable the line
 */
	CALL(enable)
	mov	r8,%mar
/*
 * Go to delaych2
 */
	br	delaych2
/*
 * The following code checks for output character completions
 */
csrcheck:
/*
 * Set page register = page number of DZ at top of queue
 */
	mov	r12,brg
	mov	brg,r8|%mar
/*
 * Set up a unibus request for the current value of CSR
 */
	mov	csrget,mar
	mov	mem,ial|mar++
	mov	mem,iah|mar++
	mov	mem,npr
/*
 * Wait for the unibus request to complete.
 * If an error occurs then go to buserr.
 */
	buswait
/*
 * If there is an output data request then go to csrchk2
 */
	mov	idh,brg
	br7	csrchk2
/*
 * Clear the flag which indicates that a CSR check is needed
 */
	mov	status,mar
	mov	~(1<<0),brg
	mov	brg,r0
	and	mem,r0,mem
/*
 * Remove the current DZ from the csr-check-needed queue
 */
	mov	csrlink,mar
	mov	mem,r12
	brz	1f
	br	2f
1:
	mov	mem,r11
2:
/*
 * Go to disp
 */
	br	disp
/*
 * The DZ-11 has requested an output character.  Get the line
 * number and look up the address of the line-table entry.
 * Save the address of the line-table entry in register r10.
 */
csrchk2:
	mov	idh,brg
	mov	7,r7
	and	brg,r7,r7|mar
	mov	mem,r10|mar
/*
 * Get flags
 */
	mov	FLAGS1,brg
	add	brg,r10,mar
	mov	mem,r0
/*
 * See if there is an express character
 */
	mov	~SNDEXP,brg
	or	brg,r0,-
	brz	1f
	br	2f
/*
 * Send express character reguardless of state of line
 */
1:
	and	brg,r0,mem
	mov	EXPCHR,brg
	add	brg,r10,mar
	mov	mem,odl
	br	sendit
2:
/*
 * If TTSTOP is set then go to disable
 */
	mov	TTSTOP,brg
	orn	brg,r0,-
	brz	disable
/*
 * Get OFLAG into r6
 */
	mov	OFLAG,brg
	add	brg,r10,mar
	mov	mem,r6
/*
 * Get the current state of this line
 */
	mov	STATE,brg
	add	brg,r10,mar
	mov	mem,r0
/*
 * If state == 0 then go to disable
 * else if state == 1 then go to odrst1
 * else if state == 2 then go to odrst2
 * else if state == 3 then go to odrst3
 * else if state == 4 then go to odrst4
 * else if state == 5 then go to odrst5
 */
	mov	statesw,brg
	br	(add,brg,r0),%statesw
/*
 * Output data request with state == 1 (sending normal data)
 */
odrst1:
/*
 * If there is a saved byte to be sent then go to sendsave
 */
	mov	FLAGS,brg
	add	brg,r10,mar
	mov	mem,brg
	br4	sendsave
/*
 * If there are no characters remaining in the current
 * buffer for this line then go to odrst1a
 */
	mov	NCH,brg
	add	brg,r10,mar
	mov	mem,r0
	dec	r0,-
	brz	odrst1a
/*
 * Decrement the number of characters remaining in the current
 * buffer for this line
 */
	dec	r0
	mov	r0,mem
/*
 * If this was the last character queue the line
 */
	dec	r0,-
	brc	odrst1b
	CALL(oqueue)
	mov	r8,%mar
	br	odrst1b
/*
 * There are no more characters remaining in the current
 * buffer for this line
 */
odrst1a:
/*
 * If the host computer has not yet presented a new buffer then
 * go to nomore
 */
	mov	NCHNEW,brg
	add	brg,r10,mar
	mov	mem,r0
	dec	r0,-
	brz	nomore
	mov	0,mem
/*
 * Decrement the character count for the new output
 * buffer and recore the result as the character
 * count for the current buffer.
 */
	mov	NCH,brg
	add	brg,r10,mar
	dec	r0
	mov	r0,mem
/*
 * If there was only one character in the buffer queue the line
 */
	dec	r0,-
	brc	1f
	CALL(oqueue)
	mov	r8,%mar
1:
/*
 * If the buffer address is even then go to odrst1c
 */
	mov	BUFAD,brg
	add	brg,r10,mar
	mov	mem,brg
	br0	1f
	br	odrst1c
1:
/*
 * Clear the low-order bit of the buffer address
 */
	mov	1,r1
	xor	mem,r1,mem
/*
 * Fetch two bytes
 */
	mov	mem,ial|mar++
	mov	mem,iah|mar++
	mov	mem,r0
	mov	3,brg
	and	brg,r0
	asl	r0
	asl	r0
	mov	NRQ,brg
	or	brg,r0,npr
	buswait;
/*
 * Save the high-order byte as the current output character
 */
	mov	idh,odl
/*
 * Set buffer address += 2
 */
	mov	BUFAD,brg
	add	brg,r10,mar
	mov	2,r2
	add	mem,r2,mem|mar++
	mov	mem,r0
	adc	r0,mem|mar++
	mov	mem,r0
	adc	r0,mem
/*
 * Go to sendtest
 */
	br	sendtest
/*
 * The DZ has just started sending the last byte of the
 * current output buffer.  Call oflush which just
 * bookkeeps the lte and dispable the line.
 * The next buffer-send command will reactivate the line.
 */

nomore:
	CALL(oflush)
	mov	r8,%mar
	br	disable

/*
 * Output data request with non-empty buffer
 */
odrst1b:
/*
 * If there is a high-order byte to send then go to sendhigh
 */
	mov	FLAGS,brg
	add	brg,r10,mar
	mov	mem,brg
	br1	sendhigh
/*
 * We need to get another word of data for this line.
 */
odrst1c:
/*
 * Get the buffer address field of the line-table entry,
 * then set up a unibus request for the next two bytes.
 */
	mov	BUFAD,brg
	add	brg,r10,mar
	mov	mem,ial|mar++
	mov	mem,iah|mar++
	mov	mem,r0
	asl	r0
	asl	r0
	mov	NRQ,brg
	or	brg,r0,npr
/*
 * Wait for the unibus transfer to complete.
 * If an error occurs then go to buserr.
 */
	buswait
/*
 * Set the bit that says to send the high-order byte
 */
	mov	FLAGS,brg
	add	brg,r10,mar
	mov	SENDHIGH,brg
	mov	mem,r0
	or	brg,r0,mem|mar++
/*
 * Save the high-order data byte in the line-table entry
 */
	mov	HIBYTE,brg
	add	brg,r10,mar
	mov	idh,mem
/*
 * Move the low-order data byte to the output data register
 */
	mov	idl,odl
/*
 * Increment the buffer address
 */
	mov	BUFAD,brg
	add	brg,r10,mar
	mov	2,r2
	add	mem,r2,mem|mar++
	mov	mem,r2
	adc	r2,mem|mar++
	mov	mem,r2
	adc	r2,mem
/*
 * Go to sendtest
 */
	br	sendtest
/*
 * Prepare to send the high-order byte.
 */
sendhigh:
/*
 * Move the high-order data byte from the line-table entry 
 * to the output data register
 */
	mov	HIBYTE,brg
	add	brg,r10,mar
	mov	mem,odl
/*
 * Clear the flag that says to send the high-order byte
 */
	mov	FLAGS,brg
	add	brg,r10,mar
	mov	mem,r0
	mov	~SENDHIGH,brg
	and	brg,r0,mem
/*
 * Go to sendtest
 */
	br	sendtest
/*
 * Prepare to send the byte in SAVECHAR
 */
sendsave:
/*
 * Clear the flag that says there is a saved byte to be sent
 */
	mov	~SENDSAVE,brg
	mov	mem,r0
	and	brg,r0,mem
/*
 * Move the saved byte into odl
 */
	mov	SAVECHAR,brg
	add	brg,r10,mar
	mov	mem,odl
/*
 * Go to sendtest
 */
	br	sendtest
/*
 * The following code acts as a filter for outgoing characters
 */
sendtest:
/*
 * Get output flags
 */
	mov	OFLAG,brg
	add	brg,r10,mar
	mov	mem,r6
/*
 * If output post-processing is not enabled then go to sendit
 */
	mov	OPOST,brg
	orn	brg,r6,-
	brz	1f
	br	sendit
1:
/*
 * Get the delays
 */
	mov	DELAYS,brg
	add	brg,r10,mar
	mov	mem,r5
/*
 * Get the output character
 */
	mov	odl,r0
/*
 * If the character is a control character then go to iscc
 */
	mov	040,brg
	sub	brg,r0,-
	brc	1f
	br	iscc
1:
/*
 * If the character is a DEL then go to sendit
 */
	mov	0177,brg
	addn	brg,r0,-
	brz	sendit
/*
 * Increment the column pointer
 */
	mov	ICOL,brg
	add	brg,r10,mar
	mov	mem,r1
	inc	r1,mem
/*
 * If OLCUC is set then translate lower case alpha to upper case
 */
	mov	OLCUC,brg
	orn	brg,r6,-
	brz	1f
	br	2f
1:
	mov	'a',brg
	sub	brg,r0,-
	brc	1f
	br	2f
1:
	mov	'z',brg
	addn	brg,r0,-
	brc	2f
	mov	~040,brg
	and	brg,r0
	mov	r0,odl
2:
/*
 * Go to sendit
 */
	br	sendit
/*
 * Process an ASCII control character
 */
iscc:
/*
 * If the character is a backspace then go to isbs;
 * else if the character is a horizontal tab then go to isht;
 * else if the character is a newline then go to isnl;
 * else if the character is a vertical tab then go to isvt;
 * else if the character is a form-feed then go to isff;
 * else if the character is a carriage return then go to iscr;
 * else go to sendit
 */
	mov	charsw,brg
	br	(add,brg,r0),%charsw
/*
 * Process a newline character
 */
isnl:
/*
 * If ONLRET is set then go to iscr2
 */
	mov	ONLRET,brg
	orn	brg,r6,-
	brz	iscr2
/*
 * If we are not expanding NL for this line then go to nlpart3
 */
	mov	ONLCR,brg
	orn	brg,r6,-
	brz	1f
	br	nlpart3
1:
/*
 * If column pointer == 0 and ONOCR is set then go to iscr2.
 * (We omit the carriage return in this case.)
 */
	mov	ICOL,brg
	add	brg,r10,mar
	mov	mem,r0
	dec	r0,-
	brc	1f
	mov	ONOCR,brg
	orn	brg,r6,-
	brz	iscr2
1:
/*
 * If this NL has already been expanded then go to nlpart2
 */
	mov	FLAGS,brg
	add	brg,r10,mar
	mov	mem,brg
	br0	nlpart2
/*
 * Set flags to indicate that there is a saved byte in SAVECHAR
 * and that the NL character has already been expanded
 */
	mov	SENDSAVE|NLEXP,brg
	mov	mem,r0
	or	brg,r0,mem
/*
 * Put a new-line character into SAVECHAR
 */
	mov	SAVECHAR,brg
	add	brg,r10,mar
	mov	NL,mem
/*
 * Put a carriage-return character into odl
 */
	mov	CR,brg
	mov	brg,odl
/*
 * Go to sendit
 */
	br	sendit
/*
 * Part two of newline character processing
 *
 * The generated carriage return has already been sent.
 *
 * At this point we just have to send the saved newline character
 * and generate the appropriate time delay, if any.
 */
nlpart2:
/*
 * Clear the flag which indicates that this particular newline
 * has already been expanded
 */
	mov	FLAGS,brg
	add	brg,r10,mar
	mov	mem,r0
	mov	~NLEXP,brg
	and	brg,r0,mem
/*
 * Go to iscr2
 */
	br	iscr2
/*
 * Process a carriage-return character
 */
iscr:
/*
 * If translate-CR-to-NL is set then go to xmtnl
 */
	mov	OCRNL,brg
	orn	brg,r6,-
	brz	xmtnl
/*
 * If at col 0 and ONOCR is set do not send out carriage return
 */
	mov	ICOL,brg
	add	brg,r10,mar
	mov	mem,r2
	dec	r2
	brc	iscr2
	mov	ONOCR,brg
	orn	brg,r6,-
	brz	odrst1
/*
 * Entered here from isnl
 */
iscr2:
/*
 * Save column pointer
 */
	mov	ICOL,brg
	add	brg,r10,mar
	mov	mem,r2
/*
 * Set column pointer = 0
 */
	mov	0,mem
/*
 * Get delay type for carriage-return.
 * If delay type == 0 then go to sendit
 * else if delay type == 1 then go to cr1
 * else if delay type == 2 then go to cr2
 * else go to cr3
 */
	mov	CRDELAY,brg
	and	brg,r5,brg
	mov	0,brg>>
	br1	1f
	br0	cr1
	br	sendit
1:
	br0	cr3
	br	cr2
/*
 * Carriage-return delay type 1
 */
cr1:
/*
 * If OFILL is set then go to filltwo
 */
	mov	OFILL,brg
	orn	brg,r6,-
	brz	filltwo
/*
 * Set delay ticks = (icol>>4)+3
 */
	mov	017<<4,brg
	and	brg,r2,brg
	mov	brg,brg>>
	mov	brg,brg>>
	mov	brg,brg>>
	mov	brg,brg>>
	mov	3,r3
	add	brg,r3
/*
 * If delay > 6 then set delay = 6
 */
	mov	6,brg
	sub	brg,r3,-
	brc	1f
	mov	r3,brg
1:
/*
 * Go to delay
 */
	br	delay
/*
 * Carriage-return delay type 2
 */
cr2:
/*
 * If OFILL is set then go to fillfour
 */
	mov	OFILL,brg
	orn	brg,r6,-
	brz	fillfour
/*
 * Schedule a delay
 */
	mov	5,brg
/*
 * Go to delay
 */
	br	delay
/*
 * Carriage-return delay type 3
 */
cr3:
/*
 * If OFILL is set then go to fill40
 */
	mov	OFILL,brg
	orn	brg,r6,-
	brz	fill40
/*
 * Schedule a .15 second delay
 */
	mov	9,brg
/*
 * Go to delay
 */
	br	delay
/*
 * Translate carriage return to newline
 */
xmtnl:
	mov	NL,brg
	mov	brg,odl
	br	nlpart3
/*
 * Entered here from isnl when neither ONLRET nor ONLCR is set
 */
nlpart3:
/*
 * If newline delay is not enabled then go to sendit
 */
	mov	NLDELAY,brg
	orn	brg,r5,-
	brz	1f
	br	sendit
1:
/*
 * Schedule a .083 second delay
 */
	mov	5,brg
/*
 * Go to delay
 */
	br	delay
/*
 * Process a horizontal tab control character.
 */
isht:
/*
 * If we are expanding tabs into spaces then go to ht3
 */
	mov	HTDELAY,brg
	orn	brg,r5,-
	brz	ht3
/*
 * Save the column pointer
 */
	mov	ICOL,brg
	add	brg,r10,mar
	mov	mem,r2
/*
 * Update the column pointer
 */
	mov	mem,r1
	mov	7,brg
	or	brg,r1
	inc	r1,mem
/*
 * Get the delay type for horizontal tabs.  If the delay type is
 * one then go to ht1; else if the delay type is two then go to ht2;
 * else go to sendit
 */
	mov	HTDELAY,brg
	and	brg,r5,brg
	mov	0,brg>>
	mov	0,brg>>
	mov	0,brg>>
	br0	ht1
	br1	ht2
	br	sendit
/*
 * HT delay type 1
 */
ht1:
/*
 * If OFILL is set then go to filltwo
 */
	mov	OFILL,brg
	orn	brg,r6,-
	brz	filltwo
/*
 * Calculate delay ticks = 1 - (icol|~7)
 */
	mov	7,brg
	orn	brg,r2,brg
	mov	1,r1
	sub	brg,r1
/*
 * If delay < 5 then go to sendit
 */
	mov	5,brg
	sub	brg,r1,-
	brc	sendit
	mov	r1,brg
/*
 * Go to delay
 */
	br	delay
/*
 * HT delay type 2
 */
ht2:
/*
 * If OFILL is set then go to filltwo
 */
	mov	OFILL,brg
	orn	brg,r6,-
	brz	filltwo
/*
 * Schedule a .1 second delay
 */
	mov	6,brg
/*
 * Go to delay
 */
	br	delay
/*
 * HT delay type 3:  Expand horizontal tab into spaces
 */
ht3:
/*
 * Calculate the number of spaces needed
 *
 * The number is calculated by evaluating 7 & ~icol
 */
	mov	ICOL,brg
	add	brg,r10,mar
	mov	0,r0
	orn	mem,r0
	mov	7,brg
	and	brg,r0
/*
 * Update the column pointer
 */
	mov	mem,r1
	or	brg,r1
	inc	r1,mem
/*
 * Save the count of the number of spaces needed
 */
	mov	COUNT,brg
	add	brg,r10,mar
	mov	r0,mem
/*
 * Put a space into odl
 */
	mov	SP,brg
	mov	brg,odl
/*
 * Set state = 3 (expanding horizontal tab)
 */
	mov	STATE,brg
	add	brg,r10,mar
	mov	3,mem
/*
 * Go to sendit
 */
	br	sendit
/*
 * Process a backspace character
 */
isbs:
/*
 * If the column pointer is not already zero then decrement it
 */
	mov	ICOL,brg
	add	brg,r10,mar
	mov	mem,r1
	dec	r1
	brz	1f
	mov	r1,mem
1:
/*
 * If backspace delays have not been requested then go to sendit
 */
	mov	BSDELAY,brg
	orn	brg,r5,-
	brz	1f
	br	sendit
1:
/*
 * If OFILL is set then go to fillone
 */
	mov	OFILL,brg
	orn	brg,r6,-
	brz	fillone
/*
 * Schedule a .05 second delay
 */
	mov	3,brg
/*
 * Go to delay
 */
	br	delay
/*
 * Process a vertical-tab character
 */
isvt:
/*
 * If form-feed delays have not been requested then go to sendit
 */
	mov	VTDELAY,brg
	orn	brg,r5,-
	brz	1f
	br	sendit
1:
/*
 * Schedule a two-second delay
 */
	mov	127,brg
/*
 * Go to delay
 */
	br	delay
/*
 * Process a form-feed character
 */
isff:
/*
 * If form-feed delays have not been requested then go to sendit
 */
	mov	FFDELAY,brg
	orn	brg,r5,-
	brz	1f
	br	sendit
1:
/*
 * Schedule a two-second delay
 */
	mov	127,brg
/*
 * Go to delay
 */
	br	delay
/*
 * Schedule one fill character
 */
fillone:
	mov	1,brg
	br	fdelay
/*
 * Schedule two fill characters
 */
filltwo:
	mov	2,brg
	br	fdelay
/*
 * Schedule four fill characters
 */
fillfour:
	mov	4,brg
	br	fdelay
/*
 * Schedule the appropriate number of fill characters for a
 * Teletype 40 printer or a GE Terminet 1200
 */
fill40:
	mov	36,brg
	sub	brg,r2,brg
	brc	sendit
	mov	0,r0
	sub	brg,r0,brg
	br	fdelay
/*
 * Schedule a time delay on a given line
 *
 * The amount of delay needed has already been calculated.
 * Register brg contains the number of 16.7 msec ticks needed
 * for the delay.  The character which requires time fill has not
 * yet been passed to the DZ11.
 */
delay:
/*
 * If delay == 0 then go to sendit
 */
	mov	brg,r1
	dec	r1
	brz	sendit
/*
 * Add 6 to the delay count and save the result in the LTE
 *
 * The reason for adding 6 to the delay count is roughly as follows:
 *
 * 6/60 = .1 second, which is one character time at 110 baud;
 * this compensates for the double-buffering in the DZ11;
 * at speeds greater than 110 baud it overcompensates, but this is
 * traditional
 */
	mov	COUNT,brg
	add	brg,r10,mar
	mov	6,brg
	add	brg,r1,mem
/*
 * Set STATE = 2
 */
	mov	STATE,brg
	add	brg,r10,mar
	mov	2,mem
/*
 * Go to sendit
 */
	br	sendit
/*
 * Schedule a time delay on a given line
 *
 * The amount of delay needed has already been calculated.
 * Register brg contains the number of character times needed
 * for the delay.  The character which requires time fill has not
 * yet been passed to the DZ11.
 *
 * This code differs from the delay code above by using fill
 * characters rather than timed pauses
 */
fdelay:
/*
 * If delay == 0 then go to sendit
 */
	mov	brg,r1
	dec	r1,-
	brz	sendit
/*
 * Put the delay count in the lte
 */
	mov	COUNT,brg
	add	brg,r10,mar
	mov	r1,mem
/*
 * Set STATE = 4  (Sending fill characters)
 */
	mov	STATE,brg
	add	brg,r10,mar
	mov	4,mem
/*
 * Go to sendit
 */
	br	sendit
/*
 * Output data request with state == 2 (sending a character that needs 
 * time fill)
 *
 * Note:  The line was put into state 2 when we passed the
 * character that needs time fill to the DZ11.  Since the DZ11
 * is double-buffered, we get another output data request when
 * the DZ11 actually starts sending the character that needs
 * time fill.
 * At this point we turn off the enable bit and put the lte into the
 * twait state.
 *
 * Note that, because of double-buffering in the DZ11, the wait
 * time is overlapped with the time required to send the character
 * that needs time fill.
 */
odrst2:
/*
 * Put this lte into the twait state
 */
	mov	r10,mar
	mov	5,mem
/*
 * Go to disable
 */
	br	disable
/*
 * Output data request with state == 3 (expanding horizontal tab)
 */
odrst3:
/*
 * Decrement the count field of the lte.  If the result is
 * negative then go to resume
 */
	mov	COUNT,brg
	add	brg,r10,mar
	mov	mem,r0
	dec	r0,mem
	brz	resume
/*
 * Put a space character into odl
 */
	mov	SP,brg
	mov	brg,odl
/*
 * Go to sendit
 */
	br	sendit
/*
 * Output data request with state == 4 (sending fill characters)
 */
odrst4:
/*
 * Decrement the count field of the lte.  If the result is
 * negative then go to resume
 */
	mov	COUNT,brg
	add	brg,r10,mar
	mov	mem,r0
	dec	r0,mem
	brz	resume
/*
 * Put a fill character into odl
 */
	mov	DEL,brg
	mov	brg,odl
	mov	OFDEL,brg
	orn	brg,r6,-
	brz	1f
	mov	NUL,brg
	mov	brg,odl
1:
/*
 * Go to sendit
 */
	br	sendit
/*
 * Output data request with state == 5 (twait)
 *
 * This may happen if an express character has been sent while in this state
 */
odrst5:
	br	disable
/*
 * The following code is used to restart a line which has been
 * delaying or expanding a horizontal tab character
 */
resume:
/*
 * Set state = 1 (sending normal data)
 */
	mov	STATE,brg
	add	brg,r10,mar
	mov	1,mem
/*
 * Go to odrst1
 */
	br	odrst1
/*
 * Send one character on the specified line.  The character to be
 * sent has already been placed in odl
 */
sendit:
/*
 * Set up a unibus request to put one byte into TBUF
 */
	mov	tdrput,mar
	mov	mem,oal|mar++
	mov	mem,oah|mar++
	mov	nprx,r0
	mov	~(BRQ|ACLO),brg
	and	brg,r0
	or	mem,r0,nprx|mar++
	mov	mem,npr
/*
 * Wait for the unibus transfer to complete.
 * If an error occurs then go to buserr.
 */
	buswait
/*
 * Go to disp
 */
	br	disp

/*
 * Subroutine to enable a line
 */
enable:
/*
 * Set up a unibus request to enable the line
 */
	mov	r8,%mar
	mov	enblbits,brg
	add	brg,r7,mar
	mov	mem,r0
	mov	actline,mar
	or	mem,r0,mem
	mov	mem,odl
	mov	tcrput,mar
	mov	mem,oal|mar++
	mov	mem,oah|mar++
	mov	nprx,r0
	mov	~(BRQ|ACLO),brg
	and	brg,r0
	or	mem,r0,nprx|mar++
	mov	mem,npr
/*
 * Wait for the unibus transfer to complete.
 */
	buswait
/*
 * Return to caller
 */
	RETURN
/*
 * The following code is used to turn off the transmit-enable bit 
 * at the beginning of a timed delay period or when there are no
 * characters to transmit
 */
disable:
/*
 * Clear the KMC copy of the transmit-enable bit for this line
 * and set up a unibus request to clear the corresponding bit
 * in the DZ11
 */
	mov	r8,%mar
	mov	enblbits,brg
	add	brg,r7,mar
	mov	mem,r0
	mov	actline,mar
	and	mem,r0
	xor	mem,r0,mem
	mov	mem,odl
	mov	tcrput,mar
	mov	mem,oal|mar++
	mov	mem,oah|mar++
	mov	nprx,r0
	mov	~(BRQ|ACLO),brg
	and	brg,r0
	or	mem,r0,nprx|mar++
	mov	mem,npr
/*
 * Wait for the unibus transfer to complete.
 */
	buswait
/*
 * Go to disp
 */
	br	disp

/*
 * This two entry point subroutine is called at oflush to
 * flush any current output regardless of the current state
 * of the line.  Oflush is also called when the last character
 * of normal output has been sent out and the dz is requesting
 * another character.  This simply bookkeeps the LTE.  The
 * entry point oqueue is called when the last character in
 * the current output buffer is used so the host may reload
 * another buffer before the dz requests another character.
 */
oflush:
	mov	r8,%mar
/*
 * Clear output flags
 */
	mov	FLAGS,brg
	add	brg,r10,mar
	mov	~(SENDSAVE|SENDHIGH|NLEXP),brg
	mov	brg,r0
	and	mem,r0,mem
	mov	FLAGS1,brg
	add	brg,r10,mar
	mov	mem,r0
	mov	~(TTSTOP|PREVNB),brg
	and	brg,r0,mem
	mov	NCH,brg
	add	brg,r10,mar
	mov	0,mem
	mov	NCHNEW,brg
	add	brg,r10,mar
	mov	0,mem
/*
 * Set state = 0
 */
	mov	STATE,brg
	add	brg,r10,mar
	mov	0,mem
	RETURN
/*
 * Queue the output buffer
 */
oqueue:
	mov	r8,%mar
/*
 * Set the buffer-completion bit for this line
 */
	mov	enblbits,brg
	add	brg,r7,mar
	mov	mem,r0
	mov	buffcomp,mar
	or	mem,r0,mem
/*
 * If a report to the driver has already been requested for this
 * DZ then return
 */
	mov	status,mar
	mov	mem,brg
	br1	2f
/*
 * Set a flag to indicate that a report to the driver has been
 * requested
 */
	mov	1<<1,r2
	or	mem,r2,mem
/*
 * Insert the page for the current DZ at the end of the
 * driver-report-needed queue
 */
	mov	rptlink,mar
	mov	NIL,mem
	mov	r13,brg
	brz	1f
	mov	brg,%mar
	mov	r8,mem
	mov	r8,brg|%mar
	mov	brg,r13
	br	2f
1:
	mov	r8,brg
	mov	brg,r13
	mov	brg,r14
2:
	RETURN

/*
 * The unibus transfer failed to complete within a reasonable time
 */
buserr:
/*
 * Clear the non-existent memory bit
 */
	mov	nprx,r0
	mov	~(BRQ|ACLO|NEM),brg
	and	brg,r0,nprx
/*
 * Set a flag to indicate that a bus error has occurred
 */
	mov	status,mar
	mov	mem,brg
	mov	1<<5,brg
	mov	brg,r0
	or	mem,r0,mem
/*
 * If a report to the driver has already been requested for this
 * DZ then go to notlast
 */
	mov	status,mar
	mov	mem,brg
	br1	disp
/*
 * Set a flag to indicate that a report to the driver has been
 * requested
 */
	mov	1<<1,r2
	or	mem,r2,mem
/*
 * Insert the page for the current DZ at the end of the
 * driver-report-needed queue
 */
	mov	rptlink,mar
	mov	NIL,mem
	mov	r13,brg
	brz	1f
	mov	brg,%mar
	mov	r8,mem
	mov	r8,brg|%mar
	mov	brg,r13
	br	2f
1:
	mov	r8,brg
	mov	brg,r13
	mov	brg,r14
2:
/*
 * Go to disp
 */
	br	disp
/*
 * Error loops for debugging
 */
e1:
	mov	NIL,brg
	mov	brg,r15
	br	.

endseg:
