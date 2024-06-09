/*
**	mail [ person ]
**	mail -f file
*/

#include	<stdio.h>
#include	<pwd.h>
#include	<utmp.h>
#include	<signal.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<setjmp.h>
#include	<sys/utsname.h>

/*copylet flags */
#define	REMOTE		1		/* remote mail, add rmtmsg */
#define ORDINARY	2
#define ZAP		3		/* zap header and trailing empty line */
#define FORWARD		4
#define	LSIZE		256
#define	MAXLET		300		/* maximum number of letters */
#ifndef	MFMODE
#define	MFMODE		0664		/* create mode for `/usr/mail' files */
#endif

struct	let	{
	long	adr;
	char	change;
} let[MAXLET];

struct	passwd	*getpwuid(), getpwent();
struct	utsname utsn;

char	lettmp[] = "/tmp/maXXXXX";
char	from[] = "From ";
char	TO[] = "To: ";
char	maildir[] = "/usr/mail/";
char	sendto[256];
char	*mailfile;
char	maillock[] = ".lock";
char	dead[] = "dead.letter";
char	rmtbuf[] = "/tmp/marXXXXXX";
char	*rmtmsg = " remote from %s\n";
char	*forwmsg = " forwarded by %s\n";
char	frwrd[] = "Forward to ";
char	*thissys;
char	mbox[] = "/mbox";
char	curlock[50];
char	line[LSIZE];
char	resp[LSIZE];
char	*hmbox;
char	*home;
char	*email;
char	*my_name;
char	*getlogin();
char	*malloc();
char	lfil[50];
char	*ctime();
char	*getenv();
char	*strrchr();

FILE	*tmpf;
FILE	*malf;
FILE	*rmtf;

int	error;
int	nlet	= 0;
int	locked;
int	changed;
int	forward;
int	delete();
int	flgf = 0;
int	flgp = 0;
int	flge = 0;
int	delflg = 1;
int	savdead();
int	(*saveint)();
int	(*setsig())();

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
	uname(&utsn);
	thissys = utsn.nodename;
	my_name = getenv("LOGNAME");
	if ((my_name == NULL) || (strlen(my_name) == 0))
		my_name = getlogin();
	if ((my_name == NULL) || (strlen(my_name) == 0))
		my_name = getpwuid(geteuid())->pw_name;
	if(setjmp(sjbuf)) done();
	for (i=2; i<18; i++)
		setsig(i, delete);
	setsig(1, done);
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

int (*setsig(i, f))()
int i;
int (*f)();
{
	register int (*rc)();

	if((rc=signal(i, SIG_IGN))!=SIG_IGN)
		signal(i, f);
	return(rc);
}

printmail(argc, argv)
char **argv;
{
	int	flg, i, j, print, aret, stret, goerr = 0;
	char	c, *p, *getarg();
	char	frwrdbuf[256];
	struct	stat stbuf;
	extern	char *optarg;

	if (argv[1][0] == '+') {
		forward = 1;
		argc--;
		argv++;
	}
	while((c=getopt(argc, argv, "f:rpqie")) != EOF) switch(c) {
		case 'f':
			flgf = 1;
			mailfile = optarg;
			break;
		case 'p':
			flgp++;
		case 'q':
			delflg = 0;
		case 'i':
			break;
		case 'r':
			forward = 1;
			break;
		case 'e':
			flge = 1;
			break;
		case '?':
			goerr++;
	}
	if(goerr) {
		fprintf(stderr, "usage: mail [-rpq] [-f file] [persons]\n");
		error = 2;
		done();
	}
	home = getenv("HOME");
	if((home == NULL) || (strlen(home) == 0))
		home = ".";
	hmbox = malloc(strlen(home) + strlen(mbox) + 1);
	cat(hmbox, home, mbox);
	if(!flgf) {
		mailfile = getenv("MAIL");
		if((mailfile == NULL) || (strlen(mailfile) == 0)) {
			mailfile = malloc(strlen(maildir) + strlen(my_name) + 1);
			cat(mailfile, maildir, my_name);
		}
	}
	stret = stat(mailfile, &stbuf);
	if((aret=access(mailfile, 4)) == 0)
		malf = fopen(mailfile, "r");
	if (!stret && aret) {
		fprintf(stderr, "mail: permission denied!\n");
		error = 2;
		return;
	}
	else if (flgf && (aret || (malf == NULL))) {
		fprintf(stderr, "mail: cannot open %s\n", mailfile);
		error = 2;
		return;
	}
	else if(aret || (malf == NULL) || (stbuf.st_size == 0)) {
		if(!flge) fprintf(stdout, "No mail.\n");
		error = 1;
		return;
	}
	lock(mailfile);
	if(areforwarding(mailfile)) {
		if(flge) {
			unlock();
			error = 1;
			return;
		}
		printf("Your mail is being forwarded to ");
		fseek(malf, (long)(sizeof(frwrd) - 1), 0);
		fgets(frwrdbuf, sizeof(frwrdbuf), malf);
		printf("%s", frwrdbuf);
		if(getc(malf) != EOF)
			printf("and your mailbox contains extra stuff\n");
		unlock();
		return;
	}
	if(flge) {
		unlock();
		return;
	}
	copymt(malf, tmpf);
	fclose(malf);
	fclose(tmpf);
	unlock();
	tmpf = fopen(lettmp, "r");
	changed = 0;
	print = 1;
	for (i = 0; i < nlet; ) {
		j = forward ? i : nlet - i - 1;
		if( setjmp(sjbuf) == 0 && print != 0 )
				copylet(j, stdout, ORDINARY);
		if(flgp) {
			i++;
			continue;
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
			fprintf(stderr, "x\t\texit without changing mail\n");
			fprintf(stderr, "p\t\tprint\n");
			fprintf(stderr, "s [file]\tsave (default mbox)\n");
			fprintf(stderr, "w [file]\tsame without header\n");
			fprintf(stderr, "-\t\tprint previous\n");
			fprintf(stderr, "d\t\tdelete\n");
			fprintf(stderr, "+\t\tnext (no delete)\n");
			fprintf(stderr, "m [user]\tmail to user\n");
			fprintf(stderr, "! cmd\t\texecute cmd\n");
			break;

		case '+':
		case 'n':
		case '\n':
			i++;
		case 'p':
			print = 1;
			break;
		case 'x':
			changed = 0;
		case 'q':
			goto donep;
		case '^':
		case '-':
			if (--i < 0)
				i = 0;
			print = 1;
			break;
		case 'y':
		case 'w':
		case 's':
			if (resp[1] == '\n' || resp[1] == '\0')
				cat(resp+1, hmbox, "");
			else if(resp[1] != ' ') {
				printf("invalid command\n");
				print = 0;
				continue;
			}
			umask(umsave);
			flg = 0;
			for (p = resp+1; (p = getarg(lfil, p)) != NULL; ) {
				if((aret=legal(lfil)))
					malf = fopen(lfil, "a");
				if ((malf == NULL) || (aret == 0)) {
					fprintf(stdout, "mail: cannot append to %s\n", lfil);
					flg++;
					continue;
				}
				if(aret==2)
					chown(lfil, geteuid(), getgid());
				copylet(j, malf, resp[0]=='w'? ZAP: ORDINARY);
				fclose(malf);
			}
			umask(0);
			if (flg)
				print = 0;
			else {
				let[j].change = 'd';
				changed++;
				i++;
			}
			break;
		case 'm':
			if (resp[1] == '\n' || resp[1] == '\0') {
				i++;
				continue;
			}
			if (resp[1] != ' ') {
				printf("invalid command\n");
				print = 0;
				continue;
			}
			flg = 0;
			for (p = resp+1; (p = getarg(lfil, p)) != NULL; )
				if (!sendrmt(j, lfil))	/* couldn't send it */
					flg++;
			if (flg)
				print = 0;
			else {
				let[j].change = 'd';
				changed++;
				i++;
			}
			break;
		case '!':
			system(resp+1);
			printf("!\n");
			print = 0;
			break;
		case 'd':
			let[j].change = 'd';
			changed++;
			i++;
			if (resp[1] == 'q')
				goto donep;
			print = 1;
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
	int new = 0, aret;
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
	if((aret=access(mailfile, 2)) == 0)
		malf = fopen(mailfile, "w");
	if ((malf == NULL) || (aret)) {
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
	if ((n == 0) && ((stbuf.st_mode & 0777)== MFMODE)) /* empty mailbox */
		unlink(mailfile);
	if (new && !flgf)
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

areforwarding(s) char *s;
{	FILE *fd;
	char fbuf[256], *p;
	int c;
	fd = fopen(s, "r");
	if(fd == NULL)
		return(0);
	fread(fbuf, sizeof(frwrd) - 1, 1, fd);
	if(strncmp(fbuf, frwrd, sizeof(frwrd) - 1) == 0) {
		for(p = sendto; (c = getc(fd)) != EOF && c != '\n';)
			if(c != ' ') *p++ = c;
		*p = 0;
		fclose(fd);
		return(1);
	}
	fclose(fd);
	return(0);
}

copylet(n, f, type) FILE *f;
{	int ch, k;
	fseek(tmpf, let[n].adr, 0);
	k = let[n+1].adr - let[n].adr;
	while(k-- > 1 && (ch=fgetc(tmpf))!='\n')
		if(type!=ZAP)
			fputc(ch,f);
	if(type==REMOTE)
		fprintf(f, rmtmsg, thissys);
	else if(type==FORWARD)
		fprintf(f, forwmsg, my_name);
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

	int	aret;
	char	**args;
	time(&iop);
	fprintf(tmpf, "%s%s %s", from, my_name, ctime(&iop));
	if(argc > 2) {		/* send Copy to message */
		aret = argc;
		args = argv;
		fprintf(tmpf,"%s ",TO);
		while(--aret > 0)
			fprintf(tmpf,"%s ",*++args);
		fputs("\n",tmpf);
	}
	iop = ftell(tmpf);
	flgf = 1;
	saveint = setsig(SIGINT, savdead);
	while (fgets(line, LSIZE, stdin) != NULL) {
		if (line[0] == '.' && line[1] == '\n')
			break;
		if (isfrom(line))
			fputs(">", tmpf);
		fputs(line, tmpf);
		flgf = 0;
	}
	setsig(SIGINT, saveint);
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
	if (error == 0)
		while (--argc > 0)
			if (!send(0, *++argv, 0))	/* couldn't send to him */
				error++;
	if (error) {
		umask(umsave);
		if((aret=legal(dead)))
			malf = fopen(dead, "w");
		if ((malf == NULL) || (aret == 0)) {
			fprintf(stdout, "mail: cannot create %s\n", dead);
			fclose(tmpf);
			error = 2;
			umask(0);
			return;
		}
		umask(0);
		copylet(0, malf, ZAP);
		fclose(malf);
		fprintf(stdout, "Mail saved in %s\n", dead);
	}
	fclose(tmpf);
}

savdead()
{
	setsig(SIGINT, saveint);
	error++;
}

sendrmt(n, name)
char *name;
{
	FILE *rmf, *popen();
	register char *p;
	char rsys[64], cmd[64];
	register local;

	local = 0;
	if (*name=='!')
		name++;
	for(p=rsys; *name!='!'; *p++ = *name++)
		if (*name=='\0') {
			local++;
			break;
		}
	*p = '\0';
	if ((!local && *name=='\0') || (local && *rsys=='\0')) {
		fprintf(stdout, "null name\n");
		return(0);
	}
	if (local)
		sprintf(cmd, "mail %s", rsys);
	else {
		if (strchr(name+1, '!'))
			sprintf(cmd, "uux - %s!rmail \\(%s\\)", rsys, name+1);
		else
			sprintf(cmd, "uux - %s!rmail %s", rsys, name+1);
	}
	if ((rmf=popen(cmd, "w")) == NULL)
		return(0);
	copylet(n, rmf, local? FORWARD: REMOTE);
	return(pclose(rmf)==0 ? 1 : 0);
}

send(n, name, level)	/* send letter n to name */
int n;
char *name;
{
	char	file[50], *p;
	struct	passwd	*pwd, *getpwname();

	if(level > 20) {
		fprintf(stdout, "unbounded forwarding\n");
		return(0);
	}
	if (strcmp(name, "-") == 0)
		return(1);
	for(p=name; *p!='!' &&*p!='\0'; p++)
		;
	if (*p == '!')
		return(sendrmt(n, name));
	cat(file, maildir, name);
	if(areforwarding(file))
		return(send(n, sendto, level+1));
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
		chown(file, pwd->pw_uid, getegid());
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
	if(i>3)
		fprintf(stderr, "mail: error signal %d\n", i);
	else
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

legal(file)
char *file;
{
	char	*sp, dfile[100];
	if(!access(file, 0))
		if(!access(file, 2))
			return(1);
		else	return(0);
	else {
		if((sp=strrchr(file, '/')) == NULL)
			cat(dfile, ".", "");
		else {
			strncpy(dfile, file, sp - file);
			dfile[sp - file] = '\0';
		}
		if(access(dfile, 2)) return(0);
		return(2);
	}
}

system(s)
char *s;
{
	int status, pid, w;
	register int (*istat)(), (*qstat)();

	if ((pid = fork()) == 0) {
		setgid(getgid());
		execl("/bin/sh", "sh", "-c", s, 0);
		_exit(127);
	}
	istat = signal(SIGINT, SIG_IGN);
	qstat = signal(SIGQUIT, SIG_IGN);
	while ((w = wait(&status)) != pid && w != -1)
		;
	if (w == -1)
		status = -1;
	signal(SIGINT, istat);
	signal(SIGQUIT, qstat);
	return(status);
}
