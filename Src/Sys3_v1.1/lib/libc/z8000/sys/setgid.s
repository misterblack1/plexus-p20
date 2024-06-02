#include <syscall.h>

/*
 * setgid
 */

.globl	_setgid
.globl	cerror

_setgid:
push	*r15, r14
ld	r14, r15
ld	r0, 4(r14)
sc	$SETGID
jp	c, cerror
clr	r7
pop	r14, *r15
ret
