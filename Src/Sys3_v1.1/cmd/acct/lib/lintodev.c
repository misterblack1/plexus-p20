/*
 *	convert linename to device
 *	return -1 if nonexistent or not character device
 */
#include "acctdef.h"
#include <sys/types.h>
#include <sys/stat.h>
static	char	devtty[5+LSZ+1]	= "/dev/xxxxxxxx";

dev_t
lintodev(linename)
char linename[LSZ];
{
	struct stat sb;
	strncpy(&devtty[5], linename, LSZ);
	if (stat(devtty, &sb) != -1 && (sb.st_mode&S_IFMT) == S_IFCHR)
		return((dev_t)sb.st_rdev);
	return((dev_t)-1);
}
