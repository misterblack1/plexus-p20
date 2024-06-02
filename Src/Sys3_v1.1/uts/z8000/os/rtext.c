#ifdef	PNETDFS
#include "sys/param.h"
#include "sys/systm.h"
#include "sys/map.h"
#include "sys/dir.h"
#include "sys/user.h"
#include "sys/proc.h"
#include "sys/text.h"
#include "sys/inode.h"
#include "sys/buf.h"
#include "sys/seg.h"
#include "sys/var.h"

/*
 * remove a remote shared text segment from the text table, if possible.
 */
rxrele(ip)
register struct inode *ip;
{
	register struct text *xp;

	printf( "rxrele called, code not implemented\n" );
	debug();
	/*
	if ((ip->i_flag&ITEXT) == 0)
		return;
	for (xp = &text[0]; xp < (struct text *)v.ve_text; xp++)
		if (ip==xp->x_iptr)
			xuntext(xp);
	*/
}
#endif
