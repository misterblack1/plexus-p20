/*
 *	Counts inexact matches between a and b. the arguments
 *	are both arrays, and icount returns the number of elements in
 *	a that match elements anywhere in b, with the proviso that
 *	no element in one array may match more than once with any
 *	element of the other. For example, given the arrays:
 *
 *	3 4 4 2 1    for a
 *	0 5 2 4 3    for b,
 *
 *	icount would return the value 3. This is because the 3 in a
 *	matches the 3 in b, the first 4 in a matches the 4 in b, the
 *	second 4 in a does not match anything, and the 2 in a matches
 *	the 2 in b.
 *
 *	b is destroyed by the matching process; a is left intact.
 *
 *	Only the first "nslots" elements of a and b are considered.
 *
 *	Both arrays are assumed to consist of non-negative elements.
 */
imatch (a, b)
	int a[], b[];
{
	int *ilim, *jlim;
	register int n, *i, *j;
	extern slots;

	ilim = &a[slots];
	jlim = &b[slots];
	n = 0;
	for (i = &a[0]; i < ilim; i++)
		for (j = &b[0]; j<jlim; j++)
			if (*i == *j) {
				n++;
				*j = -1;
				break;
			}
	return n;
}
