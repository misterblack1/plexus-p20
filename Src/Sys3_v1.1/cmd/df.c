#include <stdio.h>
#include <sys/param.h>
#include <mnttab.h>
#include <ustat.h>
#include <sys/stat.h>
#include <sys/filsys.h>
#include <sys/fblk.h>
#define EQ(x,y,z) (strncmp(x,y,z)==0)
struct	mnttab	M[NMOUNT];
struct stat	S;
struct filsys	sblock;
struct ustat	Fs_info, *Fs;
int	Flg, tflag;
int	fd;
daddr_t	blkno	= 1;
daddr_t	alloc();

main(argc, argv)
char **argv;
{
	register fi, i;
	register char	c;
	char	 dev[32];
	int	 j;

	while(argv[1][0] == '-') {
		switch(c = argv[1][1]) {
			case 'f':
				Flg++;
				break;

			case 'q':
				break;

			case 't':
				tflag=1;
				break;

			default:
				fprintf(stderr,"df: illegal arg -%c\n",c);
				exit(1);
		}
		argc--;
		argv++;
	}
	if((fi = open("/etc/mnttab", 0)) < 0) {
		fprintf(stderr,"df: cannot open /etc/mnttab\n");
		exit(1);
	}
	i = read(fi, &M, sizeof M);
	close(fi);

	for(i /= sizeof M[0]; --i >= 0;) {
		if(!M[i].mt_dev[0]) /* it's been umount'ed */
			continue;
		sprintf(dev, "/dev/%.8s", M[i].mt_dev);
		if(argc > 1) {
			for(j = 1; j < argc; ++j) {
				if(EQ(argv[j], dev, 14)
				|| EQ(argv[j], M[i].mt_dev, 8)
				|| EQ(argv[j], M[i].mt_filsys, 8)) {
					printit(dev, M[i].mt_filsys);
					argv[j][0] = '\0';
				}
			}
		} else
			printit(dev, M[i].mt_filsys);
	}
	for(i = 1; i < argc; ++i) {
		if(argv[i][0])
			printit(argv[i], "\0");
	}
	exit(0);
}

printit(dev, fs_name)
char *dev, *fs_name;
{
	if((fd = open(dev, 0)) < 0) {
		fprintf(stderr,"df: cannot open %s\n",dev);
		return;
	}
	if(!Flg) {
		Fs = &Fs_info;
		if(stat(dev, &S) < 0) {
bad_dev:
			fprintf(stderr,"df: cannot stat %s\n",dev);
			return;
		}
		if((S.st_mode & S_IFMT) != S_IFBLK)
			goto bad_dev;
		if(ustat(S.st_rdev, Fs) < 0 || tflag) {
			lseek(fd, (long)BSIZE, 0);
			read(fd, &sblock, sizeof sblock);
			Fs = (struct ustat *)&sblock.s_tfree;
		}
		printf("%-8s(%-10s): %8ld blocks%8u i-nodes\n",fs_name,
			dev, Fs->f_tfree, Fs->f_tinode);
		if(tflag)
			printf("                     (%8ld total blocks,%5d for i-nodes)\n",
				sblock.s_fsize, sblock.s_isize);
	}
	else {
		daddr_t	i;

		sync();
		bread(1L, (char *) &sblock, sizeof(sblock));
		i = 0;
		while(alloc())
			i++;
		printf("%-8s(%-10s): %8ld blocks\n",fs_name, dev, i);
	}
	close(fd);
}

daddr_t
alloc()
{
	int i;
	daddr_t	b;
	struct fblk buf;

	i = --sblock.s_nfree;
	if(i<0 || i>=NICFREE) {
		printf("bad free count, b=%ld\n", blkno);
		return(0);
	}
	b = sblock.s_free[i];
	if(b == 0)
		return(0);
	if(b<sblock.s_isize || b>=sblock.s_fsize) {
		printf("bad free block (%ld)\n", b);
		return(0);
	}
	if(sblock.s_nfree <= 0) {
		bread(b, &buf, sizeof(buf));
		blkno = b;
		sblock.s_nfree = buf.df_nfree;
		for(i=0; i<NICFREE; i++)
			sblock.s_free[i] = buf.df_free[i];
	}
	return(b);
}

bread(bno, buf, cnt)
daddr_t bno;
char *buf;
{
	int n;
	extern errno;

	lseek(fd, bno<<BSHIFT, 0);
	if((n=read(fd, buf, cnt)) != cnt) {
		printf("read error %ld\n", bno);
		printf("count = %d; errno = %d\n", n, errno);
		exit(0);
	}
}
