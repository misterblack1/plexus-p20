/*	ntar.c
 *
 *
 *	A Non-Toxic ARchive program.
 *
 *
 *	version 1.7, 5/8/80
 *
 *
 */

#include "stdio.h"
#define TRUE 1
#define FALSE 0
#define BLK 512		/* input buffer size */
#define HSIZE 128	/* max size of headers and trailers */
#define FSIZE 128	/* max size of filenames (with paths) */


FILE *rch = stdin;	/* input channel */
FILE *wch = stdout;	/* output channel */
FILE *ech = stderr;	/* error channel */
int rargc;		/* command line pointers */
char **rargv;
int func, list;		/* control variables for requested functions */
int ibp = 0;		/* input buffer pointer */
int backup = 0;		/* re-read char count */
char fn[FSIZE];		/* filename buffer */
char ifn[FSIZE];	/* command line file names buffer */
int fnp = 0;		/* filename (after path) pointer */

char head[HSIZE] = {
	"**************************************************start file: "};
char tail[HSIZE] = {
	"**************************************************end file: "};


char whatstring[] =
	{"@(#)ntar.c	1.7"};
main(argc,argv)
int argc;
char ** argv;
{
	register int i;
	register char *cp, *p;

	extern int null(), wrch();

	while (--argc && ((++argv)[0][0] == '-'))

	    switch (argv[0][1])	{

		case 'p':
		case 'g':
			if (!func)
				func = argv[0][1];	/* save function key */
			    else
				error("conflicting or duplicate functions");
			break;

		case 'l':
			list++;
			break;

		case 'd':
			if (--argc < 0)  opterr();
			p = (++argv)[0];
			for (fnp=0; ((fnp<(FSIZE-1))&&(fn[fnp++]=(*p++))); )
					;
			if (fnp < (FSIZE-1))  fn[fnp-1] = '/';
			break;

		case 'h':
			if (--argc < 0) opterr();
			p = (++argv)[0];
			cp = head;
			for (i=0; (i<(HSIZE-1))&&(*cp++ = *p++); i++)
				;
			break;

		case 't':
			if (--argc < 0) opterr();
			p = (++argv)[0];
			cp = tail;
			for (i=0; (i<(HSIZE-1))&&(*cp++ = *p++); i++)
				;
			break;

		default:	opterr();
			}

	if (!func && !list)	{
		print("no function (-p, -g, or -l)\n");
		exit(1);	}

	rargc = ++argc;		/* use rargc, rargv instead of originals */
	rargv = --argv;

	if (--rargc > 0)
		getfile();	/* open first file */

	if (!func)		/* then list only */

		while (find(head,null))
			pname();

	else if (func == 'p')	{

		while (wrch(gch()) != EOF)  ;	/* copy text */
		fclose(wch);	}

	else	{		/* do get function */

		while (find(head, wrch))	{

			for (i=fnp; ((i<(FSIZE-1)) &&
				((fn[i++]=gch())!='\n')); ) ;
			fn[i-1] = 0;	/* terminate filename string */
			if (list) { print(fn); print("\n"); }
			if ((i = creat(fn, 0644)) < 0)
				error("can't create ", fn);
			    else wch = fdopen(i, "w");
			if (!find(tail, wrch))
				error("missing tail for ", &fn[fnp]);
			if ((!match(&fn[fnp])) || (gch()!='\n'))
				error("wrong tail name for ", &fn[fnp]);
			fclose(wch);
			wch = stdout;		/* reset to standard output */
				}}

	exit(0);
}


opterr()
{
	print("usage:  ntar [-h `header'] [-t `trailer'] [-d path] [-l] [-g] [-p] [file ...]\n");
	exit(-1);
}
find(str, f)
char *str;
int (*f)();
{	char ch;

	while (!match(str))
		do	{
		    if ((ch = gch()) == EOF)  return FALSE;
		    (*f)(ch);	}
		while (ch != '\n');

	return TRUE;
}


match(str)
char *str;
{	int i;

	for (i = 1; gch() == *str++; i++)
		if (!*str) return TRUE;
	ibp = ibp - i;
	backup = backup + i;
	return FALSE;
}


gch()
{
	static char ib[BLK];		/* input buffer */

	if (ibp < 0) ibp = ibp + BLK;	/* wrap around */
	if (ibp == BLK) ibp = 0;

	if (!backup)	{

	    while ((ib[ibp++] = getc(rch)) == EOF)	{

		fclose(rch);		/* close input */

		if (ifn[0])	{
			wrstr(tail);	/* write matching tail */
			wrstr(ifn);
			wrch('\n');
			ifn[0] = 0;	}

		if (--rargc <= 0)
			return EOF;	/* done if no more files */
		    else
			getfile();	/* otherwise get the next */

		ibp--;	}

	    return ib[ibp-1];	}

	else if (ib[ibp++] != EOF) {
		backup--;
		return ib[ibp-1];	}

	else return EOF;
}

getfile()
{	register int i;
	register char *p;

	if ((rch=fopen((++rargv)[0],"r")) == NULL)
		error("can't open ", rargv[0]);

	if (func == 'p')	{	/* put? write name if so */
		wrstr(head);
		wrstr(rargv[0]);	/* header */
		wrch('\n');
		for (p=rargv[0],i=0; ifn[i++]=(*p++); ) ;
		if (list)	{
			print(ifn);	/* give name */
			print("\n");	}}
}

error(st1, st2)
char *st1, *st2;
{
	print(st1);
	print(st2);
	print("\n");
	exit(-1);
}


print(str)
char *str;
{
	while (*str)
		putc(*str++, ech);

}


pname()
{
	while (putc(gch(), ech) != '\n');
}


wrstr(str)
char *str;
{
	while (*str)
		wrch(*str++);
}


wrch(ch)
char ch;
{
	return (ch != EOF ? putc(ch,wch) : ch);
}



null(ch)
char ch;
{}
