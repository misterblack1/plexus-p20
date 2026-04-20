/* SID @(#)dmcio.h	5.1 */
/* @(#)dmcio.h	6.1 */
#define	IOCTYPE	0xff00

#define	DIOC	('d'<<8)
#define	DIOCGETC	(DIOC|1)
#define	DIOCGETB	(DIOC|2)
#define	DIOCSETE	(DIOC|3)
#define DIOCMAIN	(DIOC|4)
#define DIOCNORM	(DIOC|5)
