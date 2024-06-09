/*	bldtab - v 1.26 of 5/8/80
 *	
 *	This program translates an input stream of nroff/troff
 *	name table entries into a hashed table description.  The hash
 *	function used is defined by tdef.hd, the same definition used
 *	by the nroff/troff program.
 */

#include "tdef.hd"	/* get hashing table definitions */
#include "stdio.h"

#ifdef ebcdic
extern char etoa[], atoe[];
#endif

char version[] = "@(#)bldtab	1.26";
char ntversion[] = NTVER;	/* nroff/troff version */



#define vallen 15	/* length of value field */
#define llen 80		/* input line length */
#define bsize 512	/* buffer size */
#define CH 0177		/* character mask */
#define TRUE 1
#define FALSE 0


struct table	{
	int name;	/* name entry */
	int hval;	/* computed hash value */
	char val[vallen]; /* value text */
		} table[NN+NM];

int htable[NN+NM];	/* hashed table */


/*	global flags	*/

FILE *filein;		/* input from file flag */
int stats;		/* output hashing statistics */
int eof;		/* end of input reached */
int valf = -1;		/* uninitialized (-1), name (FALSE), or name
			 *	and value format (TRUE) */


/*	global variables	*/

char ibuf[bsize], obuf[bsize];	/* input and output buffers */
int ibp = 0, obp = 0;	/* input and output buffer pointers */
int ibpend = -1;	/* pointer to end of input buffer */
int rgc;		/* hold argc */
char **rgv;		/* hold argv */
int hits[10], dist[10];	/* statistics counters */
char lline[llen];	/* input line storage */
int lpos = 0, hlpos = 0;	/* line position pointers */
int elpos = -1;		/* pointer to end of line buffer */
int tsize;		/* size of target table */
main(argc, argv)
int argc;
char **argv;
{

	rgc = argc;  rgv = argv;	/* ptrs to command line args */

	readnames();	/* read all the names */
	btable();	/* build the hash table */
	outable();	/* output the table */
	exit(0);

}


btable()	/* build the hash table */
{	register int i, j, dst;

	for (i=0; table[i].name; i++)	{	/* compute hash values */
	    if ((j = HASH(table[i].name,tsize)) < 0) j = -j;
	    table[i].hval = j;	}

	if (stats) stat_pileup();	/* static statistics */

	for (i=0; table[i].name; i++)	{	/* put into hash table */
	    dst = 0;		/* distance to insert name */
	    for (j=table[i].hval; htable[j]; j=((j+1)%tsize))
			dst++;	/* find where to insert */
	    dist[(dst < 10) ? dst : 9]++;	/* mark how far */
	    htable[j] = i + 1 /*bias*/;	}

	if (stats) stat_dist();	/* search distance statistics */

}
readnames()
{	register int i, name;

	for (i=0; (!eof && i<(NN+NM-1)); i++)	{	/* read all names */

	    if (!eat("PAIR(")) rdferr();
	    name = gquotc();		/* get quoted character */

	    if (!eat(", 0 )")) {	/* second character */
		if (!eat(",")) rdferr();
		name = PAIR(name, gquotc());	/* get it */
		if (!eat(")")) rdferr();	}

	    table[i].name = name & NAMEMASK;	/* save name */

	    if (valf < 0)
		if (eat(",")) { valf = TRUE;	/* name/value format */
				gstr(table[i].val, vallen-1);	} /* first one */
			else valf = FALSE;	/* name only format */
	      else if (valf)	{
			if (!eat(",")) rdferr();
			gstr(table[i].val, vallen-1); }	/* get val string */

	    if (!eat("\n")) rdferr();

	    getch();  lpos--;	}	/* force next line or eof */
}
eat(ptr)	/* eat input matching string pointed to by ptr */
char *ptr;
{
	skip();			/* skip leading white space */
	hlpos = lpos;		/* save line position */
	while (*ptr)	{
	    if (*ptr == ' ')		/* space in (ptr) -> skip white space */
		    { skip(); ptr++; }
		else if (*ptr++ != getch())	{
			lpos = hlpos;	/* no match - restore line pointer */
			return FALSE;	}}

	return TRUE;		/* matched */
}


skip()		/* skip white space in input */
{	register int c;

	do c = getch();
	    while ((c == ' ') || (c == '\t'));
	lpos--;
}


gquotc()
{	register int c;

	if (!eat("'")) rdferr();
	c = getch();
#ifdef ebcdic
	c = etoa[c];
#endif
	if (!eat("'")) rdferr();
	return c;
}


/* store input until space or comma into (ptr) of length len */

gstr(ptr, len)
char *ptr;
int len;
{	register int i, ch;

	skip();		/* skip leading blanks */
	i = 0;
	while (((ch = getch()) != '\n') && (ch != ' ') && (i++ < len))
			*ptr++ = ch;
	lpos--;
	if (i >= len) rdferr();
	*ptr = 0;
}


rdferr()
{	register int i;

	lline[elpos] = 0;
	wwrite(lline);		/* output line */
	for (i=0; i<lpos; i++)
	    if (lline[i] == '\t') wwrite(2, "\t", 1);
		else wwrite(" ");
	wwrite("^ format error\n");
	exit(1);
}
getch()
{
	if (lpos > elpos)
	    if (!newline())	{eof = 1; return 0;}
	return lline[lpos++];
}


newline()
{
	lpos = elpos = 0;
	do	{
		if (ibp > ibpend) getblk();	/* get a new block */
		if (ibpend < 0) return FALSE;	/* no more input */
		lline[elpos] = ibuf[ibp++];	}
	    while ((lline[elpos] != '\n') && (++elpos < llen));
	return TRUE;
}


getblk()
{	int i;
	ibp = 0;
	if (!filein && (rgc > 1)) nextf();	/* process option or size */
	for (i=0; ((i<bsize)&&((ibuf[i]=getc(filein))!=EOF)); i++) ;
	if ((ibpend=i-1) == 0)
	    if (rgc > 1)	{
		nextf();			/* next file */
		getblk();	}		/* and get a block */
}


nextf()
{	register int i;

	if (--rgc <= 0) return FALSE;

	if ((**++rgv == '-') && (rgv[0][1] == 's') && (rgv[0][2] == 0))	{
		stats++;	/* turn on statistics */
		return nextf();	} 

	if (!filein)	{		/* first non-option argument */
	    filein = (FILE *)(tsize = getnum());	/* is the table size */
	    return nextf();	}

	if ((filein = fopen(*rgv,"r")) == NULL)	{
		wwrite("Can't open ");
		wwrite(*rgv);
		wwrite("\n");
		exit(1);	}

	return TRUE;
}
getnum()
{	register int ac, i;

	ac = 0;
	for (i=0; rgv[0][i]; i++)	{
	    if ((rgv[0][i] < '0') || (rgv[0][i] > '9'))	{
		wwrite("Non-numeric size argument\n");
		exit(1);	}
	    ac = (ac * 10) + (rgv[0][i] - '0');	}

	return ac;
}
outable()	/* output the hashed name table */
{	register int i, j, k;

	for (i=0; i<tsize; i++)	{	/* output it */
	    if (!(k = htable[i]))
		    if (valf) put("\t0,0,\n");
			else put("\t0,\n");
		else	{
		    k--;
		    put("\tPAIR('");
		    putccode(table[k].name & CH);
		    put("',");
		    if (j = (table[k].name>>BYTE) & CH)	{
			putchar('\'');
			putccode(j);
			put("')");	}
		      else put("0)");
		    if (valf)	{
			putchar(',');
			for (j=0; table[k].val[j]!=0; j++)
			putchar(table[k].val[j]);	}
		    put(", /*");
#ifndef ebcdic
		    putchar(table[k].name & CH);
#else
		    putchar(atoe[table[k].name & CH]);
#endif
		    if (j = (table[k].name>>BYTE) & CH)
#ifndef ebcdic
			putchar(j);
#else
			putchar(atoe[j]);
#endif
		    put("*/\n");	}}
}


put(ptr)
char *ptr;
{
	do putchar(*ptr++);
		while (*ptr);
}


putccode(ch)
char ch;
{
	putchar('\\');
#ifndef ebcdic
	putchar(((ch/0100)&07)+'\060');
	putchar(((ch/010)&07)+'\060');
	putchar(((ch)&07)+'\060');
#else
	putchar(atoe[((ch/0100)&07)+'\060']);
	putchar(atoe[((ch/010)&07)+'\060']);
	putchar(atoe[((ch)&07)+'\060']);
#endif
}


wwrite(str)
char *str;
{	char *p;

	for (p=str; *p; p++)
		putc(*p, stderr);

}
stat_pileup()	/* give static statistics */
{	int i;
	register int j, cnt;

	for (i=0; i<tsize; i++)	{
	    cnt = 0;
	    for (j=0; table[j].name; j++)
		if (table[j].hval == i) cnt++;
	    hits[(cnt>9) ? 9 : cnt]++;	}

	for (i='0'; i<'9'; i++)	{
	    wwrite("count of ");
	    putc((char)i, stderr);
	    wwrite(" hits: ");
	    wrnum(hits[i-'0']);
	    wwrite("\n");	}

	wwrite("9 or more hits: ");
	wrnum(hits[9]);
	wwrite("\n");
}


stat_dist()	/* hash travel distance statistics */
{	char i;

	for (i='0'; i<'9'; i++)	{
	    wwrite("count of ");
	    putc(i, stderr);
	    wwrite(" hash travel distances: ");
	    wrnum(dist[i-'0']);
	    wwrite("\n");	}
	wwrite("search of 9 or more slots: ");
	wrnum(dist[9]);
	wwrite("\n");
}
wrnum(n)
int n;
{	int i;

	if (n < 0)	{
	    wwrite("-");
	    i = -n;	}
	  else i = n;
	if (i / 10) wrnum(i / 10);
	*((char *)&i) = (i % 10) + '0';
	wwrite(&i);
}
