/* SID */
/* @(#)map.h	5.1 4/22/86 */

struct map
{
	ushort	m_size;
	unsigned short m_addr;
};

extern struct map swapmap[];
extern struct map coremap[];

struct imap
{
	ushort	m_size;
	unsigned m_addr;
};
extern struct map dmamap[];
extern struct map xmap[];
