static char Sccsid[]="@(#)rename	3.1";
# include "errno.h"
# include "fatal.h"

/*
	rename (unlink/link)
	Calls xlink() and xunlink().
*/

rename(oldname,newname)
char *oldname, *newname;
{
	extern int errno;

	if (unlink(newname) < 0 && errno != ENOENT)
		return(xunlink(newname));

	if (xlink(oldname,newname) == Fvalue)
		return(-1);
	return(xunlink(oldname));
}
