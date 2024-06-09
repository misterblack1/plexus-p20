/*
 *	ct [-h] [-v] [-wn] [-s speed] telno
 *
 *	dials the given telephone number, waits for the
 *	modem to answer, and initiates a login process.
 *
 *	There must be entries in /etc/inittab of the form:
 *		1:cx:k:/etc/getty culx !
 *
 *	Also, the appropriate modification must be made to
 *	the UNIX/TS 1.1 system in order that /dev/tty is
 *	reassigned by the setpgrp system call.
 *	This is done by modifying the setpgrp routine in
 *	sys4.c as follows:
 *	The lines
 *		if (uap->flag)
 *			u.u_procp->p_pgrp = u.u_procp->p_pid;
 *	in UNIX/TS 1.1 should be changed to
 *		if (uap->flag) {
 *			u.u_procp->p_pgrp = u.u_procp->p_pid;
 *			u.u_ttyp = NULL;
 *		}
 *	This modification to UNIX/TS 1.1 is included in the
 *	UNIX/TS 1.1.1 update, and will be included in future
 *	releases of UNIX/TS and PWB/UNIX.
 *
 *	The ct executable should be installed as follows:
 *		owner = root
 *		group = bin
 *		mode  = 4775
 */

# include <fcntl.h>
# include <pwd.h>
# include <sgtty.h>
# include <setjmp.h>
# include <signal.h>
# include <stdio.h>
# include <time.h>
# include <sys/types.h>
# include <sys/dir.h>

# if lint
	int	_null_;
#	define IGN _null_ = (int)
# else
#	define IGN
# endif

# define ROOT	0
# define SYS	3
# define TTYMOD	0666
# define DEV	"/dev/"
# define LCK	"/usr/spool/uucp/LCK.."

char	log[] = "/usr/adm/ctlog",
	getty[] = "/etc/getty",
	legal[] = "0123456789-*:#;e<w=f",
	acu[sizeof DEV + DIRSIZ - 5] = DEV,
	tty[sizeof DEV + DIRSIZ - 5] = DEV,
	lock[sizeof LCK + DIRSIZ - 5] = LCK,
	devtab[] = "/usr/lib/uucp/L-devices";

int	optind;
char	*optarg;
FILE	*Ldevices, *Log;

int	disconnect ();
char	*strcpy ();
char	*strncpy ();
char	*strchr ();
char	*asctime ();
long	time ();

unsigned	sleep ();

extern	long	timezone;

struct	tm	*gmtime ();
struct	sgttyb	arg;
struct	passwd	*getpwuid ();

struct {
	long	l_on;
	long	l_off;
	short	l_uid;
	char	l_spd[4];
} ct;

jmp_buf	env;

main (argc, argv)
char *argv[]; {
	register dn, pid;
	register char *dp;
	char *num, *aptr;
	char *wspeed = "300";
	char tbuf[32];
	int c, dl, status;
	int hangup = 1;
	int verbose = 0;
	int max = -1;
	int count = 0;

	IGN signal (SIGHUP, SIG_IGN);
	IGN signal (SIGQUIT, SIG_IGN);

	if ((Ldevices = fopen (devtab, "r")) == NULL) {
		fprintf (stderr, "ct: can't open %s\n", devtab);
		exit (1);
	}

	dn = 0;
	while ((c = getopt (argc, argv, "hvw:s:")) != EOF)
		switch (c) {

		case 'h':
			hangup = 0;
			break;

		case 'v':
			verbose = 1;
			break;

		case 'w':
			max = atoi (optarg);
			break;

		case 's':
			wspeed = optarg;
			break;

		case '?':
			dn++;
			break;
		}

	if (dn || optind != argc-1) {
		IGN fputs ("Usage:  ct [-h] [-v] [-wn] [-s speed] telno\n", stderr);
		exit (1);
	}
	num = argv[optind];
	if (strlen (num) >= sizeof tbuf - 1) {
		IGN fputs ("ct: phone number too long\n", stderr);
		exit (1);
	}
	if (check (num) < 0) {
		IGN fputs ("ct: bad phone number\n", stderr);
		exit (1);
	}
	for (;;) {
		dn = gdev (wspeed);
		if (count == 0) {
			if (dn >= 0)
				fprintf (stderr, "Allocated dialer at %s baud\n", wspeed);
			else {
				fprintf (stderr, "%d busy dialer", -dn);
				if (dn < -1)
					IGN fputc ('s', stderr);
				fprintf (stderr, " at %s baud\n", wspeed);
				if (max <= 0) {
					if (max < 0) {
						IGN fputs ("Wait for dialer? ", stderr);
						if (gets (tbuf) == NULL || tolower (*tbuf) != 'y')
							exit (1);
						IGN fputs ("Time, in minutes? ", stderr);
						IGN scanf ("%d", &max);
						while (getchar () != '\n');
					}
					if (max <= 0)
						exit (1);
				} else
					IGN fputs ("Waiting for dialer\n", stderr);
			}
			if (!isatty (fileno (stdin)))
				hangup = 0;
			if (hangup) {
				IGN fputs ("Confirm hang-up? ", stderr);
				if (gets (tbuf) == NULL || tolower (*tbuf) != 'y')
					if (dn >= 0)
						error ();
					else
						exit (1);
				if (isatty (fileno (stdout)))
					verbose = 0;
				sleep (2);
				IGN stty (0, &arg);
				sleep (5);
			}
			IGN close (2);
			IGN dup (1);
		}
		if (dn >= 0)
			break;
		if (verbose && count) {
			IGN fputs ("Dialer", stderr);
			if (dn == -1)
				IGN fputs (" is", stderr);
			else
				IGN fputs ("s are", stderr);
			fprintf (stderr, " busy (%d minute", count);
			if (count > 1)
				IGN fputc ('s', stderr);
			IGN fputs (")\n", stderr);
		}
		if (count++ >= max) {
			if (verbose)
				IGN fputs ("*** TIMEOUT ***\n", stderr);
			exit (1);
		}
		sleep (60);
	}
	if (count && verbose)
		IGN fputs ("Allocated dialer\n", stderr);
	if ((dl = open (tty, O_RDWR | O_NDELAY)) < 0) {
		if (!hangup || verbose)
			fprintf (stderr, "ct: can't open %s\n", tty);
		error ();
	}
	if ((aptr = strchr (strcpy (tbuf, num), '\0'))[-1] != '-') {
		*aptr++ = '-';
		*aptr = '\0';
	}
	if (verbose)
		fprintf (stderr, "Dialing %s\n", tbuf);
	if (write (dn, num, strlen (num)) < 0) {
		if (!hangup || verbose)
			fprintf (stderr, "ct: write error on %s\n", acu);
		error ();
	}
	IGN close (dn);
	if (verbose)
		IGN fputs ("Connected\n", stderr);
	if (setjmp (env))
		goto disc;
	IGN signal (SIGHUP, disconnect);
	IGN signal (SIGINT, disconnect);
	IGN signal (SIGTERM, disconnect);
	sleep (2);
	ct.l_on = time ((long *) 0);
	IGN close (0);
	if ((pid = fork ()) == 0) {
		IGN setpgrp ();
		if (strcmp (wspeed, "1200") == 0)
			dp = "3";
		else
			dp = "0";
		execl (getty, "getty", &tty[5], dp, "60", 0);
	}
	if (pid <= 0) {
		if (pid < 0 && (!hangup || verbose))
			IGN fputs ("ct: can't fork for getty\n", stderr);
		error ();
	}
	while (wait (&status) != pid);
	if (status < 0) {
		if (!hangup || verbose)
			IGN fputs ("ct: can't exec getty\n", stderr);
		error ();
	}
disc:
	IGN signal (SIGHUP, SIG_IGN);
	IGN signal (SIGINT, SIG_IGN);
	IGN signal (SIGTERM, SIG_IGN);
	ct.l_off = time ((long *) 0);
	IGN unlink (lock);
	IGN stty (dl, &arg);
	IGN close (dl);
	if (verbose)
		IGN fputs ("Disconnected\n", stderr);
	ct.l_uid = (short) getuid ();
	IGN strncpy (ct.l_spd, wspeed, 4);
	if (access (log, 0) >= 0 && (Log = fopen (log, "a")) != NULL) {
		fprintf (Log, "%-8s ", getpwuid ((int) ct.l_uid)->pw_name);
		fprintf (Log, "(%4s)  ", ct.l_spd);
		ct.l_on -= timezone;
		(aptr = asctime (gmtime (&ct.l_on)))[19] = '\0';
		fprintf (Log, "%s  ", aptr);
		ct.l_off -= timezone;
		(aptr = asctime (gmtime (&ct.l_off)))[19] = '\0';
		fprintf (Log, "%s  %s\n", aptr, num);
		IGN fclose (Log);
	}
	if ((pid = fork ()) == 0)
		execl (getty, "getty", &tty[5], "!", 0);
	if (pid <= 0) {
		if (pid < 0 && verbose)
			IGN fputs ("Can't fork to clean up utmp\n", stderr);
		exit (1);
	}
	while (wait ((int *) 0) != pid);
	if (chown (tty, ROOT, SYS) < 0 || chmod (tty, TTYMOD) < 0) {
		if (verbose)
		fprintf (stderr, "Can't chown/chmod on %s\n", tty);
		exit (1);
	}
	exit (0);
}

check (num)
char *num; {
	register char *ptr1, *ptr2;

	if (*(ptr1 = num) == '\0')
		return (-1);

	do {
		ptr2 = legal;
		do {
			if (*ptr1 == *ptr2)
				break;
		} while (*++ptr2);
		if (*ptr2 == '\0')
			return (-1);
	} while (*++ptr1);
	return (0);
}

error () {

	IGN unlink (lock);
	exit (1);
}

gdev (wspeed)
register char *wspeed; {
	register char *lspeed;
	register dn;
	char Lbuf[80];
	char *getarg ();
	int lck, pid, exists = 0;

	IGN fseek (Ldevices, (long) 0, 0);

	while (fgets (Lbuf, sizeof Lbuf, Ldevices) != NULL) {
		if (strcmp (getarg (Lbuf), "ACU"))
			continue;
		IGN strncpy (&tty[5], getarg ((char *) 0), sizeof tty - 6);
		IGN strncpy (&acu[5], getarg ((char *) 0), sizeof acu - 6);
		lspeed = getarg ((char *) 0);
		if (strcmp (wspeed, lspeed) != 0)
			continue;
		exists++;
		IGN strcpy (&lock[21], &tty[5]);
		if ((lck = open (lock, O_WRONLY | O_CREAT | O_EXCL, 0444)) >= 0
		    && (dn = open (acu, O_WRONLY)) >= 0) {
			IGN signal (SIGINT, error);
			IGN signal (SIGTERM, error);
			pid = getpid ();
			IGN write (lck, (char *) &pid, sizeof (pid));
			IGN close (lck);
			return (dn);
		}
	}
	if (exists)
		return (-exists);
	fprintf (stderr, "No %s baud dialers on this system\n", wspeed);
	error ();
/*NOTREACHED*/
}

char *
getarg (p)
register char *p; {
	register char *q;
	static char *savepoint;

	if (p == (char *) 0)
		p = savepoint;

	while (*p == ' ' || *p == '\t')
		p++;
	q = p;
	while (*p != ' ' && *p != '\t' && *p != '\0' && *p != '\n')
		p++;
	*p = '\0';
	savepoint = ++p;
	return (q);
}

disconnect () {

	longjmp (env, 1);
}
