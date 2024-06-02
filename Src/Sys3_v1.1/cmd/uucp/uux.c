	/*  uux 3.7  1/11/80  14:16:55  */
#include "uucp.h"


#define NOSYSPART 0
#define HASSYSPART 1

#define APPCMD(d) {\
char *p;\
for (p = d; *p != '\0';) *cmdp++ = *p++;\
*cmdp++ = ' ';\
*cmdp = '\0';}

#define GENSEND(f, a, b, c, d) {\
fprintf(f, "S %s %s %s - %s 0666\n", a, b, c, d);\
}
#define GENRCV(f, a, b, c) {\
fprintf(f, "R %s %s %s - \n", a, b, c);\
}
/*
 *	
 */

main(argc, argv)
char *argv[];
{
	char cfile[NAMESIZE];	/* send commands for files from here */
	char dfile[NAMESIZE];	/* used for all data files from here */
	char rxfile[NAMESIZE];	/* to be sent to xqt file (X. ...) */
	char tfile[NAMESIZE];	/* temporary file name */
	char tcfile[NAMESIZE];	/* temporary file name */
	char t2file[NAMESIZE];	/* temporary file name */
	int cflag = 0;		/*  commands in C. file flag  */
	int rflag = 0;		/*  C. files for receiving flag  */
	char buf[BUFSIZ];
	char inargs[BUFSIZ];
	int pipein = 0;
	int startjob = 1;
	char path[MAXFULLNAME];
	char cmd[BUFSIZ];
	char *ap, *cmdp;
	char prm[BUFSIZ];
	char syspart[8], rest[MAXFULLNAME];
	char xsys[8], local[8];
	FILE *fprx, *fpc, *fpd, *fp;
	FILE *xqtstr();
	extern char *getprm(), *index(), *lastpart();
	extern FILE *ufopen();
	int uid, ret;
	char redir = '\0';

	strcpy(Progname, "uux");
	uucpname(Myname);
	umask(WFMASK);
	Ofn = 1;
	Ifn = 0;
	while (argc>1 && argv[1][0] == '-') {
		switch(argv[1][1]){
		case 'p':
		case '\0':
			pipein = 1;
			break;
		case 'r':
			startjob = 0;
			break;
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

	inargs[0] = '\0';
	for (argv++; argc > 1; argc--) {
		DEBUG(4, "arg - %s:", *argv);
		strcat(inargs, " ");
		strcat(inargs, *argv++);
	}
	DEBUG(4, "arg - %s\n", inargs);
	gwd(Wrkdir);
	chdir(Spool);
	uid = getuid();
	guinfo(uid, User, path);

	sprintf(local, "%.7s", Myname);
	cmdp = cmd;
	*cmdp = '\0';
	gename(DATAPRE, local, 'X', rxfile);
	fprx = ufopen(rxfile, "w");
	ASSERT(fprx != NULL, "CAN'T OPEN", rxfile, 0);
	gename(DATAPRE, local, 'T', tcfile);
	fpc = ufopen(tcfile, "w");
	ASSERT(fpc != NULL, "CAN'T OPEN", tcfile, 0);
	fprintf(fprx, "%c %s %s\n", X_USER, User, local);

	/* find remote system name */
	ap = inargs;
	xsys[0] = '\0';
	while ((ap = getprm(ap, prm)) != NULL) {
		if (prm[0] == '>' || prm[0] == '<') {
			ap = getprm(ap, prm);
			continue;
		}


		split(prm, xsys, rest);
		break;
	}
	if (xsys[0] == '\0')
		strcpy(xsys, local);
	sprintf(Rmtname, "%.7s", xsys);
	DEBUG(4, "xsys %s\n", xsys);
	if (versys(xsys) != 0) {
		/*  bad system name  */
		fprintf(stderr, "bad system name: %s\n", xsys);
		fclose(fprx);
		fclose(fpc);
		cleanup(101);
	}

	if (pipein) {
		gename(DATAPRE, xsys, 'B', dfile);
		fpd = ufopen(dfile, "w");
		ASSERT(fpd != NULL, "CAN'T OPEN", dfile, 0);
		while (!feof(stdin)) {
			ret = fread(buf, 1, BUFSIZ, stdin);
			fwrite(buf, 1, ret, fpd);
		}
		fclose(fpd);
		if (strcmp(local, xsys) != SAME) {
			GENSEND(fpc, dfile, dfile, User, dfile);
			cflag++;
		}
		fprintf(fprx, "%c %s\n", X_RQDFILE, dfile);
		fprintf(fprx, "%c %s\n", X_STDIN, dfile);
	}
	/* parse command */
	ap = inargs;
	while ((ap = getprm(ap, prm)) != NULL) {
		DEBUG(4, "prm - %s\n", prm);
		if (prm[0] == '>' || prm[0] == '<') {
			redir = prm[0];
			continue;
		}

		if (prm[0] == ';') {
			APPCMD(prm);
			continue;
		}

		if (prm[0] == '|' || prm[0] == '^') {
			if (cmdp != cmd)
				APPCMD(prm);
			continue;
		}

		/* process command or file or option */
		ret = split(prm, syspart, rest);
		DEBUG(4, "s - %s, ", syspart);
		DEBUG(4, "r - %s, ", rest);
		DEBUG(4, "ret - %d\n", ret);
		if (syspart[0] == '\0')
			strcpy(syspart, local);

		if (cmdp == cmd && redir == '\0') {
			/* command */
			APPCMD(rest);
			continue;
		}

		/* process file or option */
		DEBUG(4, "file s- %s, ", syspart);
		DEBUG(4, "local - %s\n", local);
		/* process file */
		if (redir == '>') {
			if (rest[0] != '~')
				if (ckexpf(rest))
					cleanup(2);
			fprintf(fprx, "%c %s %s\n", X_STDOUT, rest,
			 syspart);
			redir = '\0';
			continue;
		}

		if (ret == NOSYSPART && redir == '\0') {
			/* option */
			APPCMD(rest);
			continue;
		}

		if (strcmp(xsys, local) == SAME
		 && strcmp(xsys, syspart) == SAME) {
			if (ckexpf(rest))
				cleanup(2);
			if (redir == '<')
				fprintf(fprx, "%c %s\n", X_STDIN, rest);
			else
				APPCMD(rest);
			redir = '\0';
			continue;
		}

		if (strcmp(syspart, local) == SAME) {
			/*  generate send file */
			if (ckexpf(rest))
				cleanup(2);
			gename(DATAPRE, xsys, 'A', dfile);
			DEBUG(4, "rest %s\n", rest);
			if ((chkpth(User, "", rest) || anyread(rest)) != 0) {
				fprintf(stderr, "permission denied %s\n", rest);
				cleanup(1);
			}
			if (xcp(rest, dfile) != 0) {
				fprintf(stderr, "can't copy %s to %s\n", rest, dfile);
				cleanup(1);
			}
			GENSEND(fpc, rest, dfile, User, dfile);
			cflag++;
			if (redir == '<') {
				fprintf(fprx, "%c %s\n", X_STDIN, dfile);
				fprintf(fprx, "%c %s\n", X_RQDFILE, dfile);
			}
			else {
				APPCMD(lastpart(rest));
				fprintf(fprx, "%c %s %s\n", X_RQDFILE,
				 dfile, lastpart(rest));
			}
			redir = '\0';
			continue;
		}

		if (strcmp(local, xsys) == SAME) {
			/*  generate local receive  */
			gename(CMDPRE, syspart, 'R', tfile);
			strcpy(dfile, tfile);
			dfile[0] = DATAPRE;
			fp = ufopen(tfile, "w");
			ASSERT(fp != NULL, "CAN'T OPEN", tfile, 0);
			if (ckexpf(rest))
				cleanup(2);
			GENRCV(fp, rest, dfile, User);
			fclose(fp);
			rflag++;
			if (rest[0] != '~')
				if (ckexpf(rest))
					cleanup(2);
			if (redir == '<') {
				fprintf(fprx, "%c %s\n", X_RQDFILE, dfile);
				fprintf(fprx, "%c %s\n", X_STDIN, dfile);
			}
			else {
				fprintf(fprx, "%c %s %s\n", X_RQDFILE, dfile,
				  lastpart(rest));
				APPCMD(lastpart(rest));
			}

			redir = '\0';
			continue;
		}

		if (strcmp(syspart, xsys) != SAME) {
			/* generate remote receives */
			gename(DATAPRE, syspart, 'R', dfile);
			strcpy(tfile, dfile);
			tfile[0] = CMDPRE;
			fpd = ufopen(dfile, "w");
			ASSERT(fpd != NULL, "CAN'T OPEN", dfile, 0);
			gename(DATAPRE, xsys, 'T', t2file);
			GENRCV(fpd, rest, t2file, User);
			fclose(fpd);
			GENSEND(fpc, dfile, tfile, User, dfile);
			cflag++;
			if (redir == '<') {
				fprintf(fprx, "%c %s\n", X_RQDFILE, t2file);
				fprintf(fprx, "%c %s\n", X_STDIN, t2file);
			}
			else {
				fprintf(fprx, "%c %s %s\n", X_RQDFILE, t2file,
				  lastpart(rest));
				APPCMD(lastpart(rest));
			}
			redir = '\0';
			continue;
		}

		/* file on remote system */
		if (rest[0] != '~')
			if (ckexpf(rest))
				cleanup(2);
		if (redir == '<')
			fprintf(fprx, "%c %s\n", X_STDIN, rest);
		else
			APPCMD(rest);
		redir = '\0';
		continue;

	}

	fprintf(fprx, "%c %s\n", X_CMD, cmd);
	logent(cmd, "XQT QUE'D");
	fclose(fprx);

	strcpy(tfile, rxfile);
	tfile[0] = XQTPRE;
	if (strcmp(xsys, local) == SAME) {
		link(rxfile, tfile);
		unlink(rxfile);
		if (startjob)
			if (rflag)
				xuucico("");
			else
				xuuxqt();
	}
	else {
		GENSEND(fpc, rxfile, tfile, User, rxfile);
		cflag++;
	}

	fclose(fpc);
	if (cflag) {
		gename(CMDPRE, xsys, 'A', cfile);
		link(tcfile, cfile);
		unlink(tcfile);
		if (startjob)
			xuucico(xsys);
		cleanup(0);
	}
	else
		unlink(tcfile);
}

#define FTABSIZE 30
char Fname[FTABSIZE][NAMESIZE];
int Fnamect = 0;

/***
 *	cleanup - cleanup and unlink if error
 *
 *	return - none - do exit()
 */

cleanup(code)
int code;
{
	int i;

	rmlock(CNULL);
	if (code) {
		for (i = 0; i < Fnamect; i++)
			unlink(Fname[i]);
		fprintf(stderr, "uux failed. code %d\n", code);
	}
	DEBUG(1, "exit code %d\n", code);
	exit(code);
}

/***
 *	ufopen - open file and record name
 *
 *	return file pointer.
 */

FILE *ufopen(file, mode)
char *file, *mode;
{
	if (Fnamect < FTABSIZE)
		strcpy(Fname[Fnamect++], file);
	return(fopen(file, mode));
}
