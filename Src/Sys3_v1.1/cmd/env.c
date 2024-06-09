/*
 *	env [ - ] [ name=value ]... [command arg...]
 *	set environment, then execute command (or print environment)
 *	- says start fresh, otherwise merge with inherited environment
 */
#include <stdio.h>

#define NENV	100
char	*newenv[NENV];
char	*nullp = NULL;

extern	char **environ;
extern	errno;
extern	char *sys_errlist[];
char	*nvmatch(), *strchr();

main(argc, argv, envp)
register char **argv, **envp;
{

	argc--;
	argv++;
	if (argc && strcmp(*argv, "-") == 0) {
		envp = &nullp;
		argc--;
		argv++;
	}

	for (; *envp != NULL; envp++)
		if (strchr(*envp, '=') != NULL)
			addname(*envp);
	while (*argv != NULL && strchr(*argv, '=') != NULL)
		addname(*argv++);

	if (*argv == NULL)
		print();
	else {
		environ = newenv;
		execvp(*argv, argv);
		fprintf(stderr, "%s: %s\n", sys_errlist[errno], *argv);
		exit(1);
	}
}

addname(arg)
register char *arg;
{
	register char **p;

	for (p = newenv; *p != NULL && p < &newenv[NENV-1]; p++)
		if (nvmatch(arg, *p) != NULL) {
			*p = arg;
			return;
		}
	if (p >= &newenv[NENV-1]) {
		fprintf(stderr, "too many values in environment\n");
		print();
		exit(1);
	}
	*p = arg;
	return;
}

print()
{
	register char **p = newenv;

	while (*p != NULL)
		printf("%s\n", *p++);
}

/*
 *	s1 is either name, or name=value
 *	s2 is name=value
 *	if names match, return value of s2, else NULL
 */

static char *
nvmatch(s1, s2)
register char *s1, *s2;
{

	while (*s1 == *s2++)
		if (*s1++ == '=')
			return(s2);
	if (*s1 == '\0' && *(s2-1) == '=')
		return(s2);
	return(NULL);
}
