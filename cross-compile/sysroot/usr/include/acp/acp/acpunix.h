/* SID */
/* @(#)acpunix.h	5.1 4/22/86 */

/*
 * acp sio related defines
 */

#ifndef NOS10
#ifdef PNET
#define NUMACP  2		/* number of acps in chassis */
#else
#define NUMACP  5		/* number of acps in chassis */
#endif
#else
#define NUMACP  5
#endif
#define	ANUMSIO	16		/* number of sio channels  0-15 */
#define ADNLDUNIT 17		/* unit # for down load port in acp */
#define APIOUNIT 16		/* parallel io port # in acp */
#define	ACONSOLE 0		/* unit number of console device */
