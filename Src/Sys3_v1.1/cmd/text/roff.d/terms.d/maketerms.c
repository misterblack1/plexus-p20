/*	maketerms.c - make terminal driving tables for nroff
 *
 *
 *	SCCS: @(#)maket.src	1.16
 *
 */


#include "terms.hd"	/* terminal table structure */
#include "stdio.h"

#ifndef unix
#define tso
#endif

#ifdef tso
#include "tab37.c"
#include "tablp.c"
#include "tabX.c"
#endif

#ifdef PART1
#include "tab300.c"	/* terminal table initializations */
#include "tab300-12.c"
#include "tab300s.c"
#include "tab300s-12.c"
#include "tab37.c"
#include "tab382.c"
#include "tab4000A.c"
#endif
#ifdef PART2
#include "tab450.c"
#include "tab450-12.c"
#include "tab832.c"
#include "taba1.c"
#include "tablp.c"
#include "tabtn300.c"
#include "tabX.c"
#endif
#ifdef PART3
#include "tab2631.c"
#include "tab2631-c.c"
#include "tab2631-e.c"
#endif


#define SSIZE 2000	/* string output buffer size */
#ifdef unix
#define mkterm(pname, tname)	file = fopen("pname","w"); \
				puttab(file, &tname); \
				fclose(file);
#endif
#ifdef tso
#define mkterm(pname, tname)	file = fopen("pname.t","w,BINARY"); \
				puttab(file, &tname); \
				fclose(file);
#endif


extern char *putstr();	/* it's down below */


struct termtable tbuf;	/* terminal structure output buffer */
char sbuf[SSIZE];	/* structure string output buffer */
int sbp;		/* string buffer pointer */
FILE *file;		/* file descriptor */


main()
{
#ifdef PART1
	mkterm(tab300, t300)
	mkterm(tab300-12, t30012)
	mkterm(tab300s, t300s)
	mkterm(tab300s-12, t300s12)
	mkterm(tab37, t37)
	mkterm(tab382, t382)
	mkterm(tab4000A, t4000A)
#endif
#ifdef PART2
	mkterm(tab450, t450)
	mkterm(tab450-12, t45012)
	mkterm(tab832, t832)
	mkterm(taba1, ta1)
	mkterm(tablp, tlp)
	mkterm(tabtn300, ttn300)
	mkterm(tabX, tX)
#endif
#ifdef PART3
	mkterm(tab2631, t2631)
	mkterm(tab2631-c, t2631c)
	mkterm(tab2631-e, t2631e)
#endif
#ifdef tso
	mkterm(tab37, t37)
	mkterm(tablp, tlp)
	mkterm(tabX, tX)
#endif
}


puttab(fc, tab)
FILE *fc;
struct termtable *tab;
{	int i;

	sbp = 0;		/* reset string pointer */

	tbuf.bset = tab->bset;	/* copy values to output structure */
	tbuf.breset = tab->breset;
	tbuf.Hor = tab->Hor;
	tbuf.Vert = tab->Vert;
	tbuf.Newline = tab->Newline;
	tbuf.Char = tab->Char;
	tbuf.Em = tab->Em;
	tbuf.Halfline = tab->Halfline;
	tbuf.Adj = tab->Adj;

	tbuf.twinit = putstr(tab->twinit,0);	/* copy strings and pointers */
	tbuf.twrest = putstr(tab->twrest,0);
	tbuf.twnl = putstr(tab->twnl,0);
	tbuf.hlr = putstr(tab->hlr,0);
	tbuf.hlf = putstr(tab->hlf,0);
	tbuf.flr = putstr(tab->flr,0);
	tbuf.bdon = putstr(tab->bdon,0);
	tbuf.bdoff = putstr(tab->bdoff,0);
	tbuf.iton = putstr(tab->iton,0);
	tbuf.itoff = putstr(tab->itoff,0);
	tbuf.ploton = putstr(tab->ploton,0);
	tbuf.plotoff = putstr(tab->plotoff,0);
	tbuf.up = putstr(tab->up,0);
	tbuf.down = putstr(tab->down,0);
	tbuf.right = putstr(tab->right,0);
	tbuf.left = putstr(tab->left,0);

	for (i=0; i<(256-32); i++)	/* do the same for codetab */

		tbuf.codetab[i] = putstr(tab->codetab[i],2);

	tbuf.zzz = tab->zzz;	/* ??? */

	fwrite((char *)&sbp,sizeof(sbp),1,fc);
	fwrite((char *)&tbuf,sizeof(tbuf),1,fc);
	fwrite((char *)sbuf,sbp,1,fc);
}



char *putstr(pt, cnt)
char *pt;
int cnt;
{	char *p;
	register int i, j, k;

	if (pt == (char *)0) return (0);	/* null pointer */
	for (i=0;  i<sbp;  i++)	{	/* check for string in buffer */

	    for (p=pt,k=cnt,j=i;
		  ((j<sbp) && (sbuf[j]==*p) && ((k-->0) || *p));
		    j++,p++)  ;

	    if ((j<sbp) && !sbuf[j] && !*p)
		return ((char *)i);	}

	/* not in table now - add it */

	i = sbp;

	for (k=cnt,p=pt;  ((sbp<SSIZE) && ((k-->0) || *p)); p++)

		sbuf[sbp++] = *p;	/* put string */

	sbuf[sbp++] = 0;		/* terminate string */

	if (sbp >= SSIZE)	{

		fputs("? string area overflow\n",23);
		exit(-1);	}

	return ((char *)i);
}
