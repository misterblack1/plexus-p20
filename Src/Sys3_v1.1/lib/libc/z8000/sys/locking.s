#include <syscall.h>

/*
 * locking
 */

.globl	_locking
.globl	cerror

_locking:
push	*r15, r14
ld	r14, r15
ld	r0, 4(r14)
ld	r1, 6(r14)
ldl	rr2, 8(r14)
sc	$LOCKING
jp	c, cerror
clr	r7
pop	r14, *r15
ret
