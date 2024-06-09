#include <stdio.h>
#include <varargs.h>

int _print();
extern FILE *_pfile;

int
printf (va_alist) va_dcl
{
	register char *format;
	register int rc;
	va_list ap;

	va_start (ap);
	format = va_arg (ap, char *);
	_pfile = stdout;
	rc = _print (format, &ap);
	va_end (ap);
	return rc;
}
