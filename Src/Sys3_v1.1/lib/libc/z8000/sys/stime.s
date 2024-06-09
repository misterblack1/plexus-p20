#include <syscall.h>

/*
 * stime
 */

.globl	_stime
.globl	cerror

_stime:
push	*r15, r14
ld	r14, r15
ld	r7, 4(r14)
ldl	rr0, *r7
sc	$STIME
jp	c, cerror
clr	r7
pop	r14, *r15
ret
