/*LINTLIBRARY*/
/*
 * Linear search algorithm, generalized from Knuth (6.1) Algorithm Q.
 */

typedef char *POINTER;

POINTER
lsearch(key, base, nelp, width, compar)
POINTER	key;			/* Key to be located */
POINTER	base;			/* Beginning of table */
int	*nelp;			/* Address of table size */
int	width;			/* Width of an element */
int	(*compar)();		/* Comparison function */
{
	POINTER	test = base;	/* Points to table elements */
	POINTER	next = base+(*nelp)*width;	/* End of table */
	POINTER	strncpy();

	strncpy(next, key, width);

	while((*compar)(key, test) != 0)
		test += width;

	if(test == next)
		(*nelp)++;

	return(test);
}
