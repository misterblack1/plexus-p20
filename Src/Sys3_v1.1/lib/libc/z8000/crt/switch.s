/*
 * This code is used by the pcc compiler to implement switch statements.
 * At the entrance to either switch1 or switch2 r7 contains the switch
 * index value and r6 contains the address, in the data area, of the
 * control block for the switch statement. The switch control block is
 * as follows:
 *
 * switch1:
 *
 *	0)	lowest case label value (clow)
 *	1)	highest case label value (chigh)
 *	2)	code address of default case.
 *	3)	(chigh - clow + 1) code addresses for the
 *		cases from clow to chigh in ascending order by
 *		case value.
 *
 * switch2:
 *
 *	0)	number of cases + 1 (n + 1)
 *	1)	n case values.
 *	n+1)	n case code addresses.
 *	2*n+1)	default case code address.
 */

.globl	switch1
.globl	switch2

switch1:
ld	r5, 4(r6)		/* r5 = default code address */
cp	r7, 2(r6)
jp	gt, *r5			/* out of case range on high end */
sub	r7, *r6
jp	lt, *r5			/* out of case range on low end */
add	r6, r7
add	r6, r7			/* r6 = index into case code addresses */
ld	r5, 6(r6)		/* r5 = case code address */
jp	*r5			/* jump to case */

switch2:
ld	r5, *r6			/* r5 = number of cases + 1 */
ld	r4, r5			/* r4 = number of cases + 1 */
inc	r6, $2			/* r6 = ptr to case value list */
cpir	r7, *r6, r5, eq		/* look for correct case value */
add	r6, r4
add	r6, r4			/* r6 = address of correct code address */
ld	r5, -4(r6)		/* r5 = correct case code address */
jp	*r5			/* jump to case */
