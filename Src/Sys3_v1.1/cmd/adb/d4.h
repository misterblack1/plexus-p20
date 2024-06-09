#

/*
 *	these definitions are used by the disassembler and pass3 only
 */

#define ABSREF 0    /* the reference is absolute */
#define TXTREF 2    /* reference to a location in the text */
#define DATREF 4    /* reference to a location in initialized data */
#define BSSREF 6    /* reference to a location in uninitialized data */
#define EXTREF 8    /* reference to an external symbol */
#define PCREL  03  /* reference is to a PC relative label */

#define DATFLAG 010000	/* flag data reference as DAT relative */
#define CONFLAG 020000  /* flag data reference as CON relative */

#define HEADER 16   /* size in bytes of the header in the a.out file */

#define LABSIZ 1000    /* number of internal labels permitted */
#define ORDSIZ 500     /* number of symbols permitted */
#define FUNCNO 100	/* number of functions permitted */

#define DSCMSK 0160
#define EXTERNAL 060


