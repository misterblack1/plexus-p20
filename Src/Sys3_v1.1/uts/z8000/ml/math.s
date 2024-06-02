
// min function
.globl	_min
_min:
	ld	r7,2(r15)
	cp	r7,4(r15)
	ret	ule
	ld	r7,4(r15)
	ret

// max function
.globl _max
_max:
	ld	r7,2(r15)
	cp	r7,4(r15)
	ret	uge
	ld	r7,4(r15)
	ret

