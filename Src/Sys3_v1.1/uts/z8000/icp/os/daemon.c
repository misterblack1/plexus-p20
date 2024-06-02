/*
 * The following implements a daemon which looks 
 * for characters to print via the putchar command
 * buffer.
 */

#include "sys/param.h"
#include "icp/sioccomm.h"	/* icp specific */
#include "icp/siocprom.h"	/* icp specific */
#include "icp/icp.h"	/* icp specific */
#include "icp/pbsioc.h"	/* icp specific */

#ifdef COUNT
extern int profile[];
#endif
#ifdef WAITING
extern int profile[];
#endif
/* time constants */

#define	SHORTTIME	5		/* sample every .1 sec */
#define	LONGTIME	200		/* sample every 40 sec */
#define	WAITTIME	200		/* revert to long after 20 short */

/* static variables */

int	time;

extern struct pctl pctl;

/*
 * This routine is called by the clock to restart the daemon
 */

godaemon()
{

#ifdef COUNT
	profile[37]++;
#endif

	wakeup((caddr_t) &time);
}


/*
 * This is the daemon
 */

daemon()
{
	struct cmd cmd;
	int	s;

#ifdef COUNT
	profile[38]++;
#endif

	time = 1;

	while (1) {
		s = spl6();
#ifdef VPMSYS
		timeout(godaemon, 0, time, 2002);
#else
		timeout(godaemon, 0, time);
#endif
#ifdef WAITING
		profile[6]++;
#endif
		sleep((caddr_t) &time, PUSER);
		splx( s );
		mvfrommb(&cmd, pctl.pc_paddr, sizeof(cmd));
		if (cmd.c_gate == 0xff) {
			switch( cmd.c_command ) {

			case MEMPRMS:
				pctl.pc_min = cmd.c_mbadx;
				pctl.pc_max = cmd.c_mbadx + cmd.c_len;
				break;
			default:
				panic( "daemon" );
			}
			cmd.c_gate = 0;
			mvtomb((pctl.pc_paddr + 2), &cmd.c_command, (sizeof(cmd) - 2) );
			mvtomb(pctl.pc_paddr, &cmd, 2);
			exit();
		}
		time = LONGTIME;
	}
}
