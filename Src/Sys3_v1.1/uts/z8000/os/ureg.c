#include "sys/plexus.h"
#include "sys/param.h"
#include "sys/systm.h"
#include "sys/dir.h"
#include "sys/user.h"
#include "sys/proc.h"
#include "sys/text.h"
#include "sys/seg.h"

/*
 * Load the user hardware segmentation
 * registers from the software prototype.
 * The software registers must have
 * been setup prior by estabur.
 */
sureg()
{
	int taddr, daddr;
	register i, c;
	struct text *tp;
	int maptemp[NUMLOGPAGE];

	taddr = daddr = u.u_procp->p_addr;
	if ((tp=u.u_procp->p_textp) != NULL)
		taddr = tp->x_caddr;

	for (i = 0; i < NUMLOGPAGE; i++) {
		c = u.u_uisa[i];
		if ((c & B_IP) == 0)
			if (c & B_EX)
				c += taddr;
			else
				c += daddr;
		maptemp[i] = c;
	}
	copymout(maptemp, P_MUI0, NUMLOGPAGE << 1);

	for (i = 0; i < NUMLOGPAGE; i++) {
		c = u.u_uisa[i + NUMLOGPAGE];
		if ((c & B_IP) == 0)
			if (c & B_EX)
				c += taddr;
			else
				c += daddr;
		maptemp[i] = c;
	}
	copymout(maptemp, P_MUD0, NUMLOGPAGE << 1);
}

/*
 * Set up software prototype segmentation
 * registers to implement the 3 pseudo
 * text,data,stack segment sizes passed
 * as arguments.
 * The argument sep specifies if the
 * text and data+stack segments are to
 * be separated.
 * The last argument determines whether the text
 * segment is read-write or read-only.
 */
estabur(nt, nd, ns, sep, xrw)
unsigned nt, nd, ns;
{
	register a, *ap, i;

	if (checkur(nt, nd, ns, sep))
		return(-1);
	a = 0;
	ap = &u.u_uisa[0];
	for (i = 0; i < nt; i++)
		*ap++ = a++ | B_EX | (xrw == RO ? B_RO : 0);
	if (sep)
		for (i = 0; i < (NUMLOGPAGE - nt); i++)
			*ap++ = B_IP | B_RO;
	a = USIZE;
	for (i = 0; i < nd; i++)
		*ap++ = a++;
	for (i = 0; i < (NUMLOGPAGE - nd - ns - ((!sep) ? nt : 0)); i++)
		*ap++ = B_IP | B_RO;
	for (i = 0; i < ns; i++)
		*ap++ = a++;
	if (!sep) {
		ap = &u.u_uisa[0];
		for (i = 0; i < NUMLOGPAGE; i++)
			ap[i + NUMLOGPAGE] = ap[i];
	}
	sureg();
	return(0);
}

checkur(nt, nd, ns, sep)
{
	if(sep) {
		if(ctos(nt) > NUMLOGPAGE || ctos(nd)+ctos(ns) > NUMLOGPAGE)
			goto err;
	} else {
		if(ctos(nt)+ctos(nd)+ctos(ns) > NUMLOGPAGE)
			goto err;
	}
	if(nt+nd+ns+USIZE > maxmem)
		goto err;
	return(0);

err:
	u.u_error = ENOMEM;
	return(-1);
}
