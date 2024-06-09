#include <stdio.h>
#include "sys/types.h"
#include "termio.h"

struct
{
	char	*string;
	int	speed;
} speeds[] = {
	"0",	B0,
	"50",	B50,
	"75",	B75,
	"110",	B110,
	"134",	B134,
	"134.5",B134,
	"150",	B150,
	"200",	B200,
	"300",	B300,
	"600",	B600,
	"1200",	B1200,
	"1800",	B1800,
	"2400",	B2400,
	"4800",	B4800,
	"9600",	B9600,
	"19200",EXTA,
	"extb",	EXTB,
	0,
};
struct mds {
	char	*string;
	int	set;
	int	reset;
};

struct mds cmodes[] = {
	"-parity", CS8, PARENB|CSIZE,
	"-evenp", CS8, PARENB|CSIZE,
	"-oddp", CS8, PARENB|PARODD|CSIZE,
	"parity", PARENB|CS7, PARODD|CSIZE,
	"evenp", PARENB|CS7, PARODD|CSIZE,
	"oddp", PARENB|PARODD|CS7, CSIZE,
	"parenb", PARENB, 0,
	"-parenb", 0, PARENB,
	"parodd", PARODD, 0,
	"-parodd", 0, PARODD,
	"cs8", CS8, CSIZE,
	"cs7", CS7, CSIZE,
	"cs6", CS6, CSIZE,
	"cs5", CS5, CSIZE,
	"cstopb", CSTOPB, 0,
	"-cstopb", 0, CSTOPB,
	"hupcl", HUPCL, 0,
	"hup", HUPCL, 0,
	"-hupcl", 0, HUPCL,
	"-hup", 0, HUPCL,
	"clocal", CLOCAL, 0,
	"-clocal", 0, CLOCAL,
	"cread", CREAD, 0,
	"-cread", 0, CREAD,
	"raw", CS8, (CSIZE|PARENB),
	"-raw", (CS7|PARENB), CSIZE,
	"cooked", (CS7|PARENB), CSIZE,
#ifdef STOCKIII
	"sane", (CS7|PARENB|CREAD), (CSIZE|PARODD|CLOCAL),
#else
	"sane", (CS8|CREAD|HUPCL), (CSIZE|CSTOPB|PARENB|CLOCAL),
#endif
	0
};

struct mds imodes[] = {
	"ignbrk", IGNBRK, 0,
	"-ignbrk", 0, IGNBRK,
	"brkint", BRKINT, 0,
	"-brkint", 0, BRKINT,
	"ignpar", IGNPAR, 0,
	"-ignpar", 0, IGNPAR,
	"parmrk", PARMRK, 0,
	"-parmrk", 0, PARMRK,
	"inpck", INPCK, 0,
	"-inpck", 0,INPCK,
	"istrip", ISTRIP, 0,
	"-istrip", 0, ISTRIP,
	"inlcr", INLCR, 0,
	"-inlcr", 0, INLCR,
	"igncr", IGNCR, 0,
	"-igncr", 0, IGNCR,
	"icrnl", ICRNL, 0,
	"-icrnl", 0, ICRNL,
	"-nl", ICRNL, (INLCR|IGNCR),
	"nl", 0, ICRNL,
	"iuclc", IUCLC, 0,
	"-iuclc", 0, IUCLC,
	"lcase", IUCLC, 0,
	"-lcase", 0, IUCLC,
	"LCASE", IUCLC, 0,
	"-LCASE", 0, IUCLC,
	"ixon", IXON, 0,
	"-ixon", 0, IXON,
	"ixany", IXANY, 0,
	"-ixany", 0, IXANY,
	"ixoff", IXOFF, 0,
	"-ixoff", 0, IXOFF,
	"raw", 0, -1,
	"-raw", (BRKINT|IGNPAR|ISTRIP|ICRNL|IXON), 0,
	"cooked", (BRKINT|IGNPAR|ISTRIP|ICRNL|IXON), 0,
#ifdef STOCKIII
	"sane", (BRKINT|IGNPAR|ISTRIP|ICRNL|IXON),
		(IGNBRK|PARMRK|INPCK|INLCR|IGNCR|IUCLC|IXOFF),
#else
	"sane", (BRKINT|IGNPAR|ISTRIP|ICRNL|IXON),
		(IGNBRK|PARMRK|INPCK|INLCR|IGNCR|IUCLC|IXOFF|IXANY),
#endif
	0
};

struct mds lmodes[] = {
	"isig", ISIG, 0,
	"-isig", 0, ISIG,
	"icanon", ICANON, 0,
	"-icanon", 0, ICANON,
	"xcase", XCASE, 0,
	"-xcase", 0, XCASE,
	"lcase", XCASE, 0,
	"-lcase", 0, XCASE,
	"LCASE", XCASE, 0,
	"-lcase", 0, XCASE,
	"echo", ECHO, 0,
	"-echo", 0, ECHO,
	"echoe", ECHOE, 0,
	"-echoe", 0, ECHOE,
	"echok", ECHOK, 0,
	"-echok", 0, ECHOK,
	"lfkc", ECHOK, 0,
	"-lfkc", 0, ECHOK,
	"echonl", ECHONL, 0,
	"-echonl", 0, ECHONL,
	"noflsh", NOFLSH, 0,
	"-noflsh", 0, NOFLSH,
	"raw", 0, (ISIG|ICANON|XCASE),
	"-raw", (ISIG|ICANON), 0,
	"cooked", (ISIG|ICANON), 0,
#ifdef STOCKIII
	"sane", (ISIG|ICANON|ECHO|ECHOK), (XCASE|ECHOE|ECHONL|NOFLSH),
#else
	"sane", (ISIG|ICANON|ECHO|ECHOK), (XCASE|ECHOE|ECHONL|NOFLSH),
#endif
	0,
};

struct mds omodes[] = {
	"opost", OPOST, 0,
	"-opost", 0, OPOST,
	"olcuc", OLCUC, 0,
	"-olcuc", 0, OLCUC,
	"lcase", OLCUC, 0,
	"-lcase", 0, OLCUC,
	"LCASE", OLCUC, 0,
	"-LCASE", 0, OLCUC,
	"onlcr", ONLCR, 0,
	"-onlcr", 0, ONLCR,
	"-nl", ONLCR, (OCRNL|ONLRET),
	"nl", 0, ONLCR,
	"ocrnl", OCRNL, 0,
	"-ocrnl",0, OCRNL,
	"onocr", ONOCR, 0,
	"-onocr", 0, ONOCR,
	"onlret", ONLRET, 0,
	"-onlret", 0, ONLRET,
	"fill", OFILL, OFDEL,
	"-fill", 0, OFILL|OFDEL,
	"nul-fill", OFILL, OFDEL,
	"del-fill", OFILL|OFDEL, 0,
	"ofill", OFILL, 0,
	"-ofill", 0, OFILL,
	"ofdel", OFDEL, 0,
	"-ofdel", 0, OFDEL,
	"cr0", CR0, CRDLY,
	"cr1", CR1, CRDLY,
	"cr2", CR2, CRDLY,
	"cr3", CR3, CRDLY,
	"tab0", TAB0, TABDLY,
	"tabs", TAB0, TABDLY,
	"tab1", TAB1, TABDLY,
	"tab2", TAB2, TABDLY,
	"tab3", TAB3, TABDLY,
	"-tabs", TAB3, TABDLY,
	"nl0", NL0, NLDLY,
	"nl1", NL1, NLDLY,
	"ff0", FF0, FFDLY,
	"ff1", FF1, FFDLY,
	"vt0", VT0, VTDLY,
	"vt1", VT1, VTDLY,
	"bs0", BS0, BSDLY,
	"bs1", BS1, BSDLY,
	"raw", 0, OPOST,
	"-raw", OPOST, 0,
	"cooked", OPOST, 0,
	"tty33", CR1, (CRDLY|TABDLY|NLDLY|FFDLY|VTDLY|BSDLY),
	"tn300", CR1, (CRDLY|TABDLY|NLDLY|FFDLY|VTDLY|BSDLY),
	"ti700", CR2, (CRDLY|TABDLY|NLDLY|FFDLY|VTDLY|BSDLY),
	"vt05", NL1, (CRDLY|TABDLY|NLDLY|FFDLY|VTDLY|BSDLY),
	"tek", FF1, (CRDLY|TABDLY|NLDLY|FFDLY|VTDLY|BSDLY),
	"tty37", (FF1|VT1|CR2|TAB1|NL1), (NLDLY|CRDLY|TABDLY|BSDLY|VTDLY|FFDLY),
#ifdef STOCKIII
	"sane", (OPOST|ONLCR), (OLCUC|OCRNL|ONOCR|ONLRET|OFILL|OFDEL|
			NLDLY|CRDLY|TABDLY|BSDLY|VTDLY|FFDLY),
#else
	"sane", (OPOST|ONLCR|TAB3), (OLCUC|OCRNL|ONOCR|ONLRET|OFILL|OFDEL|
			NLDLY|CRDLY|TABDLY|BSDLY|VTDLY|FFDLY),
#endif
	0,
};

char	*arg;
int	match;
char	*STTY="stty: ";
char	*USAGE="usage: stty [-ag] [modes]";
int	pitt = 0;
struct termio cb;

main(argc, argv)
char	*argv[];
{
	register i;

	if(ioctl(0, TCGETA, &cb) == -1) {
		perror(STTY);
		exit(2);
	}
	if (argc == 1) {
		prmodes();
		exit(0);
	}
	if ((argc == 2) && (argv[1][0] == '-') && (argv[1][2] == '\0'))
	switch(argv[1][1]) {
		case 'a':
			pramodes();
			exit(0);
		case 'g':
			prencode();
			exit(0);
		default:
			fprintf(stderr, "%s\n", USAGE);
			exit(2);
	}
	while(--argc > 0) {

		arg = *++argv;
		match = 0;
		if (eq("erase") && --argc)
			cb.c_cc[VERASE] = gct(*++argv);
		else if (eq("intr") && --argc)
			cb.c_cc[VINTR] = gct(*++argv);
		else if (eq("quit") && --argc)
			cb.c_cc[VQUIT] = gct(*++argv);
		else if (eq("eof") && --argc)
			cb.c_cc[VEOF] = gct(*++argv);
		else if (eq("min") && --argc)
			cb.c_cc[VMIN] = gct(*++argv);
		else if (eq("eol") && --argc)
			cb.c_cc[VEOL] = gct(*++argv);
		else if (eq("time") && --argc)
			cb.c_cc[VTIME] = gct(*++argv);
		else if (eq("kill") && --argc)
			cb.c_cc[VKILL] = gct(*++argv);
		else if (eq("ek")) {
			cb.c_cc[VERASE] = CERASE;
			cb.c_cc[VKILL] = CKILL;
		}
		else if (eq("line") && --argc)
			cb.c_line = atoi(*++argv);
		else if (eq("raw")) {
			cb.c_cc[VMIN] = 1;
			cb.c_cc[VTIME] = 1;
		}
		else if (eq("-raw") | eq("cooked")) {
			cb.c_cc[VEOF] = CEOF;
			cb.c_cc[VEOL] = CNUL;
		}
		else if(eq("sane")) {
			cb.c_cc[VERASE] = CERASE;
			cb.c_cc[VKILL] = CKILL;
			cb.c_cc[VQUIT] = CQUIT;
			cb.c_cc[VINTR] = CINTR;
			cb.c_cc[VEOF] = CEOF;
			cb.c_cc[VEOL] = CNUL;
		}
		for(i=0; speeds[i].string; i++)
			if(eq(speeds[i].string)) {
				cb.c_cflag &= ~CBAUD;
				cb.c_cflag |= speeds[i].speed&CBAUD;
			}
		for(i=0; imodes[i].string; i++)
			if(eq(imodes[i].string)) {
				cb.c_iflag &= ~imodes[i].reset;
				cb.c_iflag |= imodes[i].set;
			}
		for(i=0; omodes[i].string; i++)
			if(eq(omodes[i].string)) {
				cb.c_oflag &= ~omodes[i].reset;
				cb.c_oflag |= omodes[i].set;
			}
		for(i=0; cmodes[i].string; i++)
			if(eq(cmodes[i].string)) {
				cb.c_cflag &= ~cmodes[i].reset;
				cb.c_cflag |= cmodes[i].set;
			}
		for(i=0; lmodes[i].string; i++)
			if(eq(lmodes[i].string)) {
				cb.c_lflag &= ~lmodes[i].reset;
				cb.c_lflag |= lmodes[i].set;
			}
		if(!match)
			if(!encode(arg)) {
				fprintf(stderr, "unknown mode: %s\n", arg);
				exit(2);
			}
	}
	if(ioctl(0, TCSETAW, &cb) == -1) {
		perror(STTY);
		exit(2);
	}
}

eq(string)
char *string;
{
	register i;

	if(!arg)
		return(0);
	i = 0;
loop:
	if(arg[i] != string[i])
		return(0);
	if(arg[i++] != '\0')
		goto loop;
	match++;
	return(1);
}

prmodes()
{
	register m;

	m = cb.c_cflag;
	prspeed("speed ", m&CBAUD);
	if (m&PARENB)
		if (m&PARODD)
			printf("oddp ");
		else
			printf("evenp ");
	else
		printf("-parity ");
	if(((m&PARENB) && !(m&CS7)) || (!(m&PARENB) && !(m&CS8)))
		printf("cs%c ",'5'+(m&CSIZE)/CS6);
	if (m&CSTOPB)
		printf("cstopb ");
	if (m&HUPCL)
		printf("hupcl ");
	if (!(m&CREAD))
		printf("cread ");
	if (m&CLOCAL)
		printf("clocal ");
	printf("\n");
	if(cb.c_line != 0)
		printf("line = %d; ", cb.c_line);
	if(cb.c_cc[VINTR] != CINTR)
		pit(cb.c_cc[VINTR], "intr", "; ");
	if(cb.c_cc[VQUIT] != CQUIT)
		pit(cb.c_cc[VQUIT], "quit", "; ");
	if(cb.c_cc[VERASE] != CERASE)
		pit(cb.c_cc[VERASE], "erase", "; ");
	if(cb.c_cc[VKILL] != CKILL)
		pit(cb.c_cc[VKILL], "kill", "; ");
	if(cb.c_cc[VEOF] != CEOF)
		pit(cb.c_cc[VEOF], "eof", "; ");
	if(cb.c_cc[VEOL] != CNUL)
		pit(cb.c_cc[VEOL], "eol", "; ");
	if(pitt) printf("\n");
	m = cb.c_iflag;
	if (m&IGNBRK)
		printf("ignbrk ");
	else if (m&BRKINT)
		printf("brkint ");
	if (!(m&INPCK))
		printf("-inpck ");
	else if (m&IGNPAR)
		printf("ignpar ");
	if (m&PARMRK)
		printf("parmrk ");
	if (!(m&ISTRIP))
		printf("-istrip ");
	if (m&INLCR)
		printf("inlcr ");
	if (m&IGNCR)
		printf("igncr ");
	if (m&ICRNL)
		printf("icrnl ");
	if (m&IUCLC)
		printf("iuclc ");
	if (!(m&IXON))
		printf("-ixon ");
	else if (!(m&IXANY))
		printf("-ixany ");
	if (m&IXOFF)
		printf("ixoff ");
	m = cb.c_oflag;
	if (!(m&OPOST))
		printf("-opost ");
	else {
	if (m&OLCUC)
		printf("olcuc ");
	if (m&ONLCR)
		printf("onlcr ");
	if (m&OCRNL)
		printf("ocrnl ");
	if (m&ONOCR)
		printf("onocr ");
	if (m&ONLRET)
		printf("onlret ");
	if (m&OFILL)
		if (m&OFDEL)
			printf("del-fill ");
		else
			printf("nul-fill ");
	delay((m&CRDLY)/CR1, "cr");
	delay((m&NLDLY)/NL1, "nl");
	delay((m&TABDLY)/TAB1, "tab");
	delay((m&BSDLY)/BS1, "bs");
	delay((m&VTDLY)/VT1, "vt");
	delay((m&FFDLY)/FF1, "ff");
	}
	printf("\n");
	m = cb.c_lflag;
	if (!(m&ISIG))
		printf("-isig ");
	if (!(m&ICANON))
		printf("-icanon ");
	if (m&XCASE)
		printf("xcase ");
	printf("-echo "+((m&ECHO)!=0));
	printf("-echoe "+((m&ECHOE)!=0));
	printf("-echok "+((m&ECHOK)!=0));
	if (m&ECHONL)
		printf("echonl ");
	if (m&NOFLSH)
		printf("noflsh ");
	printf("\n");
}

pramodes()
{
	register m;

	prspeed("speed ", cb.c_cflag&CBAUD);
	printf("line = %d; ", cb.c_line);
	pit(cb.c_cc[VINTR], "intr", "; ");
	pit(cb.c_cc[VQUIT], "quit", "; ");
	pit(cb.c_cc[VERASE], "erase", "; ");
	pit(cb.c_cc[VKILL], "kill", "; ");
	pit(cb.c_cc[VEOF], "eof", "; ");
	pit(cb.c_cc[VEOL], "eol", "\n");
	m = cb.c_cflag;
	printf("-parenb "+((m&PARENB)!=0));
	printf("-parodd "+((m&PARODD)!=0));
	printf("cs%c ",'5'+(m&CSIZE)/CS6);
	printf("-cstopb "+((m&CSTOPB)!=0));
	printf("-hupcl "+((m&HUPCL)!=0));
	printf("-cread "+((m&CREAD)!=0));
	printf("-clocal "+((m&CLOCAL)!=0));
	printf("\n");
	m = cb.c_iflag;
	printf("-ignbrk "+((m&IGNBRK)!=0));
	printf("-brkint "+((m&BRKINT)!=0));
	printf("-ignpar "+((m&IGNPAR)!=0));
	printf("-parmrk "+((m&PARMRK)!=0));
	printf("-inpck "+((m&INPCK)!=0));
	printf("-istrip "+((m&ISTRIP)!=0));
	printf("-inlcr "+((m&INLCR)!=0));
	printf("-igncr "+((m&IGNCR)!=0));
	printf("-icrnl "+((m&ICRNL)!=0));
	printf("-iuclc "+((m&IUCLC)!=0));
	printf("\n");
	printf("-ixon "+((m&IXON)!=0));
	printf("-ixany "+((m&IXANY)!=0));
	printf("-ixoff "+((m&IXOFF)!=0));
	printf("\n");
	m = cb.c_lflag;
	printf("-isig "+((m&ISIG)!=0));
	printf("-icanon "+((m&ICANON)!=0));
	printf("-xcase "+((m&XCASE)!=0));
	printf("-echo "+((m&ECHO)!=0));
	printf("-echoe "+((m&ECHOE)!=0));
	printf("-echok "+((m&ECHOK)!=0));
	printf("-echonl "+((m&ECHONL)!=0));
	printf("-noflsh "+((m&NOFLSH)!=0));
	printf("\n");
	m = cb.c_oflag;
	printf("-opost "+((m&OPOST)!=0));
	printf("-olcuc "+((m&OLCUC)!=0));
	printf("-onlcr "+((m&ONLCR)!=0));
	printf("-ocrnl "+((m&OCRNL)!=0));
	printf("-onocr "+((m&ONOCR)!=0));
	printf("-onlret "+((m&ONLRET)!=0));
	printf("-ofill "+((m&OFILL)!=0));
	printf("-ofdel "+((m&OFDEL)!=0));
	delay((m&CRDLY)/CR1, "cr");
	delay((m&NLDLY)/NL1, "nl");
	delay((m&TABDLY)/TAB1, "tab");
	delay((m&BSDLY)/BS1, "bs");
	delay((m&VTDLY)/VT1, "vt");
	delay((m&FFDLY)/FF1, "ff");
	printf("\n");
}

gct(cp)
register char *cp;
{
	register c;

	c = *cp++;
	if (c == '^') {
		c = *cp;
		if (c == '?')
			c = 0177;
		else if (c == '-')
			c = 0377;
		else
			c &= 037;
	}
	return(c);
}

pit(what, itsname, sep)
	unsigned char what;
	char *itsname, *sep;
{

	pitt++;
	printf("%s", itsname);
	if (what == 0377) {
		printf(" <undef>%s", sep);
		return;
	}
	printf(" = ");
	if (what & 0200) {
		printf("-");
		what &= ~ 0200;
	}
	if (what == 0177) {
		printf("DEL%s", sep);
		return;
	} else if (what < ' ') {
		printf("^");
		what += '`';
	}
	printf("%c%s", what, sep);
}

delay(m, s)
char *s;
{
	if(m)
		printf("%s%d ", s, m);
}

int	speed[] = {
	0,50,75,110,134,150,200,300,600,1200,1800,2400,4800,9600,19200,0
};

prspeed(c, s)
char *c;
{

	printf("%s", c);
	if (s == EXTB) {
		printf("extb\n");
	} else {
		printf("%d baud\n", speed[s]);
	}
}

prencode()
{
	printf("%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x\n",
	cb.c_iflag,cb.c_oflag,cb.c_cflag,cb.c_lflag,cb.c_cc[0],
	cb.c_cc[1],cb.c_cc[2],cb.c_cc[3],cb.c_cc[4],cb.c_cc[5],
	cb.c_cc[6],cb.c_cc[7]);
}

encode(arg)
char *arg;
{
	int grab[12], i;
	i = sscanf(arg, "%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x",
	&grab[0],&grab[1],&grab[2],&grab[3],&grab[4],&grab[5],&grab[6],
	&grab[7],&grab[8],&grab[9],&grab[10],&grab[11]);

	if(i != 12) return(0);

	cb.c_iflag = (ushort) grab[0];
	cb.c_oflag = (ushort) grab[1];
	cb.c_cflag = (ushort) grab[2];
	cb.c_lflag = (ushort) grab[3];

	for(i=0; i<8; i++)
		cb.c_cc[i] = (char) grab[i+4];
	return(1);
}
