/*
 * Enter a password in the password file.
 * This program should be suid with the owner
 * having write permission on /etc
 */

#include <stdio.h>
#include <signal.h>
#include <pwd.h>
#include <sys/types.h>

char	passwd[] = "/etc/passwd";
char	opasswd[] = "/etc/opasswd";
char	temp[]	 = "/etc/ptmp";
struct	passwd *pwd, *getpwent();
int	endpwent();
char	*crypt();
char	*getpass();
char	*pw;
char	pwbuf[10];
char	opwbuf[10];
char	buf[10];

time_t	when;
time_t	now;
time_t	maxweeks;
time_t	minweeks;
long	a64l();
char	*l64a();
long	time();
int	count; /* count verifications */

#define WEEK (24L * 7 * 60 * 60) /* seconds per week */
#define MINLENGTH 3  /* for passwords */

main (argc, argv)
	int argc;
	char *argv[];
{
	char *p;
	int i;
	char saltc[2];
	long salt;
	int u;
	int insist;
	int ok, flags;
	int c;
	int pwlen;
	FILE *tf;
	char *uname, *getlogin();

	insist = 0;
	count = 0;
	if (argc < 2) {
		if ((uname = getlogin()) == NULL) {
			fprintf (stderr, "Usage: passwd user\n");
			goto bex;
		} else
			fprintf (stderr, "Changing password for %s\n", uname);
	} else
		uname = argv[1];

	while ((pwd = getpwent()) != NULL && strcmp (pwd->pw_name, uname) != 0)
		;
	u = getuid();
	if (pwd == NULL || (u != 0 && u != pwd->pw_uid)) {
		fprintf (stderr, "Permission denied.\n");
		goto bex;
	}
	endpwent();
	if (pwd->pw_passwd[0] && u != 0) {
		strcpy (opwbuf, getpass ("Old password:"));
		pw = crypt (opwbuf, pwd->pw_passwd);
		if (strcmp (pw, pwd->pw_passwd) != 0) {
			fprintf (stderr, "Sorry.\n");
			goto bex;
		}
	} else
		opwbuf[0] = '\0';
	if (*pwd->pw_age != NULL) {
		/* password age checking applies */
		when = (long) a64l (pwd->pw_age);
		/* max, min and week of last change are encoded in radix 64 */
		maxweeks = when & 077;
		minweeks = (when >> 6) & 077;
		when >>= 12;
		now  = time ((long *) 0)/WEEK;
		if (u != 0 && (now < when + minweeks)) {
			fprintf (stderr, "Sorry: < %ld weeks since the last change\n", minweeks);
			goto bex;
		}
		if (minweeks > maxweeks && u != 0) {
			fprintf (stderr, "You may not change this password.\n");
			goto bex;
		}
	}
tryagn:
	strcpy (pwbuf, getpass ("New password:"));
	pwlen = strlen (pwbuf);
	if (u != 0 && (pwlen <= MINLENGTH || strcmp (pwbuf, opwbuf) == 0)) {
		if (pwlen <= MINLENGTH) fprintf (stderr, "Too short. ");
		fprintf (stderr, "Password unchanged.\n");
		goto bex;
	}
	ok = 0;
	flags = 0;
	p = pwbuf;
	while (c = *p++){
		if (c>='a' && c<='z') flags |= 2;
		else if (c>='A' && c<='Z') flags |= 4;
		else if (c>='0' && c<='9') flags |= 1;
		else flags |= 8;
	}
	if (flags >= 7 && pwlen >= 4) ok = 1;
	if ((flags == 2 || flags == 4) && pwlen >= 6) ok = 1;
	if ((flags == 3 || flags == 5 || flags == 6) && pwlen >= 5) ok = 1;

	if (ok == 0 && insist < 3) {
		if (flags==1)
			fprintf (stderr, "Please use at least one non-numeric character.\n");
		else
			fprintf (stderr, "Please use a longer password.\n");
		insist++;
		goto tryagn;
	}
	strcpy (buf, getpass ("Re-enter new password:"));
	if (strcmp (buf, pwbuf)) {
		if (++count > 2) {
			fprintf (stderr, "Too many tries; try again later.\n");
			goto bex;
		} else
			fprintf (stderr, "They don't match; try again.\n");
		goto tryagn;
	}
	time (&salt);
	salt += getpid();

	saltc[0] = salt & 077;
	saltc[1] = (salt >> 6) & 077;
	for (i=0; i<2; i++) {
		c = saltc[i] + '.';
		if (c>'9') c += 7;
		if (c>'Z') c += 6;
		saltc[i] = c;
	}
	pw = crypt (pwbuf, saltc);
	signal (SIGHUP, SIG_IGN);
	signal (SIGINT, SIG_IGN);
	signal (SIGQUIT, SIG_IGN);

	umask (0333);

	if (access (temp, 0) >= 0) {
		fprintf (stderr, "Temporary file busy; try again later.\n");
		goto bex;
	}

/*
 *	Between the time that the previous access completes
 *	and the following fopen completes, it is possible for
 *	some other user to sneak in and foul things up.
 *	It is not possible to solve this by using creat to
 *	create the file with mode 0444, because the creat will
 *	always succeed if our effective uid is 0.
 */

	if ((tf = fopen (temp, "w")) == NULL) {
		fprintf (stderr, "Cannot create temporary file\n");
		goto bex;
	}

/*
 *	copy passwd to temp, replacing matching lines
 *	with new password.
 */

	while ((pwd = getpwent()) != NULL) {
		if (strcmp (pwd->pw_name, uname) == 0) {
			u = getuid();
			if (u != 0 && u != pwd->pw_uid) {
				fprintf (stderr, "Permission denied.\n");
				goto out;
			}
			pwd->pw_passwd = pw;
			if (*pwd->pw_age != NULL) {
				if (maxweeks == 0) 
					*pwd->pw_age = '\0';
				else {
					when = maxweeks + (minweeks << 6) + (now << 12);
					pwd->pw_age = l64a (when);
				}
			}
		}
		putpwent (pwd, tf);
	}
	endpwent ();
	fclose (tf);

/*
 *	Rename temp file back to passwd file.
 */

	if (unlink (opasswd) && access (opasswd, 0) == 0) {
		fprintf (stderr, "cannot unlink %s\n", opasswd);
		goto out;
	}

	if (link (passwd, opasswd)) {
		fprintf (stderr, "cannot link %s to %s\n", passwd, opasswd);
		goto out;
	}

	if (unlink (passwd)) {
		fprintf (stderr, "cannot unlink %s\n", passwd);
		goto out;
	}

	if (link (temp, passwd)) {
		fprintf (stderr, "cannot link %s to %s\n", temp, passwd);
		if (link (opasswd, passwd)) {
			fprintf (stderr, "cannot recover %s\n", passwd);
			goto bex;
		}
		goto out;
	}

	if (unlink (temp)) {
		fprintf (stderr, "cannot unlink %s\n", temp);
		goto out;
	}

	exit (0);

out:
	unlink (temp);

bex:
	exit (1);
}
