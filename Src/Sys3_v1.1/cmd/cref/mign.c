#
# include <stdio.h>
# include "mcons.h"
FILE	*ibuf;
struct	htab	{
		int	hsiz;
		short	ssiz;
		int	nsym;
		int	curb;
		short	*hptr;
		char	*symt;
		};

struct	htab	xtab;

short	ipsp[PTRI];
char	issp[CHARI];
search(symbol,length,params,install)
	char	*symbol;
	int	length;
	struct	htab	*params;
	int	install;
{
	char	*sp,*p;
	int	curb,hsiz,nsym;
	short	*hptr,ssiz;
	char	*symt;
	auto	h,i,j;

	hptr = params->hptr;
	hsiz = params->hsiz;
	symt = params->symt;
	ssiz = params->ssiz;
	curb = params->curb;
	nsym = params->nsym;

	symbol[length] = '\0';
	sp = symbol;

	i = length;
	h = 1;
	while(i--)
		h *= *sp++;

	if(h == 0100000)h = 1;
	h = h<0?(-h)%hsiz:h%hsiz;
	if(h == 0)	h++;
/*	printf("%s %d\n",symbol,h);	/*DEBUG*/

	while((p = &symt[hptr[h]]) > symt) {
		j = length + 1;
		sp = symbol;
		while(j--) {
			if(*p++ != *sp++)	goto no;
		}
		return(*p);
no:
		h = (h + h)%hsiz;
	}
	if(install) {
		if(++nsym >= hsiz)	err("too many","symbols");

		hptr[h] = curb;
		length++;
		if((short)(curb + length) >= ssiz)	err("too many","chars");

		while(length--)
			symt[curb++] = *symbol++;
		symt[curb++] = install;
		params->curb = curb;
		params->nsym = nsym;
	}
	return(0);
}

main(argc,argv)	char **argv; 
{

	auto	ofile,i,error,val;
	char t[20];

	if(argc != 3)exit();

	if((ibuf = fopen(argv[1],"r")) == NULL)err("open",argv[1]);

	ofile = creat(argv[2],0644);
	if(ofile < 0)err("creat",argv[2]);

	xtab.hptr = ipsp;
	xtab.symt = issp;
	xtab.hsiz = PTRI;
	xtab.ssiz = CHARI;
	xtab.nsym = 0;
	xtab.curb = 1;

	i = xtab.hsiz;
	while(i)xtab.hptr[--i] = 0;

	i = -1;
	while((t[++i] = getc(ibuf)) >= 0){
		if(t[i] == '\n'){
			i = search(t,i,&xtab,1);
			i = -1;
		} else {
			if(t[i] == '\t') {
				val = gen();
				search(t,i,&xtab,val);
				i = -1;
			}
		}
	}

/*	printf("collisions = %d\nsymbols = %d\n",ncol,nsym); /*INSTR*/
	i = 0100200;
	error = write(ofile,&i,2);
	if(error < 0)err("write",argv[2]);
	i = 2*xtab.hsiz;
	error = write(ofile,&i,2);
	if(error < 0)err("write",argv[2]);
	error = write(ofile,&xtab.ssiz,2);
	if(error < 0)err("write",argv[2]);
	error = write(ofile,xtab.hptr,i);
	if(error < 0)err("write",argv[2]);
	error = write(ofile,xtab.symt,xtab.ssiz);
	if(error < 0)err("write",argv[2]);
	exit(0);
}

err(a,b)
	char	*a,*b;
{
	printf("%s %s\n",a,b);
	exit();
}

gen()
{
	auto i,v;

	v = 0;
	while((i = getc(ibuf)) != EOF) {
		if(i == '\n')	break;
		v = v*10 + (i - '0');
	}
	return(v);
}
