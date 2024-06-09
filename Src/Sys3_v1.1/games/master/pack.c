/*
 *	Packs an array a into its result, by taking each element of
 *	a and assigning it to a separate octal digit of the result.
 *	The first "slots" elements of a are thus assigned. The order
 *	will effectively be reversed because the elements of b will be
 *	fetched from right to left.
 */
pack (a)
	int a[];
{
	int register i, *p, r;
	extern int slots;

	p = &a[slots];
	r = 0;
	for (i = 0; i < slots; i++)
		r = (r << 3) | (*--p & 07);
	return r;
}
