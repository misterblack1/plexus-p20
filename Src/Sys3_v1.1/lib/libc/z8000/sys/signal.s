#include <syscall.h>
#define	ASLANG	1
#include <sys/param.h>
#include <errno.h>

/*
 * signal
 */

.globl	_signal
.globl	cerror


_signal:
push	*r15, r14
ld	r14, r15
ld	r6, 4(r14)		/* r6 = signal number */
cp	r6, $NSIG
jr	ult, t1
ld	r0, $EINVAL
jp	cerror			/* error: signal number out of range */

t1:
ld	signal, r6		/* store signal arg */
sla	r6, $1
ld	r5, 6(r14)		/* r5 = new signal handler address */
ld	r7, dvect(r6)		/* r7 = old signal handler address */
ld	dvect(r6), r5		/* save new handler address */
test	r5
jr	z, t2
bit	r5, $0
jr	nz, t2
ld	r5, $sigent		/* r5 = address of signal initiator code */

t2:
ld	action, r5		/* store signal handler address arg */
sc	$INDIR
.word	t9
jp	c, cerror		/* if boo boo then error return */
test	r0
jr	z, t3
bit	r0, $0
jr	nz, t3
ld	r0, r7			/* get actual old handler address from
				   our table since the one in the kernel
				   points to sigent */

t3:
ld	r7, r0
pop	r14, *r15
ret


/* 
 * enter here when a signal occurs. At this point the stack has the following
 * data on it:
 *
 *	tos-2	pc of instruction after one that caused the trap
 *	tos-1	processor flags at time of trap
 *	tos	signal number
 *
 * This code fiddles around with the stack and calls the signal handler. The
 * stack is as follows at the first instruction in the handler:
 *
 *	pc of instruction after the one that caused the trap
 *	processor flags at time of trap
 *	r13 at time of trap
 *	.
 *	.
 *	r0 at time of trap
 *	signal number
 *	pc of next instruction in this code
 *
 * When the handler returns this code restores all the registers from the
 * stack including the flags and returns to the users at the instruction 
 * after the trap. Note that the signal handler can modify any of these
 * registers and they will be set on return.
 */

sigent:
ex	r13, *r15		/* put signal number in r13 and r13 on tos */
sub	r15, $26			/* add room for regs 0 thru 12 */
ldm	*r15, r0, $13		/* stack now contains regs 0 thru 13 */
push	*r15, r13		/* put signal number on tos */
sla	r13, $1
ld	r6, dvect(r13)		/* get signal handler address */
call	*r6			/* call signal handler */
inc	r15, $2			/* del signal number parameter */
ldm	r0, *r15, $14		/* put regs 0 thru 13 back */
add	r15, $28		/* cut stack back */
ex	r0, *r15		/* put old flags word into r0 */
ldctlb	flags, rl0		/* set flags into fcw */
pop	r0, *r15		/* restore proper value for r0 */
ret				/* return to user code */

.data

t9:
sc	$SIGNAL

signal:
.word	0

action:
.word	0

.bss

dvect:
. = . + (NSIG * 2)
