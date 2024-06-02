/*
 * login [ name ] [ hangup ]
 * if CONSOLE is defined, then uid 0 may only login from that device
 * if NOSHELL is defined, then SHELL will not be put in the environment
 * for non-standard login "shells"
 */

#include <sys/types.h>
#include <termio.h>
#include <utmp.h>
#include <signal.h>
#include <pwd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#define SCPYN(a, b)	strncpy(a, b, sizeof(a))
#ifdef PWB
#define CONSOLE	"/dev/console"
#endif

#undef	CONSOLE

/* char	maildir[20] =	"/usr/mail/"; */
struct	passwd nouser = {"", "nope"};
struct	termio ttyb;
struct	utmp utmp;
char	minusnam[16] = "-";
char	homedir[64] = "HOME=";
#ifdef PWB
char	logname[20] = "LOGNAME=";
#endif
char	shell[45] = "SHELL=";
#ifdef PWB
char	*envinit[] = {homedir, "PATH=:/bin:/usr/bin", logname, 0, 0};
#else
char	*envinit[] = {homedir, "PATH=:/bin:/usr/bin", 0, 0};
#endif
struct	passwd *pwd;

struct	passwd *getpwnam();
int	setpwent();
char	*ttyname();
char	*crypt();
char	*getpass();
char	*strrchr();
int	bailout();
extern	char **environ;

#define	WEEK	(24L * 7 * 60 * 60) /* 1 week in seconds */
time_t	when;
time_t	maxweeks;
time_t	minweeks;
time_t	now;
long 	a64l();
long	time();

main(argc, argv)
char **argv;
{
	register char *namep;
	int t, f, c;
	char *ttyn;
	int nopassword;
	struct	stat	sbuf;

	alarm(argc>2?atoi(argv[2]):60);
	signal(SIGQUIT, 1);
	signal(SIGINT, 1);
	signal(SIGALRM, bailout);
	nice (-nice(0));
	ttyn = ttyname(0);
	if (ttyn==0)
		ttyn = "/dev/tty??";

    loop:
	SCPYN(utmp.ut_name, "");
	if (argc>1) {
		SCPYN(utmp.ut_name, argv[1]);
		argc = 0;
	}
	while (utmp.ut_name[0] == '\0') {
		namep = utmp.ut_name;
		printf("login: ");
		while ((c = getchar()) != '\n') {
			if(c == ' ')
				c = '_';
			if (c == EOF)
				exit(0);
			if (namep < utmp.ut_name+8)
				*namep++ = c;
		}
	}
	setpwent();
	if ((pwd = getpwnam(utmp.ut_name)) == NULL)
		pwd = &nouser;
	endpwent();
	nopassword = 1;
	if (*pwd->pw_passwd != '\0') {
		nopassword = 0;
		namep = crypt(getpass("Password:"),pwd->pw_passwd);
		if (strcmp(namep, pwd->pw_passwd)) {
			printf("Login incorrect\n");
			goto loop;
		}
	}
	time(&utmp.ut_time);
	if ((f = open("/etc/utmp", 2)) >= 0) {
		strcpy(utmp.ut_line, strrchr(ttyn, '/')+1);
		if(utscan(f, utmp.ut_line)==0)
		{
			printf("No entry in utmp\n");
			exit(5);
		}
		write(f, (char *)&utmp, sizeof(utmp));
		close(f);
	}
	if ((f = open("/usr/adm/wtmp", O_WRONLY|O_APPEND)) >= 0) {
		write(f, (char *)&utmp, sizeof(utmp));
		close(f);
	}
	chown(ttyn, pwd->pw_uid, pwd->pw_gid);
	if(*pwd->pw_shell == '*')
	{
		if(chroot(pwd->pw_dir) < 0)
		{
			printf("No Root Directory\n");
			goto loop;
		}
		printf("Subsystem root: %s\n",pwd->pw_dir);
		execl("/etc/login", "login", 0);
		execl("/bin/login", "login", 0);
		printf("No login in /etc or /bin on root\n");
		exit(11);
	}
	if((setgid(pwd->pw_gid) != 0) || (setuid(pwd->pw_uid) != 0)) {
		printf("Invalid ID\n");
		exit(2);
	}
	if(chdir(pwd->pw_dir) < 0) {
		printf("No directory\n");
		exit(2);
	}
	if(pwd->pw_uid == 0)
	{
#ifdef CONSOLE
		if(strcmp(ttyn, CONSOLE))
		{
			printf("Not on system console\n");
			exit(10);
		}
#endif
		envinit[1] = "PATH=/bin:/usr/bin:/etc";
	}
	if (*pwd->pw_shell == '\0')
		pwd->pw_shell = "/bin/sh";
#ifndef NOSHELL
	else {
#ifdef PWB
		envinit[3] = shell;
#else
		envinit[2] = shell;
#endif
		strcat(shell, pwd->pw_shell);
	}
#endif
	environ = envinit;
	strcat(homedir, pwd->pw_dir);
#ifdef PWB
	strcat(logname, pwd->pw_name);
#endif
	if ((namep = strrchr(pwd->pw_shell, '/')) == NULL)
		namep = pwd->pw_shell;
	else
		namep++;
	strcat(minusnam, namep);
	alarm(0);
/*
	if (nopassword) {
		printf("You don't have a password.  Choose one.\n");
		execl("/bin/passwd", "passwd", utmp.ut_name, 0);
	}
*/

/* is the age of the password to be checked? */
	if (*pwd->pw_age != NULL) {
		/* retrieve (a) week of previous change; 
			(b) maximum number of valid weeks	*/
		when = (long) a64l (pwd->pw_age);
		/* max, min and weeks since last change are packed radix 64 */
		maxweeks = when & 077;
		minweeks = (when >> 6) & 077;
		when >>= 12;
		now  = time(0)/WEEK;
		if ((now > when + maxweeks) && (maxweeks >= minweeks)) {
			printf ("Your password has expired. Choose a new one\n");
			execl("/bin/passwd", "passwd", utmp.ut_name, 0);
			exit(9);
		}
	}
/*
	showmotd();
	strcat(maildir, pwd->pw_name);
	if(stat(maildir, &sbuf) >= 0)
		if(sbuf.st_size > 0)
			printf("You have mail.\n");
*/
	signal(SIGQUIT, 0);
	signal(SIGINT, 0);
	execlp(pwd->pw_shell, minusnam, 0);
	printf("No shell\n");
	exit(7);
}

/*
int	stopmotd;
catch()
{
	signal(SIGINT, 1);
	stopmotd++;
}

showmotd()
{
	FILE *mf;
	register c;

	signal(SIGINT, catch);
	if((mf = fopen("/etc/motd","r")) != NULL) {
		while((c = getc(mf)) != EOF && stopmotd == 0)
			putchar(c);
		fclose(mf);
	}
	signal(SIGINT, 1);
}
*/

bailout () {
/* invoked after SIGALRM */
	struct termio s;
	s.c_cflag = B0|HUPCL;
	ioctl(0, TCSETA, &s);
	exit (1);
}
utscan(utf,lnam)
register utf;
char *lnam;
{
	register i;
	struct utmp wtmp;

	lseek(utf, (long)0, 0);
	while(read(utf, (char *)&wtmp, sizeof(wtmp)) == sizeof(wtmp))
	{
		for(i=0; i<8; i++)
			if(wtmp.ut_line[i] != lnam[i])
				goto contin;
		lseek(utf, -(long)sizeof(wtmp), 1);
		return(1);
		contin:;
	}
	return(0);
}
