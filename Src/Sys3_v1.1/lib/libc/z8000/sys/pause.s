#include <syscall.h>

/*
 * pause
 */

.globl	_pause
.globl	cerror

_pause:
push	*r15, r14
ld	r14, r15
sc	$PAUSE
jp	c, cerror
clr	r7
pop	r14, *r15
ret
