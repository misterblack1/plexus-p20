/* SID @(#)ecc.h	5.2 */
/*
 * definitions for ecc single bit error logging
 */

#define	NUMECC		30	/* number of entries in ecc table */
#define	ECCTHRESH	20	/* number of ecc failures allowed between */
				/* clock ticks */
#ifdef z8000
#define	ECCLOGOFF	0x3f	/* syndrome code and eccstate value */
				/* when single error logging is turned off */
#endif
#ifdef m68
#define	ECCLOGOFF   M_SYNDROME	/* syndrome code and eccstate value */
				/* when single error logging is turned off */
#endif

struct ecc {
	char	e_syndrome;	/* syndrome code for error */
	unsigned short e_bank;	/* physical page number in error */
	time_t	e_time;		/* time of error */
};
