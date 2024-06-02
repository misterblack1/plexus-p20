/*
 *	Returns a random integer k such that 0<=k<s
 */
randint(s)
	int s;
{

	return (int) ((long) rand() * s / 32768L);
}
