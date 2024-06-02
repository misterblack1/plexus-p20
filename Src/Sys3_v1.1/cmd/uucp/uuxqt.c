	/*  uuxqt 3.10  1/30/80  21:52:22  */
#include "uucp.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/dir.h>


#define APPCMD(d) {\
char *p;\
for (p = d; *p != '\0';) *cmdp++ = *p++;\
*cmdp++ = ' ';\
*cmdp = '\0';}

/*
 *	uuxqt will execute commands set up by a uux command,
 *	usually from a remote machine - set by uucp.
 */

char *Cmds[] = {
	"rmail",
	"lpr",
	"opr",
	"fsend",
	"fget",
	NULL
	};
#define PATH	"PATH=/bin:/usr/bin;"
/*  to remove restrictions from uuxqt
 *  define ALLOK 1
 *
 *  to add allowable commands, add to the list under Cmds[]
 */

main(argc, argv)
char *argv[];
{
	char xcmd[100];
	int argnok;
	int exitsig, exitcode;
	char xfile[MAXFULLNAME], user[10], buf[BUFSIZ];
	char lbuf[30];
	char cfile[NAMESIZE], dfile[MAXFULLNAME];
	char file[NAMESIZE];
	char fin[MAXFULLNAME], sysout[NAMESIZE], fout[MAXFULLNAME];
	FILE *xfp, *dfp, *fp;
	char path[MAXFULLNAME];
	char cmd[BUFSIZ];
	char *cmdp, prm[MAXFULLNAME], *ptr;
	char *getprm(), *lastpart();
	int uid, ret, badfiles;
	int stcico = 0;
	char retstat[30];

	strcpy(Progname, "uuxqt");
	uucpname(Myname);
	umask(WFMASK);
	Ofn = 1;
	Ifn = 0;
	while (argc>1 && argv[1][0] == '-') {
		switch(argv[1][1]){
		case 'x':
			Debug = atoi(&argv[1][2]);
			if (Debug <= 0)
				Debug = 1;
			break;
		default:
			fprintf(stderr, "unknown flag %s\n", argv[1]);
				break;
		}
		--argc;  argv++;
	}

	DEBUG(4, "\n\n** %s **\n", "START");
	chdir(Spool);
	strcpy(Wrkdir, Spool);
	uid = getuid();
	guinfo(uid, User, path);
	DEBUG(4, "User - %s\n", User);
	if (ulockf(X_LOCK, (time_t)  X_LOCKTIME) != 0)
		exit(0);

	DEBUG(4, "process %s\n", "");
	while (gtxfile(xfile) > 0) {
		DEBUG(4, "xfile - %s\n", xfile);

		xfp = fopen(xfile, "r");
		ASSERT(xfp != NULL, "CAN'T OPEN", xfile, 0);

		/*  initialize to default  */
		strcpy(user, User);
		strcpy(fin, "/dev/null");
		strcpy(fout, "/dev/null");
		sprintf(sysout, "%.7s", Myname);
		while (fgets(buf, BUFSIZ, xfp) != NULL) {
			switch (buf[0]) {
			case X_USER:
				sscanf(&buf[1], "%s%s", user, Rmtname);
				break;
			case X_STDIN:
				sscanf(&buf[1], "%s", fin);
				expfile(fin);
				if (chkpth("", "", fin) || (anyread(fin) != 0))
					badfiles = 1;
				break;
			case X_STDOUT:
				sscanf(&buf[1], "%s%s", fout, sysout);
				sysout[7] = '\0';
				if (fout[0] != '~' || prefix(sysout, Myname))
					expfile(fout);
				if (chkpth("", "", fout))
					badfiles = 1;
				break;
			case X_CMD:
				strcpy(cmd, &buf[2]);
				if (*(cmd + strlen(cmd) - 1) == '\n')
					*(cmd + strlen(cmd) - 1) = '\0';
				break;
			default:
				break;
			}
		}

		fclose(xfp);
		DEBUG(4, "fin - %s, ", fin);
		DEBUG(4, "fout - %s, ", fout);
		DEBUG(4, "sysout - %s, ", sysout);
		DEBUG(4, "user - %s\n", user);
		DEBUG(4, "cmd - %s\n", cmd);

		/*  command execution  */
		if (strcmp(fout, "/dev/null") == SAME)
			strcpy(dfile,"/dev/null");
		else
			gename(DATAPRE, sysout, 'O', dfile);

		/* expand file names where necessary */
		expfile(dfile);
		strcpy(buf, PATH);
		cmdp = buf + strlen(buf);
		ptr = cmd;
		xcmd[0] = '\0';
		argnok = 0;
		while ((ptr = getprm(ptr, prm)) != NULL) {
			if (prm[0] == ';' || prm[0] == '^'
			  || prm[0] == '&'  || prm[0] == '|') {
				xcmd[0] = '\0';
				APPCMD(prm);
				continue;
			}

			if ((argnok = argok(xcmd, prm)) != 0) 
				/*  command not valid  */
				break;

			if (prm[0] == '~')
				expfile(prm);
			APPCMD(prm);
		}
		if (argnok || badfiles) {
			sprintf(lbuf, "%s XQT DENIED", user);
			logent(cmd, lbuf);
			DEBUG(4, "bad command %s\n", prm);
			notify(user, Rmtname, cmd, "DENIED");
			goto rmfiles;
		}
		sprintf(lbuf, "%s XQT", user);
		logent(buf, lbuf);
		DEBUG(4, "cmd %s\n", buf);

		mvxfiles(xfile);
		chdir(XQTDIR);
		ret = shio(buf, fin, dfile, NULL);
		exitsig = ret & 0377;
		exitcode = (ret>>8) & 0377;
		sprintf(retstat, "exit %d, signal %d", ret & 0377,
		  (ret>>8) & 0377);
		if (strcmp(xcmd, "rmail") != SAME
		  && strcmp(xcmd, "mail") != SAME)
			notify(user, Rmtname, cmd, retstat);
		else if (ret != 0) {
			/* mail failed - return letter to sender  */
			retosndr(user, Rmtname, fin);
			sprintf(buf, "ret (%o) from %s!%s", ret, Rmtname, user);
			logent("MAIL FAIL", buf);
		}
		DEBUG(4, "exit cmd - %d\n", ret);
		chdir(Spool);
		rmxfiles(xfile);
		if (ret != 0) {
			/*  exit status not zero */
			dfp = fopen(dfile, "a");
			ASSERT(dfp != NULL, "CAN'T OPEN", dfile, 0);
			fprintf(dfp, "exit status %d", ret);
			fclose(dfp);
		}
		if (strcmp(fout, "/dev/null") != SAME) {
			if (prefix(sysout, Myname)) {
				xmv(dfile, fout);
			}
			else {
				gename(CMDPRE, sysout, 'O', cfile);
				fp = fopen(cfile, "w");
				ASSERT(fp != NULL, "OPEN", cfile, 0);
				fprintf(fp, "S %s %s %s - %s 0666\n",
				dfile, fout, user, lastpart(dfile));
				fclose(fp);
			}
		}
	rmfiles:
		xfp = fopen(xfile, "r");
		ASSERT(xfp != NULL, "CAN'T OPEN", xfile, 0);
		while (fgets(buf, BUFSIZ, xfp) != NULL) {
			if (buf[0] != X_RQDFILE)
				continue;
			sscanf(&buf[1], "%s", file);
			unlink(file);
		}
		unlink(xfile);
	}

	if (stcico)
		xuucico("");
	cleanup(0);
}


cleanup(code)
int code;
{
	logcls();
	rmlock(CNULL);
	exit(code);
}


/*******
 *	gtxfile(file)	get a file to execute
 *	char *file;
 *
 *	return codes:  0 - no file  |  1 - file to execute
 */

gtxfile(file)
char *file;
{
	static FILE *pdir;
	char pre[2];

	if (pdir == NULL) {
		pdir = fopen(Spool, "r");
		ASSERT(pdir != NULL, "GTXFILE CAN'T OPEN", Spool, 0);
	}

	pre[0] = XQTPRE;
	pre[1] = '\0';
	while (gnamef(pdir, file) != 0) {
		DEBUG(4, "file - %s\n", file);
		if (!prefix(pre, file))
			continue;
		if (gotfiles(file))
			/*  return file to execute */
			return(1);
	}

	fclose(pdir);
	return(0);
}


/***
 *	gotfiles(file)		check for needed files
 *	char *file;
 *
 *	return codes:  0 - not ready  |  1 - all files ready
 */

gotfiles(file)
char *file;
{
	struct stat stbuf;
	FILE *fp;
	char buf[BUFSIZ], rqfile[MAXFULLNAME];

	fp = fopen(file, "r");
	if (fp == NULL)
		return(0);

	while (fgets(buf, BUFSIZ, fp) != NULL) {
		DEBUG(4, "%s\n", buf);
		if (buf[0] != X_RQDFILE)
			continue;
		sscanf(&buf[1], "%s", rqfile);
		expfile(rqfile);
		if (stat(rqfile, &stbuf) == -1) {
			fclose(fp);
			return(0);
		}
	}

	fclose(fp);
	return(1);
}


/***
 *	rmxfiles(xfile)		remove execute files to x-directory
 *	char *xfile;
 *
 *	return codes - none
 */

rmxfiles(xfile)
char *xfile;
{
	FILE *fp;
	char buf[BUFSIZ], file[NAMESIZE], tfile[NAMESIZE];
	char tfull[MAXFULLNAME];

	if((fp = fopen(xfile, "r")) == NULL)
		return;

	while (fgets(buf, BUFSIZ, fp) != NULL) {
		if (buf[0] != X_RQDFILE)
			continue;
		if (sscanf(&buf[1], "%s%s", file, tfile) < 2)
			continue;
		sprintf(tfull, "%s/%s", XQTDIR, tfile);
		unlink(tfull);
	}
	fclose(fp);
	return;
}


/***
 *	mvxfiles(xfile)		move execute files to x-directory
 *	char *xfile;
 *
 *	return codes - none
 */

mvxfiles(xfile)
char *xfile;
{
	FILE *fp;
	char buf[BUFSIZ], ffile[MAXFULLNAME], tfile[NAMESIZE];
	char tfull[MAXFULLNAME];
	int ret;

	if((fp = fopen(xfile, "r")) == NULL)
		return;

	while (fgets(buf, BUFSIZ, fp) != NULL) {
		if (buf[0] != X_RQDFILE)
			continue;
		if (sscanf(&buf[1], "%s%s", ffile, tfile) < 2)
			continue;
		expfile(ffile);
		sprintf(tfull, "%s/%s", XQTDIR, tfile);
		unlink(tfull);
		ret = link(ffile, tfull);
		ASSERT(ret == 0, "LINK ERROR", "", ret);
		unlink(ffile);
	}
	fclose(fp);
	return;
}


/***
 *	argok(xc, cmd)		check for valid command/argumanet
 *			*NOTE - side effect is to set xc to the
 *				command to be executed.
 *	char *xc, *cmd;
 *
 *	return 0 - ok | 1 nok
 */

argok(xc, cmd)
char *xc, *cmd;
{
	char **ptr;
	char *index();

#ifndef ALLOK
	if (index(cmd, '`') != NULL
	  || index(cmd, '>') != NULL
	  || index(cmd, ';') != NULL
	  || index(cmd, '^') != NULL
	  || index(cmd, '&') != NULL
	  || index(cmd, '|') != NULL
	  || index(cmd, '<') != NULL)
		/* don't allow sh command strings `....` */
		/*  don't allow redirection of standard in or out  */
		return(1);
#endif

	if (xc[0] != '\0')
		return(0);

#ifndef ALLOK
	ptr = Cmds;
	while(*ptr != NULL) {
		if (strcmp(cmd, *ptr) == SAME)
			break;
	ptr++;
	}
	if (*ptr == NULL)
		return(1);
#endif
	strcpy(xc, cmd);
	return(0);
}


/***
 *	notify	send mail to user giving execution results
 *	return code - none
 *	This program assumes new mail command - send remote mail
 */

notify(user, rmt, cmd, str)
char *user, *rmt, *cmd, *str;
{
	char text[100];
	char ruser[100];

	sprintf(text, "uuxqt cmd (%.50s) status (%s)", cmd, str);
	if (prefix(rmt, Myname))
		strcpy(ruser, user);
	else
		sprintf(ruser, "%s!%s", rmt, user);
	mailst(ruser, text, "");
	return;
}

/***
 *	retosndr - return mail to sender
 *
 *	return code - none
 */

retosndr(user, rmt, file)
char *user, *rmt, *file;
{
	char ruser[100];

	if (strcmp(rmt, Myname) == SAME)
		strcpy(ruser, user);
	else
		sprintf(ruser, "%s!%s", rmt, user);

	if (anyread(file) == 0)
		mailst(ruser, "Mail failed.  Letter returned to sender.\n", file);
	else
		mailst(ruser, "Mail failed.  Letter returned to sender.\n", "");
	return;
}
