/*
 *	This program plays the part of "Mastermind" where the
 *	human player has to guess the machine's configuration.
 *	It returns the number of guesses necessary.
 */

#include "max.h"

play2()
{
	extern int t1[];
	int t2[MAXSLOTS];
	extern int slots, colors, revcount, revtab[];
	int config, score, g, e, i;

	/* Select a random configuration */
	for (i = 0; i < slots; i++)
		t1[i] = randint (colors);

	/* Other initialization */
	score = 0;
	config = pack (t1);
	printf ("Your turn to guess my configuration.\n");
	revcount = 0;

	/* The play's the thing */
	do {	g = rguess();
		if (revcount < (REVLEN-1))
			revtab[revcount++] = g;
		unpack (g, t2);
		e = ematch (g, config);
		i = imatch (t1, t2) - e;
		printf ("%d %d\n", e, i);
		score++;
	} while	(e != slots);
	printf ("You guessed my configuration in %d %s.\n",
		score, score == 1 ? "try" : "tries");
	return score;
}
