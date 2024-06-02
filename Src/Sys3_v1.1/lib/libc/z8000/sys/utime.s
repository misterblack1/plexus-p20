#include <syscall.h>

/*
 * utime
 */

.globl	_utime
.globl	cerror

_utime:
push	*r15, r14
ld	r14, r15
ld	r7, 4(r14)
ld	t0, r7
ld	r7, 6(r14)
ld	t0+2, r7
sc	$INDIR
.word	t9
jp	c, cerror
clr	r7
pop	r14, *r15
ret

.data

t9:
sc	$UTIME

t0:
.word	0
.word	0
