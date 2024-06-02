.data
// Bootstrap program executed in user mode
// to bring up the system.
.globl	_icode, _szicode
_icode:
	sc	$11		/* Exec system call */
	.word	init - _icode
	.word	initp - _icode
	jr	.
initp:
	.word	init - _icode
	.word	0
init:
	.asciiz "/etc/init"
endicode:

_szicode:
	.word	endicode - _icode

