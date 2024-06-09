#include <syscall.h>

/*
 * nice
 */

.globl	_nice
.globl	cerror

_nice:
push	*r15, r14
ld	r14, r15
ld	r0, 4(r14)
sc	$NICE
jp	c, cerror
ld	r7, r0
pop	r14, *r15
ret
