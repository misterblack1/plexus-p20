typedef	struct { int r[1]; } *	physadr;
typedef	long		daddr_t;
typedef	char *		caddr_t;
typedef	unsigned short	ushort;
typedef	ushort		ino_t;
#ifdef vax
typedef short		cnt_t;
#else
typedef char		cnt_t;
#endif
typedef	long		time_t;
#ifdef vax
typedef	int		label_t[10];
#else
typedef	int		label_t[9];	/* program status regs r7 - r15 */
#endif
typedef	short		dev_t;
typedef	long		off_t;
typedef	long		paddr_t;
