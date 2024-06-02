#include <syscall.h>

/*
 * ulimit
 */

.globl	_ulimit
.globl	cerror

_ulimit:
push	*r15, r14
ld	r14, r15
ld	r7, 4(r14)
ld	t0, r7
ld	r7, 6(r14)
ld	t0+2, r7
ld	r7, 8(r14)
ld	t0+4, r7
sc	$INDIR
.word	t9
ldl	rr6, rr0
jp	c, cerror
pop	r14, *r15
ret

.data

t9:
sc	$ULIMIT

t0:
.word	0
.word	0
.word	0
