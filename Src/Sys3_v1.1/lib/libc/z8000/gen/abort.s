#include <sys/plexus.h>
/*
 * abort
 */

.globl	_abort
.globl	csav
.globl	cret

_abort:
ldk	r0, $LENSAVEREG
call	csav
in	r7, *r1		/* this should cause a trap */
clr	r7
jp	cret
