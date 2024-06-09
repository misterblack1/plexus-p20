#include <syscall.h>

/*
 * sbrk, brk
 */

.globl	_sbrk
.globl	_brk
.globl	cerror
.globl	_end

_sbrk:
push	*r15, r14
ld	r14, r15
ld	r7, 4(r14)
test	r7
jp	z, t1
add	r7, nd
ld	t0, r7
sc	$INDIR
.word	t9
jp	c, cerror

t1:
ld	r7, nd
ld	r6, 4(r14)
add	r6, r7
ld	nd, r6
pop	r14, *r15
ret

_brk:
push	*r15, r14
ld	r14, r15
ld	r7, 4(r14)
ld	t0, r7
sc	$INDIR
.word	t9
jp	c, cerror
ld	r7, 4(r14)
ld	nd, r7
clr	r7
pop	r14, *r15
ret

.data

t9:
sc	$BREAK

t0:
.word	0

nd:
.word	_end
