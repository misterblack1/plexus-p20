#include <syscall.h>

/*
 * time
 */

.globl	_time

_time:
push	*r15, r14
ld	r14, r15
sc	$TIME
jp	c, cerror
ld	r7, 4(r14)
test	r7
jp	z, t1
ldl	*r7, rr0

t1:
ldl	rr6, rr0
pop	r14, *r15
ret
