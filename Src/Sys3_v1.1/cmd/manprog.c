/*	MANPROG SID (@(#)manprog.c	1.3)
	This program calculates and returns the last
	modified date of the give file name.
	NOTE: Months are given from 0 to 11
	therefore to calculate the proper month
	number, 1 must be added to the month returned.  */

#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
struct stat buf;
struct tm *tm;
struct tm *localtime();

main(argc,argv)
char **argv;
{
	if(argc<1)
		return(0);
	if(stat(argv[1],&buf)==-1)
		return(0);
	tm = localtime(&buf.st_mtime);
	printf("-rd%d -rm%d -ry%d\n",
		tm->tm_mday,tm->tm_mon,tm->tm_year);
	return(0);
}
