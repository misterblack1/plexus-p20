#define getw(fildes) (read(fildes,&readbuf,2) == 2 ? readbuf : -1)
extern unsigned segno,contname,mysp[2];
unsigned readbuf;
#define SYMSEG (16 << 8)
#ifndef OVKRNL
#include <sys/plexus.h>
#else
#include	"/usr/include/sys/plexus.h"
#endif

#define AUTOBOOT	(!(in_local(P_SWIT) & 0x08))
#define UNIXADDR	0x100000
#define UNIXSEG		0x0200

#ifdef PROM

#define SAADDR		0x130000
#define SASEG		0x1300

#else

#define SAADDR		UNIXADDR
#define SASEG		UNIXSEG

#endif



char line[100];
unsigned pages;			/* Number of pages used for loaded program. */
int autotried;			/* Non zero if not first time through */
#ifdef PROM
int p25;			/* TRUE if IMSC present */
#endif
int dataseg, textseg;
#ifndef PROM
int symseg;
#endif
				/*  This includes an I/O page (page 31) and */
				/*  a stack page (page 30). */
#ifdef	OVKRNL
struct	{
	unsigned	osize;
	unsigned	oseg;
} ov[5] = { 0, 0, 0, SDSEG, 0, SDSEG1, 0, SDSEG2, 0, SDSEG3 };
#endif

struct {
	char *fname;
	char *tname;
} defnames[] = {
	{	"help",		"rm(0,0)"	},
	{	"boot",		"rm(0,1)"	},
	{	"v7",		"rm(0,2)"	},
	{	"sys3",		"rm(0,2)"	},
	{	"dformat",	"rm(0,3)"	},
	{	"mkfs",		"rm(0,4)"	},
	{	"restor",	"rm(0,5)"	},
	{	"icheck",	"rm(0,6)"	},
	{	"dd",		"rm(0,7)"	},
	{	"fbackup",	"rm(0,8)"	},
	{	"od",		"rm(0,9)",	},
	{	"dconfig",	"rm(0,10)",	},
	{	"diag",		"rm(0,19)"	},
	{	"?",		"rm(0,0)"	},
	{	"d",		"rm(0,19)"	},
};

main()
{
register i;
register char *bootname;

#ifdef PROM
	printf("\nBoot --\n");
#else
	printf("\nSecondary boot --\n");
printf("controller name=0x%x	",contname);
printf("mysp = 0x%x 0x%x\n",mysp[0], mysp[1]);
#endif

	bootname = line;
#ifdef PROM
	p25 = imsc();
#endif

	do {
		autotried++;
#ifndef PROM
		autotried+=2;
#endif
		printf(": ");
		if (!(AUTOBOOT && autotried<=2))
			gets(line);
		else
			printf("\r");
printf("just read %s\n",line);
		for (i=0; i<((sizeof defnames) / 4); i++)
			if (!strcmp(defnames[i].fname,bootname)) {
				bootname = defnames[i].tname;
				}
#ifdef PROM
		if ((autotried==2) && AUTOBOOT)
			spin(30*60);
#endif
		i = open(bootname,0);
printf("open of %s returned %u\n",bootname,i);
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
#ifndef PROM
unsigned symsiz;
unsigned debflag;
unsigned spages;
#endif
int	magic;
#ifdef OVKRNL
unsigned	otpages, ovseg, otxtsiz;
int	i;
#endif


	lseek(io, 0L, 0);
	magic = getw(io);
	txtsiz = getw(io);
	datsiz = getw(io);
	bsssiz = getw(io);
#ifndef PROM
	symsiz = getw(io);
	getw(io);
	debflag = getw(io);
#else
	getw(io);
	getw(io);
	getw(io);
#endif
	getw(io);
	physpage = (int)(UNIXADDR>>11);
	dataseg = UNIXSEG;
	textseg = dataseg + 0x0100;
#ifndef PROM
	symseg = SYMSEG;
#endif


printf("pages=0x%x dataseg=0x%x segno=0x%x\n",pages,dataseg,segno);
	switch (magic) {
#ifdef	OVKRNL
	case 0431:
		for ( i=0; i<5; i++ ) {
			if ( (ov[i].osize = getw(io)) && i )
				ov[0].oseg++;
#ifdef	MONTE
			printf( "overlay %x, size %x, cnum %x\n",
				i, ov[i].osize, ov[0].oseg );
#endif
		}
		getw(io); getw(io); getw(io);
#endif
	case 0411:
		/*
		/* Split I&D file, data is loaded at lowest physical address
		 */
		tpages = ((txtsiz - 1) / PAGESIZE) + 1;
		dpages = ((datsiz + bsssiz - 1) / PAGESIZE) + 1;
#ifndef PROM
		spages = symsiz? ((symsiz-1)/PAGESIZE)+1 : 0;
#endif

		for (pageno = 0; pageno < NUMLOGPAG; pageno++) {
			out_local(mapport(dataseg,pageno),B_RO|B_IP);
			out_local(mapport(textseg,pageno),B_RO|B_IP);
#ifndef PROM
			out_local(mapport( symseg,pageno),B_RO|B_IP);
#endif
#ifdef	OVKRNL
			if ( ov[0].oseg ) {
				for ( i = 1; i < 5; i++ ) {
					if ( ov[i].osize ) {
						out_local(mapport(ov[i].oseg,
							pageno),B_RO|B_IP);
						out_local(mapport((ov[i].oseg+
							0x0100),pageno),
							B_RO|B_IP);
					}
				}
			}
#endif
		}


		for (pageno = 0; pageno < 30; pageno++,physpage++) {
			out_local(mapport(dataseg,pageno), physpage);
			clrpage(dataseg,pageno);
#ifdef	OVKRNL
			if ( ov[0].oseg ) {
				for ( i = 1; i < 5; i++ ) {
					if ( ov[i].osize ) {
						ovseg = ov[i].oseg;
						out_local(mapport(ovseg,
								    pageno),
								physpage);
						clrpage(ovseg, pageno );
					}
				}
			}
#endif
		}

		for (pageno = 0; pageno < tpages; pageno++,physpage++) {
			out_local(mapport(textseg,pageno), physpage);
			clrpage(textseg,pageno);
#ifdef	OVKRNL
			if ( ov[0].oseg ) {
				for ( i = 1; i < 5; i++ ) {
					if ( ov[i].osize ) {
						ovseg = ov[i].oseg + 0x0100;
						out_local(mapport(ovseg,
								     pageno),
							         physpage|B_RO);
						clrpage(ovseg, pageno);
					}
				}
			}
#endif
		}

		/* Set up I/O page to be last page */
#ifndef	OVKRNL
		out_local(mapport(dataseg,31), physpage++);
#else
		out_local(mapport(dataseg,31), physpage);
#endif
		clrpage(dataseg,31);
#ifdef	OVKRNL
		if ( ov[0].oseg ) {
			for ( i = 1; i < 5; i++ ) {
				ovseg = ov[i].oseg;
				out_local(mapport(ovseg,31),physpage);
			}
		}
		physpage++;
#endif

		/* Set up mapping and clear out stack area */
#ifndef	OVKRNL
		out_local(mapport(dataseg,30), physpage++);
#else
		out_local(mapport(dataseg,30), physpage);
#endif
		clrpage(dataseg,30);
#ifdef	OVKRNL
		if ( ov[0].oseg ) {
			for ( i = 1; i < 5; i++ ) {
				ovseg = ov[i].oseg;
				out_local(mapport(ovseg,30),physpage);
			}
		}
		physpage++;
#endif

		/* Load text area */
		for(addr=0; addr<txtsiz; addr+=2) {
			xmemld(getw(io),textseg,addr);
		}

#ifdef	OVKRNL
		/* Load overlays if magic == 431 */
		if ( (magic == 0431) && ov[0].oseg ) {
			for ( i = 1; i < 5; i++ ) {
				if ( ov[i].osize == 0 )
					break;
				printf( "loading overlay %x ", i );
				printf( "size = %x ", ov[i].osize );
				ovseg = ov[i].oseg + 0x0100;
				otpages = ((ov[i].osize - 1)/PAGESIZE)+1;
				printf( "starts at %x, ends at %x\n",
						txtsiz, (txtsiz+ov[i].osize) );
				for (pageno=tpages; pageno<(otpages+tpages); 
							pageno++, physpage++) {
					out_local(mapport(ovseg, pageno),
							physpage|B_RO);
					clrpage(ovseg,pageno);
				}
				otxtsiz = ov[i].osize;
				for (addr=txtsiz;addr<(txtsiz+otxtsiz); 
							addr += 2 ) {
					xmemld(getw(io),ovseg,addr);
				}
			}
		}
#endif
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
		textseg = SASEG + 0x0100;
		phys = txtsiz+datsiz;

		/* Set up mapping ram and clear pages */
		for (pageno = 0; pageno < 32; pageno++,physpage++) {
			out_local(mapport(dataseg,pageno), physpage);
			out_local(mapport(textseg,pageno), physpage);
			clrpage(dataseg,pageno);
		}


	endboot:
		for (addr=0; addr<phys; addr+=2) {
			xmemld(getw(io),dataseg,addr);
		}

#ifndef PROM
		if ((magic==0411) && spages && debflag) {
			for (pageno=0;pageno<spages;pageno++,physpage++){
				out_local(mapport(symseg,pageno),physpage|B_RO);
				clrpage(symseg,pageno);
				}
			lseek(io,(long)(020L+txtsiz+datsiz),0);
			for (addr=0;addr<symsiz;addr += 2) {
				xmemld(getw(io),symseg,addr);
				}
			}
#endif

		close(io);

		pages = physpage;
printf("pages=0x%x dataseg=0x%x segno=0x%x\n",pages,dataseg,segno);
#ifdef	MONTE
		printf( "pages = %x\n", pages );
#endif
#ifndef PROM
		exec();
#endif
		return;

	default:
		close(io);
		printf("Invalid object file (0%o)\n",magic);
		exit(1);
	}
}

