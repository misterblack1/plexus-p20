#include	<stdio.h>
#include	<sys/utsname.h>
struct utsname unstr, *un;
main(argc, argv)
char **argv;
int argc;
{
	register i;
	int	sflg=1, nflg=0, rflg=0, vflg=0, errflg=0;
	int	optlet;
	un = &unstr;
	uname(un);
	while((optlet=getopt(argc, argv, "asnrv")) != EOF) switch(optlet) {
	case 'a':
		sflg++; nflg++; rflg++; vflg++;
		break;
	case 's':
		sflg++;
		break;
	case 'n':
		nflg++;
		break;
	case 'r':
		rflg++;
		break;
	case 'v':
		vflg++;
		break;
	case '?':
		errflg++;
	}
	if(errflg) {
		fputs("usage: uname [-snrv]\n", stderr);
		exit(2);
	}
	if(nflg | rflg | vflg) sflg--;
	if(sflg)
		fputs(un->sysname,stdout);
	if(nflg) {
		if(sflg) putchar(' ');
		fputs(un->nodename,stdout);
	}
	if(rflg) {
		if(sflg | nflg) putchar(' ');
		fputs(un->release,stdout);
	}
	if(vflg) {
		if(sflg | nflg | rflg) putchar(' ');
		fputs(un->version,stdout);
	}
	putchar('\n');
}
