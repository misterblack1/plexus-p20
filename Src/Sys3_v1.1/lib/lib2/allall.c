#include <sys/plexus.h>
/* allocate all pages of data space.  Find highest used phys page
 * in text and data space; use it as first page of newly allocated
 * data pages.
 * Note that this routine could overwrite the bootstrap's physical 
 * text/data pages.  Currently, 9/16/82, it doesn't.
 */
short	allalldone;
extern unsigned segno;
allall()
{
	int		dataseg;
	int		hi_physpage;
	int		inv_pageno;
	register int	pageno;
	register int	physpage;
	int		textseg;

	if(allalldone++ != 0) return 0;

	hi_physpage = 0;
	inv_pageno = -1;
	dataseg = segno;
	textseg = dataseg + 0x100;

		/* find 1st invalid page in data space */

	for(pageno = 0; pageno < NUMLOGPAG; pageno++) {
		physpage = in_local(mapport(dataseg,pageno));
/*		printf("data page %u = 0x%x\n",pageno,physpage); */
		if((physpage & B_IP) && (inv_pageno == -1))
			inv_pageno = pageno;
		if((physpage & B_IP) == 0)
			if(hi_physpage < (physpage & M_PPN))
				hi_physpage = physpage & M_PPN;
	}
	for(pageno = 0; pageno < NUMLOGPAG; pageno++) {
		physpage = in_local(mapport(textseg,pageno));
/*		printf("text page %u = 0x%x\n",pageno,physpage); */
		if((physpage & B_IP) == 0)
			if(hi_physpage < (physpage & M_PPN))
				hi_physpage = physpage & M_PPN;
	}
/*	printf("hi_physpage=0x%x  inv_pageno=%u\n",hi_physpage,inv_pageno);*/

		/* allocate all data pages up to page 30, the stack */

	if(inv_pageno == -1) return 0;
	for(pageno=inv_pageno; pageno < NUMLOGPAG-2; pageno++) {
		out_local(mapport(dataseg,pageno),++hi_physpage);
	}
	return 0;
}
