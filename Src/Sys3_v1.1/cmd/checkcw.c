#include "stdio.h"
char *CW = ".CW";
char *CN = ".CN";
char *CD = ".CD";
char buf[256];
char l[20];
char r[20];
char	*pgmname;
char	*left;
char	*right;
int	wleft = 0;
int	wright = 0;
int	cwfound = 0;
FILE *in;
main(argc,argv)
int argc;
char **argv;
{

	char *cp;
	int nleft,nright;
	int comflag;
	int lineno;
	int cwno;
	int lmatch;
	char *cwname;

	pgmname = argv[0];
	if(argc < 2){
		printf("Usage: %s -lx -ry file ...\n",pgmname);
		printf("prints lines with unbalanced delimiters  x  y\n");
		printf("and line numbers of unmatched .CW .CN pairs\n");
		exit(1);
	}

	argv++;
	while (--argc>1 && **argv == '-'){
		switch(*++*argv){

		case 'l':
			if(*++*argv)
				left = *argv;
			else {
				argc--;
				left = *++argv;
			}
			break;

		case 'r':
			if(*++*argv)
				right = *argv;
			else {
				argc--;
				right = *++argv;
			}
			break;

		default:
			error("Illegal argument :%s",*argv);
		}

	argv++;
	}
	cwno = 0;
	while(argc--){
		if((in=fopen(*argv,"r")) == NULL)
			error("Cannot open %s",*argv);

		lineno = 0;
		if(left != 0)
			wleft = strlen(left);
		if(right != 0)
			wright = strlen(right);
		while(fgets(buf,255,in) != NULL){
			lineno++;
			nright = nleft = 0;
			if(strncmp(buf,CW,3) == 0){
				cwfound = 1;
				getdelim(buf);
				if(cwno != 0)
					printf("Missing CN CW1 = %s: %d CW2 = %s: %d\n",cwname,cwno,*argv,lineno);
				cwno = lineno;
				cwname = *argv;
			}
			if(strncmp(buf,CN,3) == 0){
				cwfound = 0;
				getdelim(buf);
				if(cwno == 0)
					printf("Missing CW  CN = %s:%d\n",*argv,lineno);
				cwno = 0;
			}
			if(strncmp(buf,CD,3) == 0)
				getdelim(buf);

			comflag= (strcmp(left,right) == 0) ? 1 : 0;

			for(cp=buf; *cp != '\0'; cp++){
				lmatch = 0;
				if(wleft != 0)
					if(strncmp(left,cp,wleft) == 0){
						nleft++;
					/*do not count left delim as right */
					/* if left and right identical */
						lmatch = 1;
					}
				if(wright != 0 && lmatch == 0)
					if(strncmp(right,cp,wright) == 0){
						nright++;
					}
				if(*cp == '\n')
					break;
			}


			if(cwfound == 0) {
				/* do not look for delims between
				  .CW and .CN pairs */

				if(comflag){
					if(nleft%2 != 0)
						printf("%s",buf);
				} else {
					if(nleft != nright)
						if(wleft != 0 || wright != 0)
							printf("%s",buf);
				}
			}
		}
	argv++;
	}
	if(cwno != 0)
		printf("Missing CN at EOF, CW in %s :%d\n",cwname,cwno);
}

error(s1,s2)
char *s1, *s2;
{
	fprintf(stderr,"%s: ",pgmname);
	fprintf(stderr,s1,s2);
	putc('\n',stderr);
	exit(1);
}

getdelim(cpp)
char *cpp;
{

	char *cp;
	char *sp;

	if(strlen(cpp) > 3){
		for(cp=cpp; *cp != '\0'; cp++){
			if(*cp == '-'){
				switch (*++cp){

				case 'l':
					cp++; /* pass over "l" */
					sp = l;
					while(*cp != ' ' && *cp != '\t' && *cp != '\n') 
						*sp++ = *cp++;
					*sp = '\0';
					left = l;
					wleft = strlen(left);
					break;

				case 'r':
					cp++; /* pass over "r" */
					sp = r;
					while(*cp != ' ' && *cp != '\t' && *cp != '\n')
						*sp++ = *cp++;
					*sp = '\0';
					right = r;
					wright = strlen(right);
					break;
				}
			}
			if(*cp == '\n')
				break;
		}
	}
}
