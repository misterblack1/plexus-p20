#include <syscall.h>
#include <sys/plexus.h>
/*
 * exit
 */

.globl	_exit
.globl	__cleanup
.globl	csav

_exit:
ldk	r0, $LENSAVEREG
call	csav
call	__cleanup
ld	r0, 4(r14)
sc	$EXIT
