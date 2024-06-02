/*
**	rm [-fir] file ...
*/

#include	<stdio.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<sys/dir.h>

int	errcode;

main(argc, argv)
char *argv[];
{
	register char *arg;
	int	fflg, iflg, rflg;

	fflg = 0;
	iflg = 0;
	rflg = 0;
	if(argc>1 && argv[1][0]=='-') {
		arg = *++argv;
		argc--;
		while(*++arg != '\0')
			switch(*arg) {
			case 'f':
				fflg++;
				break;
			case 'i':
				iflg++;
				break;
			case 'r':
				rflg++;
				break;
			default:
				fprintf(stderr, "usage: rm [-fir] file ...\n");
				exit(2);
			}
	}
	while(--argc > 0) {
		if(!strcmp(*++argv, "..")) {
			fprintf(stderr, "rm: cannot remove ..\n");
			continue;
		}
		rm(*argv, fflg, rflg, iflg);
	}

	exit(errcode?2:0);
}

rm(arg, fflg, rflg, iflg)
char arg[];
{
	struct	stat	buf;
	struct	direct	direct;
	char	name[100];
	int	d;

	if(stat(arg, &buf)) {
		if (fflg==0) {
			fprintf(stderr, "rm: %s non-existent\n", arg);
			++errcode;
		}
		return;
	}
	if ((buf.st_mode&S_IFMT) == S_IFDIR) {
		if(rflg) {
			if(iflg && !dotname(arg)) {
				printf("directory %s: ", arg);
				if(!yes())
					return;
			}
			if((d=open(arg, 0)) < 0) {
				fprintf(stderr, "rm : cannot read %s\n", arg);
				exit(2);
			}
			while(read(d, &direct, sizeof(direct)) == sizeof(direct)) {
				if(direct.d_ino != 0 && !dotname(direct.d_name)) {
					sprintf(name, "%s/%.14s", arg, direct.d_name);
					rm(name, fflg, rflg, iflg);
				}
			}
			close(d);
			errcode += rmdir(arg, iflg);
			return;
		}
		fprintf(stderr, "rm: %s directory\n", arg);
		++errcode;
		return;
	}

	if(iflg) {
		printf("%s: ", arg);
		if(!yes())
			return;
	}
	else if(!fflg) {
		if(access(arg, 02) < 0 && isatty(fileno(stdin))) {
			printf("%s: %o mode ", arg, buf.st_mode&0777);
			if(!yes())
				return;
		}
	}
	if(unlink(arg) && (fflg==0 || iflg)) {
		fprintf(stderr, "rm: %s not removed\n", arg);
		++errcode;
	}
}

dotname(s)
char *s;
{
	if(s[0] == '.')
		if(s[1] == '.')
			if(s[2] == '\0')
				return(1);
			else
				return(0);
		else if(s[1] == '\0')
			return(1);
	return(0);
}

rmdir(f, iflg)
char *f;
{
	int	status, i;

	if(dotname(f))
		return(0);
	if(iflg) {
		printf("%s: ", f);
		if(!yes())
			return(0);
	}
	while((i=fork()) == -1)
		sleep(3);
	if(i) {
		wait(&status);
		return(status);
	}
	execl("/bin/rmdir", "rmdir", f, 0);
	printf("No rmdir\n");
	exit(2);
}

yes()
{
	int	i, b;

	i = b = getchar();
	while(b != '\n' && b > 0)
		b = getchar();
	return(i == 'y');
}
