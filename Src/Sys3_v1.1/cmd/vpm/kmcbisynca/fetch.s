/*
 * Process state == READY:  Fetch next instruction
 */
fetch:
/*
 * If there is a report in progress then go to disp
 */
	mov	r14,brg
	br1	disp
/*
 * Set MAR = base address + PC
 */
	mov	start,brg
	add	brg,r8,mar
	adc	r9,brg|%mar
/*
 * Set PC += 2
 */
	mov	2,brg
	add	brg,r8
	adc	r9
/*
 * Put first byte in r1
 */
	mov	mem,brg|r1|mar++
/*
 * If instruction is in format one then go to isjump
 */
	br7	isjump
/*
 * Validate op code
 */
	mov	lastop-opsw,brg
	sub	brg,r1,-
	brc	illegal
	mov	opsw,brg
	add	brg,r1
/*
 * Get the second byte of the instruction
 */
	mov	mem,r5|brg|mar
/*
 * Set the page register for a possible data reference
 */
	mov	%regs,%mar
/*
 * Branch on op code
 */
	br	(r1),%opsw
/*
 * Process an instruction in format one (most jumps)
 */
isjump:
/*
 * Extract op code
 */
	mov	0,brg>>
	mov	0,brg>>
	mov	0,brg>>
	mov	0,brg>>
	mov	brg,r2
	mov	7,brg
	and	brg,r2
/*
 * Set r1 = high-order bits of jump address
 */
	mov	017,brg
	and	brg,r1
/*
 * Put second byte of instruction into r0
 */
	mov	mem,r0
/*
 * Set page register for possible data reference
 */
	mov	%regs,%mar
/*
 * Branch on op code
 */
	mov	jumpsw,brg
	br	(add,brg,r2),%jumpsw
/*
 * MOV:  AC = MEM[Y]
 */
movd:
	mov	mem,r6
	br	disp
/*
 * AND:  AC &= MEM[Y]
 */
andd:
	and	mem,r6
	br	disp
/*
 * OR:  AC |= MEM[Y]
 */
ord:
	or	mem,r6
	br	disp
/*
 * XOR:  AC ^= MEM[Y]
 */
xord:
	xor	mem,r6
	br	disp
/*
 * BIC:  AC &= ~MEM[Y]
 */
bicd:
	mov	0,r0
	orn	mem,r0,brg
	and	brg,r6
	br	disp
/*
 * ADD:  AC += MEM[Y]
 */
addd:
	add	mem,r6
	br	disp
/*
 * SUB:  AC -= MEM[Y]
 */
subd:
	sub	mem,r6
	br	disp
/*
 * TSTEQL:  AC = (AC==MEM[Y])
 */
tsteqld:
	addn	mem,r6
	brz	true
	br	false
/*
 * TSTNEQ:  AC = (AC!=MEM[Y])
 */
tstneqd:
	addn	mem,r6
	brz	false
	br	true
/*
 * TSTGTR:  AC = (AC>MEM[Y])
 */
tstgtrd:
	addn	mem,r6
	brc	true
	br	false
/*
 * TSTLSS:  AC = (AC<MEM[Y])
 */
tstlssd:
	sub	mem,r6
	brc	false
	br	true
/*
 * TSTGEQ:  AC = (AC>=MEM[Y])
 */
tstgeqd:
	sub	mem,r6
	brc	true
	br	false
/*
 * TSTLEQ:  AC = (AC<=MEM[Y])
 */
tstleqd:
	addn	mem,r6
	brc	false
	br	true
/*
 * MOVI:  AC = Y
 */
movi:
	mov	brg,r6
	br	disp
/*
 * ANDI:  AC &= Y
 */
andi:
	and	brg,r6
	br	disp
/*
 * ORI:  AC |= Y
 */
ori:
	or	brg,r6
	br	disp
/*
 * XORI:  AC ^= Y
 */
xori:
	xor	brg,r6
	br	disp
/*
 * BICI:  AC &= ~Y
 */
bici:
	mov	0,r0
	orn	brg,r0,brg
	and	brg,r6
	br	disp
/*
 * ADDI:  AC += Y
 */
addi:
	add	brg,r6
	br	disp
/*
 * SUBI:  AC -= Y
 */
subi:
	sub	brg,r6
	br	disp
/*
 * TSTEQLI:  AC = (AC==Y)
 */
tsteqli:
	addn	brg,r6
	brz	true
	br	false
/*
 * TSTNEQI:  AC = (AC!=Y)
 */
tstneqi:
	addn	brg,r6
	brz	false
	br	true
/*
 * TSTGTRI:  AC = (AC>Y)
 */
tstgtri:
	addn	brg,r6
	brc	true
	br	false
/*
 * TSTLSSI:  AC = (AC<Y)
 */
tstlssi:
	sub	brg,r6
	brc	false
	br	true
/*
 * TSTGEQI:  AC = (AC>=Y)
 */
tstgeqi:
	sub	brg,r6
	brc	true
	br	false
/*
 * TSTLEQI:  AC = (AC<=Y)
 */
tstleqi:
	addn	brg,r6
	brc	false
	br	true
/*
 * MOVM:  MEM[Y] = AC
 */
movm:
	mov	r6,mem
	br	disp
/*
 * CLR:  MEM[Y] = 0
 */
clrm:
	mov	0,mem
	br	disp
/*
 * ORM:  AC = MEM[Y] |= AC
 */
orm:
	or	mem,r6
	br	movm
/*
 * ANDM:  AC = MEM[Y] &= AC
 */
andm:
	and	mem,r6
	br	movm
/*
 * XORM:  AC = MEM[Y] ^= AC
 */
xorm:
	xor	mem,r6
	br	movm
/*
 * INC:  AC = ++MEM[Y]
 */
incm:
	mov	mem,r6
	inc	r6
	br	movm
/*
 * DEC:  AC = --MEM[Y]
 */
decm:
	mov	mem,r6
	dec	r6
	br	movm
/*
 * BICM:  AC = MEM[Y] &= ~AC
 */
bicm:
	mov	~0,brg
	xor	brg,r6		/* r6 = ~AC */
	and	mem,r6
	br	movm
/*
 * ADDM:  AC = MEM[Y] += AC
 */
addm:
	add	mem,r6
	br	movm
/*
 * SUBM:  AC = MEM[Y] -= AC
 */
subm:
	sub	mem,r6
	br	movm
/*
 * CRCLOC:	Set crcsav = Y, then  set MEM[Y] = MEM[Y+1] = 0.
 */
crcloc:
	mov	0,mem|mar++
	mov	0,mem
	mov	crcsav,mar
	mov	%crcsav,%mar
	mov	r5,mem
	br	disp
/*
 * CRC16:  Combine the character in MEM[Y] with the CRC remainder
 *	   in MEM[crcsav], MEM[crcsav+1]
 */
crc16:
/*
 * Get the character
 */
	mov	mem,brg
/*
 * Merge with code for crc16i
 */
	br	crc16i
/*
 * CRC16I:  Combine Y with the CRC remainder
 *	   in MEM[CRC0], MEM[CRC0+1]
 */
crc16i:
/*
 * Note:  The following code calculates CRC-16
 *	 (x**0 + x**2 + x**15 + x**16).
 *
 *	 The algorithm processes one data byte in parallel.
 */
	mov	brg,r3
	mov	crcsav,mar
	mov	%crcsav,%mar
	mov	mem,brg|r5|mar
	dec	r5,-
	brz	crcerr
	mov	%regs,%mar
	xor	mem,r3
	mov	r3,mem
	asl	r3
	xor	mem,r3,r3|brg|mar++
	mov	brg,brg>>
	mov	brg,brg>>
	mov	3<<6,r0
	and	brg,r0
	xor	brg,r0,brg
	mov	brg,r4
	xor	brg,r3,brg
	mov	brg,r2
	mov	0,brg>>
	mov	0,brg>>
	mov	0,brg>>
	mov	0,brg>>
	xor	brg,r2
	mov	3,brg
	and	brg,r2,r2|brg
	mov	brg,brg>>
	mov	1,r1
	and	brg,r1
	mov	brg,brg>>
	xor	brg,r4
	xor	mem,r0,brg
	xor	brg,r1
	mov	r4,mem
	mov	r5,mar
	mov	r1,mem
	br	disp
/*
 * TESTOP:  Set AC = 1 if MEM[Y] has odd parity, otherwise
 *	      set AC = 0.
 */
testop:
	mov	mem,r6|brg
	mov	0,brg>>
	xor	brg,r6,r6|brg
	mov	0,brg>>
	mov	0,brg>>
	xor	brg,r6,r6|brg
	mov	0,brg>>
	mov	0,brg>>
	mov	0,brg>>
	mov	0,brg>>
	xor	brg,r6,r6|brg
	br0	true
	br	false
/*
 * RCV:  Set MEM[Y] = next received character; set AC = flags.
 *
 * Here we just save Y and set the process state = INPUT WAIT.
 * The real work is done following label 'inwait'.
 */
rcv:
	mov	ioadr,mar
	mov	%ioadr,%mar
	mov	r5,mem
	mov	INWAIT,brg
	mov	brg,r12
	br	disp
/*
 * XMT:  Transmit MEM[Y]; set AC = flags.
 */
xmt:
	mov	mem,r5
	br	xmti
/*
 * XMTI:  Transmit Y; set AC = flags.
 *
 * Here we just save Y and set the process state = OUTPUT WAIT.
 * The real work is done following label 'outwait'.
 */
xmti:
	mov	ioadr,mar
	mov	%ioadr,%mar
	mov	r5,mem
	mov	OUTWAIT,brg
	mov	brg,r12
	br	disp
/*
 * XSOMI:	Clear the transmitter, then send Y as the
 *		sync character.
 */
xsomi:
	mov	OCLRP,brg
	mov	brg,lur1
	mov	TSOM,brg
	mov	TSOM,brg
	mov	TSOM,brg
	mov	TSOM,brg
	mov	TSOM,brg
	mov	TSOM,brg
	mov	TSOM,brg
	mov	brg,lur1
	mov	r5,brg
	mov	brg,lur0
	mov	0,r0
	mov	r0,lur1
/*
 * Send six or seven sync characters
 */
	mov	r5,brg
	mov	brg,lur0
	mov	r5,brg
	mov	brg,lur0
	mov	r5,brg
	mov	brg,lur0
	mov	r5,brg
	mov	brg,lur0
	mov	r5,brg
	mov	brg,lur0
	mov	r5,brg
	mov	brg,lur0
	mov	r5,brg
/*
 * Go to disp
 */
	br	disp
/*
 * XEOMI:	Send Y, then shut down the transmitter.
 */
xeomi:
/*
 * Set XLAST
 */
	mov	XLAST,brg
	or	brg,r14
/*
 * Merge with code for xmti
 */
	br	xmti
/*
 * RSOMI:	Clear the receiver, then set Y as the
 *		receive sync character.
 */
rsomi:
	mov	ICLRP,brg
	mov	brg,lur2
	mov	r5,brg
	mov	r5,brg
	mov	r5,brg
	mov	r5,brg
	mov	r5,brg
	mov	r5,brg
	mov	brg,lur4
	br	disp
/*
 * GETXBUF: Open the transmit buffer;
 *	    set MEM[Y] = RTYPE;
 *	    set MEM[Y+1] = RDEV;
 *	    set MEM[Y+2] = RSTA
 */
getxbuf:
/*
 * Get buffer state
 */
	mov	x_state,mar
	mov	%x_state,%mar
	mov	mem,r0
/*
 * If buffer is already open then go to getxbuf2
 */
	mov	OPEN,brg
	addn	brg,r0,-
	brz	getxbuf2
/*
 * If there is not a transmit buffer waiting then
 * return a non-zero status
 */
	mov	x_state,mar
	mov	%x_state,%mar
	mov	mem,r0
	mov	HAVEBUF,brg
	addn	brg,r0
	brz	1f
	br	true
1:
/*
 * Set buffer state = OPEN
 */
	mov	OPEN,mem
/*
 * Copy the buffer address into cp
 */
getxbuf2:
	mov	x_bufadr,mar
	mov	mem,r0|brg|mar++
	mov	mem,r1|mar++
	mov	mem,r2
	mov	x_cp,mar
	mov	r0,mem|mar++
	mov	r1,mem|mar++
	mov	r2,mem|mar++
/*
 * If buffer address is odd then go to xbuferr
 */
	br0	xbuferr
/*
 * Set npc = 0
 */
	mov	x_npc,mar
	mov	0,mem|mar++
	mov	0,mem
/*
 * Copy type, station, and device to user data area
 */
#ifdef	XBP
	mov	x_type,mar
	mov	mem,r0|mar++
	mov	mem,r1|mar++
	mov	mem,r2
	mov	r5,mar
	mov	%regs,%mar
	mov	r0,mem|mar++
	mov	r1,mem|mar++
	mov	r2,mem
#endif
/*
 * Clear the AC, then go to disp
 */
	br	false
/*
 * GETRBUF: Open the receive buffer, if any;
 *	    set MEM[Y] = RTYPE;
 *	    set MEM[Y+1] = RDEV;
 *	    set MEM[Y+2] = RSTA
 */
getrbuf:
/*
 * Get buffer state
 */
	mov	r_state,mar
	mov	%r_state,%mar
	mov	mem,r0
/*
 * If buffer is already open then go to getrbuf2
 */
	mov	OPEN,brg
	addn	brg,r0,-
	brz	getrbuf2
/*
 * If there is not a receive buffer waiting then
 * return a non-zero status
 */
	mov	HAVEBUF,brg
	addn	brg,r0
	brz	1f
	br	true
1:
/*
 * Set buffer state = OPEN
 */
	mov	OPEN,mem
/*
 * Copy the buffer address into cp
 */
getrbuf2:
	mov	r_bufadr,mar
	mov	mem,r0|mar++
	mov	mem,r1|mar++
	mov	mem,r2
	mov	r_cp,mar
	mov	r0,mem|mar++
	mov	r1,mem|mar++
	mov	r2,mem|mar++
/*
 * Set npc = 0
 */
	mov	r_npc,mar
	mov	0,mem|mar++
	mov	0,mem
/*
 * Copy type, station, and device to user data area
 */
#ifdef	RBP
	mov	r_type,mar
	mov	mem,r0|mar++
	mov	mem,r1|mar++
	mov	mem,r2
	mov	r5,mar
	mov	%regs,%mar
	mov	r0,mem|mar++
	mov	r1,mem|mar++
	mov	r2,mem
#endif
/*
 * Clear the AC, then go to disp
 */
	br	false
/*
 * RSXBUF:  Reset the transmit buffer
 *
 * The buffer status is restored to the state just prior
 * to the last GETXBUF
 */
rsxbuf:
/*
 * If transmit buffer is not open then error
 */
	mov	x_state,mar
	mov	%x_state,%mar
	mov	mem,r0
	mov	OPEN,brg
	addn	brg,r0
	brz	1f
	br	xbuferr
1:
/*
 * Set buffer state = ready to be opened
 */
	mov	HAVEBUF,mem
/*
 * Go to disp
 */
	br	disp
/*
 * RSRBUF:  Reset the transmit buffer
 *
 * The buffer status is restored to the state just prior
 * to the last GETRBUF
 */
rsrbuf:
/*
 * If the receive buffer is not open then error
 */
	mov	r_state,mar
	mov	%r_state,%mar
	mov	mem,r0
	mov	OPEN,brg
	addn	brg,r0
	brz	1f
	br	rbuferr
1:
/*
 * Set buffer state = ready to be opened
 */
	mov	HAVEBUF,mem
/*
 * Go to disp
 */
	br	disp
/*
 * GETBYTE:  Set MEM[Y] = next byte from transmit buffer;
 *	     set AC = flags.
 */
getbyte:
/*
 * If transmit buffer is not open then error
 */
	mov	x_state,mar
	mov	%x_state,%mar
	mov	mem,r0
	mov	OPEN,brg
	addn	brg,r0
	brz	1f
	br	xbuferr
1:
/*
 * If there are no characters remaining then go to geterr1
 */
	mov	x_npc,mar
	mov	%x_npc,%mar
	mov	mem,r0|mar++
	mov	mem,r1
	mov	x_count,mar
	sub	mem,r0,mar++
	subc	mem,r1,-
	brc	geterr1
/*
 * Increment the count of fetched characters
 */
	mov	x_npc,mar
	inc	r0,mem|mar++
	adc	r1,mem
/*
 * If the count is odd then go to gettwo
 */
	mov	x_npc,mar
	mov	mem,brg
	br0	gettwo
/*
 * Copy the saved byte to the specified address
 */
	mov	x_save,mar
	mov	mem,r0
	mov	%regs,%mar
	mov	r5,mar
	mov	r0,mem
/*
 * Clear the AC, then go to disp
 */
	br	false
/*
 * There is no saved byte from a previous fetch.  Therefore
 * we need to fetch two more characters from the pdp-11.
 */
gettwo:
/*
 * Set up a unibus request for the next two bytes
 */
	mov	x_cp,mar
	mov	mem,ial|mar++
	mov	mem,iah|mar++
	mov	3,r3
	and	mem,r3
	asl	r3
	asl	r3
	mov	NRQ,brg
	or	brg,r3,brg
	mov	brg,npr
/*
 * Wait for the unibus transfer to complete
 */
	BUSWAIT
/*
 * Increment the buffer pointer
 */
	mov	x_cp,mar
	mov	2,r2
	add	mem,r2,mem|mar++
	mov	mem,r0
	adc	r0,mem|mar++
	mov	mem,r0
	adc	r0,mem
/*
 * Save the high-order byte
 */
	mov	x_save,mar
	mov	idh,mem
/*
 * Store the low-order byte at the specified address
 */
	mov	r5,mar
	mov	%regs,%mar
	mov	idl,mem
/*
 * Clear the AC, then go to disp
 */
	br	false
/*
 * No characters remaining
 */
geterr1:
	mov	NOMORE,brg
	mov	brg,r6
	br	disp

/*
 * PUTBYTE:  Put MEM[Y] into the next available position
 *	     in the receive buffer
 */
putbyte:
	mov	mem,r5
	br	putbytei
/*
 * PUTBYTEI:  Put Y into the next available position
 *	      in the receive buffer
 */
putbytei:
/*
 * If the receive buffer is not open then error
 */
	mov	r_state,mar
	mov	%r_state,%mar
	mov	mem,r0
	mov	OPEN,brg
	addn	brg,r0
	brz	1f
	br	rbuferr
1:
/*
 * If there is no room for another character then go to puterr1
 */
	mov	r_npc,mar
	mov	%r_npc,%mar
	mov	mem,r0|mar++
	mov	mem,r1
	mov	r_count,mar
	sub	mem,r0,mar++
	subc	mem,r1,-
	brc	puterr1
/*
 * Increment the count of stored characters
 */
	mov	r_npc,mar
	inc	r0,mem|mar++
	adc	r1,mem
/*
 * If the count is odd then go to putlow
 */
	inc	r0,brg
	br0	putlow
/*
 * Set up a unibus request to move two bytes to the pdp-11
 */
	mov	r_save,mar
	mov	mem,odl
	mov	r5,brg
	mov	brg,odh
	mov	r_cp,mar
	mov	mem,oal|mar++
	mov	mem,oah|mar++
	mov	3,r3
	and	mem,r3
	asl	r3
	asl	r3
	mov	BRQ|VEC4|PCLK,brg
	mov	nprx,r0
	and	brg,r0,nprx
	mov	r3,brg
	mov	nprx,r0
	or	brg,r0,nprx
	mov	OUT|NRQ,brg
	mov	brg,npr
/*
 * Wait for the unibus transfer to complete
 */
	BUSWAIT
/*
 * Increment the buffer pointer
 */
	mov	r_cp,mar
	mov	2,r2
	add	mem,r2,mem|mar++
	mov	mem,r0
	adc	r0,mem|mar++
	mov	mem,r0
	adc	r0,mem
/*
 * Clear the AC, then go to disp
 */
	br	false
/*
 * Here we just save the specified byte for transfer later
 */
putlow:
/*
 * Copy the specified byte to the save area
 */
	mov	r_save,mar
	mov	r5,mem
/*
 * Clear the AC, then go to disp
 */
	br	false
/*
 * No room for this character
 */
puterr1:
/*
 * Put error code in AC
 */
	mov	NOROOM,brg
	mov	brg,r6
/*
 * Go to disp
 */
	br	disp
/*
 * RTNXBUF:  Return a transmit buffer:
 *	     Set XTYPE = MEM[Y];
 *	     send a RTNXBUF signal to the driver
 */
rtnxbuf:
/*
 * If the transmit buffer is not open then error
 */
	mov	x_state,mar
	mov	%x_state,%mar
	mov	mem,r0
	mov	OPEN,brg
	addn	brg,r0
	brz	1f
	br	xbuferr
1:
/*
 * Set buffer state = CLOSED
 */
	mov	CLOSED,mem
/*
 * Get the new value for XTYPE
 */
#ifdef	XBP
	mov	r5,mar
	mov	%regs,%mar
	mov	mem,r0
	mov	x_type,mar
	mov	%x_type,%mar
	mov	r0,mem
#endif
/*
 * Schedule a RTNXBUF report
 */
	mov	1<<RRTNXBUF,brg
	br	rptschd
/*
 * RTNRBUF:  Return a receive buffer:
 *	     Set RTYPE = MEM[Y];
 *	     send a RTNRBUF signal to the driver
 */
rtnrbuf:
/*
 * If the receive buffer is not open then error
 */
	mov	r_state,mar
	mov	%r_state,%mar
	mov	mem,r0
	mov	OPEN,brg
	addn	brg,r0
	brz	1f
	br	rbuferr
1:
/*
 * Set buffer state = CLOSED
 */
	mov	CLOSED,mem
/*
 * Get the count of stored characters
 * If the count is even then go to rtnrbuf2
 */
	mov	r_npc,mar
	mov	%r_npc,%mar
	mov	mem,brg
	br0	1f
	br	rtnrbuf2
1:
/*
 * Set up a unibus request to move two bytes to the pdp-11
 */
	mov	r_save,mar
	mov	mem,odl
	mov	r5,brg
	mov	brg,odh
	mov	r_cp,mar
	mov	mem,oal|mar++
	mov	mem,oah|mar++
	mov	3,r3
	and	mem,r3
	asl	r3
	asl	r3
	mov	BRQ|VEC4|PCLK,brg
	mov	nprx,r0
	and	brg,r0,nprx
	mov	r3,brg
	mov	nprx,r0
	or	brg,r0,nprx
	mov	OUT|NRQ,brg
	mov	brg,npr
/*
 * Wait for the unibus transfer to complete
 */
	BUSWAIT
/*
 * Branch to here from above if count is even
 */
rtnrbuf2:
/*
 * Get the new value for RTYPE
 */
#ifdef	RBP
	mov	r5,mar
	mov	%regs,%mar
	mov	mem,r0
	mov	r_type,mar
	mov	%r_type,%mar
	mov	r0,mem
#endif
/*
 * Schedule a RTNRBUF report
 */
	mov	1<<RRTNRBUF,brg
	br	rptschd
/*
 * MWAIT:  Wait for modem-ready
 */
dsrwait:
	mov	WAITDSR,brg
	mov	brg,r12
	br	disp
/*
 * EXIT:  Terminate execution and send an EXIT signal
 *	  to the PDP-11 with MEM[Y] as a parameter.
 */
exit:
	mov	mem,r5
	br	exiti
/*
 * EXITI:  Terminate execution and send an EXIT signal
 *	   to the PDP-11 with Y as a parameter.
 */
exiti:
/*
 * Load the reason for termination
 */
	mov	errcode,mar
	mov	%errcode,%mar
	mov	r5,mem
	mov	EXITINST,brg
/*
 * Go to errterm
 */
	br	errterm
/*
 * TRACE:  Send an TRACE signal to the driver
 *	    with MEM[Y] as a parameter.
 */
trace:
	mov	mem,r5
	br	tracei
/*
 * TRACEI:  Send an TRACE signal to the driver
 *	    with Y as a parameter.
 */
tracei:
	mov	traceval,mar
	mov	%traceval,%mar
	mov	r5,mem
/*
 * Schedule a TRACE report
 */
	mov	1<<RTRACE,brg
	br	rptschd
/*
 * TIMEOUT:   If(Y==0) then cancel any previous time-out request;
 *	     else{
 *	     save stack pointer and frame pointer;
 *	     set TPC = address of next instruction;
 *	     set timer = Y;
 *	     set AC = 0;
 *	     }
 *	     When the timer expires, restore the saved
 *	     stack pointer and frame pointer, set AC = 1,
 *	     and set PC = TPC.
 *	     A return from the frame which was active at
 *	     the time the time-out request was made
 *	     will cancel the time-out request.
 *
 * Note:  The timeout parameter passed via the AC is the number
 *	  of 100-millisecond increments until the timeout occurs.
 */
timeout:
/*
 * If Y == 0 then go to cancel
 */
	dec	r5,-
	brz	cancel
/*
 * Point to the save area
 */
	mov	toutsave,mar
	mov	%toutsave,%mar
/*
 * Save the stack pointer and the frame pointer
 */
	mov	r10,mem|mar++
	mov	r11,mem|mar++
/*
 * Save the address of the next instruction
 */
	mov	r8,mem|mar++
	mov	r9,mem|mar++
/*
 * Set the 100-millisecond software timer
 *
 * (100 milliseconds = 2000 ticks of the 50-microsecond hardware
 * timer; 2000 = 256*7 + 208)
 */
	mov	clock,mar
	mov	208,mem|mar++
	mov	7,mem
/*
 * Put the timeout interval in clock1
 */
	mov	clock1,mar
	dec	r5,mem
/*
 * Set the timeout-in-progress flag
 */
	mov	TOUT,brg
	or	brg,r14
/*
 * Clear the AC, then go to disp
 */
	br	false
/*
 * Cancel the current timeout request, if any
 */
cancel:
	mov	~TOUT,brg
	and	brg,r14
/*
 * Clear the AC, then go to disp
 */
	br	false
/*
 * CALL:  DPUSH(PC); JMP X
 */
pushj:
/*
 * If this will cause a stack overflow then error
 */
	dec	r10
	brc	1f
	br	stackerr
1:
/*
 * Push PC
 */
	dec	r10,r10|mar
	mov	r8,mem|mar++
	mov	r9,mem
/*
 * Go to jump
 */
	br	jump
/*
 * RETURN:  DPOP(PC); AC = MEM[Y]
 */
popj:
/*
 * Put value to be returned in AC
 */
	mov	mem,brg
	br	popji
/*
 * RETURN:  DPOP(PC); AC = MEM[Y]
 */
popji:
/*
 * Put value to be returned in AC
 */
	mov	brg,r6
/*
 * Pop saved PC
 */
	mov	r10,mar
	mov	mem,r0|mar++
	mov	mem,r1
	mov	2,brg
	add	brg,r10
/*
 * If there is a stack underflow then error
 */
	mov	SSTACK,brg
	addn	brg,r10,-
	brc	stackerr
/*
 * If there is a timeout in progress in the routine
 * we are returning from then cancel the timeout 
 */
	mov	TOUT,brg
	orn	brg,r14,-
	brz	1f
	br	2f
1:
	mov	toutsave,mar
	mov	%toutsave,%mar
	sub	mem,r10,-
	brc	2f
	mov	~TOUT,brg
	and	brg,r14
2:
/*
 * Go to jump
 */
	br	jump
/*
 * JMPNEZ:  if(AC!=0)PC = X
 */
jumpnez:
	dec	r6,-
	brc	jump
	br	disp
/*
 * JMPEQZ:  if(AC==0)PC = X
 */
jumpeqz:
	dec	r6,-
	brz	jump
	br	disp
/*
 * JMP:  Set PC = X
 *
 * Entered with r0 = bits 7-0 of the jump address,
 * r1 = bits 11-8 of the jump address.
 */
jump:
/*
 * If the jump address is not even then error
 */
	mov	r0,brg
	br0	jumperr
/*
 * Set PC = jump address
 * (low-order bits in r8, page bits in r9)
 */
	mov	brg,r8
	mov	3,brg
	and	brg,r1,brg
	mov	brg,r9
/*
 * Go to disp
 */
	br	disp
/*
 * Set AC = TRUE
 */
true:
	mov	1,brg
	mov	brg,r6
	br	disp
/*
 * Set AC = FALSE
 */
false:
	mov	0,brg
	mov	brg,r6
	br	disp
/*
 * Undefined op code
 */
illegal:
	mov	ILLEGAL,brg
	br	errterm
/*
 * PC value out of bounds
 */
fetcherr:
	mov	FETCHERR,brg
	br	errterm
/*
 * Jump address is not even
 */
jumperr:
	mov	JMPERR,brg
	br	errterm
/*
 * Stack overflow or underflow
 */
stackerr:
	mov	STACKERR,brg
	br	errterm
/*
 * Transmit-buffer state error
 */
xbuferr:
	mov	XBUFERR,brg
	br	errterm
/*
 * Receive-buffer state error
 */
rbuferr:
	mov	RBUFERR,brg
	br	errterm
/*
 * UNIBUS error
 */
buserr:
	mov	BUSERR,brg
	br	errterm
/*
 * crc location not specified
 */
crcerr:
	mov	CRCERR,brg
	br	errterm
/*
 * Terminate the virtual process
 */
errterm:
/*
 * Save the reason for termination
 */
	mov	termcode,mar
	mov	%termcode,%mar
	mov	brg,mem
/*
 * Set process state = IDLE
 */
	mov	IDLE,brg
	mov	brg,r12
/*
 * Clear the timeout-in-progress flag
 */
	mov	~TOUT,brg
	and	brg,r14
/*
 * Schedule an error-termination report
 */
	mov	1<<ERRTERM,brg
	br	rptschd
/*
 * Error loops for debugging
 */
e1:
	mov	brg,r15
	mov	e1,brg
	br	.
/*
 * The following loop effectively halts the kmc
hang:
	mov	brg,r4
	br	.
 */
