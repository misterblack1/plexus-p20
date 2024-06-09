/*LINTLIBRARY*/
/*
 * Binary search algorithm, generalized from Knuth (6.2.1) Algorithm B.
 */

typedef char *POINTER;

POINTER
bsearch(key, base, nel, width, compar)
POINTER	key;			/* Key to be located */
POINTER	base;			/* Beginning of table */
int	nel;			/* Number of elements in the table */
int	width;			/* Width of an element */
int	(*compar)();		/* Comparison function */
{
	POINTER u = base+(nel-1)*width;	/* Last element in table */
	POINTER l = base;	/* First element in table */
	POINTER i;		/* Approximate middle element of table*/
	int	res;		/* Result of comparison */        

	while(u >= l) {
		i = l+width*((nel-1)/2);
		res = (*compar)(key, i);
		if(res == 0)
			return(i);
		else if(res < 0)
			u = i-width;
		else
			l = i+width;
		nel = 1+(u-l)/width;
	}
	return(0);
}
