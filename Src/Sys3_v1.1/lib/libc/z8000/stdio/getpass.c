/*LINTLIBRARY*/
#include <stdio.h>
#include <signal.h>
#include <termio.h>

char *
getpass(prompt)
char *prompt;
{
	struct termio ttyb;
	unsigned short flags;
	register char *p;
	register c;
	FILE *fi;
	static char pbuf[9];
	int (*signal())();
	int (*sig)();

	if((fi = fopen("/dev/tty", "r")) == NULL)
		return((char *) NULL);
	else
		setbuf(fi, (char *)NULL);
	sig = signal(SIGINT, SIG_IGN);
	ioctl(fileno(fi), TCGETA, &ttyb);
	flags = ttyb.c_lflag;
	ttyb.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL);
	ioctl(fileno(fi), TCSETAF, &ttyb);
	fprintf(stderr, prompt);
	for(p=pbuf; (c = getc(fi))!='\n' && c!=EOF;) {
		if(p < &pbuf[8])
			*p++ = c;
	}
	*p = '\0';
	fprintf(stderr, "\n");
	ttyb.c_lflag = flags;
	ioctl(fileno(fi), TCSETA, &ttyb);
	signal(SIGINT, sig);
	if(fi != stdin)
		fclose(fi);
	return(pbuf);
}
