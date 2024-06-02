/*
 *	unpacks an integer a into an array b, by taking each octal
 *	digit, starting from the right of a, and assigning it to
 *	consecutive elements of b. Only the rightmost "slots"
 *	digits are thus affected; note that the order will be
 *	reversed because b is assigned from left to right.
 */
unpack (a, b)
	int a, b[];
{
	register int i, aa, *p;
	extern slots;

	aa = a;
	p = &b[0];
	for (i = 0; i < slots; i++) {
		*p++ = aa & 07;
		aa = aa >> 3;
	}
}
