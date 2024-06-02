/*
 *	namtouid converts login names to uids
 *	maintains ulist for speed only
 */
#include "acctdef.h"
#include <stdio.h>
#include <pwd.h>
static	usize;
static	struct ulist {
	char	uname[NSZ];
	uid_t	uuid;
} ul[USIZE];
char	ntmp[NSZ+1];

uid_t
namtouid(name)
char	name[NSZ];
{
	register struct ulist *up;
	register uid_t tuid;
	struct passwd *getpwnam();
	register struct passwd *pp;

	for (up = ul; up < &ul[usize]; up++)
		if (strncmp(name, up->uname, NSZ) == 0)
			return(up->uuid);
	strncpy(ntmp, name, NSZ);
	setpwent();
	if ((pp = getpwnam(ntmp)) == NULL)
		tuid = -1;
	else {
		tuid = pp->pw_uid;
		if (usize < USIZE) {
			CPYN(up->uname, name);
			up->uuid = tuid;
			usize++;
		}
	}
	return(tuid);
}
