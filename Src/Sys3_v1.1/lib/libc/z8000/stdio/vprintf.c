#include <stdio.h>
#include <varargs.h>

int _print();
extern FILE *_pfile;

int
vprintf (format, args)
	register char *format;
	va_list args;
{
	register int rc;

	_pfile = stdout;
	rc = _print (format, &args);
	return rc;
}
