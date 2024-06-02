#include "/p3/usr/include/icp/sioc.h"	/* icp specific */
#include "/p3/usr/include/icp/sioccomm.h"	/* icp specific */
#include "/p3/usr/include/icp/icp.h"	/* icp specific */
#include "/p3/usr/include/icp/pbsioc.h"	/* icp specific */
#include "/p3/usr/include/icp/ctc.h"	/* icp specific */

/*
 * Move data from multibus memory to local memory.
 */

mvfrommb(lcladx, mbadx, len)
long mbadx;
register int *lcladx;
register int len;
{
	register int *madx;
	register int cnt;

	while (len) {
		madx = (int *) ((mbadx & MBADRMSK) + MBADRADD);
		out_local(P796MSB, (int)((mbadx >> MBMSBSHFT) & MBMSBMSK) | 1 );
		cnt = MBADRSPACE - (mbadx & MBADRMSK);
		if (cnt > MBMAX) {
			cnt = MBMAX;
		}
		if (cnt > len) {
			cnt = len;
		}
		mbadx += cnt;
		len -= cnt;

		/* get multibus */

		out_local(P796ARBINT, CCTR | CUPCLK | CTIME | CRESET | CCTRL);
		out_local(P796ARBINT, 2);
		mres();	/* ask for bus */
		while ((in_local(P796ARBINT) & 0xff) == 2) {
		}

		/* move bytes */

		for (; cnt; cnt -= 2) {
			*lcladx++ = *madx++;
		}

		/* give back bus */

		mset();
	}
}


/*
 * Move data from local memory to multibus memory.
 */

mvtomb(mbadx, lcladx, len)
register int *lcladx;
long mbadx;
register int len;
{
	register int *madx;
	register int cnt;

	while (len) {
		madx = (int *) ((mbadx & MBADRMSK) + MBADRADD);
		out_local(P796MSB, (int)((mbadx >> MBMSBSHFT) & MBMSBMSK) | 1 );
		cnt = MBADRSPACE - (mbadx & MBADRMSK);
		if (cnt > MBMAX) {
			cnt = MBMAX;
		}
		if (cnt > len) {
			cnt = len;
		}
		mbadx += cnt;
		len -= cnt;

		/* get multibus */

		out_local(P796ARBINT, CCTR | CUPCLK | CTIME | CRESET | CCTRL);
		out_local(P796ARBINT, 2);
		mres();	/* ask for bus */
		while ((in_local(P796ARBINT) & 0xff) == 2) {
		}

		/* move bytes */

		for (; cnt; cnt -= 2) {
			*madx++ = *lcladx++;
		}

		/* give back bus */

		mset();
	}
}
