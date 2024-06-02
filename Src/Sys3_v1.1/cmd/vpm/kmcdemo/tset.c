char	msg[] = "sdflkjavp8wtfmn v;uzxv9wrjhd vpizvpf q98bdfkjhfb v98f:2rwhf sdf s8";
char	buff[512];
main() {
	int fd;
	int count;
printf("open\n");
	fd = open("/dev/vpm0",2);
	if (fd < 0)
		exit(13);
printf("write\n");
	count = write(fd,msg,sizeof msg);
printf("count=%d\n",count);
	if (count != sizeof msg)
		exit(14);
	count = read(fd,buff,512);
printf("read count = %d\n",count);
		printf("String = %s\n",buff);
	if (count != sizeof msg)
		exit(15);
	if (strcmp(msg,buff) != 0){
		printf("String = %s\n",buff);
		exit(16);
	}
	printf("It worked !!!!!!");
	exit(0);
}
