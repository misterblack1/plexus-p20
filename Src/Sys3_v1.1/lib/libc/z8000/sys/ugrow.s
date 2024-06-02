#include <syscall.h>
/*
 *	ugrow
 */

.globl	_ugrow
.globl	__endstk
.globl	cerror

_ugrow:

push	*r15, r14
ld	r14, r15
ld	r0, 4(r14)
ld	t1, r0			/* save for later */
sc	$UGROW			/* grow stack */
jp	c, cerror
clr	r7
ld	r0,t1
ld	__endstk,r0		/* update for csav */
pop	r14, *r15
ret

.data
t1:
	.=.+2
