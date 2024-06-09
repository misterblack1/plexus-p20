/*LINTLIBRARY*/
/*
 * convert long int to base 64 ascii
 * char set is [./0-9A-Za-z]
 */
char *
l64a(l)
long l;
{
	register int i, c;
	register char *s;
	static char buf[7];

	i = 6;
	s = buf;
	while (--i > 0 && l != 0) {
		c = (l & 077) + '.';
		if (c > '9')
			c += 'A'-'9'-1;
		if (c > 'Z')
			c += 'a'-'Z'-1;
		*s++ = c;
		l >>= 6;
	}
	*s = '\0';
	return buf;
}
