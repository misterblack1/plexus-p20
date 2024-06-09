#include <syscall.h>

/*
 * alarm
 */

.globl	_alarm

_alarm:
push	*r15, r14
ld	r14, r15
ld	r0, 4(r14)
sc	$ALARM
ld	r7, r0
pop	r14, *r15
ret
