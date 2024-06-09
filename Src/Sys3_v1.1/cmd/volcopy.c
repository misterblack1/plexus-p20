/*	volcopy	COMPILE:	cc -O volcopy.c -s -i -o volcopy	*/
#include <sys/param.h>
#include <sys/filsys.h>
#include <sys/stat.h>
#include <stdio.h>
#include <signal.h>
#define FILE_SYS 1
#define DEV_FROM 2
#define FROM_VOL 3
#define DEV_TO 4
#define TO_VOL 5
#define IFTAPE(s) (EQ(s,"/dev/rmt",8)||EQ(s,"rmt",3))
#define _2_DAYS 172800L
#define MAX 1000000L
#define Ft800x10	15L
#define Ft1600x10	28L
#define Ft6250x88	120L

struct {
	char	t_magic[8];
	char	t_volume[6];
	char	t_reels,
		t_reel;
	long	t_time;
	char	t_fill[492];
} Tape_hdr;

int	Nblocks;
long	Reelblks = MAX;
int	Reels = 1;
int	Reelsize = 0;
int	Bpi = 0;
long	Fs;
short	*Buf;
/*

filesystem copy with propagation of volume ID and filesystem name:

  volcopy  filesystem /dev/from from_vol /dev/to to_vol

  Example:

  volcopy root /dev/rrp2 pk5 /dev/rrp12 pk12

  volcopy u3 /dev/rrp15 pk1 /dev/rmt0 tp123

  volcopy u5 /dev/rmt0 -  /dev/rrp15 -

In the last example, dashed volume args mean "use label that's there."

From/to devices are printed followed by `?'.
User has 10 seconds to DEL if mistaken!
 */

long	Block;
char *Totape, *Fromtape;
FILE	*Devtty;
char	*Tape_nm;

struct filsys	Superi, Supero, *Sptr;

extern unsigned	read(), write();

sigalrm()
{
	signal(SIGALRM, sigalrm);
}
sigint()
{
	if(asks("Want Shell?   "))
		system("sh");
	else if(asks("Want to quit?    "))
		exit(2);
	signal(SIGINT, sigint);
}
char *tapeck();

main(argc, argv) char **argv;
{
	int	fsi, fso;
	struct	stat statb;
	long	tvec;
	int i;
	FILE	*fb, *popen();
	long	rblock = 0, reeloff = 0;
	int	reel = 1;
	char	vol[12], dev[12];
	long	saveFs;

	signal(SIGALRM, sigalrm);

	while(argv[1][0] == '-') {
		if(EQ(argv[1], "-bpi", 4))
			Bpi = atoi(&argv[1][4]);
		else if(EQ(argv[1], "-feet", 5))
			Reelsize = atoi(&argv[1][5]);
		else {
			fprintf(stderr, "<%s> invalid option\n",
				argv[1]);
			exit(1);
		}
		++argv;
		--argc;
	}

	Devtty = fopen("/dev/tty", "r");
	time(&tvec);
	if(argc!=6)
		 err("Usage: volcopy fsname /devfrom volfrom /devto volto");

	if((fsi = open(argv[DEV_FROM],0)) < 1)
		fprintf(stderr, "%s: ",argv[DEV_FROM]), err("cannot open");
	if((fso = open(argv[DEV_TO],2)) < 1)
		fprintf(stderr, "%s: ",argv[DEV_TO]), err("cannot open");

	if(fstat(fsi, &statb)<0 || (statb.st_mode&S_IFMT)!=S_IFCHR)
		err("From device not character-special");
	if(fstat(fso, &statb)<0 || (statb.st_mode&S_IFMT)!=S_IFCHR)
		err("To device not character-special");

	Totape = tapeck(argv[DEV_TO], argv[TO_VOL], fso);
	Fromtape = tapeck(argv[DEV_FROM], argv[FROM_VOL], fsi);
	if(Totape && Fromtape)
		err("Use dd(1) command to copy tapes");

	Fs = Nblocks = ((Totape||Fromtape)&&(Bpi!=6250))? 10: 88;
	Buf = (short *)sbrk(512*Nblocks);
	if((int)Buf == -1 && Fs == 88) {
		Fs = Nblocks = 22;
		Buf = (short *)sbrk(512*Nblocks);
	}
	if((int)Buf == -1) {
		fprintf(stderr, "Not enough memory--get help\n");
		exit(1);
	}

	if(read(fso, Buf, 512*Nblocks) != 512*Nblocks)
		err("Read error on output\n");
	Sptr = (struct filsys *)&Buf[256];
	sprintf(Supero.s_fname, "%.6s", Sptr->s_fname);
	sprintf(Supero.s_fpack, "%.6s", Sptr->s_fpack);
	Supero.s_fsize = Sptr->s_fsize;
	Supero.s_time = Sptr->s_time;

	if(read(fsi, Buf, 512*Nblocks) != 512*Nblocks)
		err("read error on input");
	Sptr = (struct filsys *)&Buf[256];
	sprintf(Superi.s_fname, "%.6s", Sptr->s_fname);
	sprintf(Superi.s_fpack, "%.6s", Sptr->s_fpack);
	Fs = Superi.s_fsize = Sptr->s_fsize;
	Superi.s_time = Sptr->s_time;
	if(Totape || Fromtape) {
		Reels = Superi.s_fsize / Reelblks +
			((Superi.s_fsize % Reelblks) && 1);
		printf("You will need %d reels.\n", Reels);
		if(Totape)
			printf("(The same size and density is expected for all reels)\n");
		if(Fromtape && (Tape_hdr.t_reel!=1 || Tape_hdr.t_reels!=Reels))
			fprintf(stderr, "Tape disagrees: Reel %d of %d\n",
				Tape_hdr.t_reel, Tape_hdr.t_reels);
	}

	if(!EQ(argv[FILE_SYS],Superi.s_fname, 6)) {
		printf("arg. (%s) doesn't agree with from fs. (%s)\n",
			argv[FILE_SYS],Superi.s_fname);
		ask();
	}
	if(!EQ(argv[FROM_VOL],Superi.s_fpack, 6)) {
		printf("arg. (%s) doesn't agree with from vol.(%s)\n",
			argv[FROM_VOL],Superi.s_fpack);
		ask();
	}

	if(argv[FROM_VOL][0]=='-') argv[FROM_VOL] = Superi.s_fpack;
	if(argv[TO_VOL][0]=='-') argv[TO_VOL] = Supero.s_fpack;

	if(Supero.s_time+_2_DAYS > Superi.s_time) {
		printf("%s less than 48 hours older than %s\n",
			argv[DEV_TO], argv[DEV_FROM]);
		printf("To filesystem dated:  %s", ctime(&Supero.s_time));
		ask();
	}
	if(!EQ(argv[TO_VOL],Supero.s_fpack, 6)) {
		printf("arg.(%s) doesn't agree with to vol.(%s)\n",
			argv[TO_VOL],Supero.s_fpack);
		ask();
		sprintf(Supero.s_fpack, "%.6s", argv[TO_VOL]);
	}
	if(Superi.s_fsize > Supero.s_fsize && !Totape) {
		printf("from fs larger than to fs\n");
		ask();
	}
	if(!Totape && !EQ(Superi.s_fname,Supero.s_fname, 6)) {
		printf("warning! from fs(%s) differs from to fs(%s)\n",
			Superi.s_fname,Supero.s_fname);
		ask();
	}


COPY:
    printf("From: %s, to: %s? (DEL if wrong)\n", argv[DEV_FROM], argv[DEV_TO]);
	close(fso); close(fsi);
	sync();
	sleep(10);  /* 10 seconds to DEL  */
	fsi = open(argv[DEV_FROM], 0);
	fso = open(argv[DEV_TO], 1);
	if(Totape) {
		Tape_hdr.t_reels = Reels;
		Tape_hdr.t_reel = 1;
		Tape_hdr.t_time = tvec;
		write(fso, &Tape_hdr, 512);
	} else if(Fromtape) {
		read(fsi, &Tape_hdr, 512);
	}
	read(fsi, Buf, 512*Nblocks); /* skip */
	sprintf(Sptr->s_fpack, "%.6s", argv[TO_VOL]);
	sprintf(Sptr->s_fname, "%.6s", argv[FILE_SYS]);
	if(write(fso, Buf, 512*Nblocks) != 512*Nblocks)
		err("Cannot write");
	Fs -= Nblocks;
	Block += Nblocks;

	signal(SIGINT, sigint);

	for(; Fs; Fs -= Nblocks) {
		Nblocks = Fs > Nblocks ? Nblocks : Fs;
		if(rblock >= Reelblks) {
newtape:
			rblock = 0;
			reeloff = Block;
			saveFs = Fs;
			++reel;
again:
			close(Totape? fso: fsi);
			printf(
			 "Changing drives? (type RETURN for no, `/dev/rmt_' for yes: ");
			fgets(dev, 12, Devtty);
			dev[strlen(dev) - 1] = '\0';
			printf("Mount tape %d\nType volume-ID when ready:   ",
				reel);
			fgets(vol, 10, Devtty);
			vol[strlen(vol) - 1] = '\0';
			if(*dev)
				Tape_nm = dev;
			if(Totape) {
				fso = open(Tape_nm, 0);
				if(!hdrck(fso, vol))
					goto again;
				Tape_hdr.t_reel = reel;
				Tape_hdr.t_reels = Reels;
				Tape_hdr.t_time = tvec;
				close(fso); open(Tape_nm, 1);
				if(write(fso, &Tape_hdr, 512) < 0) {
 				 fprintf(stderr, "Cannot re-write header %s\n",
					"Try again!");
				  goto again;
				}
			} else {
				fsi = open(Tape_nm, 0);
				if(!hdrck(fsi, vol))
					goto again;
				if(Tape_hdr.t_reel != reel) {
				  fprintf(stderr, "Wrong reel-try again!");
				  goto again;
				}
			}
		}
		if(read(fsi, Buf, 512*Nblocks)!=512*Nblocks) {
			printf("Read error block %ld...\n", Block);
			for(i=0;i!=Nblocks*256;++i) Buf[i] = 0;
			if(!Fromtape)
				lseek(fsi, (long)((Block+Nblocks)*512), 0);
		}
		if(write(fso, Buf, 512*Nblocks) != 512*Nblocks) {
			if(Totape && asks("Write error--want to try another tape?   ")) {
				asks("Type `y' when ready:   ");
				--reel;
				Block = reeloff;
				Fs = saveFs;
				lseek(fsi, (long)reeloff*512, 0);
				close(fso);
				goto newtape;
			} else {
				printf("Write error block %ld...\n", Block);
				exit(9);
			}
		}
		Block += Nblocks;
		rblock += Nblocks;
	}
	printf("END: %ld blocks.\n", Block);

	if(access("/etc/log/filesave", 6) == -1) {
		fprintf(stderr, "volcopy: cannot access the log file </etc/log/filesave>\n");
		exit(0);
	}
	fb = popen("tail -200 /etc/log/filesave>Junk;mv Junk /etc/log/filesave", "r");
	pclose(fb);
	fb = popen("cat>>/etc/log/filesave", "w");
	time(&tvec);
	fputs(ctime(&tvec), fb);
	fprintf(fb, "%s;%.6s;%.6s -> %s;%.6s;%.6s\n", argv[DEV_FROM],
	 Superi.s_fname, Superi.s_fpack, argv[DEV_TO], Supero.s_fname,
	 Supero.s_fpack);
	pclose(fb);
	exit(0);
}

err(s) char *s; {
	printf("%s\n",s);
	exit(9);
}
EQ(s1, s2, ct)
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
ask() {
	char ans[12];
	printf("Type `y' to override:     ");
	fgets(ans, 10, Devtty);
	if(EQ(ans,"y", 1))
		return;
	exit(9);
}
asks(s)
char *s;
{
	char ans[12];
	printf(s);
	fgets(ans, 10, Devtty);
	return EQ(ans, "y", 1);
}
char *tapeck(dev, vol, fd)
char *dev, *vol;
{
	char	resp[16];

	if(!IFTAPE(dev))
		return 0;
	Tape_nm = dev;
	alarm(5);
	read(fd, &Tape_hdr, 512);
	if(!EQ(Tape_hdr.t_magic, "Volcopy", 7))
		err("Not a labeled tape");
	signal(SIGALRM, SIG_IGN);
	if(!EQ(Tape_hdr.t_volume, vol, 6)) {
		fprintf(stderr, "Header volume(%.6s) does not match %s\n",
			Tape_hdr.t_volume, vol);
		err("");
	}
tapein:
	if(Reelsize == 0) {
		printf("Enter size of reel in feet for <%s>:   ", vol);
		fgets(resp, 10, Devtty);
		Reelsize = atoi(resp);
	}
	if(Reelsize <= 0 || Reelsize > 2400) {
		fprintf(stderr, "Size of reel must be > 0, <= 2400--try again\n");
		goto tapein;
	}
	if(!Bpi) {
		printf("Tape density? (i.e., 800 | 1600 | 6250)?   ");
		fgets(resp, 10, Devtty);
		Bpi = atoi(resp);
	}
	if(Bpi == 800)
		Reelblks = Ft800x10 * Reelsize;
	else if(Bpi == 1600)
		Reelblks = Ft1600x10 * Reelsize;
	else if(Bpi == 6250)
		Reelblks = Ft6250x88 * Reelsize;
	else {
		fprintf(stderr, "Bpi must be 800, 1600, or 6250--try again\n");
		Bpi = 0;
		goto tapein;
	}
	return dev;
}
hdrck(fd, id)
char *id;
{
	if(read(fd, &Tape_hdr, 512) != 512) {
		close(fd);
		fprintf(stderr, "Cannot read header\n");
		if(asks("Want Shell?    "))
			system("sh");
		return 0;
	}
	if(!EQ(Tape_hdr.t_volume, id, 6)) {
		fprintf(stderr, "Volume is <%.6s>, not <%s>.\n",
			Tape_hdr.t_volume, id);
		if(Totape) {
			if(asks("Want to override?   ")) {
				strncpy(Tape_hdr.t_volume, id, 6);
				return 1;
			}
		}
		return 0;
	}
	return 1;
}
