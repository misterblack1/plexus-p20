/ long / int

	.globl	_divide
_divide:
	jsr	r5,csv
	mov	6(r5),r3	/ divisor
	mov	4(r5),r4	/ dividend address

	clr	r0
	mov	0(r4),r1	/ hi dividend
	div	r3,r0
	mov	r0,0(r4)	/high quotient
	mov	r1,r0		/ r
	mov	2(r4),r1	/ lo dividend
	div	r3,r0
	bvc	1f
	sub	r3,r0		/ this is the clever part
	div	r3,r0
	tst	r1
	bge	1f
	add	r3,r1
	dec	r0
1:
	mov	r0,2(r4)	/ lo quotient
	mov	r1,r0		/ remainder
	jmp	cret
