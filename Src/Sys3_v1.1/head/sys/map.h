struct map
{
	short	m_size;
	unsigned short m_addr;
};

extern struct map swapmap[];

#ifdef pdp11
extern struct map coremap[];
#endif
