/* SID */
/* @(#)stack.h	5.2 6/19/86 */

#if defined(SCHROEDER) || defined(KICKER)
/* m68020 hardware stack on trap/exceptions */
struct hwstuff
{	/* info put on stack by 68020 processor */
	unsigned long sr;	/* stretched to long by s/w */
	unsigned long  pc;
	unsigned short vector;
	unsigned short intern;
	unsigned short ssw;
	unsigned short pipeC;
	unsigned short pipeB;
	unsigned long  aa;
	unsigned short ir;	/* 68000 compatability */
	unsigned short ir1;
	unsigned long doutb;
	unsigned short unused[4]; /* only 2 of these pushed on short bus */
	unsigned short Badd;	  /* error and nothing else */
	unsigned short inter[2];
	unsigned long dinb;
	unsigned short internal[22];
};
#else
/* m68010 hardware stack on trap/exceptions */
struct hwstuff
{	/* info put on stack by 68010 processor */
	unsigned long sr;	/* stretched to long by s/w */
	unsigned long  pc;
	unsigned short vector;
	unsigned short ssw;
	unsigned long  aa;
	unsigned short ir;	/* 68000 compatability */
	unsigned short unused[4];
	unsigned short iib;
	unsigned short internal[16];
};
#endif

/* short form of registers saved on stack */
struct sregs 
{
	long	d0, d1;
	long	a0, a1;
};

/* long form of registers saved on stack */
struct lregs
{
	long	d0, d1, d2, d3, d4, d5, d6, d7;
	long	a0, a1, a2, a3, a4, a5, a6;
};
