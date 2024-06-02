#include <syscall.h>

/*
 * dup
 */

.globl	_dup
.globl	cerror

_dup:
push	*r15, r14
ld	r14, r15
ld	r0, 4(r14)
ld	r1, 6(r14)
sc	$DUP
jp	c, cerror
ld	r7, r0
pop	r14, *r15
ret
