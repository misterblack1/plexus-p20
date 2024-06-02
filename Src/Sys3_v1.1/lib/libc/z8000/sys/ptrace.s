#include <syscall.h>

/*
 * ptrace
 */

.globl	_ptrace
.globl	cerror
.globl	_errno

_ptrace:
push	*r15, r14
ld	r14, r15
ld	r7, 4(r14)
ld	t0+4, r7
ld	r7, 6(r14)
ld	t0, r7
ld	r7, 8(r14)
ld	t0+2, r7
ld	r0, 10(r14)
clr	_errno
sc	$INDIR
.word	t9
jp	c, cerror
ld	r7, r0
pop	r14, *r15
ret

.data

t9:
sc	$PTRACE

t0:
.word	0
.word	0
.word	0
