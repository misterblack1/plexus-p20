#include <syscall.h>

/*
 * close
 */

.globl	_close
.globl	cerror

_close:
push	*r15, r14
ld	r14, r15
ld	r0, 4(r14)
sc	$CLOSE
jp	c, cerror
clr	r7
pop	r14, *r15
ret
