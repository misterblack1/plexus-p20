static char Sccsid[]="@(#)giveup	3.2";
/*
	Chdir to "/" if argument is 0.
	Set IOT signal to system default.
	Call abort(III).
	(Shouldn't produce a core when called with a 0 argument.)
*/

# include "signal.h"

giveup(dump)
{
	if (!dump)
		chdir("/");
	signal(SIGIOT,0);
	abort();
}
