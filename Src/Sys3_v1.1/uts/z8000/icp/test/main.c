#include <termio.h>
#include <sys/ioctl.h>
#include <signal.h>
#define	OPEN	1
#define	CLOSE	2
#define	READ	3
#define	WRITE	4
#define	GETA	5
#define	SETA	6
#define	SETAW	7
#define	SETAF	8
#define	BRK	9
#define	XONC	10
#define	XOFFC	11
#define	FLSH	12
#define	QUIT	13
#define	NULL	-1

struct index {
    char *cmd;
    int swval;
} incmd[] = {

	"open",		OPEN,
	"close",	CLOSE,
	"read",		READ,
	"write",	WRITE,
	"get",		GETA,
	"set",		SETA,
	"setw",		SETAW,
	"setf",		SETAF,
	"brk",		BRK,
	"start",	XONC,
	"stop",		XOFFC,
	"flush",	FLSH,
	"quit",		QUIT,
	0,		NULL,
};

int openttys[32];

char inbuf [200];
char outbuf[200];

extern struct termio ttystate;
extern struct sgttyb oldstate;

main()
{
    char buf [80];
    char buf0[80];
    char buf1[80];
    char buf2[80];
    char buf3[80];
    char buf4[80];
    char *dev = "/dev/tty000";
    char *ptr,*gets();
    int n,sw,val1,oflag,fd,user,val2,nc,i,c;

    /* set process group */
    setpgrp();

    /* enable interrupts */
    signal(SIGHUP,SIG_DFL);
    signal(SIGINT,SIG_DFL);
    signal(SIGQUIT,SIG_DFL);

    while(1) {
	putchar(':');
	ptr = gets(buf);
	n = sscanf(ptr,"%s%s%s%s%s\n",buf0,buf1,buf2,buf3,buf4);
	if(n == NULL) continue;
	switch(sw = type(buf0,incmd)) {
	case OPEN:

		/* get line # */
	    if(n < 2)
		val1 = 0;
	    else if(sscanf(buf1,"%d",&val1) == NULL || val1<0 || val1>32) {
		printf("invalid line #\n");
		continue;
	    }
	    if(val1 > 9) {
	        dev[8] = '0' + val1/10;
		dev[9] = '0' + val1%10;
		dev[10] = '\0';
	    }
	    else {
	        dev[8] = '0' + val1;
		dev[9] = '\0';
	    }


		/* line already open? */
	    if(openttys[val1]) {
		printf("line %d is already open.\n",val1);
		continue;
	    }

		/* get file status */
	    oflag = 0;
	    if(n >= 3) {
		if (strchr(buf2,'r') && !strchr(buf2,'w')) oflag = 0;
		else if (!strchr(buf2,'r') && strchr(buf2,'w')) oflag = 1;
		else if (strchr(buf2,'r') && strchr(buf2,'w')) oflag = 2;
		if(strchr(buf2,'d')) oflag |= 4;

	    }
		/* open line */
	    if((fd = open(dev,oflag)) == -1) {
		printf("can not open device: %s with mode %o\n",dev,oflag);
		continue;
	    }

		/* remember open lines */
	    openttys[val1] = fd;
	    printf("%s %o %d\n",dev,oflag,val1);
	    break;

	case CLOSE:

		/* get line # */
	    if(n < 2)
		val1 = 0;
	    else if(sscanf(buf1,"%d",&val1) == NULL || val1<0 || val1>32) {
		printf("invalid line #\n");
		continue;
	    }

		/* line open? */
	    if(!openttys[val1]) {
		printf("line is already closed\n");
		continue;
	    }

	    close(openttys[val1]);
	    openttys[val1] = 0;
	    break;

	case READ:

		/* get line # */
	    if(n < 2)
		val1 = 0;
	    else if(sscanf(buf1,"%d",&val1) == NULL || val1<0 || val1>32) {
		printf("invalid line #\n");
		continue;
	    }

		/* line open? */
	    if(!openttys[val1]) {
		printf("line is not open\n");
		continue;
	    }

		/* get buffer size */
	    if(n < 3) {
		val2 = 1;
	    }
	    else if(sscanf(buf2,"%d",&val2) == NULL || val2<0 || val2>128) {
		printf("invalid buffer size\n");
		continue;
	    }

		/* get repeat option */
	    if(n < 4) {
		user = 1;
	    }
	    else if(!strcmp(buf3,"man")) user = 1;
	    else if(!strcmp(buf3,"auto")) user = 0;
	    else user = 1;

		/* read data from line */
	    do {
		nc = read(openttys[val1],inbuf,val2);
		if(!nc && !user) break;

		printf("%d: ",nc);
		for(i=0;i<nc;i++) {
		    if(inbuf[i]&0200)
			printf("\\%o",inbuf[i]&0377);
		    else if(
		             inbuf[i] <  ' '  &&
		             inbuf[i] != '\n' &&
		             inbuf[i] != '\t' &&
		             inbuf[i] != '\b' &&
		             inbuf[i] != '\r'
		           )  printf("^%c",inbuf[i]+'A'-1);
		    else if(inbuf[i] == 0177) printf("\\0177");
		    else  putchar(inbuf[i]);
		}
		printf("\n");
		if(user) {
		    putchar('>');
		    if((c=getchar()) == 'q') {
		        for( ; c != '\n' ; c=getchar());
			break;
		    }
		    else for( ; c != '\n' ; c=getchar());
		}
	    }
	    while(1);
	    break;

	case WRITE:

		/* get line # */
	    if(n < 2)
		val1 = 0;
	    else if(sscanf(buf1,"%d",&val1) == NULL || val1<0 || val1>32) {
		printf("invalid line #\n");
		continue;
	    }

	    while(nc = read(0,outbuf,80)) write(openttys[val1],outbuf,nc);

	    break;
	case GETA:

		/* get line # */
	    if(n < 2)
		val1 = 0;
	    else if(sscanf(buf1,"%d",&val1) == NULL || val1<0 || val1>32) {
		printf("invalid line #\n");
		continue;
	    }

		/* line open? */
	    if(!openttys[val1]) {
		printf("line is not open\n");
		continue;
	    }

		/* get new/old command format */
	    if(n < 3) {
		val2 = 0;
	    }
	    else if(!strcmp(buf2,"new")) val2 = 0;
	    else if(!strcmp(buf2,"old")) val2 = 1;
	    else {
		printf("invalid status request (old/new)\n");
		continue;
	    }
	    if(val2 == 0)
		getnew(openttys[val1]);
	    else
		getold(openttys[val1]);
	    break;

	case SETA:

		/* get line # */
	    if(n < 2)
		val1 = 0;
	    else if(sscanf(buf1,"%d",&val1) == NULL || val1<0 || val1>32) {
		printf("invalid line #\n");
		continue;
	    }

		/* line open? */
	    if(!openttys[val1]) {
		printf("line is not open\n");
		continue;
	    }

		/* get new/old command format */
	    if(n < 3) {
		val2 = 0;
	    }
	    else if(!strcmp(buf2,"new")) val2 = 0;
	    else if(!strcmp(buf2,"old")) val2 = 1;
	    else {
		printf("invalid status request (old/new)\n");
		continue;
	    }
	    if(val2 == 0)
		setnew(openttys[val1]);
	    else
		setold(openttys[val1]);


	    if(val2 == 0)
		ioctl(openttys[val1],TCSETA,&ttystate);
	    else
		ioctl(openttys[val1],TIOCSETP,&oldstate);

	    break;

	case SETAW:
	    printf("wait for output, then set line parameters\n");
		/* get line # */
	    if(n < 2)
		val1 = 0;
	    else if(sscanf(buf1,"%d",&val1) == NULL || val1<0 || val1>32) {
		printf("invalid line #\n");
		continue;
	    }

		/* line open? */
	    if(!openttys[val1]) {
		printf("line is not open\n");
		continue;
	    }

		/* get new/old command format */
	    if(n < 3) {
		val2 = 0;
	    }
	    else if(!strcmp(buf2,"new")) val2 = 0;
	    else if(!strcmp(buf2,"old")) val2 = 1;
	    else {
		printf("invalid status request (old/new)\n");
		continue;
	    }

	    if(val2 == 0)
		setnew(openttys[val1]);

	    ioctl(openttys[val1],TCSETAW,&ttystate);

	    break;

	case SETAF:
	    printf("wait for output, flush Qs, then set line parameters\n");
		/* get line # */
	    if(n < 2)
		val1 = 0;
	    else if(sscanf(buf1,"%d",&val1) == NULL || val1<0 || val1>32) {
		printf("invalid line #\n");
		continue;
	    }

		/* line open? */
	    if(!openttys[val1]) {
		printf("line is not open\n");
		continue;
	    }

		/* get new/old command format */
	    if(n < 3) {
		val2 = 0;
	    }
	    else if(!strcmp(buf2,"new")) val2 = 0;
	    else if(!strcmp(buf2,"old")) val2 = 1;
	    else {
		printf("invalid status request (old/new)\n");
		continue;
	    }

	    if(val2 == 0)
		setnew(openttys[val1]);

	    ioctl(openttys[val1],TCSETAF,&ttystate);

	    break;

	case BRK:
	    printf("wait for output, then break\n");
		/* get line # */
	    if(n < 2)
		val1 = 0;
	    else if(sscanf(buf1,"%d",&val1) == NULL || val1<0 || val1>32) {
		printf("invalid line #\n");
		continue;
	    }

		/* line open? */
	    if(!openttys[val1]) {
		printf("line is not open\n");
		continue;
	    }

		/* get break request parameter */
	    if(n < 3) {
		val2 = 1;
		printf("default is no break\n");
	    }
	    else if(!strcmp(buf2,"break"))
		val2 = 0;
	    else
		val2 = 1;

	    ioctl(openttys[val1],TCSBRK,val2);

	    break;
	case XONC:
	    printf("start control\n");
		/* get line # */
	    if(n < 2)
		val1 = 0;
	    else if(sscanf(buf1,"%d",&val1) == NULL || val1<0 || val1>32) {
		printf("invalid line #\n");
		continue;
	    }

		/* line open? */
	    if(!openttys[val1]) {
		printf("line is not open\n");
		continue;
	    }

	    ioctl(openttys[val1],TCXONC,1);

	    break;

	case XOFFC:
	    printf("stop control\n");
		/* get line # */
	    if(n < 2)
		val1 = 0;
	    else if(sscanf(buf1,"%d",&val1) == NULL || val1<0 || val1>32) {
		printf("invalid line #\n");
		continue;
	    }

		/* line open? */
	    if(!openttys[val1]) {
		printf("line is not open\n");
		continue;
	    }

	    ioctl(openttys[val1],TCXONC,0);

	    break;

	case FLSH:
	    printf("flush Qs\n");
		/* get line # */
	    if(n < 2)
		val1 = 0;
	    else if(sscanf(buf1,"%d",&val1) == NULL || val1<0 || val1>32) {
		printf("invalid line #\n");
		continue;
	    }

		/* line open? */
	    if(!openttys[val1]) {
		printf("line is not open\n");
		continue;
	    }

		/* get flush mode request parameter */
	    if(n < 3) {
		val2 = 2;
		printf("default is flush input & output\n");
	    }
	    else if(!strcmp(buf2,"in"))
		val2 = 0;
	    else if(!strcmp(buf2,"out"))
		val2 = 1;
	    else if(!strcmp(buf2,"in/out"))
		val2 = 2;
	    else {
		printf("in, out, in/out only\n");
		continue;
	    }

	    ioctl(openttys[val1],TCFLSH,val2);

	    break;

	case QUIT:
	    printf("good bye\n");
	    goto done;
	default:
	    printf("unknown request. ignore\n");
	    break;
	}
    }
done:;
}

/* convert string into switch id */

int type(ptr,dex)
char *ptr;
struct index dex[];
{
    int i;
    for(i=0 ; dex[i].cmd && strcmp(dex[i].cmd , ptr); i++);
    return dex[i].swval;
}
