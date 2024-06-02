/*
 * Low level part of the sioc software.
 */

#include "/p3/usr/include/icp/sioc.h"	/* icp specific */
#include "/p3/usr/include/icp/sioccsv.h"	/* icp specific */

/* externals */
.globl	_main
.globl	psa

.globl	start
start:
di	vi		/* disable vectored interrupts */
mset			/* release multibus lock */

ld	r0, $CMDINIT	/* initialize cmd latch */
out	PCMD, r0		/* initialize hardware latch */

ld	r0, $RFRESH
ldctl	refresh, r0		/* start refresh for ram */

lda	r0, psa
ldctl	psap, r0		/* set up psap */

ld	r1, $RAM		/* clear all of ram */
/* temporarily we will not clear RAM because that is where we are testing
 * this program.
 */
jr	noclear

clrloop:
clr	*r1
inc	r1, $2
cp	r1, $MBUS
jr	ult, clrloop

noclear:
ld	r15, $MBUS		/* set stack top */
clr	r14			/* initial frame pointer */

call	_main			/* go to user code */

halt				/* really should not return here */


/*
 * This routine is the first thing done after an interrupt or trap
 * happens. It saves the values of regs 0 - 7. The stack upon
 * return from this routine is as follows:
 *
 *	pc
 *	fcw
 *	interrupt or trap id
 *	r7
 *	r6
 *	r5
 *	r4
 *	r3
 *	r2
 *	r1
 *	r0
 */

.globl	pushregs
pushregs:
clrb	2(r15)			/* clear msb of interrupt id */
ex	r7, *r15		/* save r7 on stack, set r7 to return pc */
sub	r15, $14		/* make room for regs 0 - 6 */
ldm	*r15, r0, $7		/* save regs 0 - 6 */
jp	*r7			/* return to caller */


/*
 * This code causes a return to the code which was last interrupted. The
 * stack is assumed to be just as it was at the end of the call to pushregs.
 * Note that regs 8 - 14 must already be set to the proper values for the
 * interrupt return.
 */

.globl	intret
intret:
ldm	r0, *r15, $8		/* restore old r0 - r7 */
add	r15, $16		/* set top of stack for iret */
iret				/* return to interrupted code */


/*
 * The following routines set the processor interrupt priority. Currently
 * there are only two levels, on and off.
 */

.globl	_spl0
_spl0:
ldctl	r7, fcw		/* save old fcw for return to caller */
ei	vi		/* interrupts on */
ret

.globl	_spl7
_spl7:

.globl	_spl6
_spl6:

.globl	_spl5
_spl5:
ldctl	r7, fcw		/* save old fcw for return to caller */
di	vi		/* interrupts off */
ret

.globl	_splx
_splx:
ld	r7, 2(r15)
ldctl	fcw, r7
ret


/*
 * The following code is where the system sits when there
 * is nothing to do.
 */

.globl	_idle
_idle:
ldctl	r7, fcw		/* save current fcw */
ei	vi		/* vectored interrupts on */
halt			/* wait for interrupt */

waitloc:
nop			/* place where pc sits while in halt state */
nop
ldctl	fcw, r7		/* restore callers fcw */
ret


/*
 * This code handles C call and return. No stack overflow check is
 * made since there should be no stack overflows in system mode.
 */


.globl csv
csv:
ld	r1, *r15	/* r1 = return address */
ld	*r15, r14	/* save old frame ptr */
ld	r14, r15	/* set up new frame ptr */
sub	r15, r0		/* make room for locals and registers */
ldm	-LENSAVEREG(r14), FIRSTSAVEREG, $NUMSAVEREG	/* save registers */
call	*r1		/* return and add one word to the stack */

.globl cret
cret:
ldm	FIRSTSAVEREG, -LENSAVEREG(r14), $NUMSAVEREG	/* restore registers */
ld	r15, r14	/* move stack top to previous frame */
pop	r14, *r15	/* restore old previous frame ptr */
ret			/* return thru the address on the stack which
			   was placed there during the original 
			   call. */

.globl csv0
.globl cret0

csv0:
ld	r1, *r15		/* r1 = return address */
ld	*r15, r14		/* save old frame ptr */
ld	r14, r15		/* set up new frame ptr */
sub	r15, r0			/* allocate storage for frame locals */
call	*r1			/* return and add 1 to the stack */


cret0:
ld	r15, r14		/* move stack top to previous frame */
pop	r14, *r15		/* restore old previous frame ptr */
ret				/* return thru the address on the stack which
				   was placed there during the original function 
				   call. */


.globl csv1
.globl cret1

csv1:
ld	r1, *r15		/* r1 = return address */
ld	*r15, r14		/* save old frame ptr */
ld	r14, r15		/* set up new frame ptr */
sub	r15, r0			/* allocate storage for frame locals */
ldm	-2(r14), r13, $1	/* save registers */
call	*r1			/* return and add 1 to the stack */

cret1:
ldm	r13, -2(r14), $1	/* restore registers */
ld	r15, r14		/* move stack top to previous frame */
pop	r14, *r15		/* restore old previous frame ptr */
ret				/* return thru the address on the stack which
				   was placed there during the original function 
				   call. */


.globl csv2
.globl cret2

csv2:
ld	r1, *r15		/* r1 = return address */
ld	*r15, r14		/* save old frame ptr */
ld	r14, r15		/* set up new frame ptr */
sub	r15, r0			/* allocate storage for frame locals */
ldm	-4(r14), r12, $2	/* save registers */
call	*r1			/* return and add 1 to the stack */

cret2:
ldm	r12, -4(r14), $2	/* restore registers */
ld	r15, r14		/* move stack top to previous frame */
pop	r14, *r15		/* restore old previous frame ptr */
ret				/* return thru the address on the stack which
				   was placed there during the original function 
				   call. */


.globl csv3
.globl cret3

csv3:
ld	r1, *r15		/* r1 = return address */
ld	*r15, r14		/* save old frame ptr */
ld	r14, r15		/* set up new frame ptr */
sub	r15, r0			/* allocate storage for frame locals */
ldm	-6(r14), r11, $3	/* save registers */
call	*r1			/* return and add 1 to the stack */

cret3:
ldm	r11, -6(r14), $3	/* restore registers */
ld	r15, r14		/* move stack top to previous frame */
pop	r14, *r15		/* restore old previous frame ptr */
ret				/* return thru the address on the stack which
				   was placed there during the original function 
				   call. */


.globl csv4
.globl cret4

csv4:
ld	r1, *r15		/* r1 = return address */
ld	*r15, r14		/* save old frame ptr */
ld	r14, r15		/* set up new frame ptr */
sub	r15, r0			/* allocate storage for frame locals */
ldm	-8(r14), r10, $4	/* save registers */
call	*r1			/* return and add 1 to the stack */

cret4:
ldm	r10, -8(r14), $4	/* restore registers */
ld	r15, r14		/* move stack top to previous frame */
pop	r14, *r15		/* restore old previous frame ptr */
ret				/* return thru the address on the stack which
				   was placed there during the original function 
				   call. */


.globl csv5
.globl cret5

csv5:
ld	r1, *r15		/* r1 = return address */
ld	*r15, r14		/* save old frame ptr */
ld	r14, r15		/* set up new frame ptr */
sub	r15, r0			/* allocate storage for frame locals */
ldm	-10(r14), r9, $5	/* save registers */
call	*r1			/* return and add 1 to the stack */

cret5:
ldm	r9, -10(r14), $5	/* restore registers */
ld	r15, r14		/* move stack top to previous frame */
pop	r14, *r15		/* restore old previous frame ptr */
ret				/* return thru the address on the stack which
				   was placed there during the original function 
				   call. */

/*
 * This code is used by cc compilers to implement switch statements.
 * At the entrance to either switch1 or switch2 r7 contains the switch
 * index value and r6 contains the address, in the data area, of the
 * control block for the switch statement. The switch control block is
 * as follows:
 *
 * switch1:
 *
 *	0)	lowest case label value (clow)
 *	1)	highest case label value (chigh)
 *	2)	code address of default case.
 *	3)	(chigh - clow + 1) code addresses for the
 *		cases from clow to chigh in ascending order by
 *		case value.
 *
 * switch2:
 *
 *	0)	number of cases + 1 (n + 1)
 *	1)	n case values.
 *	n+1)	n case code addresses.
 *	2*n+1)	default case code address.
 */

.globl	switch1
switch1:
ld	r5, 4(r6)		/* r5 = default code address */
cp	r7, 2(r6)
jp	gt, *r5			/* out of case range on high end */
sub	r7, *r6
jp	lt, *r5			/* out of case range on low end */
add	r6, r7
add	r6, r7			/* r6 = index into case code addresses */
ld	r5, 6(r6)		/* r5 = case code address */
jp	*r5			/* jump to case */

.globl	switch2
switch2:
ld	r5, *r6			/* r5 = number of cases + 1 */
ld	r4, r5			/* r4 = number of cases + 1 */
inc	r6, $2			/* r6 = ptr to case value list */
cpir	r7, *r6, r5, eq		/* look for correct case value */
add	r6, r4
add	r6, r4			/* r6 = address of correct code address */
ld	r5, -4(r6)		/* r5 = correct case code address */
jp	*r5			/* jump to case */


/*
 * out_local(port, value)
 * in_local(port)
 *
 * These routines do word i/o instructions to the normal i/o space. 
 */

.globl	_out_local
_out_local:
ld	r1, 2(r15)	/* r1 = port address */
ld	r2, 4(r15)	/* r2 = value */
out	*r1, r2
ret

.globl	_in_local
_in_local:
ld	r1, 2(r15)	/* r1 = port address */
in	r7, *r1
ret


/*
 * reset the interrupt chain
 */

.globl	_putreti
_putreti:
ldctl	r0, fcw		/* save callers fcw */
di	vi		/* disable interrupts */
ld	r1, $PRETI	/* r1 = port address of reti port */
ld	r2, $reti	/* r2 = address of data to output to port */
ld	r3, $2
otir	*r1, *r2, r3
ldctl	fcw, r0		/* put back users fcw */
ret

.data
reti:
.word	0xed
.word	0x4d
.text


/*
 * multi micro set.
 */

.globl	_mset
_mset:
mset
ret


/*
 * multi micro reset.
 */

.globl	_mres
_mres:
mres
ret


/*
 * jmp(adx) jumps to the address 'adx' in system mode with vectored ints off
 */

.globl	_jmp
_jmp:
inc	r15, $2			/* delete return address */
push	*r15, $(SYS | NVIE)	/* push on fcw */
push	*r15, $0		/* push on fake int id */
iret				/* do return from int to get to new pc */
