#include <sys/plexus.h>
/*
 * setjmp, longjmp
 */

.globl	_setjmp
.globl	_longjmp
.globl	csav
.globl	cret

_setjmp:
ldk	r0, $LENSAVEREG
call	csav
ld	r5, *r14		/* r5 = frame ptr of caller */
lda	r4, 2(r14)		/* r4 = stk ptr at entrance to setjmp */
ld	r3, 2(r14)		/* r3 = next code address in caller of setjmp */
ld	r6, 4(r14)		/* r6 = env ptr */
ld	*r6, r5			/* store state for later call to longjmp */
ld	2(r6), r4
ld	4(r6), r3
clr	r7			/* original call to setjmp returns 0 */
jp	cret


_longjmp:
ldk	r0, $LENSAVEREG
call	csav
ld	r6, 4(r14)		/* r6 = ptr to env */
ld	r7, 6(r14)		/* r7 = value to be returned */
test	r7
jr	ne, t1
inc	r7, $1			/* if return would be zero make it one
				   so that the caller can tell between the
				   original call to setjmp and the longjmp
				   return */

t1:
ld	r5, *r6			/* get values from env */
ld	r4, 2(r6)
ld	r3, 4(r6)

loop:
cp	r5, *r14
jr	eq, goodexit		/* this is where setjmp was called before */
ld	r14, *r14		/* chain back one frame */
test	r14
jr	ne, loop

/* we hit the end of the frame chain without getting to the exact place where
   setjmp was called. In this case we blindly set all pointers to where they
   were before and return without restoring the values of the reg variables
   since these are probably wrong */

ld	r14, r5
ld	r15, r4
ld	*r15, r3
ret

/*
 * Return in such a way that it looks like a return from setjmp.
 * Currently r14 is pointing to the marker which has the values of the
 * user registers (r8 - r13) for the caller of setjmp. We must do a cret
 * by hand here since the last routine called by the caller of setjmp may
 * not have had the same number of parameters as setjmp had.
 */

goodexit:
ldm	FIRSTSAVEREG, -LENSAVEREG(r14), $NUMSAVEREG	/* restore user regs */
ld	r14, r5			/* restore callers frame ptr */
lda	r15, 2(r4)		/* set top of stk to point of call to setjmp */
jp	*r3			/* jump to next inst in caller of setjmp */
