/*
 * newgrp [group]
 *
 * rules
 *	if no arg, group id in password file is used
 *	else if group id == id in password file
 *	else if login name is in member list
 *	else if password is present and user knows it
 *	else too bad
 */
#include <stdio.h>
#include <pwd.h>
#include <grp.h>

#define	SHELL	"/bin/sh"

char	PW[] = "Password: ";
char	NG[] = "Sorry";
char	PD[] = "Permission denied";
char	UG[] = "Unknown group";
char	NS[] = "You have no shell";

struct	group *getgrnam();
struct	passwd *getpwnam();
char	*cuserid();
char	*getpass();

char	*crypt();


main(argc,argv)
char *argv[];
{
	register char *s;
	register struct passwd *p;
	char *rname();

#ifdef	DEBUG
	chroot(".");
#endif
	if (((s = cuserid(0)) == NULL) || ((p = getpwnam(s)) == NULL))
		error(NG);
	endpwent();
	if (argc > 1)
		p->pw_gid = chkgrp(argv[1], p);
	if (setgid(p->pw_gid) < 0 || setuid(getuid()) < 0)
		error(NG);
	if (!*p->pw_shell)
		p->pw_shell = SHELL;
	execl(p->pw_shell, p->pw_shell, NULL);
	error(NS);
}

warn(s)
char *s;
{
	fprintf(stderr, "%s\n", s);
}

error(s)
char *s;
{
	warn(s);
	exit(1);
}

chkgrp(gname, p)
char	*gname;
struct	passwd *p;
{
	register char **t;
	register struct group *g;

	g = getgrnam(gname);
	endgrent();
	if (g == NULL) {
		warn(UG);
		return getgid();
	}
	if (p->pw_gid == g->gr_gid || getuid() == 0)
		return g->gr_gid;
	for (t = g->gr_mem; *t; ++t)
		if (strcmp(p->pw_name, *t) == 0)
			return g->gr_gid;
	if (*g->gr_passwd) {
		if (!isatty(fileno(stdin)))
			error(PD);
		if (strcmp(g->gr_passwd, crypt(getpass(PW), g->gr_passwd)) == 0)
			return g->gr_gid;
	}
	warn(NG);
	return getgid();
}
/*
 * return pointer to rightmost component of pathname
 */
char *rname(pn)
char *pn;
{
	register char *q;

	q = pn;
	while (*pn)
		if (*pn++ == '/')
			q = pn;
	return q;
}
