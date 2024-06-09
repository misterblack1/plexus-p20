/*
 * C error return. Set _errno to value of r0. Also set
 * r7 = r6 = -1.
 */

.globl cerror

cerror:
ld	_errno, r0
ld	r6, $-1
ld	r7, r6
pop	r14, *r15
ret

.comm _errno, 2			/* global for error number */
