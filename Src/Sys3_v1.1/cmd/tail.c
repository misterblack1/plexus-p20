/* tail command 
**
**	tail where [file]
**	where is +_n[type]
**	- means n lines before end
**	+ means nth line from beginning
**	type 'b' means tail n blocks, not lines
**	type 'c' means tail n characters
**	option 'f' means loop endlessly trying to read more
**		characters after the end of file, on the  assumption
**		that the file is growing
*/

#include	<stdio.h>
#include	<ctype.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<errno.h>

#define	LBIN	4097

struct	stat	statb;
char	bin[LBIN];
int	errno;
int	follow;
int	piped;

main(argc,argv)
char **argv;
{
	register i,j,k;
	long	n,di;
	int	fromend;
	int	partial,bylines;
	char	*p;
	char	*arg;

	lseek(0,(long)0,1);
	piped = errno==ESPIPE;
	arg = argv[1];
	if(argc<=1 || *arg!='-'&&*arg!='+') {
		arg = "-10l";
		argc++;
		argv--;
	}
	fromend = *arg=='-';
	arg++;
	n = (isdigit(*arg)) ? 0 : 10;
	while(isdigit(*arg))
		n = n*10 + *arg++ - '0';
	if(!fromend&&n>0)
		n--;
	if(argc>2) {
		close(0);
		piped = 0;
		if(open(argv[2],0)!=0) {
			fprintf(stderr, "tail: cannot open input\n");
			exit(2);
		}
	}
	bylines = -1;
	while(*arg)
	switch(*arg++) {
	case 'b':
		n <<= 9;
		if(bylines!=-1) goto errcom;
		bylines=0;
		break;
	case 'c':
		if(bylines!=-1) goto errcom;
		bylines=0;
		break;
	case 'f':
		follow = 1;
		break;
	case 'l':
		if(bylines!=-1) goto errcom;
		bylines = 1;
		break;
	default:
		goto errcom;
	}
	if(bylines==-1) bylines = 1;
	if(fromend)
		goto keep;

			/*seek from beginning */

	if(bylines) {
		j = 0;
		while(n-->0) {
			do {
				if(j--<=0) {
					p = bin;
					j = read(0,p,512);
					if(j--<=0)
						fexit();
				}
			} while(*p++ != '\n');
		}
		write(1,p,j);
	} else  if(n>0) {
		if(!piped)
			fstat(0,&statb);
		if(piped||(statb.st_mode&S_IFMT)==S_IFCHR)
			while(n>0) {
				i = n>512?512:n;
				i = read(0,bin,i);
				if(i<=0)
					fexit();
				n -= i;
			}
		else
			lseek(0,n,0);
	}
	while((i=read(0,bin,512))>0)
		write(1,bin,i);
	fexit();

			/*seek from end*/

keep:
	if(n <= 0)
		fexit();
	if(!piped) {
		fstat(0,&statb);
		di = !bylines&&n<LBIN?n:LBIN-1;
		if(statb.st_size > di)
			lseek(0,-di,2);
	}
	partial = 1;
	for(;;) {
		i = 0;
		do {
			j = read(0,&bin[i],LBIN-i);
			if(j<=0)
				goto brka;
			i += j;
		} while(i<LBIN);
		partial = 0;
	}
brka:
	if(!bylines) {
		k =
		    n<=i ? i-n:
		    partial ? 0:
		    n>=LBIN ? i+1:
		    i-n+LBIN;
		k--;
	} else {
		k = i;
		j = 0;
		do {
			do {
				if(--k<0) {
					if(partial)
						goto brkb;
					k = LBIN -1;
				}
			} while(bin[k]!='\n'&&k!=i);
		} while(j++<n&&k!=i);
brkb:
		if(k==i) do {
			if(++k>=LBIN)
				k = 0;
		} while(bin[k]!='\n'&&k!=i);
	}
	if(k<i)
		write(1,&bin[k+1],i-k-1);
	else {
		write(1,&bin[k+1],LBIN-k-1);
		write(1,bin,i);
	}
	fexit();
errcom:
	fprintf(stderr, "usage: tail [+_[n][lbc][f]] [file]\n");
	exit(2);
}
fexit()
{	register int n;
	if (!follow || piped) exit(0);
	for (;;)
	{	sleep(1);
		while ((n = read (0, bin, 512)) > 0)
			write (1, bin, n);
	}
}
