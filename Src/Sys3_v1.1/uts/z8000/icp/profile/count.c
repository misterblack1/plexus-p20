#include	"sys/param.h"
#include	"sys/ioctl.h"
#define	RBUFSIZE	512
#define	LOOPCNT	128
main( argc, argv )
char *argv[];
{
	char *device;
	time_t	stime, etime, ttime;
	int	tfd, ffd, i;
	int buf[512];

	if ( argc != 2 ) {
		printf( "usage: getdata device\n" );
	}

	device = argv[1];
	tfd = open( device, 2 );
	if ( tfd <= 0 ) {
		printf( "Can not open device!\n" );
		exit(2);
	}

	if(ioctl( tfd, TCGETP, buf ) == -1) {
		printf("problem with ioctl call.\n");
		exit(1);
	}
	printf("sioopen:	%d\n",buf[0]);
	printf("sioclose:	%d\n",buf[1]);
	printf("sioread:	%d\n",buf[2]);
	printf("siowrite:	%d\n",buf[3]);
	printf("sioioctl:	%d\n",buf[4]);
	printf("sioparam:	%d\n",buf[5]);
	printf("siorec:		%d\n",buf[6]);
	printf("sioproc:	%d\n",buf[7]);
	printf("siomctl:	%d\n",buf[8]);
	printf("siospec:	%d\n",buf[9]);
	printf("sioext:		%d\n",buf[10]);
	printf("sioclean:	%d\n",buf[11]);
	printf("putchar:	%d\n",buf[12]);
	printf("startdma:	%d\n",buf[13]);
	printf("dma:		%d\n",buf[14]);
	printf("pioi:		%d\n",buf[15]);
	printf("piostart:	%d\n",buf[16]);
	printf("ttin:		%d\n",buf[17]);
	printf("ttxput:		%d\n",buf[18]);
	printf("tttimeo:	%d\n",buf[19]);
	printf("siggo:		%d\n",buf[20]);
	printf("ttiocom:	%d\n",buf[21]);
	printf("ttywait:	%d\n",buf[22]);
	printf("ttyflush:	%d\n",buf[23]);
	printf("canon:		%d\n",buf[24]);
	printf("ttrstrt:	%d\n",buf[25]);
	printf("endbreak:	%d\n",buf[26]);
	printf("getc:		%d\n",buf[27]);
	printf("putc:		%d\n",buf[28]);
	printf("getcb:		%d\n",buf[29]);
	printf("putcb:		%d\n",buf[30]);
	printf("getcf:		%d\n",buf[31]);
	printf("putcf:		%d\n",buf[32]);
	printf("bcopy:		%d\n",buf[33]);
	printf("clock:		%d\n",buf[34]);
	printf("timeout:	%d\n",buf[35]);
	printf("delay:		%d\n",buf[36]);
	printf("godaemon:	%d\n",buf[37]);
	printf("daemon:		%d\n",buf[38]);
	printf("mbmore:		%d\n",buf[39]);
	printf("mbstart:	%d\n",buf[40]);
	printf("qmbq:		%d\n",buf[41]);
	printf("mvtomb:		%d\n",buf[42]);
	printf("mvfrommb:	%d\n",buf[43]);
	printf("chkit:		%d\n",buf[44]);
	printf("msignal:	%d\n",buf[45]);
	printf("mwakeup:	%d\n",buf[46]);
	printf("mflush:		%d\n",buf[47]);
	printf("dqpbiq:		%d\n",buf[48]);
	printf("pbint:		%d\n",buf[49]);
	printf("pbomore:	%d\n",buf[50]);
	printf("printf:		%d\n",buf[51]);
	printf("printn:		%d\n",buf[52]);
	printf("panic:		%d\n",buf[53]);
	printf("smalloc:	%d\n",buf[54]);
	printf("smfree:		%d\n",buf[55]);
	printf("sched:		%d\n",buf[56]);
	printf("sleep:		%d\n",buf[57]);
	printf("wakeup:		%d\n",buf[58]);
	printf("setrun:		%d\n",buf[59]);
	printf("swtch:		%d\n",buf[60]);
	printf("newproc:	%d\n",buf[61]);
	printf("setproc:	%d\n",buf[62]);
	printf("delproc:	%d\n",buf[63]);
	printf("delsleep:	%d\n",buf[64]);
	printf("delrun:		%d\n",buf[65]);
	printf("setbufp:	%d\n",buf[66]);
	printf("clrbufp:	%d\n",buf[67]);
	printf("exit:		%d\n",buf[68]);
	printf("startup:	%d\n",buf[69]);
	printf("max:		%d\n",buf[70]);
	printf("min:		%d\n",buf[71]);
	printf("nodev:		%d\n",buf[72]);
	printf("nulldev:	%d\n",buf[73]);
	printf("wcopy:		%d\n",buf[74]);
}
