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
/*	copytape [-rwv] [-p nfiles] [-f filenum] srcfile dstfile	*/
/*									*/
/* 	where -r is for read, -w is for write (default), -v specifes 	*/
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
int  nfiles;			/* Count of files on tape to be read */
int  filenum;			/* Specifies only one file is to be accessed */
int  mode, vflag, pflag, fflag;	/* Flags denoting options selected */
extern char *optarg;
extern int optind;
char *usage =
	"copytape [ -rwv ] [ -p nfiles] [ -f filenum ]\n\t\tsrcfile dstfile";



main(argc, argv)
char **argv;
{
	char c;

	if (argc < 3)
		fatal(usage,"usage");

	while ((c = getopt(argc,argv,"rwvp:f:i:o:")) != EOF) {
	    switch(c) {
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

		case 'v':		/* Handle variable sized blocks */
			vflag++;
			break;

		}
	}

	if ((!srcfile) && (optind+1 <= argc))
		srcfile = argv[optind++];
	if ((!dstfile) && (optind+1 <= argc))
		dstfile = argv[optind++];

	if ((srcid = open(srcfile,O_RDONLY)) == -1)
		fatal("could not open srcfile",srcfile);
	if ((dstid = open(dstfile,O_WRONLY|O_CREAT,0664)) == -1)
		fatal("could not open dstfile",dstfile);


	if (pflag)
		imsccopy();
	else if (mode == 'r')
		readtape();
	else 
		writetape();
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
		if ((total == 0) && !vflag)
			readcnt = cnt;
		/* If we hit one EOF on tape, put out a -1, */
		/*   if we hit two EOF's on tape, we are all done */
		if (cnt == 0) {
			if (total)
				fprintf(stdout,"%ld	%d\n",total,lastcnt);
			fprintf(stdout,"%d\n",-1);
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
		total++;
		if ((cnt != lastcnt) && (lastcnt)) {
			if (!vflag)
			    fatal("tape file contains variable size blocks",
					srcfile);
			fprintf(stdout,"%ld %d\n",total,cnt);
			lastcnt = cnt;
			total = 0;
		}
		lastcnt = cnt;
		if (fflag && (filenum != totalfiles))
			continue;
		if (cnt != write(dstid,buf,cnt))
			fatal("could not write",dstfile);
	}
	fprintf(stderr,"%d files read from tape\n",totalfiles);
}


writetape() {
	int argcnt;
	int cnt;
	int eof;
	long total;
	int totalfiles;

	totalfiles = eof = 0;
	while (1) {
		argcnt = fscanf(stdin,"%ld",&total);
		if (argcnt == 0)
			fatal("missing EOF marker in description file","stdin");

		if (total != -1) {
			argcnt = fscanf(stdin,"%d",&cnt);
			if (argcnt == 0)
				fatal("invalid description file","stdin");
		}
		else {
			if (eof++)
				break;
			if (!fflag || (totalfiles == filenum)) {
				close(dstid);
				dstid = open(dstfile,"w");
			}
			if (fflag && (filenum == totalfiles)) {
			    printf("file %d copied successfully\n",filenum);
			    exit(0);
			}
			totalfiles++;
			continue;
		}

		eof = 0;
		if (fflag && (totalfiles != filenum))
			continue;
		while (total--) {
			if (cnt != read(srcid,buf,cnt))
				fatal("could not read",srcfile);
			write(dstid,buf,cnt);
		}
	}
	fprintf("%d files written to tape\n",totalfiles);
}


fatal(mesg,arg)
char *mesg, *arg;
{
	fprintf(stderr,"copytape:%s: %s\n",arg,mesg);
	exit(1);
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
			if (ioctl(srcid,C_RECALL,&ptcmd) != 0) {
				fatal("ioctl","C_RECALL error");
				}
			fprintf(stdout,"%ld	%d\n",ptcmd.blkcnt,512);
			totalfiles++;
			blkno += ptcmd.blkcnt;
			fprintf(stdout,"%d\n",-1);
			}
		fprintf(stdout,"%d\n",-1);
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
		fscanf(stdin,"%ld",&ptcmd.blkcnt);
		while (ptcmd.blkcnt != -1) {
			fscanf(stdin,"%d",&bsize);
			if (bsize % 512) {
				fatal("blocksize","not 512 multiple");
				}
			ptcmd.blkcnt *= bsize/512L;
			ptcmd.blkno = blkno;
			if (ioctl(dstid,C_SAVE,&ptcmd) != 0) {
				fatal("ioctl","C_SAVE error");
				}
			blkno += ptcmd.blkcnt;
			totalfiles++;
			fscanf(stdin,"%ld",&ptcmd.blkcnt);
			if (ptcmd.blkcnt == -1) {
				if (ioctl(dstid,C_WEOF,&ptcmd) != 0) {
					fatal("ioctl","C_WEOF error");
					}
				fscanf(stdin,"%ld",&ptcmd.blkcnt);
				}
			}
		fprintf(stderr,"%d files written to tape\n",totalfiles);
		break;
	default:
		fatal("illegal","mode not r or w");
	}
}
