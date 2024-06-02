#define	ASLANG 1
#include <sys/param.h>
#include <sys/plexus.h>
#include <syscall.h>

/*
 * This code handles C call and return. The difference from the
 * PDP 11 version is that extra code is executed to check if
 * a new stack page must be allocated.
 */

.globl csav
.globl csv
.globl __endstk

csav:
csv:
ld	r1, *r15		/* r1 = return address */
ld	*r15, r14		/* save old frame ptr */
ld	r14, r15		/* set up new frame ptr */
sub	r15, r0			/* allocate storage for frame locals */
cp	r15, __endstk
jr	uge, save		/* if not into safety zone then goto save */
ld	r0, r15
sub	r0, $SAFETYZONELEN
and	r0, $B_PAGENUM
add	r0, $SAFETYZONELEN	/* r0 = new safety zone ptr */
ld	__endstk, r0		/* save for later */
sc	$UGROW			/* grow stack */
jr	nc, save		/* if stack could be expanded goto save */
sc	$GETPID			/* get pid of this process */
sc	$KILL			/* send this process a segment violation trap */
.word	SIGSEGV

save:
ldm	-LENSAVEREG(r14), FIRSTSAVEREG, $NUMSAVEREG	/* save registers */
call	*r1			/* return and add one word to the stack */

.globl csav0
.globl csv0

csav0:
csv0:
ld	r1, *r15		/* r1 = return address */
ld	*r15, r14		/* save old frame ptr */
ld	r14, r15		/* set up new frame ptr */
sub	r15, r0			/* allocate storage for frame locals */
cp	r15, __endstk
jr	uge, save0		/* if not into safety zone then goto save */
ld	r0, r15
sub	r0, $SAFETYZONELEN
and	r0, $B_PAGENUM
add	r0, $SAFETYZONELEN	/* r0 = new safety zone ptr */
ld	__endstk, r0		/* save for later */
sc	$UGROW			/* grow stack */
jr	nc, save0		/* if stack could be expanded goto save */
sc	$GETPID			/* get pid of this process */
sc	$KILL			/* send this process a segment violation trap */
.word	SIGSEGV

save0:
call	*r1			/* return and add one word to the stack */


.globl csav1
.globl csv1

csav1:
csv1:
ld	r1, *r15		/* r1 = return address */
ld	*r15, r14		/* save old frame ptr */
ld	r14, r15		/* set up new frame ptr */
sub	r15, r0			/* allocate storage for frame locals */
cp	r15, __endstk
jr	uge, save1		/* if not into safety zone then goto save */
ld	r0, r15
sub	r0, $SAFETYZONELEN
and	r0, $B_PAGENUM
add	r0, $SAFETYZONELEN	/* r0 = new safety zone ptr */
ld	__endstk, r0		/* save for later */
sc	$UGROW			/* grow stack */
jr	nc, save1		/* if stack could be expanded goto save */
sc	$GETPID			/* get pid of this process */
sc	$KILL			/* send this process a segment violation trap */
.word	SIGSEGV

save1:
ldm	-2(r14), r13, $1	/* save registers */
call	*r1			/* return and add one word to the stack */


.globl csav2
.globl csv2

csav2:
csv2:
ld	r1, *r15		/* r1 = return address */
ld	*r15, r14		/* save old frame ptr */
ld	r14, r15		/* set up new frame ptr */
sub	r15, r0			/* allocate storage for frame locals */
cp	r15, __endstk
jr	uge, save2		/* if not into safety zone then goto save */
ld	r0, r15
sub	r0, $SAFETYZONELEN
and	r0, $B_PAGENUM
add	r0, $SAFETYZONELEN	/* r0 = new safety zone ptr */
ld	__endstk, r0		/* save for later */
sc	$UGROW			/* grow stack */
jr	nc, save2		/* if stack could be expanded goto save */
sc	$GETPID			/* get pid of this process */
sc	$KILL			/* send this process a segment violation trap */
.word	SIGSEGV

save2:
ldm	-4(r14), r12, $2	/* save registers */
call	*r1			/* return and add one word to the stack */


.globl csav3
.globl csv3

csav3:
csv3:
ld	r1, *r15		/* r1 = return address */
ld	*r15, r14		/* save old frame ptr */
ld	r14, r15		/* set up new frame ptr */
sub	r15, r0			/* allocate storage for frame locals */
cp	r15, __endstk
jr	uge, save3		/* if not into safety zone then goto save */
ld	r0, r15
sub	r0, $SAFETYZONELEN
and	r0, $B_PAGENUM
add	r0, $SAFETYZONELEN	/* r0 = new safety zone ptr */
ld	__endstk, r0		/* save for later */
sc	$UGROW			/* grow stack */
jr	nc, save3		/* if stack could be expanded goto save */
sc	$GETPID			/* get pid of this process */
sc	$KILL			/* send this process a segment violation trap */
.word	SIGSEGV

save3:
ldm	-6(r14), r11, $3	/* save registers */
call	*r1			/* return and add one word to the stack */


.globl csav4
.globl csv4

csav4:
csv4:
ld	r1, *r15		/* r1 = return address */
ld	*r15, r14		/* save old frame ptr */
ld	r14, r15		/* set up new frame ptr */
sub	r15, r0			/* allocate storage for frame locals */
cp	r15, __endstk
jr	uge, save4		/* if not into safety zone then goto save */
ld	r0, r15
sub	r0, $SAFETYZONELEN
and	r0, $B_PAGENUM
add	r0, $SAFETYZONELEN	/* r0 = new safety zone ptr */
ld	__endstk, r0		/* save for later */
sc	$UGROW			/* grow stack */
jr	nc, save4		/* if stack could be expanded goto save */
sc	$GETPID			/* get pid of this process */
sc	$KILL			/* send this process a segment violation trap */
.word	SIGSEGV

save4:
ldm	-8(r14), r10, $4	/* save registers */
call	*r1			/* return and add one word to the stack */


.globl csav5
.globl csv5

csav5:
csv5:
ld	r1, *r15		/* r1 = return address */
ld	*r15, r14		/* save old frame ptr */
ld	r14, r15		/* set up new frame ptr */
sub	r15, r0			/* allocate storage for frame locals */
cp	r15, __endstk
jr	uge, save5		/* if not into safety zone then goto save */
ld	r0, r15
sub	r0, $SAFETYZONELEN
and	r0, $B_PAGENUM
add	r0, $SAFETYZONELEN	/* r0 = new safety zone ptr */
ld	__endstk, r0		/* save for later */
sc	$UGROW			/* grow stack */
jr	nc, save5		/* if stack could be expanded goto save */
sc	$GETPID			/* get pid of this process */
sc	$KILL			/* send this process a segment violation trap */
.word	SIGSEGV

save5:
ldm	-10(r14), r9, $5	/* save registers */
call	*r1			/* return and add one word to the stack */
