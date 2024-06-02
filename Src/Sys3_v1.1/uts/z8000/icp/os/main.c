/*
 * the main program
 */

#include "sys/param.h"
#ifdef DEBUG
#include "sys/tty.h"
#endif
#include "icp/sioc.h"	/* icp specific */
#include "icp/siocunix.h"	/* icp specific */
#include "icp/siocprom.h"	/* icp specific */
#include "icp/sioccomm.h"	/* icp specific */
#include "icp/icp.h"	/* icp specific */
#include "icp/pbsioc.h"	/* icp specific */

/*
 * static variables
 */

char deftab[8] = {
	3,	/* 0 - 110 baud */
	7,	/* 1 - 300 baud */
	8,	/* 2 - 600 baud */
	9,	/* 3 - 1200 baud */
	11,	/* 4 - 2400 baud */
	12,	/* 5 - 4800 baud */
	13,	/* 6 - 9600 baud */
	14,	/* 7 - 19.2 kbaud */
};

extern struct pctl pctl;

int	sched();
int	daemon();
#ifndef	VPMSYS
/*
int	spdaemon();
int	rbdaemon();
*/
int	rbspdaem();
int	pbdaemon();
int	dmadaemon();
#endif
#ifdef VPMSYS
int     interp();
#endif
#ifdef DEBUG
extern struct tty ttytab[];
int	debugger();
int	mbimissed();
#endif


/*
 * setup and main driver for sioc software
 */

main()
{

	/* set up for parity checking and clear pio req flop */

#ifdef	VPMSYS
	out_local(PCMD, cmdsav |= PERREN | PIOREQ);
#else
	out_local(PCMD, (cmdsav |= PERREN | PIOREQ) | RESPERR);
	out_local(PCMD, cmdsav );
#endif

	/* get multibus addresses of communications areas */

	pctl.pc_paddr = (((long) in_local(PWUA)) & 0xffff) << 4;
	pctl.pc_oaddr = pctl.pc_paddr + sizeof(struct cmd);
	pctl.pc_iaddr = pctl.pc_oaddr + sizeof(struct scb);

	/* disable 796 bus requests */

	mset();

	/* initialize various board functions */

	cinit();	/* initialize cblocks */
	binit();	/* initialize icp-unix communication blocks */
#ifndef	VPMSYS
	spinit();
#endif
	sioinit();
	ctcinit();
#ifndef VPMSYS
	pioinit();
#endif
	dmainit();

	/* Issue message before starting up the scheduler. */

	printf( "\nPlexus ICP Software Initialization Complete.\n" );

	/* start the scheduler and the putchar daemon */

	newproc(daemon, PUSER, 0, 0);
	newproc(sched, PUSER, 0, 0);
#ifdef VPMSYS
	newproc(interp, PUSER, 0, 0);
#endif
#ifndef	VPMSYS
/*
	newproc( spdaemon, PINOD, 0, 0 );
	newproc( rbdaemon, PINOD, 0, 0 );
*/
	newproc( rbspdaem, PINOD, 0, 0 );
	newproc( pbdaemon, PINOD-1, 0, 0 );
	newproc( dmadaemon, PINOD, 0, 0 );
#endif
#ifdef DEBUG
	/* make sure we can talk with CONSOLE */
	ttytab[CONSOLE].t_cflag = B9600|CS8|CREAD|CSTOPB;
	sioparam(CONSOLE);
	newproc(debugger, PUSER, 0, 0);
#ifndef VPMSYS
	timeout(mbimissed,0,50);
#endif
#endif
	startup();
}
