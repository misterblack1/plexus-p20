#include <syscall.h>

/*
 * profil
 */

.globl	_profil
.globl	cerror

_profil:
push	*r15, r14
ld	r14, r15
ld	r7, 4(r14)
ld	t0, r7
ld	r7, 6(r14)
ld	t0+2, r7
ld	r7, 8(r14)
ld	t0+4, r7
ld	r7, 10(r14)
ld	t0+6, r7
sc	$INDIR
.word	t9
jp	c, cerror
clr	r7
pop	r14, *r15
ret

.data

t9:
sc	$PROFIL

t0:
.word	0
.word	0
.word	0
.word	0
