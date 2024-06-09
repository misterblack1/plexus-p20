#include <syscall.h>

/*
 * lseek
 */

.globl	_lseek
.globl	cerror

_lseek:
push	*r15, r14
ld	r14, r15
ld	r0, 4(r14)
ldl	rr6, 6(r14)
ldl	t0, rr6
ld	r7, 10(r14)
ld	t0+4, r7
sc	$INDIR
.word	t9
jp	c, cerror
ldl	rr6, rr0
pop	r14, *r15
ret

.data

t9:
sc	$LSEEK

t0:
.word	0, 0
.word	0
