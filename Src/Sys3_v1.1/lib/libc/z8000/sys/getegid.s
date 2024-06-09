#include <syscall.h>

/*
 * getegid
 */

.globl	_getegid

_getegid:
push	*r15, r14
ld	r14, r15
sc	$GETGID
ld	r7, r1
pop	r14, *r15
ret
