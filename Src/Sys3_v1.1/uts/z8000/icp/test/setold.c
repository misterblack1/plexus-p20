#include <termio.h>
#include <sys/ttold.h>
#include <sys/ioctl.h>
#define	NULL	-1

struct index {
	char *cmd;
	int swval;
} oldout[] = {
    "TANDEM",	0000001,
    "CBREAK",	0000002,
    "LCASE",	0000004,
    "ECHO",	0000010,
    "CRMOD",	0000020,
    "RAW",	0000040,
    "ODDP",	0000100,
    "EVENP",	0000200,
    "NL0",	0,
    "NL1",	0000400,
    "NL2",	0001000,
    "TAB0",	0,
    "TAB1",	0002000,
    "TAB2",	0004000,
    "XTABS",	0006000,
    "CR0",	0,
    "CR1",	0010000,
    "CR2",	0020000,
    "CR3",	0030000,
    "FF0",	0,
    "FF1",	0040000,
    "BS0",	0,
    "BS1",	0100000,
    0,		NULL
};

/* line speed */
struct index oldspeed[] = {
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
    0,		NULL
};

struct sgttyb oldstate;

setold(line)
int line;
{

    char arg[5][20];
    char buf [80];
    char *ptr,*gets();
    int n,i,c,flag,ispeed,ospeed,flags;

    ioctl(line,TIOCGETP,&oldstate);

    ispeed = oldstate.sg_ispeed;
    ospeed = oldstate.sg_ospeed;
    flags  = oldstate.sg_flags;

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

	    if((c=type(ptr,oldout)) != NULL) {
		if(!strcmp(ptr,"NL0") || !strcmp(ptr,"NL1") ||
		   !strcmp(ptr,"NL2")                      ) {
		    flags &= ~01400;
		    flags |= c;
		    continue;
		}
		if(!strcmp(ptr,"CR0") || !strcmp(ptr,"CR1") ||
		   !strcmp(ptr,"CR2") || !strcmp(ptr,"CR3")) {
		    flags &= ~030000;
		    flags |= c;
		    continue;
	        }
	        if(!strcmp(ptr,"TAB0") || !strcmp(ptr,"TAB1") ||
	           !strcmp(ptr,"TAB2") || !strcmp(ptr,"XTABS")) {
		    flags &= ~06000;
		    flags |= c;
		    continue;
	        }
	        if(!strcmp(ptr,"BS0") || !strcmp(ptr,"BS1")) {
		    flags &= ~0100000;
		    flags |= c;
		    continue;
	        }
	        if(!strcmp(ptr,"FF0") || !strcmp(ptr,"FF1")) {
		    flags &= ~040000;
		    flags |= c;
		    continue;
	        }
	        if(flag)
		    flags |= c;
	        else
		    flags &= ~c;
	    }
	    else if((c=type(ptr,oldspeed)) != NULL) {
		    ispeed = c;
		    ospeed = c;
		    continue;
	    }
	    else
	        printf("unknown flag: %s\n",ptr);
        }
    }

    oldstate.sg_ispeed = ispeed;
    oldstate.sg_ospeed = ospeed;
    oldstate.sg_flags = flags;
}


