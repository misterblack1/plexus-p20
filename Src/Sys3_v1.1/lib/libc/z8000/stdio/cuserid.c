/*LINTLIBRARY*/
#include <stdio.h>
#include <pwd.h>

char *cuserid(s)
char *s;
{
	static char res[L_cuserid];
	struct passwd *pw, *getpwuid();
	char *getlogin();
	int getuid();
	register char *p, *r;

	r = s==NULL? res: s;
	p = getlogin();
	if(p != NULL)
		strcpy(r, p);
	else {
		pw = getpwuid(getuid());
		if(pw == NULL) {
			*r = '\0';
			r = NULL;
		} else
			strcpy(r, pw->pw_name);
		endpwent();
	}
	return(r);
}
