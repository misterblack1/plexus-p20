/*LINTLIBRARY*/
#include <stdio.h>
#include <pwd.h>

/*
 * format a password file entry
 */
putpwent(p, f)
register struct passwd *p;
register FILE *f;
{
	fprintf(f, "%s:%s", p->pw_name, p->pw_passwd);
	if ((*p->pw_age) != '\0')
		fprintf(f, ",%s", p->pw_age);
	fprintf(f, ":%u:%u:%s:%s:%s",
		p->pw_uid, p->pw_gid,
		p->pw_gecos,
		p->pw_dir,
		p->pw_shell
	);
	putc('\n', f);
	return ferror(f);
}
