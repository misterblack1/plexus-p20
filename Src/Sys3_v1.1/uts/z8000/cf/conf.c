
/*
 * Revision Level %I%
 * Last Delta     %G% %U%
 * File Name      %M%
 */

#define PLEXPROF

/*
 *  Configuration information
 */


#define	MEMORY_0 1
#define	TTY_0 1
#define	ERRLOG_0 1
#define PRF_0 1

#define	NBUF	64		/* Stock SYSTEM III was 20 */
#ifndef PNETDFS
#define	NINODE	200		/* Stock SYSTEM III was 40 */
#else
#define NINODE	150
#endif
#define	NFILE	175		/* Stock SYSTEM III was 40 */
#ifndef PNETDFS
#define	NMOUNT	12		/* Stock SYSTEM III was 2 */
#else
#define NMOUNT   5
#endif
#define	CMAPSIZ	50
#define	SMAPSIZ	50
#define	NCALL	40		/* Stock SYSTEM III was 20 */
#ifndef	PNETDFS
#define	NPROC	100		/* Stock SYSTEM III was 25 */
#else
#define NPROC	75
#endif
#define	NTEXT	40		/* Stock SYSTEM III was 15 */
#define	NCLIST	20
#ifndef PNETDFS
#define	NSABUF	9		/* Stock SYSTEM III was 3 */
#else
#define NSABUF  6
#endif
#define	POWER	0
#define	MAXUP	25
#define	NHBUF	64

#include	"sys/plexus.h"
#include	"sys/param.h"
#include	"sys/space.h"
#include	"sys/systm.h"

extern	nodev(), nulldev();
extern	dkopen(), dkread(), dkwrite(), dkstrategy(), dktab;
extern	mtopen(), mtclose(), mtread(), mtwrite(), mtstrategy(),
	mtioctl(), mttab;
#ifndef VPMSYSIS
extern	pdopen(), pdread(), pdwrite(), pdstrategy(), pdtab;
extern	ptopen(), ptclose(), ptread(), ptwrite(), ptstrategy(),
	ptioctl(),pttab;
#endif
#ifndef VPMSYSIMSC
extern	isopen(), isread(), iswrite(), isstrategy(), istab;
extern	rmopen(), rmclose(), rmread(), rmwrite(), rmstrategy(),
	rmioctl(), rmtab;
#endif
extern	usopen(), usclose(), usread(), uswrite(), usioctl();
extern	syopen(), syclose(), syread(), sywrite(), syioctl();
extern	icopen(), icclose(), icread(), icwrite(), icioctl();
extern	spopen(), spclose(), spread(), spwrite(), spioctl();
extern	ppopen(), ppclose(), ppread(), ppwrite(), ppioctl();
extern	mmopen(), mmread(),  mmwrite();
extern  prfread(),prfwrite(),prfioctl();
extern	erropen(), errclose(), errread();
#ifdef NCF
extern	etopen(), etclose(), etread(), etwrite();
extern ncfopen(); ncfclose(); ncfread(); ncfwrite(), ncfioctl();
#endif
#ifdef VPMSYS
extern	vpmopen(), vpmclose(), vpmread(), vpmwrite(), vpmioctl();
extern	tropen(), trclose(), trread(), trioctl(), trsave();
#endif

typedef int	(*pfunc_t)();

int	(*bdevsw[])() =
{
/* 0*/	dkopen, nulldev, dkstrategy, (pfunc_t)&dktab,	/* dk */
#ifndef VPMSYSIMSC
/* 1*/	rmopen,	rmclose, rmstrategy, (pfunc_t)&rmtab,	/* mt==rm */
#else
/* 1*/	nodev, nodev, nodev, 0,
#endif
#ifndef VPMSYSIS
/* 2*/	pdopen, nulldev, pdstrategy, (pfunc_t)&pdtab,	/* pd */
/* 3*/	nodev,	nodev,	 nodev,	     0,		/* no pt blk */
#else
/* 2*/	nodev,	nodev, 	nodev, 	0,
/* 3*/	nodev,	nodev, 	nodev, 	0,
#endif
#ifndef VPMSYSIMSC
/* 4*/	isopen, nulldev, isstrategy, (pfunc_t)&istab,	/* is */
/* 5*/	rmopen, rmclose, rmstrategy, (pfunc_t)&rmtab,	/* rm */
#else
/* 4*/	nodev,	nodev, 	nodev, 	0,
/* 5*/	nodev,	nodev, 	nodev, 	0,
#endif
	0
};

int	(*cdevsw[])() =
{
/* 0*/	usopen,	usclose,usread,	uswrite,usioctl,	/* console */
/* 1*/	nodev,	nodev, 	nodev, 	nodev, 	nodev,	
/* 2*/	nulldev,nulldev,mmread,	mmwrite,nodev, 		/* mem */
/* 3*/	dkopen,	nodev, 	dkread,	dkwrite,nodev,		/* dk */
/* 4*/	mtopen,	mtclose,mtread,	mtwrite,mtioctl,	/* mt */
#ifndef VPMSYSIS
/* 5*/	pdopen,	nodev, 	pdread,	pdwrite,nodev,		/* pd */
/* 6*/	ptopen,	ptclose,ptread,	ptwrite,ptioctl,	/* pt */
#else
/* 5*/	nodev,	nodev, 	nodev, 	nodev, 	nodev,
/* 6*/	nodev,	nodev, 	nodev, 	nodev, 	nodev,
#endif
#ifndef VPMSYSIMSC
/* 7*/	isopen,	nodev, 	isread,	iswrite,nodev,		/* is */
/* 8*/	rmopen,	rmclose,rmread,	rmwrite,rmioctl,	/* rm */
#else
/* 7*/	nodev,	nodev, 	nodev, 	nodev, 	nodev,
/* 8*/	nodev,	nodev, 	nodev, 	nodev, 	nodev,
#endif
#ifdef NCF
/* 9*/	etopen, etclose,etread, etwrite,nodev,
#else
/* 9*/	nodev,	nodev, 	nodev, 	nodev, 	nodev,
#endif
/*10*/	nodev,	nodev, 	nodev, 	nodev, 	nodev,
/*11*/	nodev,	nodev, 	nodev, 	nodev, 	nodev,
/*12*/	nodev,	nodev, 	nodev, 	nodev, 	nodev,
/*13*/	syopen,	nulldev,syread,	sywrite,syioctl,	/* tty */
/*14*/	icopen,	icclose,icread,	icwrite,nodev,		/* ic */
/*15*/	spopen,	spclose,spread,	spwrite,spioctl,	/* sp */
/*16*/	ppopen,	ppclose,ppread,	ppwrite,ppioctl,	/* pp */
#ifdef NCF
/*17*/	ncfopen, ncfclose,ncfread, ncfwrite,ncfioctl,
#else
/*17*/	nodev,	nodev, 	nodev, 	nodev, 	nodev,
#endif
#ifdef VPMSYS
/*18*/	vpmopen,vpmclose,vpmread,vpmwrite,vpmioctl,	/* vpm */
#else
/*18*/	nodev,	nodev, 	nodev, 	nodev, 	nodev,
#endif
#ifdef VPMSYS
/*19*/	tropen, trclose,trread, trsave, trioctl,        /* trace */
#else
/*19*/	nodev,	nodev, 	nodev,  nodev,  nodev,	
#endif
/*20*/	erropen,errclose,errread,nodev, nodev, 		/* err */
/*21*/	nulldev,nulldev,prfread,prfwrite,prfioctl	/* prf */
};

extern	dkinit();
extern	mtinit();
#ifndef VPMSYSIS
extern	pdinit();
extern	ptinit();
#endif
#ifndef VPMSYSIMSC
extern	isinit();
extern	rminit();
#endif
extern	siinit();
#ifdef NCF
extern	etinit();
extern  ncfinit();
#endif

int	(*initsw[])() =
{
	dkinit,
	mtinit,
#ifndef VPMSYSIS
	pdinit,
	ptinit,
#endif
#ifndef VPMSYSIMSC
	isinit,
	rminit,
#endif
	siinit,
#ifdef NCF
	etinit,
	ncfinit,
#endif
	0
};

int	ctc0tim = 5 * 25;	/* 4*25 for 4MHz, 5*25 for 5MHz */
				/* modified by system to appropriate value */

int	bdevcnt = 6;
int	cdevcnt	= 22;

int	rootdev	= makedev(0,1);
int	pipedev = makedev(0,1);
int	dumpdev	= makedev(0,1);
int	swapdev	= makedev(0,1);

daddr_t	swplo	= 36000;
int	nswap	= 4000;

int	phypage;
int	diagswits;

struct locklist locklist[NFLOCKS];

#ifdef	PLEXPROF
struct	profbuf profbuf = 
	{ 0L, 0L, 0L, -1}; /* profsf == -1 means no profiling */
#endif
	
int	(*pwr_clr[])() = 
{
	(int (*)())0
};

int	(*dev_init[])() = 
{
	(int (*)())0
};
