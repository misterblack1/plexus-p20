/*
 * First fit memory allocation.  (Generally uses memory pretty efficiently,
 * although it is slower than some other memory allocators.)
 *
 * Copyright (C) 1989 by Kenneth Almquist.  All rights reserved.
 * This file is part of ash, which is distributed under the terms specified
 * by the Ash General Public License.  See the file named LICENSE.
 */


#include "shell.h"
#include "machdep.h"
#include "mystring.h"
#include <sys/types.h>


/*
 * The free memory pool consists of a collection of contiguous blocks.
 * Each block has an integer at the beginning of it which specifies the
 * size of the block.  If the block is allocated, the integer contains
 * the negative of the size.  After the last block comes an integer with
 * a value of zero.
 *
 * To allocate a block, we want to scan the list of blocks starting with
 * the first block, merging adjacent free blocks, until we get a free
 * block which is large enough.  The actual implementation uses some
 * hacks to decrease the amount of scanning required.  Startfree always
 * points to the first free block or a block before it.  This keeps us
 * from repeatedly scanning allocated block at the start of the memory
 * pool.  In a similar vein, startbig points to what we believe is the
 * first free block whose size is >= BIG or a block before it.  Startbig
 * can actually point to some location after the first large free if the
 * first large free block was formed by freeing several small blocks
 * which have not yet been merged into a single free block.  To make this
 * less likely, the free routine will merge a freed block with any free
 * blocks after it, but the free routine cannot merge a freed block with
 * free blocks which precede it because there is no (efficient) way for
 * free to locate the preceding block.
 *
 * The variables lastsize and lastloc are used to implement one final
 * method to cut down on scanning.  When a malloc is performed, the
 * variable lastsize is set to the size of the largest block skipped
 * during the scan, and the variable lastloc is set to the end of the
 * scan.  The next call to malloc can start where the preceding one left
 * off if the number of bytes reqested is larger than the size of any
 * blocks skipped on the preceding malloc.  When a block is freed with a
 * lower address than lastloc, free assumes that the block is adjacent to
 * the largest free block skipped by malloc, and updates lastsize
 * accordingly.  This is necessary to ensure that starting at lastloc
 * will never cause a block that could be allocated to be skipped; a more
 * aggressive policy could be used.
 */



/*
 * Machine dependent stuff:
 *
 * PAGE_SIZE is the size of a page.  Malloc will try to keep the break
 * location on a page boundary to avoid wasting space (since the operating
 * system presumably has to allocate a whole page even if we only request
 * part of one).  PAGE_SIZE must be a power of 2.
 *
 * Head_t is a signed integer type that is capable of holding a value one
 * less than the maximum size of the pool.  Type int works fine on a VAX
 * because on a VAX processes only get 31 bits of address space.  In
 * practice most other 32 bit machines aren't going to allow processes to
 * allocate more that 2 gigabytes either.
 *
 * Machines generally have alignment restrictions which malloc must
 * obey.  ALIGN(n) returns the value of n rounded up to the minimum
 * value that malloc must allocate to keep things aligned.
 *
 * The code here assumes a linear address space, with sbrk allocating
 * successively higher addresses.
 */


#define PAGE_SIZE 1024
#define PAGE_MASK (PAGE_SIZE - 1)


#define head_t int
#define HEADSIZE ALIGN(sizeof (head_t))

#define DEREF(p)	(*(head_t *)(p))


#ifndef ALIGN
union align {
      long l;
      char *cp;
};

#define ALIGN(nbytes)	((nbytes) + sizeof(union align) - 1 &~ (sizeof(union align) - 1))
#endif


/*
 * Tunable paramaters.  SLOP is the smallest free block that malloc or
 * realloc will create.  If they would have to create a smaller free
 * block to satisfy the caller's request, they allocate the extra bytes
 * to the caller rather than forming a free block.  BIG is the smallest
 * block size that will cause the scan to start at startbig; this is
 * used to keep requests for large blocks from scanning lots of small
 * blocks.  MINSBRK is the smallest number of pages that will be requested
 * from sbrk at a time.  A larger value can cut down the number of calls
 * to sbrk.  MINSBRK should be a multiple of PAGE_SIZE.
 */

#define SLOP 8
#define BIG 500
#define MINSBRK (2 * PAGE_SIZE)


pointer startfree;	/* where to start search for n < BIG */
pointer startbig;	/* where to start search for n >= BIG */
pointer lastloc;	/* where last search terminated */
head_t lastsize;	/* largest block skipped on last search */



pointer realloc();
void free();
caddr_t sbrk();



pointer
malloc(n)
      unsigned n;
      {
      return realloc((pointer)0, n);
}



pointer
realloc(old, nbytes)
      pointer old;
      unsigned nbytes;
      {
      head_t n = nbytes + HEADSIZE;
      pointer p, q;
      head_t i;
      head_t size;
      head_t largest;
      pointer next;
      head_t allocsize;

      if (n < 0)
	    return NULL;	/* nbytes out of range */
      n = ALIGN(n);
      if (startfree == NULL) {	/* first time called */
	    p = sbrk(0);
	    allocsize = PAGE_SIZE - ((int)p & PAGE_MASK);
	    if (allocsize < n + 2 * HEADSIZE)
		  allocsize += MINSBRK;
	    if (sbrk(allocsize) != p)
		  return NULL;
	    DEREF(p) = allocsize - HEADSIZE;
	    startfree = startbig = lastloc = p;
	    lastsize = 0;
      }
      if (old) {	/* it's a realloc; try resizing */
	    p = old - HEADSIZE;
	    q = p - DEREF(p);
	    while (DEREF(q) > 0) {
		  if (startbig == q)
			startbig = p;
		  if (startfree == q)
			startfree = p;
		  if (lastloc == q)
			lastloc = p;
		  q += DEREF(q);
	    }
	    size = q - p;
	    if (size >= n) {
		  if (size - n <= SLOP) {
			DEREF(p) = -size;
		  } else {
			next = p + n;
			DEREF(p) = -n;
			DEREF(next) = size - n;
		  }
		  return old;
	    }
      }
      if (n > lastsize) {
	    p = lastloc;
	    largest = lastsize;
      } else {
	    p = startfree;
	    largest = 0;
      }
      if (n >= BIG && p < startbig) {
	    p = startbig;
	    largest = BIG - 1;
      }
      for (;;) {
	    while ((size = DEREF(p)) < 0)
		  p -= size;
	    if (largest < BIG) {
		  if (largest == 0)
			startfree = p;
		  if (p > startbig)
			startbig = p;
	    }
	    q = p + size;
	    if (DEREF(q) > 0) {
		  do {
			if (startbig == q)
			      startbig = p;
			q += DEREF(q);
		  } while (DEREF(q) > 0);
		  size = q - p;
		  DEREF(p) = size;
	    }
	    if (size >= n) {	/* found a block that's large enough */
		  if (size - n <= SLOP) {
			DEREF(p) = -size;
			next = q;
		  } else {
			next = p + n;
			DEREF(p) = -n;
			DEREF(next) = size - n;
		  }
		  if (next < startbig && size - n >= BIG)
			startbig = next;
		  lastsize = largest;
		  lastloc = next;
		  break;
	    }
	    if (DEREF(q) == 0) { /* out of space; must get some from sbrk */
		  if (old && old + DEREF(old - HEADSIZE) == p) {
			p = old - HEADSIZE;
			size += -DEREF(p);
			old = NULL;
		  }
		  allocsize = (n - size - 1 + PAGE_SIZE) &~ PAGE_MASK;
		  if (allocsize < MINSBRK)
			allocsize = MINSBRK;
		  if ((next = sbrk(allocsize)) == (caddr_t)-1)
			return NULL;
		  if (next != q + HEADSIZE) {
			if (largest < size)
			      largest = size;
			if (allocsize < n + HEADSIZE) {
			      if (sbrk(PAGE_SIZE) == (caddr_t)-1) {
				    sbrk(-allocsize);
				    return NULL;
			      }
			      allocsize += PAGE_SIZE;
			}
			DEREF(q) = -(next - q);
			p = next;
		  }
		  q = next + allocsize - HEADSIZE;
		  DEREF(q) = 0;			/* delete */
		  next = p + n;
		  DEREF(p) = -n;
		  DEREF(next) = q - next;
		  lastsize = largest;
		  lastloc = next;
		  break;
	    }
	    if (largest < size)
		  largest = size;
	    p = q;
      }
      /* allocated a block */
      p += HEADSIZE;
      if (old) {
	    size = -DEREF(old - HEADSIZE);
	    bcopy(old, p, size);
	    free(old);
      }
      return p;
}



void
free(p)
      pointer p;
      {
      pointer q;
      head_t size;

      if (p == (pointer)0)
	    return;
      p -= HEADSIZE;
      if (DEREF(p) >= 0)
	    abort();
      q = p - DEREF(p);
      for (;;) {
	    if (startbig == q)
		  startbig = p;
	    if (lastloc == q)
		  lastloc = p;
	    if (DEREF(q) <= 0)
		  break;
	    q += DEREF(q);
      }
      size = q - p;
      DEREF(p) = size;
      if (startfree > p)
	    startfree = p;
      if (size >= BIG && startbig > p)
	    startbig = p;
      if (p < lastloc)
	    lastsize += size;
}
