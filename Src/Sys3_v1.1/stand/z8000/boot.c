#include "sys/plexus.h"

#define AUTOBOOT	(!(in_local(P_SWIT) & 0x08))
#define UNIXADDR	0x100000
#define UNIXSEG		0x02

#ifdef PROM

#define SAADDR		0x130000
#define SASEG		0x13

#else

#define SAADDR		UNIXADDR
#define SASEG		UNIXSEG

#endif

#ifndef PROM
#define getw(fildes) (read(fildes,&readbuf,2) == 2 ? readbuf : -1)
unsigned readbuf;
#endif

char line[100];
unsigned pages;			/* Number of pages used for loaded program. */
int ignorerrors;		/* Flag to ignore error returns from io. */
int autotried;			/* Non zero if not first time through */
int dataseg, textseg;
				/*  This includes an I/O page (page 31) and */
				/*  a stack page (page 30). */

struct {
	char *fname;
	char *tname;
} defnames[] = {
	{	"?",		"mt(,)"		},
	{	"help",		"mt(,)"		},
	{	"boot",		"mt(,1)"	},
	{	"v7",		"mt(,2)"	},
	{	"sys3",		"mt(,2)"	},
	{	"dformat",	"mt(,3)"	},
	{	"mkfs",		"mt(,4)"	},
	{	"restor",	"mt(,5)"	},
	{	"icheck",	"mt(,6)"	},
	{	"fsck",		"mt(,6)"	},
	{	"dd",		"mt(,7)"	},
	{	"fbackup",	"mt(,8)"	},
	{	"od",		"mt(,9)",	},
	{	"dconfig",	"mt(,10)"	},
	{	"sash",		"mt(,11)"	},
	{	"fsdb",		"mt(,12)"	},
	{	"du",		"mt(,13)"	},
	{	"ls",		"mt(,14)"	},
	{	"cat",		"mt(,15)"	},
	{	"diag",		"mt(,19)"	},
	{	"d",		"mt(,19)"	},

};

main(argc,argv)
int argc;
char **argv;
{
register i;
register char *bootname;

#ifdef PROM
	printf("\nPLEXUS PRIMARY BOOT REV 1.0\n: ");
	bootname = line;
#else
	argc = getargv("boot",&argv,0);
	bootname = argv[1];
#endif

	do {
#ifdef PROM
		autotried++;
		if (!(AUTOBOOT && autotried<=2))
			gets(line);
		else
			printf("\r");
#endif
		if (*bootname == '!') {
			ignorerrors++;
			bootname++;
		}
		for (i=0; i<((sizeof defnames) / 4); i++)
			if (match(defnames[i].fname,bootname))
				bootname = defnames[i].tname;
#ifdef PROM
		if ((autotried==2) && AUTOBOOT)
			spin(30*60);
#endif
		i = open(bootname,0);
		if (i<0) {
			printf("?? ");
			bootname = line;
			gets(line);
		}
	} while (i < 0);


	copyunix(i);

}


copyunix(io)
register io;
{
register unsigned addr;
register int pageno;
register int physpage;
register int dpages, tpages;
long phys;
unsigned txtsiz,datsiz,bsssiz;
unsigned nextpage;
int	magic;


	lseek(io, 0L, 0);
	magic = getw(io);
	txtsiz = getw(io);
	datsiz = getw(io);
	bsssiz = getw(io);
	getw(io); getw(io); getw(io); getw(io);
	physpage = (int)(UNIXADDR>>11);
	dataseg = UNIXSEG;
	textseg = dataseg + 0x01;


	switch (magic) {
	case 0411:
		/*
		/* Split I&D file, data is loaded at lowest physical address
		 */
		tpages = ((txtsiz - 1) / PAGESIZE) + 1;
		dpages = ((datsiz + bsssiz - 1) / PAGESIZE) + 1;
		nextpage = tpages + dpages;

		for (pageno = 0; pageno < NUMLOGPAG; pageno++) {
			out_local(mmapport(dataseg,pageno),B_RO|B_IP);
			out_local(mmapport(textseg,pageno),B_RO|B_IP);
		}


		for (pageno = 0; pageno < dpages; pageno++,physpage++) {
			out_local(mmapport(dataseg,pageno), physpage);
			clrpage(dataseg,pageno);
		}

		for (pageno = 0; pageno < tpages; pageno++,physpage++) {
			out_local(mmapport(textseg,pageno), physpage|B_RO);
			clrpage(textseg,pageno);
		}

		/* Set up I/O page to be last page */
		out_local(mmapport(dataseg,31), physpage++);
		clrpage(dataseg,31);

		/* Set up mapping and clear out stack area */
		out_local(mmapport(dataseg,30), physpage++);
		clrpage(dataseg,30);

		/* Load text area */
		for(addr=0; addr<txtsiz; addr+=2) {
			memld(textseg,addr,getw(io));
		}

		/* Load data area */
		phys = datsiz;
		goto endboot;

	case 0407:
		/*
		 * Non-split file, both text and data maps point to same
		 * physical memory
		 */
		physpage = (int)((SAADDR)>>11);
		dataseg = SASEG;
		textseg = SASEG + 0x01;
		phys = txtsiz+datsiz;
		nextpage = ((phys + bsssiz - 1) / PAGESIZE) + 1;

		/* Set up mapping ram and clear pages */
		for (pageno = 0; pageno < 32; pageno++,physpage++) {
			out_local(mmapport(dataseg,pageno), physpage);
			out_local(mmapport(textseg,pageno), physpage);
			clrpage(dataseg,pageno);
		}


	endboot:
		for (addr=0; addr<phys; addr+=2) {
			memld(dataseg,addr,getw(io));
		}

		close(io);

		pages = physpage;
#ifndef PROM
		exec();
#endif
		return;

	default:
		close(io);
#ifndef PROM
		printf("Invalid object file (0%o)\n",magic);
#else
		fatalerr(E_BADOBJ,magic);
#endif
		exit(1);
	}
}

mmapport(segment,page) {
	return(mapport(segment<<8,page));
}

#ifndef PROM
static
match(s1,s2)
register char *s1,*s2;
{
	register int cc;

	cc = 14;
	while (cc--) {
		if ((*s1=='(') && !(*s2))
			return(1);
		if (*s1 != *s2)
			return(0);
		if (*s1++ && *s2++)
			continue; else
			return(1);
	}
	return(1);
}
#endif

