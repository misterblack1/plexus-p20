#include <stdio.h>
#include <sys/param.h>
#include <sys/filsys.h>
#include <signal.h>
#define DEV 1
#define FSNAME 2
#define VOLUME 3
 /* write fsname, volume # on disk superblock */
struct {
	char fill1[BSIZE];
	struct filsys fs;
	char fill2[5120];
} super;
#define IFTAPE(s) (equal(s,"/dev/rmt",8)||equal(s,"rmt",3))
struct {
	char	t_magic[8];
	char	t_volume[6];
	char	t_reels,
		t_reel;
	long	t_time;
	char	t_fill[492];
} Tape_hdr;

sigalrm()
{
	signal(SIGALRM, sigalrm);
}

main(argc, argv) char **argv; {
int fsi, fso;
long curtime;
int i;

	signal(SIGALRM, sigalrm);

	if(argc!=4 && argc!=2 && argc!=5)  {
showusage:
		fprintf(stderr,"Usage: labelit /dev/r??? [fsname volume [-n]]\n");
		exit(2);
	}
	if(argc==5) {
		if(strcmp(argv[4], "-n")!=0)
			goto showusage;
		if(!IFTAPE(argv[DEV])) {
			fprintf(stderr, "labelit: `-n' option for tape only\n");
			exit(2);
		}
		printf("Skipping label check!\n");
		goto do_it;
	}

	if((fsi = open(argv[DEV],0)) < 1) {
		fprintf(stderr, "labelit: cannot open device\n");
		exit(2);
	}

	if(IFTAPE(argv[DEV])) {
		alarm(5);
		read(fsi, &Tape_hdr, BSIZE);
		if(!equal(Tape_hdr.t_magic, "Volcopy", 7)) {
			fprintf(stderr, "labelit: tape not labelled!\n");
			exit(2);
		}
		printf("Tape volume: %s, reel %d of %d reels\n",
			Tape_hdr.t_volume, Tape_hdr.t_reel, Tape_hdr.t_reels);
		printf("Written: %s", ctime(&Tape_hdr.t_time));
		if(argc==2 && Tape_hdr.t_reel>1)
			exit(0);
	}
	if((i=read(fsi, &super, 5120)) != 5120)  {
		fprintf(stderr, "labelit: cannot read 5120 char block.\n");
		exit(2);
	}

	printf("Current fsname: %.6s, Current volname: %.6s, Blocks: %ld\n",
		super.fs.s_fname,super.fs.s_fpack, super.fs.s_fsize);
	printf("Date last mounted: %s", ctime(&super.fs.s_time));
	if(argc==2)
		exit(0);
do_it:
	printf("NEW fsname = %.6s, NEW volname = %.6s -- DEL if wrong!!\n",
		argv[FSNAME], argv[VOLUME]);
	sleep(10);
	sprintf(super.fs.s_fname, "%.6s", argv[FSNAME]);
	sprintf(super.fs.s_fpack, "%.6s", argv[VOLUME]);

	close(fsi);
	fso = open(argv[DEV],1);
	if(IFTAPE(argv[DEV])) {
		strcpy(Tape_hdr.t_magic, "Volcopy");
		sprintf(Tape_hdr.t_volume, "%.6s", argv[VOLUME]);
		if(write(fso, &Tape_hdr, BSIZE) < 0)
			goto cannot;
	}
	if(write(fso, &super, 5120) < 0) {
cannot:
		fprintf(stderr, "labelit cannot write label\n");
		exit(2);
	}
	exit(0);
}
equal(s1, s2, ct)
char *s1, *s2;
int ct;
{
	register i;

	for(i=0; i<ct; ++i) {
		if(*s1 == *s2) {;
			if(*s1 == '\0') return(1);
			s1++; s2++;
			continue;
		} else return(0);
	}
	return(1);
}
