/*	du	COMPILE:	cc -O du.c -s -i -o du	*/

/*
**	du -- summarize disk usage
**		du [-ars] [name ...]
*/

#include	<stdio.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<sys/dir.h>

#define EQ(x,y)	(strcmp(x,y)==0)
#define ML	500
#define DIRECT	10	/* Number of direct blocks */
#ifdef STOCKIII
#define INDIR	128	/* Number of pointers in an indirect block */
#define INSHFT	7	/* Number of right shifts to divide by INDIR */
#else
#define INDIR	256	/* Number of pointers in an indirect block */
#define INSHFT	8	/* Number of right shifts to divide by INDIR */
#endif

struct 	{
	dev_t	dev;
	ino_t	ino;
} ml[ML];
int	linkc = 0;

struct	stat	Statb;

char	path[256];

int	aflag = 0;
int	rflag = 0;
int	sflag = 0;
long	descend();

main(argc, argv)
char **argv;
{
	long blocks = 0;

#ifdef STANDALONE
	if (argv[0][0] == '\0')
		argc = getargv("du", &argv, 0);
#endif
	if(--argc && argv[1][0] == '-' && argv[1][1] != '\0') {
		argv++;
		while(*++*argv)
			switch(**argv) {
			case 'a':
				aflag++;
				continue;

			case 'r':
				rflag++;
				continue;

			case 's':
				sflag++;
				continue;

			default:
				fprintf(stderr, "usage: du [-ars] [name ...]\n");
				exit(2);
			}
		argc--;
	}
	if(argc == 0) {
		argc = 1;
		argv[1] = ".";
	}
	while(argc--) {
		strcpy(path, *++argv);
		blocks = descend(path);
		if(sflag)
			printf("%ld	%s\n", blocks, path);
	}

	exit(0);
}

long descend(name)
char *name;
{
	register struct	direct	*dp;
	register char	*c1, *c2;
	long blocks = 0;
	struct	direct	dentry[32];
	int	dir = 0;		/* open directory */
	long	offset, dirsize;
	int	dsize, entries, i;
	char	*endofname;
	long nblock();

	if(stat(name,&Statb)<0) {
		if(rflag)
			fprintf(stderr, "du: bad status < %s >\n", name);
		return(0);
	}
	if(Statb.st_nlink>1 && (Statb.st_mode&S_IFMT)!=S_IFDIR && linkc<ML) {
		for(i = 0; i <= linkc; ++i) {
			if(ml[i].ino==Statb.st_ino && ml[i].dev==Statb.st_dev)
				return 0;
		}
		ml[linkc].dev = Statb.st_dev;
		ml[linkc].ino = Statb.st_ino;
		++linkc;
	}
	blocks = nblock(Statb.st_size);

	if((Statb.st_mode&S_IFMT)!=S_IFDIR) {
		if(aflag)
			printf("%ld	%s\n", blocks, name);
		return(blocks);
	}

	for(c1 = name; *c1; ++c1);
	endofname = c1;
	if(Statb.st_size > 32000)
		fprintf(stderr, "Huge directory < %s >--call administrator\n", name);
	dirsize = Statb.st_size;
	for(offset=0 ; offset < dirsize ; offset += 512) { /* each block */
		dsize = 512<(dirsize-offset)? 512: (dirsize-offset);
		if(!dir) {
			if((dir=open(name,0))<0) {
				if(rflag)
					fprintf(stderr, "du: cannot open < %s >\n", name);
				return(0);
			}
			if(offset)
				lseek(dir, (long)offset, 0);
			if(read(dir, dentry, dsize)<0) {
				if(rflag)
					fprintf(stderr, "du: cannot read < %s >\n", name);
				close(dir);
				return(0);
			}
			if(dir > 10) {
				close(dir);
				dir = 0;
			}
		} else 
			if(read(dir, dentry,dsize)<0) {
				if(rflag)
					fprintf(stderr, "du: cannot read < %s >\n", name);
				close(dir);
				return(0);
			}
		for(dp=dentry, entries=dsize>>4; entries; --entries, ++dp) {
			/* each directory entry */
			if(dp->d_ino==0
			|| EQ(dp->d_name, ".")
			|| EQ(dp->d_name, ".."))
				continue;
			if (dp->d_ino == -1)
				continue;
			c1 = endofname;
			*c1++ = '/';
			c2 = dp->d_name;
			for(i=0; i<DIRSIZ; ++i)
				if(*c2)
					*c1++ = *c2++;
				else
					break;
			*c1 = '\0';
			if(c1 == endofname) /* ?? */
				return(0);
			blocks += descend(name);
		}
	}
	if(dir)
		close(dir);
	*endofname = '\0';
	if(!sflag)
		printf("%ld	%s\n", blocks, name);
	return(blocks);
}

long nblock(size)
long size;
{
	long blocks, tot;

#ifdef STOCKIII
	blocks = tot = (size + 511) >> 9;
#else
	blocks = tot = (size + 1023) >> 10;
#endif
	if(blocks > DIRECT)
		tot += ((blocks - DIRECT - 1) >> INSHFT) + 1;
	if(blocks > DIRECT + INDIR)
		tot += ((blocks - DIRECT - INDIR - 1) >> (INSHFT * 2)) + 1;
	if(blocks > DIRECT + INDIR + INDIR*INDIR)
		tot++;
	return(tot);
}
