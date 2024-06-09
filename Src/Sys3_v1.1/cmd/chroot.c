# include <stdio.h>
/* chroot */

main(argc, argv)
char **argv;
{
	if(argc < 3) {
		printf("usage: chroot rootdir command arg ...\n");
		exit(1);
	}
	argv[argc] = 0;
	if(argv[argc-1] == (char *) -1) /* don't ask why */
		argv[argc-1] = (char *) -2;
	if (chroot(argv[1]) < 0) {
		perror(argv[1]);
		exit(1);
	}
	if (chdir("/") < 0) {
		printf("Can't chdir to new root\n");
		exit(1);
	}
	execv(argv[2], &argv[2]);
	close(2);
	open("/dev/tty", 1);
	printf("%s: not found\n",argv[2]);
	exit(1);
}
