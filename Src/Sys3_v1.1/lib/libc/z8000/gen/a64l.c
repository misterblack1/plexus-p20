/*LINTLIBRARY*/
/*
 * convert base 64 ascii to long int
 * char set is [./0-9A-Za-z]
 */

long
a64l(s)
register char *s;
{
	register i, c;
	long l;

	i = 0;
	l = 0;
	while ((c = *s++) != '\0') {
		if (c >= 'a')
			c -= 'a'-'Z'-1;
		if (c >= 'A')
			c -= 'A'-'9'-1;
		l |= (long)(c - '.') << i;
		i += 6;
	}
	return l;
}
