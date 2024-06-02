/*#define NOSIOC*/
/*
 * The next to define's are used throughout the si driver to determine 
 * if code is compiled for the additional SIOC's.  Setting these
 * defines to 0 will cause exclusion of code assoicated to each SIOC.
 * Setting them non-zero causes inclusion of said code.
 */

#ifndef NOSIOC
#define	SIOC1DEF	0		/* SIOC1 */
#define SIOC2DEF	0		/* SIOC2 */
#define SIOC3DEF	0		/* SIOC3 */
#define SIOC4DEF	0		/* SIOC4 */
#endif
