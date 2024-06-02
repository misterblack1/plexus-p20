/*
 * Floating point.
 *
 * The following code handles the simulation of instructions which
 * have opcodes in the range 0x8e00 to 0x8eff.
 */

#include "sys/plexus.h"

.globl	fpinst
fpinst:
ei	vi
ld	r7, 20(r15)		/* r7 = floating point instruction */
lda	r2, 4(r15)		/* set r2 to adx of msb of src operand */
bit	r7, $6
jr	nz, fpinst1 
inc	r2, $8

fpinst1:			/* set r3 to adx of msb of dest operand */
lda	r3, 4(r15)
bit	r7, $7
jr	nz, fpinst2 
inc	r3, $8

fpinst2:
bit	r7, $4
jp	nz,  fconv		/* if (conversion op) goto fconv */


/* handle opcodes which do arithmetic operations involving the fpp chip */

fopr:				/* set r1 to number of bytes in operands */
ldk	r1, $8
bit	r7, $5
jr	nz, opr1		/* if (double operands) goto opr1 */
dec	r1, $4			/* adjust for float size and address */
inc	r2, $4
inc	r3, $4

opr1:				/* move src operand to fpp chip */
ld	r4, r1			/* r4 = count */
ld	r5, r3
add	r5, r1
dec	r5, $1			/* r5 = adx of lsb of src */

opr4:
ldb	rl0, *r5
sout	P_FPPDATA, r0
dec	r5, $1
djnz	r4, opr4
ld	r4, r1			/* move dest operand to fpp chip */
ld	r5, r2
add	r5, r1
dec	r5, $1			/* r5 = adx of lsb of dest */

opr5:
ldb	rl0, *r5
sout	P_FPPDATA, r0
dec	r5, $1
djnz	r4, opr5
ld	r4, r7			/* build fpp command word for operation */
and	r4, $7			/* r4 = opcode */
clr	r5			/* r5 = 0 (not compare operation) */
cp	r4, $6
jr	ne, opr6		/* if (not compare operator) goto opr6 */
ldk	r4, $2			/* use subtract op for compare */
com	r5			/* r5 = -1 (compare operation) */

opr6:
bit	r7, $5
jr	z, opr7			/* if (float operation) goto opr7 */
add	r4, $0x28		/* add constant to make command double */

opr7:
sout	P_FPPCMD, r4		/* issue command */

opr8:
sin	r0, P_FPPCMD		/* read status */
bit	r0, $7
jr	ne, opr8		/* if (busy) goto opr8 */
ld	r4, $0
sout	P_FPPCMD, r4		/* clear status of fpp chip */
test	r5			/* move result from fpp chip to dest */
jr	nz, opr9		/* if (compare operation) goto opr9 */
ld	r4, r1			/* r4 = count */
ld	r5, r3			/* r5 = adx of msb of dest */

opr10:
sin	r0, P_FPPDATA
ldb	*r5, rl0
inc	r5, $1
djnz	r4, opr10

opr9:				/* put operation status into users fcw */
ld	r1, r0			/* r1 = new fcw (start with fpp status) */
and	r1, $0x0e		/* get fpp div by 0, underflow, and overflow */
sll	r1, $1			/* put into fcw pv, da, and h bits */
bit	r0, $5
jr	eq, opr11		/* if (zero status clear) goto opr11 */
set	r1, $6			/* set fcw zero bit */
res	r0, $6			/* take care of possible neg zero */

opr11:
bit	r0, $6
jr	eq, opr12		/* if (sign status clear) goto opr12 */
set	r1, $5			/* set fcw sign bit */

opr12:
ldb	23(r15), rl1		/* save new fcw flags in users fcw */
and	r0, $0x0e
jr	eq, opr13		/* if (no hard error) goto opr13 */
/* dont cause any traps for now */

opr13:
jp	intret			/* return to users code */


/* handle conversion operations */

fconv:
ld	r1, r7			/* adjust src ptr for operand type */
and	r1, $0x0003
cp	r1, $3
jp	ge, fconv1		/* if (double operand) goto fconv1 */
inc	r2, $4
cp	r1, $1
jp	ge, fconv1		/* if (long or float) goto fconv1 */
inc	r2, $2

fconv1:
ld	r1, r7			/* adjust dest ptr for operand type */
srl	r1, $2
and	r1, $0x0003
cp	r1, $3
jp	ge, fconv2		/* if (double operand) goto fconv2 */
inc	r3, $4
cp	r1, $1
jp	ge, fconv2		/* if (long or float) goto fconv2 */
inc	r3, $2

fconv2:
ld	r1, r7
and	r1, $0x000f		/* r1 = conversion operation index */
add	r1, r1			/* make into byte offset */
ld	r1, fconvjmp(r1)	/* r1 = address of conversion code */
jp	*r1			/* jump to conversion code */

.data
fconvjmp:
.word	f_ii, f_il, f_if, f_id
.word	f_li, f_ll, f_lf, f_ld
.word	f_fi, f_fl, f_ff, f_fd
.word	f_di, f_dl, f_df, f_dd
.text

f_ii:
ldk	r1, $1
ldir	*r3, *r2, r1
jp	intret

f_il:
ld	r7, 2(r2)
ld	*r3, r7
jp	intret

f_if:
clr	r4			/* r4 = default result */
ld	r1, *r2			/* get exponent into r1 */
res	r1, $15
srl	r1, $7
sub	r1, $127 + 15		/* adjust for bias and offset */
cp	r1, $-15
jr	lt, f_if1		/* if (result < 1.0) goto f_if1 */
ldl	rr4, *r2		/* rr4 = source */
slll	rr4, $8			/* move mantissa to top of word */
or	r4, $0x8000		/* put on implied most sig bit */
sdl	r4, r1			/* adjust mantissa to proper place */
bit	*r2, $15
jr	z, f_if1
neg	r4			/* make result neg */

f_if1:
ld	*r3, r4			/* store result in dest */
jp	intret

f_id:
clr	r4			/* r4 = default result */
ld	r1, *r2			/* get exponent into r1 */
res	r1, $15
srl	r1, $4
sub	r1, $1023 + 15		/* adjust for bias and offset */
cp	r1, $-15
jr	lt, f_id1		/* if (result < 1.0) goto f_id1 */
ldl	rr4, *r2		/* rr4 = 2 msw of source */
slll	rr4, $11		/* move mantissa to top of word */
or	r4, $0x8000		/* put on implied most sig bit */
sdl	r4, r1			/* adjust mantissa to proper place */
bit	*r2, $15
jr	z, f_id1
neg	r4			/* make result neg */

f_id1:
ld	*r3, r4			/* store result in dest */
jp	intret

f_li:
ld	r7, *r2
exts	rr6
ldl	*r3, rr6
jp	intret

f_ll:
ldk	r1, $2
ldir	*r3, *r2, r1
jp	intret

f_lf:
clr	r4			/* rr4 = default result */
ld	r1, *r2			/* get exponent into r1 */
res	r1, $15
srl	r1, $7
sub	r1, $127 + 31		/* adjust for bias and offset */
cp	r1, $-31
jr	lt, f_lf1		/* if (result < 1.0) goto f_lf1 */
ldl	rr4, *r2		/* rr4 = source */
slll	rr4, $8			/* move mantissa to top of word */
or	r4, $0x8000		/* put on implied most sig bit */
sdll	rr4, r1			/* adjust mantissa to proper place */
bit	*r2, $15
jr	z, f_lf1
clr	r6
clr	r7
subl	rr6, rr4
ldl	rr4, rr6

f_lf1:
ldl	*r3, rr4		/* store result in dest */
jp	intret

f_ld:
clr	r4			/* rr4 = default result */
clr	r5
ld	r1, *r2			/* get exponent into r1 */
res	r1, $15
srl	r1, $4
sub	r1, $1023 + 31		/* adjust for bias and offset */
cp	r1, $-31
jr	lt, f_ld1		/* if (result < 1.0) goto f_ld1 */
ldl	rr4, *r2		/* rq4 = source */
ldl	rr6, 4(r2)
ldk	r0, $11			/* move mantissa to msbs of rq4 */

f_ld2:
sll	r6, $1
rlc	r5, $1
rlc	r4, $1
djnz	r0, f_ld2
or	r4, $0x8000		/* put on implied most sig bit */
sdll	rr4, r1			/* adjust mantissa to proper place */
bit	*r2, $15
jr	z, f_ld1
clr	r6
clr	r7
subl	rr6, rr4
ldl	rr4, rr6

f_ld1:
ldl	*r3, rr4		/* store result in dest */
jp	intret

f_fi:
ld	r4, *r2
clr	r5
test	r4
jr	z, f_fi1
jr	pl, f_fi2
neg	r4

f_fi2:
ld	r1, $127 + 16

f_fi3:				/* shift left till first one bit */
dec	r1, $1
sll	r4, $1
jr	nc, f_fi3
srll	rr4, $9			/* adjust mantissa to proper place */
sll	r1, $7			/* shift exponent to proper place */
or	r4, r1			/* add exponent to mantissa */
call	f_setsign		/* set the sign of the result */

f_fi1:
ldm	*r3, r4, $2		/* store result into dest */
jp	intret

f_fl:
ldl	rr4, *r2
testl	rr4
jr	z, f_fl1
jr	pl, f_fl2
clr	r6
clr	r7
subl	rr6, rr4
ldl	rr4, rr6

f_fl2:
ld	r1, $127 + 32

f_fl3:
dec	r1, $1
slll	rr4, $1
jr	nc, f_fl3
srll	rr4, $9
sll	r1, $7
or	r4, r1
call	f_setsign

f_fl1:
ldm	*r3, r4, $2
jp	intret

f_ff:
ldk	r1, $2
ldir	*r3, *r2, r1
jp	intret

f_fd:
ldl	rr4, *r2		/* rq4 = source */
ldl	rr6, 4(r2)
ldk	r0, $3			/* shift mantissa left 3 bits */

f_fd1:
sll	r6, $1
rlc	r5, $1
rlc	r4, $1
djnz	r0, f_fd1
ld	r1, *r2			/* get src exponent */
res	r1, $15
srl	r1, $4
jr	z, f_fd2		/* if (src = 0.0) goto f_fd2 */
add	r1, $127 - 1023		/* adjust for new bias */
sll	r1, $7			/* move to proper position */
and	r4, $0x007f		/* mask off old exponont bits */
or	r4, r1			/* install new exponent */
call	f_setsign		/* set the sign of the result  */

f_fd2:
ldm	*r3, r4, $2		/* store result into dest */
jp	intret

f_di:
ld	r4, *r2
clr	r5
clr	r6
clr	r7
test	r4
jr	z, f_di1
jr	pl, f_di2
neg	r4

f_di2:
ld	r1, $1023 + 16

f_di3:				/* shift left till first one bit */
dec	r1, $1
sll	r4, $1
jr	nc, f_di3
srll	rr4, $12		/* adjust mantissa to proper place */
sll	r1, $4			/* shift exponent to proper place */
or	r4, r1			/* add exponent to mantissa */
call	f_setsign		/* set the sign of the result */

f_di1:
ldm	*r3, r4, $4		/* store result into dest */
jp	intret

f_dl:
ldl	rr4, *r2
clr	r6
clr	r7
testl	rr4
jr	z, f_dl1
jr	pl, f_dl2
subl	rr6, rr4
ldl	rr4, rr6
clr	r6
clr	r7

f_dl2:
ld	r1, $1023 + 32

f_dl3:
dec	r1, $1
slll	rr4, $1
jr	nc, f_dl3
ldk	r0, $12

f_dl4:
srll	rr4, $1
rrc	r6, $1
djnz	r0, f_dl4
sll	r1, $4
or	r4, r1
call	f_setsign

f_dl1:
ldm	*r3, r4, $4
jp	intret

f_df:
ldl	rr4, *r2		/* rq4 = source padded with zero */
clr	r6
clr	r7
ldk	r0, $3

f_df1:				/* shift mantissa right 3 for new exp bits */
srll	rr4, $1
rrc	r6, $1
djnz	r0, f_df1
ld	r1, *r2			/* get original exponent */
res	r1, $15
srl	r1, $7
jr	z, f_df2		/* if (src = 0.0) goto f_df2 */
add	r1, $1023 - 127		/* adjust for new bias */
sll	r1, $4			/* move to new exponent bit position */
and	r4, $0x000f		/* clear out old exponent bits */
or	r4, r1			/* install new exponent */
call	f_setsign		/* set sign of result */

f_df2:
ldm	*r3, r4, $4		/* store result into dest */
jp	intret

f_dd:
ldk	r1, $4
ldir	*r3, *r2, r1
jp	intret


/*
 * set the mantissa sign of the floating value in rq4 to
 * the sign of the source operand whose adx is in r2.
 */

f_setsign:
res	r4, $15
bit	*r2, $15
ret	z
set	r4, $15
ret
