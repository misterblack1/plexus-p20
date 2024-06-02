
/*
 * This source contains code to access the real time clock
 * on the cpu board. Time in this clock is maintained in
 * GMT. 
 */

#include "sys/param.h"
#include "sys/plexus.h"

#define	NUMDIGIT	13		/* number of digits in clock */
#define	DELAY150	70		/* used to produce a 150 usec delay */

char dmsize[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/*
 * Read the clock chip contents into buf.
 */

tdread(buf)
char *buf;
{
	register	pioaval;
	register	piobval;
	register	savepri;
	register	i;

	savepri = spl7();	/* don't let anyone disturb this code */

	pioaval = in_local(P_PIOADAT);	/* get bit settings for ports */
	piobval = in_local(P_PIOBDAT);

	out_local(P_PIOBCTL, PIOBCTL);	/* change b port to read data */
	out_local(P_PIOBCTL, PIOBRD);


	out_local(P_PIOADAT, pioaval | B_PIOHOLD); /* turn on hold */
	for (i = 0; i < DELAY150; i++){		/* delay approx 150 usec */
	}

	out_local(P_PIOADAT, pioaval | B_PIOHOLD | B_PIOREAD); /* read */

	for ( i = 0; i < NUMDIGIT; i++) {
		out_local(P_PIOBDAT, i << S_TODADX);	/* set up address */
		buf[i] = in_local(P_PIOBDAT) & M_TODDATA;	/* get data */
	}

	out_local(P_PIOADAT, pioaval);	/* restore old bit fields */
	out_local(P_PIOBDAT, piobval);

	out_local(P_PIOBCTL, PIOBCTL);	/* restore b port to write data */
	out_local(P_PIOBCTL, PIOBWRT);

	splx(savepri);
}


/*
 * Write data in buf to the clock chip.
 */

tdwrite(buf)
char	*buf;
{
	register	savepri;
	register	pioaval;
	register	piobval;
	register	i;

	savepri = spl7();	/* don't disturb this code */

	pioaval = in_local(P_PIOADAT);	/* save old bit definitions */
	piobval = in_local(P_PIOBDAT);

	out_local(P_PIOADAT, pioaval | B_PIOHOLD);	/* turn on hold */
	for ( i = 0; i < DELAY150; i++) {	/* delay about 150 usec */
	}

	for (i = 0; i < NUMDIGIT; i++) {
		out_local(P_PIOBDAT,
	    	     	 (i << S_TODADX) + ((buf[i] & M_TODDATA) << S_TODDATA));
		out_local(P_PIOADAT, pioaval | B_PIOHOLD | B_PIOWRITE);
		out_local(P_PIOADAT, pioaval | B_PIOHOLD);
	}

	out_local(P_PIOADAT, pioaval);		/* restore port a data */
	out_local(P_PIOBDAT, piobval);		/* restore port b data */

	splx(savepri);	/* restore old priority */
}

/*
 * calculate number of days in a year
 */

dysize(year)
int year;
{
	return((year % 4) == 0 ? 366: 365);
}

/*
 * return the time from the tod chip as a long value which
 * is the number of seconds since JAN 1, 1970 GMT.
 * Returns zero if the data from the tod chip is fubar.
 */

long gettod()
{
	long t;		/* temp to keep time */
	register i;	/* counter */
	register y;	/* year */
	register m;	/* month */
	register d;	/* day */
	char b[NUMDIGIT];	/* temp for current tod reading */

	tdread(b);	/* get current time */
	y = 1900 + b[12] * 10 + b[11];
	m = b[10] * 10 + b[9];
	d = (b[8] & 0x3) * 10 + b[7];
	if (m == 0 || d == 0) {
		return(0);
	}
	t = 0;	
	for (i = 1970; i < y; i++) {
		t += dysize(i);
	}
	if ((dysize(y) == 366) & (m >= 3)) {
		t++;
	}
	while (--m) {
		t += dmsize[m - 1];
	}
	t += d - 1;
	t = (24 * t) + (b[5] & 0x3) * 10 + b[4];
	t = (60 * t) + b[3] * 10 + b[2];
	t = (60 * t) + b[1] * 10 + b[0];
	return(t);
}

/*
 * write t to clock chip where t is the number of seconds since
 * JAN 1, 1970 00:00 GMT.
 */

puttod(t)
long t;
{
	long hms;
	long day;
	register d0;
	register d1;
	char b[NUMDIGIT];

	hms = t % 86400;
	day = t / 86400;
	if(hms < 0) {
		hms += 86400;
		day -= 1;
	}

	/* sec */

	b[0] = 0;
	b[1] = 0;

	/* min */

	d1 = hms / 60;
	b[2] = (d1 % 60) % 10;
	b[3] = (d1 % 60) / 10;

	/* hr (24 hr format) */

	d1 /= 60;
	b[4] = d1 % 10;
	b[5] = (d1 / 10) | 0x8;

	/* day of week */

	b[6] = (day + 7340036) % 7;

	/* year */

	if (day >= 0) {
		for (d1 = 70; day >= dysize(d1); d1++) {
			day -= dysize(d1);
		}
	} else {
		for (d1 = 70; day < 0; d1--) {
			day += dysize(d1 - 1);
		}
	}
	d0 = day;
	b[11] = d1 % 10;
	b[12] = d1 / 10;

	/* month */

	if (dysize(d1) == 366) {
		dmsize[1] = 29;
	}
	for (d1 = 0; d0 >= dmsize[d1]; d1++) {
		d0 -= dmsize[d1];
	}
	b[9] = (d1 + 1) % 10;
	b[10] = (d1 + 1) / 10;

	/* day */

	b[7] = (d0 + 1) % 10;
	b[8] = ((d0 + 1) / 10) + (dmsize[1] == 29 ? 0x4 : 0);
	dmsize[1] = 28;
	tdwrite(b);
}
