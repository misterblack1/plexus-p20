#include <stdio.h>
int fd,k;
main()
{

	fd = open("/dev/vpm0",2);
	fprintf(stderr,"fd = %d\n",fd);
	if(fork()){
		fprintf(stderr,"send fork\n");
		sleep(10);
		send();
		sleep(10);
		exit();
	}else{
		fprintf(stderr,"rec fork\n");
		rec();
		exit();
	}
}
send()
{
char buf[512];
int n;
long t1,t2,t3,time();


	fprintf(stderr,"send fd = %d\n",fd);
	time(&t1);
	while((n = read(0,buf,512))>0){
		fprintf(stderr,"n = %d\n",n);
		k += write(fd,buf,n);
	}
	time(&t2);
	fprintf(stderr,"bytes sent = %d\n",k);
	t3 = t2 - t1;
	if(t3 <= 0)
		t3 = 1;
	t1 = k/t3;
	fprintf(stderr,"time = %d BYTES/SEC = %d\n",t3,t1);
}
rec()
{
char buf[512];
int n;
int stop();


	fprintf(stderr,"rec fd = %d\n",n);
	while(1){
		signal(14,stop);
		alarm(30);
		n = read(fd,buf,512);
		if(n<0){
			fprintf(stderr,"read err\n");
			exit();
		}
		alarm(0);
		fprintf(stderr,"k = %d\n",n);
		k += write(1,buf,n);
	}
}
stop()
{
	fprintf(stderr,"bytes rec'ed %d\n",k);
	exit();
}
