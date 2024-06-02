/*
 * Low level part of the sioc software.
 */

#include "icp/sioc.h"		/* icp specific */
#include "icp/sioccsv.h"	/* icp specific */
#ifdef	MDP
#define	B_VI	12
#define B_NVI	11
mvion:
.word	0x1
mnvion:
.word	0x1
#endif

/* externals */
.globl	_main
.globl	psa
.globl	_cmdsav
/*.globl	_edata*/

.globl	start
start:
di	vi		/* disable all interrupts */
#ifdef	MDP
clr	mvion
#endif

ld	r0, $RFRESH
ldctl	refresh, r0		/* start refresh for ram */

lda	r0, psa
ldctl	psap, r0		/* set up psap */

ld	r1, $_edata		/* clear bss and stack areas to zero */

clrloop:
clr	*r1
inc	r1, $2
cp	r1, $MBUS
jr	ult, clrloop

ld	_cmdsav, $CMDINIT	/* initialize command latch save word */
ld	r0, _cmdsav		/* initialize command latch */
out	PCMD, r0

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

#ifdef	MDP
inint:
.word	0x0
#endif
.globl	pushregs
pushregs:
#ifdef	MDP
ld	inint,$1
#endif
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

intcnt:
	.word	0x0
intcntl:
	.word	0x0
.globl	intret
intret:
ldm	r0, *r15, $8		/* restore old r0 - r7 */
add	r15, $16		/* set top of stack for iret */
inc	intcntl,$1
jr	nz,nobump
inc	intcnt,$1
nobump:
#ifdef	MDP
clr	inint
#endif
iret				/* return to interrupted code */


/*
 * The following routines set the processor interrupt priority. Currently
 * there are only two levels, on and off.
 */

.globl	_spl0
_spl0:
ldctl	r7, fcw		/* save old fcw for return to caller */
#ifdef	MDP
ld	mvion,$1
ld	mnvion,$1
#endif
ei	vi		/* interrupts on */
ei	nvi
ret

.globl	_spl7
_spl7:

.globl	_spl6
_spl6:

.globl	_spl5
_spl5:
ldctl	r7, fcw		/* save old fcw for return to caller */
di	vi		/* interrupts off */
di	nvi		/* no-vectored off too */
#ifdef	MDP
clr	mvion
clr	mnvion
#endif
ret

.globl	_spl4
_spl4:
ldctl	r7, fcw		/* save old fcw for return to caller */
di	vi		/* interrupts off */
#ifdef	MDP
clr	mvion
#endif
ret

.globl	_splx
_splx:
ld	r7, 2(r15)
#ifdef	MDP
clr	mvion
bit	r7,$B_VI
jr	z,notvi
ld	mvion,$1
notvi:
clr	mnvion
bit	r7,$B_NVI
jr	z,notnvi
ld	mnvion,$1
notnvi:
#endif
ldctl	fcw, r7
ret


/*
 * The following code is where the system sits when there
 * is nothing to do.
 */

.globl	_idle
_idle:
ldctl	r7, fcw		/* save current fcw */
#ifdef	MDP
ld	mvion,$1
ld	mnvion,$1
#endif
ei	vi		/* vectored interrupts on */
ei	nvi
halt			/* wait for interrupt */

waitloc:
nop			/* place where pc sits while in halt state */
nop
#ifdef	MDP
bit	r7,$B_VI
jr	nz,isvi
clr	mvion
isvi:
bit	r7,$B_NVI
jr	nz,isnvi
clr	mnvion
isnvi:
#endif
ldctl	fcw, r7		/* restore callers fcw */
ret


/*
 * save(buf)	
 * 
 * saves the information needed to restart the current 
 * process in `buf'.
 *
 * resume(buf)
 *
 * Restarts a process using the information in `buf'.
 *
 * The return from the call to save is 0. When a resume is done
 * with the information from the save it will appear to the resumed
 * process that save returned a 1.
 */

.globl	_save
_save:
pop	r7, *r15	/* save callers next code address */
ld	r1, *r15	/* r1 = address of save area */
ldm	*r1, r7, $9	/* store next pc and r8 - r15 */
ld	r6, r7
clr	r7		/* return 0 for normal return */
jp	*r6		/* return to caller */

.globl	_resume
_resume:
ld	r1, 2(r15)	/* r1 = address of restart info */
ldm	r7, *r1, $9	/* restore registers */
ld	r6, r7
ld	r7, $1		/* return 1 for resumption */
jp	*r6		/* return to caller */


/*
 * This code handles C call and return. No stack overflow check is
 * made since there should be no stack overflows in system mode.
 */
#ifdef	VPMSYS
savlst:
.word	0x0
.globl	_curproc
.globl  _runflag
#endif

.globl csv
csv:
ld	r1, *r15	/* r1 = return address */
ld	*r15, r14	/* save old frame ptr */
ld	r14, r15	/* set up new frame ptr */
sub	r15, r0		/* make room for locals and registers */
ldm	-LENSAVEREG(r14), FIRSTSAVEREG, $NUMSAVEREG	/* save registers */
#ifdef	VPMSYS
csvex:
test	_runflag
jr	z,nogo
ld	r7,_curproc
add	r7,$0x210
sub	r7,r15
ld	r6,savlst
sub	r7,r6
jr	mi,nogo
ld	savlst,r7
nogo:
#endif
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
#ifdef	VPMSYS
jr	csvex
#else
call	*r1
#endif


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
#ifdef	VPMSYS
jr	csvex
#else
call	*r1
#endif

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
#ifdef	VPMSYS
jr	csvex
#else
call	*r1
#endif

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
#ifdef	VPMSYS
jr	csvex
#else
call	*r1
#endif

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
#ifdef	VPMSYS
jr	csvex
#else
call	*r1
#endif

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
#ifdef	VPMSYS
jr	csvex
#else
call	*r1
#endif

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
 *  Find specified character in string of specified length.


.globl 	_fndchar
_fndchar:
ldb	rl0, 3(r15)
ld	r1, 4(r15)
ld	r3, 6(r15)
cpirb	rl0, *r1, r3, eq
jr	z,found
clr	r7
ret
found:
ld	r7, r1
ret
*/


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

#ifndef	VPMSYS
prcl:
	.word	0x0
#endif
prcount:
	.word	0x0
.globl	_putreti
_putreti:
ldctl	r0, fcw		/* save callers fcw */
di	vi		/* disable interrupts */
#ifdef	MDP
clr	mvion
#endif
ld	r1, $PRETI	/* r1 = port address of reti port */
ld	r2, $reti	/* r2 = address of data to output to port */
ld	r3, $2
otir	*r1, *r2, r3
#ifdef	MDP
bit	r0,$B_VI
jr	z,prsvi
ld	mvion,$1
prsvi:
#endif
ldctl	fcw, r0		/* put back users fcw */
inc	prcount,$1
#ifndef	VPMSYS
jr	nz,prcno
inc	prcl,$1
prcno:
#endif
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

#ifdef DEBUG
#define T_DBG	12
.globl	_debug
_debug:
push	*r15, r7
ldctl	r7, fcw
di	vi
ex	r7, *r15
push	*r15,$0
call	pushregs
push	*r15,$T_DBG
call	_sysdebug
inc	r15,$2
jp	intret
#endif


