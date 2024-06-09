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
	printf("%d:\tsleep for carrier detect (sioopen)\n",buf[0]);
	printf("%d:\tsleep for pio output completion (siowrite)\n",buf[1]);
	printf("%d:\tsleep due to excess output chars (siowrite)\n",buf[2]);
	printf("%d:\twait for outpput to flush (ttyflush)\n",buf[3]);
	printf("%d:\twait for delimiter in read (canon)\n",buf[4]);
	printf("%d:\twait for delay (delay)\n",buf[5]);
	printf("%d:\tdaemon timeout wait\n",buf[6]);
	printf("%d:\tmbq queue empty wait\n",buf[7]);
	printf("%d:\tsleep on mb transfer to main memory completion\n",buf[8]);
	printf("%d:\tsleep on mb transfer from main memory completion\n",buf[9]);
	printf("%d:\tmsiganl queue is empty\n",buf[10]);
	printf("%d:\tmwakeup queue is empty\n",buf[11]);
	printf("%d:\tmflush queue is empty\n",buf[12]);
	printf("%d:\tdppbiq queue is empty\n",buf[13]);
	printf("%d:\tbuffer data pool area exhausted\n",buf[14]);
	printf("%d:\tsleep on excess number of procs\n",buf[15]);
}
