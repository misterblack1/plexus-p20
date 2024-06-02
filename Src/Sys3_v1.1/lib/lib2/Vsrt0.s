# Startup code for standalone programs

	.set	HIGH,0x18
	.set	IPL,18
	.set	SCBB,17

	.set	NFILES,6

	.globl	start
	.globl	_edata
	.globl	_end
	.globl	__init
	.globl	_main
	.globl	_exit
	.globl	__exit
	.globl	__cleanup
	.globl	__devsw
	.globl	_close

	.text

# Scb starts at location 0 
	.space	128*4

# dump to tape (loc 200 hex)
	calls	$0,_tpdump
	halt

# restart program (208 hex)
	movq	save,fp
	brb	L0

save:	.space	4			# save area for fp and sp

start:
	.word	0x0000
	mtpr	$HIGH,$IPL		# up ipl
	mtpr	$0,$SCBB		# show where vectors are
	movq	fp,save			# save fp and sp
L0:
	moval	_edata,r0		# calculate bss size
	subl3	r0,$_end,r1
	movc5	$0,(r0),$0,r1,(r0)	# clear bss
	calls	$0,__init
	pushal	_environ		# environment
	cmpl	$0x12345678,4(ap)	# called by 'sa' or 'fboot'?
	bneq	L1
	pushl	12(ap)
	pushl	8(ap)
	brb	L2
L1:
	pushal	argv0
	pushl	$1
L2:
	calls	$3,_main
	calls	$0,_exit
	brb	L4

__exit:
	.word	0x0000
L4:
	movl	$NFILES-1,r2
L5:
	pushl	r2
	calls	$1,_close
	sobgeq	r2,L5

	movq	save,fp
	ret

argv0:	.long	argv00
argv00:	.long	0
	.comm	_environ,4
