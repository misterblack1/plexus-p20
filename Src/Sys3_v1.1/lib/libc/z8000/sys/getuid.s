#include <syscall.h>

/*
 * getuid
 */

.globl	_getuid

_getuid:
push	*r15, r14
ld	r14, r15
sc	$GETUID
ld	r7, r0
pop	r14, *r15
ret
