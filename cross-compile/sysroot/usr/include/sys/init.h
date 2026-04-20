/* SID @(#)init.h	5.4 */
/* @(#)init.h	6.3 */

/* routines all machines need for initialization */
extern int clkstart(),cinit(),binit(),errinit(),iinit(),inoinit();
extern int finit();
extern dkinit(),mtinit(),miinit();

/* initialization routines for LUNDELL and SCHROEDER machines */
#if defined(LUNDELL)| defined(SCHROEDER)
extern pdinit(),ptinit(),rminit(),siinit(),xyinit();
extern skyinit();
extern ccbinit();
extern	scsi_init();
extern	odinit();
#endif

/* initialization routines for ROBIN machines */
#ifdef robin
extern rminit(),siinit();
extern skyinit();
extern sqinit();
extern scinit();
extern fpinit();
extern tpinit();
extern initoff();
#endif

/* initialization routines for KICKER machines */
#ifdef KICKER
extern tthinit();
extern tsinit();
extern sminit();
extern ccbinit();
extern	scsi_init();
extern	odinit();
#endif

/* NOS initialization routines */
#ifdef NOS
extern etinit();
extern ncfinit();
#endif

/*	Array containing the addresses of the various initializing	*/
/*	routines executed by "main" at boot time.			*/

int (*init_tbl[])() = {
/* initialize disks then tapes */
#ifdef robin
	sqinit,
	scinit,
	fpinit,
#endif
	dkinit,
	mtinit,
#if defined(LUNDELL)| defined(SCHROEDER)
	pdinit,
	xyinit,			/* must come after pdinit; (kludge) JF */
	ptinit,
	rminit,
	siinit,
	skyinit,
#endif
#ifdef robin
	tpinit,
	initoff,
	rminit,
	siinit,
	skyinit,
#endif
#ifdef KICKER
	sminit,
	tthinit,
	tsinit,
#endif

/* initialize NOS */
#ifdef NOS
	etinit,
	ncfinit,
#endif

/* general operating system initialization */
	inoinit,
	clkstart,
	cinit,
	binit,
	errinit,
	miinit,
	finit,
	iinit,

/* some less important drivers */
#if defined(LUNDELL)| defined(SCHROEDER) | defined(KICKER)
	ccbinit,
	scsi_init,
	odinit,
#endif
	0
};
