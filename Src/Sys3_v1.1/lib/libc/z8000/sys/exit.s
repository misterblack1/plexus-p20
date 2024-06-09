#include <syscall.h>

/*
 * _exit
 */

.globl	__exit

__exit:
push	*r15, r14
ld	r14, r15
ld	r0, 4(r14)
sc	$EXIT
