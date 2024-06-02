	.globl	_divide
_divide:
	ediv	8(ap),*4(ap),*4(ap),r0
	ret
