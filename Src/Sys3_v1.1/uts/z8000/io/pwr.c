#include "sys/param.h"
#include "sys/systm.h"
#include "sys/proc.h"


pwr()
{
	register (**clr)();

	if (pwrlock())
		return;
	printf("\377\377\377\n\nPower fail #%d\n\n", pwr_cnt/2);
	pwr_act = 1;
	/* call clear and start routines */
	clr = &pwr_clr[0];
	while (*clr) {
		(**clr)();
		clr++;
	}
	/* if none - halt */
	if (clr == &pwr_clr[0]) {
		printf("Stopped\n");
		for (;;);
	}
	/* tell Init */
	psignal(&proc[1], SIGHUP);
	pwr_act = 0;
	pwrunlock();
}
