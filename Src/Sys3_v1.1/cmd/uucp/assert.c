	/*  assert.c 3.5  1/5/80  16:35:55  */
#include "uucp.h"
#include <time.h>
#include <sys/types.h>

/*******
 *	assert - print out assetion error
 *
 *	return code - none
 */

assert(s1, s2, i1)
char *s1, *s2;
{
	FILE *errlog;
	struct tm *tp;
	extern struct tm *localtime();
	time_t clock;
	int pid;

	if (Debug)
		errlog = stderr;
	else
		errlog = fopen(ERRLOG, "a");
	if (errlog == NULL)
		return;

	pid = getpid();
	fprintf(errlog, "ASSERT ERROR (%.9s)  ", Progname);
	fprintf(errlog, "pid: %d  ", pid);
	time(&clock);
	tp = localtime(&clock);
	fprintf(errlog, "(%d/%d-%d:%2.2d) ", tp->tm_mon + 1,
		tp->tm_mday, tp->tm_hour, tp->tm_min);
	fprintf(errlog, "%s %s (%d)\n", s1, s2, i1);
	fclose(errlog);
	return;
}
