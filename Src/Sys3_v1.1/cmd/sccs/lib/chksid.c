# include	"../hdr/defines.h"

SCCSID(@(#)chksid	5.1);

chksid(p,sp)
char *p;
register struct sid *sp;
{
	if (*p ||
		(sp->s_rel == 0 && sp->s_lev) ||
		(sp->s_lev == 0 && sp->s_br) ||
		(sp->s_br == 0 && sp->s_seq))
			fatal("invalid sid (co8)");
}
