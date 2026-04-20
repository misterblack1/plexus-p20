/* SID @(#)dmk.h	5.1 */
/*	dmk.h 6.1 of 8/22/83
	@(#)dmk.h	6.1
 */


#define	DMK	('V'<<8)
#define DMKSETM	(DMK|13)
#define DMKATTACH (DMK|14)
#define DMKDETACH (DMK|15)
#define	DMKDTR	(1<<1)
#define	DMKRTS	(1<<2)
#define	DMKNS	(1<<3)

struct dmkcmd {
	short line, mode;
};

struct dmksave {
	unsigned char status[8];
};
