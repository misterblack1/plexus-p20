/*LINTLIBRARY*/
/*
 * Suspend the process for `sleep_tm' seconds - using alarm/pause
 * system calls.  If caller had an alarm already set to go off `n'
 * seconds from now, then Case 1: (sleep_tm >= n) sleep for n, and
 * cause the callers previously specified alarm interrupt routine
 * to be executed, then return the value (sleep_tm - n) to the caller
 * as the unslept amount of time, Case 2: (sleep_tm < n) sleep for
 * sleep_tm, after which, reset alarm to go off when it would have
 * anyway.  In case process is aroused during sleep by any caught
 * signal, then reset any prior alarm, as above, and return to the
 * caller the (unsigned) quantity of (requested) seconds unslept.
 */
#include <signal.h>

unsigned
sleep(sleep_tm)
unsigned sleep_tm;
{
	int	alrm_flg, (*alrm_sig)(), awake();
	unsigned unslept, alrm_tm, left_ovr, alarm();

	if(sleep_tm == 0)
		return(0);

	alrm_tm = alarm(0);			/* prev. alarm time */
	alrm_sig = signal(SIGALRM, awake);	/* prev. alarm prog */

	alrm_flg = 0;
	left_ovr = 0;

	if(alrm_tm != 0) {	/* skip all this if no prev. alarm */
		if(alrm_tm > sleep_tm) {	/* alarm set way-out */
			alrm_tm -= sleep_tm;
			++alrm_flg;
		} else {	/* alarm will shorten sleep time */
			left_ovr = sleep_tm - alrm_tm;
			sleep_tm = alrm_tm;
			alrm_tm = 0;
			--alrm_flg;
			signal(SIGALRM, alrm_sig);
		}
	}

	alarm(sleep_tm);
	pause();		/* Some other signal may be caught,
				   returning here, in which case we
				   set-up to return to our caller */
	unslept = alarm(0);
	if(alrm_flg >= 0)
		signal(SIGALRM, alrm_sig);
	if(alrm_flg > 0 || (alrm_flg < 0 && unslept != 0))
		alarm(alrm_tm + unslept);
	return(left_ovr + unslept);
}

static
awake() {}
