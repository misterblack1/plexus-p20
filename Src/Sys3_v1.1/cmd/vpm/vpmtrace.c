#include <stdio.h>
#include "sys/ioctl.h"
struct event {
	char	chno;
	char	ctn;
	short	seq;
	char	typ;
	char	dev;
	short	wd1;
	short	wd2;
}ev[52];
char *pp;
#define RSIZE	sizeof(struct event)
main()
{
	int fd,k,n;

	fd = open("/dev/trace",2);
	setbuf(stdout,NULL);
	printf("fd = %d\n",fd);
	k = ioctl(fd,VPMTRCO,0);
	printf("sr = %d\n",k);
	for(;;){
		n = read(fd,ev,512);
		for(k = 0;k < n/RSIZE;k++){
			if(((ev[k].wd2>>8)&0377) == 0){
				printf("%d %c %o %o %u\n",ev[k].seq,ev[k].typ,ev[k].dev,
				ev[k].wd1,ev[k].wd2);
			}else{
				printf("%d %c %o %o %u %u\n",ev[k].seq,ev[k].typ,ev[k].dev,
				ev[k].wd1,ev[k].wd2&0377,(ev[k].wd2>>8)&0377);
			}
		}
	}
}
