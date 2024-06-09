
#include "sys/param.h"
#include "sys/tty.h"

struct cblock cfree[NCLIST];

struct chead cfreelist;

/*
 * Initialize clist by freeing all character blocks.
 */
cinit()
{
	register struct cblock *cp;

	for(cp = &cfree[0]; cp < &cfree[NCLIST]; cp++) {
		cp->c_next = cfreelist.c_next;
		cfreelist.c_next = cp;
	}
	cfreelist.c_size = CLSIZE;
}

