/ Startup code for two-stage bootstrap

PS	= 177776
cflg	= 240^sys
rtt	= 6
NFILES	= 6

.globl	_edata, _end
.globl	__init, _main
.globl	_exit, __exit, __trap
.globl	__cleanup, __devsw
.globl	_close

.text
	jmp	*$start

/
/ trap vectors
/
	trap;340	/ CPU ERROR
	trap;341	/ ILLEGAL INSTRUCTION
	trap;342	/ BPT
	trap;343	/ IOT
	trap;344	/ POWER FAIL
	trap;345	/ EMT
	trap;346	/ TRAP
.=400^.

/ restart
	mov	spsave,sp
	br	1f

spsave:	.=.+2
pcsave:	.=.+2

start:
	mov	sp,spsave
	mov	(sp)+,pcsave
	mov	$340,*$PS

	mov	$_edata,r0
	mov	$_end,r1
	sub	r0,r1
	inc	r1
	clc
	ror	r1
1:
	clr	(r0)+
	sob	r1,1b

	jsr	pc,__init
	cmp	$123456,4(sp)
	beq	2f

	tst	-(sp)
	mov	$argv0,-(sp)
	mov	$1,-(sp)
2:
	mov	$_environ,4(sp)
	jsr	pc,_main
	jsr	pc,_exit
__exit:
	mov	$NFILES-1,r2
1:
	mov	r2,(sp)
	jsr	pc,_close
	sob	r2,1b

	mov	spsave,sp
	mov	pcsave,(sp)
	rts	pc

trap:
	mov	*$PS,(sp)
	jsr	pc,__trap
	br	.

.globl	_environ

.bss
_environ:	.=.+2

.data
argv0:		argv00
argv00:		0
		<@(#)srt0.s	4.5\0>
