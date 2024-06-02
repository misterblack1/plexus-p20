#include <syscall.h>
// C library -- general system call
//	syscall(sysnum, r0, r1, arg1, arg2, ...)
//		max of 5 args (size u.u_arg in system)

.globl	_syscall, cerror

_syscall:
push	*r15, r14
ld	r14, r15
ld	r3, $t9
ld	r0, 4(r14)
and	r0, $0x0ff	//construct sys call opcode
or	r0, $0x7f00
ld	*r3, r0
inc	r3, $2
ld	r0, 6(r14)
ld	r1, 8(r14)
lda	r2, 10(r14)
ld	r4, $5
ldir	*r3,*r2,r4
sc	$INDIR
.word	t9
ld	r7,r0
jp	c,cerror
pop	r14, *r15
ret

.data
t9:
sc	$INDIR		//will be changed by code
.word	0
.word	0
.word	0
.word	0
.word	0
