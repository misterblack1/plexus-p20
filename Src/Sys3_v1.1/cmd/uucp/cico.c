	/*  cico 3.7  1/5/80  16:32:57  */
#include "uucp.h"
#include <signal.h>
#include <setjmp.h>
#include <sys/types.h>
#include <termio.h>
#include "uust.h"
#include "uusub.h"


jmp_buf Sjbuf;
	/*  call fail text  */
char *Stattext[] = {
	"",
	"BAD SYSTEM",
	"WRONG TIME",
	"SYSTEM LOCKED",
	"NO DEVICE",
	"DIAL FAILED",
	"LOGIN FAILED",
	"BAD SEQUENCE"
	};

int Role = 0;
char *ttyname();

	/*  call fail codes  */
int Stattype[] = {0, 0, 0, 0,
	SS_NODEVICE, SS_FAIL, SS_FAIL, SS_BADSEQ
	};


int Errorrate = 0;
struct termio Savettyb;

/*******
 *	cico - this program is used  to place a call to a
 *	remote machine, login, and copy files between the two machines.
 */

main(argc, argv)
char *argv[];
{
	int ret, seq;
	int onesys = 0;
	char wkpre[NAMESIZE], file[NAMESIZE];
	char msg[BUFSIZ], *p, *q;
	extern onintr(), timeout();
	extern intrEXIT();
	extern char *pskip();
	char rflags[30];
	char *ttyn;

	strcpy(Progname, "uucico");
	signal(SIGILL, intrEXIT);
	signal(SIGTRAP, intrEXIT);
	signal(SIGIOT, intrEXIT);
	signal(SIGEMT, intrEXIT);
	signal(SIGFPE, intrEXIT);
	signal(SIGBUS, intrEXIT);
	signal(SIGSEGV, intrEXIT);
	signal(SIGSYS, intrEXIT);
	signal(SIGINT, onintr);
	signal(SIGHUP, onintr);
	signal(SIGQUIT, onintr);
	signal(SIGTERM, onintr);
	ret = guinfo(getuid(), User, msg);
	strcpy(Loginuser, User);
	ASSERT(ret == 0, "BAD UID ", "", ret);

	rflags[0] = '\0';
	uucpname(Myname);
	umask(WFMASK);
	strcpy(Rmtname, Myname);
	Ifn = Ofn = -1;
	while(argc>1 && argv[1][0] == '-'){
		switch(argv[1][1]){
		case 'd':
			Spool = &argv[1][2];
			break;
#ifdef PROTODEBUG
		case 'E':
			Errorrate = atoi(&argv[1][2]);
			if (Errorrate <= 0)
				Errorrate = 100;
			break;
		case 'g':
			Pkdrvon = 1;
			break;
		case 'G':
			Pkdrvon = 1;
			strcat(rflags, " -g ");
			break;
#endif
		case 'r':
			Role = atoi(&argv[1][2]);
			break;
		case 's':
			sprintf(Rmtname, "%.7s", &argv[1][2]);
			if (Rmtname[0] != '\0')
				onesys = 1;
			break;
		case 'x':
			Debug = atoi(&argv[1][2]);
			if (Debug <= 0)
				Debug = 1;
			strcat(rflags, argv[1]);
			break;
		default:
			printf("unknown flag %s\n", argv[1]);
			break;
		}
		--argc;  argv++;
	}

	chdir(Spool);
	strcpy(Wrkdir, Spool);

	if (Role == SLAVE) {
		/* initial handshake */
		onesys = 1;
		ret = ioctl(0, TCGETA, &Savettyb);
		Savettyb.c_cflag = (Savettyb.c_cflag & ~CS8) | CS7;
		Savettyb.c_oflag |= OPOST;
		Savettyb.c_lflag |= (ISIG|ICANON|ECHO);
		Ifn = 0;
		Ofn = 1;
		fixmode(Ifn);
		fclose(stderr);
		fopen(RMTDEBUG, "w");
		omsg('S', "here", Ofn);
		signal(SIGALRM, timeout);
		alarm(MAXMSGTIME);
		if (setjmp(Sjbuf)) {
			/* timed out */
			ret = ioctl(0, TCSETA, &Savettyb);
			exit(0);
		}
		for (;;) {
			ret = imsg(msg, Ifn);
			if (ret != 0) {
				alarm(0);
				ret = ioctl(0, TCSETA, &Savettyb);
				exit(0);
			}
			if (msg[0] == 'S')
				break;
		}
		alarm(0);
		q = &msg[1];
		p = pskip(q);
		sprintf(Rmtname, "%.7s", q);
		DEBUG(4, "sys-%s\n", Rmtname);
		if (mlock(Rmtname)) {
			omsg('R', "LCK", Ofn);
			cleanup(0);
		}
		else if (callback(Loginuser)) {
			signal(SIGINT, SIG_IGN);
			signal(SIGHUP, SIG_IGN);
			omsg('R', "CB", Ofn);
			logent("CALLBACK", "REQUIRED");
			/*  set up for call back  */
			systat(Rmtname, SS_CALLBACK, "CALL BACK");
			gename(CMDPRE, Rmtname, 'C', file);
			close(creat(file, 0666));
			xuucico(Rmtname);
			cleanup(0);
		}
		seq = 0;
		while (*p == '-') {
			q = pskip(p);
			switch(*(++p)) {
			case 'g':
				Pkdrvon = 1;
				break;
			case 'x':
				Debug = atoi(++p);
				if (Debug <= 0)
					Debug = 1;
				break;
			case 'Q':
				seq = atoi(++p);
				break;
			default:
				break;
			}
			p = q;
		}
		if (callok(Rmtname) == SS_BADSEQ) {
			logent("BADSEQ", "PREVIOUS");
			omsg('R', "BADSEQ", Ofn);
			cleanup(0);
		}
		if ((ret = gnxseq(Rmtname)) == seq) {
			omsg('R', "OK", Ofn);
			cmtseq();
		}
		else {
			systat(Rmtname, Stattype[7], Stattext[7]);
			logent("BAD SEQ", "HANDSHAKE FAILED");
			ulkseq();
			omsg('R', "BADSEQ", Ofn);
			cleanup(0);
		}
	}
loop:
	if (!onesys) {
		ret = gnsys(Rmtname, Spool, CMDPRE);
		if (ret == FAIL)
			cleanup(100);
		if (ret == 0)
			cleanup(0);
	}
	else if (Role == MASTER && callok(Rmtname) != 0) {
		logent("SYSTEM STATUS", "CAN NOT CALL");
		cleanup(0);
	}

	sprintf(wkpre, "%c.%.7s", CMDPRE, Rmtname);

	if (Role == MASTER) {
		/*  master part */
		signal(SIGINT, SIG_IGN);
		signal(SIGHUP, SIG_IGN);
		signal(SIGQUIT, SIG_IGN);
		if (!iswrk(file, "chk", Spool, wkpre) && !onesys) {
			logent(Rmtname, "NO WORK");
			goto next;
		}
		if (Ifn != -1 && Role == MASTER) {
			write(Ofn, EOTMSG, strlen(EOTMSG));
			close(Ofn);
			close(Ifn);
			Ifn = Ofn = -1;
			rmlock(CNULL);
			clsacu();
			sleep(3);
		}
		sprintf(msg, "call to %s ", Rmtname);
		if (mlock(Rmtname) != 0) {
			logent(msg, "LOCKED");
			US_SST(us_s_lock);
			goto next;
		}
		Ofn = Ifn = conn(Rmtname);
		if (Ofn < 0) {
			logent(msg, "FAILED");
			US_SST(-Ofn);
			UB_SST(-Ofn);
			systat(Rmtname, Stattype[-Ofn],
				Stattext[-Ofn]);
			goto next;
		}
		else {
			logent(msg, "SUCCEEDED");
			US_SST(us_s_cok);
			UB_SST(ub_ok);
		}
	
		if (setjmp(Sjbuf))
			goto next;
		signal(SIGALRM, timeout);
		alarm(2 * MAXMSGTIME);
		for (;;) {
			ret = imsg(msg, Ifn);
			if (ret != 0) {
				alarm(0);
				goto next;
			}
			if (msg[0] == 'S')
				break;
		}
		alarm(MAXMSGTIME);
		seq = gnxseq(Rmtname);
		sprintf(msg, "%.7s -Q%d %s", Myname, seq, rflags);
		omsg('S', msg, Ofn);
		for (;;) {
			ret = imsg(msg, Ifn);
			DEBUG(4, "msg-%s\n", msg);
			if (ret != 0) {
				alarm(0);
				ulkseq();
				goto next;
			}
			if (msg[0] == 'R')
				break;
		}
		alarm(0);
		if (msg[1] == 'B') {
			/* bad sequence */
			logent("BAD SEQ", "HANDSHAKE FAILED");
			US_SST(us_s_hand);
			systat(Rmtname, Stattype[7], Stattext[7]);
			ulkseq();
			goto next;
		}
		if (strcmp(&msg[1], "OK") != SAME)  {
			logent(&msg[1], "HANDSHAKE FAILED");
			US_SST(us_s_hand);
			ulkseq();
			goto next;
		}
		cmtseq();
	}
	ttyn = ttyname(Ifn);
	if (ttyn != NULL)
		chmod(ttyn, 0600);
	DEBUG(1, " Rmtname %s, ", Rmtname);
	DEBUG(1, "Role %s,  ", Role ? "MASTER" : "SLAVE");
	DEBUG(1, "Ifn - %d, ", Ifn);
	DEBUG(1, "Loginuser - %s\n", Loginuser);

	ret = startup(Role);
	if (ret != SUCCESS) {
		logent("startup", "FAILED");
		US_SST(us_s_start);
		systat(Rmtname, SS_FAIL, "STARTUP");
		goto next;
	}
	else {
		logent("startup", "OK");
		US_SST(us_s_gress);
		systat(Rmtname, SS_INPROGRESS, "TALKING");
		ret = cntrl(Role, wkpre);
		DEBUG(1, "cntrl - %d\n", ret);
		signal(SIGINT, SIG_IGN);
		signal(SIGHUP, SIG_IGN);
		signal(SIGALRM, timeout);
		if (ret == 0) {
			logent("conversation complete", "OK");
			US_SST(us_s_ok);
			rmstat(Rmtname);

		}
		else {
			logent("conversation complete", "FAILED");
			US_SST(us_s_cf);
			systat(Rmtname, SS_FAIL, "CONVERSATION");
		}
		alarm(MAXMSGTIME);
		omsg('O', "OOOOO", Ofn);
		DEBUG(4, "send OO %d,", ret);
		if (!setjmp(Sjbuf)) {
			for (;;) {
				omsg('O', "OOOOO", Ofn);
				ret = imsg(msg, Ifn);
				if (ret != 0)
					break;
				if (msg[0] == 'O')
					break;
			}
		}
		alarm(0);
	}
next:
	if (!onesys) {
		goto loop;
	}
	cleanup(0);
}

/***
 *	cleanup(code)	cleanup and exit with "code" status
 *	int code;
 */

cleanup(code)
int code;
{
	int ret;
	char *ttyn;

	signal(SIGINT, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	rmlock(CNULL);
	clsacu();
	logcls();
	if (Role == SLAVE) {
		Savettyb.c_cflag |= HUPCL;
		ret = ioctl(0, TCSETA, &Savettyb);
		DEBUG(4, "ret ioctl - %d\n", ret);
	}
	if (Ofn != -1) {
		if (Role == MASTER)
			write(Ofn, EOTMSG, strlen(EOTMSG));
		ttyn = ttyname(Ifn);
		if (ttyn != NULL)
			chmod(ttyn, 0666);
		close(Ifn);
		close(Ofn);
	}
	DEBUG(1, "exit code %d\n", code);
	if (code == 0)
		xuuxqt();
	exit(code);
}

/***
 *	onintr(inter)	interrupt - remove locks and exit
 */

onintr(inter)
int inter;
{
	char str[30];
	signal(inter, SIG_IGN);
	sprintf(str, "SIGNAL %d", inter);
	logent(str, "CAUGHT");
	cleanup(inter);
}

intrEXIT()
{
	signal(SIGIOT, SIG_DFL);
	signal(SIGILL, SIG_DFL);
	setuid(getuid());
	abort();
}

/***
 *	fixmode(tty)	fix kill/echo/raw on line
 *
 *	return codes:  none
 */

fixmode(tty)
int tty;
{
	struct termio ttbuf;
	int ret;

	ioctl(tty, TCGETA, &ttbuf);
	ttbuf.c_iflag = ttbuf.c_oflag = ttbuf.c_lflag = (ushort)0;
	ttbuf.c_cflag &= (CBAUD);
	ttbuf.c_cflag |= (CS8|CREAD);
	ttbuf.c_cc[VMIN] = 6;
	ttbuf.c_cc[VTIME] = 1;
	ret = ioctl(tty, TCSETA, &ttbuf);
	ASSERT(ret >= 0, "STTY FAILED", "", ret);
	return;
}


/***
 *	timeout()	catch SIGALRM routine
 */

timeout()
{
	longjmp(Sjbuf, 1);
}

static char *
pskip(p)
register char *p;
{
	while( *p && *p != ' ' )
		++p;
	if( *p ) *p++ = 0;
	return(p);
}
