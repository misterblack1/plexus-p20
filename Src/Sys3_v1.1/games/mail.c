/*
**	mail [ person ]
**	mail -f file
**	(without Shell escapes, for use with "games", etc.)
*/

#include	<stdio.h>
#include	<pwd.h>
#include	<utmp.h>
#include	<signal.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<setjmp.h>

/*copylet flags */
#define	REMOTE		1		/* remote mail, add rmtmsg */
#define ORDINARY	2
#define ZAP		3		/* zap header and trailing empty line */
#define	LSIZE		256
#define	MAXLET		300		/* maximum number of letters */
#ifndef	MFMODE
#define	MFMODE		0666		/* create mode for `/usr/mail' files */
#endif

struct	let	{
	long	adr;
	char	change;
} let[MAXLET];

struct	passwd	*getpwuid(), getpwent();

char	lettmp[] = "/tmp/maXXXXX";
char	from[] = "From ";
char	maildir[] = "/usr/mail/";
char	mailfile[] = "/usr/mail/xxxxxxxxxxxxxxxxxxxxxxx";
char	maillock[] = ".lock";
char	dead[] = "dead.letter";
char	rmtbuf[] = "/tmp/marXXXXXX";
char	*rmtmsg = " remote from usg\n";
char	*thissys = "usg";
char	mbox[] = "/mbox";
char	curlock[50];
char	line[LSIZE];
char	resp[LSIZE];
char	*hmbox;
char	*home;
char	*my_name;
char	*getlogin();
char	*malloc();
char	lfil[50];
char	*ctime();
char	*getenv();

FILE	*tmpf;
FILE	*malf;
FILE	*rmtf;

int	error;
int	nlet	= 0;
int	locked;
int	changed;
int	forward;
int	delete();
int	flgf;
int	delflg;

long	ftell();
long	iop;

jmp_buf	sjbuf;

unsigned umsave;

main(argc, argv)
char **argv;
{
	register i;

	umsave = umask(0);
	setbuf(stdout, malloc(BUFSIZ));
	mktemp(lettmp);
	unlink(lettmp);
	my_name = getlogin();
	if (my_name == NULL)
		my_name = getpwuid(getuid())->pw_name;
	if(setjmp(sjbuf)) done();
	for (i=0; i<20; i++)
		setsig(i, delete);
	tmpf = fopen(lettmp, "w");
	if (tmpf == NULL) {
		fprintf(stderr, "mail: cannot open %s for writing\n", lettmp);
		error = 2;
		done();
	}
	if (argv[0][0] != 'r' &&	/* no favors for rmail */
	   (argc == 1 || argv[1][0] == '-' || argv[1][0] == '+'))
		printmail(argc, argv);
	else
		sendmail(argc, argv);
	done();
}

setsig(i, f)
int i;
int (*f)();
{
	if(signal(i, SIG_IGN)!=SIG_IGN)
		signal(i, f);
}

printmail(argc, argv)
char **argv;
{
	int	flg, i, j, print;
	char	*p, *getarg();

	if (argv[1][0] == '+') {
		forward = 1;
		argc--;
		argv++;
	}
	flgf = 0;
	home = getenv("HOME");
	if(home == NULL)
		fprintf(stderr, "mail: cannot find home directory\n");
	hmbox = malloc(strlen(home) + strlen(mbox) + 1);
	strcpy(hmbox, home);
	strcat(hmbox, mbox);
	cat(mailfile, maildir, my_name);
	if ((argv[1][0] == '-') && (argv[1][1] == 'i')) {
		delflg++;
		argc--; argv++;
	}
	if (argc > 2 && argv[1][1] == 'f') {
		flgf = 1;
		cat(mailfile, argv[2], "");
	}
	malf = fopen(mailfile, "r");
	if (malf == NULL) {
		fprintf(stdout, "No mail.\n");
		return;
	}
	lock(mailfile);
	copymt(malf, tmpf);
	fclose(malf);
	fclose(tmpf);
	unlock();
	tmpf = fopen(lettmp, "r");
	changed = 0;
	print = 1;
	for (i = 0; i < nlet; ) {
		j = forward ? i : nlet - i - 1;
		if(setjmp(sjbuf)) {
			print=0;
		} else {
			if (print)
				copylet(j, stdout, ORDINARY);
			print = 1;
		}
		setjmp(sjbuf);
		fprintf(stdout, "? ");
		fflush(stdout);
		if (fgets(resp, LSIZE, stdin) == NULL)
			break;
		switch (resp[0]) {

		default:
			fprintf(stderr, "usage\n");
		case '?':
			print = 0;
			fprintf(stderr, "q\t\tquit\n");
			fprintf(stderr, "p\t\tprint\n");
			fprintf(stderr, "s [file]\tsave (default mbox)\n");
			fprintf(stderr, "w [file]\tsame without header\n");
			fprintf(stderr, "-\t\tprint previous\n");
			fprintf(stderr, "d\t\tdelete\n");
			fprintf(stderr, "+\t\tnext (no delete)\n");
			fprintf(stderr, "m [user]\tmail to user\n");
			break;

		case 'n':
		case '\n':
			i++;
			break;
		case 'q':
			goto donep;
		case 'p':
			break;
		case '^':
		case '-':
			if (--i < 0)
				i = 0;
			break;
		case 'y':
		case 'w':
		case 's':
			flg = 0;
			if (resp[1] == '\n' || resp[1] == '\0')
				cat(resp+1, hmbox, "");
			else if(resp[1] != ' ') {
				printf("invalid command\n");
				flg++;
				print = 0;
				continue;
			}
			for (p = resp+1; (p = getarg(lfil, p)) != NULL; ) {
				umask(umsave);
				malf = fopen(lfil, "a");
				umask(0);
				if (malf == NULL) {
					fprintf(stdout, "mail: cannot append to %s\n", lfil);
					flg++;
					continue;
				}
				copylet(j, malf, resp[0]=='w'? ZAP: ORDINARY);
				fclose(malf);
			}
			if (flg)
				print = 0;
			else {
				let[j].change = 'd';
				changed++;
				i++;
			}
			break;
		case 'm':
			flg = 0;
			if (resp[1] == '\n' || resp[1] == '\0') {
				i++;
				continue;
			}
			if (resp[1] != ' ') {
				printf("invalid command\n");
				flg++;
				print = 0;
				continue;
			}
			for (p = resp+1; (p = getarg(lfil, p)) != NULL; )
				if (!send(j, lfil))	/* couldn't send it */
					flg++;
			if (flg)
				print = 0;
			else {
				let[j].change = 'd';
				changed++;
				i++;
			}
			break;
		case 'd':
			let[j].change = 'd';
			changed++;
			i++;
			if (resp[1] == 'q')
				goto donep;
			break;
		}
	}
   donep:
	if (changed)
		copyback();
}

copyback()	/* copy temp or whatever back to /usr/mail */
{
	register i, n, c;
	int new = 0;
	struct stat stbuf;

	signal(SIGINT, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	lock(mailfile);
	stat(mailfile, &stbuf);
	if (stbuf.st_size != let[nlet].adr) {	/* new mail has arrived */
		malf = fopen(mailfile, "r");
		if (malf == NULL) {
			fprintf(stdout, "mail: can't re-read %s\n", mailfile);
			error = 2;
			done();
		}
		fseek(malf, let[nlet].adr, 0);
		fclose(tmpf);
		tmpf = fopen(lettmp, "a");
		fseek(tmpf, let[nlet].adr, 0);
		while ((c = fgetc(malf)) != EOF)
			fputc(c, tmpf);
		fclose(malf);
		fclose(tmpf);
		tmpf = fopen(lettmp, "r");
		let[++nlet].adr = stbuf.st_size;
		new = 1;
	}
	malf = fopen(mailfile, "w");
	if (malf == NULL) {
		fprintf(stderr, "mail: can't rewrite %s\n", mailfile);
		error = 2;
		done();
	}
	n = 0;
	for (i = 0; i < nlet; i++)
		if (let[i].change != 'd') {
			copylet(i, malf, ORDINARY);
			n++;
		}
	fclose(malf);
	if (n == 0)	/* none written, empty mailbox */
		unlink(mailfile);
	if (new)
		fprintf(stdout, "new mail arrived\n");
	unlock();
}

copymt(f1, f2)	/* copy mail (f1) to temp (f2) */
FILE *f1, *f2;
{
	long nextadr;

	nlet = nextadr = 0;
	let[0].adr = 0;
	while (fgets(line, LSIZE, f1) != NULL) {
		if (isfrom(line))
			let[nlet++].adr = nextadr;
		nextadr += strlen(line);
		fputs(line, f2);
	}
	let[nlet].adr = nextadr;	/* last plus 1 */
}

copylet(n, f, type) FILE *f;
{	int ch, k;
	fseek(tmpf, let[n].adr, 0);
	k = let[n+1].adr - let[n].adr;
	while(k-- > 1 && (ch=fgetc(tmpf))!='\n')
		if(type!=ZAP)
			fputc(ch,f);
	if(type==REMOTE)
		fprintf(f, rmtmsg);
	else if(type==ORDINARY)
		fputc(ch,f);
	while(k-->1)
		fputc(ch=fgetc(tmpf), f);
	if(type!=ZAP || ch!= '\n')
		fputc(fgetc(tmpf), f);
}

isfrom(lp)
register char *lp;
{
	register char *p;

	for (p = from; *p; )
		if (*lp++ != *p++)
			return(0);
	return(1);
}

sendmail(argc, argv)
char **argv;
{

	time(&iop);
	fprintf(tmpf, "%s%s %s", from, my_name, ctime(&iop));
	iop = ftell(tmpf);
	flgf = 1;
	while (fgets(line, LSIZE, stdin) != NULL) {
		if (line[0] == '.' && line[1] == '\n')
			break;
		if (isfrom(line))
			fputs(">", tmpf);
		fputs(line, tmpf);
		flgf = 0;
	}
	fputs("\n", tmpf);
	nlet = 1;
	let[0].adr = 0;
	let[1].adr = ftell(tmpf);
	fclose(tmpf);
	if (flgf)
		return;
	tmpf = fopen(lettmp, "r");
	if (tmpf == NULL) {
		fprintf(stderr, "mail: cannot reopen %s for reading\n", lettmp);
		error = 2;
		return;
	}
	while (--argc > 0)
		if (!send(0, *++argv))	/* couldn't send to him */
			error++;
	if (error) {
		umask(umsave);
		malf = fopen(dead, "w");
		if (malf == NULL) {
			fprintf(stdout, "mail: cannot create %s\n", dead);
			fclose(tmpf);
			error = 2;
			return;
		}
		copylet(0, malf, ZAP);
		fclose(malf);
		fprintf(stdout, "Mail saved in %s\n", dead);
	}
	fclose(tmpf);
}

sendrmt(n, name) char *name;
{	char rsys[50], cmd[128], *p;
	if(*name == '!') strcpy(name, thissys);
	else	for(p=rsys; *name!='!'; *p++ = *name++);
	*p = '\0';
	if(*name++=='\0')
	{	fprintf(stdout, "null name\n");
		return(0);
	}
	if(rmtf==NULL)
	{	mktemp(rmtbuf);
		unlink(rmtbuf);
		rmtf = fopen(rmtbuf, "w");
		if(rmtf==NULL)
		{	fprintf(stderr,"can't open %s for writing\n",rmtbuf);
			return(0);
		}
	}
	else
	{	close(creat(rmtbuf, 0600)); /*truncate*/
		rewind(rmtf);
	}
	copylet(n, rmtf, REMOTE);
	sprintf(cmd, "cat %s | uux - '%s!rmail %s'", rmtbuf, rsys, name);
	fflush(rmtf);
	return(!system(cmd));
}

send(n, name)	/* send letter n to name */
int n;
char *name;
{
	char	file[50], *p;
	struct	passwd	*pwd;

	for(p=name; *p!='!' &&*p!='\0'; p++);
	if(*p == '!')
		return(sendrmt(n, name));
	setpwent();	/* rewind password file -- dumb design */
	pwd = getpwnam(name);
	if (pwd == NULL) {
		fprintf(stdout, "mail: can't send to %s\n", name);
		return(0);
	}
	cat(file, maildir, name);
	lock(file);
	if(access(file, 0) < 0) {
		close(creat(file, MFMODE));
		chown(file, pwd->pw_uid, pwd->pw_gid);
	}
	malf = fopen(file, "a");
	if (malf == NULL) {
		fprintf(stdout, "mail: cannot append to %s\n", file);
		unlock();
		return(0);
	}
	copylet(n, malf, ORDINARY);
	fclose(malf);
	unlock();
	return(1);
}

delete(i)
{
	setsig(i, delete);
	fprintf(stderr, "\n");
	if(delflg)
		longjmp(sjbuf, 1);
	done();
}

done()
{
	unlock();
	unlink(lettmp);
	unlink(rmtbuf);
	exit(error);
}

lock(file)
char *file;
{
	int f, i;

	if (locked)
		return;
	cat(curlock, file, maillock);
	for (i=0; i<10; i++) {
		f = creat(curlock, 0);
		if (f >= 0) {
			close(f);
			locked = 1;
			return;
		}
		sleep(2);
	}
	fprintf(stderr, "mail: %s not creatable after %d tries\n", curlock, i);
	fprintf(stderr, "gok whats wrong\n");
	error = 2;
	done();
}

unlock()
{
	unlink(curlock);
	locked = 0;
}

cat(to, from1, from2)
char *to, *from1, *from2;
{
	int i, j;

	j = 0;
	for (i=0; from1[i]; i++)
		to[j++] = from1[i];
	for (i=0; from2[i]; i++)
		to[j++] = from2[i];
	to[j] = 0;
}

char *getarg(s, p)	/* copy p... into s, update p */
register char *s, *p;
{
	while (*p == ' ' || *p == '\t')
		p++;
	if (*p == '\n' || *p == '\0')
		return(NULL);
	while (*p != ' ' && *p != '\t' && *p != '\n' && *p != '\0')
		*s++ = *p++;
	*s = '\0';
	return(p);
}
