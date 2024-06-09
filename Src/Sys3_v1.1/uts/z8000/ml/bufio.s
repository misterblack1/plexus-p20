#include "sys/plexus.h"

.globl	_bigetc, _biputc
_bigetc:
	ldctl	r1,fcw		/* Save old fcw */
	ldl	rr2,2(r15)	/* Get address of buffer */
	add	r3,6(r15)	/* Add in offset */
	ld	r0,$(B_SEG+B_SYS+B_NVI)
	ldctl	fcw,r0		/* Change to segmented mode */
	ldb	rl7,*r2		/* Fetch character from buffer */
	clrb	rh7		/* Clear high byte for return */
	ldctl	fcw,r1		/* Restore fcw to where it was */
	ret

_biputc:
	ldctl	r1,fcw		/* Save old fcw */
	ldl	rr2,2(r15)	/* Get address of buffer */
	add	r3,6(r15)	/* Add in offset */
	ld	r7,8(r15)	/* Get value to be stored */
	ld	r0,$(B_SEG+B_SYS+B_NVI)
	ldctl	fcw,r0		/* Change to segmented mode */
	ldb	*r2,rl7		/* Stick character into buffer */
	ldctl	fcw,r1		/* Restore fcw to where it was */
	ret

.globl	_biget, _biput
_biget:
	ldctl	r1,fcw		/* Save old fcw */
	ldl	rr2,2(r15)	/* Get address of buffer */
	add	r3,6(r15)	/* Add in offset */
	ld	r0,$(B_SEG+B_SYS+B_NVI)
	ldctl	fcw,r0		/* Change to segmented mode */
	ld	r7,*r2		/* Fetch word from buffer */
	ldctl	fcw,r1		/* Restore fcw to where it was */
	ret

_biput:
	ldctl	r1,fcw		/* Save old fcw */
	ldl	rr2,2(r15)	/* Get address of buffer */
	add	r3,6(r15)	/* Add in offset */
	ld	r7,8(r15)	/* Get value to be stored */
	ld	r0,$(B_SEG+B_SYS+B_NVI)
	ldctl	fcw,r0		/* Change to segmented mode */
	ld	*r2,r7		/* Stick word into buffer */
	ldctl	fcw,r1		/* Restore fcw to where it was */
	ret

.globl	_bigetl, _biputl
_bigetl:
	ldctl	r1,fcw		/* Save old fcw */
	ldl	rr2,2(r15)	/* Get address of buffer */
	add	r3,6(r15)	/* Add in offset */
	ld	r0,$(B_SEG+B_SYS+B_NVI)
	ldctl	fcw,r0		/* Change to segmented mode */
	ldl	rr6,*r2		/* Fetch long from buffer */
	ldctl	fcw,r1		/* Restore fcw to where it was */
	ret

_biputl:
	ldctl	r1,fcw		/* Save old fcw */
	ldl	rr2,2(r15)	/* Get address of buffer */
	add	r3,6(r15)	/* Add in offset */
	ldl	rr6,8(r15)	/* Get value to be stored */
	ld	r0,$(B_SEG+B_SYS+B_NVI)
	ldctl	fcw,r0		/* Change to segmented mode */
	ldl	*r2,rr6		/* Stick long into buffer */
	ldctl	fcw,r1		/* Restore fcw to where it was */
	ret

