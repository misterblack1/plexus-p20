#include <syscall.h>

/*
 * execle
 */

.globl	_execle
.globl	cerror

_execle:
push	*r15, r14
ld	r14, r15
ld	r7, 4(r14)
ld	t0, r7
lda	r7, 6(r14)		/* r7 = address of argument list */
ld	t0+2, r7
clr	r5			/* r5 = arg list terminator (0) */
clr	r6			/* r6 = count (0 is really 32768) */
cpir	r5, *r7, r6, eq		/* search until list terminator found */
ld	r7, 2(r7)		/* get word after arg list terminator */
ld	t0+4, r7		/* make it the environ arg */
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
