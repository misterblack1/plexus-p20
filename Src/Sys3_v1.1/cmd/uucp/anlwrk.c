	/*  anlwrk 3.5  10/26/79  11:20:30  */
#include "uucp.h"
#include <sys/types.h>
#include <sys/stat.h>
#include "uust.h"



#define LLEN 10
#define MAXRQST 250
int Nfiles = 0;
char Filent[LLEN][NAMESIZE];


/*******
 *	anlwrk(file, wvec)	create a vector of command arguments
 *	char *file, **wvec;
 *
 *	return codes:
 *		0  -  no more work in this file
 *		positive number  -  number of arguments
 */

anlwrk(file, wvec)
char *file, **wvec;
{
	static char str[MAXRQST];
	static FILE *fp = NULL;

	if (file[0] == '\0')
		return(0);
	if (fp == NULL) {
		fp = fopen(file, "r");
		if (fp == NULL)
			return(0);
		Usrf = 0;	/* for UUSTAT stuff */
	}

	if (fgets(str, MAXRQST, fp) == NULL) {
		fclose(fp);
		unlink(file);
		USRF(USR_COMP);
		US_RRS(file, Usrf);
		Usrf = 0;
		file[0] = '\0';
		fp = NULL;
		return(0);
	}

	return(getargs(str, wvec));
}


/***
 *	iswrk(file, reqst, dir, pre)
 *	char *file, *reqst, *dir, *pre;
 *
 *	iswrk  -  this routine will check the work list (list).
 *	If it is empty or the present work is exhausted, it
 *	will call gtwrk to generate a new list.
 *	The "reqst" field will be the string "chk" or "get" to
 *	check for work, or get the next work file respectively.
 *
 *	return codes:
 *		0  -  no more work (or some error)
 *		1  -  there is work
 */

iswrk(file, reqst, dir, pre)
char *file, *reqst, *dir, *pre;
{

	if (Nfiles == 0)
		bldflst(dir, pre);
	if (Nfiles == 0)
		return(0);

	if (*reqst == 'g')
		gtwrkf(dir, file);
	else
		Nfiles = 0;
	return(1);
}

/***
 *	bldflst - build list of work files for given system
 *	 Nfiles, Filent are global
 *
 *	return value - none
 */

bldflst(dir, pre)
char *dir, *pre;
{
	FILE *pdir;
	char filename[NAMESIZE];
	struct stat s;

	Nfiles = 0;
	if ((pdir = fopen(dir, "r")) == NULL)
		return;
	while (gnamef(pdir, filename)) {
		if (!prefix(pre, filename))
			continue;
		if (stat(filename, &s) == -1)
			continue;
		if ((s.st_mode & ANYREAD) == 0)
			continue;
		entflst(filename);
	}

	fclose(pdir);
	return;
}

/***
 *	entflst - put new name if list is not full
 *		  or new name is less than the MAX
 *		  now in the list.
 *	Nfiles, Filent[] are modified.
 *	return value - none
 */

entflst(file)
char *file;
{
	char *p;
	int i;

	if (Nfiles < LLEN) {
		strcpy(Filent[Nfiles++], file);
		return;
	}

	/* find MAX in table  */
	p = Filent[0];
	for (i = 1; i < Nfiles; i++)
		if (strcmp(Filent[i], p) > 0)
			p = Filent[i];

	if (strcmp(p, file) > 0)
		strcpy(p, file);

	return;
}

/***
 *	gtwrkf - get next work file
 *	 Nfiles, Filent[] are modified.
 *
 *	return value - none
 */

gtwrkf(dir, file)
char *file, *dir;
{
	char *p;
	int i;

	p = Filent[0];
	for (i = 1; i < Nfiles; i++) 
		if (strcmp(Filent[i], p) < 0)
			p = Filent[i];
	sprintf(file, "%s/%s", dir, p);
	strcpy(p, Filent[--Nfiles]);
	return;
}

/***
 *	gtwvec(file, dir, wkpre, wrkvec)	get work vector 
 *	char *file, *dir, *wkpre, **wrkvec;
 *
 *	return codes:
 *		positive number  -  number of arguments
 *		0 -  no arguments - fail
 */

gtwvec(file, dir, wkpre, wrkvec)
char *file, *dir, *wkpre, **wrkvec;
{
	int nargs;

	while ((nargs = anlwrk(file, wrkvec)) == 0) {
		if (!iswrk(file, "get", dir, wkpre))
			return(0);
	}
	return(nargs);
}
