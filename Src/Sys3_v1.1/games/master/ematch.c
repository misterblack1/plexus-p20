/*
 *	Exact match between a and b; counts the number of octal
 *	digits in a that exactly match the corresponding octal
 *	digits in b. Only the rightmost "slots" digits are
 *	compared.
 */
ematch (a, b)
	register int a, b;
{
	return zcount (a ^ b);
}
