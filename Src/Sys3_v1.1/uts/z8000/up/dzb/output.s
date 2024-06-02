/*
 * Output segment
 */
	.org	1024
seg1:
/*
 * The code in this segment is entered via label 'outreq' when it is
 * recognized that a DZ11 has requested an output character
 */
statesw:
	br	disable
	br	odrst1
	br	odrst2
	br	odrst3
	br	odrst4
	br	odrst5
	ERROR(INVALCD)
	br	.
/*
 * The following table of jump instructions is accessed by a
 * calculated branch instruction using the ASCII character
 * as index.  It provides a fast implementation of tests
 * for specific control characters
 */
charsw:
	br	sendit		/* 0 - NUL */
	br	sendit		/* 1 - SOH */
	br	sendit		/* 2 - STX */
	br	sendit		/* 3 - ETX */
	br	sendit		/* 4 - EOT */
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
 * Entered here from main segment
 */
outreq:
/*
 * The DZ11 has requested an output character.  Get the line
 * number and look up the address of the line-table entry.
 * Save the address of the line-table entry in register r10.
 */
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
 * If a break is being sent then go to disable
 */
	mov	OUTBRK,brg
	orn	brg,r0,-
	brz	disable
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
	mov	%dzst,brg
	add	brg,r8,%mar
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
	mov	OUTBUF,brg
	add	brg,r10,mar
	mov	mem,r4
	brz	nomore
/*
 * Get the buffer address from the queue entry
 */
	mov	03,brg
	and	brg,r4,%mar
	mov	~03,brg
	and	brg,r4,mar
	mov	0,r0|mar++
	mov	mem,r3|mar++
	mov	mem,r2|mar++
	mov	mem,r1
	mov	3,brg
	and	brg,r1
/*
 * Set the page register to the page for this DZ11
 */
	mov	%dzst,brg
	add	brg,r8,%mar
/*
 * Get the index of the first and last characters
 */
	mov	C_FIRST,brg
	add	brg,r3,brg
	mov	brg,ial
	adc	r2,brg
	mov	brg,iah
	adc	r1,brg
	mov	brg,r0
	mov	3,brg
	and	brg,r0
	asl	r0
	asl	r0
	mov	NRQ,brg
	or	brg,r0,npr
	BUSWAIT(buserr1)
/*
 * Compute the character count - 1 for the new output
 * buffer and record the result as the remaining character
 * count for the current buffer.
 */
	mov	NCH,brg
	add	brg,r10,mar
	mov	idh,r0
	mov	idl,brg
	sub	brg,r0
	dec	r0
	mov	r0,mem
/*
 * If there was only one character in this buffer release it
 */
	dec	r0,-
	brc	1f
	CALL(oqueue)
	mov	%dzst,brg
	add	brg,r8,%mar
1:
/*
 * Set bufad = address of the first character
 */
	mov	BUFAD,brg
	add	brg,r10,mar
	mov	idl,r0
	mov	C_DATA,brg
	add	brg,r0,brg
	add	brg,r3,mem|mar++
	adc	r2,mem|mar++
	adc	r1,mem
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
	BUSWAIT(buserr1);
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
	mov	%dzst,brg
	add	brg,r8,%mar
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
 * If an error occurs then go to buserr1.
 */
	BUSWAIT(buserr1)
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
	mov	csrget,mar
	mov	mem,r0|mar++
	mov	mem,r1|mar++
	mov	mem,r2
	mov	6,brg
	add	brg,r0,oal
	adc	r1,brg
	mov	brg,oah
	adc	r2
	mov	nprx,r0
	mov	VEC4,brg
	and	brg,r0
	asl	r2
	asl	r2,brg
	or	brg,r0,nprx
	mov	BYTE|OUT|NRQ,brg
	mov	brg,npr
/*
 * Wait for the unibus transfer to complete.
 * If an error occurs then go to buserr1.
 */
	BUSWAIT(buserr1)
/*
 * Go to dispret
 */
	br	dispret
/*
 * Set the break bit in the dz.  This code is entered from
 * cmdin after output is drained.
 */
break:
/*
 * Set up a unibus request to set the break bit
 */
	mov	enblbits,brg
	add	brg,r7,mar
	mov	%enblbits,%mar
	mov	mem,r0
	mov	brkbits,mar
	mov	%dzst,brg
	add	brg,r8,%mar
	or	mem,r0,mem
	mov	mem,odh
	mov	csrget,mar
	mov	mem,r0|mar++
	mov	mem,r1|mar++
	mov	mem,r2
	mov	7,brg
	add	brg,r0,oal
	adc	r1,brg
	mov	brg,oah
	adc	r2
	mov	nprx,r0
	mov	VEC4,brg
	and	brg,r0
	asl	r2
	asl	r2,brg
	or	brg,r0,nprx
	mov	BYTE|OUT|NRQ,brg
	mov	brg,npr
/*
 * Wait for the unibus transfer to complete.
 * If an error occurs then go to buserr1.
 */
	BUSWAIT(buserr1)
/*
 * Set OUTBRK
 */
	mov	FLAGS1,brg
	add	brg,r10,mar
	mov	mem,r0
	mov	OUTBRK,brg
	or	brg,r0,mem
/*
 * Delay for 250 milliseconds before turning off the break bit
 */
	mov	COUNT,brg
	add	brg,r10,mar
	mov	15,mem
/*
 * Set state = 5
 */
	mov	STATE,brg
	add	brg,r10,mar
	mov	5,mem
/*
 * Return to caller 
 */
	RETURN

/*
 * The following code is used to turn off the break bit
 */
ubreak:
/*
 * Clear the KMC copy of the break bit for this line
 * and set up a unibus request to clear the corresponding bit
 * in the DZ11
 */
	mov	enblbits,brg
	add	brg,r7,mar
	mov	%enblbits,%mar
	mov	mem,r0
	mov	brkbits,mar
	mov	%dzst,brg
	add	brg,r8,%mar
	and	mem,r0
	xor	mem,r0,mem
	mov	mem,odh
	mov	csrget,mar
	mov	mem,r0|mar++
	mov	mem,r1|mar++
	mov	mem,r2
	mov	7,brg
	add	brg,r0,oal
	adc	r1,brg
	mov	brg,oah
	adc	r2
	mov	nprx,r0
	mov	VEC4,brg
	and	brg,r0
	asl	r2
	asl	r2,brg
	or	brg,r0,nprx
	mov	BYTE|OUT|NRQ,brg
	mov	brg,npr
/*
 * Wait for the unibus transfer to complete.
 * If an error occurs then go to buserr1.
 */
	BUSWAIT(buserr1)
/*
 * Reset OUTBRK
 */
	mov	FLAGS1,brg
	add	brg,r10,mar
	mov	mem,r0
	mov	~OUTBRK,brg
	and	brg,r0,mem
/*
 * Schedule a delay for uart to reset
 */
	mov	COUNT,brg
	add	brg,r10,mar
	mov	2,mem
/*
 * Return to caller 
 */
	RETURN

/*
 * Subroutine to enable a line
 */
enable:
/*
 * Set up a unibus request to enable the line
 */
	mov	enblbits,brg
	add	brg,r7,mar
	mov	%enblbits,%mar
	mov	mem,r0
	mov	actline,mar
	mov	%dzst,brg
	add	brg,r8,%mar
	or	mem,r0,mem
	mov	mem,odl
	mov	csrget,mar
	mov	mem,r0|mar++
	mov	mem,r1|mar++
	mov	mem,r2
	mov	4,brg
	add	brg,r0,oal
	adc	r1,brg
	mov	brg,oah
	adc	r2
	mov	nprx,r0
	mov	VEC4,brg
	and	brg,r0
	asl	r2
	asl	r2,brg
	or	brg,r0,nprx
	mov	BYTE|OUT|NRQ,brg
	mov	brg,npr
/*
 * Wait for the unibus transfer to complete.
 * If an error occurs then go to buserr0.
 */
	BUSWAIT(buserr1)
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
	mov	enblbits,brg
	add	brg,r7,mar
	mov	%enblbits,%mar
	mov	mem,r0
	mov	actline,mar
	mov	%dzst,brg
	add	brg,r8,%mar
	and	mem,r0
	xor	mem,r0,mem
	mov	mem,odl
	mov	csrget,mar
	mov	mem,r0|mar++
	mov	mem,r1|mar++
	mov	mem,r2
	mov	4,brg
	add	brg,r0,oal
	adc	r1,brg
	mov	brg,oah
	adc	r2
	mov	nprx,r0
	mov	VEC4,brg
	and	brg,r0
	asl	r2
	asl	r2,brg
	or	brg,r0,nprx
	mov	BYTE|OUT|NRQ,brg
	mov	brg,npr
/*
 * Wait for the unibus transfer to complete.
 * If an error occurs then go to buserr1.
 */
	BUSWAIT(buserr1)
/*
 * Go to dispret
 */
	br	dispret

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
	mov	%dzst,brg
	add	brg,r8,%mar
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
/*
 * Set state = 0
 */
	mov	STATE,brg
	add	brg,r10,mar
	mov	0,mem
/*
 * See if there is an output buffer
 */
oqueue:
	mov	%dzst,brg
	add	brg,r8,%mar
	mov	OUTBUF,brg
	add	brg,r10,mar
	mov	mem,r4
	brz	oqret
	mov	NIL,mem
	mov	03,brg
	and	brg,r4,%mar
	mov	~03,brg
	and	brg,r4,mar
/*
 * Set link to next queue entry = NIL
 */
	mov	NIL,mem|mar++
/*
 * Chain this queue entry to the end of the output-buffer-completion-
 * report queue
 */
	mov	tailoutq,mar
	mov	%tailoutq,%mar
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
	mov	headoutq,mar
	mov	r4,mem
2:
/*
 * Set a flag to indicate that the output-buffer-completion-report
 * queue is non-empty
 */
	mov	CHKOUTQ,brg
	or	brg,r14
oqret:
	RETURN

/*
 * The unibus transfer failed to complete within a reasonable time
 */
buserr1:
/*
 * Clear the non-existent memory bit
 */
	mov	nprx,r0
	mov	~(BRQ|ACLO|NEM),brg
	and	brg,r0,nprx
	ERROR(BUSERR1)
/*
 * Go to dispret
 */
	br	dispret
/*
 * Return to top of dispatcher loop in main segment
 */
dispret:
	mov	%disp,brg
	mov	brg,pcr
	jmp	disp
/*
 * Error loops for debugging
 */
errh:
	mov	NIL,mem
	ERROR(ERRH)
	br	dispret
endseg1:
