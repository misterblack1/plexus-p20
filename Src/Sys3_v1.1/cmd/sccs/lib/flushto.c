# include	"../hdr/defines.h"

SCCSID(@(#)flushto	5.1);

flushto(pkt,ch,put)
register struct packet *pkt;
register char ch;
int put;
{
	register char *p;

	while ((p = getline(pkt)) != NULL && !(*p++ == CTLCHAR && *p == ch))
		pkt->p_wrttn = put;

	if (p == NULL)
		fmterr(pkt);

	putline(pkt,0);
}
