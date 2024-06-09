#include <sys/utsname.h>

/*
 * Return a pointer to a string
 * containing the name of the unix system.
 * Return 0 on error.
 */
char *
sysname()
{
	static struct utsname uinfo;

	if(uname(&uinfo) < 0)
		return(0);
	else
		return(uinfo.sysname);
}
