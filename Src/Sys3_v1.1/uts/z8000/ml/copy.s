#include "sys/plexus.h"

/*
 * copyio((long)addr1, addr2, count, whichseg)
 *
 * This routine copies from one address to another for count
 * bytes, with the last argument dictating which direction,
 * and which segment is involved.  The first address is a
 * segmented address, the second is an offset in the segment
 * passed in as an argument.
 *
 *	r0	old fcw
 *	r1	saved fcw
 *	r2	which seg number (read/write) / pc of error code
 *	r3	count
 *	rr4	addr1
 *	rr6	addr2 (r5 passed in)
 *
 */

.globl	_copyio
_copyio:
	ldctl	r1,fcw		/* Save old fcw */
	ldl	rr4,2(r15)	/* First address is a long */
	ld	r7,6(r15)	/* Second address */
	ld	r3,8(r15)	/* Number of bytes to transfer */
	ld	r2,10(r15)	/* Read/write, user/kernel, i/d */
	ld	r6,r2		/* Determine segment number */
	res	r6,$0		/* Get rid of read/write bit */
	ld	r6,cbase(r6)	/* Get segment number from table */
	ld	r0,$(B_SEG+B_SYS+B_NVI)
	ldctl	fcw,r0		/* Change to segmented mode */
	bit	r2,$0		/* Now figure out if read or write */
	ld	r2,$coperr	/* Where to go upon error */
	jr	z,writio	/* Even values are writes */
	ldirb	*r6,*r4,r3	/* Copy block */
	ldctl	fcw,r1		/* Restore old fcw */
	clr	r7
	ret
writio:
	ldirb	*r4,*r6,r3	/* Copy block */
	ldctl	fcw,r1		/* Restore old fcw */
	clr	r7
	ret

.data
cbase:
	.word	UDSEG
	.word	SDSEG
	.word	UISEG
	.word	SISEG
.text


/*
 * copyseg(src, dest)
 *
 * This routine copies `src' to `dest' where both are physical
 * page numbers. Interrupts are turned off for the whole transfer
 * to protect the map slots used to get to the pages. There are
 * no error conditions. This routine should not cause a segmentation
 * trap.
 *
 *	r0	current fcw
 *	r1	callers fcw
 *	r2	temp
 *	r3	word count
 *	r4	src seg number
 *	r5	src offset
 *	r6	dest seg number
 *	r7	dest offset
 */

.globl	_copyseg
_copyseg:
	ldctl	r1, fcw
	ld	r3, $(PAGESIZE >> 1)
	ld	r4, $XDSEG
	ld	r5, $0
	ld	r6, $XISEG
	ld	r7, $0
	di	vi		/* vectored interrupts off */
	ld	r2, 2(r15)
	sout	P_MXD0, r2	/* set source phy page in source map */
	ld	r2, 4(r15)
	sout	P_MXI0, r2	/* set dest phy page in dest map */
	ld	r0, $(B_SEG + B_SYS + B_NVI)
	ldctl	fcw, r0		/* segmented mode on */
	ldir	*r6, *r4, r3
	ldctl	fcw, r1		/* restore callers fcw */
	ret
	
	
/*
 * clearseg(pn)
 *
 * This code sets `pn' physical page number to zeros. Interrupts are
 * off during the procedure to protect the map slot used to access
 * the page. No errors are returned. The registers are as follows:
 *
 *	r0	current fcw
 *	r1	callers fcw
 *	r2	temp
 *	r3	word count
 *	r4	src seg
 *	r5	src offset
 *	r6	dest seg
 *	r7	dest offset
 */

.globl	_clearseg
_clearseg:
	ldctl	r1, fcw
	ld	r3, $((PAGESIZE >> 1) - 1)
	ld	r4, $XDSEG
	ld	r5, $0
	ld	r6, $XDSEG
	ld	r7, $2
	di	vi		/* vectored interrupts off */
	ld	r2, 2(r15)
	sout	P_MXD0, r2	/* set mapping to phsical page number */
	ld	r0, $(B_SEG + B_SYS + B_NVI)
	ldctl	fcw, r0		/* segmented mode on */
	clr	*r4
	ldir	*r6, *r4, r3
	ldctl	fcw, r1		/* restore callers fcw */
	ret


/*
 * The following routine clears out a block of memory.  It expects
 * a segmented address and a count as arguments.
 */
.globl	_clear
_clear:
	ldctl	r1,fcw		/* Save old fcw */
	ldl	rr2,2(r15)	/* Get starting address of where to clear */
	ldl	rr4,rr2		/* Make another copy */
	ld	r6,6(r15)	/* Number of bytes to clear */
	ld	r0,$(B_SEG+B_SYS+B_NVI)
	ldctl	fcw,r0		/* Turn on segmented mode */
	bit	r6,$0		/* Check for odd count */
	jr	ne,clearbytes
	bit	r3,$0		/* Check for odd address */
	jr	ne,clearbytes
	srl	r6,$1		/* Change to word count */
	ld	*r2,$0		/* Write a zero to first word */
	inc	r5,$2		/* Increment pointer to next word */
	dec	r6,$1		/* Decrement count since we've done one word */
	ldir	*r4,*r2,r6	/* Clear out block */
	ldctl	fcw,r1		/* Change back to old fcw */
	ret

clearbytes:
	ldb	*r2,$0		/* Write a zero to first byte */
	inc	r5,$1		/* Increment to next byte */
	dec	r6,$1		/* Decrement count since we've done one byte */
	ldirb	*r4,*r2,r6	/* Clear out block */
	ldctl	fcw,r1		/* Change back to old fcw */
	ret
	


/*
 * The following routines are for coping large block between system
 * data space and the current users instruction or data space or
 * the local i/o space.
 * All the routines are called with source address, destination address, and
 * byte count. Note that interrupts are left on like in the PDP 11
 * version. The registers are used as follows:
 *
 * 	r0	temp for fcw
 *	r1	callers fcw
 *	r2	pc of error code
 *	r3	byte/word count for transfer
 *	r4	source segment
 *	r5	source offset
 *	r6	dest segment
 *	r7	dest offset
 */

.globl	_copyin
_copyin:
ld	r4, _udseg
ld	r5, 2(r15)
ld	r6, $SDSEG
ld	r7, 4(r15)
ld	r3, 6(r15)
jr	copcomm

.globl	_copyiin
_copyiin:
	ld	r4, _uiseg
	ld	r5, 2(r15)
	ld	r6, $SDSEG
	ld	r7, 4(r15)
	ld	r3, 6(r15)
	jr	copcomm

.globl	_copymin
_copymin:
	ld	r4,$LIOSEG 
	ld	r5, 2(r15)
	ld	r6, $SDSEG
	ld	r7, 4(r15)
	ld	r3, 6(r15)
	jr	copcomm

.globl	_copyout
_copyout:
	ld	r4, $SDSEG
	ld	r5, 2(r15)
	ld	r6, _udseg
	ld	r7, 4(r15)
	ld	r3, 6(r15)
	jr	copcomm

.globl	_copyiout
_copyiout:
	ld	r4, $SDSEG
	ld	r5, 2(r15)
	ld	r6, _uiseg
	ld	r7, 4(r15)
	ld	r3, 6(r15)
	jr	copcomm

.globl	_copymout
_copymout:
	ld	r4, $SDSEG
	ld	r5, 2(r15)
	ld	r6, $LIOSEG
	ld	r7, 4(r15)
	ld	r3, 6(r15)
	jr	copcomm

copcomm:
	ldctl	r1, fcw
	ld	r2, $coperr
	ld	r0, $(B_SEG | B_SYS | B_NVI)
	ldctl	fcw, r0		/* segmented mode on */
	bit	r5,$0		/* check to see if odd source address */
	jr	ne,copbytes
	bit	r7,$0		/* check to see if odd dest address */
	jr	ne,copbytes
	srl	r3, $1		/* r3 = number of words, carry if odd */
	jr	z, onebyte	/* zero flag set if single byte */
	ldir	*r6, *r4, r3
	nop
	jr	nc, copdone	/* if carry not set, then count even */
onebyte:
	ldb	rl3, *r4	/* take care of last byte */
	ldb	*r6, rl3	/* (this is faster than ldib) */
	nop
copdone:
	ldctl	fcw, r1		/* restore users fcw */
	clr	r7
	ret

copbytes:
	ldirb	*r6, *r4, r3
	nop
	ldctl	fcw, r1		/* restore users fcw */
	clr	r7
	ret

coperr:
	ldctl	fcw, r1
	ld	r7, $-1
	ret

/*
 * cpyinb( src, dest, count, segflg )
 *
 * The following routine copies (using ldirb) from the users data area
 * space to the system data area.
 */

.globl 	_cpyinb
_cpyinb:
	ld	r2, 8(r15)
	test	r2
	ld	r4, _udseg
	jp	z, cpyud
	sub	r2, $1
	jp	nz, cpyui
	ld 	r7, 4(r15)
	ld	r5, 2(r15)
	ld	r3, 6(r15)
	ld	r2,$coperr	/* Where to go upon error */
	ldirb	*r7, *r5, r3
	jp	cpyok
cpyui:
	ld	r4, _uiseg
cpyud:
	ld	r5, 2(r15)
	ld	r6, $SDSEG
	ld	r7, 4(r15)
	ld	r3, 6(r15)
	ld	r2,$coperr	/* Where to go upon error */
	ldctl	r1, fcw
	ld	r0, $(B_SEG | B_SYS | B_NVI)
	ldctl	fcw, r0
	ldirb	*r6, *r4, r3
	ldctl	fcw, r1
cpyok:
	clr	r7
	ret

