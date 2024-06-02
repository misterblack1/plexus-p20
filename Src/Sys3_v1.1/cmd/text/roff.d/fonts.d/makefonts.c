/*	makefonts.c - @(#)makefonts.c	1.8	*/

#include "stdio.h"

#ifndef unix
#define tso
#endif

#include "ftB.c"
#include "ftBC.c"
#include "ftC.c"
#include "ftCE.c"
#include "ftCI.c"
#include "ftCK.c"
#include "ftCS.c"
#include "ftCW.c"
#include "ftG.c"
#include "ftGI.c"
#include "ftGM.c"
#include "ftGR.c"
#include "ftI.c"
#include "ftL.c"
#include "ftLI.c"
#include "ftPA.c"
#include "ftPB.c"
#include "ftPI.c"
#include "ftR.c"
#include "ftS.c"
#include "ftSB.c"
#include "ftSI.c"
#include "ftSM.c"
#include "ftUD.c"
#include "ftXM.c"

FILE *i;
#ifdef unix
#define mkfont(name)	i = fopen("name","w"); \
				fwrite(name,sizeof(name),1,i); \
				fclose(i);
#endif
#ifdef tso
#define mkfont(name)	i = fopen("name.f","w,BINARY"); \
				fwrite(name,sizeof(name),1,i); \
				fclose(i);
#endif

main()
{	mkfont(ftB)
	mkfont(ftBC)
	mkfont(ftC)
	mkfont(ftCE)
	mkfont(ftCI)
	mkfont(ftCK)
	mkfont(ftCS)
	mkfont(ftCW)
	mkfont(ftG)
	mkfont(ftGI)
	mkfont(ftGM)
	mkfont(ftGR)
	mkfont(ftI)
	mkfont(ftL)
	mkfont(ftLI)
	mkfont(ftPA)
	mkfont(ftPB)
	mkfont(ftPI)
	mkfont(ftR)
	mkfont(ftS)
	mkfont(ftSB)
	mkfont(ftSI)
	mkfont(ftSM)
	mkfont(ftUD)
	mkfont(ftXM)
}
