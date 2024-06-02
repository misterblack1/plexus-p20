#include <syscall.h>

/*
 * pipe
 */

.globl	_pipe
.globl	cerror

_pipe:
push	*r15, r14
ld	r14, r15
sc	$PIPE
jp	c, cerror
ld	r7, 4(r14)		/* address for result */
ld	*r7, r0			/* store read file descriptor */
ld	2(r7), r1		/* store write file descriptor */
clr	r7
pop	r14, *r15
ret
