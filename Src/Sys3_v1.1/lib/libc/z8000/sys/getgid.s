#include <syscall.h>

/*
 * getgid
 */

.globl	_getgid

_getgid:
push	*r15, r14
ld	r14, r15
sc	$GETGID
ld	r7, r0
pop	r14, *r15
ret
