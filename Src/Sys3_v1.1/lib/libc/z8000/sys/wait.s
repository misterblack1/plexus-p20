#include <syscall.h>

/*
 * wait
 */

.globl	_wait
.globl	cerror

_wait:
push	*r15, r14
ld	r14, r15
sc	$WAIT
jp	c, cerror
ld	r7, 4(r14)
test	r7
jp	z, t1
ld	*r7, r1

t1:
ld	r7, r0
pop	r14, *r15
ret
