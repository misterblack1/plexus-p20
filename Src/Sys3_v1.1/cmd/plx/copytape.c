/************************************************************************/
/*									*/
/*			C O P Y T A P E					*/
/*									*/
/* This program can be used to make duplicates of magnetic tapes.	*/
/*	It reads tapes into a file (normally a raw disk device) and	*/
/*	creates a descriptor file that describes the format.  It can	*/
/*	also read from a file and write to tape given a descriptor file.*/
/*	The argument -p followed by a file count does a special copy	*/
/*	from the imsc tape to disk or disk to tape at high speed.	*/
/*									*/
/* Usage is as follows:							*/
/*	copytape [-crwv] [-p nfiles] [-f filenum] [-i] srcfile 		*/
/*		[-o] dstfile						*/
/*									*/
/* 	where -c is for direct copy from tape to tape, -r		*/
/*	is for read, -w is for write (default), -v specifes 		*/
/* 	that the the tape contains variable size records within files,	*/ 
/*	-p specifies the imsc is being used, filenum specifies that only*/
/*	the file specifed is to be processed.				*/
/*									*/
/* To compile this program,						*/
/*	cc -o copytape -O copytape.c					*/
/*									*/
/************************************************************************/

#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/imsc.h>

char buf[0x4000];		/* Big buffer for data */
char *srcfile;			/* Source file name */
char *dstfile;			/* Destination file name */
int  srcid;			/* Source file descriptor */
int  dstid;			/* Destination file descriptor */
char *descfile;			/* Description file name */
int  descid;			/* Description file descriptor */
FILE *descstr;			/* Stream for descriptor file */
int  nfiles;			/* Count of files on tape to be read */
int  filenum;			/* Specifies only one file is to be accessed */
int  mode, vflag, pflag, fflag;	/* Flags denoting options selected */
int  tapetotape;		/* Flag indicating direct tape to tape copy */
int  syncflag;			/* Flag indicating to write in 512 byte incs */
extern char *optarg;
extern int optind;
char *usage =
	"copytape [ -crwvs ] [ -p nfiles] [ -f filenum ] [ -d descfile ]\n                [ -i ] srcfile [ -o ] dstfile";



main(argc, argv)
char **argv;
{
	char c;

	if (argc < 3)
		fatal(usage,"usage");

	while ((c = getopt(argc,argv,"crwvsp:f:i:o:d:")) != EOF) {
	    switch(c) {
		case 'c':		/* Direct copy */
			tapetotape++;
			break;

		case 'd':		/* Descriptor file */
			descfile = optarg;
			break;

		case 'f':		/* Individual file only */
			fflag++;
			filenum = atoi(optarg);
			break;

		case 'i':		/* Srcfile */
			srcfile = optarg;
			break;

		case 'o':		/* Dstfile */
			dstfile = optarg;
			break;

		case 'r':		/* Read tape */
	    	case 'w':		/* Write tape */
			mode = c;
			break;

		case 'p':		/* Special handling for imsc */
			pflag++;
			nfiles = atoi(optarg);
			break;
		
		case 's':		/* Sync flag, write in 512 byte incs */
			syncflag++;
			break;

		case 'v':		/* Handle variable sized blocks */
			vflag++;
			break;

		}
	}

	descstr = (mode == 'r') ? stdout : stdin;
	if ((descfile) && (*descfile != '-')) {
		if ((descid = open(descfile,O_RDWR)) == -1)
			fatal("could not open descriptor file",descfile);
		descstr = fdopen(descid,"r+");
	}

	if ((!srcfile) && (optind+1 <= argc))
		srcfile = argv[optind++];
	if ((!dstfile) && (optind+1 <= argc))
		dstfile = argv[optind++];

	if (*srcfile == '-')		/* Get from standard input */
		srcid = 0;
	else if ((srcid = open(srcfile,O_RDONLY)) == -1)
		fatal("could not open source file",srcfile);
	if (*dstfile == '-')		/* Go to standard output */
		dstid = 1;
	else if ((dstid = open(dstfile,O_WRONLY|O_CREAT,0664)) == -1)
		fatal("could not open destination file",dstfile);


	if (pflag)
		imsccopy();
	else if (tapetotape)
		copytape();
	else if (mode == 'r')
		readtape();
	else 
		writetape();
}


copytape() {
int cnt;
int lastcnt;
int readcnt;
int totalfiles;
long total;
int eof;

	totalfiles = lastcnt = eof = total = 0;
	readcnt = sizeof buf;
	while (1) {
		cnt = read(srcid,buf,readcnt);
		if (syncflag)
			cnt = ((cnt+511)/512)*512;
		if ((total == 0) && !vflag)
			readcnt = cnt;
		/* If we hit one EOF on tape, put out an EOF, */
		/*   if we hit two EOF's on tape, we are all done */
		if (cnt == 0) {
			if (eof++) break;
			if (!fflag || (totalfiles == filenum)) {
				close(dstid);
				dstid = open(dstfile,O_WRONLY);
			}
			if (fflag && (filenum == totalfiles)) {
				printf("file %d copied successfully\n",filenum);
				exit(0);
			}
			readcnt = sizeof buf;
			lastcnt = total = 0;
			totalfiles++;
			continue;
		}
		eof = 0;
		if ((cnt != lastcnt) && (lastcnt)) {
			if (!vflag)
			   fatal("file contains variable size blocks (use -v)",
					srcfile,cnt);
			total = 0;
		}
		total++;
		lastcnt = cnt;
		if (fflag && (filenum != totalfiles))
			continue;
		if (cnt != write(dstid,buf,cnt))
			fatal("could not write",dstfile);
	}
	fprintf(stderr,"%d file%s read from and written to tape\n",totalfiles,
		totalfiles!=1?"s":"");
}


readtape() {
int cnt;
int lastcnt;
int readcnt;
int totalfiles;
long total;
int eof;

	totalfiles = lastcnt = eof = total = 0;
	readcnt = sizeof buf;
	while (1) {
		cnt = read(srcid,buf,readcnt);
		if (syncflag)
			cnt = ((cnt+511)/512)*512;
		if ((total == 0) && !vflag)
			readcnt = cnt;
		/* If we hit one EOF on tape, put out a -1, */
		/*   if we hit two EOF's on tape, we are all done */
		if (cnt == 0) {
			if (total)
				fprintf(descstr,"%-6ld %d\n",total,lastcnt);
			fprintf(descstr,"%d\n",-1);
			if (eof++) break;
			if (fflag && (filenum == totalfiles)) {
				printf("file %d copied successfully\n",filenum);
				exit(0);
			}
			readcnt = sizeof buf;
			lastcnt = total = 0;
			totalfiles++;
			continue;
		}
		eof = 0;
		if ((cnt != lastcnt) && (lastcnt)) {
			fprintf(descstr,"%-6ld %d\n",total,lastcnt);
			if (!vflag)
			   fatal("file contains variable size blocks (use -v)",
					srcfile,cnt);
			total = 0;
		}
		total++;
		lastcnt = cnt;
		if (fflag && (filenum != totalfiles))
			continue;
		if (cnt != write(dstid,buf,cnt))
			fatal("could not write",dstfile);
	}
	fprintf(stderr,"%d file%s read from tape\n",totalfiles,
		totalfiles!=1?"s":"");
}

writetape() {
	int argcnt;
	int cnt;
	int eof;
	long total;
	int totalfiles;

	totalfiles = eof = 0;
	while (1) {
		argcnt = fscanf(descstr,"%ld",&total);
		if (argcnt == 0)
			fatal("missing EOF marker in description file","descfile");

		if (total != -1) {
			argcnt = fscanf(descstr,"%d",&cnt);
			if (argcnt == 0)
				fatal("invalid description file","descfile");
		}
		else {
			if (eof++)
				break;
			if (!fflag || (totalfiles == filenum)) {
				close(dstid);
				dstid = open(dstfile,O_WRONLY);
			}
			if (fflag && (filenum == totalfiles)) {
			    printf("file %d copied successfully\n",filenum);
			    exit(0);
			}
			totalfiles++;
			continue;
		}

		eof = 0;
		while (total--) {
			if (cnt != read(srcid,buf,cnt))
				fatal("could not read",srcfile);
			if (fflag && (totalfiles != filenum))
				continue;
			if (syncflag)
				cnt = ((cnt+511)/512)*512;
			write(dstid,buf,cnt);
		}
	}
	fprintf(stderr,"%d files written to tape\n",totalfiles);
}

imsccopy()
{
struct stat srcstat,dststat;
long blkno;
int bsize;
struct ptcmd ptcmd;
int totalfiles;

/*
 * stat the sourcefile and make sure it's a character special file
 */
	if (fstat(srcid,&srcstat) != 0) {
		fatal("can't stat",srcfile);
		}
	if ((srcstat.st_mode & S_IFMT) != S_IFCHR) {
		fatal("not character special file",srcfile);
		}
/*
 * stat the destination file and make sure it's a character special file
 */
	if (fstat(dstid,&dststat) != 0) {
		fatal("can't stat",dstfile);
		}
	if ((dststat.st_mode & S_IFMT) != S_IFCHR) {
		fatal("not character special file",dstfile);
		}
/*
 * perform one time initializations
 */
	totalfiles = 0;
	blkno = 0L;
	switch (mode) {

	case 'r':
		ptcmd.dknum = dststat.st_rdev;
/*
 * now loop through streaming each file from tape into succeeding
 *	blocks on disk. Write the number of blocks read from each
 *	file into the definition file
 */
		while (nfiles--) {
			ptcmd.blkno = blkno;
			ptcmd.blkcnt = 100000L; /* a large number */
			if (ioctl(srcid,C_IRECALL,&ptcmd) != 0) {
				fatal("ioctl","C_IRECALL error");
				}
			fprintf(descstr,"%-6ld %d\n",ptcmd.blkcnt,512);
			totalfiles++;
			blkno += ptcmd.blkcnt;
			fprintf(descstr,"%d\n",-1);
			}
		fprintf(descstr,"%d\n",-1);
		fprintf(stderr,"%d files read from tape\n",totalfiles);
		break;
/*
 * here we are going to read the number of tape file images stored
 *	on disk from the definition file and loop through the
 *	disk images streaming the files out to tape, placing
 *	an EOF between them
 */
	case 'w':
/*
 * first setup the major/minor device numbers for the ioctl call
 *	and get the number of file images from the definition file
 */
		ptcmd.dknum = srcstat.st_rdev;
/*
 * loop through reading the number of blocks in each file image
 *	streaming them to tape, updating the file image block number,
 *	and writing EOF's.
 */
		fscanf(descstr,"%ld",&ptcmd.blkcnt);
		while (ptcmd.blkcnt != -1) {
			fscanf(descstr,"%d",&bsize);
			if (bsize % 512) {
				fatal("blocksize","not 512 multiple");
				}
			ptcmd.blkcnt *= bsize/512L;
			ptcmd.blkno = blkno;
			if (ioctl(dstid,C_ISAVE,&ptcmd) != 0) {
				fatal("ioctl","C_ISAVE error");
				}
			blkno += ptcmd.blkcnt;
			fscanf(descstr,"%ld",&ptcmd.blkcnt);
			if (ptcmd.blkcnt == -1) {
				totalfiles++;
				if (ioctl(dstid,C_IWEOF,&ptcmd) != 0) {
					fatal("ioctl","C_IWEOF error");
					}
				fscanf(descstr,"%ld",&ptcmd.blkcnt);
				}
			}
		fprintf(stderr,"%d files written to tape\n",totalfiles);
		break;
	default:
		fatal("illegal","mode not r or w");
	}
}

fatal(mesg,arg)
char *mesg, *arg;
{
	fprintf(stderr,"copytape: %s: %s\n",arg,mesg);
	exit(1);
}

