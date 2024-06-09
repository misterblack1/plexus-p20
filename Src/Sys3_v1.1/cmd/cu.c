#define void int
/***************************************************************
 *	cu [-s speed] [-a acu] [-l line] [ h ] [-o | -e] telno | 'dir'
 *
 *	speeds are: 110, 150, 300, 1200, 4800, 9600.
 *		Set default speed by defining the value of BAUD.
 *	-l and -a are for line and unit arguments from the file whose
 *		name is defined under LDEVS below.
 *	-h is for half-duplex (local echoing).
 *	-d can be used (with ddt) to get some tracing & diagnostics.
 *	-o or -e is for odd or even parity on transmission to remote.
 *	telno is either a telephone number or "dir" for a direct line.
 *	If "dir" is used, the the "-l dev" option is required.  If
 *	"-l dev", or "-a acu" are used, speed is taken from LDEVS.
 *
 *	Escape with `~' at beginning of line.
 *	Silent output diversions are ~>:filename and ~>>:filename.
 *	Terminate output diversion with ~> alone.
 *	~. is quit, and ~! gives local command or shell.
 *	Also ~$ for canned local procedure pumping remote.
 *	Both ~%put from [to]  and  ~%take from [to] invoke built-ins.
 *	Also, ~%break or just ~%b will transmit a BREAK to remote.
 *	~nostop turns off the DC3/DC1 input control from remote,
 *		(certain remote systems cannot cope with DC3 or DC1).
 *
 *	As a device-lockout semaphore mechanism, create an entry
 *	in the directory #defined as LOCK whose name is LCK..dev
 *	where dev is the device name taken from the "line" column
 *	in the file #defined as LDEVS.  Be sure to trap every possible
 *	way out of cu execution in order to "release" the device.
 *	Also, have the system start-up procedure clean all such
 *	entries from the LOCK directory.
 ***************************************************************/
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <termio.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#define	LDEVS	"/usr/lib/uucp/L-devices"
#define	LOCK	"/usr/spool/uucp/LCK.."	/* where to synch with uucp */
#define	BAUD	"300"		/* default baud rate */
#define	ACULAST	"<"		/* character which terminates dialing*/
#define	RUB	'\177'		/* mnemonic */
#define	XON	'\21'		/* mnemonic */
#define	XOFF	'\23'		/* mnemonic */
#define	TTYIN	0		/* mnemonic */
#define	TTYOUT	1		/* mnemonic */
#define	TTYERR	2		/* mnemonic */
#define	YES	1		/* mnemonic */
#define	NO	0		/* mnemonic */
#define	EQUALS	!strcmp		/* mnemonic */
#define	DIFFER	strcmp		/* mnemonic */
#define	COPY	strcpy		/* mnemonic */
#define	EXEC(A, B, C) execl("/bin/sh", A, B, C, 0)

FILE	*Ldevices;		/* file pointer for Device name file */
struct	termio	tv, tv0, lv;

union {				/* this helps to quiet lint */
	int	i;
	unsigned char u;
	char	c;
} cx;

char
	tkill,			/* current input kill */
	terase,			/* current input erase */
	*devtype="ACU",		/* indicates dial or direct connect */
	*acu="",		/* ptr to name of requested ACU */
	*line="",		/* ptr to name of requested device */
	dspeed[6]=BAUD,		/* default baud rate */
	*speed,			/* ptr to requested line speed */
	*telno,			/* ptr to dialed tel-no or "dir" */
	cul[50],		/* line chosen */
	cua[50],		/* acu's device-name */
	lock[30+sizeof(LOCK)],	/* directory in which to make lockfile*/
	*strtok(),
	*strcat(),
	*strncpy(),
	*strpbrk(),
	*strcpy();

unsigned sleep(), alarm();
extern	errno;

int
	found=0,		/* set when device is seen legal */
	linarg,			/* length of arg for `-l' option */
	acuarg,			/* length of arg for `-a' option */
	rlfd,			/* fd for remote comm line */
	lfd= -1,		/* fd for the device-lock file */
	parity=0,		/* 0 for no parity, -1 odd, 1 even */
	intrupt=NO,		/* interrupt indicator */
#ifdef	ddt
	debug=NO,		/* flag turns on more diagnostics */
#endif
	duplex=YES,		/* half(NO), or full(YES) duplex */
	sstop=YES,		/* NO means remote can't XON/XOFF */
	rtn_code=1,
	takeflag=NO;		/* indicates a ~%take is in progress */
	onintrpt(),		/* interrupt routine */
	sigalarm(),		/* interrupt routine */
	hangup(),		/* interrupt routine */
	byebye(),		/* interrupt routine */
	die(),			/* interrupt routine */
	strcmp();

char	*msg[]= {
/*  0*/	"usage: cu [-s speed] [-a acu] [-l line] [ h ] [-o | -e] telno | \"dir\"\n",
/*  1*/	"interrupt",
/*  2*/	"dialer hung",
/*  3*/	"no answer",
/*  4*/	"can't fork",
/*  5*/	"acu problem",
/*  6*/	"line problem",
/*  7*/	"line hung",
/*  8*/	"Can not open: %s\r\n",
/*  9*/	"Connect failed: %s\r\n",
/* 10*/	"No line %s at %s baud\r\n",
/* 11*/	"\"%s\" with \"%s\" not %s at %s baud\r\n",
/* 12*/	"Device \"%s\" not %s\r\n",
/* 13*/	"Line gone\r\n",
/* 14*/	"Can't execute shell\r\n",
/* 15*/	"Can't divert %s\r\n",
/* 16*/	"Use `~~' to start line with `~'\r\n",
/* 17*/	"character missed\r\n",
/* 18*/	"after %ld bytes\r\n",
/* 19*/	"%d lines/%ld characters\r\n",
/* 20*/	"Only digits & '-'s or '='s in telno\n",
/* 21*/	"file transmission interrupted\r\n"
};

/***************************************************************
 *	main: get connection, and fork.
 *	Child invokes "receive" to read from remote & write to tty.
 *	Main line invokes "transmit" to read tty & write to remote.
 ***************************************************************/

main(argc, argv)
char *argv[];
{
	int	fk, errflag=0;
	extern	int	optind;
	extern	char	*optarg;

	speed = dspeed;
	while((cx.i = getopt(argc, argv, "dheos:l:a:")) != EOF)
		switch(cx.i) {
			case 'd':
#ifdef	ddt
				debug = YES;
#else
				++errflag;
#endif
				break;
			case 'h':
				duplex ^= YES;
				break;
			case 'e':
				if(++parity != 1)
					++errflag;
				break;
			case 'o':
				if(--parity != -1)
					++errflag;
				break;
			case 's':
				speed = optarg;
			  	break;
			case 'l':
				line = optarg;
			  	break;
			case 'a':
				acu = optarg;
			  	break;
			case '?':
				++errflag;
		}

	linarg = strlen(line);
	acuarg = strlen(acu);

	if(optind < argc && optind > 0) {
	    telno = argv[optind];
	    if(strlen(telno) != strspn(telno, "0123456789=-"))
		if(EQUALS(telno,"dir")) {
			if(linarg == 0)
				++errflag;
			devtype = COPY(devtype, "DIR");
		} else {
			display(msg[20]);
			++errflag;
		}
	} else
	    ++errflag;

	if(errflag) {
		display(msg[0]);
		exit(1);
	}

	if((Ldevices = fopen(LDEVS, "r")) == NULL) {
		display(msg[8], LDEVS);
		exit(1);
	}
	(void) ioctl(TTYIN, TCGETA, &tv0); /* save initial tty state */
	tkill = tv0.c_cc[VKILL];
	terase = tv0.c_cc[VERASE];

	(void) signal(SIGHUP, die);
	(void) signal(SIGQUIT, die);
	(void) signal(SIGINT, onintrpt);
	(void) signal(SIGALRM, sigalarm);

	while(1) {
		char	dvc[50], *find_dev();

		(void) COPY(dvc, find_dev(acu, line, speed));
		if(strlen(dvc) == 0) {
		    char  *why;

			why = found? "available": "known";
		    if(acuarg == 0 && linarg == 0)
			display(msg[10], why, speed);
		    else
	 		if(acuarg != 0 && linarg != 0)
			    display(msg[11], line, acu, why, speed);
			else
			    display(msg[12], linarg? line: acu, why);
		    die(8);
		}
		(void) COPY(lock, LOCK);
		(void) strcat(lock, dvc);
		if(geteuid() == 0 && access(lock, 0) == 0)
			continue;
		if((lfd = creat(lock, 0444)) < 0)
			continue;
		else {
			cx.i = getpid();
			(void) write(lfd, &cx.c, sizeof(cx.i));
		}
		(void) fclose(Ldevices);
		if(intrupt == YES)
			die(-1);
		if((rlfd = connect(cul, cua, telno, speed)) < 0) {
			display(msg[9], msg[-rlfd]);
			die(-rlfd);
		}
		break;
	}

	/* When we get this far we have an open communication line */

	mode(1);			/* put terminal in `raw' mode */

	display("Connected\r\n");
	
	if((fk = dofork()) == 0) {
		(void) signal(SIGHUP, hangup);
		(void) signal(SIGINT, SIG_IGN);
		(void) signal(SIGQUIT, hangup);
		receive();	/* This should run until killed */
		display("\r\n\07Lost carrier\r\n");
#ifdef ddt
		if(debug == YES) tdmp(rlfd);
#endif
		hangup();
	}
	(void) signal(SIGUSR1, byebye);
	(void) signal(SIGHUP, SIG_IGN);
	(void) signal(SIGQUIT, SIG_IGN);
	rtn_code = transmit();	/* This is the parent proccess */
	(void) kill(fk, SIGKILL); /* Kill paired "receive" proccess */
	byebye();
}

/***************************************************************
 *	transmit: copy stdin to rlfd, except:
 *	~.	terminate
 *	~!	local login-style shell
 *	~!cmd	execute cmd locally
 *	~$proc	execute proc locally, send output to line
 *	~%cmd	execute builtin cmd (put, take, or break)
 ****************************************************************/

transmit()
{
	char	b[200];
	register char	*p;
	register int	escape;

#ifdef	ddt
	if(debug == YES) display("transmit started\n\r");
#endif
	while(1) {
		p = b;
		while(r_char(TTYIN) == YES) {
			if(p == b)	/* Escape on leading  ~    */
				escape = (cx.c == '~');
			if(p == b+1)	/* But not on leading ~~   */
				escape &= (cx.c != '~');
			if(escape) {
				if(cx.c == '\r' || cx.c == '\n') {
					*p = '\0';
					if(tilda(b+1) == YES)
						return(0);
					break;
				}
				if(cx.c == RUB || cx.c == tkill
						|| cx.c == '\0') {
					display("\r\n");
					break;
				}
				if(w_char(TTYERR) == NO)
					return(7);	/* tty gone */
				if(cx.c == terase) {
					p = (--p < b)? b:p;
					continue;
				}
			} else {
				if(cx.c == '\0') {
					(void) ioctl(rlfd, TCSBRK, 0);
					flush();
					break;
				}
				if(w_char(rlfd) == NO) {
					display(msg[13]);
					return(2);
				}
				if(duplex == NO)
					if(w_char(TTYERR) == NO)
						return(7);
				if(cx.c == RUB) {
					flush();
					break;
				}
				if(cx.c == '\n' || cx.c == '\r'
					|| cx.c == tkill)
					break;
			}
			*p++ = cx.c;
		}
	}
}

flush()	/* routine to halt input from remote and flush buffers */
{
	(void) ioctl(TTYOUT, TCXONC, 0);
	(void) ioctl(rlfd, TCFLSH, 0);
	(void) ioctl(TTYOUT, TCFLSH, 1);
	(void) ioctl(TTYOUT, TCXONC, 1);
	if(takeflag == NO) {
		return;
	}
	display(msg[21]);
	(void) sleep(3);
	w_str("echo '~>';mesg y\n");
	takeflag = NO;
}

tilda(cmd)
char	*cmd;
{
	display("\r\n");
#ifdef	ddt
	if(debug == YES) display("call tilda(%s)\r\n", cmd);
#endif
	switch(cmd[0]) {
		case '.':
			if(EQUALS(devtype,"DIR"))
				if(cmd[1] != '.')
					w_str("\04\04\04\04\04");
			return(YES);
		case '!':
		case '$':
			shell(cmd);	/* Local shell */
			display("\r%c\r\n", *cmd);
			w_str("\n");
			break;
		case '%':
			dopercen(++cmd);
			break;
#ifdef ddt
		case 't':
			tdmp(TTYIN);
			break;
		case 'l':
			tdmp(rlfd);
			break;
#endif
		default:
			display(msg[16]);
	}
	return(NO);
}

/***************************************************************
 *	The routine "shell" takes an argument starting with
 *	either "!" or "$", and terminated with '\0'.
 *	If $arg, arg is the name of a local shell file which
 *	is executed and its output is passed to the remote.
 *	If !arg, we escape to a local shell to execute arg
 *	with output to TTY, and if arg is null, escape to
 *	a local shell and blind the remote line.  In either
 *	case, RUBout or '^D' will kill the escape status.
 **************************************************************/

shell(str)
char	*str;
{
	int	fk;

#ifdef	ddt
	if(debug == YES) display("call shell(%s)\r\n", str);
#endif
	fk = dofork();
	if(fk == 0) {
		/***********************************************
		 * Hook-up our "standard output"
		 * to either the tty or the line
		 * as appropriate for '!' or '$'
		 ***********************************************/
		(void) close(TTYOUT);
		(void) fcntl((*str == '$')? rlfd:TTYERR,F_DUPFD,TTYOUT);
		(void) close(rlfd);
		mode(0);	/* normal */
		(void) signal(SIGINT, SIG_DFL);
		if(*++str == '\0')
			EXEC("-", (char *) 0, (char *) 0);
		else
			EXEC("sh", "-c", str);
		display(msg[14]);
		exit(0);
	}
	if(fk > 0)
		while(wait((int *) 0) != fk);
	mode(1);
}

/***************************************************************
 *	This function implements the 'put', 'take', and 'break'
 *	commands which are internal to cu.
 ***************************************************************/

dopercen(cmd)
register char *cmd;
{
	char	*arg[5];
	int	narg;

#ifdef	ddt
	if(debug == YES) display("call dopercen(\"%s\")\r\n", cmd);
#endif
	arg[narg=0] = strtok(cmd, " \t\n");
	/* following loop breaks out the command and args */
	while((arg[++narg] = strtok((char*) NULL, " \t\n")) != NULL) {
		if(narg < 5)
			continue;
		else
			break;
	}

	if(EQUALS(arg[0], "take")) {
		if(narg < 2 || narg > 3) {
			display("usage: ~%%take from [to]\r\n");
			return;
		}
		if(narg == 2)
			arg[2] = arg[1];
		w_str("mesg n;echo '~>':");
		w_str(arg[2]);
		w_str(";cat ");
		w_str(arg[1]);
		w_str(";echo '~>';mesg y\n");
		takeflag = YES;
		return;
	}
	else if(EQUALS(arg[0], "put")) {
		FILE	*file;
		char	buf[512], spec[3], *b, *p, *q;
		int	len, tc=0, lines=0;
		long	chars=0L;

		if(narg < 2 || narg > 3) {
			display("usage: ~%%put from [to]\r\n");
			goto R;
		}
		if(parity) {
			display("parity problem\r\n");
			goto R;
		}
		if(narg == 2)
			arg[2] = arg[1];
		if((file = fopen(arg[1], "r")) == NULL) {
			display(msg[8], arg[1]);
R:
			w_str("\n");
			return;
		}
		w_str("stty -echo; cat - > ");
		w_str(arg[2]);
		w_str("; stty echo\n");
		intrupt = NO;
		mode(2);
		spec[0] = terase;
		spec[1] = tkill;
		spec[2] = '\0';
		(void) sleep(5);
		while(intrupt == NO &&
				fgets(b= &buf[256],256,file) != NULL) {
			len = strlen(b);
			chars += len;		/* character count */
			p = b;
			while(q = strpbrk(p, spec)) {
				b = strncpy(b-1, b, q-b);
				*(q-1) = '\\';
				p = q+1;
			}
			if((tc += len) >= 256) {
				(void) sleep(1);
				tc = len;
			}
			if(write(rlfd, b, strlen(b)) < 0) {
				display(msg[17]);
				intrupt = YES;
				break;
			}
			++lines;		/* line count */
		}
		mode(1);
		(void) fclose(file);
		if(intrupt == YES) {
			intrupt = NO;
			display(msg[21]);
			w_str("\n");
			display(msg[18], ++chars);
		} else
			display(msg[19], lines, chars);
		w_str("\04");
		(void) sleep(3);
		return;
	}
	else if(EQUALS(arg[0], "b") || EQUALS(arg[0], "break")) {
		(void) ioctl(rlfd, TCSBRK, 0);
		return;
	}
	else if(EQUALS(arg[0], "nostop")) {
		(void) ioctl(rlfd, TCGETA, &tv);
		tv.c_iflag &= ~IXOFF;
		(void) ioctl(rlfd, TCSETA, &tv);
		sstop = NO;
		mode(1);
		return;
	}
	display("~%%%s unknown to cu\r\n", arg[0]);
}

/***************************************************************
 *	receive: read from remote line, write to fd=1 (TTYOUT)
 *	catch:
 *	~>[>]:file
 *	.
 *	. stuff for file
 *	.
 *	~>	(ends diversion)
 ***************************************************************/

receive()
{
	register silent=NO, file;
	register char *p;
	int	tic;
	char	b[512];
	long	lseek(), count;

#ifdef	ddt
	if(debug == YES) display("receive started\r\n");
#endif
	file = -1;
	p = b;
	while(r_char(rlfd) == YES) {
		if(silent == NO)
			if(w_char(TTYOUT) == NO)
				return;
		*p++ = cx.c;
		if(cx.c != '\n' && (p-b) < 512)
			continue;
		/* remove (CR) and junk inserted at remote */
		while(p-2 >= b && (*(p-2) == '\r' || *(p-2) == '\0')) {
			--p;
			*(p-1) = *p;
		}
		/***********************************************
		 * The rest of this code is to deal with what
		 * happens at the beginning, middle or end of
		 * a diversion to a file.
		 ************************************************/
		if(b[0] == '~' && b[1] == '>') {
			/****************************************
			 * The line is the beginning or
			 * end of a diversion to a file.
			 ****************************************/
			if((file < 0) && (b[2] == ':' || b[2] == '>')) {
				/**********************************
				 * Beginning of a diversion
				 *********************************/
				int	append;

				*(p-1) = NULL; /* terminate file name */
				append = (b[2] == '>')? 1:0;
				p = b + 3 + append;
				if(append && (file=open(p,O_WRONLY))>0)
					(void) lseek(file, 0L, 2);
				else
					file = creat(p, 0666);
				if(file < 0)
					display(msg[15], p);
				else {
					silent = YES; 
					count = tic = 0;
				}
			} else {
				/*******************************
				 * End of a diversion (or queer data)
				 *******************************/
				if(b[2] != '\n')
					goto D;		/* queer data */
				if(silent = close(file)) {
					display(msg[15], b);
					silent = NO;
				}
				display("~>\r\n");
				display(msg[19], tic, count);
				file = -1;
			}
		} else {
			/***************************************
			 * This line is not an escape line.
			 * Either no diversion; or else yes, and
			 * we've got to divert the line to the file.
			 ***************************************/
D:
			if(file > 0) {
				(void) write(file, b, p-b);
				count += p-b;	/* tally char count */
				++tic;		/* tally lines */
			}
		}
		p = b;
	}
}

/***************************************************************
 *	change the TTY attributes of the users terminal:
 *	0 means restore attributes to pre-cu status.
 *	1 means set `raw' mode for use during cu session.
 *	2 means like 1 but accept interrupts from the keyboard.
 ***************************************************************/
mode(arg)
{
#ifdef	ddt
	if(debug == YES) display("call mode(%d)\r\n", arg);
#endif
	if(arg == 0) {
		(void) ioctl(TTYIN, TCSETAW, &tv0);
	} else {
		(void) ioctl(TTYIN, TCGETA, &tv);
		if(arg == 1) {
			tv.c_iflag &= ~(INLCR | ICRNL | BRKINT);
			if(sstop == YES)
				tv.c_iflag |= IXON;
			else
				tv.c_iflag &= ~IXON;
			tv.c_oflag |= OPOST;
			tv.c_oflag &= ~(OLCUC | ONLCR | OCRNL | ONOCR
						| ONLRET);
			tv.c_lflag &= ~(ICANON | ECHO | ISIG);
			tv.c_cc[VMIN] = '\01';
			tv.c_cc[VTIME] = '\0';
		}
		if(arg == 2) {
			tv.c_iflag |= (IXON | BRKINT);
			tv.c_lflag |= ISIG;
		}
		(void) ioctl(TTYIN, TCSETAW, &tv);
	}
}

#ifdef ddt
tdmp(arg)
{
	struct termio xv;
	int	i;

	(void) printf("\rdevice status for fd=%d\n", arg);
	(void) printf("\rF_GETFL=%o,", fcntl(arg, F_GETFL,1));
	if(ioctl(arg, TCGETA, &xv) < 0) {
		char	buf[100];
		(void) sprintf(buf, "\rtdmp for fd=%d:", arg);
		perror(buf);
		return;
	}
	(void) printf("iflag=`%o',", xv.c_iflag);
	(void) printf("oflag=`%o',", xv.c_oflag);
	(void) printf("cflag=`%o',", xv.c_cflag);
	(void) printf("lflag=`%o',", xv.c_lflag);
	(void) printf("line=`%o'\r\n", xv.c_line);
	(void) printf("cc[0]=`%o',", xv.c_cc[0]);
	for(i=1; i<8; ++i)
		(void) printf("[%d]=`%o',", i, xv.c_cc[i]);
	(void) printf("\r\n");
}
#endif

dofork()
{
	register x, i=0;

	while(++i < 6)
		if((x = fork()) != -1)
			return(x);
#ifdef	ddt
	if(debug == YES) perror("dofork");
#endif
	display(msg[4]);
	return(x);
}

r_char(fd)
{
	extern	errno;
	int	rtn;
	char	str[30];

	errno = 0;
	if((rtn = read(fd, &cx.c, 1)) != 1) {
	    (void) sprintf(str,"read from fd=%d returned %d", fd, rtn);
	    perror(str);
#ifdef ddt
	    if(debug == YES) tdmp(fd);
#endif
	}
	return((rtn == 1)? YES:NO);
}

w_char(fd)
{
	return((write(fd, &cx.c, 1) == 1)? YES:NO);
}

/*VARARGS1*/
display(fmt, arg1, arg2, arg3, arg4, arg5)
char	*fmt;
{
	(void) fprintf(stderr, fmt, arg1, arg2, arg3, arg4, arg5);
}

w_str(string)
register char *string;
{
	int	len;

	len = strlen(string);
	if(write(rlfd, string, len) != len)
		display(msg[13]);
}

onintrpt()
{
	(void) signal(SIGINT, onintrpt);
	intrupt = YES;
}

sigalarm()
{
	(void) signal(SIGALRM, sigalarm);
}

byebye()	/* this is executed only in the parent proccess */
{
	(void) wait((int *) 0);
	display("\r\nDisconnected\r\n");
	die(rtn_code);
}

die(arg)	/* this is executed only in the parent proccess */
{
#ifdef ddt
	if(debug == YES) display("call die(%d)\r\n", arg);
#endif
	if(rlfd > 0)
		(void) close(rlfd);
	if(lfd > 0) {
		if(fork() == 0) {
			if(EQUALS(devtype, "ACU")) {
			/* hold lock-file entry 'till acu recovers */
				(void) signal(SIGINT, SIG_IGN);
				(void) signal(SIGALRM, SIG_IGN);
				(void) signal(SIGHUP, SIG_IGN);
				(void) sleep(12);
			}
			(void) close(lfd);
			if(unlink(lock) < 0)
				display("Can't unlink lock-file\n");
#ifdef ddt
			else if(debug == YES)
				display("Lock-file unlinked\n");
#endif
			exit(0);
		}
	}
	mode(0);	/* restore users prior tty status */
	exit(arg);
}

hangup()	/* this is executed only in the receive proccess */
{
	exit(kill(getppid(), SIGUSR1));
}

/***************************************************************
 *	connect: establish dial-out connection.
 *	Negative values returned (-2...-7) are error message indices.
 *	Be sure to disconnect tty when done, via HUPCL or stty 0.
 ***************************************************************/

connect(dev, xacu, xtelno, wspd)
char	*dev, *xacu, *xtelno, *wspd;
{
	int	er=0, dum, fdac, fd=0, t, w;
	char	sp_code, b[30];
	extern	errno;

#ifdef	ddt
	if(debug == YES) display("call connect(%s,%s,%s,%s)\r\n", dev,
						xacu, xtelno, wspd);
#endif
	switch(atoi(wspd)) {
		case 110:
			sp_code = B110;
			break;
		case 150:
			sp_code = B150;
			break;
		case 300:
			sp_code = B300;
			break;
		case 1200:
			sp_code = B1200;
			break;
		case 4800:
			sp_code = B4800;
			break;
		case 9600:
			sp_code = B9600;
			break;
		default:
			sp_code = B300;
	}
	if((fd = open(dev, O_RDWR | O_NDELAY)) < 0) {
		perror(dev);
		er = 6;
		goto X;
	}
	if((t = ioctl(fd, TCGETA, &lv)) < 0)
		perror("gtty for remote");
	else {
		lv.c_cflag &= ~CBAUD;
		lv.c_cflag |= (CREAD | HUPCL | sp_code);
		if(parity) {
			lv.c_cflag |= PARENB;
			lv.c_cflag &= ~CSIZE;
			lv.c_cflag |= CS7;
			if(parity < 0)
				lv.c_cflag |= PARODD;
		}
		if(EQUALS(devtype, "DIR"))
			lv.c_cflag |= CLOCAL;
		lv.c_iflag |= (IGNPAR | IGNBRK | ISTRIP | IXON | IXOFF);
		lv.c_iflag &= ~(INLCR | ICRNL);
		lv.c_cc[VERASE] = '\0';
		lv.c_cc[VKILL] = '\0';
		lv.c_cc[VMIN] = '\1';
		lv.c_cc[VTIME] = '\0';
		lv.c_lflag &= ~(ICANON | ECHO | ISIG);
		if((t = ioctl(fd, TCSETA, &lv)) < 0)
			perror("stty for remote");
	}
	if(t < 0) {
		er = (errno == EINTR? 7:6);
		goto X;
	}
	if(EQUALS(devtype, "ACU")) {
		struct stat statbuf;
		char execbuf[100];
		if(stat(xacu, &statbuf) == 0) {
			switch (statbuf.st_mode) {
			case S_IFCHR:
			case S_IFDIR:
			case S_IFIFO:
			case S_IFBLK:
				break;
			default:
				sprintf(execbuf,"%s %s %s %s",xacu,dev,speed,
					xtelno);
				display("DIALER executing %s\n",execbuf);
				if (system(execbuf)>>8 != 0) {
					er = 2;
					goto X;
				}
				goto DDONE;
			}
		}
			
		if((fdac = open(xacu, O_WRONLY)) < 0) {
			perror(xacu);
			er = 5;
			goto X;
		}
		t = strlen(strcat(COPY(b,xtelno),ACULAST));
#ifdef	ddt
		if(debug == YES) display("dialing %s\n", b);
#endif
		(void) alarm((unsigned) (2*t+5));
		w = write(fdac, b, t);		/* dial the number */
		(void) alarm(0);
		if(w != t) {
			perror("write to acu");
			er = (errno == EINTR)? 2:5;
			goto X;
		}
		(void) close(fdac);	/* dialing is complete */
#ifdef	ddt
		if(debug == YES) display("dialing complete\n");
#endif
	}
DDONE:
	(void) alarm((unsigned) (t+15));
	dum = open(dev, O_RDWR);	/* wait here for carrier */
	(void) alarm(0);
	if(dum < 0) {
#ifdef	ddt
		if(debug == YES) perror(dev);
#endif
		er = (errno == EINTR)? 3:6;
		goto X;
	}
	(void) close(dum);	/* the dummy open used for waiting*/
	(void) fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) & ~O_NDELAY);
#if ddt
	if(debug == YES && fd > 0) tdmp(fd);
#endif
X:
	if(intrupt == YES)
		er = 1;
#ifdef	ddt
	if(debug == YES)
		display("connect ends eith er=%d, fd=%d\n", er, fd);
#endif
	return(er? -er:fd);
}

/***************************************************************
 *	find_dev: find a device pair with the wanted characteristics
 *	      specified in line, acu, and speed arguments.
 *	The global 'devtype' should have the value "ACU" or "DIR",
 *		depending on value of 'telno' arg on the command line.
 *	Return pointer to device name for use in lock-out synch.
 *	The globals 'cua' and 'cul' will be set to contain the
 *		/dev directory names of the corresponding devices.
 *	If the L-devices list contains a '0' entry because the
 *		line is direct, the global 'cua' is set to '\0'.
 ***************************************************************/

char *
find_dev(xacu, ln, spd)
char *xacu, *ln, *spd;
{
	char	buf[50], *b;	/* place to read L-devices into */
	int	acarg, lnarg;

#ifdef	ddt
	if(debug == YES)
		display("call find_dev(%s, %s, %s)\n", xacu, ln, spd);
#endif

	acarg = strlen(xacu);
	lnarg = strlen(ln);
	while(fgets(buf, 50, Ldevices) != NULL) {
		if(DIFFER(devtype, strtok(buf, " \t")))
			continue;
		(void) strcat(COPY(cul,"/dev/"),strtok((char*)NULL," \t"));
		if(*(b = strtok((char *) NULL, " \t")) == '0')
			cua[0] = '\0';
		else {
			if (*b != '/')
				(void) strcat(COPY(cua,"/dev/"),b);
			else
				(void) COPY(cua,b);
		}
		if(acarg) {
			if(DIFFER(devtype, "ACU"))
				break;
			if(DIFFER(xacu, cua) && DIFFER(xacu, &cua[5]))
				continue;
			(void) COPY(spd, strtok((char *)NULL, " \t\n"));
			++found;
			return(cul+5);
		}
		if(lnarg) {
			if(DIFFER(ln, cul) && DIFFER(ln, &cul[5]))
				continue;
			(void) COPY(spd, strtok((char *)NULL, " \t\n"));
			++found;
			return(cul+5);
		}
		if(EQUALS(devtype, "ACU")) {
			if(DIFFER(spd, strtok((char *) NULL, " \t\n")))
				continue;
			++found;
			return(cul+5);
		}
	}
	return("");
}
