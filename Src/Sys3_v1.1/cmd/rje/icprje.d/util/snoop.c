#include <errno.h>
#include <stdio.h>
#include <sys/ioctl.h>

#define TRACE	"/dev/trace"

struct event {
	char	chno;
	char	ctn;
	short	seq;
	char	typ;
	char	dev;
	short	wd1;
	short	wd2;
} ev[52];

int dev;
char buf[64];

main(argc,argv)
int argc;
char **argv;
{
	register int k, n, fd;

	if((fd = open(TRACE,0)) < 0) {
		fprintf(stderr,"Can't open %s\n",TRACE);
		exit(1);
	}
	while(*++*argv);
	dev = (int) (*--*argv - '0');
	setbuf(stdout,NULL);
	if(ioctl(fd,VPMTRCO,0) == -1) {
		fprintf(stderr,"Ioctl failed\n");
		exit(1);
	}
	fprintf(stdout,"Tracing vpm%o\n",dev);
	for(;;) {
		sleep(1);
		n = read(fd,ev,512);
		for(k = 0; k < n/10; k++)
			prinfo(&ev[k]);
	}
}

prinfo(st)
register struct event *st;
{

	if(((int)st->dev & 07) != dev)
		return;
	sprintf(buf,"%d\t",(st->seq) % 100);
	cvttyp(st->typ);
	getwd1(st);
	getwd2(st);
	fprintf(stdout,"%s\n",buf);
}

cvttyp(c)
register char c;
{
	char ch;

	switch(c) {
	case 's':
		strcat(buf,"ST\tStart");
		break;
	case 'T':
		strcat(buf,"TR\t");
		break;
	case 'o':
		strcat(buf,"OP\tOpened");
		break;
	case 'p':
		strcat(buf,"OP\tFailed");
		break;
	case 't':
		strcat(buf,"ST\tStopped");
		break;
	case 'w':
		strcat(buf,"WR\t");
		break;
	case 'r':
		strcat(buf,"RD\t");
		break;
	case 'q':
		strcat(buf,"OP\tFailed(open)");
		break;
	case 'c':
		strcat(buf,"CL\tClosed");
		break;
	case 'X':
		strcat(buf,"RX\tBuf");
		break;
	case 'R':
		strcat(buf,"RR\tBuf");
		break;
	case 'E':
		strcat(buf,"SC\tExit");
		break;
	case 'C':
		strcat(buf,"CL\tClean");
		break;
	case 'S':
		strcat(buf,"ST\tStartup");
		break;
	default:
		ch = c;
		strncat(buf,&ch,1);
		strcat(buf,"\t");
		return;
	}
}

getwd1(s)
register struct event *s;
{
	char tbuf[12];

	switch(s->typ) {
	case 'r':
	case 'w':
		sprintf(tbuf,"%d bytes",s->wd1);
		strcat(buf,tbuf);
		break;
	default:
		break;
	}
}

getwd2(s)
register struct event *s;
{
	char tbuf[8];

	switch(s->typ) {
	case 'p':
		if(s->wd1 == ENXIO)
			strcat(buf,"(dev)");
		else
			strcat(buf,"(set)");
		break;
	case 'E':
		sprintf(tbuf,"(%d)",s->wd2);
		strcat(buf,tbuf);
		break;
	case 'T':
		switch(s->wd2) {
		case 0:
			strcat(buf,"Started");
			break;
		case 1:
			strcat(buf,"R-ACK");
			break;
		case 2:
			strcat(buf,"R-WAIT");
			break;
		case 3:
			strcat(buf,"R-ENQ");
			break;
		case 4:
			strcat(buf,"S-EOT");
			break;
		case 5:
			strcat(buf,"R-ERRBLK");
			break;
		case 6:
			strcat(buf,"R-NAK");
			break;
		case 7:
			strcat(buf,"R-OKBLK");
			break;
		case 8:
			strcat(buf,"R-SEQERR");
			break;
		case 9:
			strcat(buf,"R-JUNK");
			break;
		case 10:
			strcat(buf,"TIMEOUT");
			break;
		case 11:
			strcat(buf,"S-ENQ");
			break;
		case 12:
			strcat(buf,"S-NAK");
			break;
		case 13:
			strcat(buf,"S-ACK");
			break;
		case 14:
			strcat(buf,"S-BLK");
			break;
		case 15:
			strcat(buf,"S-BADBLK");
			break;
		default:
			sprintf(tbuf,"%-7x",s->wd2);
			strcat(buf,tbuf);
		}
		break;
	case 'r':
	case 'w':
	case 's':
	case 'o':
	case 't':
	case 'q':
	case 'c':
	case 'X':
	case 'R':
	case 'C':
	case 'S':
		break;
	default:
		sprintf(tbuf,"%-5o",s->wd2);
		strcat(buf,tbuf);
	}
}
