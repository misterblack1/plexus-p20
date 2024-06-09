/*
 *	Returns 1 if a points to a string which is an abbreviation for
 *	b, 0 otherwise. "abbreviation" includes the possibility that
 *	the strings may be equal.
 */
abbrev (a, b)
	register char *a, *b;
{
	while (*a == *b && *a != '\0') {
		a++;
		b++;
	}
	return *a == '\0';
}
