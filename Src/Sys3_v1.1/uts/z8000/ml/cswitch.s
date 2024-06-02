#include "sys/plexus.h"

/*
 * save(buf)	
 * 
 * saves the information needed to restart the current 
 * user in `buf'.
 *
 * resume(ublockpage, buf)
 *
 * Sets `ublockpage' as the current user block and restarts the user
 * using the information in `buf'.
 */


.globl	_save
_save:
	pop	r7, *r15	/* save callers next code address */
	ld	r1, *r15	/* r1 = address of save area */
#ifndef	OVKRNL
	ldm	*r1, r7, $9	/* store next pc and r8 - r15 */
#else
/* The overlay number is also saved w/ the overlay kernel to resume
   in the correct overlay.
 */
	ld	r6,__ovnum	/* get current overlay number into r6 */
	ldm	*r1, r6, $10	/* store ovnum, next pc and r8 - r15 */
#endif
	ld	r6, r7
	clr	r7		/* return 0 for normal return */
	jp	*r6		/* return to caller */
	
.globl	_resume
_resume:
	ld	r0, 2(r15)	/* r0 = new user block phy page number */
	ld	r1, 4(r15)	/* r1 = address of restart info */
	di	vi,nvi		/* all interrupts off */
	sout	P_MUBLK, r0	/* set new user block */
#ifdef	OVKRNL
/*  The user block has to go into all overlay maps too! */
	sout	P_MUBLK1, r0	/* set new user block in overlay 1 too */
	sout	P_MUBLK2, r0	/* set new user block in overlay 2 too */
#endif
#ifndef	OVKRNL
	ldm	r7, *r1, $9	/* restore registers */
#else
/*  The overlay number was 'save'd.  Check the resuming process 
    overlay to see if it the same as the current, if not change to 
    correct overlay segment.
 */
	ldm	r6, *r1, $10	/* restore ovnum, pc, and r8 - r15 */
	cp	r6,__ovnum	/* compare saved ovnum w/ current */
	jr	eq, resskp	/* skip segment change if the same */
	ldctl	r1, fcw		/* save current fcw */
	ld	__ovnum, r6	/* set new overlay number */
	sla	r6, $1		/* make ovnum table offset */
	ld	r2, __ovtb(r6)	/* get new segment */
	ld	__ovseg, r2	/* set new current overlay segment */
	lda	r3, resjmp	/* local jump address into r3 */
	ld	r0, $(B_SYS+B_SEG+B_NVI)	/* new fcw into r0 */
	ldctl	fcw, r0		/* go into segmented mode */
	jp	*r2		/* change to new segment number */
resjmp:
	ldctl	fcw, r1		/* restore fcw */
resskp:
#endif
	ld	r6, r7
	ld	r7, $1		/* return 1 for resumption */
	ei	vi,nvi		/* all interrupts on */
	jp	*r6		/* return to caller */
	
	
.globl	_setjmp
_setjmp:
	pop	r7, *r15	/* save callers next code address */
	ld	r1, *r15	/* r1 = address of save area */
#ifndef OVKRNL
	ldm	*r1, r7, $9	/* store next pc and r8 - r15 */
#else
/*  The overlay number has to be saved also for the overlay kernel. */
	ld	r6,__ovnum	/* current overlay number */
	ldm	*r1, r6, $10	/* store ovnum, next pc and r8 - r15 */
#endif
	ld	r6, r7
	clr	r7		/* return 0 for normal return */
	jp	*r6		/* return to caller */
	
	
.globl	_longjmp
_longjmp:
	ld	r1, 2(r15)	/* r1 = address of restart info */
#ifndef OVKRNL
	ldm	r7, *r1, $9	/* restore registers */
#else
/*  The overlay number was 'save'd.  Check the resuming process 
    overlay to see if it the same as the current, if not change to 
    correct overlay segment.
 */
	ldm	r6, *r1, $10	/* restore ovnum, pc, and r8 - r15 */
	ldctl	r1,fcw
	di	vi,nvi
	cp	r6,__ovnum	/* compare saved ovnum w/ current */
	jr	eq, ljpskp	/* skip segment change if the same */
	ld	__ovnum, r6	/* set new overlay number */
	sla	r6, $1		/* make ovnum table offset */
	ld	r2, __ovtb(r6)	/* get new segment */
	ld	__ovseg, r2	/* set new current overlay segment */
	lda	r3, ljpjmp	/* local jump address into r3 */
	ld	r0, $(B_SYS+B_SEG+B_NVI)	/* new fcw into r0 */
	ldctl	fcw, r0		/* go into segmented mode */
	jp	*r2		/* change to new segment number */
ljpjmp:
ljpskp:
	ldctl	fcw, r1		/* restore fcw */
#endif
	ld	r6, r7
	ld	r7, $1		/* return 1 for resumption */
	jp	*r6		/* return to caller */
	
#ifdef	OVKRNL
.globl ovhndlr
.globl __ovnum
.globl __ovseg
.globl __ovtb
.globl __ovhc

ovhndlr:
	ldctl	r1,fcw
	ld	r6,*r15		/* debug where from */
	ld	__ovnum,r7	/* set new overlay number */
	sla	r7,$1		/* times 2 for word index */
	ld	r6,__ovtb(r7)	/* new overlay segment number */
	ld	__ovseg,r6	/* seg new overlay segment number */
	lda	r7,ovlab	/* segmented jump address */
	ld	r4,$(B_SYS+B_SEG+B_NVI)
	ldctl	fcw,r4		/* into segmented mode */
	jp	*r6		/* change sement numbers */
ovlab:
	ldctl	fcw,r1		/* restore fcw */
	ldl	rr6,__ovhc	/* get ovhndlr called count */
	addl	rr6,$1		/* bump it by one */
	ldl	__ovhc,rr6	/* save it */
	ret
.data
__ovnum:
.word	0x0
__ovseg:
.word	SDSEG
__ovhc:
.word	0x0
.word	0x0
__ovtb:
.word	SDSEG
.word	SDSEG1
.word	SDSEG2
.word	SDSEG3
.word	SDSEG4
.text
#endif
