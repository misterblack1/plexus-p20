#include <syscall.h>

/*
 * getppid
 */

.globl	_getppid

_getppid:
push	*r15, r14
ld	r14, r15
sc	$GETPID
ld	r7, r1
pop	r14, *r15
ret
