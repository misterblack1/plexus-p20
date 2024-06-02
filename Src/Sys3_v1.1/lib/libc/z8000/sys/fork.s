#include <syscall.h>

/*
 * fork
 */

.globl	_fork
.globl	cerror
.globl	_par_uid

_fork:
push	*r15, r14
ld	r14, r15
sc	$FORK
jr	t1
jr	nc, t2
jp	cerror

t1:
ld	_par_uid, r0
clr	r0

t2:
ld	r7, r0
pop	r14, *r15
ret

.bss

_par_uid:
. = . + 2
