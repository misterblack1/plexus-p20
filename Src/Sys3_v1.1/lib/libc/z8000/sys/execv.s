#include <syscall.h>

/*
 * execv
 */

.globl	_execv
.globl	cerror
.globl	_environ

_execv:
push	*r15, r14
ld	r14, r15
ld	r7, 4(r14)
ld	t0, r7
ld	r7, 6(r14)
ld	t0+2, r7
ld	r7, _environ
ld	t0+4, r7
sc	$INDIR
.word	t9
jp	cerror

.data

t9:
sc	$EXECE

t0:
.word	0
.word	0
.word	0
