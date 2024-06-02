mfpi	= 6500^tst
mtpi	= 6600^tst
rtt	= 6
PMSV	= 10340
PS	= 177776
SISA0	= 172240
SISD0	= 172200
kw11p	= 0			/ separate clock for profiling

.globl	_prfintr, _prfbuf, _prfctr

_prfintr:
.if	kw11p - 1
	mov	PS, -(sp)
.endif
	mov	$PMSV, PS
	mov	SISA0, -(sp)
	mov	SISD0, -(sp)
	mov	r4, -(sp)
	mov	r3, -(sp)
	mov	r2, -(sp)
	mov	r1, -(sp)
	mov	r0, -(sp)

	mov	_prfbuf, SISA0	/ address of profile addresses
	mov	$77406, SISD0
.if	kw11p
	mov	16(sp), r3	/ pc
.endif
.if	kw11p - 1
	mov	22(sp), r3	/ pc
.endif
	clr	r0		/ lower bound
	mov	_prfctr, r2
	asr	r2
	dec	r2		/ upper bound

.if	kw11p
	bit	$100000, 20(sp)	/ ps
.endif
.if	kw11p - 1
	bit	$100000, 24(sp)	/ ps
.endif
	bne	4f		/ not kernel or supv, hence user

1:				/ binary search
	mov	r0, r1
	add	r2, r1
	asr	r1		/ (upper + lower) / 2
	cmp	r1, r0
	beq	2f		/ middle == lower for termination
	mov	r1, r4
	add	r1, r4
	mfpi	(r4)
	cmp	r3, (sp)+
	beq	2f		/ exact match
	bhi	3f		/ pc > profile address
	mov	r1, r2		/ lower the upper bound
	br	1b
3:
	mov	r1, r0		/ raise the lower bound
	br	1b
4:
	mov	r2, r1
	inc	r1		/ index for user mode counter
2:
	add	r1, r1		/ offset for low word of counter =
	inc	r1		/	4 * r1 + 2
	add	r1, r1
	add	_prfctr, r1	/ address of profile counters
	mfpi	(r1)
	inc	(sp)
	mtpi	(r1)
	bne	1f		/ no overflow
	sub	$2, r1
	mfpi	(r1)
	inc	(sp)		/ high word
	mtpi	(r1)
1:
	mov	(sp)+, r0
	mov	(sp)+, r1
	mov	(sp)+, r2
	mov	(sp)+, r3
	mov	(sp)+, r4
	mov	(sp)+, SISD0
	mov	(sp)+, SISA0
.if	kw11p
	rtt
.endif
.if	kw11p - 1
	mov	(sp)+, PS
	rts	pc
.endif
