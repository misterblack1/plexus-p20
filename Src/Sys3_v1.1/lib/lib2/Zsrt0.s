#include <sys/plexus.h>
NFILES = 6		/* also set in saio.h */

#define MAPBIT	2

/* Startup code for two-stage bootstrap */

/************************************************************************/
/*									*/
/* This is used by the standalone programs for startup			*/
/*  It can start in segmented or nonsegmented mode, and needs		*/
/*  to return to its caller (boot) in segmented mode.			*/
/*									*/
/* It sets up the program status area to be at location 0, and sets	*/
/*  all the system trap locations to go to trap, and leaves the		*/
/*  interrupt vectors alone.  The standalone program is then free	*/
/*  to stuff whatever it likes in these locations.  Notice that most	*/
/*  programs will never find any need to turn interrupts on.		*/
/* 									*/
/* This program can also run in split or non-split I and D space	*/
/*									*/
/************************************************************************/


.globl	_main, __rtt
.globl	_exec
.globl	_psa
.globl	_segno
.globl	_pages
.globl	_dataseg
.globl	__trap
.globl	_pfail
.globl	__init
.globl	__exit
.globl	_cleanup
.globl	_close
.globl	_environ
.globl	_end
.globl	_edata

.text
.org	0

_zero:
_psa:
	calr	_start	/* Go to start of program */
	.word	0		/* Not used */
	.word	0		/* Not used */
	.word	0		/* Not used */

exins:	.word	0,0xc800,0,xtrap	/* Extended instruction trap */
priv:	.word	0,0xc800,0,ptrap	/* Privileged instruction trap */
scall:	.word	0,0xc800,0,ctrap	/* System call trap */
segt:	.word	0,0xc800,0,strap	/* Segment trap */
nmint:	.word	0,0xc800,0,mtrap	/* Non-maskable interrupt */
nvint:	.word	0,0xc800,0,vtrap	/* Non-vectored interrupt */

.org	0x23a			/* End of program status area */

				/* We don't know if we are segmented or not, */
				/*  so be careful to reset stack accordingly */
_start:
	ldctl	r0,FCW		/* Check to see if in segmented mode */
	ld	r1,r0		/* Save a copy for return to caller */
	bit	r0,$S_SEG	/* Check segmented bit */
	jr	z,nonseg	/* Segmented bit not set */
	inc	r15,$2		/* Fixup stack from calr at location 0 */
nonseg:
	inc	r15,$2

	ld	r0,$0xc800	/* Change to system, segmented, nvi mode */
	ldctl	FCW,r0
	popl	rr2,*r14	/* PC of where to go to	return to caller */
	pop	r7,*r14		/* Get next available page on stack */

/******  DON'T TOUCH R1,R2,R3 OR R7 UNTIL YOU SAVE THEM BELOW */

	ldar	r4,_psa		/* Figure out which segment we are in. */
				/* At this point, r4 has data seg. # */
	ldctl	PSAPSEG,r4	/* Set up PSA segment number */
	ldctl	PSAPOFF,r5	/* Set up PSA offset */
#ifndef V7
	ld	r0,$0x4800	/* Change to system, nonsegmented, nvi mode */
	ldctl	FCW,r0

	ld	_segno,r4	/* Save segment number for use by program */
				/* Turn off R/O bit of text segment	*/
				/* so we can write into psa		*/
				/* Find out if we are already mapped, */
	sin	r0,P_PIOADAT
	bit	r0,$MAPBIT	/* Check mapped bit */
	jr	nz,psasetup	/* If nonzero (unmapped), skip next section */
				/* Turn off R/O bits of text pages */
				/* Turn off segmentation */
	ld	r6,r4		/* r6 <= text seg # for this process */
	set	r6,$8		/* force to text seg. for split I&D */
	srl	r6,$2		/* r6 <= port adx for text seg. page 0 */
	add	r6,$0x8000	/* ... */
	lda	r5,textmap
	sini	*r5,*r6,r0	/* *r5 <= C(mapset, text seg, page 0) */

	dec	r5,$2		/* sini above bumped r5 by 2 */
	res	*r5,$S_RO	/* turn off R/O flag */
	souti	*r6,*r5,r0	/* r5 <= adx textmap+2 */
	inc	r6,$2		/* r6 <= port adx for text seg page 1 */
	sini	*r5,*r6,r0	/* *r5 <= C(mapset, text seg, page 0) */
	dec	r5,$2		/* sini above bumped r5 by 2 */
	res	*r5,$S_RO	/* turn off R/O flag */
	souti	*r6,*r5,r0
	
psasetup:
	ld	r0,$0xc800	/* Return to system, segmented, nvi mode */
	ldctl	FCW,r0
#endif
		/* ldr always accesses text space for memory operand */
		/*  in Plexus Z8000 CPU */
	ldr	exins+4,r4	/* Make sure psa goes to this segment */
	ldr	priv+4,r4
	ldr	scall+4,r4
	ldr	segt+4,r4
	ldr	nmint+4,r4
	ldr	nvint+4,r4
	ldr	saved_fcw,r1	/* Now we are in non-segmented mode ??? */
/*	ldrl	saved_pc,rr2	/* Save segment and pc for return */
	ldr	saved_pc,r2
	ldr	saved_pc+2,r3
/*	ldrl	saved_sp,rr14	/* Save stack pointer */
	ldr	saved_sp,r14
	ldr	saved_sp+2,r15
	ldr	lastpage,r7	/* Remember last page */

				/* Find out if we are already mapped, */
				/*  if not, set up maps accordingly */
	sin	r1,P_PIOADAT
	bit	r1,$MAPBIT	/* Check mapped bit */
	jr	z,mbuson	/* If zero (mapped), forget about setting up maps */

	ldar	r2,_zero	/* Determine physical address of boot */
	srl	r2,$3		/* Shift segment number to find page number */
	and	r2,$0xfff	/* Only use low 12 bits */
	ld	r6,$32		/* Number of data (or text) pages */

	ldar	r4,_zero	/* Get segment number */
	and	r4,$0xfeff	/* Move down to even (data) segment */
	srll	rr4,$18		/* Shift segment number for port offset */
	add	r5,$P_M0	/* Add in first map for port address */
	ld	r4,$0x7f00	/* Map number 127 for special I/O directly */
loadmap:
	ld	*r4,r2		/* Data map */
	ld	r4($64),r2	/* Text map */
	inc	r5,$2		/* Point to next mapslot */
	inc	r2,$1		/* Get next page number */
	djnz	r6,loadmap

	ld	r4,$0x7f00	/* Map number 127 for special I/O directly */
	ld	r5,$P_M0	/* First map port address */
	ld	r2,$0		/* Physical location 0 */
	ld	r6,$32		/* Number of data (or text) pages */
loadprommap:
	ld	*r4,r2		/* Data map */
	ld	r4($64),r2	/* Text map */
	inc	r5,$2		/* Point to next mapslot */
	inc	r2,$1		/* Get next page number */
	djnz	r6,loadprommap

				/* Clear status registers */
mbuson:
	clr	r1
	sout	P_MRE1,r1	/* Multibus to memory transfer enable */
	sout	P_MRE2,r1	/* Unlock multibus */

				/* Turn on mapping and multibus */
				/* Set up PIO, normal mode, no error reporting */
	ld	r1,$B_ALLOWRESET|B_ECCOFF|B_RESETOFF
	sout	P_PIOADAT,r1	/* Load the data first so that there is no glitch */
	ld	r1,$PIOACTL
	sout	P_PIOACTL,r1
	ld	r1,$PIOAWRT
	sout	P_PIOACTL,r1
	ld	r1,$PIOBCTL
	sout	P_PIOBCTL,r1
	ld	r1,$PIOBWRT
	sout	P_PIOBCTL,r1

	ldar	r4,_zero	/* Find out which segment we are in */
	ldb	rl4,rh4		/* Move segment number to low byte */
	com	r4		/* Complement it */
	sout	P_MB0 + 7,r4	/* Set up slot 7 of the mb map to this seg */

	sin	r1,P_SWIT	/* Get baud rate from switches */
	ld	r2,$0xed	/* 2 stop bits, even parity, parity disabled */
				/*  8 bit characters, async 1X rate */
	sout	P_USCTL,r2	/* Output mode register 1 */
	and	r1,$0x7		/* Only look at low order 3 bits */
	ldar	r2,baudtab	/* Get address of baud rate table */
	set	r2,$8		/* set r2 to text seg for Split I & D */
	add	r3,r1		/* Add in offset to access USART speed */
	ldb	rl1,*r2		/* Fetch USART speed from table */
	or	r1,$0xf0	/* I, I, 16X, BKDET, async */
	sout	P_USCTL,r1	/* Output mode register 2 */
	ld	r1,$0x37	/* CR5|CR4|CR2|CR1|CR0 to command register */
	sout	P_USCMD,r1

	ld	r4,$0x4800	/* Nonsegmented mode, nvi on */
	ldctl	FCW,r4

	ld	r15,$0xf800	/* End of page 30 */
	push	*r15,$~USERLED	/* Turn off all but user led before going to main */
	call	_ledsoff
	ld	*r15,$USERLED	/* Turn on user led */
	call	_ledson
	call	clrbss		/* clear this for restarting of code */
	ld	r2,$argv00
	ld	argv0,r2
	call	_allall		/* allocate all pages of data space */
	call	__init		/* Create and mount devices */
	push	*r15,_environ
	push	*r15,$argv0
	push	*r15,$1
	call	_main		/* Go off to user program */

__exit:				/* Close all open files: called by _exit */
	ld	r2,$NFILES-1
	push	*r15,r2
exit1:
	ld	*r15,r2
	call	_close
	djnz	r2,exit1
	ld	*r15,r2
	call	_close		/* close 0 */
	inc	r15,$2		
	jp	__rtt		/* Return to bootstrap */


baudtab:
	.byte	0x0f			/* 19200 baud (7) */
	.byte	0x0e			/* 9600 baud (6) */
	.byte	0x0d			/* 4800 baud (5) */
	.byte	0x0c			/* 2400 baud (4) */
	.byte	0x09			/* 1200 baud (3) */
	.byte	0x07			/* 600 baud (2) */
	.byte	0x06			/* 300 baud (1) */
	.byte	0x02			/* 110 baud (0) */

/* This is a direct return to the bootstrap */
__rtt:
	ld	*r15,$USERLED	/* Turn off USER led */
	call	_ledsoff
	ld	r0,$0xc800	/* Change back to segmented mode */
	ldctl	FCW,r0		/*  so we can do iret and reference
				/*  system r14 */
/*	ldrl	rr14,saved_sp	/* Get stack pointer from when entered */
	ldr	r14,saved_sp	/* must be in seg mode to use system r14 */
	ldr	r15,saved_sp+2
	ldr	r1,saved_fcw	/* Get FCW */
/*
/*	ldrl	rr2,saved_pc	/* Get pc for return */
	ldr	r2,saved_pc
	ldr	r3,saved_pc+2
	pushl	*r14,rr2	/* Simulate stack for an iret, pc first */
	push	*r14,r1		/* Push caller's FCW */
	push	*r14,$0		/* Push dummy id */
	iret			/* Return to bootstrap program */

/* This is executed in order to execute a loaded program */
_exec:
	ld	r2,_dataseg	/* r2 <= called pgm's data segment */
	ldb	rh2,rl2		/* move segment number into high byte */
	clrb	rl2
	ld	r1,_pages
	ldctl	r0,FCW		/* Turn on segmented mode */
	set	r0,$S_SEG
	ldctl	FCW,r0
	ld	r14,r2		/* set up (segmented) stack pointer */
				/*  note that this is system r14 */
	ld	r15,$0xf800	/* End of page 30 */

	push	*r14,'dk'	/* Place controller name on stack */
	push	*r14,r1		/* Place last page used on stack */
	ldar	r2,_zero	/* Get segment number */
	pushl	*r14,rr2

	push	*r14,$0		/* Set up stack to do iret to location 0 */
	push	*r14,r14	/* PC segment of user program */
	push	*r14,$0xc800	/* Push FCW with segmented, system mode */
	push	*r14,$0		/* Dummy id */
	iret			/* Execute loaded program in map 2 */
				/*  and return to address pushed on stack */


xtrap:
ptrap:
ctrap:
strap:
	ldctl	r1,FCW		/* Change to non-segmented mode */
	res	r1,$S_SEG	/* Turn off segmented mode */
	ldctl	FCW,r1
	sub	r15,$20		/* Put stack pointer back past registers */
	ldm	4(r15),r0,$8	/* Save registers on stack */
/*
 * stack immediately prior to call to trap:
 *	<empty>		<- SP
 *	<empty>
 *	[r0]
 *	[r1]
 *	[r2]
 *	[r3]
 *	[r4]
 *	[r5]
 *	[r6]
 *	[r7]
 *	<trap id>	<- SP on entry to this routine
 *	<fcw at time of trap>
 *	<pc segment>
 *	<pc offset>
 */
	call	__trap		/* Handle trap */
	ldm	r0,4(r15),$8	/* Restore registers from stack */
	add	r15,$20		/* Set stack back to where it was */
	push	*r15,r1		/* Save r1 so we can use it for ldctl */
	ldctl	r1,FCW		/* Change back to segmented mode */
	set	r1,$S_SEG	/* Set segmented bit */
	ldctl	FCW,r1
	pop	r1,*r15		/* Restore r1 */
	iret			/* Return from trap */

mtrap:
	ldctl	r1,FCW		/* Change to non-segmented mode */
	res	r1,$S_SEG	/* Turn off segmented mode */
	ldctl	FCW,r1
	call	_pfail		/* Go print power fail message */
	jp	_start		/* Try to start again just in case */

vtrap:
	ldm	0x8000,r0,$16	/* Put registers where the prom can see them */
	.word	0x8600
	pop	r2,*r14		/* Take off id */
	pop	r2,*r14		/* Get old FCW */
	ld	0x8000,r2	/* Let prom get at it */
	.word	0x8624
	popl	rr2,*r14	/* Get trapped pc */
	ldl	0x8000,rr2	/* Let prom get at it */
	.word	0x8620
	ld	r14,$0		/* Move back to prom */
	ld	r15,$0x8800
	push	*r14,$0x1000
	push	*r14,$0
	push	*r14,$0x4000
	push	*r14,$0x0
	iret			/* Set up for iret, go back to prom */

.globl _spin
_spin:
	clr	r2
	ldl	rr4,rr14
	inc	r5,$4
	ld	r3,*r4
	multl	rq0,$0x880
spinloop:
	subl	rr2,$1
	testl	rr2
	jr	nz,spinloop
	ret

.globl _absjump
_absjump:
	ldl	rr2,4(r15)	/* Get offset of where to go */
	ldb	rh2,3(r15)	/* Get segment number */
	ldctl	r0,FCW		/* Retrieve current FCW */
	set	r0,$S_SEG	/* Set segmented mode bit */
	ldctl	FCW,r0		/* Set FCW */
	pushl	*r14,rr2	/* Push address of entry location */
	push	*r14,$0x4800	/* Push FCW with nonsegmented, system mode */
	push	*r14,$0		/* Push dummy id */
	iret			/* Simulated iret data on stack,so go for it */

.globl csav
.globl csav0
.globl csav1
.globl csav2
.globl csav3
.globl csav4
.globl csav5
.globl csv
.globl csv0
.globl csv1
.globl csv2
.globl csv3
.globl csv4
.globl csv5
csav:
csav0:
csav1:
csav2:
csav3:
csav4:
csav5:
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


.globl switch1
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

/*
 * out_multibus(port, value)
 * in_multibus(port)
 * out_local(port, value)
 * in_local(port)
 *
 * These four routines do word I/O instructions to the normal and special
 *  I/O spaces.  Special I/O space is used for local I/O and normal I/O
 *  space for multibus I/O.
 */

.globl _out_multibus
_out_multibus:
	ld	r1,2(r15)	/* r1 = port address */
	ld	r2,4(r15)	/* r2 = value */
	out	*r1,r2
	ret

.globl _in_multibus
_in_multibus:
	ld	r1,2(r15)	/* r1 = port address */
	in	r7,*r1
	ret

.globl _out_local
_out_local:
	ld	r1,2(r15)	/* r1 = port address */
	lda	r2,4(r15)	/* r2 = value */
	souti	*r1,*r2,r3	/* souti used since sout can't indirect */
	ret

.globl _in_local
_in_local:
	ld	r1,2(r15)	/* r1 = port address */
	lda	r2,-2(r15)	/* r2 = address of temp location */
	sini	*r2,*r1,r3	/* sini used since sin can't indirect */
	ld	r7, -2(r15)
	ret

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
	
.globl _xmemld
_xmemld:
	ld	r1,2(r15)	/* Value to be loaded */
	ldl	rr2,4(r15)	/* Memory address */

	ldctl	r0,FCW		/* Turn on segmented mode */
	set	r0,$S_SEG
	ldctl	FCW,r0

	ld	*r2,r1		/* Store value into memory */
	
	res	r0,$S_SEG	/* Turn off segmented mode */
	ldctl	FCW,r0
	ret

.globl _xmemrd
_xmemrd:
	ldl	rr2,2(r15)	/* Memory address */

	ldctl	r0,FCW		/* Turn on segmented mode */
	set	r0,$S_SEG
	ldctl	FCW,r0

	ld	r7,*r2		/* Get value from memory */
	
	res	r0,$S_SEG	/* Turn off segmented mode */
	ldctl	FCW,r0
	ret			/* Return with value in r7 */

.globl _bxmemld
_bxmemld:
	ld	r1,2(r15)	/* Value to be loaded */
	ldl	rr2,4(r15)	/* Memory address */

	ldctl	r0,FCW		/* Turn on segmented mode */
	set	r0,$S_SEG
	ldctl	FCW,r0

	ldb	*r2,rl1		/* Store value into memory */
	
	res	r0,$S_SEG	/* Turn off segmented mode */
	ldctl	FCW,r0
	ret

.globl _bxmemrd
_bxmemrd:
	ldl	rr2,2(r15)	/* Memory address */

	ldctl	r0,FCW		/* Turn on segmented mode */
	set	r0,$S_SEG
	ldctl	FCW,r0

	ldb	rl7,*r2		/* Get value from memory */
	
	res	r0,$S_SEG	/* Turn off segmented mode */
	ldctl	FCW,r0
	ret			/* Return with value in r7 */

.globl _clrpage
_clrpage:
	ldl	rr2,2(r15)	/* Get segment and page number */
	sll	r2,$8		/* Shift segment number to get segment adx */
	sll	r3,$11		/* Shift page number to get offset */
	ldl	rr4,rr2		/* Make another copy for ldir */
	ld	r1,$(PAGESIZE/2-1) /* Number of words in a page minus one */
	ldctl	r0,FCW		/* Turn on segmented mode */
	set	r0,$S_SEG
	ldctl	FCW,r0
	clr	*r4		/* Clear first word in page */
	inc	r3,$2		/* Increment address */
	ldir	*r2,*r4,r1	/* Clear segment */
	res	r0,$S_SEG	/* Turn off segmented mode */
	ldctl	FCW,r0
	ret

	/* blkmove(from_dataseg,from_offset,to_dataseg,to_offset,count)
  	 * int long from_dataseg,from_offset,to_dataseg,to_offset; int count;
	 */
.globl _blkmove	
_blkmove:
	ldl	rr2,2(r15)	/* From location */
	ldl	rr4,6(r15)	/* To location */
	ld	r6,10(r15)	/* Byte count */
	srl	r6,$1		/* Change byte count to word count */
	ldctl	r1,FCW		/* Change to segmented mode */
	ld	r0,r1
	set	r1,$S_SEG
	ldctl	FCW,r1
	ldir	*r4,*r2,r6	/* Do block move */

	ldctl	FCW,r0		/* Change back to nonsegmented mode */
	ret

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
clrbss:
	ld	r5,$_edata	/* Address of first word */
	clr	r4
/*	ld	rr2,$(_end-_edata)/2-1	/* Size of bss in words minus one */
	ld	r3,$_end
	clr	r2
	subl	rr2,rr4
	srl	r3
	dec	r3,$1
	clr	*r5		/* Zero first word */
	ld	r4,$_edata+2
	ldir	*r4,*r5,r3	/* Zero all */
	ret
	

saved_sp:	. = .+4
saved_pc:	. = .+4
saved_fcw:	. = .+2
lastpage:	. = .+2
	/* Global data variables must be in data space so that */
	/* C programs compiled split I&D can access them */
	/* Don't use ldr instructions to access them */
.data
_segno:		. = .+2
_dataseg:	. = .+2
_pages:		. = .+2

textmap:	.=.+4
.bss
_environ:	. = .+2
argv00:		. = .+2
argv0:		. = .+2
