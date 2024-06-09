#include <stdio.h>
#include <sys/ioctl.h>
#define	MASK16 0177777
#define	SSTOL(x,y)	((((long)x)<<16)|(((long)y)&MASK16))
struct event {
	char	chno;
	char	ctn;
	short	seq;
	char	typ;
	char	dev;
	short	wd1;
	short	wd2;
	short	lbolt1;
	short	lbolt2;
}ev[36];
char *pp;
#define RSIZE	sizeof(struct event)
main()
{
	int fd,k,n;
	long	time0,time1,lbolt;

	fd = open("/dev/snap",2);
	setbuf(stdout,NULL);
	printf("fd = %d\n",fd);
	if(fd< 0)
		exit(1);
	k = ioctl(fd,VPMTRCO,0);
	printf("sr = %d\n",k);
	while((n = read(fd,ev,RSIZE)) != RSIZE){
	}
	time0 = SSTOL(ev[0].lbolt2,ev[0].lbolt1);
	printf("%d %c %o",ev[0].seq,ev[0].typ,ev[0].dev);
	printf(" %o %o",(ev[0].wd1&0377),(ev[0].wd1>>8)&0377);
	printf(" %o %o",(ev[0].wd2&0377),(ev[0].wd2>>8)&0377);
	time1 = SSTOL(ev[0].lbolt2,ev[0].lbolt1) - time0;
	printf(" %u\n",time1);
	for(;;){
		n = read(fd,ev,512);
		for(k = 0;k < n/RSIZE;k++){
		printf("%d %c %o",ev[k].seq,ev[k].typ,ev[k].dev);
		printf(" %o %o",(ev[k].wd1&0377),(ev[k].wd1>>8)&0377);
		printf(" %o %o",(ev[k].wd2&0377),(ev[k].wd2>>8)&0377);
		time1 = SSTOL(ev[k].lbolt2,ev[k].lbolt1) - time0;
		printf(" %u\n",time1);
		}
	}
}
