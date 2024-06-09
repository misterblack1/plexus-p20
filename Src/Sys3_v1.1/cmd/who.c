/*
 * who
 */

#include <stdio.h>
#include <utmp.h>
#include <pwd.h>
struct utmp utmp;
struct passwd *pw;
struct passwd *getpwuid();

char *ttyname(), *strrchr(), *ctime(), *strcpy();
main(argc, argv)
char **argv;
{
	register char *tp, *s;
	register FILE *fi;
	int i;
	static char stdbuf[BUFSIZ];

	setbuf(stdout,stdbuf);
	s = "/etc/utmp";
	if(argc == 2)
		s = argv[1];
	if (argc>2) {
		for(i=0;i<=2;i++)
			if((tp=ttyname(i)) != NULL) break;
		if (tp)
			tp = strrchr(tp, '/') + 1;
		else {	/* no tty - use best guess from passwd file */
			pw = getpwuid(getuid());
			strcpy(utmp.ut_name, pw?pw->pw_name: "?");
			strcpy(utmp.ut_line, "tty??");
			time(&utmp.ut_time);
			putline();
			exit(0);
		}
	}
	if ((fi = fopen(s, "r")) == NULL) {
		fprintf(stderr, "who: cannot open %s\n", s);
		exit(2);
	}
	while (fread((char *)&utmp, sizeof(utmp), 1, fi) == 1) {
		if(argc>2) {
			if (strcmp(utmp.ut_line, tp))
				continue;
#ifdef vax
/*			printf("(Vax) ");*/
#endif
			putline();
			exit(0);
		}
		if(utmp.ut_name[0] == '\0' && argc==1)
			continue;
		putline();
	}
}

putline()
{
	register char *cbuf;

	printf("%-8.8s %-8.8s", utmp.ut_name, utmp.ut_line);
	cbuf = ctime(&utmp.ut_time);
	printf("%.12s\n", cbuf+4);
}
