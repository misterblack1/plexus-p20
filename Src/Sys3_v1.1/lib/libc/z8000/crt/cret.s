#define	ASLANG 1
#include <sys/param.h>
#include <sys/plexus.h>
#include <syscall.h>

/*
 * This code handles C call and return. The difference from the
 * PDP 11 version is that extra code is executed to check if
 * a new stack page must be allocated.
 */

.globl cret
.globl __endstk

cret:
ldm	FIRSTSAVEREG, -LENSAVEREG(r14), $NUMSAVEREG	/* restore registers */
ld	r15, r14		/* move stack top to previous frame */
pop	r14, *r15		/* restore old previous frame ptr */
ret				/* return thru the address on the stack which
				   was placed there during original function 
				   call. */

.globl cret0


cret0:
ld	r15, r14		/* move stack top to previous frame */
pop	r14, *r15		/* restore old previous frame ptr */
ret				/* return thru the address on the stack which
				   was placed there during original function 
				   call. */


.globl cret1


cret1:
ldm	r13, -2(r14), $1	/* restore registers */
ld	r15, r14		/* move stack top to previous frame */
pop	r14, *r15		/* restore old previous frame ptr */
ret				/* return thru the address on the stack which
				   was placed there during original function 
				   call. */


.globl cret2


cret2:
ldm	r12, -4(r14), $2	/* restore registers */
ld	r15, r14		/* move stack top to previous frame */
pop	r14, *r15		/* restore old previous frame ptr */
ret				/* return thru the address on the stack which
				   was placed there during original function 
				   call. */


.globl cret3


cret3:
ldm	r11, -6(r14), $3	/* restore registers */
ld	r15, r14		/* move stack top to previous frame */
pop	r14, *r15		/* restore old previous frame ptr */
ret				/* return thru the address on the stack which
				   was placed there during original function 
				   call. */


.globl cret4


cret4:
ldm	r10, -8(r14), $4	/* restore registers */
ld	r15, r14		/* move stack top to previous frame */
pop	r14, *r15		/* restore old previous frame ptr */
ret				/* return thru the address on the stack which
				   was placed there during original function 
				   call. */


.globl cret5


cret5:
ldm	r9, -10(r14), $5	/* restore registers */
ld	r15, r14		/* move stack top to previous frame */
pop	r14, *r15		/* restore old previous frame ptr */
ret				/* return thru the address on the stack which
				   was placed there during original function 
				   call. */


