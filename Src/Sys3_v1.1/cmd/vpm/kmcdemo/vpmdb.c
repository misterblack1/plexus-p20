#include <stdio.h>
main(argc,argv)
int argc;
char *argv[];
{
	int nn,num;
	char buf[4000];
	int fdco;
	FILE *fdnm;
	char nm[36];
	char core[36];
	char str[32];

	if(argc > 1)
		strcpy(nm,argv[1]);
	else
		strcpy(nm,"nmlist");
	if(argc > 2)
		strcpy(core,argv[2]);
	else
		strcpy(core,"core");
	if((fdnm = fopen(nm,"r"))  == NULL){
		printf("Can not open %s\n",nm);
		exit();
	}
	if((fdco = open(core,2)) < 0){
		printf("Can not open %s\n",core);
		exit();
	}
	lseek(fdco,(long)027500,0);
	nn = read(fdco,buf,4000);
	while(fscanf(fdnm,"%o %s",&num,str)  != EOF)
		printf("%o	%s	%d\n",num,str,(unsigned)buf[num]);
}
