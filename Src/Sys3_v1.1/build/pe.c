/*	pe [e]
 *		reads standard input for output of find
 *		prints a proto file on standard output.
 *	BUG
 *		Does not put the final few "$" to terminate the last
 *		few directories.  Count number of dirs and number of $
 *		and add the deficient amount to end of output file.
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

char type[] = "  c d b -      ";
char suid[] = "-u";
char sgid[] = "-g";
char tabs[] ="                     ";
char *endtabs = tabs;
char fname[16];
char dir[100];		/* current file's directory.  if cur file is directory,
			   it is part of dir */
char ldir[100];		/* last file's directory */
int dirlevel;
int ldirlevel;
int startlevel;
int commonlevel;
int notfirst;
int eflag;		/* non zero prints error messages */
char * rindex();

main(argc, argv)
int argc;
char **argv;
{
	char name[256];
	char *p;
	int i;
	
	for (i = 1; i < argc; i++) 
		for(p = argv[i]; *p; p++) switch (*p) {
		case 'e':
			fprintf(stderr,"eflag set\n");
			eflag++;
			break;
		}

	argc--;
	argv++;
	while (1) {
		if (gets(name) == NULL) {
			break;
		}
		printent(name);
	}
}


/*
 * prints out the entry for a file in a format compatible with
 * mkfs.
 */

printent(s)
char *s;
{
	struct stat buf;
	char *p;
	char *d;
	int i;

	if (stat(s, &buf) < 0) {
		if(eflag) fprintf(stderr,"cannot stat %s\n",s);
		return(1);
	}
	if ((buf.st_mode & S_IFMT) == S_IFDIR) {
		p = rindex(s, '/');
		strcpy(dir, s);
		strcat(dir, "/");
		strcpy(fname, p+1);
	} else {
		p = rindex(s, '/');
		strncpy(dir, s, p - s + 1);
		dir[p - s + 1] = '\0';
		strcpy(fname, p+1);
	}
	if (notfirst) {
		d = dir;
		p = ldir;
		commonlevel = 0;
		while ((*p == *d) && (*p != '\0')) {
			commonlevel += (*p == '/');
			p++;
			d++;
		}
		dirlevel = cnt(dir);
		ldirlevel = cnt(ldir);
	}
	else startlevel=dirlevel=cnt(dir);
	strcpy(ldir, dir);
	notfirst = 1;
	for (i = 0; i < (ldirlevel - commonlevel); i++) {
		printf("%s$\n",tabs);
		*endtabs-- = ' ';
		*endtabs = 0;
	}
	*endtabs = ' ';
	endtabs = tabs+dirlevel-startlevel;
	*endtabs = 0;
	if ((buf.st_mode & S_IFMT) == S_IFDIR)
		printf("%s%s\t",tabs, fname);
	else
		printf(" %s%s\t",tabs, fname);
	if (strlen(fname) < 8) {
		printf("\t");
	}
	printf("%c%c%c%03o ",
		type[(buf.st_mode & S_IFMT) >> 12],
		suid[(buf.st_mode & S_ISUID) == S_ISUID],
		sgid[(buf.st_mode & S_ISGID) == S_ISGID],
		buf.st_mode & 0777);
	printf("%3d %3d", buf.st_uid, buf.st_gid);
	if ((buf.st_mode & S_IFMT) == S_IFDIR) {
		printf("\n");
	} else if (((buf.st_mode & S_IFMT) == S_IFCHR) ||
		   ((buf.st_mode & S_IFMT) == S_IFBLK)) {
		   printf("%3d %3d\n",major(buf.st_rdev),
			  minor(buf.st_rdev));
	} else {
		printf(" %s\n", s);
	}
}


cnt(s)
register char *s;
{
	register cn;

	cn = 0;
	while (*s) {
		cn += (*s == '/');
		s++;
	}
	return(cn);
}
