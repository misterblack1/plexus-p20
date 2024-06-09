#include <syscall.h>

/*
 * setuid
 */

.globl	_setuid
.globl	cerror

_setuid:
push	*r15, r14
ld	r14, r15
ld	r0, 4(r14)
sc	$SETUID
jp	c, cerror
clr	r7
pop	r14, *r15
ret
