#include <rje.h>

struct joblog lbuf;
struct loghdr header;
int fd;

main(argc,argv)
int argc;
char **argv;
{

	if(argc < 2) {
		printf("Arg count\n");
		exit(1);
	}
	if((fd = open(*(++argv),0)) < 0) {
		printf("Can't open %s\n",*argv);
		exit(1);
	}

	/* Get header and print */

	if((read(fd,&header,sizeof(struct loghdr))) == 0)
		exit(0);
	printf("%d\n",header.h_pgrp);
	while(get()) {
		printf("%s ",lbuf.j_file);
		printf("%d %ld ",lbuf.j_uid,lbuf.j_cnt);
		printf("%d\n",lbuf.j_lvl);
	}
}

get()
{
	if(read(fd,&lbuf,sizeof(lbuf)) == 0)
		return(0);
	return(1);
}
