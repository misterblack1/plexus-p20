#
/*
 * lpd -- line printer daemon dispatcher for versatek printer
 *
 */

#define	SPIDER	0
#define	PHONE	0
#define	LPD	1

#ifndef	SPDIR
#define	SPDIR	"/usr/spool/lpd"
#endif
#ifndef	LOCK
#define	LOCK	"/usr/spool/lpd/lock"
#endif
#ifndef	PRT
#define	PRT	"/usr/lib/lpd.pr"
#endif

char	dpd[]	= SPDIR;
char	dfname[30] = SPDIR;
char	lock[]	= LOCK;

#include	"daemon.c"

/*
 * The remaining part is the line printer interface.
 */

#define	PLOT	"/usr/lib/vplot"
#define	VCAT	"/usr/lbin/vcat"
#define	FONTDIR	"/usr/lib/vfont/"
#define	FONTS	"RIBS"

char	banbuf[64];
char	eline[MXLINE+2];
char	fonts[4][50] = {
	"/usr/lib/vfont/R",
	"/usr/lib/vfont/I",
	"/usr/lib/vfont/B",
	"/usr/lib/vfont/S"
};


dem_con()
{
	return(0);
}

dem_dis()
{

}

dem_open(file)
char	*file;
{
	int i, j;

	banbuf[0] = 0;
	eline[0] = 0;
	for(i = 0; i < 4; i++){
		for(j = 0; j < 49; j++)
			if((fonts[i][j] = FONTDIR[j]) == 0)
				break;
		fonts[i][j++] = FONTS[i];
		fonts[i][j] = 0;
	}
}

dem_close()
{
}

get_snumb()
{
}

lwrite()
{
	strncpy(banbuf, line+1, sizeof(banbuf)-1);
}


sascii(fc)
{
	static char obuf[100];
	int i;
	int rm;
	int p;
	FILE *cfb;
	char *filter;

	if((cfb = fopen(&line[1], "r")) == NULL){
		sprintf(obuf, "Can't open %s", &line[1]);
		if(mesp+strlen(obuf) <= message+MXMESS)
			mesp = copline(obuf, 400, mesp);
		return(0);
	}
	switch(fc){

	case 'F':
		if(((char)getc(cfb) != '\100') || ((char)getc(cfb) != '\357')){
			filter = PRT;
			break;
		}
		fc = 'T';
	case 'T':
		filter = VCAT;
		break;

	case 'P':
		filter = PLOT;
		break;
	}
	fclose(cfb);
	if(access(filter, 01)){
		sprintf(obuf, "ERROR from %s: can't find %s.", DAEMNAM, filter);
		if((pmail = popen("mail root", "w")) != NULL){
			fprintf(pmail, obuf);
			pclose(pmail);
		}
		if(mesp + strlen(obuf) <= message + MXMESS)
			mesp = copline(obuf, 400, mesp);
		return(0);
	}
	if (p = fork()) {
		if (p == -1)
			return(1);
		wait(&p);
		return(p);
	}
	switch(fc){

	case 'F':
	case 'P':
		strncat(eline, line+1, sizeof(eline)-3);
		if (banbuf[0]) {
			execl(filter, filter, "-b", banbuf, eline, 0);
			break;
		}
		execl(filter, filter, eline, 0);
		break;

	case 'T':
		unlink(".railmag");
		rm = creat(".railmag", 0666);
		for (i = 0; i < 4; i++) {
			if (fonts[i][0] != '/')
				write(rm, FONTDIR, strlen(FONTDIR));
			write(rm, fonts[i], strlen(fonts[i]));
			write(rm, "\n", 1);
		}
		close(rm);
		if (banbuf[0]) {
			execl("/bin/sh", filter, filter, "-b", banbuf, line+1, 0);
			break;
		}
		execl("/bin/sh", filter, filter, line+1, 0);
		break;
	}
	exit(1);
}


etcp1(fc)
char	fc;
{
	switch(fc){

	case 'X':
		strcpy(eline, "-e");
		break;

	case '1':
	case '2':
	case '3':
	case '4':
		strcpy(fonts[line[0]-'1'], line+1);
		break;
	}
}

/* VARARGS */
trouble(s, a1, a2, a3, a4)
char	*s;
{
	if(retcode > 0){
		FCLOSE(dfb);
	}
	longjmp(env, 1);
}

/* VARARGS */
logerr()
{
}

getowner()
{
}

maildname()
{
	fprintf(pmail, "Your %s job is finished for file %s%s",
		DAEMNAM, mailfname, message);
}

