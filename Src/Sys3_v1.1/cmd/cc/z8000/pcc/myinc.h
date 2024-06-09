/*	Plexus - Sys3 - August 1982	*/

static char myinc_h[] = "@(#)myinc.h	1.1";

	/* the following are used to recover the optimal register */
	/* they should only be used in cases where a register is reclaimed */
	/* only one of the flags may appear in a reclaim */
# define RESCLIQ 0100	/* take low 16 bits of quad */
# define RESCLLQ 0200	/* take low 32 bits of quad */
# define RESCLIL 0400	/* take high 16 bits of long */
