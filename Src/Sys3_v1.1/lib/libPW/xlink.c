static char Sccsid[]="@(#)xlink	3.1";
/*
	Interface to link(II) which handles all error conditions.
	Returns 0 on success,
	fatal() on failure.
*/

# include	"errno.h"

xlink(f1,f2)
{
	extern errno;
	extern char Error[];

	if (link(f1,f2)) {
		if (errno == EEXIST || errno == EXDEV) {
			sprintf(Error,"can't link `%s' to `%s' (%d)",
				f2,f1,errno == EEXIST ? 111 : 112);
			return(fatal(Error));
		}
		if (errno == EACCES)
			f1 = f2;
		return(xmsg(f1,"xlink"));
	}
	return(0);
}
