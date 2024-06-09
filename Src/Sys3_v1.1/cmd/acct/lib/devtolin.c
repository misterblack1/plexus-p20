/*
 *	convert device to linename (as in /dev/linename)
 *	return ptr to LSZ-byte string, "?" if not found
 *	device must be character device
 *	maintains small list in tlist for speed
 */

#include <sys/types.h>
#include "acctdef.h"
#include <stdio.h>
#include <sys/dir.h>

#define TSIZE1	50	/* # distinct names, for speed only */
static	tsize1;
static struct tlist {
	char	tname[LSZ];	/* linename */
	dev_t	tdev;		/* device */
} tl[TSIZE1];

static struct direct d;

dev_t	lintodev();

char *
devtolin(device)
dev_t device;
{
	register struct tlist *tp;
	FILE *fdev;

	for (tp = tl; tp < &tl[tsize1]; tp++)
		if (device == tp->tdev)
			return(tp->tname);

	if ((fdev = fopen("/dev", "r")) == NULL)
		return("?");
	while (fread(&d, sizeof(d), 1, fdev) == 1)
		if (d.d_ino != 0 && lintodev(d.d_name) == device) {
			if (tsize1 < TSIZE1) {
				tp->tdev = device;
				CPYN(tp->tname, d.d_name);
				tsize1++;
			}
			fclose(fdev);
			return(d.d_name);
		}
	fclose(fdev);
	return("?");
}
