/* modified 2/23/82 to remove alloca. PAF */
static char Sccsid[]="@(#)xcreat	3.1";
# include	"sys/types.h"
# include	"macros.h"

/*
	"Sensible" creat: write permission in directory is required in
	all cases, and created file is guaranteed to have specified mode
	and be owned by effective user.
	(It does this by first unlinking the file to be created.)
	Returns file descriptor on success,
	fatal() on failure.
*/

xcreat(name,mode)
char *name;
int mode;
{
	register int fd;
	register char *d;
    char temp[512];

	/* d = (char *)alloca(size(name)); */
    d = temp;
	copy(name,d);
	if (!exists(dname(d))) {
		sprintf(Error,"directory `%s' nonexistent (ut1)",d);
		fatal(Error);
	}
	unlink(name);
	if ((fd = creat(name,mode)) >= 0)
		return(fd);
	return(xmsg(name,"xcreat"));
}
