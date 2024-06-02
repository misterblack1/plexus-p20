/*
 * this is a test program for the kmc
 *
 * this program converts a null-terminated string of ascii digits
 * to an 8-bit unsigned binary integer
 * the result is accumulated in registers r4 and r5
 * each digit is validated, then the result is multiplied by ten and the
 * decimal value of the digit is added to the result register
 */
.data
/*
 * here is the string to be converted to binary
 */
num:	.byte	'1','9','0','\0'
.text
/*
 * execution starts here
 *
 * set mar = address of first digit
 */
	mov	num,mar
	mov	%num,%mar
/*
 * set result = 0
 */
	mov	0,brg
	mov	brg,r5
	mov	brg,r4
loop:	mov	mem,r0|mar++
	mov	0,brg
	addn	brg,r0,-
	brz	done
	mov	'9'+1,brg
	sub	brg,r0,-
	brc	e1
	mov	'0',brg
	sub	brg,r0
	brc	1f
	br	e2
1:
/*
 * the following code multiplies the result by ten
 */
	asl	r5,r5|brg
	mov	brg,r7
	aslc	r4,r4|brg
	mov	brg,r6
	asl	r5
	aslc	r4
	asl	r5
	aslc	r4
	mov	r7,brg
	add	brg,r5
	mov	r6,brg
	addc	brg,r4
/*
 * now add in the new digit
 */
	mov	r0,brg
	add	brg,r5
	adc	r4
/*
 * end of loop--go look for next digit
 */
	br	loop
/*
 * all done--display the result
 */
done:
	mov	r5,brg
	mov	brg,csr4
	mov	r5,brg
	mov	brg,csr5
/*
 * set error code = 0
 */
	mov	0,brg
	mov	brg,csr3
	br	hang
e1:
	mov	1,brg
	mov	brg,csr3
	br	hang
e2:
	mov	2,brg
	mov	brg,csr3
	br	hang
hang:
	br	hang
