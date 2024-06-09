/*
 * This is the process which decides what to do for each command
 * that is received by the sioc.
 */

#include "sys/param.h"
#include "sys/conf.h"
#include "icp/sioccomm.h"	/* icp specific */
#include "icp/icp.h"	/* icp specific */
#include "icp/pbsioc.h"	/* icp specific */
#ifdef COUNT
extern int profile[];
#endif

#ifdef	MDP
unsigned	savlst;
#endif

sched()
{
#ifdef	VPMSYS
	struct scb scb;
#else
	register struct scb scb;
#endif
	int	schedpri, thistime;
	int	sioclean();
	int	siosig();
#ifndef	VPMSYS
	register struct spr	*ssg;
#endif
#ifdef COUNT
	profile[56]++;
#endif

	schedpri = SIOCPRI;
	thistime = 0;
#ifdef
	savlst = 0;
#endif
	while (1) {
		dqpbiq(&scb);	/* get the next command */
		if ( ++schedpri > SIOCPRI + 10 )
			if ( thistime ) {
				thistime--;
			} else {
				schedpri = SIOCPRI;
				thistime++;
			}
#ifndef VPMSYS
		ssg = &scb.si_un.gdc5;
#endif
		switch (scb.si_cmd) {

#ifdef VPMSYS
		case DOPEN:
			newproc(cdevsw[major(scb.si_un.gdc5.si_parm[0])].d_open, PUSER,
				scb.si_un.gdc5.si_parm, sizeof(scb.si_un.gdc5.si_parm));
			break;

		case DCLOSE:
			newproc(cdevsw[major(scb.si_un.gdc5.si_parm[0])].d_close, PUSER,
				scb.si_un.gdc5.si_parm, sizeof(scb.si_un.gdc5.si_parm));
			break;

		case DREAD:
			newproc(cdevsw[major(scb.si_un.gdc5.si_parm[0])].d_read, PUSER,
				scb.si_un.gdc5.si_parm, sizeof(scb.si_un.gdc5.si_parm));
			break;

		case DWRITE:
			newproc(cdevsw[major(scb.si_un.gdc5.si_parm[0])].d_write, PUSER,
				scb.si_un.gdc5.si_parm, sizeof(scb.si_un.gdc5.si_parm));
			break;

		case DIOCTL:
			newproc(cdevsw[major(scb.si_un.gdc5.si_parm[0])].d_ioctl, PUSER,
				scb.si_un.gdc5.si_parm, sizeof(scb.si_un.gdc5.si_parm));
			break;
		case DCLEAN:
			newproc( sioclean, PUSER,
				scb.si_un.gdc5.si_parm, sizeof(scb.si_un.gdc5.si_parm));
			break;
#else
		case DOPEN:
		     newproc(cdevsw[major(ssg->si_parm[0])].d_open,
					PUSER, ssg->si_parm,
					sizeof(ssg->si_parm));
			break;

		case DCLOSE:
		     newproc(cdevsw[major(ssg->si_parm[0])].d_close,
					PUSER, ssg->si_parm,
					sizeof(ssg->si_parm));

			break;

		case DREAD:
		     newproc(cdevsw[major(ssg->si_parm[0])].d_read,
					PUSER, ssg->si_parm,
					sizeof(ssg->si_parm));
			break;

		case DWRITE:
		     newproc(cdevsw[major(ssg->si_parm[0])].d_write,
					PUSER, ssg->si_parm,
					sizeof(ssg->si_parm));
			break;

		case DIOCTL:
		     newproc(cdevsw[major(ssg->si_parm[0])].d_ioctl,
					PUSER, ssg->si_parm,
					sizeof(ssg->si_parm));
			break;
		case DCLEAN:
			newproc( sioclean, PUSER,
				ssg->si_parm, sizeof(ssg->si_parm));
			break;
#endif
		}
	}
}
