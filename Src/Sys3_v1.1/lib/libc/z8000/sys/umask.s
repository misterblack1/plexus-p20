#include <syscall.h>

/*
 * umask
 */

.globl	_umask
.globl	cerror

_umask:
push	*r15, r14
ld	r14, r15
ld	r7, 4(r14)
ld	t0, r7
sc	$INDIR
.word	t9
jp	c, cerror
ld	r7, r0
pop	r14, *r15
ret

.data

t9:
sc	$UMASK

t0:
.word	0
