#include <sys/param.h>
#include <sys/plexus.h>
#include <sys/disk.h>

/*
 * Standalone dconfig.  Used to configure block 0 of a disk drive.
 */

extern char *copystr();

struct block0 block0;
char dbuf[50];
char buf[50];
char *bptr;
int infile;
long maxblks;
short ignoreblock0 = 1;	/* don't use initialization info in block 0 */

main(argc,argv)
int argc;
char **argv;
{
	register int i ,j;

	extern long atol();
#ifdef STANDALONE
	argc = getargv("dconfig",&argv,0);
#endif
	do {
		if(argc <= 1) {
			argv[1] = dbuf;
			printf("Disk? : ");
			gets(dbuf);
		}
		else
			argc = 0;
		infile = open(argv[1],2);
	} while (infile < 0);

	lseek(infile, 0L, 0);
#ifndef STANDALONE
	if(read(infile, &block0, sizeof block0) != sizeof block0) {
		perror("dconfig");
		exit(1);
	}
#else
	read(infile, &block0, sizeof block0);
#endif
	if (block0.id != 'dk' && block0.id != 'pd' && block0.id != 'is')
		for (bptr = (char *)&block0, i=0; i<sizeof block0; i++)
			*bptr++;

	do {		
		printf("Disk id? [%c%c]: ", block0.id>>8, block0.id & 0xff);
		gets(buf);
		if (*buf)
			block0.id = *(unsigned *)buf;

		printf("Primary bootname? [%s]: ", block0.bootname);
		gets(buf);
		if (*buf) {
			bptr = copystr(block0.bootname, buf);
			*bptr = 0;
		}
		else {
			bptr = block0.bootname;
			while (*bptr++);
		}
		printf("Secondary bootname? [%s]: ", bptr);
		gets(buf);
		if (*buf)
			bptr = copystr(bptr, buf);
		else
			while (*bptr++);
		
		while (bptr < &block0.bootname[sizeof(block0.bootname)])
			*bptr++ = 0;
		
		printf("Number of cylinders? [%d]: ", block0.init.cylinders);
		gets(buf);
		if (*buf)
			block0.init.cylinders = atol(buf);

		printf("Number of heads on removable? [%d]: ",
			block0.init.remheads);
		gets(buf);
		if (*buf)
			block0.init.remheads = atol(buf);

		printf("Number of heads on fixed? [%d]: ",
			block0.init.fixheads);
		gets(buf);
		if (*buf)
			block0.init.fixheads = atol(buf);

		printf("Data bytes per sector? [%d]: ",
			(block0.init.msb_sectsize << 8) +
			(block0.init.lsb_sectsize & 0xff));
		gets(buf);
		if (*buf) {
			block0.init.msb_sectsize = atol(buf) >> 8;
			block0.init.lsb_sectsize = atol(buf) & 0xff;
		}

		printf("Sectors per track? [%d]: ", block0.init.sectpertrack);
		gets(buf);
		if (*buf) 
			block0.init.sectpertrack = atol(buf);
			
		printf("Number of alternate cylinders? [%d]: ",
			block0.init.altcylinders);
		gets(buf);
		if (*buf)
			block0.init.altcylinders = atol(buf);

		printf("File system blocksize? [%d]: ", block0.fsbsize);
		gets(buf);
		if (*buf)
			block0.fsbsize = atol(buf);

		printf("Sys3 nodename? [%s]: ", block0.nodenm);
		gets(buf);
		if (buf[0] != 0) {
			for (i=0; i<9; i++) {
				block0.nodenm[i] = buf[i];
				if (buf[i] == 0)
					break;
			}
			for (; i<9; i++)
				block0.nodenm[i] = 0;
		}

		printf("Change the ");
		printf("default unix device mapping? [y/n]: ");
		gets(buf);
		if ((*buf == 'y') || (*buf == 'Y')) {
		    printf("Rootdev? [0x%x]: ",block0.rootdev);
		    gets(buf);
		    if (*buf)
			block0.rootdev = atol(buf);
		    printf("Pipedev? [0x%x]: ",block0.pipedev);
		    gets(buf);
		    if (*buf)
			block0.pipedev = atol(buf);
		    printf("Dumpdev? [0x%x]: ",block0.dumpdev);
		    gets(buf);
		    if (*buf)
			block0.dumpdev = atol(buf);
		    printf("Swapdev? [0x%x]: ",block0.swapdev);
		    gets(buf);
		    if (*buf)
			block0.swapdev = atol(buf);
		    printf("Swplo? [%ld]: ",block0.swplo);
		    gets(buf);
		    if (*buf)
			block0.swplo = atol(buf);
		    printf("Nswap? [%d]: ",block0.nswap);
		    gets(buf);
		    if (*buf)
			block0.nswap = atol(buf);
		}

		printf("Change the ");
		printf("file system disk configuration? [y/n]: ");
		gets(buf);
		if ((*buf == 'y') || (*buf == 'Y')) {
		    printf("File system logical configuration?");
		    printf(" [sector start,sector count]\n");
		    for (i=0;i<16;i++){
			printf("\tdk%d\t[\t%ld,\t%ld\t]: ", i,
				block0.confinfo[i].blkoff,
				block0.confinfo[i].nblks);
			gets(buf);
			if ((*buf == 'q') || (*buf == 'Q'))
				break;
			if (*buf) {
				block0.confinfo[i].blkoff = atol(buf);
				for (j=0; buf[j]!=',' && j<sizeof(buf); j++);
				maxblks = (long) block0.init.fixheads
					* (long) block0.init.sectpertrack
					* (long) (block0.init.cylinders -
					   block0.init.altcylinders);
				if (buf[j+1] == '~')
					block0.confinfo[i].nblks = maxblks 
						- block0.confinfo[i].blkoff;
				else
					block0.confinfo[i].nblks
						= atol(&buf[j+1]);
			}
		    }	
		    for (; i<16; i++) {
			block0.confinfo[i].blkoff = 0;
			block0.confinfo[i].nblks = 0;
		    }
		    for (i=0; i<16; i++)
			if ((block0.confinfo[i].blkoff != 0)
				|| (block0.confinfo[i].nblks != 0)) break;
		    if (i < 16)
		    	block0.initsize = 16 * sizeof(struct diskconf);
		    else
			block0.initsize = 0;
		}
				
		printf("Is the above information correct? [y/n]: ");
		gets(buf);
	} while ((*buf != 'y') && (*buf != 'Y'));	
	printf("Are you sure you want to rewrite block 0? [y/n]: ");
	gets(buf);
	if ((*buf != 'y') && (*buf != 'Y'))
		exit(1);
	lseek(infile, 0L, 0);
	if(write(infile, &block0, sizeof block0) != sizeof block0)
		exit(2);
	printf("Block 0 of %s initialized successfully!\n",argv[1]);
	exit(0);
}

char *copystr(tobuf, frombuf)
char *tobuf, *frombuf;
{
	while (*tobuf++ = *frombuf++);
	return(tobuf);
}

#ifndef STANDALONE
gets(buf)
char	*buf;
{
register char *lp;
register c;

	lp = buf;
	for (;;) {
		c = getchar() & 0177;
		if (c>='A' && c<='Z')
			c -= 'A' - 'a';
		if (lp != buf && *(lp-1) == '\\') {
			lp--;
			if (c>='a' && c<='z') {
				c += 'A' - 'a';
				goto store;
			}
			switch ( c) {
			case '(':
				c = '{';
				break;
			case ')':
				c = '}';
				break;
			case '!':
				c = '|';
				break;
			case '^':
				c = '~';
				break;
			case '\'':
				c = '`';
				break;
			}
		}
	store:
		switch(c) {
		case '\n':
		case '\r':
			c = '\n';
			*lp++ = '\0';
			return;
		case '\b':
		case '#':
			lp--;
			if (lp < buf)
				lp = buf;
			continue;
		case '@':
			lp = buf;
			putchar('\n');
			continue;
		default:
			*lp++ = c;
		}
	}
}
long
atol(p)
register char *p;
{
	long n;
	register int f;
	register int base;

	n = 0;
	f = 0;
	base = 10;
	for(;;p++) {
		switch(*p) {
		case ' ':
		case '\t':
			continue;
		case '0':
			base = 8;
			continue;
		case 'x':
		case 'X':
			if (base == 8) {
				base = 16;
				p++;
			}
			break;
		case '-':
			f++;
		case '+':
			p++;
		}
		break;
	}
	for(;;) {
		switch (*p) {
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			n = n*base + *p++ - '0';
			continue;
		case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
			if (base <= 10)
				break;
			n = n*base + *p++ - 'a' + 10;
			continue;
		case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
			if (base <= 10)
				break;
			n = n*base + *p++ - 'A' + 10;
			continue;
		}
		break;
	}
	return(f? -n: n);
}
#endif
