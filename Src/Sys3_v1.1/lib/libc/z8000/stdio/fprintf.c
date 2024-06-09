#include <stdio.h>
#include <varargs.h>

int _print();
extern FILE *_pfile;

int
fprintf (va_alist) va_dcl
{
	register char *format;
	register int rc;
	va_list ap;

	va_start (ap);
	_pfile = va_arg (ap, FILE *);
	format = va_arg (ap, char *);
	rc = _print (format, &ap);
	va_end (ap);
	return rc;
}
