/*	suftabmake.c  -  make suftab data file for nroff	*/


#include "suftab.c"
#include "stdio.h"
#define yes 1
#define no 0


#define deftoc(table, exists)	if (exists) {suftoc[suflet++] = filepos; \
					     filepos += sizeof(table);	} \
				    else suftoc[suflet++] = 0;

#define puttab(table)	if (!fwrite(table,sizeof(table),1,fp)) \
				error("? error writting suftab\n");


int suftoc[26];		/* table of contents for all letters */
int filepos = 26*sizeof(int);	/* current position in suftab */
int suflet = 0;		/* current letter offset */
int nothing;


main()
{	FILE *fp;		/* file descriptor */

	deftoc(sufa, yes)
	deftoc(nothing, no)	/* no b */
	deftoc(sufc, yes)
	deftoc(sufd, yes)
	deftoc(sufe, yes)
	deftoc(suff, yes)
	deftoc(sufg, yes)
	deftoc(sufh, yes)
	deftoc(sufi, yes)
	deftoc(nothing, no)	/* no j */
	deftoc(sufk, yes)
	deftoc(sufl, yes)
	deftoc(sufm, yes)
	deftoc(sufn, yes)
	deftoc(sufo, yes)
	deftoc(sufp, yes)
	deftoc(nothing, no)	/* no q */
	deftoc(sufr, yes)
	deftoc(sufs, yes)
	deftoc(suft, yes)
	deftoc(nothing, no)	/* no u */
	deftoc(nothing, no)	/* no v */
	deftoc(nothing, no)	/* no w */
	deftoc(nothing, no)	/* no x */
	deftoc(sufy, yes)
	deftoc(nothing, no)	/* no z */


/* produce output file */


#ifdef unix
	if ((fp = fopen("suftab","w")) == NULL)
#endif
#ifdef ibm
	if ((fp = fopen("suftab","w,BINARY")) == NULL)
#endif
		error("? can't create suftab\n");

	if (!fwrite(suftoc,sizeof(suftoc),1,fp))
		error("? can't write suftab\n");

	puttab(sufa);
	puttab(sufc);
	puttab(sufd);
	puttab(sufe);
	puttab(suff);
	puttab(sufg);
	puttab(sufh);
	puttab(sufi);
	puttab(sufk);
	puttab(sufl);
	puttab(sufm);
	puttab(sufn);
	puttab(sufo);
	puttab(sufp);
	puttab(sufr);
	puttab(sufs);
	puttab(suft);
	puttab(sufy);

	fclose(fp);
}

error(spt)
char *spt;
{	register char *p;
	for (p=spt; (*p && putc(*p,stderr)); p++) ;
	exit(-1);
}
