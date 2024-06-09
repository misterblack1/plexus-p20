#ifdef STOCKIII
#else
#include <sys/plexus.h>
#endif
#ifdef	vax
#include	"stdio.h"
#include	"sys/var.h"
#include	"a.out.h"

/* Unibus vector table */

#define	D0	0x00000000
#define	D1	0x08000000
#define	D2	0x10000000
#define	D3	0x18000000
#define	D4	0x20000000
#define	D5	0x28000000
#define	D6	0x30000000
#define	D7	0x38000000

#define	HDR	sizeof (struct exec)
#define	INTSZ	sizeof (int)
#define	HIGHBIT	0x80000000
#define	SYM_VALUE(ptr)	((unsigned)ptr->n_value & ~HIGHBIT)
#define	UBA_DEV		((int)(umemvad->n_value) - 0160000)
#define	MAPSZ	4
#define	MAXI	120
#define	IFC	01000
#define	BUS	0340
#define	CC	0037
#define	BLOCK	010

struct	var	v;
char	*os = "/unix";
char	*mr = "/etc/master";
char	line[256], nm[20], pre[20], intrnm[20], devaddr[20], devcnt[20];
char	strat[20];
int	x, address, vsz, unit, bus, deft, count, typ, bcnt,
	root, swap, dump, nsp, pwr, pipe;
long	spl;
int	*vec;
int	bndry();
int	offset;
FILE	*system, *mast, *sysseek;

struct nlist	nl[MAXI], *nlptr, *setup(), *endnm,
	*bdevcnt, *bdevsw, *rootdev, *swapdev, *dumpdev, *swplo, *nswap,
	*pipedev, *power, *UNIvec, *umemvad;

struct	link {
	char	l_cfnm[8];	/* config name from master table */
	struct	nlist	*l_nptr;	/* ptr to name list structure */
	int	l_vsz;		/* vector size */
	int	l_def;		/* default unit count */
	int	l_dtype;	/* device type - BLOCK */
	int	l_bus;		/* bus request from master table */
} ln[MAXI/3], *lnptr, *setln(), *endln, *search(), *majsrch();

struct	vector {
	int	vaxvec;
} vector;


struct	bdev {
	int	b_open;
	int	b_close;
	int	b_strategy;
	int	b_tab;
} bent;

main(argc, argv)
	int	argc;
	char	**argv;
{

	struct	nlist	*vs, *smap, *cmap, *callo;

	switch(argc) {
	case 3:	mr = argv[2];
	case 2: os = argv[1];
	case 1: break;
	default:
		fprintf(stderr, "usage: sysdef  [ /unix [ /etc/master ] ]");
		exit(1);
	}
	offset = bndry();

	if((system = fopen(os,"r")) == NULL) {
		fprintf(stderr,"cannot read %s\n",os);
		exit(1);
	}
	if((sysseek = fopen(os,"r")) == NULL) {
		fprintf(stderr,"cannot read %s\n",os);
		exit(1);
	}
	if((mast = fopen(mr, "r")) == NULL) {
		fprintf(stderr, "cannot read %s\n", mr);
		exit(1);
	}

	nlptr = nl;
	lnptr = ln;
	UNIvec = setup("_UNIvec");
	umemvad = setup("_umemvad");
	bdevcnt = setup("_bdevcnt");
	bdevsw = setup("_bdevsw");
	rootdev = setup("_rootdev");
	pipedev = setup("_pipedev");
	swapdev = setup("_swapdev");
	swplo = setup("_swplo");
	nswap = setup("_nswap");
	dumpdev = setup("_dumpdev");
	power = setup("_pwr_clr");
	pre[0] = '_';

	while(fgets(line, 256, mast) != NULL) {
		if(line[0] == '*')
			continue;
		if(line[0] == '$')
			break;
		if(sscanf(line, "%s %d %d %d %s %d %o %d %d %d ",
			nm, &vsz, &x, &typ, &pre[1], &x, &x, &x, &deft, &bus, &x)
			!= 10)
			continue;
		if(!strcmp(nm, "memory") || !strcmp(nm, "tty"))
			continue;
		strcpy(intrnm, pre);
		strcpy(devaddr, pre);
		strcpy(devcnt, pre);
		strcat(devaddr, "_addr");	/* _pre_addr */
		strcat(devcnt, "_cnt");		/* _pre_cnt */
		switch(vsz) {
			case 8: strcat(intrnm, "rint"); break;
			case 4: strcat(intrnm, "intr"); break;
			default: continue;
		}
		setln(nm, setup(intrnm), vsz, deft, typ & BLOCK, bus);
		setup(devaddr);
		setup(devcnt);
		if(typ & BLOCK) {
			strcpy(strat, pre);
			strcat(strat, "strategy");
			setup(strat);
		}
	}
	endnm = setup("");
	endln = lnptr;
	nlist(os, nl);

	if(UNIvec->n_type == 0) {
		fprintf(stderr, "%s %s\n",
			"symbol \"UNIvec\" undefined; ",
			"invalid /unix file");
		exit(1);
	}
	if(fseek(system, (long)SYM_VALUE(UNIvec) - offset, 0) != NULL) {
		fprintf(stderr, "%s: seek failed on %s\n",argv[0],system);
		exit(1);
	}

	printf("*\n* dev\tvector\taddr\tbus\tcount\n*\n");
	for(vec=0; vec < (int *)(INTSZ * 128); ++vec) {
		fread(&vector, sizeof vector, 1, system);
		if(vector.vaxvec == NULL) continue;
		if((lnptr = search(vector.vaxvec)) == NULL) {
			fprintf(stderr,
				"unknown device interrupts at vector %3o\n",
				vec);
			continue;
		}
		printf("%s\t%3o", lnptr->l_cfnm, vec);
		unit = (vector.vaxvec & D7) >> 27;
		if(fseek(sysseek,
			(long)(((lnptr->l_nptr[1]).n_value & ~HIGHBIT)
			- ftell(sysseek) + (INTSZ * unit) - offset), 1) != NULL) {
			fprintf(stderr,"bad seek for device address\n");
		}
		if(fread(&address, sizeof(address), 1, sysseek) != 1) {
			fprintf(stderr,"cannot read dev_addr\n");
		}
		fseek(sysseek,
			(long)(lnptr->l_nptr[2].n_value & ~HIGHBIT)
			- ftell(sysseek) - offset, 1);
		fread(&count, INTSZ, 1, sysseek);
		printf("\t%lo\t%1o",(address - UBA_DEV), lnptr->l_bus);
		if((unit + 1) * lnptr->l_def > count)
			printf("\t%d", count % lnptr->l_def);
		else
			printf("\t%d", lnptr->l_def);
		printf("\n");

		if(lnptr->l_vsz == 8) {
			vec++;
			fread(&vector, sizeof vector, 1, system);
		}
	}

	/* rootdev, swapdev, pipedev, dumpdev, power */

	fseek(system, (long) SYM_VALUE(bdevcnt) - offset, 0);
	fread(&bcnt, INTSZ, 1, system);
	fseek(system, (long) SYM_VALUE(rootdev) - offset, 0);
	fread(&root, INTSZ, 1, system);
	fseek(system, (long) SYM_VALUE(pipedev) - offset, 0);
	fread(&pipe, INTSZ, 1, system);
	fseek(system, (long) SYM_VALUE(swapdev) - offset, 0);
	fread(&swap, INTSZ, 1, system);
	fseek(system, (long) SYM_VALUE(dumpdev) - offset, 0);
	fread(&dump, INTSZ, 1, system);

	printf("*\n* System Devices\n*\n");

		printf("root\t%s\t%o\n", "rp06", root & 0377);
		printf("swap\t%s\t%o\t", "rp06", swap & 0377);
		fseek(system, (long) SYM_VALUE(swplo) - offset, 0);
		fread(&spl, 4, 1, system);
		fseek(system, (long) SYM_VALUE(nswap) - offset, 0);
		fread(&nsp, INTSZ, 1, system);
		printf("%ld\t%d\n", spl, nsp);
		printf("pipe\t%s\t%o\n", "rp06", pipe & 0377);
		printf("dump\t%s\t%o\n", "te16", dump & 0377);

	/* easy stuff */

	nlptr = nl;
	vs = setup("_v");
	smap = setup("_swapmap");
	callo = setup("_callout");
	endnm = setup("");
	nlist(os, nl);
	for(nlptr = vs; nlptr != endnm; nlptr++) {
		if(nlptr->n_type == 0) {
			fprintf(stderr, "namelist error\n");
			exit(1);
		}
	}
	fseek(system, (long) SYM_VALUE(vs) - offset, 0);
	fread(&v, sizeof v, 1, system);
	printf("*\n* Tunable Parameters\n*\n");
	printf("buffers\t%d\n", v.v_buf);
	printf("calls\t%d\n", v.v_call);
	printf("inodes\t%d\n", v.v_inode);
	printf("files\t%d\n", v.v_file);
	printf("mounts\t%d\n", v.v_mount);
	printf("procs\t%d\n", v.v_proc);
	printf("texts\t%d\n", v.v_text);
	printf("clists\t%d\n", v.v_clist);
	printf("sabufs\t%d\n", v.v_sabuf);
	printf("maxproc\t%d\n", v.v_maxup);
	printf("power\t%d\n", pwr ? 1 : 0);
	exit(0);
}

/*
 * setup - add an entry to a namelist structure array
 */
struct	nlist	*
setup(nam)
	char	*nam;
{
	if(nlptr >= &nl[MAXI]) {
		fprintf(stderr, "internal name list overflow\n");
		exit(1);
	}
	strcpy8(nlptr->n_name, nam);
	nlptr->n_type = 0;
	nlptr->n_value = 0;
	return(nlptr++);
}

/*
 * strcpy8 - copy exactly 8 characters with null padding
 */
strcpy8(to, from)
	register  char  *to, *from;
{
	register  int  i;

	for(i=0; i<8; i++) {
		if((*to++ = *from++) == '\0') {
			while(++i < 8)
				*to++ = '\0';
		}
	}
}

/*
 * setln - set up internal link structure for later
 * function look-up.  Records useful information from the
 * /etc/master table description.
 */
struct	link	*
setln(cf, np, vz, dft, dt, br)
	char	*cf;
	struct	nlist	*np;
	int	vz;
	int	dft;
	int	dt;
	int	br;
{
	if(lnptr >= &ln[MAXI/3]) {
		fprintf(stderr, "internal link array overflow\n");
		exit(1);
	}
	strcpy(lnptr->l_cfnm, cf);
	lnptr->l_nptr = np;
	lnptr->l_vsz = vz;
	lnptr->l_def = dft;
	lnptr->l_dtype = dt;
	lnptr->l_bus = br;
	return(lnptr++);
}

/*
 * search - search for a function indirectly through
 * the link structure.
 */
struct	link	*
search(vctr)
	int	vctr;
{
	struct	link	*lnkptr;

	for(lnkptr = ln; lnkptr != endln; lnkptr++) {
		if((lnkptr->l_nptr)->n_value == (vctr & ~D7)) {
			return(lnkptr);
		}
	}
	return(NULL);
}

/*
 * majsrch - search for a link structure given the major
 * device number of the device in question.
 */
struct	link *
majsrch(maj)
	int	maj;
{
	struct	link	*lnkptr;

	if(maj > bcnt - 1)
		return(NULL);
	fseek(system, (long) SYM_VALUE(bdevsw) + (maj * sizeof bent) - offset, 0);
	fread(&bent, sizeof bent, 1, system);
	for(lnkptr = ln; lnkptr != endln; lnkptr++) {
		if(lnkptr->l_dtype) {
			if(lnkptr->l_nptr[3].n_value == bent.b_strategy) {
				return(lnkptr);
			}
		}
	}
	return(NULL);
}
/*
 *	The function bndry() calculates the roundup necessary
 *	  to calculate the offset for an actual address in an
 *	  a.out file such as /unix.
*/
bndry()
{
	struct exec exec;

	FILE	*fopen(), *stream;

	if((stream=fopen(os,"r")) == NULL) {
		fprintf(stderr,"cannot open %s\n",os);
		exit(1);
	}
	if(fread(&exec, sizeof(struct exec), 1, stream) == NULL) {
		fprintf(stderr,"cannot read a.out hdr\n");
		exit(1);
	}
	fclose(stream);

	return(BUFSIZ - (exec.a_text % BUFSIZ) - 32);
}
#endif
#ifdef	pdp11
#include	"stdio.h"
#include	"sys/var.h"

#define	HDR	020
#define	MAPSZ	4
#define	MAXI	120
#define	IFC	01000
#define	BUS	0340
#define	CC	0037
#define	BLOCK	010

struct	var	v;
#ifdef STOCKIII
char	*os = "/unix";
#else
char	*os = UNIXNAME;
#endif
char	*mr = "/etc/master";
char	line[256], nm[20], pre[20], intrnm[20], devaddr[20], devcnt[20];
char	strat[20];
int	x, vsz, address, unit, bus, deft, count, typ, bcnt,
	root, swap, dump, nsp, pwr, pipe;
long	spl;
FILE	*system, *mast;

struct	nmlist {
	char	n_name[8];
	int	n_type;
	int	n_value;
}	nl[MAXI], *nlptr, *setup(), *endnm, *start,
	*bdevcnt, *bdevsw, *rootdev, *swapdev, *dumpdev, *swplo, *nswap,
	*pipedev, *power;

struct	link {
	char	l_cfnm[8];	/* config name from master table */
	struct	nmlist	*l_nptr;	/* ptr to name list structure */
	int	l_vsz;		/* vector size */
	int	l_def;		/* default unit count */
	int	l_dtype;	/* device type - BLOCK */
} ln[MAXI/3], *lnptr, *setln(), *endln, *search(), *majsrch();

struct	vector {
	int	v_pc;	/* program counter */
	int	v_psw;	/* processor status word */
} *addr, vct;

struct	interface {	/* jsr r5, call; function */
	int	i_jsr;
	int	i_call;
	int	i_func;
} ifc[MAXI/3];

struct	bdev {
	int	b_open;
	int	b_close;
	int	b_strategy;
	int	b_tab;
} bent;

main(argc, argv)
	int	argc;
	char	**argv;
{

	struct	nmlist	*vs, *smap, *cmap, *callo;

	switch(argc) {
	case 3:	mr = argv[2];
	case 2: os = argv[1];
	case 1: break;
	default:
		fprintf(stderr, "usage: sysdef  [ /unix [ /etc/master ] ]");
		exit(1);
	}
	if((system = fopen(os,"r")) == NULL) {
		fprintf(stderr,"cannot read %s\n",os);
		exit(1);
	}
	if((mast = fopen(mr, "r")) == NULL) {
		fprintf(stderr, "cannot read %s\n", mr);
		exit(1);
	}

	nlptr = nl;
	lnptr = ln;
	start = setup("start");
	bdevcnt = setup("_bdevcnt");
	bdevsw = setup("_bdevsw");
	rootdev = setup("_rootdev");
	pipedev = setup("_pipedev");
	swapdev = setup("_swapdev");
	swplo = setup("_swplo");
	nswap = setup("_nswap");
	dumpdev = setup("_dumpdev");
	power = setup("_pwr_clr");
	pre[0] = '_';

	while(fgets(line, 256, mast) != NULL) {
		if(line[0] == '*')
			continue;
		if(line[0] == '$')
			break;
		if(sscanf(line, "%s %d %d %d %s %d %o %d %d %d ",
			nm, &vsz, &x, &typ, &pre[1], &x, &x, &x, &deft, &x, &x)
			!= 10)
			continue;
		if(!strcmp(nm, "memory") || !strcmp(nm, "tty"))
			continue;
		strcpy(intrnm, pre);
		strcpy(devaddr, pre);
		strcpy(devcnt, pre);
		strcat(devaddr, "_addr");	/* _pre_addr */
		strcat(devcnt, "_cnt");		/* _pre_cnt */
		switch(vsz) {
			case 8: strcat(intrnm, "rint"); break;
			case 4: strcat(intrnm, "intr"); break;
			default: continue;
		}
		setln(nm, setup(intrnm), vsz, deft, typ & BLOCK);
		setup(devaddr);
		setup(devcnt);
		if(typ & BLOCK) {
			strcpy(strat, pre);
			strcat(strat, "strategy");
			setup(strat);
		}
	}
	endnm = setup("");
	endln = lnptr;
	nlist(os, nl);

	if(start->n_type == 0) {
		fprintf(stderr, "%s %s\n",
			"symbol \"start\" undefined; ",
			"invalid /unix file");
		exit(1);
	}
	if((start->n_value - IFC) > (MAXI / 3 * (sizeof ifc[0]))) {
		fprintf(stderr, "internal interface array overflow\n");
		exit(1);
	}
	fseek(system, (long) IFC + HDR, 0);
	fread(ifc, sizeof ifc[0], (start->n_value - IFC) /
		sizeof ifc[0], system);

	printf("*\n* dev\tvector\taddr\tbus\tcount\n*\n");
	for(addr = (struct vector *)0; addr != (struct vector *)IFC; addr++) {
		if(addr == (struct vector *)060)
			addr = (struct vector *)070;	/* skips console */
		fseek(system, (long)addr + HDR, 0);
		fread(&vct, sizeof vct, 1, system);
		if((vct.v_pc <= IFC) || (vct.v_pc >= start->n_value))
			continue;	/* skips clio, traps, jmp, etc */
		if((lnptr = search(&ifc[(vct.v_pc - IFC) / sizeof ifc[0]]))
			== NULL) {
			fprintf(stderr,
				"unknown device interrupts at vector %3o\n",
				addr);
			continue;
		}
		printf("%s\t%3o", lnptr->l_cfnm, addr);
		unit = vct.v_psw & CC;
		bus = (vct.v_psw & BUS) >> 5;
		fseek(system, (long)(lnptr->l_nptr[1].n_value + 2 * unit + HDR),
			0);
		fread(&address, 2, 1, system);
		fseek(system, (long)(lnptr->l_nptr[2].n_value + HDR), 0);
		fread(&count, 2, 1, system);
		printf("\t%6o\t%1o", address, bus);
		if((unit + 1) * lnptr->l_def > count)
			printf("\t%d", count % lnptr->l_def);
		printf("\n");
		if(lnptr->l_vsz == 8)
			addr++;
	}

	/* rootdev, swapdev, pipedev, dumpdev, power */

	fseek(system, (long) bdevcnt->n_value + HDR, 0);
	fread(&bcnt, 2, 1, system);
	fseek(system, (long) rootdev->n_value + HDR, 0);
	fread(&root, 2, 1, system);
	fseek(system, (long) pipedev->n_value + HDR, 0);
	fread(&pipe, 2, 1, system);
	fseek(system, (long) swapdev->n_value + HDR, 0);
	fread(&swap, 2, 1, system);
	fseek(system, (long) dumpdev->n_value + HDR, 0);
	fread(&dump, 2, 1, system);
	fseek(system, (long) power->n_value + HDR, 0);
	fread(&pwr, 2, 1, system);

	printf("*\n* System Devices\n*\n");
	if((lnptr = majsrch((root >> 8) & 0377)) == NULL)
		fprintf(stderr, "unknown root device\n");
	else
		printf("root\t%s\t%o\n", lnptr->l_cfnm, root & 0377);

	if((lnptr = majsrch((swap >> 8) & 0377)) == NULL)
		fprintf(stderr, "unknown swap device\n");
	else {
		printf("swap\t%s\t%o\t", lnptr->l_cfnm, swap & 0377);
		fseek(system, (long) swplo->n_value + HDR, 0);
		fread(&spl, 4, 1, system);
		fseek(system, (long) nswap->n_value + HDR, 0);
		fread(&nsp, 2, 1, system);
		printf("%ld\t%d\n", spl, nsp);
	}

	if((lnptr = majsrch((pipe >> 8) & 0377)) == NULL)
		fprintf(stderr, "unknown pipe device\n");
	else
		printf("pipe\t%s\t%o\n", lnptr->l_cfnm, pipe & 0377);

	if((lnptr = majsrch((dump >> 8) & 0377)) == NULL)
		fprintf(stderr, "unknown dump device\n");
	else
		printf("dump\t%s\t%o\n", lnptr->l_cfnm, dump & 0377);

	/* easy stuff */

	nlptr = nl;
	vs = setup("_v");
	cmap = setup("_coremap");
	smap = setup("_swapmap");
	callo = setup("_callout");
	endnm = setup("");
	nlist(os, nl);
	for(nlptr = vs; nlptr != endnm; nlptr++) {
		if(nlptr->n_type == 0) {
			fprintf(stderr, "namelist error\n");
			exit(1);
		}
	}
	fseek(system, (long) vs->n_value + HDR, 0);
	fread(&v, sizeof v, 1, system);
	printf("*\n* Tunable Parameters\n*\n");
	printf("buffers\t%d\n", v.v_buf);
	printf("sabufs\t%d\n", v.v_sabuf);
	printf("procs\t%d\n", v.v_proc);
	printf("mounts\t%d\n", v.v_mount);
	printf("inodes\t%d\n", v.v_inode);
	printf("files\t%d\n", v.v_file);
	printf("calls\t%d\n", v.v_call);
	printf("texts\t%d\n", v.v_text);
	printf("clists\t%d\n", v.v_clist);
	printf("coremap\t%d\n", (smap->n_value - cmap->n_value) / MAPSZ);
	printf("swapmap\t%d\n", (callo->n_value - smap->n_value) / MAPSZ);
	printf("maxproc\t%d\n", v.v_maxup);
	printf("power\t%d\n", pwr ? 1 : 0);
	exit(0);
}

/*
 * setup - add an entry to a namelist structure array
 */
struct	nmlist	*
setup(nam)
	char	*nam;
{
	if(nlptr >= &nl[MAXI]) {
		fprintf(stderr, "internal name list overflow\n");
		exit(1);
	}
	strcpy8(nlptr->n_name, nam);
	nlptr->n_type = 0;
	nlptr->n_value = 0;
	return(nlptr++);
}

/*
 * strcpy8 - copy exactly 8 characters with null padding
 */
strcpy8(to, from)
	register  char  *to, *from;
{
	register  int  i;

	for(i=0; i<8; i++) {
		if((*to++ = *from++) == '\0') {
			while(++i < 8)
				*to++ = '\0';
		}
	}
}

/*
 * setln - set up internal link structure for later
 * function look-up.  Records useful information from the
 * /etc/master table description.
 */
struct	link	*
setln(cf, np, vz, dft, dt)
	char	*cf;
	struct	nmlist	*np;
	int	vz;
	int	dft;
	int	dt;
{
	if(lnptr >= &ln[MAXI/3]) {
		fprintf(stderr, "internal link array overflow\n");
		exit(1);
	}
	strcpy(lnptr->l_cfnm, cf);
	lnptr->l_nptr = np;
	lnptr->l_vsz = vz;
	lnptr->l_def = dft;
	lnptr->l_dtype = dt;
	return(lnptr++);
}

/*
 * search - search for a function indirectly through
 * the link structure.
 */
struct	link	*
search(intfc)
	struct	interface  *intfc;
{
	struct	link	*lnkptr;

	for(lnkptr = ln; lnkptr != endln; lnkptr++) {
		if((lnkptr->l_nptr)->n_value == intfc->i_func)
			return(lnkptr);
	}
	return(NULL);
}

/*
 * majsrch - search for a link structure given the major
 * device number of the device in question.
 */
struct	link *
majsrch(maj)
	int	maj;
{
	struct	link	*lnkptr;

	if(maj > bcnt - 1)
		return(NULL);
	fseek(system, (long) bdevsw->n_value + maj * sizeof bent + HDR, 0);
	fread(&bent, sizeof bent, 1, system);
	for(lnkptr = ln; lnkptr != endln; lnkptr++)
		if(lnkptr->l_dtype)
			if(lnkptr->l_nptr[3].n_value == bent.b_strategy)
				return(lnkptr);
	return(NULL);
}

#endif
