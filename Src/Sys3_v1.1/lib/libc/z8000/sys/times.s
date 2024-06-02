#include <syscall.h>

/*
 * times
 */

.globl	_times

_times:
push	*r15, r14
ld	r14, r15
ld	r7, 4(r14)
ld	t0, r7
sc	$INDIR
.word	t9
jp	c, cerror
ldl	rr6,rr0
pop	r14, *r15
ret

.data

t9:
sc	$TIMES

t0:
.word	0
