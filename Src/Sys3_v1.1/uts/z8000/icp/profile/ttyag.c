#include	<sys/param.h>
#include	<sgtty.h>
#define	RBUFSIZE	512
#define	LOOPCNT	128
main( argc, argv )
char *argv[];
{
	char rbuf[RBUFSIZE];
	char *device;
	time_t	stime, etime, ttime;
	int	tfd, ffd, i;
	struct sgttyb ttbuf;

	if ( argc != 2 ) {
		printf( "usage: ttyag device\n" );
	}
	device = argv[1];
	tfd = open( device, 2 );
	if ( tfd <= 0 ) {
		printf( "Can not open device!\n" );
		exit(2);
	}
	ffd = open( "profiledata", 0 );
	if ( ffd <= 0 ) {
		printf( "Can not open test data file!\n" );
		exit(3);
	}
	read( ffd, rbuf, RBUFSIZE );
	i = ioctl( tfd, TIOCGETP, &ttbuf );
	ttbuf.sg_flags |= CRMOD;
	i = ioctl( tfd, TIOCSETP, &ttbuf );
	time(&stime);
	for ( i = 0; i < LOOPCNT; i++ ) {
		write( tfd, rbuf, RBUFSIZE );
	}
	time(&etime);
	ttime = etime - stime;
	printf( "%s = %ld\n", device, ttime );
	close( ffd );
	close( tfd );
	exit(0);
}
