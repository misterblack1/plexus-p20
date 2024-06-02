 /***********************************************
 *						*
 *	Z8000 IEEE Floating Point Package	*
 *						*
 *	Kevin Enderby				*
 *	1852 Edgewood Drive 			*
 *	Palo Alto, CA 94303			*
 *	(415) 321-1154				*
 *						*
 *	Release 2.0		10/23/81	*
 *	Modified by Jim Edele	 1/6/82		*
 ************************************************/

st_ur0	= 6	//stack offset for user register 0
st_fpop	= 22	//stack offset for floating point opcode

fpext	= 0x8e	//floating point operation code 8eXX
//bit positions in opcode
op_conv	= 4	//operation (1) or conversion (0)
db_flt	= 5	//double (1) or float (0) operation
src_reg	= 6	//floating point src register fpr1 (1) or fpr0 (0)
dst_reg	= 7	//floating point dest register fpr1 (1) or fpr0 (0)

//masks for types of operations
op_add	= 0x0001
op_sub	= 0x0002
op_mult	= 0x0003
op_div	= 0x0004
op_neg	= 0x0005

//types of number formats
type_i	= 0x0		//integer
type_lg	= 0x1		//long
type_fl	= 0x2		//float
type_db = 0x3		//double

signbit	= 0x8000	//sign bit

/* masks to decode the opcode */
op_mask		= 0x0f	/* opcode mask */
dest_mask	= 0x0c	/* conversion destination type mask */
source_mask	= 0x03	/* conversion source type mask */

/* error codes returned */
ovflow		= 0x02	/* exponent overflow */
unflow		= 0x04	/* exponent underflow */
divzero		= 0x08	/* division by zero */

mbitsfl		= 23	/* number of mantissa bits for a float */
mbitsdb		= 52	/* number of mantissa bits for a double */
xcesfl		= 127	/* exponent bias for a float */
xcesdb		= 1023	/* exponent bias for a double */
minxfl		= -126	/* minimum exponent for a float */
minxdb		= -1022	/* minimum exponent for a double */
maxxfl		= 127	/* maximum exponent for a float */
maxxdb		= 1023	/* maximum exponent for a double */
man1fl		= 7	/* number of mantissa bits in the first word of a float */
man1db		= 4	/* number of mantissa bits in the first word of a double */

/* masks to put together and take apart floating point numbers */
expfl		= 0x7f80	/* mask for float exponent */
expdb		= 0x7ff0	/* mask for double exponent */
mandb		= 0x000f	/* mask to get mantissa for double */
manfl		= 0x007f	/* mask to get mantissa for float */
imp1db		= 4		/* implied 1 position for a double */
imp1fl		= 7		/* implied 1 position for a float */

/* masks for normalization */
d1shfl		= 0xff80	/* masks to decide which direction to shift */
d2shfl		= 0x8000	/*  to normalize a float */
d1shdb		= 0xfff0	/* masks to decide which direction to shift */
d2shdb		= 0xe000	/*  to normalize a double */
rrmaskfl	= 0xff00	/* mask to know when to stop rolling right for float */
rrmaskdb	= 0xffe0	/* mask to know when to stop rolling right for a double */
rlmaskfl	= 0x0080	/* mask to know when to stop rolling left for a float */
rlmaskdb	= 0x0010	/* mask to know when to stop rolling left for a double */

/* masks for rounding */
guardbit	= 0x8000	/* the guard bit in rounding */
stkmask		= 0x3fff	/* mask to know if sticky bit is to be set */
stkset		= 0x3fff	/* mask to set sticky bit */
stkclr		= 0xc000	/* mask to clear sticky bit */
stkbit		= 13		/* sticky bit position */
.page
.globl	_softfp
/*
 * Front end processor: calls fp simulator, places results
 * in user's registers, handles error condition.
 * (written by Jim Edele)
 */
op_cp = 6			//compare opcode
opcd = st_fpop - 2		//where fp opcode is on stack for front end
.text
_softfp:
.globl _debug
	ei	vi
	ld	r0,opcd(r15)	//r0<= fp opcode
	ld	r1,$0		//r1<= 0 implies no compare oper
	bit	r0,$op_conv
	jr	nz,sof1		//if(conversion) goto sof1
	and	r0,$op_mask	//r0<= operation opcode
	cpb	rl0,$op_cp
	jr	ne,sof1		//if(not a compare oper) goto sof1
	res	opcd(r15),$2	//change compare to subtract
	ld	r1,$1		//r1<= 1 implies compare oper
sof1:
	call	soft2fp
	test	r1
	jr	z,sof10		//if(not a compare oper) goto to sof10
	set	opcd(r15),$2	//restore compare oper
/* build new fcw for user in r6 (lower byte only) */
sof10:
	ldb	rl6, *r7	/* r6 <= error code */
	and	r6, $0x0e	/* get fpp div by 0, underflow, and overflow */
	sll	r6, $1		/* put into fcw pv, da, and h bits */
/* determine size of result */
	ld	r0,opcd(r15) //r0 <= fp opcode
	ld	r1,$2		//r1<= size of result (2 for float result)
	bit	r0,$op_conv	//test for operation or conversion
	jr	z,anop		//jp for an operation
/* a conversion: is result an integer, double, or float */
	and	r0,$dest_mask
	jr	nz,dblorflt	//a double or float result
	ld	r1,$1		//an integer result. r1 <= size of result
	jr	sof5
dblorflt:			//conversion: double or float result
	cpb	rl0,$(type_db<<2)	//is result a double?
	jr	ne,sof5	//jp if result not a double
	ld	r1,$4		//conversion: double result
	jp	sof5
/* an operation: is result a double or float */
anop:
	bit	r0,$db_flt
	jr	z,zerores	//if(float result) goto zerores
	ld	r1,$4		//double result.  r1<= size of result
//check for zero result (double)
	testl	6(r7)
	jr	nz,negres	//if(double result != 0) goto negres
//check for zero result (float)
zerores:
	ldl	rr4,2(r7)	//rr4 <= float result or upper half double
	and	r4,$0x7fff	//mask out sign bit
	testl	rr4
	jr	nz,negres	//if(result != 0) goto negres
	set	r6,$6		//set zero bit in user's fcw
	res	2(r7),$15	//turn off sign bit in result (for neg 0)
//check for negative result
negres:
	bit	2(r7),$15
	jr	z,sof4		//if(result is positive) goto sof4
	set	r6,$5		//set sign bit in user's fcw
sof4:				//now check for compare operator
	and	r0,$op_mask	//r0<= operation opcode
	cpb	rl0,$op_cp
	jr	eq,sof3		//if(compare oper) goto sof3
//put result in user's dest register(s)
//r1 has size of dest opnd
sof5:				//load r3 with adx of user's dest reg
	lda	r2,12(r15)	//r2 <= adx user's r4 
	bit	opcd(r15),$dst_reg	//is destination register fpr0 ?
	jr	nz,sof6		//jp if dest reg is fpr1
	inc	r2,$8		//r2 <= adx user's r7 + 2
sof6:
	sub	r2,r1
	sub	r2,r1		//r2 <= adx user's dest reg (fp0 or fp1)
	ld	r3,r7		//r3 <= adx error code
	inc	r3,$2		//r3 <= adx result
	ldir	*r2,*r3,r1	//copy results to user's destination reg(s)
/* put operation status into users fcw */
sof3:
	ldb	23(r15), rl6	/* save new fcw flags in users fcw */
	jp	intret

/* end of front end processor */

soft2fp:
	ldl	savereg,rr0	//save reg0,1
	ldl	savereg+4,rr2	//save reg2,3
	ld	r3,$savereg+8
	ldm	*r3,r4,$11	//save reg4-14

	ld	r14,r15		//set up frame pointer

	ld	r0,st_fpop(r14)	//load floating point opcode
	cpb	rh0,$fpext
	jp	nz,done		//if not a floating point opcode exit

	ldb	op_code,rl0	//opcode for return
	ldb	error_code,$0	//clear return error code

	bit	r0,$op_conv	//test for operation or conversion
	jr	z,itsop
	jr	itsconv

//select operation
itsop:
	bit	r0,$db_flt
	jr	z,itsfl

//double operations selection

//load destination (original: source) into (r6,r7,r8,r9) 
//load source (original: destination) into (r10,r11,r12,r13)

	bit	r0,$src_reg
	jr	z,sdfp0
	ldl	rr10,st_ur0(r14)	//load source from fpr1
	ldl	rr12,st_ur0+4(r14)
	jr	dbdest
sdfp0:	ldl	rr10,st_ur0+8(r14)	//load source from fpr0
	ldl	rr12,st_ur0+12(r14)
dbdest:	bit	r0,$dst_reg
	jr	z,ddfpr0
	ldl	rr6,st_ur0(r14)		//load dest from fpr1
	ldl	rr8,st_ur0+4(r14)
	jr	dbcase
ddfpr0:	ldl	rr6,st_ur0+8(r14)	//load dest from fpr0
	ldl	rr8,st_ur0+12(r14)

//branch to double operations
dbcase:	and	r0,$op_mask
	cp	r0,$op_div
	jp	z,div_db
	cp	r0,$op_mult
	jp	z,mult_db
	cp	r0,$op_add
	jp	z,add_db
	cp	r0,$op_sub
	jr	nz,dbc20
//negate second operand if not zero
	testl	rr10
	jr	nz,dbc10
	testl	rr12
	jp	z,add_db_zp	//jump to add_db at the point where second operand is zero
dbc10:	xor	r10,$signbit
	jp	add_db
dbc20:	cp	r0,$op_neg
	jp	nz,done		//operation code doesn't exist default NOP
//negate src (original: dest) if not zero
	testl	rr10
	jr	nz,db_neg
	testl	rr12
	jp	z,exit_db
db_neg:	xor	r10,$signbit
	jp	exit_db

//single floating point operation select

itsfl:

//load destination (original: source) and source (original: destination)
//into (r8,r9) and (r12,r13) respectively
	bit	r0,$src_reg
	jr	z,sffpr0
	ldl	rr12,st_ur0+4(r14)	//load source from fpr1
	jr	fldest
sffpr0:	ldl	rr12,st_ur0+12(r14)	//load source from fpr0
fldest:	bit	r0,$dst_reg
	jr	z,dffpr0
	ldl	rr8,st_ur0+4(r14)	//load dest from fpr1
	jr	flcase
dffpr0:	ldl	rr8,st_ur0+12(r14)	//load dest from fpr0

//branch to floating point operations
flcase:	and	r0,$op_mask
	cp	r0,$op_div
	jp	z,div_fl
	cp	r0,$op_mult
	jp	z,mult_fl
	cp	r0,$op_add
	jp	z,add_fl
	cp	r0,$op_sub
	jr	nz,flc10
//negate second operand if not zero
	testl	rr12
	jp	z,add_fl_zp	//jump to add_fl at the point the second operand is zero
	xor	r12,$signbit
	jp	add_fl
flc10:	cp	r0,$op_neg
	jp	nz,done		//operation code doesn't exist default NOP
//negate src (original: dest) if not zero
	testl	rr12
	jp	z,exit_fl
	xor	r12,$signbit
	jp	exit_fl

//select conversion
itsconv:
	ld	r1,r0		//decode operand for conversions
	and	r1,$source_mask
	jp	z,ld_int
	cpb	rl1,$type_db
	jr	nz,con40

//source is double
//load double into (r10,r11,r12,r13)
ld_db:	bit	r0,$src_reg
	jr	z,ld_db10
	ldl	rr10,st_ur0(r14)	//load source (double) from fpr1
	ldl	rr12,st_ur0+4(r14)
	jr	ld_db20
ld_db10:
	ldl	rr10,st_ur0+8(r14)	//load source (double) from fpr0
	ldl	rr12,st_ur0+12(r14)
ld_db20:
	and	r0,$dest_mask
	jr	nz,con10

//source is double dest is int
db_fix:	sub	r1,r1		//set fix flag (r1) to int (0)

dfx10:	testl	rr10		//if zero return zero
	jr	nz,dfx20
	testl	rr12
	jr	nz,dfx20
	clr	w0
	clr	w1
	clr	w2
	clr	w3
	jp	done

dfx20:	ld	r2,r10		//extract exponent
	and	r2,$expdb
	srl	r2,$man1db
	sub	r2,$xcesdb
	ld	r5,r2
	sub	r5,$man1db

	ld	r4,r10		//extract sign
	and	r4,$signbit

	and	r10,$mandb	//extract mantissa
	set	r10,$imp1db
	ex	r10,r12
	ex	r11,r13
	ld	r11,r10
	sub	r10,r10
	jr	ff15		//branch to the floating point to fix routine

con10:	cpb	rl0,$(type_fl<<2)
	jr	nz,con20

//source is double dest is float
condb_fl:
	testl	rr10		//if zero return zero
	jr	nz,cdf10
	testl	rr12
	jr	nz,cdf10
	clr	w0
	clr	w1
	jp	done

cdf10:	ld	r4,r10		//extract sign
	and	r4,$signbit

	ld	r2,r10		//extract exponent
	and	r2,$expdb
	srl	r2,$man1db

	cp	r2,$maxxfl+xcesdb	//if exponent is too large return 
	jr	le,cdf20		// maximum values (+ or -)
	or	r4,$0x7fff
	ld	w0,r4
	ld	w1,$0xffff
	jp	done
cdf20:	cp	r2,$minxfl+xcesdb	//if exponent is too small return zero
	jr	ge,cdf30
	clr	w0
	clr	w1
	jp	done

cdf30:	and	r10,$mandb		//extract mantissa

	rlc	r12			//shift mantissa into position
	rlc	r11
	rlc	r10

	rlc	r12
	rlc	r11
	rlc	r10

	rlc	r12
	rlc	r11
	rlc	r10

	sub	r2,$xcesdb-xcesfl	//put together result
	sll	r2,$man1fl
	or	r10,r2
	or	r10,r4
	ld	w0,r10
	ld	w1,r11
	jp	done

con20:	cpb	rl0,$(type_lg<<2)
	jr	nz,con30

//source is double dest is long
	ldk	r1,$type_lg		//set fix flag (r1) to type long 
	jr	dfx10

//source is double dest is double
con30:	ldl	w0,rr10
	ldl	w2,rr12
	jp	done

con40:	cpb	rl1,$type_fl
	jp	nz,con80

//source is float
//load float into rr12
ld_fl:	bit	r0,$src_reg
	jr	z,ld_fl10
	ldl	rr12,st_ur0+4(r14)	//load source (float) from fpr1
	jr	ld_fl20
ld_fl10:
	ldl	rr12,st_ur0+12(r14)	//load source (float) from fpr0
ld_fl20:
	and	r0,$dest_mask
	jr	nz,con50

//source is float dest is int
fl_fix:	sub	r1,r1		//set fix flag (r1) to int (0)

ff5:	testl	rr12		//if zero return zero
	jr	nz,ff10
	clr	w0
	clr	w1
	jp	done

ff10:	ld	r2,r12		//extract exponent (r2)
	and	r2,$expfl
	srl	r2,$man1fl
	sub	r2,$xcesfl
	ld	r5,r2		//exponent used for shifting (r5)
	sub	r5,$man1fl

	ld	r4,r12		//extract sign (r4)
	and	r4,$signbit

	and	r12,$manfl	//extract mantissa of float (rr12)
	set	r12,$imp1fl
	subl	rr10,rr10	//clear extra high and low words of mantissa
				// (r10,r11) used in shifting

ff15:	or	r2,r2		//if the exponent is negative return zero
	jr	pl,ff20
	clr	w0
	or	r1,r1
	jp	z,done
	clr	w1
	jp	done

ff20:	or	r1,r1		//if the exponent is too large return maximum
	jr	nz,ff35		// values (+ or -)
	cp	r2,$14
	jr	le,ff40	
	or	r4,r4
	jr	z,ff30
	ld	w0,r4
	jp	done
ff30:	ld	w0,$0x7fff
	jp	done

ff35:	cp	r2,$30
	jr	le,ff40
	or	r4,r4
	jr	z,ff36
	ld	w0,r4
	clr	w1
	jp	done
ff36:	ld	w0,$0x7fff
	ld	w1,$0xffff
	jp	done

ff40:	//full word shifts
	cp	r5,$24		//two word shift
	jr	le,ff50
	sub	r5,$32
	ld	r10,r12
	ld	r12,r13
	ld	r13,r11
	jr	ff70
ff50:	cp	r5,$8		//one word shift
	jr	le,ff60
	dec	r5,$16
	jr	ff70
ff60:	ld	r11,r13		//no word shift
	ld	r13,r12
	sub	r12,r12

ff70:	or	r5,r5		//final shifts
	jr	z,ff110
	jr	mi,ff90
ff80:	rlc	r11		//final left shifts
	rlc	r13
	rlc	r12
	djnz	r5,ff80
	jr	ff110
ff90:	neg	r5		//final right shifts
ff100:	rrc	r10
	rrc	r12
	rrc	r13
	djnz	r5,ff100

ff110:	or	r4,r4		//if negative negate result
	jr	z,ff120
	com	r12
	com	r13
	addl	rr12,$1

ff120:	or	r1,r1		//return result
	jr	z,ff130
	ldl	w0,rr12
	jp	done
ff130:	ld	w0,r13
	jp	done

con50:	cpb	rl0,$(type_db<<2)
	jr	nz,con60

//source is float dest is double
confl_db:
	testl	rr12		//if zero return zero
	jr	nz,cfd10
	clr	w0
	clr	w1
	clr	w2
	clr	w3
	jp	done

cfd10:	ld	r4,r12		//extract sign
	and	r4,$signbit

	ld	r2,r12		//calculate exponent
	and	r2,$expfl
	srl	r2,$(man1fl-man1db)
	add	r2,$((xcesdb-xcesfl)<<(man1db))

	and	r12,$manfl	//extract mantissa

	ld	r10,r13		//shift mantissa into position
	sub	r11,r11
	srll	rr10,$(man1fl-man1db)
	srll	rr12,$(man1fl-man1db)

	or	r12,r2		//put together result
	or	r12,r4
	ldl	w0,rr12
	ld	w2,r11
	clr	w3
	jp	done

con60:	cpb	rl0,$(type_lg<<2)
	jr	nz,con70

//source is float dest is long
	ldk	r1,$type_lg	//set fix flag (r1) to long (type_lg = 1)
	jp	ff5

//source is float dest is float
con70:	ldl	w0,rr12
	jp	done

con80:	cpb	rl1,$type_lg
	jr	nz,con120

//source is long
//load long into rr12
ld_lg:	bit	r0,$src_reg
	jr	z,ld_lg10
	ldl	rr12,st_ur0+4(r14)	//load source (long) from fpr1
	jr	ld_lg20
ld_lg10:
	ldl	rr12,st_ur0+12(r14)	//load source (long) from fpr0
ld_lg20:
	and	r0,$dest_mask
	jr	nz,con90

//source is long dest is int
	ld	w0,r13
	jp	done

con90:	cpb	rl0,$(type_db<<2)
	jr	nz,con100

//source is long dest is double
fix_db:	testl	rr12		//if zero return zero
	jr	nz,fxd10
	clr	w0
	clr	w1
	clr	w2
	clr	w3
	jp	done

fxd10:	jr	pl,fxd20	//if negative negate and set sign (r4)
	com	r12
	com	r13
	addl	rr12,$1
	ld	r4,$signbit
	jr	fxd30

fxd20:	sub	r4,r4		//set sign for positive numbers

fxd30:	sub	r1,r1		//clear sticky word (r1)
	sub	r3,r3		//clear round flag (r3)
	subl	rr10,rr10	//clear the rest of the mantissa
	ld	r2,$mbitsdb+xcesdb	//set exponent (r2)
	jp	norm_db

con100:	cpb	rl0,$(type_fl<<2)
	jr	nz,con110

//source is long dest is float
fix_fl:	testl	rr12		//if zero return zero
	jr	nz,fxf10
	clr	w0
	clr	w1
	jp	done

fxf10:	jr	pl,fxf20	//if negative negate it and set sign (r4)
	com	r12
	com	r13
	addl	rr12,$1
	ld	r4,$signbit
	jr	fxf30

fxf20:	sub	r4,r4		//set sign for positive numbers

fxf30:	sub	r11,r11		//clear sticky word (r11)
	sub	r3,r3		//clear round flag (r3)
	ld	r10,$mbitsfl+xcesfl	//set exponent (r10)
	jp	norm_fl

//source is long dest is long
con110:	ldl	w0,rr12
	jp	done

//source is int
//load int into r13
ld_int:	bit	r0,$src_reg
	jr	z,ld_int10
	ld	r13,st_ur0+6(r14)	//load source (int) from fpr1
	jr	ld_int20
ld_int10:
	ld	r13,st_ur0+14(r14)	//load source (int) from fpr0
ld_int20:
con120:	and	r0,$dest_mask
	jr	nz,con130

//source is int dest is int
	ld	w0,r13
	jp	done

con130:	cpb	rl0,$(type_db<<2)
	jr	nz,con140

//source is int dest is double
	exts	rr12
	jr	fix_db

con140:	cpb	rl0,$(type_fl<<2)
	jr	nz,con150

//source is int dest is float
	exts	rr12
	jr	fix_fl

//source is int dest is long
con150:	exts	rr12
	ldl	w0,rr12
	jp	done

.page
/****************************************
 * double floating point multiplication *
 ****************************************/
/*
	INPUTS:	source operand	(r6,r7,r8,r9)
		dest operand	(r10,r11,r12,r13)
	Both operands are double floating point numbers

	Register Allocations:
	(r2)	exponent of result, right justified, still biased
	(rh3)	sign of result
	(r4,r5,r6,r7,r1)   is where the mantissa of the result is accumulated
			   (Note (r1) is also the sticky register)
	the source mantissa is loaded into (workarea)
	the dest mantissa is loaded into (workarea+8)
*/
mult_db:
	testl	rr6		//if either number is zero return zero
	jr	nz,md10
	testl	rr8
	jr	z,md20
md10:	testl	rr10
	jr	nz,md30
	testl	rr12
	jr	nz,md30
md20:	clr	w0
	clr	w1
	clr	w2
	clr	w3
	jp	done

md30:	ld	r2,r6		//calculate exponent of result (r2)
	and	r2,$expdb
	ld	r5,r10
	and	r5,$expdb
	add	r2,r5
	srl	r2,$man1db
	sub	r2,$xcesdb+man1db

	ld	r3,r6		//calculate sign of result (rh3)
	xor	r3,r10
	and	r3,$signbit

	and	r6,$mandb	//extract mantissas
	set	r6,$imp1db
	and	r10,$mandb
	set	r10,$imp1db

	ldl	workarea+4,rr8	//load workarea with mantissas to perform a 4
	rlc	r8		// word multiply in partial products. To be
	rlc	r7		// accumulated in rq4. Note unsigned multiply
	rlc	r6		// is 31 bits times 31 bits. This is the reason
	ldl	workarea,rr6	// for all the shifting before adding to the
	ldl	workarea+12,rr12 // accumulator. rq8 and rr12 are used to form
	rlc	r12		// the partial products.
	rlc	r11
	rlc	r10
	ldl	workarea+8,rr10

	sub	r4,r4		//clear accumulator
	sub	r6,r6
	
	ldl	rr10,workarea+4	//first partial product
	res	r10,$15
	ldl	rr12,workarea+12
	res	r12,$15
	multl	rq8,rr12
	ldl	workarea+16,rr10
	ld	r7,r8
	ld	r1,r9

	ldl	rr10,workarea+4	//second partial product
	res	r10,$15
	ldl	rr12,workarea+8
	multl	rq8,rr12
	ld	r12,r11
	resflg	c
	rrc	r8
	rrc	r9
	rrc	r10
	rrc	r11
	and	r12,$0x8000
	add	r12,workarea+16
	ld	workarea+16,r12
	adc	r1,r11
	adc	r7,r10
	adc	r6,r9
	ld	r5,r8

	ldl	rr10,workarea	//third partial product
	ldl	rr12,workarea+12
	res	r12,$15
	multl	rq8,rr12
	ld	r12,r11
	resflg	c
	rrc	r8
	rrc	r9
	rrc	r10
	rrc	r11
	and	r12,$0x8000
	add	r12,workarea+16
	ld	workarea+16,r12
	adc	r1,r11
	adc	r7,r10
	adc	r6,r9
	adc	r5,r8
	jr	nc,md40
	inc	r4
md40:
	ldl	rr10,workarea	//fourth partial product
	ldl	rr12,workarea+8
	multl	rq8,rr12
	sub	r12,r12
	resflg	c
	rrc	r9
	rrc	r10
	rrc	r11
	rrc	r12
	rrc	r9
	rrc	r10
	rrc	r11
	rrc	r12
	add	r7,r12
	adc	r6,r11
	adc	r5,r10
	adc	r4,r9

	testl	workarea+16	//make sure sticky bit will get set correctly
	jr	z,md50
	set	r1,$0

//set up to branch to normalize routine
md50:	ldl	rr10,rr4	//load mantissa
	ldl	rr12,rr6
	ldb	rh4,rh3		//load sign
	sub	r7,r7		//r7 must be cleared to jump into the middle of the normalize routine
	jp	norm_db

.page
/****************************************
 * single floating point multiplication *
 ****************************************/
/*
	INPUTS:	source operand	(r8,r9)
		dest operand	(r12,r13)
	Both operands are single floating point numbers

	Register Allocations:
	(r5)	exponent of result, right justified, still biased
	(rh4)	sign of result
	(r10)	sticky register
*/
mult_fl:
	testl	rr8		//if either number is zero return a zero
	jr	z,mf10
	testl	rr12
	jr	nz,mf20
mf10:	clr	w0
	clr	w1
	jp	done

mf20:	ld	r6,r8		//calculate the exponent of the result (r5)
	and	r6,$expfl
	ld	r5,r12
	and	r5,$expfl
	add	r5,r6
	srl	r5,$man1fl
	sub	r5,$xcesfl-1

	ld	r4,r12		//calculate the sign of the result (rh4)
	xor	r4,r8
	and	r4,$signbit

	and	r12,$manfl	//extract mantissas
	set	r12,$imp1fl
	and	r8,$manfl
	set	r8,$imp1fl

	ldl	rr10,rr8
	multl	rq8,rr12

	or	r11,r11		//set sticky word
	jr	z,mf25
	set	r10,$0
mf25:	ld	r6,r10
	and	r6,$stkmask
	jr	z,mf30
	set	r10,$0

mf30:	ld	r12,r9		//adjust mantissa to minimize normalize effort
	ld	r13,r10
	srll	rr12,$man1fl+1
	ld	r2,r10
	ldb	rh2,rl2
	subb	rl2,rl2
	ld	r11,r2
//set up to branch to normalize routine
	ld	r10,r5		//load exponent
	sub	r7,r7		//r7 must be cleared to jump into the middle of the normalize routine
	jp	norm_fl_mult

.page
/**********************************
 * double floating point division *
 **********************************/
/*
	INPUTS:	source operand	(r6,r7,r8,r9)
		dest operand	(r10,r11,r12,r13)
	Both operands are double floating point numbers

	Register Allocations:
	(r2)	exponent of result, right justified, still biased
	(rh3)	sign of result
	the mantissa of the result is accumulated in (workarea)
	(r1)&(r3)   are used as counters for the divide algorithm
	(r5)	is a pointer to the words of the accumulator
	(r1)	is then used as the sticky register
*/
div_db:
	testl	rr10		//check if divisor is zero
	jr	nz,dd10
	testl	rr12
	jr	nz,dd10
	ldb	error_code,$divzero	//if divisor is zero set error code and
	xor	r10,r6			// return + or - infinity
	and	r10,$0xffff
	ld	w0,r10
	ld	w1,$0xffff
	ld	w2,$0xffff
	ld	w3,$0xffff
	jp	done

dd10:	testl	rr6		//if dividend is zero return zero
	jr	nz,dd20
	testl	rr8
	jr	nz,dd20
	clr	w0
	clr	w1
	clr	w2
	clr	w3
	jp	done

dd20:	ld	r2,r6		//calculate exponent of result (r2)
	and	r2,$expdb
	ld	r5,r10
	and	r5,$expdb
	sub	r2,r5
	sra	r2,$man1db
	add	r2,$xcesdb-(16-man1db)+1

	ld	r4,r6		//calculate sign of result (rh4)
	xor	r4,r10
	and	r4,$signbit

	and	r6,$mandb	//extract mantissas
	set	r6,$imp1db
	and	r10,$mandb
	set	r10,$imp1db

	clr	workarea	//workarea will hold result
	clr	workarea+2
	clr	workarea+4
	clr	workarea+6

	sub	r1,r1		//set up to start the subtract and shift algorithm
	ld	r5,$workarea

dd30:	sub	r3,r3
dd40:	cp	r3,$16
	jr	ge,dd60
	cp	r1,$mbitsdb+5
	jr	ge,dd70
	ld	r0,*r5
	sll	r0
	ld	*r5,r0
	cpl	rr6,rr10	//test if dividend >= divisor
	jr	lt,dd50
	jr	ne,dd45
	cpl	rr8,rr12
	jr	ult,dd50

dd45:	subl	rr8,rr12	//subtract divisor from dividend
	sbc	r7,r11
	sbc	r6,r10
	inc	*r5

	testl	rr6		//see if division is done (no remainder)
	jr	nz,dd50
	testl	rr10
	jr	nz,dd50
	ld	r0,*r5		//shift last word into place
	neg	r3
	inc	r3,$(16-1)
	sdl	r0,r3
	ld	*r5,r0
	jr	dd80

dd50:	slll	rr8		//shift dividend over one bit
	rlc	r7
	rlc	r6
	inc	r1
	inc	r3
	jr	dd40

dd60:	inc	r5,$2		//move pointer to next word of result in workarea

	cp	r1,$mbitsdb+5	//see if division is done
	jr	lt,dd30
dd70:	ld	r0,*r5		//shift last word into place
	neg	r3
	inc	r3,$16
	sdl	r0,r3
	ld	*r5,r0	
//set up to branch to normalize routine
dd80:	ld	r1,r9		//set sticky word
	or	r1,r8
	or	r1,r7
	or	r1,r6
	ldl	rr10,workarea	//load mantissa
	ldl	rr12,workarea+4
	jp	norm_db

.page
/**********************************
 * single floating point division *
 **********************************/
/*
	INPUTS:	source operand	(r8,r9)
		dest operand	(r12,r13)
	Both operands are single floating point numbers

	Register Allocations:
	(r5)	exponent of result, right justified, still biased
	(rh4)	sign of result
	(r10,r11)   mantissa of result
	(r11)	is then used as the sticky register
*/
div_fl:
	testl	rr12		//check if divisor is zero
	jr	nz,df10
	ldb	error_code,$divzero	//if divisor is zero set error_code and
	xor	r12,r8			// return + or - infinity
	and	r12,$0xffff
	ld	w0,r12
	ld	w1,$0xffff
	jp	done
df10:	testl	rr8		//if dividend is zero return a zero
	jr	nz,df20
	clr	w0
	clr	w1
	jp	done

df20:	ld	r5,r8		//calculate exponent of result (r5)
	and	r5,$expfl
	ld	r6,r12
	and	r6,$expfl
	sub	r5,r6
	sra	r5,$man1fl
	add	r5,$xcesfl-6

	ld	r4,r12		//calculate sign of result (rh4)
	xor	r4,r8
	and	r4,$signbit
	
	and	r12,$manfl	//extract mantissas
	set	r12,$imp1fl
	and	r8,$manfl
	set	r8,$imp1fl

	sub	r10,r10		//set up to do division
	sub	r11,r11
	resflg	c

	ld	r7,$3
df25:	rrc	r8
	rrc	r9
	rrc	r10
	djnz	r7,df25

	divl	rq8,rr12

//set up to branch to normalize routine
df30:	ldl	rr12,rr10	//load mantissa
	ld	r10,r5		//load exponent
	or	r8,r9		//set sticky bit
	ld	r11,r8
	and	r8,$stkmask
	jr	z,df40
	or	r11,$stkset
df40:	jp	norm_fl_div

.page
/**********************************
 * double floating point addition *
 **********************************/
/*
	INPUTS:	source operand	(r6,r7,r8,r9)
		dest operand	(r10,r11,r12,r13)
	Both operands are double floating point numbers

	Register Allocations:

	Exponents, NOT right justified, still biased
	(r2)	dest exponent, then result exponent
	(r0)	src exponent

	(r5)	the exponent difference
	(r1)	the absolute value of the exponent difference

	Signs
	(rh4)	dest sign, then result sign
	(rh3)	src sign

	(rl4)	same sign flag (same=0)

	(r1)	is then used as the sticky register
	(r0)	is then used as the low word of mantissa when it is
		shifted right
	(r10,r11,r12,r13)   is the mantissa of the result
*/
add_db:
	testl	rr8		//if src is zero return dest
	jr	nz,ad10
	testl	rr6
	jp	z,exit_db
ad10:	testl	rr12		//if dest is zero return src
	jr	nz,ad20
	testl	rr10
	jr	nz,ad20

add_db_zp:
	ldl	w0,rr6
	ldl	w2,rr8
	jp	done

ad20:	ldl	rr4,rr6		//if the same except sign return zero
	xor	r4,$signbit
	cpl	rr4,rr10
	jr	nz,ad30
	cpl	rr12,rr8
	jr	nz,ad30
	clr	w0
	clr	w1
	clr	w2
	clr	w3
	jp	done

ad30:	and	r4,$expdb	//find difference in exponents (r5)
	ld	r0,r4
	ld	r5,r10
	and	r5,$expdb
	ld	r2,r5		//save dest exponent (r2)
	sub	r5,r4
	sra	r5,$man1db

	ld	r1,r5		//if the difference is greater than the number
	jr	pl,ad40		// of bit of precision in the mantissa return 
	neg	r1		// the larger number
ad40:	cp	r1,$mbitsdb+1
	jr	le,ad60
	or	r5,r5
	jr	mi,ad50
	jp	exit_db		//return dest
ad50:	ldl	w0,rr6		//return src
	ldl	w2,rr8
	jp	done

ad60:	ld	r3,r6		//extract signs
	and	r3,$signbit
	ld	r4,r10
	and	r4,$signbit

	cp	r3,r4		//set same sign flag (rl4)
	jr	eq,ad70		// Note if the signs are the same rl4 is already zero
	setb	rl4,$0

ad70:	and	r6,$mandb	//extract mantissas
	set	r6,$imp1db
	and	r10,$mandb
	set	r10,$imp1db

	or	r5,r5		//set result exponent (r2) and prepare to shift
	jr	pl,ad80
	ld	r2,r0		//result exponent (r2) is src exponent (r0)
	ld	r5,r1
	sub	r1,r1		//clear sticky word (r1)
	jr	ad100
ad80:	clr	r1		//clear sticky word (r1)
	jr	z,ad175
	ex	r6,r10		//swap mantissas
	ex	r7,r11
	ex	r8,r12
	ex	r9,r13
	exb	rh3,rh4		//swap signs

//full word shifts
ad100:	sub	r0,r0		//clear extra word in full word shifts
	sll	r5,$4		//calculate the number of full word shifts
	srlb	rl5,$4
	cpb	rl5,$8
	jr	le,ad110
	incb	rh5
ad110:	orb	rh5,rh5
	jr	z,ad140
	cpb	rh5,$1
	jr	ne,ad120
	ld	r1,r13		//one full word shift
	ld	r13,r12
	ld	r12,r11
	ld	r11,r10
	sub	r10,r10
	jr	ad140
ad120:	cpb	rh5,$2
	jr	ne,ad130
	ld	r0,r13		//two full word shifts
	ld	r1,r12
	ld	r13,r11
	ld	r12,r10
	sub	r11,r11
	sub	r10,r10
	jr	ad140
ad130:	ld	r0,r13		//three full word shifts
	or	r0,r12
	ld	r1,r11
	ld	r13,r10
	sub	r12,r12
	sub	r11,r11
	sub	r10,r10
//final shifts
ad140:	subb	rh5,rh5
	cp	r5,$8
	jr	le,ad160
	neg	r5		//final left shifts
	inc	r5,$16
	jr	z,ad170
ad150:	resflg	c
	rlc	r1
	rlc	r13
	rlc	r12
	rlc	r11
	rlc	r10
	djnz	r5,ad150
	jr	ad170

ad160:	or	r5,r5		//final right shifts
	jr	z,ad170
ad165:	resflg	c
	rrc	r10
	rrc	r11
	rrc	r12
	rrc	r13
	rrc	r1
	djnz	r5,ad165
ad170:	or	r0,r0		//if any ones were shifted out make sure the
	jr	z,ad175		// sticky bit gets set
	set	r1,$0
ad175:	orb	rl4,rl4		//see whether to add or subtract
	jr	nz,ad180
	addl	rr12,rr8	//perform addition
	adc	r11,r7
	adc	r10,r6
	jr	ad210

ad180:	cpl	rr10,rr6	//perform subtraction
	jr	eq,ad190
	jr	gt,ad200
	jr	ad195
ad190:	cpl	rr12,rr8
	jr	uge,ad200
ad195:	ldb	rh4,rh3
	ex	r6,r10
	ex	r7,r11
	ex	r8,r12
	ex	r9,r13
ad200:	neg	r1
	sbc	r13,r9
	sbc	r12,r8
	sbc	r11,r7
	sbc	r10,r6

ad210:	sra	r2,$man1db	//unshift result exponent
.page
/***************************************
 * double floating point normalization *
 ***************************************/
/*
	INPUTS:
	(r10,r11,r12,r13)  mantissa
	(r2)	exponent, right justified, still biased
	(r1)	sticky register
	(rh4)	sign

	Register Allocations:
	(r7)	is used as the high word of the mantissa when shifting right

	OUTPUT:
	(r10,r11,r12,r13) double floating point number
*/
norm_db:
	ld	r8,r1		//set sticky bit
	and	r8,$stkmask
	jr	z,nd10
	or	r1,$stkset
nd10:	or	r10,r10		//full word shifts and exponent adjustment
	jr	nz,nd40
	or	r11,r11
	jr	nz,nd30
	or	r12,r12
	jr	nz,nd20

	ld	r10,r13		//three full word shifts
	ld	r11,r1
	and	r11,$stkclr
	sub	r12,r12
	sub	r13,r13
	and	r1,$stkmask
	sub	r2,$48
	jr	nd40

nd20:	ld	r10,r12		//two full word shifts
	ld	r11,r13
	ld	r12,r1
	and	r12,$stkclr
	sub	r13,r13
	and	r1,$stkmask
	sub	r2,$32
	jr	nd40

nd30:	ld	r10,r11		//one full word shift
	ld	r11,r12
	ld	r12,r13
	ld	r13,r1
	and	r13,$stkclr
	and	r1,$stkmask
	dec	r2,$16

//final shifts and exponent adjustment
nd40:	sub	r7,r7		//clear extra word for final shifts (r7)

	ld	r6,r10		//select optimal shift direction
	and	r6,$d2shdb
	jr	nz,nd70
	ld	r6,r10
	and	r6,$d1shdb
	jr	z,nd60

nd50:	ld	r6,r10		//final right shifts
	and	r6,$rrmaskdb
	jr	z,nd90
	resflg	c
	rrc	r10
	rrc	r11
	rrc	r12
	rrc	r13
	rrc	r1
	inc	r2
	jr	nd50

norm_db_mult:
nd60:	ld	r6,r10		//final left shifts not using extra word (r7)
	and	r6,$rlmaskdb
	jr	nz,nd90
	resflg	c
	rlc	r1
	rlc	r13
	rlc	r12
	rlc	r11
	rlc	r10
	dec	r2
	jr	nd60

nd70:	inc	r2,$16		//final left shifts using extra word (r7)
nd80:	ld	r6,r7
	and	r6,$rlmaskdb
	jr	nz,nd90
	resflg	c
	rlc	r1
	rlc	r13
	rlc	r12
	rlc	r11
	rlc	r10
	rlc	r7
	dec	r2
	jr	nd80

nd90:	or	r7,r7		//see if leading bit of mantissa is in (r7)
	jr	nz,nd120

	ld	r8,r1		//rounding mantissa in r10,r11,r12,r13
	and	r8,$stkmask	//set sticky bit
	jr	z,nd100
	and	r1,$stkclr
	set	r1,$stkbit
nd100:	sub	r0,r0
	add	r1,$guardbit
	adc	r13,r0
	adc	r12,r0
	adc	r11,r0
	adc	r10,r0
	test	r1
	jr	nz,nd110
	res	r13,$0
nd110:	bit	r10,$(imp1db+1)	//see if rounding produced a needed extra shift
	jr	z,nd160		//Note for this to happen the mantissa is 10.000...0
	sub	r10,r10
	inc	r2
	jr	nd160

nd120:	ld	r8,r13		//round mantissa in r7,r10,r11,r12
	and	r8,$stkmask	//set sticky bit
	jr	nz,nd130
	or	r1,r1
	jr	z,nd140
nd130:	and	r13,$stkclr
	set	r13,$stkbit
nd140:	sub	r0,r0
	add	r13,$guardbit
	adc	r12,r0
	adc	r11,r0
	adc	r10,r0
	adc	r7,r0
	or	r13,r13
	jr	nz,nd150
	res	r12,$0
nd150:	bit	r7,$(imp1db+1)	//see if rounding produced an extra needed shift
	jr	z,nd160		//Note for this to happen the mantissa is 10.000...0
	sub	r7,r7
	inc	r2

nd160:	cp	r2,$(minxdb+xcesdb)	//check for underflow
	jr	ge,nd170
	ld	r2,$(minxdb+xcesdb)
	ld	error_code,$unflow
	jr	nd180

nd170:	cp	r2,$(maxxdb+xcesdb)	//check for overflow
	jr	le,nd180
	ld	r2,$(maxxdb+xcesdb)
	ld	error_code,$ovflow
nd180:	sll	r2,$man1db		//put together result
	subb	rl4,rl4
	or	r7,r7
	jr	z,nd190
	ld	r13,r12
	ld	r12,r11
	ld	r11,r10
	ld	r10,r7
nd190:	res	r10,$imp1db
	or	r10,r2
	or	r10,r4
	jp	exit_db

.page
/**********************************
 * single floating point addition *
 **********************************/
/*
	INPUTS:	source operand	(r8,r9)
		dest operand	(r12,r13)
	Both operands are single floating point numbers

	Register Allocations:

	Exponents, NOT right justified, still biased
	(r10)	src exponent, then result exponent
	(r6)	dest exponent

	(r5)	exponent difference
	(r1)	absolute value of exponent difference

	Signs
	(rh4)	src sign, then result sign
	(rh3)	dest sign

	(rl4)	same sign flag (same=0)
	(r11)	sticky register
*/
add_fl:
	testl	rr8		//if src is zero return dest
	jp	z,exit_fl
	testl	rr12		//if dest is zero return src
	jr	nz,af10
add_fl_zp:
	ldl	w0,rr8
	jp	done
af10:	ldl	rr6,rr8		//if the same except sign return zero
	xor	r6,$signbit
	cpl	rr6,rr12
	jr	nz,af20
	clr	w0
	clr	w1
	jp	done
af20:	and	r6,$expfl	//find difference in exponents (r5)
	ld	r5,r12
	and	r5,$expfl
	ld	r10,r5		//save dest exponent
	sub	r5,r6
	sra	r5,$man1fl
	ld	r1,r5		//if the difference is greater than the number
				//of bits of precision in the mantissa return
				//the larger number
	jr	pl,af30
	neg	r1
af30:	cp	r1,$mbitsfl+1
	jr	le,af50
	or	r5,r5
	jr	mi,af40
	jp	exit_fl		//return dest
af40:	ldl	w0,rr8		//return src
	jp	done
af50:	ld	r3,r8		//extract signs
	and	r3,$signbit	//sign of src in rh3
	ld	r4,r12
	and	r4,$signbit	//sign of dest in rh4
	cp	r3,r4		//set same sign flag (rl4)
	jr	z,af70		//signs the same note rl4 is zero
	setb	rl4,$0		//signs not the same
af70:	and	r12,$manfl	//extract mantissas
	set	r12,$imp1fl
	and	r8,$manfl
	set	r8,$imp1fl
	sub	r11,r11		//clear sticky register
	or	r5,r5		//set result exponent (r10) and prepare to shift
	jr	pl,af80
	ld	r10,r6		//result exponent is src exponent
	jr	z,af140		//exponent dif is zero no shifting is needed
	ld	r5,r1		//absolute value of exponent difference (r5)
	jr	af100
af80:				//note result exponent (r10) is exp of dest
	ex	r8,r12		//swap mantissas
	ex	r9,r13
	exb	rh3,rh4		//swap signs

af100:	cp	r5,$8
	jr	le,af130
	ld	r11,r13		//full word shift
	ld	r13,r12
	sub	r12,r12
	cp	r5,$16
	jr	z,af140		//no more shifting is needed
	jr	lt,af110
	dec	r5,$16
af110:	cp	r5,$8
	jr	le,af130
	neg	r5
	inc	r5,$16
	or	r5,r5		//see if shift is needed
	jr	z,af140
af120:	resflg	c		//perform final left shifts (clear the carry flag)
	rlc	r11
	rlc	r13
	rlc	r12
	djnz	r5,af120
	jr	af140
af130:	or	r5,r5		//see if shift is needed
	jr	z,af140
	resflg	c		//perform final right shift (clear the carry flag)
	rrc	r12
	rrc	r13
	rrc	r11
	djnz	r5,af130
af140:	orb	rl4,rl4		//see whether to add or subtract
	jr	nz,af150
	addl	rr12,rr8	//perform the addition
	jr	af170
af150:	cpl	rr12,rr8	//perform the subtraction
	jr	ge,af160
	ldb	rh4,rh3		//result sign is sign of src
	ex	r8,r12
	ex	r9,r13
af160:	subl	rr12,rr8
	neg	r11
	jr	nc,af170
	subl	rr12,$1
af170:	sra	r10,$man1fl	//unshift result exponent
.page
/***************************************
 * single floating point normalization *
 ***************************************/
/*
	INPUTS:
	(r12,r13)   mantissa
	(r10)	exponent, right justifed, still biased
	(rh4)	sign
	(r11)	sticky register

	Register Allocations:
	(r7)	is used as the high word of the mantissa when shifting left

	OUTPUT:
	(r12,r13)   single floating point number
*/
norm_fl:
	ld	r8,r11		//set sticky bit (r11)
	and	r8,$stkmask
	jr	z,nf10
	or	r11,$stkset

nf10:	or	r12,r12		//full word shift and exponent adjustment
	jr	nz,nf20
	ld	r12,r13
	ld	r13,r11
	and	r13,$stkclr
	and	r11,$stkmask
	dec	r10,$16

nf20:	sub	r7,r7		//(r7) is used as the high word of the mantissa
				// when shifting left
	ld	r6,r12		//select the optimal shift direction
	and	r6,$d2shfl
	jr	nz,nf50
	ld	r6,r12
	and	r6,$d1shfl
	jr	z,nf40

norm_fl_div:
nf30:	ld	r6,r12		//shift mantissa right
	and	r6,$rrmaskfl
	jr	z,nf60		//done shifting
	resflg	c
	rrc	r12
	rrc	r13
	rrc	r11
	inc	r10		//increment exponent
	jr	nf30

norm_fl_mult:
nf40:	ld	r6,r12		//shift mantissa left not using tmp (r7)
	and	r6,$rlmaskfl
	jr	nz,nf60		//done shifting
	resflg	c
	rlc	r11
	rlc	r13
	rlc	r12
	dec	r10		//decrement exponent
	jr	nf40

nf50:	inc	r10,$16
nf55:	ld	r6,r7		//shift mantissa left using tmp (r7)
	and	r6,$rlmaskfl
	jr	nz,nf60		//done shifting
	resflg	c
	rlc	r11
	rlc	r13
	rlc	r12
	rlc	r7
	dec	r10		//decrement exponent
	jr	nf55

nf60:	or	r7,r7		//see if leading bit of mantissa is in (r7)
	jr	nz,nf90

//mantissa is in (r12,r13,r11)
	ld	r8,r11		//set sticky bit
	and	r8,$stkmask
	jr	z,nf70
	and	r11,$stkclr
	set	r11,$stkbit

nf70:	sub	r0,r0		//round result
	add	r11,$guardbit
	adc	r13,r0
	adc	r12,r0
	or	r11,r11
	jr	nz,nf80
	res	r13,$0

nf80:	bit	r12,$(imp1fl+1)	//see if rounding produced a needed extra shift
	jr	z,nf130		//Note for this to happen the mantissa is 10.000...0
	sub	r12,r12
	inc	r10
	jr	nf130

//mantissa is in (r7,r12,r13)
nf90:	ld	r8,r13		//set sticky bit
	and	r8,$stkmask
	jr	nz,nf100
	or	r11,r11
	jr	z,nf110
nf100:	and	r13,$stkclr
	set	r13,$stkbit

nf110:	sub	r0,r0		//round result
	add	r13,$guardbit
	adc	r12,r0
	adc	r7,r0
	or	r13,r13
	jr	nz,nf120
	res	r13,$0

nf120:	bit	r7,$(imp1fl+1)	//see if rounding produced a needed extra shift
	jr	z,nf130		//Note for this to happen the mantissa is 10.000...0
	sub	r7,r7
	inc	r10

nf130:	cp	r10,$(minxfl+xcesfl)	//check for underflow
	jr	ge,nf140
	ld	r10,$(minxfl+xcesfl)
	ld	error_code,$unflow
	jr	nf150
nf140:	cp	r10,$(maxxfl+xcesfl)	//check for overflow
	jr	le,nf150
	ld	r10,$(maxxfl+xcesfl)
	ld	error_code,$ovflow

nf150:	sll	r10,$man1fl		//put together result
	subb	rl4,rl4
	or	r7,r7
	jr	z,nf160
	ld	r13,r12
	ld	r12,r7
nf160:	res	r12,$imp1fl
	or	r12,r10
	or	r12,r4

.page
exit_fl:
	ldl	w0,rr12
	jr	done

exit_db:
	ldl	w0,rr10
	ldl	w2,rr12

done:	ld	r3,$savereg
	ldm	r0,*r3,$15	//restore reg0-14

	ld	r7,$ret_val	//pointer to return values
	ret
.bss
ret_val:
error_code:	. = . +1
op_code:	. = . +1
w0:		. = . +2
w1:		. = . +2
w2:		. = . +2
w3:		. = . +2
savereg:	. = . +30
workarea:	. = . +20
