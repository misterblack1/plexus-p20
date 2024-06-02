#include "sys/param.h"
#include "icp/sioccomm.h"
#include "icp/icp.h"
#ifdef COUNT
extern int profile[];
#endif
#ifdef WAITING
extern int profile[];
#endif

#ifdef VPMSYS
#ifdef DEBUG
#define NUMBUFS	2
#else
#define NUMBUFS 4
#endif
#endif

#ifndef VPMSYS
#ifdef DEBUG
#define NUMBUFS	4
#else
#define NUMBUFS 8
#endif
#endif

char roomneeded;

struct sioblk {
	struct sioblk *b_next;
#ifdef VPMSYS
	char icpbuf[VWBUFSIZE-2];
#else
	char icpbuf[WBUFSIZE-2];
#endif
} siocbuf[NUMBUFS];

struct sioblk *bfreelist;

binit()
{
	register struct sioblk *bp;

	for(bp = &siocbuf[0]; bp<&siocbuf[NUMBUFS]; bp++) {

		bp->b_next = bfreelist;
		bfreelist  = bp;

	}
}

char *smalloc()
{
	struct sioblk *tptr;

	/*------------------------------------------------------*\
	|   no race conditions here. Only from siowrite which is |
	|   entered by unix processes one at a time. (see si.c)  |
	\*------------------------------------------------------*/
	
#ifdef COUNT
	profile[54]++;
#endif

	while(! (tptr = bfreelist)) {
#ifdef WAITING
		profile[14]++;
#endif
		roomneeded = 1;
		sleep((caddr_t) &roomneeded, SIOCPRI);
	}
	bfreelist = bfreelist->b_next;
	return (char *) tptr;
}

smfree(ptr)
struct sioblk *ptr;
{

	/*------------------------------------------------------*\
	|   no race conditions here. Only from siowrite which is |
	|   entered by unix processes one at a time. (see si.c)  |
	\*------------------------------------------------------*/

#ifdef COUNT
	proflie[55]++;
#endif

	if(roomneeded) {
		roomneeded = 0;
		wakeup((caddr_t) &roomneeded);
	}
	ptr->b_next = bfreelist;
	bfreelist = ptr;
}
