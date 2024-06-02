#
/*
 *	UNIX/INTERDATA debugger
 */

/* unix parameters */
#define DBNAME "adb\n"
#define TXTRNDSIZ 8192L

TYPE	unsigned TXTHDR[8];
#ifdef OVKRNL
TYPE	struct ovhead	OVLVEC;
TYPE	unsigned SYMV;
TYPE	char  OVTAG;
#else
TYPE	unsigned SYMV;
#endif

/* symbol table in a.out file */
struct symtab {
	char	symc[8];
#ifdef OVKRNL
	OVTAG	ovnumb;
	char	symf;
#else
	int	symf;
#endif
	SYMV	symv;
};
#define SYMTABSIZ (sizeof (struct symtab))

#define SYMCHK 047
#define SYMTYPE(symflag) (( (int)symflag>=041 || ((int)symflag>=02 && (int)symflag<=04))\
			?  (((int)symflg&07)>=3 ? DSYM : ((int)symflg&07))\
			: NSYM\
			)
#ifdef OVKRNL
struct	ovhead {
	unsigned	max;
	unsigned	ov[7];
};
#endif
