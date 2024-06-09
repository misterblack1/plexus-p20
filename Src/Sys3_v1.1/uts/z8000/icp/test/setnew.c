#include <termio.h>
#include <sys/ioctl.h>
#define	NULL	-1

/* input modes */
struct index {
	char *cmd;
	int swval;
} inputmode[] = {
    "IGNBRK",	0000001,
    "BRKINT",	0000002,
    "IGNPAR",	0000004,
    "PARMRK",	0000010,
    "INPCK",	0000020,
    "ISTRIP",	0000040,
    "INLCR",	0000100,
    "IGNCR",	0000200,
    "ICRNL",	0000400,
    "IUCLC",	0001000,
    "IXON",	0002000,
    "IXANY",	0004000,
    "IXOFF",	0010000,
    0,		NULL
};

/* output modes */
struct index outputmode[] = {
    "OPOST",	0000001,
    "OLCUC",	0000002,
    "ONLCR",	0000004,
    "OCRNL",	0000010,
    "ONOCR",	0000020,
    "ONLRET",	0000040,
    "OFILL",	0000100,
    "OFDEL",	0000200,
    "NL0",	0,
    "NL1",	0000400,
    "CR0",	0,
    "CR1",	0001000,
    "CR2",	0002000,
    "CR3",	0003000,
    "TAB0",	0,
    "TAB1",	0004000,
    "TAB2",	0010000,
    "TAB3",	0014000,
    "BS0",	0,
    "BS1",	0020000,
    "VT0",	0,
    "VT1",	0040000,
    "FF0",	0,
    "FF1",	0100000,
    0,		NULL
};

/* control modes */
struct index ctrlmode[] = {
    "B0",	0,
    "B50",	0000001,
    "B75",	0000002,
    "B110",	0000003,
    "B134",	0000004,
    "B150",	0000005,
    "B200",	0000006,
    "B300",	0000007,
    "B600",	0000010,
    "B1200",	0000011,
    "B1800",	0000012,
    "B2400",	0000013,
    "B4800",	0000014,
    "B9600",	0000015,
    "EXTA",	0000016,
    "EXTB",	0000017,
    "CS5",	0,
    "CS6",	0000020,
    "CS7",	0000040,
    "CS8",	0000060,
    "CSTOPB",	0000100,
    "CREAD",	0000200,
    "PARENB",	0000400,
    "PARODD",	0001000,
    "HUPCL",	0002000,
    "CLOCAL",	0004000,
    0,		NULL
};

/* line discipline 0 modes */
struct index discipline[] = {
    "ISIG",	0000001,
    "ICANON",	0000002,
    "XCASE",	0000004,
    "ECHO",	0000010,
    "ECHOE",	0000020,
    "ECHOK",	0000040,
    "ECHONL",	0000100,
    "NOFLSH",	0000200,
    0,		NULL
};

/* control characters */
struct index control[] = {
    "INTR",	0,
    "QUIT",	1,
    "ERASE",	2,
    "KILL",	3,
    "EOF",	4,
    "EOL",	5,
    0,		NULL
};


struct termio ttystate;

setnew(line)
int line;
{

    char arg[5][20];
    char buf [80];
    char *ptr,*gets();
    int n,i,cc,c,flag,iflag,oflag,cflag,lflag;

    ioctl(line,TCGETA,&ttystate);

    iflag = ttystate.c_iflag;
    oflag = ttystate.c_oflag;
    cflag = ttystate.c_cflag;
    lflag = ttystate.c_lflag;

    while(1) {
	ptr = gets(buf);
	n = sscanf(ptr,"%s%s%s%s%s%s\n",arg[0],arg[1],arg[2],
					arg[3],arg[4],arg[5]);
	if(n == NULL) break;

	for(i=0;i<n;i++) {
	    ptr = arg[i];
	    if(*ptr == '-') {
		flag = 0;
		ptr++;
	    }
	    else
		flag = 1;

	    if((c=type(ptr,inputmode)) != NULL) {
		if(flag)
		    iflag |= c;
		else
		    iflag &= ~c;
	    }
	    else if((c=type(ptr,outputmode)) != NULL) {
		if(!strcmp(ptr,"NL0") || !strcmp(ptr,"NL1")) {
		    oflag &= ~0400;
		    oflag |= c;
		    continue;
		}
		if(!strcmp(ptr,"CR0") || !strcmp(ptr,"CR1") ||
		   !strcmp(ptr,"CR2") || !strcmp(ptr,"CR3")) {
		    oflag &= ~03000;
		    oflag |= c;
		    continue;
	        }
	        if(!strcmp(ptr,"TAB0") || !strcmp(ptr,"TAB1") ||
	           !strcmp(ptr,"TAB2") || !strcmp(ptr,"TAB3")) {
		    oflag &= ~014000;
		    oflag |= c;
		    continue;
	        }
	        if(!strcmp(ptr,"BS0") || !strcmp(ptr,"BS1")) {
		    oflag &= ~020000;
		    oflag |= c;
		    continue;
	        }
	        if(!strcmp(ptr,"VT0") || !strcmp(ptr,"VT1")) {
		    oflag &= ~040000;
		    oflag |= c;
		    continue;
	        }
	        if(!strcmp(ptr,"FF0") || !strcmp(ptr,"FF1")) {
		    oflag &= ~0100000;
		    oflag |= c;
		    continue;
	        }
	        if(flag)
		    oflag |= c;
	        else
		    oflag &= ~c;
	    }
	    else if((c=type(ptr,ctrlmode)) != NULL) {
	        if(strcmp(ptr,"CS5") && c>=0 && c<020) {
		    cflag &= ~017;
		    cflag |= c;
		    continue;
	        }
	        if(!strcmp(ptr,"CS5") || !strcmp(ptr,"CS6") ||
	           !strcmp(ptr,"CS7") || !strcmp(ptr,"CS8")) {
		    cflag &= ~060;
		    cflag |= c;
		    continue;
	        }
	        if(flag)
		    cflag |= c;
	        else
		    cflag &= ~c;
	    }
	    else if((c=type(ptr,discipline)) != NULL) {
	        if(flag)
		    lflag |= c;
	        else
		    lflag &= ~c;
	    }
	    else if((c=type(ptr,control)) != NULL) {
		if(++i >= n) continue;
		if((cc=arg[i][0]) == '#')
		    cc = arg[i][1] - '0';
		ttystate.c_cc[c] = cc;
		continue;
	    }
	    else
	        printf("unknown flag: %s\n",ptr);
        }
    }

    ttystate.c_iflag = iflag;
    ttystate.c_oflag = oflag;
    ttystate.c_cflag = cflag;
    ttystate.c_lflag = lflag;
}


