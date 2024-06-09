# include	"stdio.h"
static char SCCSID[]="@(#)xalloc	4.3";

/*
	xalloc/xfree based on alloc/free in C library at one time.
	Also xfreeall() frees all memory allocated (calls brk(II)).
 
	Xfree always coalesces contiguous free blocks.
	Xalloc uses a first fit strategy.
	Xalloc always allocates words (rounds up).
	Xalloc actually allocates one more word than the
	amount requested.  The extra word (the first word of the
	allocated block) contains the size (in bytes) of the entire block.
	This size is used by xfree to identify contiguous blocks,
	and is used by xalloc to implement the first fit strategy.

	Bad things will happen if the size word is changed.
	Worse things happen if xfree is called with a
	garbage argument.

	Xalloc returns the address of the allocated area on success,
	fatal() on failure.
	Xfree and xfreeall don't return anything.
*/

struct fb {
	unsigned	f_size;
	char		*f_next;
};

struct fb Freelist={
	0,
	(char *)0x3FFFFFFF,
};

char *Freeend = (char *)0x3FFFFFFF;

# define SIZINCRO	10
# ifdef vax
# define MAX_PAGE	256000
# else
# define MAX_PAGE	65535 - 8000
# endif
# define SLOP	(sizeof(int *))

extern	end;
unsigned Lastbrk = (unsigned)&end;

xalloc(asize)
unsigned asize;
{
	register unsigned usize;
	register struct fb *np, *cp;
	int	tot = 0;
	long	asizel = 0;
	long	lastbrkl = 0;
	char	*xcp;

	if((usize = asize) == 0)
		return(0);
	usize += 2*sizeof(int *) -1;
	usize &= ~(sizeof(int *) -1);
	for(;;) {
		cp = &Freelist;
		while((np = (struct fb *)cp->f_next) != (struct fb *)Freeend) {
			if(np->f_size>=usize) {
			/*
				Don't break the block up if it
				is not more than SLOP bigger than the
				amount needed.
			*/
				if(usize+SLOP >= np->f_size)
					cp->f_next = np->f_next;
			/*
				Break the block into 2 pieces.
			*/
				else {
					cp = (struct fb *)(cp->f_next = (char *)(((int)np) + usize));
					cp->f_size = np->f_size - usize;
					cp->f_next = np->f_next;
					np->f_size = usize;
				}
				return((int)&np->f_next);
			}
			cp = np;
		}
	/*
		Nothing on the free list is big enough;
		get more core from the operating system.
	*/
		asize = usize<1024? 1024: usize;
		asize = (asize+511) & (~511);
		asizel = asize;
		lastbrkl = Lastbrk;
		if(((asizel + lastbrkl) > MAX_PAGE) ||
			(cp = (struct fb *)sbrk(asize)) == (struct fb *)-1) {
			xcp = (char *)sbrk(SIZINCRO);
			while(((cp = (struct fb *)sbrk(SIZINCRO)) != (struct fb *)-1) &&
				(xcp != (char *)-1)) {
				tot += SIZINCRO;
				lastbrkl = Lastbrk = ((int) cp) + SIZINCRO;
				if((SIZINCRO + lastbrkl) > MAX_PAGE) {
					break;
				}
			}
			if((!tot) || (xcp == (char *)-1))
				return(fatal("out of space (ut9)"));
			cp = (struct fb *)xcp;
			cp->f_size = tot + SIZINCRO;
		}
		else {
			Lastbrk = ((int)cp) + asize;
			cp->f_size = asize;
		}
	/*
		Add new piece to free list.
	*/
		xfree(&cp->f_next);
	}
}


xfree(aptr)
char *aptr;
{
	register struct fb *ptr, *cp, *np;

	if (aptr && aptr < (char *)Lastbrk) {
		ptr = (struct fb *) ((unsigned)aptr - sizeof(int *));
		cp = &Freelist;
		while((np = (struct fb *)cp->f_next) < ptr)
			cp = np;
	/*
		Try to coalesce with the following block.
	*/
		if(((int)ptr) + ptr->f_size == ((int)np)) {
			ptr->f_size += np->f_size;
			ptr->f_next = np->f_next;
			np = ptr;
		} else
			ptr->f_next = (char *)np;
	/*
		Try to coalesce with the preceding block.
	*/
		if(((int)cp) + cp->f_size == ((int)ptr)) {
			cp->f_size += ptr->f_size;
			cp->f_next = ptr->f_next;
		} else
			cp->f_next = (char *)ptr;
	}
}


xfreeall()
{
	brk(&end);
	Lastbrk = (unsigned)&end;
	Freelist.f_size = 0;
	Freelist.f_next = Freeend;
}
