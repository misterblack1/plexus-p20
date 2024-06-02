/*
 *	tape [-9cft [tapename] ] command ["rev"] [number]
 *	cart [-cft [tapename]] command [number]
 */
#include <sys/types.h>
#include <sys/imsc.h>
#include <sys/rm.h>
#include <sys/stat.h>
#include <stdio.h>

char *tapefname = "/dev/nrrmh0";
char *atapefname = "/dev/nrmt0";	/* alternate tape name */

struct CMDTAB {
	short c;
	char *cname;
} *ctab;
struct CMDTAB  rmtab[] = {
	C_OLREW, "olrew",
	C_NOP, "nop",
	C_RFOREIGN, "rforeign",
	C_STATUS, "status",
	C_REW, "rew",
	C_UNLOAD, "unload",
	C_WEOF, "weof",
	C_SRCHEOF, "srcheof",
	C_SPACE, "space",
	C_ERASE, "erase",
	C_ERASEALL, "eraseall",
	C_SPACEEOF, "spaceeof",
	C_SRCHMEOF, "srchmeof",
	0, "",
};
struct CMDTAB pttab[] = {
	C_IREW, "rew",
	C_IWEOF, "weof",
	C_IMOVE, "srcheof",
	C_IERASE, "erase",
	C_IERASE, "eraseall",
	C_IRETENSION,"retension",
	C_IRETENSION, "ret",
	0, "",
};

struct rmcmd_struct rmcmd;

main(argc, argv)
short argc;
char *argv[];
{
	short fn;
	register char *p1;
	char *keyword;
	short cmd;
	short status;
	char *tapename; /* points to tape name that is accessed */
	char ttype;	/* type of tape: 'c' for cartridge; '9' for 9 track */
	short	cnt,
		i,
		narg,
		tflag;	/* switch to print type of tape */
	struct ptcmd ptcmd;
	struct stat ststat;	/* structure to stat the tape file */

	cnt = 0;
	narg = 1;
	tflag = 0;
	ttype = 0;
	p1 = strrchr(argv[0],'/');
	if(*p1 == '/') p1++;
	else p1 = argv[0];
	if(strcmp(p1,"cart") == 0) {
		ttype = 'c';
		tapefname = atapefname;
	}
	if(*argv[narg] == '-') for(p1 = argv[narg++]; *p1; p1++) switch (*p1) {
		case 't':	/* type of tape */
			tflag++;
			continue;
		case 'f':
			if(narg >= argc) {
				fprintf(stderr,"%s: missing tape name\n",
					argv[0]);
				exit(2);
			}
			tapefname = argv[narg];
			atapefname = argv[narg];
			narg++;
			continue;
		case 'c':
			if(ttype == '9') {
				fprintf(stderr,"%s: invalid flag: c\n",
					argv[0]);
				exit(2);
			}
			ttype = 'c';
			tapefname = atapefname;
			continue;
		case '9':
			if(ttype == 'c') {
				fprintf(stderr,"%s: invalid flag: 9\n",
					argv[0]);
				exit(2);
			}
			ttype = '9';
			continue;
		}

	tapename = tapefname;
	if ((fn = open(tapefname, 0)) < 0) {
		tapename = atapefname;
		if ((fn = open(atapefname, 0)) < 0) {
			fprintf(stderr,"%s: cannot open %s\n",argv[0],
				atapefname);
			exit(2);
		}
	}

		/* is tape file a character device?? */

	if(fstat(fn,&ststat) != 0) {
		fprintf(stderr,"%s: can't stat %s\n",argv[0],tapename);
		exit(2);
	}
	if((ststat.st_mode & S_IFMT) != S_IFCHR) {
		fprintf(stderr,"%s: %s must be character device\n",
			argv[0],tapename);
		exit(2);
	}
	if(ttype == 0 || tflag) { /* figger out which type of drive */
		if(anrmdrive(fn)) {
			ttype = '9';
		}
		else {
			ttype = 'c';
		}
	}
	if(tflag) printf("%s\n",ttype == '9' ? "9-track tape" : "cartridge");

		/* Set up default command and arguments in case there are
		   no more arguments */
	if(ttype == '9') {
		rmcmd.rm_status = -1;
		rmcmd.rm_cnt = 0;
		cmd = C_STATUS;
		ctab = rmtab;
	}
	else {
		cmd = C_IMOVE;
		ctab = pttab;
	}

	if (narg < argc) {
		for (i = 0; ctab[i].c > 0; i++) {
			if(0 == strcmp(ctab[i].cname, argv[narg])) {
				keyword = ctab[i].cname;
				break;
			}
		}
		if (ctab[i].c == 0) {
			fprintf(stderr,"%s: illegal command: %s\n",argv[0],
				argv[narg]);
			exit(2);
		}
		narg++;
		cmd = ctab[i].c;

			/* default srcheof for cartridge is 1 */
		if(cmd == C_IMOVE && ttype == 'c') cnt = 1;

		if (narg < argc) {
			if (0 == strcmp("rev", argv[narg])) {
				if(ttype == '9') cmd |= C_REV;
				narg++;
			}
			if (narg < argc) {
				cnt = atoi(argv[narg]);
			}
		}
		
	}

	if(ttype == '9') {
		rmcmd.rm_cmd = cmd;
		rmcmd.rm_cnt = cnt;
		if(cmd != C_SRCHEOF) cnt = 1;
		do {
			if (ioctl(fn, RMPOSN, &rmcmd) < 0) {
				fprintf(stderr,"%s: ioctl request %s error\n",
					argv[0], keyword);
				break;
			}
		} while (--cnt > 0);

		fprintf(stdout, "%04x", rmcmd.rm_status, rmcmd.rm_cnt);
		if (rmcmd.rm_cmd == C_RFOREIGN) {
			fprintf(stdout, " %d\n", rmcmd.rm_cnt);
		} else {
			fprintf(stdout, "\n");
		}

	}
	else {
		ptcmd.blkcnt = cnt;
		if(ioctl(fn,cmd,&ptcmd) != 0) {
			fprintf(stderr,"%s: ioctl request %s error\n",
				argv[0], keyword);
		}
		else {
			fprintf(stdout,"DONE\n");
		}
	}
	close(fn);
}
anrmdrive(fn)
short fn;
{
	
	rmcmd.rm_status = -1;
	rmcmd.rm_cnt = 0;
	rmcmd.rm_cmd = C_NOP;
	if (ioctl(fn, RMPOSN, &rmcmd) < 0) 
		return 0;
	else
		return 1;
}

