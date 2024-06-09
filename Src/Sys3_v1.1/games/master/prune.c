/*
 *	Unmarks all configurations that do not match "e" in their
 *	ematch and "i" in their imatch. Matching is against "x".
 */

#include "max.h"

prune (e, i, x)
{
	int t[MAXSLOTS], t2[MAXSLOTS];
	register int conf;

	unpack (x, t);
	conf = -1;
	while ((conf = next (conf)) >= 0) {
		if (ematch (x, conf) != e)
			unmark (conf);
		else {
			unpack (conf, t2);
			if (imatch (t, t2) != i)
				unmark (conf);
		}
	}
}
