/* SID @(#)tp.h	5.1 */

/* these definitions come from imsc.h. the tp driver does not include
   imsc.h, hence this file. 
   notice that the struct tpcmd is similar to ptcmd in imsc.h.

*/
#define C_IREW 1
#define C_IWEOF 2
#define C_IERASE 3
#define C_IRETENSION 4
#define C_IIO 9
#define C_IMOVE 10
#define C_ISENSE 11
#define C_IRNOP 12	/* this is a robin command only and must not be
			   used in any other cartridge driver.  This is 
			   used in the tape program to differentiate
			   between robin and other cartridge tapes.  This
			   define MUST be the same as in imsc.h */

#define C_IRDY 21 /* this is not from imsc.h but is needed for tp.c */

struct tpcmd {
	short dknum;
	long blkno;
	long blkcnt;
	};

