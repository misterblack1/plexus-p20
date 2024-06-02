/*
 *	Compares strings a and b for equality; returns 1 if
 *	equal, 0 of not. Both strings are assumed to be terminated
 *	by a null character.
 */
equal(a, b)
	char *a, *b;
{
	while (*a == *b && *a != '\0') {
		a++;
		b++;
	}
	return *a == *b;
}
