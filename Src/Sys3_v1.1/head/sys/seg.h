/*
 * Memory management addresses and bits
 */

#ifdef vax
#define	RO	PG_URKR		/* access abilities */
#define	RW	PG_UW

#else
#define	UISD	((physadr)0177600)	/* first user I-space descriptor register */
#define	UISA	((physadr)0177640)	/* first user I-space address register */
#define	UDSA	((physadr)0177660)	/* first user D-space address register */
#define	RO	02		/* access abilities */
#define	RW	06
#define	ED	010		/* extend direction */
#define	TX	020		/* Software: text segment */

physadr	ka6;		/* 11/40 KISA6; 11/45 KDSA6 */

/*
 * address to access 11/70 UNIBUS map
 */
#define	UBMAP	((physadr)0170200)
#endif
