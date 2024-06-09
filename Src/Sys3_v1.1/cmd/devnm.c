#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/dir.h>

main(argc, argv)
char	**argv;
{

	struct stat sbuf;
	struct direct dbuf;
	short	dv, fno;

	if (chdir("/dev") == -1)
		goto  err;
	if ((dv = open(".", 0)) < 0) {
err:
		fprintf(stderr, "Cannot open /dev\n");
		exit(1);
	}
	while(--argc) {
		lseek(dv, (long)0, 0);
		if (stat(*++argv, &sbuf) == -1) continue;
		fno = sbuf.st_dev;
		while(read(dv, &dbuf, sizeof dbuf) == sizeof dbuf) {
			if (!dbuf.d_ino) continue;
			if (stat(dbuf.d_name, &sbuf) == -1) {
				fprintf(stderr, "/dev stat error\n");
				exit(1);
			}
			if ((fno != sbuf.st_rdev) || ((sbuf.st_mode & S_IFMT) !=
				S_IFBLK)) continue;
			printf("%s %s\n", dbuf.d_name, *argv);
		}
	}
	exit (0);
}
