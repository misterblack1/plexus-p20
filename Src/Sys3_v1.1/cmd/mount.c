#include <sys/param.h>
#include <sys/filsys.h>
#include <stdio.h>
#include <mnttab.h>
#include <signal.h>

char	*flg[] = {
	"read/write",
	"read only"
	};

struct mnttab mtab[NMOUNT], *mp;
struct filsys super;
main(argc,argv)
char **argv;
{
	int rec, p, dev;
	register char *np;
	char	*ctime();


	rec = open("/etc/mnttab",0);
	if(rec < 0) {
		fprintf(stderr, "mount: cannot open /etc/mnttab!\n");
		exit(2);
	}
	read(rec, mtab, sizeof mtab);
	if(argc == 1) {
		for(mp = mtab; mp < &mtab[NMOUNT]; mp++) {
			if(mp->mt_dev[0]) {
				printf("%s on /dev/%s %s on %s",
				mp->mt_filsys, mp->mt_dev,
				flg[mp->mt_ro_flg], ctime(&mp->mt_time));
			}
		}
		return;
	}
/* check for proper arguments */

	signal(SIGHUP, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGINT, SIG_IGN);
	if(argc == 2) {
		fprintf(stderr, "usage: mount [device] [name]\n");
		exit(2);
	}
	if(*argv[2] != '/') {
		fprintf(stderr, "usage: preceed argument with / such as : /%s\n",argv[2]);
		exit(2);
	}
	if((dev = open(argv[1],0)) <1) {
		fprintf(stderr, "mount: cannot open %s\n", argv[1]);
		exit(2);
	}
	if(lseek(dev, (long)BSIZE, 0) < 0) {
		fprintf(stderr, "mount: lseek error on %s\n", argv[1]);
		exit(2);
	}
	if(read(dev, &super, BSIZE) < 0) {
		fprintf(stderr, "mount: read error on %s\n", argv[1]);
		exit(2);
	}
	if(strncmp(argv[2]+1, super.s_fname, 5))
		printf("WARNING!! - mounting: <%.6s> as <%.6s>\n",
			super.s_fname, argv[2]);
	close(dev);

	if(mount(argv[1],argv[2], argc > 3) <0) {
		perror("mount");
		exit(2);
	}
	np = argv[1];
	while(*np++);
	np--;
	while(*--np == '/')
		*np = '\0';
	while(np > argv[1] && *--np != '/');
	if(*np == '/') np++;

	argv[1] = np;
	for(mp = mtab; mp < &mtab[NMOUNT]; mp++) {
		if(mp->mt_dev[0] == 0) {
			for(np = mp->mt_dev; np < &mp->mt_dev[10];)
				if((*np++ = *argv[1]++ ) == 0) argv[1]--;
			for(np = mp->mt_filsys; np < &mp->mt_filsys[10];)
				if((*np++ = *argv[2]++) == 0) argv[2]--;
			time(&mp->mt_time);
			mp->mt_ro_flg = argc - 3;
			mp = &mtab[NMOUNT];
			while((--mp)->mt_dev[0] == 0);
			rec = creat("/etc/mnttab",0644);
			write(rec, mtab, (mp-mtab+1)*sizeof mtab[0]);
			exit(0);
		}
	}
}
