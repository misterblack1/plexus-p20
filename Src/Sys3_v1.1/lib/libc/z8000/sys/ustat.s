#include <syscall.h>

/*
 * ustat
 */

.globl	_ustat
.globl	cerror
ustat=2

_ustat:
push	*r15, r14
ld	r14, r15
ld	r1, 4(r14)
ld	r0, 6(r14)
sc	$PWBSYS
.word	ustat
jp	c, cerror
clr	r7
pop	r14, *r15
ret
