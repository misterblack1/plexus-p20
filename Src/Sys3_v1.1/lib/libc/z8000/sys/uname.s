#include <syscall.h>

/*
 * uname
 */

.globl	_uname
.globl	cerror
uname=0

_uname:
push	*r15, r14
ld	r14, r15
ld	r0, 4(r14)
sc	$PWBSYS
.word	uname
jp	c, cerror
ld	r7, r0
pop	r14, *r15
ret
