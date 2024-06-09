#include <syscall.h>

/*
 * geteuid
 */

.globl	_geteuid

_geteuid:
push	*r15, r14
ld	r14, r15
sc	$GETUID
ld	r7, r1
pop	r14, *r15
ret
