/*
 *	Memory special file
 *	minor device 0 is physical memory
 *	minor device 1 is kernel memory
 *	minor device 2 is EOF/NULL
 *	minor device 3 is local i/o
 *	minor device 4 is multibus i/o
 */

#include "sys/plexus.h"
#include "sys/param.h"
#include "sys/dir.h"
#include "macros.h"
#include "sys/systm.h"
#include "sys/user.h"

mmread(dev)
{
	register unsigned n;
	register on;
	register c;
	register bn;
	register a, b;

	do {
		n = min(u.u_count, BSIZE);
		switch (dev) {
		case 0:
			bn = (u.u_offset >> S_PAGENUM) & M_PPN;
			if (bn >= phypage ||
				(bn >= NUMLOGPAGE && bn < FIRSTPPN)) {
					u.u_error = ENXIO;
					break;
			}
			on = u.u_offset & M_BIP;
			a = in_local(P_MUI0);
			b = in_local(P_MUI0+2);
			spl7();
			out_local(P_MUI0, bn);
			out_local(P_MUI0+2, bn+1);
			if (copyio(UISEG, on, u.u_base, n, U_RUD))
				u.u_error = ENXIO;
			out_local(P_MUI0, a);
			out_local(P_MUI0+2, b);
			spl0();
			break;

		case 1:
			if (copyout((short)u.u_offset, u.u_base, n))
				u.u_error = ENXIO;
			break;
		case 2:
			return;
		case 3:
		case 4:
			on = (unsigned) u.u_offset;
			if (dev == 3)
				c = in_local(on);
			else
				c = in_multibus(on);
			passc((c >> 8) & 0377);
			passc(c & 0377);
			return;
		}
		u.u_offset += n;
		u.u_base += n;
		u.u_count -= n;
	} while(u.u_error==0 && u.u_count!=0);
}

mmwrite(dev)
{
	register unsigned n;
	register on;
	register bn;
	register a, b, c, d;

	while(u.u_error==0 && u.u_count!=0) {
		n = min(u.u_count, BSIZE);
		switch (dev) {
		case 0:
			bn = (u.u_offset >> S_PAGENUM) & M_PPN;
			if (bn >= phypage ||
				(bn >= NUMLOGPAGE && bn < FIRSTPPN)) {
					u.u_error = ENXIO;
					break;
			}
			on = u.u_offset & M_BIP;
			a = in_local(P_MUI0);
			b = in_local(P_MUI0+2);
			spl7();
			out_local(P_MUI0, bn);
			out_local(P_MUI0+2, bn+1);
			if (copyio(UISEG, on, u.u_base, n, U_WUD))
				u.u_error = ENXIO;
			out_local(P_MUI0, a);
			out_local(P_MUI0+2, b);
			spl0();
			break;
		case 1:
			if (copyin(u.u_base, (short)u.u_offset, n))
				u.u_error = ENXIO;
			break;
		case 2:
			break;
		case 3:
		case 4:
			on = (unsigned) u.u_offset;
			a = cpass();
			b = cpass();
			if (a < 0 || b < 0 || u.u_error != 0) {
				return;
			}
			c = b + (a << 8);
			if (minor(dev) == 3) {
				out_local(on, c);
			} else {
				out_multibus(on, c);
			}
			return;
		}
		u.u_offset += n;
		u.u_base += n;
		u.u_count -= n;
	}
}
