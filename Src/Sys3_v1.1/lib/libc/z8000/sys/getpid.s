#include <syscall.h>

/*
 * getpid
 */

.globl	_getpid

_getpid:
push	*r15, r14
ld	r14, r15
sc	$GETPID
ld	r7, r0
pop	r14, *r15
ret
