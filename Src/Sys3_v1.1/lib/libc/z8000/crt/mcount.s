/*
 * Mcount is used to count calls to procedures that are being monitored.
 * Mcount is called at the beginning of each monitored procedure.
 * The parameter to the routine is in r0 and is the address of a word
 * reserved for the procedure being monitored. This word is either 0
 * or is the address of a 3 word monitor structure for the procedure
 * which is defined as follows:
 *
 *	0)	pc address of the instruction after the mcount call.
 *
 *	1-2)	double word counter for calls to the procedure.
 *
 * If the pointer is non zero the structure pointed to has its counter
 * bumped up by one. If, however, the pointer is zero then a new 
 * structure entry is created by mcount.
 *
 * The address of the next available structure entry is in countbase.
 * If countbase is zero then monitoring is off and mcount returns.
 * Otherwise the value of countbase is moved to the procedures pointer
 * and countbase is modified to point to the next available entry.
 *
 * This routine does not call csav and return with cret since it is 
 * supposed to be fast as possible.
 */

.globl	mcount
.comm	countbase, 2		/* address of next available mon struct */

mcount:
ld	r7, r0			/* at entrance r0 has parm, move to r7 */
ld	r6, *r7			/* r6 = ptr to mon structure */
test	r6
jr	nz, havestruct		/* if (ptr != 0) goto havestruct */
ld	r6, countbase		/* r6 = pointer to new mon structure */
test	r6
ret	z			/* if (countbase == 0) return */
ld	*r7, r6			/* save new mon struct ptr in procedure */
lda	r5, 6(r6)
ld	countbase, r5		/* countbase = next available cell */
ld	r5, *r15		/* get address of inst after mcount call */
ld	*r6, r5			/* store in mon structure */

havestruct:
inc	4(r6), $1		/* increment low order word */
ret	nz			/* return now if no roll over */
inc	2(r6), $1
ret				/* return to procedure */

