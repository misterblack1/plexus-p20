/*
 *	Generates a combination table for s slots and c colors.
 *	The table will be stored by marking all the configurations
 *	thus generated. Since tclear() will be called first, this
 *	will have the ultimate effect of placing the size in
 *	external tcount.
 *
 *	This program requires c>1.
 */

#include "max.h"

gentab (s, c)
	int s, c;
{
	register int *ii, cm1, *iimax;
	int t[MAXSLOTS+1];

	/* clear the table */
	tclear();
	cm1 = c - 1;
	iimax = &t[s];

	/* Generate the first combination (all zero) */
	for (ii = &t[0]; ii <= iimax; *ii++ = 0)
		;

	/* Generate all combinations in the following loop */
	do {	mark (pack (t));
		/*
		 *	Transform the current combination to the
		 *	next combination in sequence
		 */
		for (ii = &t[0]; *ii == cm1; *ii++ = 0)
			;
		++*ii;
	} while	(ii < iimax);
}
