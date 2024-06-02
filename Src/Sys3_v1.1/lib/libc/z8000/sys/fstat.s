#include <syscall.h>

/*
 * fstat
 */

.globl	_fstat
.globl	cerror

_fstat:
push	*r15, r14
ld	r14, r15
ld	r0, 4(r14)
ld	r7, 6(r14)
ld	t0, r7
sc	$INDIR
.word	t9
jp	c, cerror
clr	r7
pop	r14, *r15
ret

.data

t9:
sc	$FSTAT

t0:
.word	0
