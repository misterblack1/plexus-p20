/*
 *	tmsecs returns number of seconds from t1 to t2,
 *	times expressed in localtime format.
 *	assumed that t1 <= t2, and are in same day.
 */

#include <time.h>
long
tmsecs(t1, t2)
register struct tm *t1, *t2;
{
	return((t2->tm_sec - t1->tm_sec) +
		60*(t2->tm_min - t1->tm_min) +
		3600L*(t2->tm_hour - t1->tm_hour));
}
