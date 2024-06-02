#include "sys/plexus.h"
#include "sys/trap.h"

/*
 * definition of the base address of the user block in system data space
 */

.globl	_u
_u = LA_UBLK


/*
 * This routine is the first thing done after an interrupt or trap
 * happens. It saves the values of regs 0 - 7 and 15 as well as
 * pushing an extra zero on the stack which is a place holder for
 * the 'dev' interrupt and trap routine parameter. The stack upon
 * return from this routine is as follows:
 *
 *	pc
 *	pc segment
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
 *	normal mode r15
 *	0 (default 'dev' parameter)
 */

#ifdef OVKRNL
.globl	__ovseg
#endif

.globl	pushregs
pushregs:
	ex	r7, *r15	/* save r7 on stack, set r7 to ret pc */
	sub	r15, $14		/* make room for regs 0 - 6 */
	ldm	*r15, r0, $7	/* save regs 0 - 6 */
	ldctl	r0, nspoff	/* get users r15 */
	push	*r15, r0	/* put on stack */
	push	*r15, $0	/* push on default 'dev' parm */
#ifdef OVKRNL
	ld	r2,__ovseg	/* get current executing segment */
	cp	r2,$SDSEG	/* compare to SDSEG (one changed to on int.) */
	jr	eq,pushskp	/* skip around segment change if the same */
	lda	r3,pushjmp	/* address of local jump in r3 */
	ldctl	r1,fcw		/* save old fcw */
	ld	r0,$(B_SEG+B_SYS+B_NVI)	/* r0 gets new fcw */
	ldctl	fcw,r0		/* go into segmented mode */
	jp	*r2		/* change back to current segment */
pushjmp:
	ldctl	fcw,r1		/* restore fcw to saved value */
pushskp:
#endif
	jp	*r7		/* return to caller */


/*
 * This routine will reset the highest priority in service interrupt
 * on the master 8259 interrupt controller.
 */

.globl	resetmaster
resetmaster:
	ld	r0, $NSEOI
	sout	P_MPIC0, r0
	ret


/*
 * This routine will reset the highest priority in service interrupt
 * on the slave 8259 interrupt controller. The interrupt request on
 * the master for the interrupt is also reset. Notice that normally
 * a check would have to be made to see if any slave interrupts are
 * still outstanding and not reset the master in that case. This isn't
 * done here since the slave is hooked to the lowest priority master
 * interrupt which can be reset every time without causing a problem.
 */

.globl	resetslave
resetslave:
	ld	r0, $NSEOI
	sout	P_SPIC0, r0
	sout	P_MPIC0, r0
	ret


/*
 * c calls to do mset and mres for various things
 */

.globl	_mset
_mset:
	mset
	ret

.globl	_mres
_mres:
	mres
	ret



/*
 *  extended instruction trap
 */

/* externals */
.globl	_trap
.globl	_sysdebug
#ifdef HWFP
.globl	fpinst
#else
.globl	_softfp
#endif

.globl	t_ei
t_ei:
	call	pushregs
	bit	22(r15), $S_SYS
	jr	z, t_ei1
	ld	*r15, $UNIMPL		/* call system debugger */
	call	_sysdebug
	jp	intret

t_ei1:
	cpb	20(r15), $0x8e
#ifdef HWFP
	jp	eq, fpinst
#else
	jp	eq, _softfp		/* if(float point inst) goto _softfp */
#endif
	ld	*r15, $UNIMPL
	ei	vi
	call	_trap
	jp	intret


/*
 *  privileged instruction trap
 */

.globl	t_pi
t_pi:
	call	pushregs
	ld	*r15, $PRIVFLT
	ei	vi
	call	_trap
	jp	intret


/*
 *  system call trap
 */

.globl	t_sc
t_sc:
	call	pushregs
	ld	*r15, $SYSCALL
	ei	vi
	call	_trap
	jp	intret


/*
 *  segmentation trap
 */

.globl	t_seg
t_seg:
	call	pushregs
	sin	r0, P_MRE0		/* get error status */
	bit	r0, $S_PFLT
	jr	z, dotrap		/* if(! protection error) goto dotrap */
	bit	22(r15), $S_SEG
	jr	z, dotrap		/* if(!seg trap in seg mode) dotrap */
	ld	r0, 8(r15)		/* get error code pc */
	ld	26(r15), r0		/* set into pc for resumption */
	sout	P_MRINT, r0		/* reset mem request error interrupt */
	jp	intret
	
dotrap:
	ld	*r15, $SEGFLT
	call	_trap
	jp	intret


/*
 *  non maskable interrupt
 */

.globl	t_nmi
t_nmi:
	call	pushregs
	ld	*r15, $NMINT
	ei	vi
	call	_trap
	jp	intret


/*
 *  non vectored interrupt
 */

.globl	t_nvi
t_nvi:
	call	pushregs
	ld	*r15, $NVINT
	call	_sysdebug	
	jp	intret


/*
 *  clock interrupt
 */

/* externals */
.globl	_clock

.globl	i_clk
i_clk:
	call	pushregs
	sout	P_CLKINT, r0		/* reset clk int request */
	call	resetmaster
	call	_clock
	jp	intret
	
/*
 * local tty interrupt
 */

/* externals */
.globl	_usint

.globl	i_lcltty
i_lcltty:
	call	pushregs
	call	_usint
	call	resetmaster
	jp	intret


/*
 *  bus timeout interrupt
 */

.globl	i_busto
i_busto:
	call	pushregs
	ld	*r15, $BUSTO
	call	_trap
	sout	P_MBTOINT, r0
	call	resetmaster
	jp	intret


/*
 *  single step interrupt
 */

.globl	i_sstep
i_sstep:
	call	pushregs
	ld	*r15, $BPTFLT
	sin	r0, P_MPIC1
	set	r0, $S_PICSS
	sout	P_MPIC1, r0		/* disable single step interrupt */
	call	resetmaster
	ei	vi
	call	_trap
	jp	intret


/*
 *  memory request error interrupt
 */

.globl	i_mrerr
i_mrerr:
	call	pushregs
	ld	*r15,$MRERR 
	call	_trap
	sout	P_MRINT, r0
	call	resetmaster
	jp	intret
	

/*
 *  unexpected interrupt from vector not used by master or slave 8259
 */

.globl	i_bad
i_bad:
	call	pushregs
	ld	*r15, $ILLINT
	call	_trap
	jp	intret


/*
 *  unexpected interrupt from master 8259 vector not in use
 */

.globl	i_mbad
i_mbad:
	call	pushregs
	ld	*r15, $ILLINT
	call	resetmaster
	call	_trap
	jp	intret


/*
 *  unexpected interrupt from slave 8259 vector not in use
 */

.globl	i_sbad
i_sbad:
	call	pushregs
	ld	*r15, $ILLINT
	call	resetslave
	call	_trap
	jp	intret


/*
 *  simulate a system debug interrupt
 */

.globl	_debug
_debug:
#ifdef OVKRNL
	push	*r15, __ovseg	/* put what looks like an int mark */
#else
	push	*r15, $SDSEG
#endif
	push	*r15, r7	/* on the stack.                   */
	ldctl	r7, fcw
	ex	r7, *r15
	push	*r15, $0
	call	pushregs	/* now treat as if it were a legal int */
	ld	*r15, $DBG
	call	_sysdebug
	jp	intret
	
	
/*
 * This routine causes a switch back to the code that was running at the
 * time of the last interrupt or trap. The state of the stack is the same
 * as at the call to `_trap'. Note that old r8 - r14 must be set 
 * to the proper values at this point since this code does not restore 
 * them.
 */

/* externals */
.globl	_runrun
.globl	_trap

.globl	intret
intret:
	bit	22(r15), $S_SYS
	jr	nz, rtn3	/* if (old code was in system mode) goto rtn3 */
	testb	_runrun
	jr	z, rtn2		/* if (no reason to resched) goto rtn2 */
	ld	*r15, $RESCHED	/* trap parm */
	ei	vi
	call	_trap		/* call trap for resched */
	
rtn2:
	ld	r0, 2(r15)	/* r0 = users r15 */
	ldctl	nspoff, r0	/* set users r15 */
	di	vi		/* disable vectored interrupts */
	bit	22(r15), $S_SINGL
	jr	z, rtn3		/* if (old code not being single stepped) rtn3*/
	res	22(r15), $S_SINGL /* clear fake single step bit in old fcw */
	sout	P_SSINT, r0	/* clear any previous single step int request */
	sin	r0, P_MPIC1
	res	r0, $S_PICSS
	sout	P_MPIC1, r0	/* allow single step interrupts */
	
rtn3:
/*
#ifdef	OVKRNL
	ld 	r0, r15
	add	r15,$24
	ld	r1,*r15
	cp	r1,$0x1000
	jp	z,ok1
	cp	r1,$0x0200
	jp	z,ok1
	cp	r1,$0x0400
	jp	z,ok1
	ld	r15,r0
	call	_debug
ok1:
	ld	r15, r0
#endif
*/
	ld	r0, $(B_SEG | B_SYS | B_NVI)
	ldctl	fcw, r0		/* go into segmented mode */
	inc	r15, $4		/* pop off dev and r15 values */
	ldm	r0, *r15, $8	/* restore old r0 - r7 */
	add	r15, $16	/* pop off r0 - r7 */
	iret			/* return to old code */
	
