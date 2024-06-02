#include <syscall.h>

/*
 * stty
 */

.globl	_getpgrp
.globl	_setpgrp
.globl	cerror

_setpgrp:
ld	r0,$1
jr	t0

_getpgrp:
clr	r0

t0:
push	*r15, r14
ld	r14, r15
sc	$SETPGRP
jp	c, cerror
ld	r7,r0
pop	r14, *r15
ret

