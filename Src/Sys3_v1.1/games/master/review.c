/*
 *	Prints a review of past moves, along with their scores
 */

#include "max.h"

review()
{
	extern int t1[];
	extern int slots;
	extern char *cnames[];
	int t2[MAXSLOTS];
	int g, e, i, k;
	extern int revcount, revtab[];

	for (k = 0; k < revcount; k++) {
		g = revtab [k];
		unpack (g, t2);
		e = ematch (g, pack (t1));
		i = imatch (t1, t2) - e;
		printf ("%d %d: ", e, i);
		unpack (g, t2);
		for (i = 0; i < slots; i++) {
			printf ("\t%s", cnames[t2[i]]);
		}
		printf("\n");
	}
}
