/* SID @(#)page.h	5.3 */
/*
 * page table 
 */
#undef LUNDELL
#if !defined(ROBIN) && !defined(SCHROEDER) && !defined(LUNDELL) && !defined(KICKER)
#define LUNDELL
#endif

#ifdef LUNDELL
#define PG_PFNUM	0xfff		/* should include PG_RAM??? */
#define PG_RAM		0x8000
#define PG_MAPID	0xff0000
#define PG_PROT		0x38000000
#define PG_NOEX		0x08000000
#define PG_NOWR		0x10000000
#define PG_NORD		0x20000000
#define PG_MOD		0x40000000
#define PG_REF		0x80000000
#define PG_INV		0x38000000
#define PG_IDSHIFT 16
#define ptomid(x)	((x) << 16)
#endif /* LUNDELL */

#ifdef SCHROEDER
#define PG_PFNUM	0xfff
#define PG_MAPID	0xff0000
#define PG_PROT		0x07000000
#define PG_NOEX		0x01000000
#define PG_NOWR		0x02000000
#define PG_NORD		0x04000000
#define PG_MOD		0x40000000
#define PG_REF		0x80000000
#define PG_INV		0x07000000
#define PG_IDSHIFT 16
#define ptomid(x)	((x) << 16)
#endif /* SCHROEDER */

#ifdef ROBIN
#define PG_PFNUM	0xfff
#define PG_RAM		0
#define PG_MAPID	0xff000000
#define PG_PROT		0xe000
#define PG_NOEX		0x2000
#define PG_NOWR		0x4000
#define PG_NORD		0x8000
#define PG_MOD		0x00010000
#define PG_REF		0x00020000
#define PG_INV		0xe000
#define PG_IDSHIFT 24
#define ptomid(x)	((x) << 24)
#endif /* ROBIN */

#ifdef KICKER 
/* various masks for page map fields */
#define PG_PFNUM	0xffff		/* physical addr */
#define PG_MAPID	0x7ff0000	/* task id */
#define PG_PROT		0x38000000	/* protect bits */
#define PG_NOEX		0x08000000	/* no execute */
#define PG_NOWR		0x10000000	/* no write */
#define PG_NORD		0x20000000	/* no read */
#define PG_MOD		0x40000000	/* modified ? */
#define PG_REF		0x80000000	/* referenced ? */
#define PG_INV		0x38000000	/* invalid */
#define PG_IDSHIFT 16
#define ptomid(x)	((x) << 16)	/* proc # to task id */
#endif /* KICKER */

#define	svtopte(X)	((int *)SMLO + btoc(X))

/*
 * address space definitions
 */
#if defined(LUNDELL) || defined(ROBIN)
#define ULMLO		0x0		/* user log mem */
#define ULMHI		0x800000	/* user log mem */
#define UMLO		0x900000	/* user (page) map mem */
#define UMHI		0x902000	/* user (page) map mem */

#define SLMLO		0x0		/* sys log mem */
#define XLMLO		0x600000	/* sys log block mover mem */
#define XLMHI		0x700000	/* sys log block mover mem */
#define SLMHI		0x780000	/* sys log mem */
#define SLBLO		0x780000	/* sys log blk dma */
#define SLBHI		0x7c0000	/* sys log blk dma */
#define SLCLO		0x7c0000	/* sys log char dma */
#define SLCHI		0x800000	/* sys log char dma */

#define SMLO		0x902000	/* sys (page) map mem */
#define SMHI		0x903e00	/* sys (page) map mem */
#define BMLO		0x903e00	/* sys (page) map blk dma */
#define BMHI		0x903f00	/* sys (page) map blk dma */
#define CMLO		0x903f00	/* sys (page) map char dma */
#define CMHI		0x904000	/* sys (page) map char dma */
#define XMLO		0x903800	/* sys (page) map blk mover */
#define XMHI		0x903c00	/* sys (page) map blk mover */

#define PLMLO		0x800000	/* prom mem */
#define PLMHI		0x810000	/* prom mem */

#define CLMLO		0x980000	/* cache log mem */
#define CLMHI		0x981000	/* cache log mem */
#define CPMLO		0x9c0000	/* cache page/dirty */
#define CPMHI		0x9c1000	/* cache page/dirty */

#define MBILO		0xb00000	/* multibus i/o low */
#define MBIHI		0xb10000	/* multibus i/o hi */
#define MBMLO		0xb80000	/* multibus mem low */
#define MBMHI		0xc00000	/* multibus mem hi */
#ifdef ROBIN
#define DLMLO		0xc00000	/* static memory lo */
#define DLMHI		0xc04000	/* static memory hi */
#ifndef BOOTSTART
#define BOOTSTART	0x808004	/* address of address of boot start */
#endif
#endif
#endif /* LUNDELL or ROBIN */

#ifdef SCHROEDER
#define ULMLO		0x0		/* user log mem */
#define ULMHI		0x800000	/* user log mem */
#define UMLO		0x900000	/* user (page) map mem */
#define UMHI		0x902000	/* user (page) map mem */

#define SLMLO		0x0		/* sys log mem */
#define SLMHI		0x780000	/* sys log mem */
#define SLCLO		0x780000	/* sys log char dma */
#define SLCHI		0x800000	/* sys log char dma */
#define SLBLO		SLCLO		/* sys log blk dma (doesn't really
					   exist just for compatibilty */

#define SMLO		0x902000	/* sys (page) map mem */
#define SMHI		0x903e00	/* sys (page) map mem */
#define CMLO		0x903e00	/* sys (page) map char dma */
#define CMHI		0x904000	/* sys (page) map char dma */
#define BMLO		CMLO		/* sys (page) map blk dma (doesn't
					   really exist on schroeder */

#define PLMLO		0x800000	/* prom mem */
#define PLMHI		0x820000	/* prom mem */

#define DLMLO		0x880000	/* static memory lo */
#define DLMHI		0x881000	/* static memory hi */

#define C0LMLO		0x980000	/* cache 0 data log mem */
#define C0LMHI		0x981000	/* cache 0 data log mem */
#define C1LMLO		0x981000	/* cache 1 data log mem */
#define C1LMHI		0x982000	/* cache 1 data log mem */
#define C0PMLO		0x990000	/* cache 0 tag log mem */
#define C0PMHI		0x991000	/* cache 0 tag log mem */
#define C1PMLO		0x991000	/* cache 1 tag log mem */
#define C1PMHI		0x992000	/* cache 1 tag log mem */

#define MBILO		0xc00000	/* multibus i/o low */
#define MBIHI		0xc10000	/* multibus i/o hi */
#define MBMLO		0xd00000	/* multibus mem low */
#define MBMHI		0xd80000	/* multibus mem hi */
#endif /* SCHROEDER */

#ifdef KICKER 

/* user address space is 16Mb, using a 4k entry, 16k byte map 
and 4k virtual pages */

#define ULMLO		0x0		/* user log mem */
#define ULMHI		0x1000000	/* user log mem */
#define UMLO		0x9000000	/* user map mem */
#define UMHI		0x9004000	/* user map mem */

/* kernel address space is 16Mb, of which top half is used for mapping dma transfers */
/* however, to make things more exciting, we support multibus I/O in
a manner compatible with existing software, so the VME I/O space
has to use funny names */
/* be warned that the top of memory, or at least the corresponding
map entries, get stolen for various obscure purposes. 
A partial list -
smalloc takes some pages around the end of mb space
io-page for multibus takes a page at the end of multibus space which is just below 8Mb like a schroeder
an io-page for vme appears at the top of vme space
io buffers are in between the kernel and the start of multibus space
u-page/system stack takes a spot just before multibus space */
/* modules which depend heavily on these definitions include start.s, bio.c,
machdep.c, and malloc.c */

#define SLMLO		0x0		/* sys log mem */
#define SLMHI		0x800000	/* sys log mem */
#define VSLCLO		0x800000	/* vme log char dma */
#define VSLCHI		0x1000000	/* vme log char dma */
#define	SLCLO		0x780000	/* multibus dma */
#define SLCHI		0x800000	/* multibus dma */
#define SLBLO		SLCLO		/* sys log blk dma (doesn't really exist just for compatibility */

/* half of system map is used to set up dma, and part  is used
for multibus */
#define SMLO		0x9004000	/* sys map mem */
#define SMHI		0x9005e00	/* sys map mem */
#define CMLO		0x9005e00	/* mb map char dma */
#define CMHI		0x9006000	/* mb map char dma */
#define BMLO		CMLO		/* sys map blk dma (doesn't
					   really exist on kicker */
#define VCMLO		0x9006000	/* vme char dma map */
#define VCMHI		0x9008000	/* vme map char dma */

/* prom has to live somewhere */
#define PLMLO		0x8000000	/* prom mem */
#define PLMHI		0x8020000	/* prom mem */

/* so does static ram */
#define DLMLO		0x8080000	/* static memory lo */
#define DLMHI		0x8081000	/* static memory hi */

/* cache stuff */
#define C0LMLO		0x9080000	/* cache 0 data log mem */
#define C0LMHI		0x9081000	/* cache 0 data log mem */
#define C1LMLO		0x9081000	/* cache 1 data log mem */
#define C1LMHI		0x9082000	/* cache 1 data log mem */
#define	C2LMLO		0x9082000	/* Cache 2 data log mem */
#define	C2LMHI		0x9083000	/* Cache 2 data log mem */
#define	C3LMLO		0x9083000	/* Cache 3 data log mem */
#define	C3LMHI		0x9084000	/* Cache 3 data log mem */

#define C0PMLO		0x9090000	/* cache 0 tag log mem */
#define C0PMHI		0x9091000	/* cache 0 tag log mem */
#define C1PMLO		0x9091000	/* cache 1 tag log mem */
#define C1PMHI		0x9092000	/* cache 1 tag log mem */
#define	C2PMLO		0x9092000	/* Cache 2 tag log mem */
#define	C2PMHI		0x9093000	/* Cache 2 tag log mem */
#define	C3PMLO		0x9093000	/* Cache 3 tag log mem */
#define	C3PMHI		0x9094000	/* Cache 3 tag log mem */

#define	C0IMLO		0x90a0000	/* Cache 0 invalid bits */
#define	C0IMHI		0x90a1000	
#define	C1IMLO		0x90a1000	/* Cache 1 invalid bits */
#define	C1IMHI		0x90a2000
#define	C2IMLO		0x90a2000	/* Cache 2 invalid bits */
#define	C2IMHI		0x90a3000
#define	C3IMLO		0x90a3000	/* Cache 3 invalid bits */
#define	C3IMHI		0x90a4000	

/* 8Mb of VME dma space is subdivided */
#define	VMEILO		0x80000000	/* VME I/O space low */
#define	VMEIHI		0xffffffff	/* VME I/O space high */
#define VME32LO		0x80000000	/* VME extended */
#define VME32HI		0xa0000000	/* VME extended */
#define VMEUDLO		0xa0000000	/* VME user defined */
#define VMEUDHI		0xc0000000	/* VME user defined */
#define VME16LO		0xc0000000	/* VME short */
#define VME16HI		0xc0010000	/* VME short */
#define VME24LO		0xe0000000	/* VME standard */
#define VME24HI		0xe1000000	/* VME standard */

#define	VMED32		0xff000000	/* VME 32-bit DATA space */

/* Define the VME bus command stuff */
#define	in_vmebus(a)	(*(unsigned char *)(VMEILO+a)) 
#define	out_vmebus(x,a)	(*(unsigned char *)(VMEILO+a) = (x))

/* multibus access is also mapped into the I/O area */
#define MBILO		0x80c00000	/* multibus i/o low */
#define MBIHI		0x80c10000	/* multibus i/o hi */
#define MBMLO		0x80d00000	/* multibus mem low */
#define MBMHI		0x80d80000	/* multibus mem hi */

#endif /* KICKER */

/*	defines for siinit 	*/
#if defined(ROBIN) || defined(SCHROEDER) || defined(LUNDELL)
#define SMLOCLO	0x903fc0	/* first page for smalloc */
#define SMLOCPG 0xf		/* 15 pages for smalloc */
#else				/* Now for KICKER stuff */
/* KICKER */
#define SMLOCPG 15		/* 15 pages for smalloc */
#define SMLOCLO	(CMHI - SMLOCPG - 1)	/* first page for smalloc */
#endif

/* kicker has a bigger map */
#ifdef KICKER
#define MAPSIZE		4096	/* # of entries in map */
#define MAPPAGES	4	/* # of pages in map */
#else
#define MAPSIZE		2048	/* # of entries in map */
#define MAPPAGES	2	/* # of pages in map */
#endif
