main( argc, argv )
char *argv[];
{
	int	fd, wfd, n, i;
	char	buffer[0x800];
	int	cnt;
	char	*device;
	char	*filename;
	int	oldm;

	if ( argc != 3 ) {
		printf( "usage: icpdmp device file!\n" );
		exit(4);
	}
	device = argv[1];
	filename = argv[2];
	if ( (fd = open( device, 0 )) == (-1) ) {
		printf( "Can not open %s.\n", device );
		exit(1);
	}
	lseek( fd, (long)0x4200, 0 );
	oldm = umask(0);
	wfd = creat( filename, 775 );
	if ( wfd == (-1) ) {
		printf( "Can not create new dump file!\n" );
		umask(oldm);
		exit(3);
	}
	for ( i = 0; i < 16; i++ ) {
		if ( i == 15 )
			cnt = 0x600;
		else cnt = 0x800;
		n = read( fd, buffer, cnt );
		if ( n == (-1) ) {
			printf( "Read failure!\n" );
			umask(oldm);
			exit(2);
		}
		n = write( wfd, buffer, cnt );
		if ( n == (-1) ) {
			printf( "Write failure!\n" );
			umask(oldm);
			exit(3);
		}
	}
	close( fd );
	close( wfd );
	umask(oldm);
}
