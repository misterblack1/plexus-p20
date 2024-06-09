/*
 * Input segment
 *
 * This segment is entered from the main segment when it has been
 * determined that a character has been received
 */
	.org	2048
seg2:
/*
 * Define error bits in DZ-11 status register
 */
#define	PARERR	1<<4
#define	FRMERR	1<<5
/*
 * The follwoing macro calls subroutine stash to put the character
 * in r0 into the input buffer.  For efficiency, this macro can
 * only be used in the same segment as stash.  Also, registers
 * r2-r4 are destroyed.
 */

#define PUTC \
	mov	%putret,%mar; \
	mov	putret,mar; \
	mov	%0f,mem|mar++; \
	mov	0f,mem; \
	br	stash; \
0:
/*
 * Enter here from main segment
 */
inchar:
/*
 * Read RBUF
 */
	mov	csrget,mar
	mov	mem,r0|mar++
	mov	mem,r1|mar++
	mov	mem,r2
	mov	2,brg
	add	brg,r0,ial
	adc	r1,brg
	mov	brg,iah
	adc	r2
	asl	r2
	asl	r2
	mov	NRQ,brg
	or	brg,r2,brg
	mov	brg,npr
	BUSWAIT(buserr2)
/*
 * If data-valid bit is not set then go to dispret2
 */
	mov	idh,brg
	br7	1f
	br	dispret2
1:
/*
 * Set r7 = line number; set r10 = address of line table entry
 */
	mov	idh,brg
	mov	7,r7
	and	brg,r7,r7|mar
	mov	mem,r10
	mov	%dzst,brg
	add	brg,r8,%mar
/*
 * Put IFLAG1 into oal and IFLAG2 into oah
 */
	mov	IFLAG1,brg
	add	brg,r10,mar
	mov	mem,oal|mar++
	mov	mem,oah
/*
 * Get bits 8-15 of RBUF
 */
	mov	idh,r0
/*
 * If a framing error has occurred
 */
	mov	FRMERR,brg
	orn	brg,r0,-
	brz	1f
	br	ckpar
1:
/*
 * See if it is a break by seeing if data is zero, else
 * treat as a parity error
 */
	mov	idl,r1
	dec	r1
	brc	parerr
/*
 * A break has been detected, see if breaks are to be ignored
 */
	mov	oal,r6
	mov	IGNBRK,brg
	orn	brg,r6,-
	brz	dispret2
/*
 * Breaks are not being ignored, see if signal should be sent
 */
	mov	BRKINT,brg
	orn	brg,r6,-
	brz	sigbrk
/*
 * Put a null character out
 */
	mov	NUL,brg
	mov	brg,r0
	br	putit

/*
 * If parity is being checked and a parity error has occurred then translate
 * it to a null or del depending on PARDEL
 */
ckpar:
	mov	oal,r6
	mov	INPCK,brg
	orn	brg,r6,-
	brz	1f
	br	2f
1:
	mov	PARERR,brg
	orn	brg,r0,-
	brz	parerr
	br	2f
parerr:
	mov	oal,r6
/*
 * See if parity errors are being ignored
 */
	mov	IGNPAR,brg
	or	brg,r6,-
	brz	dispret2
/*
 * See if parity being marked
 *
 */
	mov	PARMRK,brg
	orn	brg,r6,-
	brz	parmrk
/*
 * Put a null character out
 */
	mov	NUL,brg
	mov	brg,r0
	br	putit
/*
 * Mark the character a a parity error
 */
parmrk:
	mov	0377,brg
	mov	brg,r0
	PUTC
	mov	0,r0
	PUTC
	mov	idl,r0
	br	putit
2:
/*
 * Fetch input char
 */
	mov	idl,r0
/*
 * See if in 7 or 8 bit mode
 */
ckstrip:
	mov	oal,r6
	mov	ISTRIP,brg
	orn	brg,r6,-
	brz	1f
	br	2f
1:
	mov	0177,brg
	and	brg,r0
	mov	r0,idl
2:
/*
 * See if start/stop being done
 */
	mov	oah,r6
	mov	IXON,brg
	orn	brg,r6,-
	brz	1f
	br	2f
/*
 * Line is under control, see if line is stopped
 */
1:
	mov	FLAGS1,brg
	add	brg,r10,mar
	mov	mem,r2
	mov	TTSTOP,brg
	orn	brg,r2,-
	brz	stopped
/*
 * Line is not stopped, see if this is stop character
 */
	mov	CSTOP,brg
	addn	brg,r0,-
	brz	1f
	br	ckignor
/*
 * Stop the line
 */
1:
	mov	TTSTOP,brg
	or	brg,r2,mem
	br	dispret2

/*
 * If character is start character or IXANY is set restart line
 */
stopped:
	mov	CSTART,brg
	addn	brg,r0,-
	brz	start
	mov	IXANY,brg
	orn	brg,r6,-
	brz	start
	br	ckignor
/*
 * Restart line
 */
start:
	mov	~TTSTOP,brg
	and	brg,r2,mem
	CALL(enable)
/*
 * Restore values destroyed by enable
 */
	mov	%dzst,brg
	add	brg,r8,%mar
	mov	IFLAG1,brg
	add	brg,r10,mar
	mov	mem,oal|mar++
	mov	mem,oah
	mov	idl,r0
/*
 * Do not return start or stop character
 */
ckignor:
	mov	CSTART,brg
	addn	brg,r0,-
	brz	dispret2
	mov	CSTOP,brg
	addn	brg,r0,-
	brz	dispret2
2:
/*
 * If character is 0377 and parity is being marked
 * then escape it with another 0377
 */
	mov	r0,brg
	brz	1f
	br	2f
1:
	mov	oal,r6
	mov	PARMRK,brg
	orn	brg,r6,-
	brz	1f
	br	2f
1:
	PUTC
	br	putit
2:
/*
 * See if a NL
 */
	mov	NL,brg
	addn	brg,r0,-
	brz	1f
	br	ckcr
/*
 * See if NL mapped to CR
 */
1:
	mov	oal,r6
	mov	INLCR,brg
	orn	brg,r6,-
	brz	1f
	br	putit
1:
	mov	CR,brg
	mov	brg,r0
	br	putit
/*
 * See if a CR
 */
ckcr:
	mov	CR,brg
	addn	brg,r0,-
	brz	1f
	br	cklc
/*
 * Character is a CR, see if it should be ignored
 */
1:
	mov	oal,r6
	mov	IGNCR,brg
	orn	brg,r6,-
	brz	dispret2
/*
 * See if CR mapped to NL
 */
	mov	oah,r6
	mov	ICRNL,brg
	orn	brg,r6,-
	brz	1f
	br	putit
1:
	mov	NL,brg
	mov	brg,r0
	br	putit
/*
 * If IUCLC is set then translate upper-case letters to lower case
 */
cklc:
	mov	oah,r6
	mov	IUCLC,brg
	orn	brg,r6,-
	brz	1f
	br	3f
1:
	mov	'A',brg
	sub	brg,r0,-
	brc	1f
	br	3f
1:
	mov	'Z',brg
	addn	brg,r0,-
	brc	3f
2:
	mov	'a'-'A',brg
	add	brg,r0
3:

/*
 * Put out the character in R0
 */
putit:
	PUTC


/*
 * ITIME is used time how long it has been since the first character
 * arrived.
 */

tsttim:
/*
 * See if timer is active
 */
	mov	%dzst,brg
	add	brg,r8,%mar
	mov	FLAGS1,brg
	add	brg,r10,mar
	mov	mem,r1
	mov	TIMACT,brg
	orn	brg,r1,-
	brz	2f
/*
 * If INITIM>0 then decrement and start timer, else queue input
 */
	mov	INITIM,brg
	add	brg,r10,mar
	mov	mem,r1
	mov	ITIME,brg
	add	brg,r10,mar
	dec	r1,mem
	brz	1f
/*
 * Indicate timer active
 */
	mov	FLAGS1,brg
	add	brg,r10,mar
	mov	mem,r1
	mov	TIMACT,brg
	or	brg,r1,mem
	br	2f
/*
 * No timer on input, queue line
 */
1:
	CALL(queue)
2:
	br	dispret2

/*
 * Stash the character in an input buffer
 */
stash:
	mov	%dzst,brg
	add	brg,r8,%mar
/*
 * If there are at least MAXINCH characters in the input queue
 * for this line then flush input and exit
 */
#define	MAXINCH	128
	mov	NICH,brg
	add	brg,r10,mar
	mov	mem,r2
	mov	MAXINCH,brg
	sub	brg,r2,-
	brc	1f
	br	2f
/*
 * Flush input and exit
 */
1:
	CALL(iflush)
	br	dispret2
/*
 * Get the pointer to the last queue entry in the input queue
 * for this line.  If the queue is empty then go to stash1a
 */
2:
	mov	TAILINQ,brg
	add	brg,r10,mar
	mov	mem,r5
	brz	stash1a
/*
 * Get the pointer to the input buffer associated with this queue
 * entry
 */
	mov	~03,brg
	and	brg,r5,brg
	mov	brg,r3
	mov	03,brg
	and	brg,r5,%mar
	inc	r3,mar
	mov	mem,r4|mar++
/*
 * Get the character count for this buffer and
 * calculate the index of the next character by adding
 * the character count to the index of the first character
 */
	mov	mem,r2|mar++
	add	mem,r2
/*
 * If there is room in this buffer for another character
 * then go to stash1b
 */
	mov	16,brg
	addn	brg,r2,-
	brz	stash1a
	br	stash1b
stash1a:
/*
 * Allocate a 16-byte input buffer and attach it via a queue entry
 * to the end of the input queue for the current line
 */
	ALLOCATE(r4,flush)
	GETQ(r5,queuerr2)
	mov	NIL,mem|mar++
	mov	r4,mem|mar++
	mov	0,mem|mar++
	mov	0,mem
	mov	TAILINQ,brg
	add	brg,r10,mar
	mov	%dzst,brg
	add	brg,r8,%mar
	mov	mem,r3
	brz	1f
	mov	r5,mem
	mov	03,brg
	and	brg,r3,%mar
	mov	~03,brg
	and	brg,r3,mar
	br	2f
1:
	mov	HEADINQ,brg
	add	brg,r10,mar
	mov	r5,mem|mar++
2:
	mov	r5,mem
stash1b:
/*
 * Increment the number of characters in the input queue for this line
 */
	mov	%dzst,brg
	add	brg,r8,%mar
	mov	NICH,brg
	add	brg,r10,mar
	mov	mem,r3
	inc	r3,mem
/*
 * Increment the character count for this buffer and calculate the index
 * of the next available character position
 */
	mov	~03,brg
	and	brg,r5,brg
	mov	brg,r3
	mov	03,brg
	and	brg,r5,%mar
	mov	2,brg
	add	brg,r3,mar
	mov	mem,r3
	inc	r3,mem|mar++
	add	mem,r3
/*
 * Put the character in r0 into the buffer pointed to by r4
 * at the offset specified by r3
 */
	mov	017,brg
	and	brg,r4,%mar
	mov	~017,brg
	and	brg,r4,brg
	add	brg,r3,mar
	mov	r0,mem
/*
 * If this is a delimeter increment # of delimeters
 */
	mov	%dzst,brg
	add	brg,r8,%mar
	mov	FLAGS1,brg
	add	brg,r10,mar
	mov	mem,r1
	mov	~DELIM,brg
	or	brg,r1,-
	brz	1f
	br	2f
/*
 * Delimeter found, turn off flag
 */
1:
	and	brg,r1,mem
/*
 * Increment delimeter count
 */
	mov	~03,brg
	and	brg,r5,%mar
	mov	03,brg
	and	brg,r5
	mov	3,brg
	add	brg,r5,mar
	mov	mem,r1
	inc	r5,mem
2:
/*
 * If there are enough characters to fill a c-list call queue
 */
	mov	%dzst,brg
	add	brg,r8,%mar
	mov	NICH,brg
	add	brg,r10,mar
	mov	mem,r3
	mov	CLSIZE,brg
	addn	brg,r3,-
	brz	1f
	br	2f
1:
	CALL(queue)
2:
/*
 * Return to main line
 */
	mov	%putret,%mar
	mov	putret,mar
	mov	mem,pcr|mar++
	mov	mem,r2
	mov	%dzst,brg
	add	brg,r8,%mar
	jmp	(r2)

enableit:
/*
 * If the line is inactive then enable it
 */
/*
 * Restore page regs
 */
	mov 	%dzst,brg
	add	brg,r8,%mar
	mov	STATE,brg
	add	brg,r10,mar
	mov	mem,r0
	dec	r0,-
	brc	1f
	mov	1,mem
	CALL(enable)
1:
/*
 * Return to the dispatcher
 */
	br	dispret2
/*
 * Process a QUIT character (FS or DEL)
 */
sigbrk:
/*
 * Schedule a BREAK signal to the host
 */
/*
 * Get an unused queue entry.  If there is not one available
 * then go to queuerr2
 */
	GETQ(r4,queuerr2)
/*
 * Set link to next queue entry = NIL
 */
	mov	NIL,mem|mar++
/*
 * Put (dz nr<<3)|line nr in second byte of queue entry
 */
	mov	r8,brg
	mov	brg,r1
	asl	r1
	asl	r1
	asl	r1
	mov	7,brg
	and	brg,r7,brg
	or	brg,r1,mem|mar++
/*
 * Chain this queue entry to the end of the break-request-
 * report queue
 */
	mov	tailbrkq,mar
	mov	%tailbrkq,%mar
	mov	mem,r5
	brz	1f
	mov	r4,mem
	mov	3,brg
	and	brg,r5,%mar
	mov	~3,brg
	and	brg,r5,mar
	mov	r4,mem
	br	2f
1:
	mov	r4,mem
	mov	headbrkq,mar
	mov	r4,mem
2:
/*
 * Set a flag to indicate that the break-request-report
 * queue is non-empty
 */
	mov	CHKBRKQ,brg
	or	brg,r14
/*
 * Flush input and output queued for this line
 */
	CALL(iflush)
	CALL(oflush)
	br	dispret2

/*
 * Flush all queued input for the current line.  
 */
iflush:
/*
 * Clean up the LTE
 */
	mov	%dzst,brg
	add	brg,r8,%mar
	mov	NICH,brg
	add	brg,r10,mar
	mov	0,mem
	mov	RBCOPO,brg
	add	brg,r10,mar
	mov	0,mem
/*
 * Clean up list heads and get pointer to first queue entry
 */
	mov	HEADINQ,brg
	add	brg,r10,mar
	mov	%dzst,brg
	add	brg,r8,%mar
	mov	mem,r4
	brz	fluret
/*
 * Zero out pointers
 */
	mov	NIL,mem|mar++
	mov	NIL,mem

2:
/*
 * Free the queue entry and associated input buffer
 */
	mov	~3,brg
	and	brg,r4,mar
	mov	3,brg
	and	brg,r4,%mar
/*
 * Get the pointer to next queue entry and pointer to input buffer
 */
	mov	mem,r3|mar++
	mov	mem,r5
/*
 * Release the buffer entry and free the current queue entry
 */
	RELEASE(r5)
	FREEQ(r4)
/*
 * Service the next queue entry if there is one
 */
	mov	r3,brg
	brz	fluret
	mov	brg,r4
	br	2b
fluret:
	RETURN

/*
 * Return to top of dispatcher loop in main segment
 */
dispret2:
	mov	%disp,brg
	mov	brg,pcr
	jmp	disp
queue:
/*
 * See if Input Ready Queue for this line already
 */
	mov	%dzst,brg
	add	brg,r8,%mar
	mov	FLAGS1,brg
	add	brg,r10,mar
	mov	mem,r4
	mov	LTEINQ,brg
	orn	brg,r4,-
	brz	3f
	or	brg,r4,mem
/*
 * Put this line on the input-ready queue
 */
/*
 * Get an unused queue entry.  If there is not one available
 * then go to queuerr2
 */
	GETQ(r4,queuerr2)
/*
 * Set link to next queue entry = NIL
 */
	mov	NIL,mem|mar++
/*
 * Put the line number and the DZ number into the queue entry
 */
	mov	r7,mem|mar++
	mov	r8,mem|mar++
/*
 * Chain this queue entry to the end of the input-ready queue
 */
	mov	tailinrq,mar
	mov	%tailinrq,%mar
	mov	mem,r5
	brz	1f
	mov	r4,mem
	mov	3,brg
	and	brg,r5,%mar
	mov	~3,brg
	and	brg,r5,mar
	mov	r4,mem
	br	2f
1:
	mov	r4,mem
	mov	headinrq,mar
	mov	r4,mem
2:
/*
 * Set a flag to indicate that the input-buffer-completion-report
 * queue is non-empty
 */
	mov	CHKINRQ,brg
	or	brg,r14
/*
 * Return to caller
 */
3:
	RETURN
/*
 * Ran out of input buffers
 */
flush:
	ERROR(FLUSH)
/*
 * little need to keep processing
 */
	br	.
/*
 * Unibus error
 */
buserr2:
/*
 * Clear the non-existent memory bit
 */
	mov	nprx,r0
	mov	~(BRQ|ACLO|NEM),brg
	and	brg,r0,nprx
	ERROR(BUSERR2)
	br	dispret2
/*
 * Ran out of queue entries
 */
queuerr2:
	br	flush
endseg2:
