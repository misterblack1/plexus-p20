#define debug 0
/*
 * CPU physical memory driver and /dev/null
 *	If dt_unit == 2, /dev/null.  Else dt_unit holds the segment
 *	number of the physical memory to be read.  In this case, the
 *	driver reads the physical memory at segment number dt_unit,
 *	e.g.,. 0x200, offset by io->i_offset+io->i_dp->dt_boff.
 *	Maximum read/write: 4k (2 pages)
 */

/************************************************************************/
/*									*/
/* This driver is used by the standalone programs to access main memory	*/
/*  just like a device.  It serves no real purpose other than enabling	*/
/*  testing of standalone programs when no drivers are available.	*/
/*									*/
/************************************************************************/


#include "sys/plexus.h"
#include "saio.h"


int segno;			/* Global logical segment number of this program */

kmemopen(io)
register struct iob *io;
{
}


/* Makaddrs sets up the mapper so that the next logical map set		*/
/*  points to the memory we are concerned with.					*/

long makaddrs (unit, offset)
int unit;
long offset;
{
	long physaddr;
	long physoffset;
	int physpage;

	physaddr = unit;
	physaddr <<= 16;
	physaddr += offset;
	physoffset = physaddr & 0x7ff;
	physpage = physaddr >> 11;
	out_local(mapport(segno+0x200,0),physpage++);
	out_local(mapport(segno+0x200,1),physpage);

	return(physoffset + ((long)segno << 16) + 0x02000000);
}


kmemstrategy(io,func)
register struct iob *io;
{
	long bufaddr;

if (debug) {
printf("kmem: func(%x),cc(%x),unit(%x),bn(%X),boff(%X),offset(%x),ma(%x)\n",
#ifdef V7
func,io->i_cc,io->i_unit,io->i_bn,io->i_boff,io->i_offset,io->i_ma);
#else
func,io->i_cc,io->i_dp->dt_unit,io->i_bn,io->i_dp->dt_boff,io->i_offset,
io->i_ma);
#endif
}

#ifdef V7
	bufaddr = makaddrs(io->i_unit,io->i_offset+io->i_boff);
#else
	switch(minor(io->i_dp->dt_unit)) {
	case 2:			/* dev/null */
		if(func == READ) return 0;
		return io->i_cc;
	default:
#endif
		bufaddr = makaddrs(io->i_dp->dt_unit,io->i_offset+
			io->i_dp->dt_boff);
		if (func == READ) {
			if (io->i_cc >= 2) {
				blkmove(bufaddr,segno,io->i_ma,io->i_cc);
				return(io->i_cc);
			}
			else {
				*io->i_ma = bxmemrd(bufaddr);
				return(1);
			}
		}
		else {
			if (io->i_cc >= 2) {
				blkmove(segno,io->i_ma,bufaddr,io->i_cc);
				return(io->i_cc);
			}
			else {
				bxmemld(*io->i_ma,bufaddr);
				return(1);
			}
		}
#ifndef V7
	}	/* end of switch */
#endif
}

physmem(io)
register struct iob *io;
{
#ifdef V7
	absjump(io->i_unit,io->i_boff);
#else
	absjump(io->i_dp->dt_unit,io->i_dp->dt_boff);
#endif
}

