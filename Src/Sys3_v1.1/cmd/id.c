#include <stdio.h>
#include <pwd.h>
#include <grp.h>

main()
{
	int uid, gid, euid, egid;
	static char stdbuf[BUFSIZ];

	setbuf (stdout, stdbuf);

	uid = getuid();
	gid = getgid();
	euid = geteuid();
	egid = getegid();

	puid ("uid", uid);
	pgid (" gid", gid);
	if (uid != euid)
		puid (" euid", euid);
	if (gid != egid)
		pgid (" egid", egid);
	putchar ('\n');
}

puid (s, id)
	char *s;
	int id;
{
	struct passwd *pw;
	struct passwd *getpwuid();

	printf ("%s=%d", s, id);
	setpwent();
	pw = getpwuid(id);
	if (pw)
		printf ("(%s)", pw->pw_name);
}

pgid (s, id)
	char *s;
	int id;
{
	struct group *gr;
	struct group *getgrgid();

	printf ("%s=%d", s, id);
	setgrent();
	gr = getgrgid(id);
	if (gr)
		printf ("(%s)", gr->gr_name);
}
