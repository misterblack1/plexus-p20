#include <syscall.h>
// C library -- tell

// long tell(f) -- returns offset of file f.

.globl	_tell, cerror

_tell:
push	*r15, r14
ld	r14, r15
ld	r0, 4(r14)
sc	$LSEEK
.word	0
.word	0
.word	1
jp	c,cerror
ldl	rr6,rr0
pop	r14, *r15
ret
