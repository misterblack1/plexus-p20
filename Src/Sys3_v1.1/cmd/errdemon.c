#include <stdio.h>
#include <signal.h>
#include <sys/param.h>
#ifdef	vax
#include <sys/mba.h>
#endif
#include <sys/utsname.h>
#include <sys/elog.h>
#include <sys/erec.h>

int	getsig();
int	stop;
int	efno;
int	edno;

char	*errfile	= "/usr/adm/errfile";
char	*errdev		= "/dev/error";

union {
	char	buf[BSIZE];
	struct errhdr ee;
} eun;

main(argc,argv)
char **argv;
{
	register n;

	if(argc > 1)
		errfile = *++argv;
	chdir("/");
	stop = 0;
	if((n = fork()) > 0)
		exit(0);
	else if(n < 0) {
		fprintf(stderr,"Can't fork\n");
		exit(8);
	}
	if((efno = open(errfile,1)) < 0)
		if((efno = creat(errfile,0666)) < 0) {
			fprintf(stderr,"Can't create %s\n",errfile);
			exit(8);
		}
	if((edno = open(errdev,0)) < 0) {
		fprintf(stderr,"Can't open %s\n",errdev);
		exit(8);
	}
	lseek(efno,(long)0,2);
	for(n = 0; n < NOFILE; n++)
		if(n != efno && n != edno)
			close(n);
	signal(SIGHUP,1);
	signal(SIGINT,1);
	signal(SIGQUIT,1);
	signal(SIGTERM,getsig);
	while(!stop) {
		if((n = read(edno,eun.buf,sizeof(eun))) > 0)
			write(efno,eun.buf,n);
	}
	eun.ee.e_type = E_STOP;
	eun.ee.e_len = sizeof(struct eend);
	time(&eun.ee.e_time);
	write(efno,eun.buf,sizeof(struct eend));
	exit(0);
}
getsig(n)
{
	signal(n,getsig);
	if(n == SIGTERM)
		stop++;
}
