/*
 *	The following functions manipulate "bittab".
 */

extern int bittab[];
extern int tcount;

/*
 *	turns on the mark bit for "config".
 */
mark (config)
	int config;
{
	bittab[config >> 4] |= 1 << (config & 017);
	tcount++;
}

/*
 *	Turns off the mark bit for "config".
 */
unmark (config)
	int config;
{
	bittab[config >> 4] &= ~(1 << (config & 017));
	tcount--;
}

/*
 *	Returns 1 if "config" is marked, 0 otherwise.
 */
marked (config)
	register int config;
{
	return (bittab[config >> 4] >> (config & 017)) & 1;
}

/*
 *	clears the configuration table
 */
tclear()
{
	register int i;

	tcount = 0;
	for (i = 0; i < 2048; i++)
		bittab [i] = 0;
}
/*
 *	returns the lowest configuration greater than "config" that
 *	is marked. If there are no remaining marked configurations,
 *	the program returns a value of -1. Some calling programs
 *	use "tcount", and therefore do not check this.
 */
next (config)
	register int config;
{
	register int c;

	c = config;
	do {	if (c++ == 077777)
			return -1;
		if ((c & 0177760) == 0)
			while (bittab[c >> 4] == 0) {
				if (c == 077760)
					return -1;
				c += 020;
			}
	} while	(!marked (c));
	return c;
}
