#include <stdio.h>
#include <varargs.h>

int _print();
extern FILE *_pfile;

int
vfprintf (file, format, args)
	FILE *file;
	register char *format;
	va_list args;
{
	register int rc;

	_pfile = file;
	rc = _print (format, &args);
	return rc;
}
