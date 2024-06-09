
#include "sys/param.h"
#include "sys/tty.h"
#ifdef COUNT
extern int profile[];
#endif

struct cblock cfree[VPMNCLIST];

struct chead cfreelist;

/*
 * Initialize clist by freeing all character blocks.
 */
cinit()
{
	register struct cblock *cp;

	for(cp = &cfree[0]; cp < &cfree[VPMNCLIST]; cp++) {
		cp->c_next = cfreelist.c_next;
		cfreelist.c_next = cp;
	}
	cfreelist.c_size = CLSIZE;
}

int getc(ptr)
register struct clist *ptr;
{
    register n;
    register struct cblock *tptr;
    register s;

#ifdef COUNT
	profile[27]++;
#endif

    s=spl6();
    if(--ptr->c_cc >= 0) {
        n = ptr->c_cf->c_data[ptr->c_cf->c_first];
        if(++ptr->c_cf->c_first == ptr->c_cf->c_last) {
            tptr = ptr->c_cf;
            ptr->c_cf = ptr->c_cf->c_next;
            if(ptr->c_cf == 0)
                ptr->c_cl = 0;
            tptr->c_next = cfreelist.c_next;
            cfreelist.c_next = tptr;
        }
	splx(s);
        return n & 0377;
    }
    else {
        ptr->c_cc = 0;
	splx(s);
        return -1;
    }
}

putc(c,ptr)
register c;
register struct clist *ptr;
{
    register struct cblock *tptr;
    register s;

#ifdef COUNT
	profile[28]++;
#endif

    /* save and disable current interrupt level */
    s=spl6();

    /* is clist emptry (no linked cblocks)? */

    if(ptr->c_cl != 0) {
        /* is last cblock space exhausted? */
        if(ptr->c_cl->c_last >= cfreelist.c_size) {
            /* is there a free cblock? */
            if((tptr=cfreelist.c_next) == 0) {
		splx(s);
		return -1;
	    }
            /* insert new cblock at end of list */
            ptr->c_cl->c_next = tptr;
            cfreelist.c_next = cfreelist.c_next->c_next;
            ptr->c_cl = tptr;
            tptr->c_next = 0;
            tptr->c_first = 0;
            tptr->c_last = 0;
        }
    }
    else { /* clist was empty */
        /* is there a free cblock? */
        if((tptr=cfreelist.c_next) == 0) {
	    splx(s);
	    return -1;
	}
        /* insert new cblock at end of list */
        ptr->c_cf = tptr;
        cfreelist.c_next = cfreelist.c_next->c_next;
        ptr->c_cl = tptr;
        tptr->c_next = 0;
        tptr->c_first = 0;
        tptr->c_last  = 0;
    }

    /* insert character into clist and adjust character count */
    ptr->c_cc++;
    ptr->c_cl->c_data[ptr->c_cl->c_last++] = c;

    splx(s);
    return 0;
}

struct cblock *getcb(ptr)
register struct clist *ptr;
{
    register struct cblock *tptr;
    register s;

#ifdef COUNT
	profile[29]++;
#endif

    s=spl6();

    if((tptr=ptr->c_cf) != 0) {
        ptr->c_cc -= (tptr->c_last - tptr->c_first);
        if((ptr->c_cf=tptr->c_next) == 0)
            ptr->c_cl = 0;
    }
    splx(s);
    return tptr;
}

putcb(cbptr,clptr)
register struct cblock *cbptr;
register struct clist *clptr;
{
    register s;

#ifdef COUNT
	profile[30]++;
#endif

    s=spl6();

    if(clptr->c_cl)
        clptr->c_cl->c_next = cbptr;
    else
	clptr->c_cf = cbptr;

    clptr->c_cl = cbptr;
    cbptr->c_next = 0;
    clptr->c_cc += cbptr->c_last - cbptr->c_first;

    splx(s);
    return 0;
}

struct cblock *getcf()
{
    register struct cblock *tptr;
    register s;

#ifdef COUNT
	profile[31]++;
#endif

    s=spl6();

    if(tptr=cfreelist.c_next) {
	cfreelist.c_next = cfreelist.c_next->c_next;
	tptr->c_next = 0;
	tptr->c_first = 0;
	tptr->c_last = cfreelist.c_size;
    }
    splx(s);
    return tptr;
}

putcf(ptr)
register struct cblock *ptr;
{
    register s;
    s=spl6();
    if(cfreelist.c_next == 0) {
         wakeup((caddr_t) &cfreelist.c_next);
    }
    putcf1(ptr);
    splx( s );
}

putcf1(ptr)
register struct cblock *ptr;
{

#ifdef COUNT
	profile[32]++;
#endif


    ptr->c_next = cfreelist.c_next;
    cfreelist.c_next = ptr;
}
