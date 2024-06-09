#include "sys/plexus.h"

/*
 * out_multibus(port, value)
 * in_multibus(port)
 * out_local(port, value)
 * in_local(port)
 *
 * These four routines do word i/o instructions to the normal and special
 * i/o spaces. Special i/o space is used for local i/o and normal i/o
 * space for multibus i/o
 */

.globl	_out_multibus
_out_multibus:
ld	r1, 2(r15)	/* r1 = port address */
ld	r2, 4(r15)	/* r2 = value */
out	*r1, r2
ret

.globl	_in_multibus
_in_multibus:
ld	r1, 2(r15)	/* r1 = port address */
in	r7, *r1
ret

.globl	_out_local
_out_local:
ld	r1, 2(r15)	/* r1 = port address */
lda	r2, 4(r15)	/* r2 = address of value */
souti	*r1, *r2, r3	/* souti used since sout can't use indirect adx */
ret

.globl	_in_local
_in_local:
ld	r1, 2(r15)	/* r1 = port address */
push	*r15, $0	/* save room for temp on stack */
ld	r2, r15		/* get address of temp into r2 */
sini	*r2, *r1, r3	/* sini used since sin can't use indirect adx */
pop	r7, *r15	/* put result in r7 and pop temp off stack */
ret



/*
 * addupc(pcval, infoadx, incr)
 *
 * `infoadx' is the address of a buffer which contains the parameters
 * to the processes last call to profil(2). This buffer is as follows
 *
 *	address of histogram buffer
 *	histogram buffer length
 *	pc offset that first histogram cell represents
 *	scale factor
 *
 * The `pc' input is the value of the pc counter to be added to the 
 * histogram. The `incr' value is what is actually added to the
 * histogram cell representing the `pc' parameter.
 */

.globl	_addupc
_addupc:
	ldctl	r1, fcw
	ld	r7, 6(r15)	/* r7 = increment value */
	ld	r6, 4(r15)	/* r6 = address of profile info buffer */
	ld	r0, 2(r15)
	sub	r0, 4(r6)
	srl	r0, $1		/* r0 = word index into histogram buffer */
	ld	r5, 6(r6)	/* r5 = scale factor */
	srl	r5, $1		/* convert to word */
	mult	rr4, r0		/* do scaling */
	srll	rr4, $14	/* put back to byte mode (not word) */
	inc	r5, $1
	res	r5, $0		/* round up to even byte address */
	cp	r5, 2(r6)
	ret	uge		/* if (outside histogram buffer) return */
	add	r5, *r6		/* r5 = offset of hist cell in user space */
	ld	r4, _udseg	/* r4 = segment address of user data space */
	ld	r2, $upcerr	/* save error code location */
	ld	r0, $(B_SEG | B_SYS | B_NVI)
	ldctl	fcw, r0		/* go into segmented mode */
	add	r7, *r4		/* add histogram cell value to increment */
	ld	*r4, r7		/* store new histogram cell value */
	ldctl	fcw, r1		/* reset fcw to callers value */
	ret
	
upcerr:
	ldctl	fcw, r1
	clr	6(r6)		/* scale = 0 (causes profiling to be turned off) */
	ret

/********************* t_prof *********************
 *   system profiling routine
 *   called by nvi
 *   if in system mode when nvi, builds histogram in buffer of pc
 *   if in normal mode, bumps the counter, profbuf.
 */
/* externals */
.globl _profbuf

.globl t_prof
	t_prof:
	dec	r15, $N_PROFREGSAVED*2	/* save N_PROFREGSAVED registers */
	ldm	*r15, r3, $N_PROFREGSAVED /* save regs starting with r3 */
	bit     (N_PROFREGSAVED*2+2)(r15),$S_SYS /* was system in sys mode ? */
	jr	nz,prof2	/* jump if system mode */
	inc	_profbuf	/* normal mode: bump k_norm */
	jr	nz,prof1	/* jp if no overflow to high word of k_norm */
	inc	_profbuf+2	/* bump high word of k_norm */
prof1:
	ld	r3,$(B_SYS | B_SEG)	/* iret must be done in seg mode */
	ldctl	fcw,r3
	ldm	r3,*r15,$N_PROFREGSAVED
	inc	r15,$N_PROFREGSAVED*2	/* pop off saved registers */
	iret

prof2:		/* sys mode: bump sys counter for either int on or */
		/* off.  bump appropriate bucket in pc histogram buf */
	bit	(N_PROFREGSAVED*2+2)(r15),$S_VI	/* were vi on? */
	jr	nz,profvion	/* jump if vi on */
	inc	_profbuf+4	/* system mode: bump k_sys */
	jr	nz,prof3	/* jp if no overflow to high word of k_sys */
	inc	_profbuf+6	/* bump high word of k_sys */
	jr	prof3
profvion:
	inc	_profbuf+8	/* system mode: bump k_sysvion */
	jr	nz,prof3	/* jp if no overflow to high word of k_sysvion */
	inc	_profbuf+10	/* bump high word of k_sysvion */
prof3:
	ld	r4,$PROFSEG	/* r4,r5<=adx in histogram buf for current pc */
	ld	r5,(N_PROFREGSAVED*2+6)(r15) /* r5 <= old pc offset */
	profshift:		/* next instruction has scale factor that determines */
				/* buffer size					*/
	srl	r5,$PROFSF	/* shift pc offset right by scaling factor */
	and	r5,$0xfffc	/* convert pc offset to a double word adx */
	ld	r3,$(B_SYS|B_SEG)
	ldctl	fcw,r3		/* Do inc and iret in segemented mode */
	inc	*r4
	jr	nz,prof4	/* jump if no overflow into upper half of counter*/
	inc	r5,$2		
	inc	*r4		/* bump upper half of double word counter */
prof4:
	ldm	r3,*r15,$N_PROFREGSAVED
	inc	r15,$N_PROFREGSAVED*2	/* pop off saved registers */
	iret

/*
 * returns to the prom without doing a reset
 */

.globl	_jptoprom
_jptoprom:
	push	*r15, $0	/* set up  pc offset */
	push	*r15, $0	/* set up  pc seg */
	push	*r15, $(B_SYS | B_NVI)	/* set up  fcw */
	push	*r15, $0	/* set identifier just for iret */
	ld	r0, $(B_SEG | B_SYS | B_NVI)
	ldctl	fcw, r0
	iret
	

/*
 * The following routines set the processor interrupt priority. Currently
 * there are only two levels, on and off.
 */

.globl	_spl0
_spl0:
ldctl	r7, fcw		/* save old fcw for return to caller */
ei	vi		/* vectored interrupts on */
ret

.globl	_spl1
_spl1:

.globl	_spl4
_spl4:

.globl	_spl5
_spl5:

.globl	_spl6
_spl6:

.globl	_spl7
_spl7:
ldctl	r7, fcw		/* save old fcw for return to caller */
di	vi		/* vectored interrupts off */
ret

.globl	_splx
_splx:
ld	r7, 2(r15)
ldctl	fcw, r7
ret


/*
 * The following code is where the operating system sits when there
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

.globl	_waitloc
.data
_waitloc:
.word	waitloc
.text


