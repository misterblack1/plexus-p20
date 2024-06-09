/*
 * mesg -- set current tty to accept or
 *	forbid write permission.
 *
 *	mesg [-y] [-n]
 *		y allow messages
 *		n forbid messages
 *	return codes
 *		0 if messages are ON or turned ON
 *		1 if messages are OFF or turned OFF
 *		2 if usage error
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

struct stat sbuf;

char *tty;
char *ttyname();

main(argc, argv)
char *argv[];
{
	int i, c, r=0, errflag=0;
	extern int optind;

	for(i = 0; i <= 2; i++) {
		if ((tty = ttyname(i)) != NULL)
			break;
	}
	if (stat(tty, &sbuf) < 0)
		error("cannot stat");
	if (argc < 2) {
		if (sbuf.st_mode & 02)
			printf("is y\n");
		else  {
			r = 1;
			printf("is n\n");
		}
	}
	while ((c = getopt(argc, argv, "yn")) != EOF) {
		switch (c){
		case 'y':
			newmode(0622);
			break;
		case 'n':
			newmode(0600);
			r = 1;
			break;
		case '?':
			errflag++;
		}
	}

	if (errflag /*  || (argc > optind) */ )
		error("usage: mesg [-y] [-n]");

/* added for temporary compat. */
	if(argc > optind) switch(*argv[optind]) {
		case 'y':
			newmode(0622);
			break;
		case 'n':
			newmode(0600);
			r = 1;
			break;
		default:
			errflag++;
		}

	if (errflag)
		error("usage: mesg [-y] [-n]");
/* added to here */
	exit(r);
}

error(s)
char *s;
{
	fprintf(stderr, "mesg: %s\n", s);
	exit(2);
}

newmode(m)
{
	if (chmod(tty, m) < 0)
		error("cannot change mode");
}
