#include "sys/plexus.h"
#include "sys/param.h"
#include "sys/seg.h"
#include "sys/map.h"
#include "sys/buf.h"
#include "sys/systm.h"
#include "sys/var.h"

struct map mbmap[NUMLOGPAGE * NUMMBIOMAPS];
static mbmflag;

mbmalloc(nseg)
unsigned nseg;
{
	register base;

	spl6();
	while ((base = malloc(mbmap, nseg)) == NULL) {
		mbmflag = 1;
		sleep(mbmap, PSWP+1);
	}
	spl0();
	return(base-1);		/* Map really starts at zero not one */
}

static
mbmload(base, nseg, pagenum)
{
	register int i;
	unsigned maptemp[NUMLOGPAGE * NUMMBIOMAP];

	for (i = base; i < nseg + base; i++)
		maptemp[i] = pagenum++;
	copymout(&maptemp[base], P_MMB0 + (base<<1), nseg<<1);
}

mbmfree(bp)
register struct buf *bp;
{
	unsigned base;

	/* Find starting map address by finding offset from start of */
	/*  multibus io segment and shifting for page offset */
	hiword(bp->b_paddr) = SEGTOMAP(hiword(bp->b_paddr) - MBSEG);
	base = (bp->b_paddr >> S_PAGENUM) + 1;
	mfree(mbmap, btoc(bp->b_bcount + (bp->b_paddr&M_BIP)), base);
	if (mbmflag) {
		wakeup(mbmap);
		mbmflag = 0;
	}
}

mbminit()
{
	int i;

	mfree(mbmap, NUMLOGPAGE, 1);
	for (i=1; i<NUMMBIOMAPS; i++)
		mfree(mbmap, NUMLOGPAGE-1, NUMLOGPAGE*i+2);
}

mbmaddr(bp)
register struct buf *bp;
{
	register nseg;
	register base;

	nseg = btoc(bp->b_bcount + (bp->b_paddr&M_BIP));
	base = mbmalloc(nseg);
	mbmload(base, nseg, (unsigned)(bp->b_paddr>>S_PAGENUM));
	bp->b_flags |= B_MAP;
	hiword(bp->b_paddr) = MBSEG + MAPTOSEG(base/NUMLOGPAGE);
	loword(bp->b_paddr) = (bp->b_paddr&M_BIP) + ctob(base);
}
