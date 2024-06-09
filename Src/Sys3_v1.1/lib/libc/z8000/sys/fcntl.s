#include <syscall.h>

/*
 * fcntl
 */

.globl	_fcntl
.globl	cerror

_fcntl:
push	*r15, r14
ld	r14, r15
ld	r0, 4(r14)
ld	r7, 6(r14)
ld	t0, r7
ld	r7, 8(r14)
ld	t0+2, r7
sc	$INDIR
.word	t9
ld	r7,r0
jp	c, cerror
pop	r14, *r15
ret

.data

t9:
sc	$FCNTL

t0:
.word	0
.word	0
