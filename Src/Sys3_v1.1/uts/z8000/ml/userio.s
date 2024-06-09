#include "sys/plexus.h"

/*
 * Global variables which give the segment numbers of the user code and
 * data maps.
 */

.data
.globl	_udseg
_udseg:
.word	UDSEG

.globl	_uiseg
_uiseg:
.word	UISEG
.text


/*
 * The following routines handle access to the current users code
 * and data spaces from system mode. Each of the routines sets up
 * some of the registers then goes to a common set of code that
 * uses segmented mode to actually do the transfer. The register
 * usage for these routines is:
 *
 *	r0	current fcw
 *	r1	callers fcw
 *	r2	pc of error code
 *	r3	data to store used by routines that store into user space
 *	r4	user segment
 *	r5	user offset in segment
 *	r6	pc of proper ld instruction in common code
 *	r7	result (-1 means error)
 */

.globl	_fuibyte
_fuibyte:
ld	r4, _uiseg
ld	r5, 2(r15)
ld	r6, $ldsub
jr	ucomm

.globl	_fubyte
_fubyte:
ld	r4, _udseg
ld	r5, 2(r15)
ld	r6, $ldsub
jr	ucomm

.globl	_fuiword
_fuiword:
ld	r4, _uiseg
ld	r5, 2(r15)
ld	r6, $ldsu
jr	ucomm

.globl	_fuword
_fuword:
ld	r4, _udseg
ld	r5, 2(r15)
ld	r6, $ldsu
jr	ucomm

.globl	_suibyte
_suibyte:
ld	r4, _uiseg
ld	r5, 2(r15)
ld	r3, 4(r15)
ld	r6, $ldusb
jr	ucomm

.globl	_subyte
_subyte:
ld	r4, _udseg
ld	r5, 2(r15)
ld	r3, 4(r15)
ld	r6, $ldusb
jr	ucomm

.globl	_suiword
_suiword:
ld	r4, _uiseg
ld	r5, 2(r15)
ld	r3, 4(r15)
ld	r6, $ldus
jr	ucomm

.globl	_suword
_suword:
ld	r4, _udseg
ld	r5, 2(r15)
ld	r3, 4(r15)
ld	r6, $ldus
jr	ucomm

ucomm:
ldctl	r1, fcw
clr	r7
di	vi		/* vectored interrupts off */
ld	r2, $uerr	/* save error code address */
ld	r0, $(B_SEG + B_SYS + B_NVI)
jp	*r6

ldsub:
ldctl	fcw, r0
ldb	rl7, *r4
nop
ldctl	fcw, r1
ret

ldsu:
ldctl	fcw, r0
ld	r7, *r4
nop
ldctl	fcw, r1
ret

ldusb:
ldctl	fcw, r0
ldb	*r4, rl3
nop
ldctl	fcw, r1
ret

ldus:
ldctl	fcw, r0
ld	*r4, r3
nop
ldctl	fcw, r1
ret

uerr:
ldctl	fcw, r1		/* reset fcw */
ld	r7, $-1		/* indicate that error occured */
ret

