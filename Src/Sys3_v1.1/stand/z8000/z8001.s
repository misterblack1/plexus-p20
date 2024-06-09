#include "/usr/include/sys/plexus.h"

/************************************************************************/
/* This module contains routines for the Z8001 processor that are for	*/
/*	one reason or another written in assembly language.  This	*/
/*	module is used by a number of programs and should not be	*/
/*	modified without extreme care and thorough testing.  To use	*/
/*	the routines, one needs to create a file named "defines" that	*/
/*	contains the names of those portions that are necessary for the	*/
/*	given application.  The following portions are currently	*/
/*	available:							*/
/*									*/	
/* 		STARTUP		Z8001 startup code (including PSA)	*/ 
/*		VECTINTS	Specifies user defined vectored intrpts */
/*		TRAP		Trap catcher				*/
/*		CONTINUE	Continue and jump code			*/
/*		INITCPU		Initializes CPU board 			*/
/*		INITUSART	Initializes CPU usart 			*/
/*		INITIMSC	Initializes IMSC board 			*/
/*		INITSIO		Initializes IMSC sio 			*/
/*		SIOC		Allows terminal I/O to SIOC if no usart	*/
/*		INTENABLE	Enables (disables) vi, nvi		*/
/*		CPUCHARIO	Input, output of characters to terminal */
/*		IMSCCHARIO	Input, output of characters to terminal */
/*		CSV		Hookup to "C" routines			*/
/*		PIOA		Output value to PIOA with control info	*/
/*		MAP		Turns on CPU mapping			*/
/*		MBUS		Turns on CPU multibus			*/
/*		ECC		Turns on ECC (CPU)			*/
/*		ABSJUMP		Jumps to specified location		*/
/*		IOINST		Interface to sin, sout, in, out		*/
/*		MEMLD		Word memory write, read			*/
/*		BMEMLD		Byte memory write, read			*/
/*		CLRPAGE		Clears specified page in memory (CPU)	*/
/*		BLKMOVE		Memory block move			*/
/*		BLKCMP		Memory block compare			*/
/*		CLRBSS		Clears bss area (between RAM and ENDRAM)*/
/*		SPIN		Spins for specified 1/60ths of a second	*/
/*		CPULEDS		Turns leds on and off for the CPU	*/
/*		IMSCLEDS	Turns leds on and off for the IMSC	*/
/*									*/
/************************************************************************/


#ifdef STARTUP
/* This section of code needs THISSEG, DATE and VERSION defined in defines */
.globl _start		/* Location zero of this module */
.globl _restart		/* Good place to restart (jumps to main) */
.globl _main		/* Entry point of "C" portion of selftest */
.globl _exins		/* Extended instruction trap */
.globl _privins		/* Privileged instruction trap */
.globl _syscall		/* System call */
.globl _segtrap		/* Segment trap */
.globl _nmint		/* Non-maskable interrupt */
.globl _nvint		/* Non-vetctored interrupt */

.text
.org	0
_start:
	jr	init		/* Should not get here normally */
	.word	0x4800		/* FCW -- System mode, non-segmented, vi, nvi */
	.word	0		/* Segment zero */
	.word	init		/* Startup code for PROM */
	.word	0,0x4800,THISSEG,_exins		/* Extended instruction trap */
	.word	0,0x4800,THISSEG,_privins	/* Priveleged instruction trap */
	.word	0,0x4800,THISSEG,_syscall	/* System call */
	.word	0,0x4800,THISSEG,_segtrap	/* Segment trap */
	.word	0,0x4800,THISSEG,_nmint		/* Non-maskable interrupt */
	.word	0,0x4000,THISSEG,_nvint		/* Non-vectored interrupt */
#ifndef VECTINTS
.globl _vectint		/* First vectored interrupt */
	.word	0,0x4800,THISSEG,_vectint	/* First vectored interrupt */
#else
#include VECTINTS
#endif
	
_version:
	.ascii	VERSION 	/* Version " AA-## " */
	.asciiz	DATE		/* Date "MM/DD/YY" */
	
.even
init:
	ldctl	r0,FCW		/* Change to segmented mode */
	set	r0,$S_SEG
	ldctl	FCW,r0
	ldar	r0,_start
	ld	r14,r0		/* Set sys stack pointer segment to current */
	ldctl	PSAPSEG,r0	/* Segment of Program Status Area (0) */
	ldctl	PSAPOFF,r1	/* Offset of Program Status Area (0) */
	ldctl	r1,FCW		/* Turn off segmented mode */
	res	r1,$S_SEG
	ldctl	FCW,r1
	ld	r15,$STKPTR	/* Set stack pointer to end of data */
	clr	r14		/* Clear frame pointer */
#ifdef INITIMSC
	ld	r1,$0		/* Reset disk sequencer for IMSC */
	out	0x22,r1
#endif INITIMSC

_restart:
	jp	_main		/* Start program */
#endif STARTUP
	

#ifdef TRAP
_exins:
	ld	-34(r15),$'x'
	jr	_regsave
_privins:
	ld	-34(r15),$'p'
	jr	_regsave
_syscall:
	ld	-34(r15),$'c'
	jr	_regsave
_breakpt:
	ld	-34(r15),$'b'
	jr	_regsave
_segtrap:
	ld	-34(r15),$'s'
	jr	_regsave
_nmint:
	ld	-34(r15),$'m'
	jr	_regsave
_nvint:
	ld	-34(r15),$'v'
	jr	_regsave
_vectint:
	ld	-34(r15),$'i'
	jr	_regsave

UR14	= 28		// User frame pointer in _r
USP 	= 30		// User stack pointer in _r
USEG	= 32		// User segment  in _r
UPC	= 34		// User program counter in _r
UFCW	= 36		// User FCW in _r

				/* Notice that we're a little way down on */
				/*  the stack here, but normally the user */
				/*  should take care of resetting his program */
				/*  stack pointer if he's in the middle of */
				/*  something. */

.globl _trap
.globl _regsave
_regsave:
trapcode:
	ldm	_r,r0,$16	// Save user registers
	ld	-32(r15),r0	// Save r0 in proper position
	ld	r0,*r15		// Get id of system call
	ld	_id,r0
	ld	r0,2(r15)	// Get FCW
	ld	_r+UFCW,r0
	ld	r0,4(r15)	// Get PC segment
	ld	_r+USEG,r0
	ld	r0,6(r15)	// Get PC offset
	ld	_r+UPC,r0
	ldctl	r0,NSPSEG	// Get user frame pointer
	ld	_r+UR14,r0	// Save user frame pointer
	dec	_r+USP,$8	// Fix up user's stack pointer
	ldm	-30(r15),r1,$15	// Save registers 1-15 on stack
	ld	-4(r15),r0	// Put user frame pointer on stack
	dec	-2(r15),$8	// Fix up stack pointer on stack
	sub	r15,$34		// Set stack back past saved character
	call	_trap		// Handle trap 
	add	r15,$34		// Restore stack
	ldm	r0,-32(r15),$15	// Restore registers 0-14 from stack
	push	*r15,r0		// Save a copy of r0
	ldctl	r0,FCW		// Change back to segmented mode
	set	r0,$S_SEG	// Set segmented bit
	ldctl	FCW,r0
	pop	r0,*r14		// Restore r0
	iret			// Return from trap
#endif TRAP


#ifdef CONTINUE
.globl _gotoit
_gotoit:
	ld	r15,_r+USP	// Put stackpointer back where it was
	jr	user		// Return to user program

.globl _jptoit
_jptoit:
	ld	r15,$STKPTR	// Set up stack pointer
user:
	push	*r15,_r+UPC	// Simulate IRET, first push offset
	push	*r15,_r+USEG	// Push PC segment
	push	*r15,_r+UFCW	// Push FCW
	push	*r15,$0		// Push dummy id
	ld	r1,_r+UR14	// Get user's r14
	ldctl	NSPSEG,r1	// Restore user's r14
	ldctl	r1,FCW		// Change to segmented mode
	set	r1,$S_SEG	// Set segmented mode bit
	ldctl	FCW,r1
/*S*/	ldar	r2,_jptoit	// Set up seg pointer with current segment */
/*S*/	ld	r14,r2
/*S*/	ldm	r0,0x8000,$14	// Restore registers (except stack, frame)*/
/*S*/	.word	_r		// Fake out assembler with segmented address
/*S*/	iret			// Go for it
#endif CONTINUE


#ifdef INITCPU
/* initcpu() */
.globl _initcpu			/* Code for initializing CPU PIO */
_initcpu:
	ld	r2,$0xcf	/* Control mode for PIO */
	clr	r3
	sin	r1,P_SWIT	/* Get switches to put out to LED's */
	sout	P_PIOBCTL,r2	/* Cannot output data first the first time */
	sout	P_PIOBCTL,r3
	sout	P_PIOBDAT,r1

	ld	r1,$0x8f	/* Clear tod control lines */
	sout	P_PIOADAT,r1
	sout	P_PIOACTL,r2
	sout	P_PIOACTL,r3
	ret
#endif INITCPU

#ifdef INITIMSC
/* initcpu() */
.globl _initcpu
_initcpu:
	ld	r1,$0x9e00	/* Set up refresh counter */
	ldctl	REFRESH,r1
	mset
	ld	r2,$LORAMSEG	/* Copy prom to ram */
	clr	r3
	ld	r4,$RPROMSEG
	clr	r5
	ld	r6,$0x2000
	ldctl	r0,FCW		/* Change to segmented mode */
	set	r0,$S_SEG
	ldctl	FCW,r0
	ldir	*r2,*r4,r6
	nop
	res	r0,$S_SEG
	ldctl	FCW,r0
	ret
#endif INITIMSC


#ifdef INITUSART
USARTRXRDY	= 0x02
USARTTXRDY	= 0x01
B_DCD		= (1 << 6)

#ifdef SIOC
P_SIOC		= 0xff7f
SIOC_GATE	= 0xf800
SIOC_COMMAND	= 0xf802
SIOC_FLAGS	= 0xf804
SIOC_CHAR	= 0xf806
SIOC_MBADX	= 0xf808
SIOC_LCLADX	= 0xf80c
SIOC_LEN	= 0xf80e
	
SIOCRESET	= 1
SIOCINT		= 2
WCHAR		= 3
RCHAR		= 4
#endif SIOC

baudtab:
	.byte	0x0f			/* 19200 baud (7) */
	.byte	0x0e			/* 9600 baud (6) */
	.byte	0x0d			/* 4800 baud (5) */
	.byte	0x0c			/* 2400 baud (4) */
	.byte	0x09			/* 1200 baud (3) */
	.byte	0x07			/* 600 baud (2) */
	.byte	0x06			/* 300 baud (1) */
	.byte	0x02			/* 110 baud (0) */

/* initio() */
.globl _initio
_initio:
	ld	r2,$0xed	/* 2 stop bits, even parity, parity disabled */
				/*  8 bit characters, async 1X rate */
	sout	P_USCTL,r2	/* Output mode register 1 */
	sin	r1,P_SWIT	/* Get baud rate from switches */
	and	r1,$0x7		/* Only look at low order 3 bits */
	ldb	rl1,baudtab(r1)	/* Get baud rate from table and switches */
	or	r1,$0xf0	/* I, I, 16X, BKDET, async */
	sout	P_USCTL,r1	/* Output mode register 2 */
	ld	r1,$0x37	/* CR5|CR4|CR2|CR1|CR0 to command register */
	sout	P_USCMD,r1
#ifndef SIOC
	ret
#endif
#ifdef SIOC
	sin	r1,P_USSTAT	/* Check to see if diagnostic port is connected */
	and	r1,$B_DCD	/* Check data carrier detect bit */
	jr	z,siocreset	/* Not connected, try sioc console port */
	ret
	
siocreset:
	call	_mapon		/* Turn on mapping */
	call	_mbuson		/* Turn on multibus */
	ld	r1,$SIOCRESET	/* Reset SIOC */
	out	P_SIOC,r1
siocinit:
	sin	r1,P_SWIT	/* Send baud rate to SIOC */
	com	r1		/* Negative logic on switches */
	and	r1,$0x0f	/* Only send lower 4 bits */
	ld	SIOC_FLAGS,r1	/* Put baud rate into flags */
	clr	SIOC_COMMAND	/* Clear command just in case */
	ld	r1,$SIOCINT	/* Interrupt SIOC */
	out	P_SIOC,r1
	ld	SIOC_GATE,$0xff	/* Close gate */
	ld	r1,$0x8000	/* Down count waiting for SIOC */
sioctimer:
	djnz	r1,sioctimer
	test	SIOC_GATE	/* Check to see if gate has been opened */
	jr	ne,siocinit	/* Not ready, try again */
	clr	SIOC_FLAGS	/* Clear flags for here on */
	ret
#endif
#endif INITUSART


#ifdef INITSIO

baudtab:
	.byte	0x01	/* 38400 baud (7) */
	.byte	0x01	/* 19200 baud (6) */
	.byte	0x01	/* 9600 baud (5) */
	.byte	0x02	/* 4800 baud (4) */
	.byte	0x04	/* 2400 baud (3) */
	.byte	0x08	/* 1200 baud (2) */
	.byte	0x10	/* 600 baud (1) */
	.byte	0x20	/* 300 baud (0) */

siotab:
	.byte	0x40	/* 38400 baud (7) */
	.byte	0x80	/* 19200 baud (6) */
	.byte	0xc0	/* 9600 baud (5) */
	.byte	0xc0	/* 4800 baud (4) */
	.byte	0xc0	/* 2400 baud (3) */
	.byte	0xc0	/* 1200 baud (2) */
	.byte	0xc0	/* 600 baud (1) */
	.byte	0xc0	/* 300 baud (0) */

/* initio() */
.globl _initio
_initio:
	in	r1,SWITCHREG	/* Get baud rate from switches */
	and	r1,$0xf0	/* Ignore lower 4 bits */
	out	OUTLATCH,r1	/* Put out switch values to LED's */
	srl	r1,$4		/* Shift right for baud rate table */
	com	r1		/* Complement for proper table entry */
	and	r1,$0x07	/* Only look at 3 bits */
	ld	r7,r1		/* Save baud rate for sio */
	ldb	rl1,baudtab(r1)	/* Get baud rate from table */

	ldb	rl0,$CNTMODE	/* Set up SIO0A,B CTC for counter mode */
	out	CTC01,r0
	out	CTC01,r1	/* Set up baud rate */
	out	CTC10,r0
	out	CTC10,r1	/* Set up baud rate */

				/* Set up SIO0A for MONITOR console */
	ld	r1,$SIO0AC	/* Put port address in register for indirect */
				/* Set up SIO0B for DOWNLOAD port */
	ld	r5,$SIO0BC	/* Put port address in register for indirect */
	ldb	rl2,$0x04	/* WR4 */
	out	*r1,r2		/* Register number */
	out	*r5,r2		/* Register number */
	ldb	rl2,$0x04	/* 1 stop bit, parity disabled, (even parity) */
	orb	rl2,siotab(r7)	/* SIO clock mode from switches thru table */
	out	*r1,r2		/* Register contents */
	out	*r5,r2		/* Register contents */
	ldb	rl2,$0x01	/* WR1 */
	out	*r1,r2		/* Register number */
	out	*r5,r2		/* Register number */
	ldb	rl2,$0x00	/* Make sure interrupts are turned off */
	out	*r1,r2		/* Register contents */
	out	*r5,r2		/* Register contents */
	ldb	rl2,$0x03	/* WR3 */
	out	*r1,r2		/* Register number */
	out	*r5,r2		/* Register number */
	ldb	rl2,$0xc1	/* 8 bits, RX enable */
	out	*r1,r2		/* Register contents */
	out	*r5,r2		/* Register contents */
	ldb	rl2,$0x05	/* WR5 */
	out	*r1,r2		/* Register number */
	out	*r5,r2		/* Register number */
	ldb	rl2,$0xea	/* DTR, 8 bits, TX enable, RTS */
	out	*r1,r2		/* Register number */
	out	*r5,r2		/* Register number */

	ret

#endif INITSIO


#ifdef INTENABLE
/* enabvi() */
.globl _enabvi
_enabvi:
	ei	vi
	ret

/* disabvi() */
.globl _disabvi
_disabvi:
	di	vi
	ret

/* enabnvi() */
.globl _enabnvi
_enabnvi:
	ei	nvi
	ret

/* disabnvi() */
.globl _disabnv
_disabnv:
	di	nvi
	ret

/* mset() */
.globl _mset
_mset:
	mset
	ret

/* mres */
.globl _mres
_mres:
	mres
	ret
#endif INTENABLE


#ifdef CSV
.globl csv
.globl csv0
.globl csv1
.globl csv2
.globl csv3
.globl csv4
.globl csv5
csv:
csv0:
csv1:
csv2:
csv3:
csv4:
csv5:
	ld	r1,*r15		/* Remember return address */
	ld	*r15,r14	/* Save old frame pointer */
	ld	r14,r15		/* Setup new frame pointer */
	sub	r15,r0		/* Set stack pointer for locals */
	ldm	-12(r14),r8,$6	/* Save user registers */
	call	*r1		/* Go back to routine */
	
.globl cret
.globl cret0
.globl cret1
.globl cret2
.globl cret3
.globl cret4
.globl cret5
cret:
cret0:
cret1:
cret2:
cret3:
cret4:
cret5:
	ldm	r8,-12(r14),$6	/* Restore user registers */
	ld	r15,r14		/* Move stack pointer to previous frame */
	pop	r14,*r15	/* Restore previous frame pointer */
	ret
	
.globl	switch1
switch1:
	ld	r5, 4(r6)	/* r5 = default code address */
	cp	r7, 2(r6)
	jp	gt, *r5		/* out of case range on high end */
	sub	r7, *r6
	jp	lt, *r5		/* out of case range on low end */
	add	r6, r7
	add	r6, r7		/* r6 = index into case code addresses */
	ld	r5, 6(r6)	/* r5 = case code address */
	jp	*r5		/* jump to case */
	
.globl switch2
switch2:
	ld	r5, *r6		/* r5 = number of cases + 1 */
	ld	r4, r5		/* r4 = number of cases + 1 */
	inc	r6, $2		/* r6 = ptr to case value list */
	cpir	r7, *r6, r5, eq	/* look for correct case value */
	add	r6, r4
	add	r6, r4		/* r6 = address of correct code address */
	ld	r5, -4(r6)	/* r5 = correct case code address */
	jp	*r5		/* jump to case */
	
#endif CSV


#ifdef CPUCHARIO
/* putchar((char) ch) */
.globl _putchar
.globl _put2cha
_putchar:
_put2cha:
	call	_xonxoff	/* Check to see if we have an xoff */
	ld	r7,2(r15)	/* Get character to be put out */
echo:
	cp	r7,$'\n'	/* Check for nl, change to lfcr */
	jr	ne,wloop	/* Not an nl */
	call	wloop		/* Output nl and return to output cr */
	ld	r7,$'\r'	/* Load cr */
	jr	echo		/* Output cr */
wloop:
#ifdef SIOC
	sin	r1,P_USSTAT	/* Check if diagnostic port is connected */
	and	r1,$B_DCD	/* Check data carrier detect bit */
	jr	nz,usartout
siocout:
	test	SIOC_GATE	/* Check to see if we are ready to transmit */
	jr	ne,siocout	/* If not, wait */
	ld	SIOC_COMMAND,$WCHAR	/* Prepare to write a character */
	ld	SIOC_CHAR,r7	/* Load up character */
	ld	SIOC_GATE,$0xff	/* Close gate */
siocoutloop:
	test	SIOC_GATE	/* Wait for character to be output */
	jr	ne,siocoutloop
	ret
#endif
	
usartout:
	sin	r1,P_USSTAT	/* Get status to check if we can output */
	and	r1,$USARTTXRDY	/* Transmit holding register empty? */
	jr	z,usartout	/* No, try again */
	sout	P_USDATA,r7	/* Send out character */
	ret			/* All done, so return (char in r7) */

/* getchar() returns (unsigned char) ch */
.globl	_getchar
.globl	_get2cha
_getchar:
_get2cha:
#ifdef SIOC
	sin	r1,P_USSTAT	/* Check diagnostic port connection */
	and	r1,$B_DCD	/* Check data carrier detect bit */
	jr	nz,usartin	/* Skip sioc if usart connected */
siocin:
	test	SIOC_GATE	/* Wait for gate to open */
	jr	ne,siocin
	ld	SIOC_COMMAND,$RCHAR	/* Prepare to read a character */
	ld	SIOC_GATE,$0xff	/* Close gate */
siocwait:
	test	SIOC_GATE	/* Wait for gate to open */
	jr	ne,siocwait
	ld	r7,SIOC_CHAR	/* Put character in r7 for return */
	jr	gotch
#endif
	
usartin:
	sin	r1,P_USSTAT	/* Get status to check if we can input */
	and	r1,$USARTRXRDY	/* Receive holding register has data? */
	jr	z,usartin	/* No, check again */
	sin	r7,P_USDATA	/* Get character */
	
gotch:
	and	r7,$0x7f	/* Mask of parity bit */
	cp	r7,$DELCH	/* Check to see if it is a rubout */
	jp	eq,DELETE	/* If it is, bail out, go proper place */
	cp	r7,$ABORTCH	/* Check to see if it is an abort character */
	jp	eq,ABORT	/* If it is, bail out, go to proper place */
	cp	r7,$'\r'	/* Check for a cr */
	jr	ne,flashtest	/* Not a cr, so let it be */
	ld	r7,$'\n'	/* Change the cr to a nl */
flashtest:
	ld	r6,r7		/* Make a copy of r7 */
	test	_verbose	/* See if we want to flash leds */
	jr	z,echocheck	/* Don't flash it, but go check for echo */
	sin	r6,P_PIOBDAT	/* Get current PIO configuration */
	clrb	rl6		/* Clear out low nibble */
	orb	rl6,rl7		/* Or in character */
	sout	P_PIOBDAT,r6	/* Output character to led's */
echocheck:
	test    _echoch		/* See if we want to echo it */
	jr	z,echo		/* Go ahead and echo it */
	ret			/* Just return with character in r7 */

.globl _xonoff
_xonoff:
	ret

#endif CPUCHARIO
	

#ifdef IMSCCHARIO
#ifndef ABORT
#define ABORT _restart
#endif
#ifndef ABORTCH
#define ABORTCH '!'
#endif
#ifndef DELETE
#define DELETE _restart
#endif
#ifndef DELCH
#define DELCH 0177
#endif

.globl _putchar
_putchar:
	call	_xonoff		/* Check to see if we have an xoff */
	ld	r7,2(r15)	/* Get character to be put out */
echo:
	cp	r7,$'\n'	/* Check for nl, change to lfcr */
	jr	ne,wloop	/* Not an nl */
	calr	wloop		/* Output nl and return to output cr */
	ld	r7,$'\r'	/* Load cr */
	jr	echo		/* Output cr */
wloop:
	in	r1,SIO0AC	/* Get status to check if we can output */
	and	r1,$SIOTXRDY	/* Transmit holding register empty? */
	jp	z,wloop		/* No, try again */
	out	SIO0AD,r7	/* Send out character */
	ret			/* All done, so return */


.globl	_getchar
_getchar:
rloop:
	in	r1,SIO0AC	/* Get status to check if we can input */
	and	r1,$SIORXRDY	/* Receive holding register has data? */
	jr	z,rloop		/* No, check again */
	in	r7,SIO0AD	/* Get character */
	and	r7,$0x7f	/* Mask off parity bit */
	cp	r7,$DELCH	/* Check to see if it is a rubout */
	jp	eq,DELETE	/* If it is, bail out, go proper place */
	cp	r7,$ABORTCH	/* Check to see if it is an abort character */
	jp	eq,ABORT	/* If it is, bail out, go to proper place */
	cp	r7,$'\r'	/* Change cr to nl */
	jp	ne,flashtest	/* Not a cr, so let it be */
	ld	r7,$'\n'	/* Change it to a nl */
flashtest:
	ld	r6,r7		/* Make a copy of r7 */
	test	_verbose	/* See if we want to flash leds */
	jr	z,echocheck	/* Don't flash it, but go check for echo */
	out	OUTLATCH,r6	/* Output low nibble */
echocheck:
	test	_echoch		/* See if we want to echo it */
	jr	z,echo		/* Go ahead and echo it */
	ret			/* Just return with character in r7 */

.globl _xonoff
_xonoff:
	in	r1,SIO0AC	/* Get status to see if we have a character */
	and	r1,$SIORXRDY	/* See if we have one */
	ret	z		/* If we don't, simply return */
	in	r7,SIO0AD	/* Get character from port */
	and	r7,$0x7f	/* Mask off all but lower 7 bits */
	cp	r7,$DELCH	/* Check to see if it is a rubout */
	jp	eq,DELETE	/* If it is, bail out, go proper place */
	cp	r7,$ABORTCH	/* Check to see if it is an abort character */
	jp	eq,ABORT	/* If it is, bail out, go to proper place */
	cp	r7,$023		/* Check for xoff otherwise */
	ret	ne		/* If not an xoff, return */
xonloop:
	push	*r15,_echoch	/* Save current status of echo flag */
	ld	_echoch,$1	/* Make sure we don't echo while waiting */
	call	_getchar	/* Go ahead and get character */
	pop	_echoch,*r15	/* Restore echo flag */
	and	r7,$0x7f	/* Mask off the parity bit */
	cp	r7,$021		/* Look for an xon */
	jr	ne,xonloop	/* If no xon, look again */
	ret			/* Xon found, so return */

.globl _put2cha
_put2cha:
	ld	r7,2(r15)	/* Get character to be put out */
echo2:
	cp	r7,$'\n'	/* Check for nl, change to lfcr */
	jr	ne,wloop2	/* Not an nl */
	calr	wloop2		/* Output nl and return to output cr */
	ld	r7,$'\r'	/* Load cr */
	jr	echo2		/* Output cr */
wloop2:
	in	r1,SIO0BC	/* Get status to check if we can output */
	and	r1,$SIOTXRDY	/* Transmit holding register empty? */
	jp	z,wloop2		/* No, try again */
	out	SIO0BD,r7	/* Send out character */
	ret			/* All done, so return */


.globl	_get2cha
_get2cha:
rloop2:
	in	r1,SIO0BC	/* Get status to check if we can input */
	and	r1,$SIORXRDY	/* Receive holding register has data? */
	jr	z,rloop2		/* No, check again */
	in	r7,SIO0BD	/* Get character */
	and	r7,$0x7f	/* Mask off parity bit */
	cp	r7,$DELCH	/* Check to see if it is a rubout */
	jp	eq,DELETE	/* If it is, bail out, go proper place */
	cp	r7,$ABORTCH	/* Check to see if it is an abort character */
	jp	eq,ABORT	/* If it is, bail out, go to proper place */
	cp	r7,$'\r'	/* Change cr to nl */
	jp	ne,flashtest2	/* Not a cr, so let it be */
	ld	r7,$'\n'	/* Change it to a nl */
flashtest2:
	ld	r6,r7		/* Make a copy of r7 */
	ld	r1,_verbose	/* See if we want to flash leds */
	bit	r1,$0		/* Check low order bit (on/off) */
	jr	z,echocheck2	/* Don't flash it, but go check for echo */
	out	OUTLATCH,r6	/* Output low nibble */
echocheck2:
	ld	r1,_echoch	/* See if we want to echo it */
	bit	r1,$0		/* Check low order bit (on/off) */
	jr	z,echo2		/* Go ahead and echo it */
	ret			/* Just return with character in r7 */
#endif IMSCCHARIO


#ifdef PIOA
/* pioaset((unsigned) value) */
.globl _pioaset
_pioaset:
	ld	r1,2(r15)	/* Get value to be put out */
	sout	P_PIOADAT,r1	/* Output it */
	ld	r1,$PIOACTL
	sout	P_PIOACTL,r1
	ld	r1,$PIOAWRT
	sout	P_PIOACTL,r1
	ret
#endif PIOA	

#ifdef MAP
MAPBIT	= 2
/* mapon() */
.globl _mapon
_mapon:
	ldctl	r0,FCW		/* Turn segmented mode on */
	ld	r3,r0		/* Save FCW for later */
	set	r0,$S_SEG	/* Turn on segmented mode */
	ldctl	FCW,r0
	ld	r4,$0x7f00	/* Map number 127 for special I/O directly */
	ld	r5,$P_M0	/* First map port address */
	ld	r2,$0		/* Physical page 0 */
	ld	r6,$31		/* Number of data (or text) pages */
loadprommap:
	ld	*r4,r2		/* Data map */
	ld	r4($64),r2	/* Text map */
	inc	r5,$2		/* Point to next map slot */
	inc	r2,$1		/* Get next page number */
	djnz	r6,loadprommap
	ld	r2,$0x10	/* Static ram page */
	ld	*r4,r2		/* Point multibus page to static ram */
	ld	r4($64),r2	/* Do text page to be complete */
	ldctl	FCW,r3		/* Turn segmented mode back off */
	sin	r1,P_PIOADAT	/* Get PIO control byte */
	res	r1,$MAPBIT	/* Turn it on */
	push	*r15,r1		/* Turn mapping on */
	call	_pioaset
	inc	r15,$2
	ret

/* mapoff() */
.globl _mapoff
_mapoff:
	sin	r1,P_PIOADAT	/* Get PIO control byte */
	set	r1,$MAPBIT	/* Turn mapping off */
	push	*r15,r1	
	call	_pioaset
	inc	r15,$2
	ret
#endif MAP

#ifdef MBUS
MBUSBIT	= 1
/* mbuson() */
.globl _mbuson
_mbuson:
	ldctl	r0,FCW		/* Change to segmented mode */
	ld	r1,r0		/* Save it so we can get back */
	set	r0,$S_SEG	/* Set segmented mode bit */
	ldctl	FCW,r0
	ldar	r2,_mbuson	/* Get this segment for multibus mapper */
	ldb	rl2,rh2		/* Put segment into low byte */
	com	r2		/* Complement it for the mapper */
	sout	P_MB0+7,r2	/* Set last page of multibus mapper to here */
	ldctl	FCW,r1		/* Turn nonsegmented mode back on */
	sin	r1,P_PIOADAT	/* Get PIO control byte */
	res	r1,$MBUSBIT	/* Otherwise, turn it on */
	push	*r15,r1		/* Turn multibus on */
	call	_pioaset
	inc	r15,$2
	sout	P_MRE1,r1	/* Reset multibus lock register */
	sout	P_MRE2,r1	/* Clear error reset register */
	sout	P_MBTOINT,r1	/* Reset multibus timeout register */
	ret
	
/* mbusoff() */
.globl _mbusoff
_mbusoff:
	sin	r1,P_PIOADAT	/* Get PIO control byte */
	set	r1,$MBUSBIT	/* Turn multibus off */
	push	*r15,r1		/* Turn multibus off */
	call	_pioaset
	inc	r15,$2
	ret
#endif MBUS

#ifdef ECC
ECCBIT	= 3
/* eccon() */
.globl _eccon
_eccon:
	sin	r1,P_PIOADAT	/* Get current status of PIO */
	res	r1,$ECCBIT	/* Turn on ECC */
	push	*r15,r1
	call	_pioaset
	inc	r15,$2
	ld	r2,$0x2000	/* Turn on error checking */
	sout	P_ECC,r2
	ret
	
/* eccoff() */
.globl _eccoff
_eccoff:
	sin	r1,P_PIOADAT	/* Get current status of PIO */
	set	r1,$ECCBIT	/* Turn off ECC */
	push	*r15,r1
	call	_pioaset
	inc	r15,$2
	ld	r1,0x4000	/* Pass-thru mode */
	sout	P_ECC,r1
	ret
#endif ECC

#ifndef MAP
_mapon:
_mapoff:
	ret
#endif

#ifndef ECC
_eccon:
_eccoff:
	ret
#endif

#ifndef MBUS
_mbuson:
_mbusoff:
	ret
#endif

	

#ifdef ABSJUMP
/* absjump((long) addr) */
.globl _absjump
_absjump:
	ld	r3,4(r15)	/* Get offset of where to go */
	ldb	rh2,3(r15)	/* Get segment number */
	clrb	rl2		/* Clear out flags just in case it matters */
	ldctl	r0,FCW		/* Retrieve current FCW */
	set	r0,$S_SEG	/* Set segmented mode bit */
	ldctl	FCW,r0		/* Set FCW */
	pushl	*r14,rr2	/* Push address of entry location */
	push	*r14,$0x4800	/* Push FCW with nonsegmented, system mode */
	push	*r14,$0		/* Push dummy id */
	iret			/* Simulated iret data on stack, so go for it */
	
#endif ABSJUMP

#ifdef SPIN
/* spin((unsigned) delay) */
.globl	_spin
_spin:
	clr	r2
	ld	r3,2(r15)	/* Get 60ths of a second to delay */
	multl	rq0,$0x880	/* Multiplier to get about 1/60 second */
spinloop:			/*  at 4 MHz */
	subl	rr2,$1
	testl	rr2
	jp	ne,spinloop
	ret
#endif SPIN


#ifdef IOINST
/*
 * out_mul(port, value)
 * in_mult(port)
 * out_loc(port, value)
 * in_loca(port)
 *
 * These four routines do word I/O instructions to the normal and special
 *  I/O spaces.  Special I/O space is used for local I/O and normal I/O
 *  space for multibus I/O.
 */
	
.globl _out_mul
_out_mul:
	ld	r1,2(r15)	/* r1 = port address */
	ld	r2,4(r15)	/* r2 = value */
	out	*r1,r2
	ret
	
.globl _in_mult
_in_mult:
	ld	r1,2(r15)	/* r1 = port address */
	in	r7,*r1
	ret
	
.globl _out_loc
_out_loc:
	ld	r1,2(r15)	/* r1 = port address */
	lda	r2,4(r15)	/* r2 = value */
	souti	*r1,*r2,r3	/* souti used since sout can't indirect */
	ret
	
.globl _in_loca
_in_loca:
	ld	r1,2(r15)	/* r1 = port address */
	lda	r2,-2(r15)	/* r2 = address of temp location */
	sini	*r2,*r1,r3	/* sini used since sin can't indirect */
	ld	r7, -2(r15)
	ret
#endif IOINST


#ifdef MEMLD
/* memld((long) addr, (unsigned) value)) */
.globl _memld
_memld:
	ld	r1,6(r15)	/* Value to be loaded */
	ldl	rr2,2(r15)	/* Memory address */
	ldb	rh2,rl2		/* Put segment number where it belongs */
	ldctl	r0,FCW		/* Turn on segmented mode */
	set	r0,$S_SEG
	ldctl	FCW,r0
	ld	*r2,r1		/* Store value into memory */
	res	r0,$S_SEG	/* Turn off segmented mode */
	ldctl	FCW,r0
	ret
	
/* memrd((long) addr) returns (unsigned) value */
.globl _memrd
_memrd:
	ldl	rr2,2(r15)	/* Memory address */
	ldb	rh2,rl2		/* Put segment number where it belongs */
	ldctl	r0,FCW		/* Turn on segmented mode */
	set	r0,$S_SEG
	ldctl	FCW,r0
	ld	r7,*r2		/* Get value from memory */
	res	r0,$S_SEG	/* Turn off segmented mode */
	ldctl	FCW,r0
	ret			/* Return with value in r7 */
#endif MEMLD

#ifdef BMEMLD
/* bmemld((long) addr, (char) value)) */
.globl _bmemld
_bmemld:
	ld	r1,6(r15)	/* Value to be loaded */
	ldl	rr2,2(r15)	/* Memory address */
	ldb	rh2,rl2		/* Put segment number where it belongs */
	ldctl	r0,FCW		/* Turn on segmented mode */
	set	r0,$S_SEG
	ldctl	FCW,r0
	ldb	*r2,rl1		/* Store value into memory */
	res	r0,$S_SEG	/* Turn off segmented mode */
	ldctl	FCW,r0
	ret

/* bmemrd((long) addr) returns (char) value */
.globl _bmemrd
_bmemrd:
	ldl	rr2,2(r15)	/* Memory address */
	ldb	rh2,rl2		/* Put segment number where it belongs */
	ldctl	r0,FCW		/* Turn on segmented mode */
	set	r0,$S_SEG
	ldctl	FCW,r0
	ldb	rl7,*r2		/* Get value from memory */
	res	r0,$S_SEG	/* Turn off segmented mode */
	ldctl	FCW,r0
	ret			/* Return with value in r7 */
#endif BMEMLD


#ifdef CLRPAGE
/* clrpage((unsigned) segment, (unsigned) pageno)) */
.globl _clrpage
_clrpage:
	ldl	rr2,2(r15)	/* Get segment and page number */
	ldb	rh2,rl2		/* Put segment number into high byte */
	clrb	rl2		/* Clear out low byte */
	sll	r3,$11		/* Shift page number to get offset */
	ldl	rr4,rr2		/* Make another copy for ldir */
	ld	r1,$(PAGESIZE/2-1) /* Number of words in a page minus one */
	ldctl	r0,FCW		/* Turn on segmented mode */
	set	r0,$S_SEG
	ldctl	FCW,r0
	clr	*r4		/* Clear first word in page */
	inc	r3,$2		/* Increment address */
	ldir	*r2,*r4,r1	/* Clear segment */
	nop
	res	r0,$S_SEG	/* Turn off segmented mode */
	ldctl	FCW,r0
	ret
#endif CLRPAGE

#ifdef BLKMOVE
/* blkmove( (long) fromaddr, (long) toaddr, (unsigned) count) */
/* nblkmove same but uses lddr */
.globl _blkmove
_blkmove:
	ldl	rr2,2(r15)	/* From location */
	sll	r2,$8		/* Put segment number where it belongs */
	ldl	rr4,6(r15)	/* To location */
	sll	r4,$8		/* Put segment number where it belongs */
	ld	r6,10(r15)	/* Byte count */
	srl	r6,$1		/* Change byte count to word count */
/* #ifdef INITIMSC
	ldctl	r7,REFRESH
	res	r7,$15
	ldctl	REFRESH,r7
#endif */
	ldctl	r1,FCW		/* Change to segmented mode */
	ld	r0,r1
	set	r1,$S_SEG
	ldctl	FCW,r1
	ldir	*r4,*r2,r6	/* Do block move */
	nop
	ldctl	FCW,r0		/* non-segmented */
/* #ifdef INITIMSC
	set	r7,$15		
	ldctl	REFRESH,r7
#endif */
        jr	nz,_cmpret	/* if compare worked */
	inc	r2		/* count where error occured */
	sll	r2,$1		/* byte count */
	sub	r3,r2		/* how far did it get (in bytes) */
_cmpret:
	ld	r7,r3		/* return count */
	ret
.globl _nblkmove
_nblkmove:
	ldl	rr2,2(r15)	/* From location */
	sll	r2,$8		/* Put segment number where it belongs */
	ldl	rr4,6(r15)	/* To location */
	sll	r4,$8		/* Put segment number where it belongs */
	ld	r6,10(r15)	/* Byte count */
	srl	r6,$1		/* Change byte count to word count */
/* #ifdef INITIMSC
	ldctl	r7,REFRESH	
	res	r7,$15		
	ldctl	REFRESH,r7
#endif */
	ldctl	r1,FCW		/* Change to segmented mode */
	ld	r0,r1
	set	r1,$S_SEG
	ldctl	FCW,r1
	lddr	*r4,*r2,r6	/* Do block move */
	nop
	ldctl	FCW,r0		/* non-segmented */
/* #ifdef INITIMSC
	set	r7,$15		
	ldctl	REFRESH,r7
#endif */
        jr	nz,_cmpret	/* if compare worked */
	inc	r2		/* count where error occured */
	sll	r2,$1		/* byte count */
	sub	r3,r2		/* how far did it get (in bytes) */
	jr	_cmpret
#endif BLKMOVE

#ifdef BLKCMP
/* blkcmp( (long) fromaddr, (long) toaddr, (unsigned) count) */
.globl _blkcmp
_blkcmp:
	ldl	rr4,2(r15)	/* From location */
	ldb	rh4,rl4		/* Put segment number where it belongs */
	ldl	rr6,6(r15)	/* To location */
	ldb	rh6,rl6		/* Put segment number where it belongs */
	ld	r2,10(r15)	/* Byte count */
	srl	r2,$1		/* Change byte count to word count */
	ldctl	r1,FCW		/* Change to segmented mode */
	ld	r0,r1
	set	r1,$S_SEG
	ldctl	FCW,r1
	cpsir	*r6,*r4,r2,ne	/* Do block compare */
	ldctl	FCW,r0		/* Change back to nonsegmented mode */
	jr	z,cmpret	/* Compare succeeded */
	dec	r7,$2		/* Point back to possibly offending word */
	ret			/* Signifies we had a bad match */
cmpret:
	ld	r7,$-1		/* If it was okay, return -1 */
	ret
#endif BLKCMP

#ifdef CLRBSS
/* Needs RAM and ENDRAM defined */

/* clrbss() */
.globl _clrbss			/* Initialize bss to zero */
_clrbss:
	ld	r1,$RAM		/* Address of first word */
	ld	r2,$RAM+2
	ld	r3,$(ENDRAM-RAM)/2-1	/* Size of RAM in words minus one */
	clr	*r1		/* Zero first word */
	ldir	*r2,*r1,r3	/* Zero all */
	ret
#endif CLRBSS
	
#ifdef CPULEDS
.globl _ledsoff
_ledsoff:
	sin	r1,P_PIOBDAT	/* Get current led values */
	ld	r2,2(r15)	/* Get value to be turned off */
	or	r1,r2		/* Turn bit(s) off */
	sout	P_PIOBDAT,r1	/* Put them into the leds */
	ld	r1,$PIOBCTL
	sout	P_PIOBCTL,r1
	ld	r1,$PIOBWRT
	sout	P_PIOBCTL,r1
	ret

.globl _ledson
_ledson:
	sin	r1,P_PIOBDAT	/* Get current led values */
	ld	r2,2(r15)	/* Get value to be turned on */
	com	r2		/* Complement it so we can turn it on */
	and	r1,r2		/* Mask it off */
	sout	P_PIOBDAT,r1	/* Put the new value into the leds */
	ld	r1,$PIOBCTL
	sout	P_PIOBCTL,r1
	ld	r1,$PIOBWRT
	sout	P_PIOBCTL,r1
	ret
#endif CPULEDS

#ifdef IMSCLEDS
.globl _leds
_leds:
	ld	r2,2(r15)	/* Get value to be put into leds */
	and	r2,$0x7f	/* Only output lower seven bits */
	out	OUTLATCH,r2	/* Put value to leds */
	ret

.globl _ledsoff
_ledsoff:
	ret

.globl _ledson
_ledson:
	ret

#endif IMSCLEDS
.page
