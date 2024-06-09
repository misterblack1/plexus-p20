/*
 *	counts the number of zero octal digits in x; only the
 *	rightmost "slots" digits are counted.
 */
zcount(x)
	int x;
{
	register int n, s, a;
	extern int slots;

	a = x;
	s = 0;
	for (n=0; n<slots; n++) {
		if ((a&07) == 0)
			s++;
		a = a >> 3;
	}
	return s;
}
