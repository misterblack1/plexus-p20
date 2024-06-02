#include "sys/plexus.h"

/*
 * This code handles C call and return. No stack overflow check is
 * made since there should be no stack overflows in system mode.
 */

#ifndef	OVKRNL

.globl csv,csav
csv:
#ifdef DEBUG
.globl csv0, csv1, csv2, csv3, csv4, csv5
csv0:
csv1:
csv2:
csv3:
csv4:
csv5:
#endif
csav:
ld	r1, *r15	/* r1 = return address */
ld	*r15, r14	/* save old frame ptr */
ld	r14, r15	/* set up new frame ptr */
sub	r15, r0		/* make room for locals and registers */
ldm	-LENSAVEREG(r14), FIRSTSAVEREG, $NUMSAVEREG	/* save registers */
#ifdef DEBUG
ld	r8,$0x8888
ld	r9,$0x9999
ld	r10,$0xaaaa
ld	r11,$0xbbbb
ld	r12,$0xcccc
ld	r13,$0xdddd
#endif
call	*r1		/* return and add one word to the stack */

.globl cret
cret:
#ifdef DEBUG
.globl cret0, cret1, cret2, cret3, cret4, cret5
cret0:
cret1:
cret2:
cret3:
cret4:
cret5:
#endif
ldm	FIRSTSAVEREG, -LENSAVEREG(r14), $NUMSAVEREG	/* restore registers */
ld	r15, r14	/* move stack top to previous frame */
pop	r14, *r15	/* restore old previous frame ptr */
ret			/* return thru the address on the stack which
			   was placed there during the original 
			   call. */

#ifndef DEBUG
.globl csv0
.globl cret0

csv0:
ld	r1, *r15		/* r1 = return address */
ld	*r15, r14		/* save old frame ptr */
ld	r14, r15		/* set up new frame ptr */
sub	r15, r0			/* allocate storage for frame locals */
call	*r1			/* return and add one word to the stack */

cret0:
ld	r15, r14		/* move stack top to previous frame */
pop	r14, *r15		/* restore old previous frame ptr */
ret				/* return thru the address on the stack which
				/*  was placed there during the original
				/*  function call. */


.globl csv1
.globl cret1

csv1:
ld	r1, *r15		/* r1 = return address */
ld	*r15, r14		/* save old frame ptr */
ld	r14, r15		/* set up new frame ptr */
sub	r15, r0			/* allocate storage for frame locals */
ldm	-2(r14), r13, $1	/* save registers */
call	*r1			/* return and add one word to the stack */

cret1:
ldm	r13, -2(r14), $1	/* restore registers */
ld	r15, r14		/* move stack top to previous frame */
pop	r14, *r15		/* restore old previous frame ptr */
ret				/* return thru the address on the stack which
				/*  was placed there during the original
				/*  function call. */


.globl csv2
.globl cret2

csv2:
ld	r1, *r15		/* r1 = return address */
ld	*r15, r14		/* save old frame ptr */
ld	r14, r15		/* set up new frame ptr */
sub	r15, r0			/* allocate storage for frame locals */
ldm	-4(r14), r12, $2	/* save registers */
call	*r1			/* return and add one word to the stack */

cret2:
ldm	r12, -4(r14), $2	/* restore registers */
ld	r15, r14		/* move stack top to previous frame */
pop	r14, *r15		/* restore old previous frame ptr */
ret				/* return thru the address on the stack which
				/*  was placed there during the original
				/*  function call. */


.globl csv3
.globl cret3

csv3:
ld	r1, *r15		/* r1 = return address */
ld	*r15, r14		/* save old frame ptr */
ld	r14, r15		/* set up new frame ptr */
sub	r15, r0			/* allocate storage for frame locals */
ldm	-6(r14), r11, $3	/* save registers */
call	*r1			/* return and add one word to the stack */

cret3:
ldm	r11, -6(r14), $3	/* restore registers */
ld	r15, r14		/* move stack top to previous frame */
pop	r14, *r15		/* restore old previous frame ptr */
ret				/* return thru the address on the stack which
				/*  was placed there during the original
				/*  function call. */


.globl csv4
.globl cret4

csv4:
ld	r1, *r15		/* r1 = return address */
ld	*r15, r14		/* save old frame ptr */
ld	r14, r15		/* set up new frame ptr */
sub	r15, r0			/* allocate storage for frame locals */
ldm	-8(r14), r10, $4	/* save registers */
call	*r1			/* return and add one word to the stack */

cret4:
ldm	r10, -8(r14), $4	/* restore registers */
ld	r15, r14		/* move stack top to previous frame */
pop	r14, *r15		/* restore old previous frame ptr */
ret				/* return thru the address on the stack which
				/*  was placed there during the original
				/*  function call. */


.globl csv5
.globl cret5

csv5:
ld	r1, *r15		/* r1 = return address */
ld	*r15, r14		/* save old frame ptr */
ld	r14, r15		/* set up new frame ptr */
sub	r15, r0			/* allocate storage for frame locals */
ldm	-10(r14), r9, $5	/* save registers */
call	*r1			/* return and add one word to the stack */

cret5:
ldm	r9, -10(r14), $5	/* restore registers */
ld	r15, r14		/* move stack top to previous frame */
pop	r14, *r15		/* restore old previous frame ptr */
ret				/* return thru the address on the stack which
				/*  was placed there during the original
				/*  function call. */
#endif

#else
.globl	__ovtb
.globl	__ovnum
.globl	__ovseg
.globl csv,csav
csv:	
#ifdef DEBUG
.globl	csv0, csv1, csv2, csv3, csv4, csv5
csv0:
csv1:
csv2:
csv3:
csv4:
csv5:
#endif
csav:
bit	r14,$0		/* test bit 0 of r14 */
jr	nz,cxclr	/* jif set to clear bit, r2 has overlay number */
ld	r2,__ovnum	/* r2 needs to have overlay number */
ldctl	r3,fcw
cxclr:
res	r14,$0		/* clear the bit */
ldctl	fcw,r3		/* reset fcw with interrupts on if they were */
ld	r1, *r15	/* r1 = return address */
ld	*r15, r14	/* save old frame ptr */
ld	r14, r15	/* set up new frame ptr */
sub	r15, r0		/* make room for locals and registers */
push	*r15,r2		/* save old overlay number */
ldm	-LENSAVEREG(r14), FIRSTSAVEREG, $NUMSAVEREG	/* save registers */
#ifdef	DEBUG
ld	r8,$0x8888
ld	r9,$0x9999
ld	r10,$0xaaaa
ld	r11,$0xbbbb
ld	r12,$0xcccc
ld	r13,$0xdddd
#endif
call	*r1		/* return and add one word to the stack */

.globl cret
cret:
#ifdef	DEBUG
.globl	cret0, cret1, cret2, cret3, cret4, cret5
cret0:
cret1:
cret2:
cret3:
cret4:
cret5:
#endif
ldctl	r1,fcw			/* save old fcw */
di	vi			/* disable vectored interrupts */
ldm	FIRSTSAVEREG, -LENSAVEREG(r14), $NUMSAVEREG	/* restore registers */
inc	r15,$2		/* points to overlay number */
ld	r2,*r15		/* get overlay number into r2 */
ld	r15, r14	/* move stack top to previous frame */
pop	r14, *r15	/* restore old previous frame ptr */
jp	ovcret		/* return through common overlay return code */

#ifndef	DEBUG
.globl csv0
.globl cret0

csv0:
bit	r14,$0		/* test bit 0 of r14 */
jr	nz,c0clr	/* jif set to clear bit, r2 has overlay number */
ld	r2,__ovnum	/* r2 needs to have overlay number */
ldctl	r3,fcw
c0clr:
res	r14,$0		/* clear the bit */
ldctl	fcw,r3		/* reset fcw with interrupts on if they were */
ld	r1, *r15		/* r1 = return address */
ld	*r15, r14		/* save old frame ptr */
ld	r14, r15		/* set up new frame ptr */
sub	r15, r0			/* allocate storage for frame locals */
push	*r15,r2			/* save old overlay number */
call	*r1			/* return and add one word to the stack */


cret0:
ldctl	r1,fcw			/* save old fcw */
di	vi			/* disable vectored interrupts */
inc	r15,$2			/* get stack to point to old overlay number */
ld	r2,*r15			/* r2 = old overlay number */
ld	r15, r14		/* move stack top to previous frame */
pop	r14, *r15		/* restore old previous frame ptr */
jp	ovcret


.globl csv1
.globl cret1

csv1:
bit	r14,$0		/* test bit 0 of r14 */
jr	nz,c1clr	/* jif set to clear bit, r2 has overlay number */
ld	r2,__ovnum	/* r2 needs to have overlay number */
ldctl	r3,fcw
c1clr:
res	r14,$0		/* clear the bit */
ldctl	fcw,r3		/* reset fcw with interrupts on if they were */
ld	r1, *r15		/* r1 = return address */
ld	*r15, r14		/* save old frame ptr */
ld	r14, r15		/* set up new frame ptr */
sub	r15, r0			/* allocate storage for frame locals */
push	*r15,r2			/* save old overlay number */
ldm	-2(r14), r13, $1	/* save registers */
call	*r1			/* return and add one word to the stack */

cret1:
ldctl	r1,fcw			/* save old fcw */
di	vi			/* disable vectored interrupts */
ldm	r13, -2(r14), $1	/* restore registers */
inc	r15,$2			/* get stack to point to old overlay number */
ld	r2,*r15			/* r2 = old overlay number */
ld	r15, r14		/* move stack top to previous frame */
pop	r14, *r15		/* restore old previous frame ptr */
jr	ovcret


.globl csv2
.globl cret2

csv2:
bit	r14,$0		/* test bit 0 of r14 */
jr	nz,c2clr	/* jif set to clear bit, r2 has overlay number */
ld	r2,__ovnum	/* r2 needs to have overlay number */
ldctl	r3,fcw
c2clr:
res	r14,$0		/* clear the bit */
ldctl	fcw,r3		/* reset fcw with interrupts on if they were */
ld	r1, *r15		/* r1 = return address */
ld	*r15, r14		/* save old frame ptr */
ld	r14, r15		/* set up new frame ptr */
sub	r15, r0			/* allocate storage for frame locals */
push	*r15,r2			/* save old overlay number */
ldm	-4(r14), r12, $2	/* save registers */
call	*r1			/* return and add one word to the stack */

cret2:
ldctl	r1,fcw			/* save old fcw */
di	vi			/* disable vectored interrupts */
ldm	r12, -4(r14), $2	/* restore registers */
inc	r15,$2			/* get stack to point to old overlay number */
ld	r2,*r15			/* r2 = old overlay number */
ld	r15, r14		/* move stack top to previous frame */
pop	r14, *r15		/* restore old previous frame ptr */
jr	ovcret

.globl csv3
.globl cret3

csv3:
bit	r14,$0		/* test bit 0 of r14 */
jr	nz,c3clr	/* jif set to clear bit, r2 has overlay number */
ld	r2,__ovnum	/* r2 needs to have overlay number */
ldctl	r3,fcw
c3clr:
res	r14,$0		/* clear the bit */
ldctl	fcw,r3		/* reset fcw with interrupts on if they were */
ld	r1, *r15		/* r1 = return address */
ld	*r15, r14		/* save old frame ptr */
ld	r14, r15		/* set up new frame ptr */
sub	r15, r0			/* allocate storage for frame locals */
push	*r15,r2			/* save old overlay number */
ldm	-6(r14), r11, $3	/* save registers */
call	*r1			/* return and add one word to the stack */

cret3:
ldctl	r1,fcw			/* save old fcw */
di	vi			/* disable vectored interrupts */
ldm	r11, -6(r14), $3	/* restore registers */
inc	r15,$2			/* get stack to point to old overlay number */
ld	r2,*r15			/* r2 = old overlay number */
ld	r15, r14		/* move stack top to previous frame */
pop	r14, *r15		/* restore old previous frame ptr */
jr	ovcret


.globl csv4
.globl cret4

csv4:
bit	r14,$0		/* test bit 0 of r14 */
jr	nz,c4clr	/* jif set to clear bit, r2 has overlay number */
ld	r2,__ovnum	/* r2 needs to have overlay number */
ldctl	r3,fcw
c4clr:
res	r14,$0		/* clear the bit */
ldctl	fcw,r3		/* reset fcw with interrupts on if they were */
ld	r1, *r15		/* r1 = return address */
ld	*r15, r14		/* save old frame ptr */
ld	r14, r15		/* set up new frame ptr */
sub	r15, r0			/* allocate storage for frame locals */
push	*r15,r2			/* save old overlay number */
ldm	-8(r14), r10, $4	/* save registers */
call	*r1			/* return and add one word to the stack */

cret4:
ldctl	r1,fcw			/* save old fcw */
di	vi			/* disable vectored interrupts */
ldm	r10, -8(r14), $4	/* restore registers */
inc	r15,$2			/* get stack to point to old overlay number */
ld	r2,*r15			/* r2 = old overlay number */
ld	r15, r14		/* move stack top to previous frame */
pop	r14, *r15		/* restore old previous frame ptr */
jr	ovcret

.globl csv5
.globl cret5

csv5:
bit	r14,$0		/* test bit 0 of r14 */
jr	nz,c5clr	/* jif set to clear bit, r2 has overlay number */
ld	r2,__ovnum	/* r2 needs to have overlay number */
ldctl	r3,fcw
c5clr:
res	r14,$0		/* clear the bit */
ldctl	fcw,r3		/* reset fcw with interrupts on if they were */
ld	r1, *r15		/* r1 = return address */
ld	*r15, r14		/* save old frame ptr */
ld	r14, r15		/* set up new frame ptr */
sub	r15, r0			/* allocate storage for frame locals */
push	*r15,r2			/* save old overlay number */
ldm	-10(r14), r9, $5	/* save registers */
call	*r1			/* return and add one word to the stack */

cret5:
ldctl	r1,fcw			/* save old fcw */
di	vi			/* disable vectored interrupts */
ldm	r9, -10(r14), $5	/* restore registers */
inc	r15,$2			/* get stack to point to old overlay number */
ld	r2,*r15			/* r2 = old overlay number */
ld	r15, r14		/* move stack top to previous frame */
pop	r14, *r15		/* restore old previous frame ptr */

#endif
ovcret:
cp	r2,__ovnum		/* compare return to overlay number with */
				/* current overlay number */
jr	eq,ovclab		/* just return if the same */
ld	__ovnum,r2		/* change to new overlay number */
sla	r2,$1
ld	r2,__ovtb(r2)		/* new segment number */
ld	__ovseg,r2		/* save new segment number */
lda	r3,ovclab		/* where we are jumping to */
ld	r0,$(B_SYS+B_SEG+B_NVI)
ldctl	fcw,r0			/* into segmented mode */
jp	*r2			/* switch segment */

ovclab:
ldctl	fcw,r1			/* restore fcw */
ret				/* return to caller */
#endif

/*
 * This code is used by cc compilers to implement switch statements.
 * At the entrance to either switch1 or switch2 r7 contains the switch
 * index value and r6 contains the address, in the data area, of the
 * control block for the switch statement. The switch control block is
 * as follows:
 *
 * switch1:
 *
 *	0)	lowest case label value (clow)
 *	1)	highest case label value (chigh)
 *	2)	code address of default case.
 *	3)	(chigh - clow + 1) code addresses for the
 *		cases from clow to chigh in ascending order by
 *		case value.
 *
 * switch2:
 *
 *	0)	number of cases + 1 (n + 1)
 *	1)	n case values.
 *	n+1)	n case code addresses.
 *	2*n+1)	default case code address.
 */

.globl	switch1
switch1:
ld	r5, 4(r6)		/* r5 = default code address */
cp	r7, 2(r6)
jp	gt, *r5			/* out of case range on high end */
sub	r7, *r6
jp	lt, *r5			/* out of case range on low end */
add	r6, r7
add	r6, r7			/* r6 = index into case code addresses */
ld	r5, 6(r6)		/* r5 = case code address */
jp	*r5			/* jump to case */

.globl	switch2
switch2:
ld	r5, *r6			/* r5 = number of cases + 1 */
ld	r4, r5			/* r4 = number of cases + 1 */
inc	r6, $2			/* r6 = ptr to case value list */
cpir	r7, *r6, r5, eq		/* look for correct case value */
add	r6, r4
add	r6, r4			/* r6 = address of correct code address */
ld	r5, -4(r6)		/* r5 = correct case code address */
jp	*r5			/* jump to case */


