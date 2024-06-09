#include <syscall.h>

/*
 * stty
 */

.globl	_stty
.globl	cerror

_stty:
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
sc	$STTY

t0:
.word	0
