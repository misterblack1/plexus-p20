#include <sys/plexus.h>
/*
 * _cleanup  
 *
 * does nothing, used if there is no user supplied routine.
 *
 */

.globl	__cleanup
.globl	csav
.globl	cret

__cleanup:
ldk	r0, $LENSAVEREG
call	csav
jp	cret
