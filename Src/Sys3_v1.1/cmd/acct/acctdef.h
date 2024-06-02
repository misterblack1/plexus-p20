/*
 *	defines, typedefs, etc. used by acct programs
 */


/*
 *	acct only typedefs
 */
typedef	unsigned short	uid_t;

#define LSZ	8	/* sizeof line name */
#define NSZ	8	/* sizeof login name */
#define P	0	/* prime time */
#define NP	1	/* nonprime time */

/*
 *	limits which may have to be increased if systems get larger
 */
#define SSIZE	1000	/* max number of sessions in 1 acct run */
#define TSIZE	100	/* max number of line names in 1 acct run */
#define USIZE	500	/* max number of distinct login names in 1 acct run */

#define EQN(s1, s2)	(strncmp(s1, s2, sizeof(s1)) == 0)
#define CPYN(s1, s2)	strncpy(s1, s2, sizeof(s1))

#ifdef stock
#define SECS(tics)	((double) tics)/60.
#else
#define SECS(tics)	((double) tics)/50.
#endif
#define MINS(secs)	((double) secs)/60.
#ifdef stock
#define MINT(tics)	((double) tics)/3600.
#else
#define MINT(tics)	((double) tics)/3000.
#endif
#ifdef pdp11
#ifdef stock
#define KCORE(clicks)	((double) clicks/16)
#else
#define KCORE(clicks)	((double) clicks*2)
#endif
#endif
#ifdef vax
#define KCORE(clicks)	((double) clicks/2)
#endif
#define SECSINDAY	86400L
