	/*  uucpname  3.1  10/26/79  11:44:15  */

#include "uucp.h"

#ifdef UNAME
#include <sys/utsname.h>
#endif
/*******
 *	uucpname(name)		get the uucp name
 *
 *	return code - none
 */

uucpname(name)
char *name;
{
	char *s, *d;

#ifdef UNAME
	struct utsname utsn;

	uname(&utsn);
	s = utsn.nodename;
#endif

#ifndef UNAME
	s = MYNAME;
#endif

	d = name;
	while ((*d = *s++) && d < name + 7)
		d++;
	*(name + 7) = '\0';
	return;
}
