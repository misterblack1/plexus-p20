#include <stdio.h>
#include <sys/types.h>
#include <mnttab.h>
#define EQ(a,b) (!strcmp(a,b))

struct mnttab mtab[NMOUNT], *mp;

main(argc, argv)
char **argv;
{
	register char *np;
	int rec;

	sync();
	rec = open("/etc/mnttab",0);
	if(rec < 0) {
		fprintf(stderr, "umount: cannot open /etc/mnttab\n");
		exit(2);
	}
	read(rec, mtab, sizeof mtab);
	if(argc != 2) {
		fprintf(stderr, "usage: umount device\n");
		exit(2);
	}
	if (umount(argv[1]) < 0) {
		fprintf(stderr, "umount: cannot unmount %s\n", argv[1]);
		exit(2);
	}
	np = argv[1];
	while(*np++);
	np--;
	while(*--np == '/') *np = '\0';
	while(np > argv[1] && *--np != '/');
	if(*np == '/') np++;
	argv[1] = np;
	for (mp = mtab; mp < &mtab[NMOUNT]; mp++) {
		if(EQ(argv[1], mp->mt_dev)) {
			mp->mt_dev[0] = '\0';
			time(&mp->mt_time);
			mp = &mtab[NMOUNT];
			while ((--mp)->mt_dev[0] == '\0');
			rec = creat("/etc/mnttab", 0644);
			write(rec, mtab, (mp-mtab+1)*sizeof mtab[0]);
			exit(0);
		}
	}
	fprintf(stderr, "warning: /dev/%s was not in mount table\n", argv[1]);
	exit(2);
}
