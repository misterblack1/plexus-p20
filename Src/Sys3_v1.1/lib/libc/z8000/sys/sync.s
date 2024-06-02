#include <syscall.h>

/*
 * sync
 */

.globl	_sync

_sync:
push	*r15, r14
ld	r14, r15
sc	$SYNC
pop	r14, *r15
ret
