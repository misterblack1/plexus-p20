/*LINTLIBRARY*/
/*
 *	tmpfile - return a pointer to an update file that can be
 *		used for scratch. The file will automatically
 *		go away if the program using it terminates.
 */

#include <stdio.h>

FILE *
tmpfile()
{
	char tfname [L_tmpnam];
	register FILE *p;

	tmpnam (tfname);
	p = fopen (tfname, "w+");
	unlink (tfname);
	return p;
}
