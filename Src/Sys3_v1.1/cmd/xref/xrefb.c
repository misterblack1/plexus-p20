#include "ctype.h"
#include "output.h"
#include "input.h"
#define eqstr(a,b) (cf(a,b)==0)
#define cpstr(x) movstr(w,x)
#define MAXW 128

char	w[MAXW];
char	word();
int	width=120;

main(argc,argv)
int	argc;
char	*argv[];
{
	char f1[MAXW], f2[MAXW];
	FILEBLK f;
	char first;

	/* args ? */
	if(argc>1) {
		width=atoi(argv[1]);
	}

	initf(0);
	f1[0]=0; f2[0]=0;

	while(eof==0) {
		word();

		if(w[0]!=first) {
			first=w[0];
			newline();
		}

		if(eqstr(w,f1)) {
			word();
			if(!eqstr(w,f2)) {
				print("\n%16t%s", w);
				ensure(32);
				cpstr(f2);
			}
		}else{	cpstr(f1);
			print("\n%s", f1);
			ensure(16);
			word(); cpstr(f2);
			print("%s", f2);
			ensure(32);
		}

		word();
		if(charpos()>=width) {
			print("\n%32t");
		}
		print("%s %4t", w);
	}
}

ensure(n)
int	n;
{
	if(charpos()<n-1)
		print("%T",n);
	else	print("\n%T",n);
}

char	word()
{
	register char *p=w;
	register c;

	while((c=readc())!=':' && c!='\n' && c!=EOF) {
		if(p<&w[MAXW]) {
			*p++ = c;
		}
	}
	*p++ = 0;
	return(c&0377);
}
