/*
 * getty -- adapt to terminal speed on dialup, and call login
 *	
 *	invoked as "/etc/getty name type delay" where
 *		name :  character  string (length <= 14) e.g. tty3
 *		type :  single character
 *		delay : number of seconds to complete getty before
 *			sudden death and hang-up. A non-zero value
 *			implies a dial-up line
 */

#include <termio.h>
#include <signal.h>
#include <fcntl.h>
#include <utmp.h>

#define XONLY '!'
#define TTYPE *argv[2-argp]
#define TDELAY argv[3-argp]
#define NARGS argc+argp

#define UTMP "/etc/utmp"
#define WTMP "/usr/adm/wtmp"

struct termio tmode;

struct	tab {
	char	tname;		/* this table name */
	char	nname;		/* successor table name */
	unsigned short	iiflag;		/* initial input mode */
	unsigned short	ioflag;		/* initial output mode */
	unsigned short	icflag;		/* initial control mode */
	unsigned short	ilflag;		/* initial local mode */
	unsigned short	fiflag;		/* final input mode */
	unsigned short	foflag;		/* final output mode */
	unsigned short	fcflag;		/* final control mode */
	unsigned short	flflag;		/* final local mode */
	char	line;			/* line discipline */
	char	*message;	/* login message */
} itab[] = {

/* table '0'-1-2-'3' 300,150,110,1200 */

	'0', 1,
	0, ONLRET|CR1|OPOST,
	B300|CREAD|CS8, ECHOK,
	BRKINT|IGNPAR|ISTRIP|IXON|IXANY, ONLRET|CR1|OPOST,
#ifdef STOCKIII
	B300|CREAD|CS7|PARENB, ECHO|ECHOK|ICANON|ISIG,
#else
	B300|CREAD|CS8, ECHO|ECHOK|ICANON|ISIG,
#endif
	0,
	"\n\r\033;\007login: ",

	1, 2,
	0, ONLRET|CR1|OPOST,
	B150|CREAD|CS8, ECHOK,
	BRKINT|IGNPAR|ISTRIP|IXON|IXANY|INPCK, ONLRET|CR1|OPOST|TAB1|FFDLY,
	B150|CREAD|CS7|PARENB, ECHO|ECHOK|ICANON|ISIG,
	0,
	"\n\r\033:\006\006\017login: ",

	2, '3',
	0, ONLRET|CR1|OPOST,
	B110|CREAD|CS8|CSTOPB, ECHOK,
	BRKINT|IGNPAR|ISTRIP|IXON|IXANY|ICRNL|IUCLC, ONLCR|CR1|OPOST|OLCUC|TAB3,
	B110|CREAD|CS7|PARENB|CSTOPB, ECHO|ECHOK|ICANON|ISIG|XCASE,
	0,
	"\n\rlogin: ",

/* table '-' -- Console TTY 110 */
	'-', '-',
	0, ONLRET|CR1|OPOST,
	B110|CREAD|CS8|CSTOPB, ECHOK,
	BRKINT|IGNPAR|ISTRIP|IXON|IXANY|ICRNL|IUCLC, ONLCR|CR1|OPOST|OLCUC|TAB3,
	B110|CREAD|CS7|PARENB|CSTOPB, ECHO|ECHOK|ICANON|ISIG|XCASE,
	0,
	"\n\rlogin: ",

/* table '1' -- 150 */
	'1', '1',
	0, ONLRET|CR1|OPOST,
	B150|CREAD|CS8, ECHOK,
	BRKINT|IGNPAR|ISTRIP|IXON|IXANY|INPCK, ONLRET|CR1|OPOST|TAB1|FFDLY,
	B150|CREAD|CS7|PARENB, ECHO|ECHOK|ICANON|ISIG,
	0,
	"\n\r\033:\006\006\017login: ",

/* table '2' -- 2400 */
	'2', '2',
	0, ONLRET|CR1|OPOST,
	B2400|CREAD|CS8, ECHOK,
	BRKINT|IGNPAR|ISTRIP|IXON|IXANY|ICRNL, ONLCR|OPOST|TAB3,
	B2400|CREAD|CS7|PARENB, ECHO|ECHOK|ICANON|ISIG,
	0,
	"\n\rlogin: ",

/* table '3'-'0' -- 1200,300 */
	'3', '0',
	0, ONLRET|CR1|OPOST,
	B1200|CREAD|CS8, ECHOK,
	BRKINT|IGNPAR|ISTRIP|IXON|IXANY|ICRNL, ONLCR|OPOST|TAB3,
#ifdef STOCKIII
	B1200|CREAD|CS7|PARENB, ECHO|ECHOK|ICANON|ISIG,
#else
	B1200|CREAD|CS8, ECHO|ECHOK|ICANON|ISIG,
#endif
	0,
	"\n\rlogin: ",

/* table '4' -- Console Decwriter */
	'4', '4',
	BRKINT|IGNPAR|ISTRIP|IXON|IXANY|ICRNL, ONLCR|OPOST|TAB3,
	B300|CREAD|CS7|PARENB, ECHO|ECHOK|ICANON|ISIG,
	BRKINT|IGNPAR|ISTRIP|IXON|IXANY|ICRNL, ONLCR|OPOST|TAB3,
	B300|CREAD|CS7|PARENB, ECHO|ECHOK|ICANON|ISIG,
	0,
	"\n\rlogin: ",

/* table '5' -- 9600 */
	'5', '5',
	0, ONLRET|CR1|OPOST,
	B9600|CREAD|CS8, ECHOK,
	BRKINT|IGNPAR|ISTRIP|IXON|IXANY|ICRNL, ONLCR|OPOST|TAB3,
	B9600|CREAD|CS7|PARENB, ECHO|ECHOK|ICANON|ISIG,
	0,
	"\n\rlogin: ",

/* table '6' -- 4800/9600 -- tektronix 4014 */
	'6', 7,
	0, ONLRET|FFDLY|OPOST,
	B4800|CREAD|CS8, ECHOK,
	BRKINT|IGNPAR|ISTRIP|IXON|IXANY|ICRNL, ONLCR|OPOST|FFDLY,
	B4800|CREAD|CS7|PARENB, ECHO|ECHOK|ICANON|ISIG,
	0,
	"\033\014login: ",

	7, '6',
	0, ONLRET|FFDLY|OPOST,
	B9600|CREAD|CS8, ECHOK,
	BRKINT|IGNPAR|ISTRIP|IXON|IXANY|ICRNL, ONLCR|OPOST|FFDLY,
	B9600|CREAD|CS7|PARENB, ECHO|ECHOK|ICANON|ISIG,
	0,
	"\033\014login: ",

/* table '7' -- 4800 */
	'7', '7',
	0, ONLRET|CR1|OPOST,
	B4800|CREAD|CS8, ECHOK,
	BRKINT|IGNPAR|ISTRIP|IXON|IXANY|ICRNL, ONLCR|OPOST|TAB3,
	B4800|CREAD|CS7|PARENB, ECHO|ECHOK|ICANON|ISIG,
	0,
	"\n\rlogin: ",
#ifndef STOCKIII
/* table 'a' -- 19200 */
	'a', 'a',
	0, ONLCR|CR1|OPOST,
	EXTA|CREAD|CS8, ECHOK,
	BRKINT|IGNPAR|ISTRIP|IXON|ICRNL, ONLCR|OPOST|TAB3,
	EXTA|CREAD|CS8|HUPCL, ECHO|ECHOK|ICANON|ISIG,
	0,
	"\nlogin: ",
/* table 'b' -- matches switch setting */
	'b', 'b',
	0, ONLCR|CR1|OPOST,
	EXTB|CREAD|CS8, ECHOK,
	BRKINT|IGNPAR|ISTRIP|IXON|ICRNL, ONLCR|OPOST|TAB3,
	EXTB|CREAD|CS8|HUPCL, ECHO|ECHOK|ICANON|ISIG,
	0,
	"\nlogin: ",
#endif

};
#ifndef STOCKIII
extern	struct tab uitab[];
extern	int n_uitab;
#endif

#define	NITAB	sizeof itab/sizeof itab[0]

char	name[16];
int	crmod;
int	upper;
int	lower;
int	bailout();
int	hupclr();
int	stin;	/* standard input file */
int	stout;	/* standard output file */
int	sterr;	/* diagnostic output file */
char	*ttyname();
char	*strrchr();
char	rline[20]="/dev/";
char	*line;

char partab[] = {
	0001,0201,0201,0001,0201,0001,0001,0201,
	0202,0004,0003,0205,0005,0206,0201,0001,
	0201,0001,0001,0201,0001,0201,0201,0001,
	0001,0201,0201,0001,0201,0001,0001,0201,
	0200,0000,0000,0200,0000,0200,0200,0000,
	0000,0200,0200,0000,0200,0000,0000,0200,
	0000,0200,0200,0000,0200,0000,0000,0200,
	0200,0000,0000,0200,0000,0200,0200,0000,
	0200,0000,0000,0200,0000,0200,0200,0000,
	0000,0200,0200,0000,0200,0000,0000,0200,
	0000,0200,0200,0000,0200,0000,0000,0200,
	0200,0000,0000,0200,0000,0200,0200,0000,
	0000,0200,0200,0000,0200,0000,0000,0200,
	0200,0000,0000,0200,0000,0200,0200,0000,
	0200,0000,0000,0200,0000,0200,0200,0000,
	0000,0200,0200,0000,0200,0000,0000,0201
};

struct tab *tabp;

main(argc, argv)
char **argv;
{
	int argp=0;
	char tname;

	signal (SIGALRM, bailout);
	signal (SIGHUP, hupclr);
	stout = -1;

	if(isatty(0))
	{
		line=ttyname(0);
		argp++;
	}
	else
	{
		close(0);
		if(argc==1)
			err(300);
		line=rline;
		strcat (line, argv[1]);
	}

/* updates to wtmp etc. */
	rmut();

/* If the term type arg is special, exit only,
   this is so you can update utmp without spawning 
   an open line
*/
	if(TTYPE == XONLY) hangup(line, (NARGS>3 ? atoi (TDELAY) : 2));

/* check for existence of line */

	if(access(line,0) != 0)	err(300);
	chown(line,0);
	chmod(line,0622);


	switch(stin=open(line,O_RDWR)){
		case -1:
			err(300);
		case 0:	/*no file descriptors before this*/
			break;
		default:
			close(stin);
	}

	close(1);
	close(2);
	stout=dup(0);
	sterr=dup(0);

	/* get terminal class from second argument */
	tname = (NARGS > 2 ? TTYPE : '0');

	/* set alarm from third argument; zero implies no hangup;
		non-zero: seconds to complete before hangup (typically 60) */
	alarm (NARGS>3 ? atoi (TDELAY) : 0);

	for (;;) {
#ifdef STOCKIII
		for(tabp = itab; tabp < &itab[NITAB]; tabp++)
			if(tabp->tname == tname)
				break;
		if(tabp >= &itab[NITAB])
			tabp = itab;
#else
		for(tabp = itab; tabp != &uitab[n_uitab]; tabp++)
			if(tabp == &itab[NITAB]) {
				tabp = &uitab[0];
				tabp--;
			}
			else if(tabp->tname == tname)
				break;
		if(tabp == &uitab[n_uitab]) tabp = itab;
#endif			
		tmode.c_iflag=tabp->iiflag;
		tmode.c_oflag=tabp->ioflag;
		tmode.c_cflag=tabp->icflag;
		tmode.c_lflag=tabp->ilflag;
		tmode.c_line = tabp->line;
		tmode.c_cc[VINTR] = CINTR;
		tmode.c_cc[VQUIT] = CQUIT;
		tmode.c_cc[VERASE] = CERASE;
		tmode.c_cc[VKILL] = CKILL;
		tmode.c_cc[VMIN] = 1;
		tmode.c_cc[VTIME] = 1;
		ioctl(0, TCSETAF, &tmode);
		puts(tabp->message);
		if(getname()) {
			tmode.c_iflag=tabp->fiflag;
			tmode.c_oflag=tabp->foflag;
			tmode.c_cflag=tabp->fcflag;
			tmode.c_lflag=tabp->flflag;
			tmode.c_cc[VINTR] = CINTR;
			tmode.c_cc[VQUIT] = CQUIT;
			tmode.c_cc[VERASE] = CERASE;
			tmode.c_cc[VKILL] = CKILL;
			tmode.c_cc[VEOF] = CEOF;
			tmode.c_cc[VEOL] = CNUL;
			if(crmod) {
				tmode.c_iflag |= ICRNL;
				tmode.c_oflag |= ONLCR;
				tmode.c_oflag &= ~ONLRET;
				if (tmode.c_oflag&CR2)
					tmode.c_oflag |= ONOCR;
			}
			if(upper) {
				tmode.c_iflag |= IUCLC;
				tmode.c_oflag |= OLCUC;
				tmode.c_lflag |= XCASE;
			}
			if(lower) {
				tmode.c_iflag &= ~IUCLC;
				tmode.c_oflag &= ~OLCUC;
				tmode.c_lflag &= ~XCASE;
			}
			ioctl(0, TCSETAF, &tmode);
			if ((tabp->ilflag&ECHO) == 0)
				putchr('\n');
			execl("/etc/login","login",name,(NARGS>3?TDELAY:"60"),0);
			err(300);
		}
		tname = tabp->nname;
	}
}

getname()
{
	register char *np;
	register c;
	char cs;

	crmod = 0;
	upper = 0;
	lower = 0;
	np = name;
	for (;;) {
		if (read(0, &cs, 1) <= 0)
			err(10);
		if ((c = cs&0177) == 0)
			return(0);
		if (c==CEOF)
			err(6);
		if (c=='\r' || c=='\n' || np >= &name[16])
			break;
		if ((tabp->ilflag&ECHO) == 0)
			putchr(c);
		if (c>='a' && c <='z')
			lower++;
		else if (c>='A' && c<='Z') {
			upper++;
			c += 'a'-'A';
		} else if (c==CERASE) {
			if (np > name)
				np--;
			continue;
		} else if (c==CKILL) {
			putchr('\r');
			putchr('\n');
			np = name;
			continue;
		} else if(c == ' ')
			c = '_';
		*np++ = c;
	}
	*np = 0;
	if (c == '\r')
		crmod++;
	return(1);
}

puts(as)
char *as;
{
	register char *s;
	if (stout == -1)  /* error case only */
		err(300);

	s = as;
	while (*s)
		putchr(*s++);
}

putchr(cc)
{
	char c;
	c = cc;
	c |= partab[c&0177] & 0200;
	write(stout, &c, 1);
}
hangup(line, delay)
char *line;
int delay;
{
	if(delay == 0) exit(0);
	signal(SIGHUP, SIG_DFL);
	alarm(delay);
	open(line, O_RDWR);
	bailout();
}
bailout ()
{
		tmode.c_cflag = B0|HUPCL;
		ioctl(0, TCSETA, &tmode);
		exit(1);
}
utscan(utf,lnam)
register utf;
char *lnam;
{
	register i;
	struct utmp wtmp;

	lseek(utf, (long)0, 0);
	while(read(utf, (char *)&wtmp, sizeof(wtmp)) == sizeof(wtmp))
	{
		for(i=0; i<8; i++)
			if(wtmp.ut_line[i] != lnam[i])
				goto contin;
		lseek(utf, -(long)sizeof(wtmp), 1);
		return(1);
		contin:;
	}
	return(0);
}

rmut()
{
	int utf, i;
	struct utmp wtmp;
	char *lnam;

	lnam = strrchr(line,'/') + 1;

	for(i=0; i<8; i++)
	{
		wtmp.ut_name[i] = '\0';
		wtmp.ut_line[i] = lnam[i];
	}
	time(&wtmp.ut_time);

	utf = open(UTMP, O_RDWR|O_CREAT, 0644);
	if(utscan(utf,lnam) != 0)
		write(utf, &wtmp, sizeof(wtmp));
	else
	{
		fcntl(utf, F_SETFL, fcntl(utf, F_GETFL, 0) | O_APPEND);
		write(utf, &wtmp, sizeof(wtmp));
	}

	close(utf);
	utf = open(WTMP, O_WRONLY|O_CREAT|O_APPEND);
	write(utf, &wtmp, sizeof(wtmp));
	close(utf);
}
err(secs)
int secs;
{
	sleep(secs);
	exit(9);
}
hupclr(){
	err(0);
}
